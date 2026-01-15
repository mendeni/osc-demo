#include <juce_osc/juce_osc.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <iostream>
#include <signal.h>

// Global flag for graceful shutdown
static volatile bool running = true;

// Signal handler for clean shutdown
void signal_handler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down..." << std::endl;
    running = false;
}

// OSC Receiver class that handles incoming messages
class OSCHost : public juce::OSCReceiver::Listener<juce::OSCReceiver::RealtimeCallback>
{
public:
    OSCHost() : sender()
    {
    }

    bool start(int port)
    {
        if (!receiver.connect(port))
        {
            std::cerr << "Failed to connect to port " << port << std::endl;
            return false;
        }

        receiver.addListener(this);
        std::cout << "Server started successfully!" << std::endl;
        return true;
    }

    void stop()
    {
        receiver.removeListener(this);
        receiver.disconnect();
    }

    void oscMessageReceived(const juce::OSCMessage& message) override
    {
        juce::String address = message.getAddressPattern().toString();
        
        if (address == "/ping")
        {
            std::cout << "Received ping" << std::endl;
            
            // Note: juce_osc's OSCReceiver doesn't provide sender information like liblo did.
            // For a production application, clients should include their return address in the message.
            // For this demo, we send pong responses to a default loopback address.
            sendPong("127.0.0.1", 7771);
        }
        else
        {
            // Generic handler for unmatched messages
            std::cout << "Received unhandled message:" << std::endl;
            std::cout << "  Path: " << address << std::endl;
            std::cout << "  Arguments: " << message.size() << std::endl;
        }
    }

    void oscBundleReceived(const juce::OSCBundle& bundle) override
    {
        std::cout << "Received OSC bundle with " << bundle.size() << " elements" << std::endl;
    }

private:
    void sendPong(const juce::String& host, int port)
    {
        if (!sender.connect(host, port))
        {
            std::cerr << "Error: Could not create reply address" << std::endl;
            return;
        }

        juce::OSCMessage pongMessage("/pong");
        pongMessage.addString("pong");
        
        if (!sender.send(pongMessage))
        {
            std::cerr << "Error: Failed to send pong response" << std::endl;
        }
        else
        {
            std::cout << "Sent pong response to " << host << ":" << port << std::endl;
        }
        
        sender.disconnect();
    }

    juce::OSCReceiver receiver;
    juce::OSCSender sender;
};

int main()
{
    const int port = 7770;
    
    // Initialize JUCE message manager (required for JUCE initialization)
    // Note: We use RealtimeCallback for OSC, so callbacks are invoked directly
    // on the network thread without needing a running message loop.
    juce::MessageManager::getInstance();
    
    std::cout << "OSC Demo Host - Simple OSC Server" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Listening on port " << port << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  /ping - Responds with /pong" << std::endl;
    std::cout << std::endl;
    std::cout << "Test with: oscsend localhost " << port << " /ping" << std::endl;
    std::cout << "Press Ctrl+C to quit" << std::endl;
    std::cout << std::endl;
    
    // Create and start OSC host
    OSCHost host;
    
    if (!host.start(port))
    {
        std::cerr << "Failed to create OSC server on port " << port << std::endl;
        return 1;
    }
    
    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    while (running)
    {
        juce::Thread::sleep(1000);
    }

    // Cleanup
    std::cout << "Stopping server..." << std::endl;
    host.stop();
    
    std::cout << "Server stopped. Goodbye!" << std::endl;
    
    // Cleanup JUCE message manager
    juce::DeletedAtShutdown::deleteAll();
    juce::MessageManager::deleteInstance();
    
    return 0;
}
