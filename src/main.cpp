#include <iostream>
#include <lo/lo.h>
#include <signal.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Global flag for graceful shutdown
static volatile bool running = true;

// Signal handler for clean shutdown
void signal_handler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down..." << std::endl;
    running = false;
}

// Error handler for liblo
void error_handler(int num, const char *msg, const char *path) {
    std::cerr << "liblo server error " << num << " in path " << path << ": " << msg << std::endl;
}

// Handler for /ping command
int ping_handler(const char *path, const char *types, lo_arg **argv, int argc,
                 lo_message msg, void *user_data) {
    lo_address sender_addr = lo_message_get_source(msg);
    
    if (!sender_addr) {
        std::cerr << "Error: Could not determine message source" << std::endl;
        return 1;
    }
    
    std::cout << "Received ping from " << lo_address_get_hostname(sender_addr) 
              << ":" << lo_address_get_port(sender_addr) << std::endl;
    
    // Send pong response back to sender
    lo_address reply_addr = lo_address_new(
        lo_address_get_hostname(sender_addr),
        lo_address_get_port(sender_addr)
    );
    
    if (!reply_addr) {
        std::cerr << "Error: Could not create reply address" << std::endl;
        return 1;
    }
    
    int result = lo_send(reply_addr, "/pong", "s", "pong");
    
    if (result == -1) {
        std::cerr << "Error: Failed to send pong response" << std::endl;
        lo_address_free(reply_addr);
        return 1;
    }
    
    std::cout << "Sent pong response" << std::endl;
    
    lo_address_free(reply_addr);
    
    return 0;
}

// Generic handler for unmatched messages
int generic_handler(const char *path, const char *types, lo_arg **argv, int argc,
                    lo_message msg, void *user_data) {
    lo_address sender_addr = lo_message_get_source(msg);
    
    std::cout << "Received unhandled message:" << std::endl;
    std::cout << "  Path: " << path << std::endl;
    
    if (sender_addr) {
        std::cout << "  From: " << lo_address_get_hostname(sender_addr) 
                  << ":" << lo_address_get_port(sender_addr) << std::endl;
    } else {
        std::cout << "  From: unknown" << std::endl;
    }
    
    std::cout << "  Types: " << types << std::endl;
    
    return 0;
}

int main() {
    const char *port = "7770";
    
    std::cout << "OSC Demo Host - Simple OSC Server" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Listening on port " << port << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  /ping - Responds with /pong" << std::endl;
    std::cout << std::endl;
    std::cout << "Test with: oscsend localhost " << port << " /ping" << std::endl;
    std::cout << "Press Ctrl+C to quit" << std::endl;
    std::cout << std::endl;
    
    // Create OSC server
    lo_server_thread server_thread = lo_server_thread_new(port, error_handler);
    
    if (!server_thread) {
        std::cerr << "Failed to create OSC server on port " << port << std::endl;
        return 1;
    }
    
    // Register handlers
    lo_server_thread_add_method(server_thread, "/ping", "", ping_handler, NULL);
    lo_server_thread_add_method(server_thread, NULL, NULL, generic_handler, NULL);
    
    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Start the server thread
    lo_server_thread_start(server_thread);
    
    std::cout << "Server started successfully!" << std::endl;
    
    while (running) {
    #ifdef _WIN32
       Sleep(1000); // Sleep takes milliseconds on Windows
    #else
       sleep(1); // sleep takes seconds on POSIX
    #endif
    }

    // Cleanup
    std::cout << "Stopping server..." << std::endl;
    lo_server_thread_stop(server_thread);
    lo_server_thread_free(server_thread);
    
    std::cout << "Server stopped. Goodbye!" << std::endl;
    
    return 0;
}
