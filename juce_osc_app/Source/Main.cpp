#include <juce_gui_extra/juce_gui_extra.h>
#include "MainComponent.h"
#include <iostream>

class OSCControlApplication : public juce::JUCEApplication
{
public:
    OSCControlApplication() {}

    const juce::String getApplicationName() override       { return "OSC Control Demo"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise(const juce::String& commandLine) override
    {
        // Parse command-line arguments
        juce::StringArray args;
        args.addTokens(commandLine, true);
        
        juce::String host;
        int port = -1;
        bool showHelp = false;
        
        for (int i = 0; i < args.size(); ++i)
        {
            if (args[i] == "--help" || args[i] == "-h")
            {
                showHelp = true;
                break;
            }
            else if (args[i] == "--host" && i + 1 < args.size())
            {
                host = args[++i];
            }
            else if (args[i] == "--port" && i + 1 < args.size())
            {
                port = args[++i].getIntValue();
            }
        }
        
        if (showHelp)
        {
            std::cout << "OSC Control Demo - Version 1.0.0\n\n";
            std::cout << "Usage: OSCControlApp [OPTIONS]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --host <address>    Set OSC target address (e.g., 127.0.0.1 or localhost)\n";
            std::cout << "  --port <number>     Set OSC target port (1-65535)\n";
            std::cout << "  --help, -h          Display this help message\n\n";
            std::cout << "Examples:\n";
            std::cout << "  OSCControlApp --host 192.168.1.100 --port 8000\n";
            std::cout << "  OSCControlApp --host localhost --port 7770\n\n";
            std::cout << "If no options are provided, the application uses saved configuration\n";
            std::cout << "or defaults to 127.0.0.1:7770.\n\n";
            std::cout << "The application listens for incoming OSC messages on port 7771.\n";
            quit();
            return;
        }
        
        // Validate command-line arguments if provided
        if (!host.isEmpty() || port != -1)
        {
            if (host.isEmpty())
            {
                std::cerr << "Error: --port specified without --host\n";
                std::cerr << "Use --help for usage information\n";
                quit();
                return;
            }
            
            if (port == -1)
            {
                std::cerr << "Error: --host specified without --port\n";
                std::cerr << "Use --help for usage information\n";
                quit();
                return;
            }
            
            if (port < 1 || port > 65535)
            {
                std::cerr << "Error: Invalid port number. Must be between 1 and 65535\n";
                quit();
                return;
            }
            
            mainWindow.reset(new MainWindow(getApplicationName(), host, port));
        }
        else
        {
            mainWindow.reset(new MainWindow(getApplicationName()));
        }
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                       .findColour(juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
           #endif

            setVisible(true);
        }
        
        MainWindow(juce::String name, const juce::String& host, int port)
            : DocumentWindow(name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                       .findColour(juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(host, port), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
           #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(OSCControlApplication)
