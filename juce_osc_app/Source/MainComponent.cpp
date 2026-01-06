#include "MainComponent.h"
#include <cstdarg>

MainComponent::MainComponent()
    : useCommandLineConfig(false)
{
    initializePropertiesFile();
    loadConfiguration();
    initializeComponent();
}

MainComponent::MainComponent(const juce::String& cmdLineHost, int cmdLinePort)
    : useCommandLineConfig(true)
{
    initializePropertiesFile();
    
    // Use command-line provided configuration
    oscTargetHost = cmdLineHost;
    oscTargetPort = cmdLinePort;
    
    std::cout << "Using command-line configuration: " << oscTargetHost << ":" << oscTargetPort << std::endl;
    
    initializeComponent();
}

void MainComponent::initializePropertiesFile()
{
    // Initialize configuration
    juce::PropertiesFile::Options options;
    options.applicationName = "OSCControlApp";
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory).getChildFile("OSCControlApp").getFullPathName();
    
    // Create folder if it doesn't exist
    juce::File settingsDir(options.folderName);
    if (!settingsDir.exists())
        settingsDir.createDirectory();
    
    properties.reset(new juce::PropertiesFile(options));
}

void MainComponent::initializeComponent()
{
    // Set up OSC server
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", OSC_PORT);
    oscServer = lo_server_thread_new(port_str, errorHandler);
    
    if (oscServer)
    {
        // Register OSC handlers
        lo_server_thread_add_method(oscServer, "/toggle", "i", toggleHandler, this);
        lo_server_thread_add_method(oscServer, "/hslider", "f", hsliderHandler, this);
        lo_server_thread_add_method(oscServer, "/vslider", "f", vsliderHandler, this);
        lo_server_thread_add_method(oscServer, "/knob", "f", knobHandler, this);
        
        lo_server_thread_start(oscServer);
        std::cout << "OSC Server started on port " << OSC_PORT << std::endl;
        std::cout << "Listening for OSC messages on:" << std::endl;
        std::cout << "  /toggle  - integer (0=OFF, 1=ON)" << std::endl;
        std::cout << "  /hslider - float (0.0-1.0)" << std::endl;
        std::cout << "  /vslider - float (0.0-1.0)" << std::endl;
        std::cout << "  /knob    - float (0.0-1.0)" << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Failed to create OSC server on port " << OSC_PORT << std::endl;
    }
    
    // Set up OSC client
    oscClient = nullptr;
    applyConfiguration();
    
    // Configure configuration UI
    addAndMakeVisible(configTitleLabel);
    configTitleLabel.setText("OSC Target Configuration", juce::dontSendNotification);
    configTitleLabel.setJustificationType(juce::Justification::centredLeft);
    configTitleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    
    addAndMakeVisible(addressLabel);
    addressLabel.setText("Target Address:", juce::dontSendNotification);
    addressLabel.setJustificationType(juce::Justification::centredLeft);
    
    addAndMakeVisible(addressEditor);
    addressEditor.setText(oscTargetHost);
    addressEditor.setInputRestrictions(0); // Allow any characters for localhost or IP
    addressEditor.setTooltip("Enter IP address (e.g., 127.0.0.1) or hostname (e.g., localhost)");
    addressEditor.setReadOnly(useCommandLineConfig);
    
    addAndMakeVisible(portLabel);
    portLabel.setText("Target Port:", juce::dontSendNotification);
    portLabel.setJustificationType(juce::Justification::centredLeft);
    
    addAndMakeVisible(portEditor);
    portEditor.setText(juce::String(oscTargetPort));
    portEditor.setInputRestrictions(5, "0123456789");
    portEditor.setTooltip("Enter port number (1-65535)");
    portEditor.setReadOnly(useCommandLineConfig);
    
    addAndMakeVisible(applyButton);
    applyButton.setButtonText("Apply");
    applyButton.setEnabled(!useCommandLineConfig);
    applyButton.onClick = [this] { onApplyButtonClicked(); };
    
    addAndMakeVisible(statusLabel);
    statusLabel.setText("", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    
    // Show status message if using command-line configuration
    if (useCommandLineConfig)
    {
        statusLabel.setText("Using command-line configuration (read-only)", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    }
    
    // Configure toggle button
    addAndMakeVisible(toggleButton);
    toggleButton.setButtonText("Toggle");
    toggleButton.onClick = [this] {
        bool state = toggleButton.getToggleState();
        toggleValueLabel.setText(state ? "ON" : "OFF", juce::dontSendNotification);
        std::cout << "Toggle clicked: " << (state ? "ON" : "OFF") << std::endl;
        sendOscMessage("/toggle", "i", state ? 1 : 0);
    };
    
    addAndMakeVisible(toggleLabel);
    toggleLabel.setText("Toggle Control", juce::dontSendNotification);
    toggleLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(toggleValueLabel);
    toggleValueLabel.setText("OFF", juce::dontSendNotification);
    toggleValueLabel.setJustificationType(juce::Justification::centred);
    
    // Configure horizontal slider
    addAndMakeVisible(horizontalSlider);
    horizontalSlider.setRange(0.0, 1.0, 0.01);
    horizontalSlider.setValue(0.5);
    horizontalSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    horizontalSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    horizontalSlider.onValueChange = [this] {
        float value = static_cast<float>(horizontalSlider.getValue());
        hSliderValueLabel.setText(juce::String(value, 2), 
                                  juce::dontSendNotification);
        std::cout << "HSlider: " << value << std::endl;
        sendOscMessage("/hslider", "f", value);
    };
    
    addAndMakeVisible(hSliderLabel);
    hSliderLabel.setText("Horizontal Slider", juce::dontSendNotification);
    hSliderLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(hSliderValueLabel);
    hSliderValueLabel.setText("0.50", juce::dontSendNotification);
    hSliderValueLabel.setJustificationType(juce::Justification::centred);
    
    // Configure vertical slider
    addAndMakeVisible(verticalSlider);
    verticalSlider.setRange(0.0, 1.0, 0.01);
    verticalSlider.setValue(0.5);
    verticalSlider.setSliderStyle(juce::Slider::LinearVertical);
    verticalSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    verticalSlider.onValueChange = [this] {
        float value = static_cast<float>(verticalSlider.getValue());
        vSliderValueLabel.setText(juce::String(value, 2), 
                                  juce::dontSendNotification);
        std::cout << "VSlider: " << value << std::endl;
        sendOscMessage("/vslider", "f", value);
    };
    
    addAndMakeVisible(vSliderLabel);
    vSliderLabel.setText("Vertical Slider", juce::dontSendNotification);
    vSliderLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(vSliderValueLabel);
    vSliderValueLabel.setText("0.50", juce::dontSendNotification);
    vSliderValueLabel.setJustificationType(juce::Justification::centred);
    
    // Configure knob (rotary slider)
    addAndMakeVisible(knobSlider);
    knobSlider.setRange(0.0, 1.0, 0.01);
    knobSlider.setValue(0.5);
    knobSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knobSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knobSlider.onValueChange = [this] {
        float value = static_cast<float>(knobSlider.getValue());
        knobValueLabel.setText(juce::String(value, 2), 
                              juce::dontSendNotification);
        std::cout << "Knob: " << value << std::endl;
        sendOscMessage("/knob", "f", value);
    };
    
    addAndMakeVisible(knobLabel);
    knobLabel.setText("Knob Control", juce::dontSendNotification);
    knobLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(knobValueLabel);
    knobValueLabel.setText("0.50", juce::dontSendNotification);
    knobValueLabel.setJustificationType(juce::Justification::centred);
    
    // Start timer for UI updates
    startTimer(50); // 20 FPS
    
    setSize(600, 650);
}

MainComponent::~MainComponent()
{
    if (properties)
    {
        properties->saveIfNeeded();
    }
    
    if (oscServer)
    {
        lo_server_thread_stop(oscServer);
        lo_server_thread_free(oscServer);
    }
    
    if (oscClient)
    {
        lo_address_free(oscClient);
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("OSC Control Demo", getLocalBounds().removeFromTop(50), 
               juce::Justification::centred, true);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Space for title
    
    // Configuration section at the top
    auto configBounds = bounds.removeFromTop(120);
    configBounds = configBounds.reduced(10);
    
    configTitleLabel.setBounds(configBounds.removeFromTop(25));
    configBounds.removeFromTop(5); // spacing
    
    auto configRow1 = configBounds.removeFromTop(30);
    addressLabel.setBounds(configRow1.removeFromLeft(120));
    configRow1.removeFromLeft(5); // spacing
    addressEditor.setBounds(configRow1.removeFromLeft(200));
    configRow1.removeFromLeft(10); // spacing
    portLabel.setBounds(configRow1.removeFromLeft(80));
    configRow1.removeFromLeft(5); // spacing
    portEditor.setBounds(configRow1.removeFromLeft(80));
    
    configBounds.removeFromTop(5); // spacing
    auto configRow2 = configBounds.removeFromTop(30);
    applyButton.setBounds(configRow2.removeFromLeft(100));
    configRow2.removeFromLeft(10); // spacing
    statusLabel.setBounds(configRow2);
    
    bounds.removeFromTop(10); // spacing
    
    auto section = bounds.removeFromTop(100);
    
    // Toggle button section
    auto toggleSection = section.removeFromLeft(150);
    toggleLabel.setBounds(toggleSection.removeFromTop(25));
    toggleButton.setBounds(toggleSection.removeFromTop(30).reduced(10));
    toggleValueLabel.setBounds(toggleSection.removeFromTop(25));
    
    section.removeFromLeft(10); // spacing
    
    // Horizontal slider section
    auto hSliderSection = section;
    hSliderLabel.setBounds(hSliderSection.removeFromTop(25));
    hSliderValueLabel.setBounds(hSliderSection.removeFromBottom(25));
    horizontalSlider.setBounds(hSliderSection.reduced(10));
    
    bounds.removeFromTop(20); // spacing
    
    // Bottom section for vertical slider and knob
    auto bottomBounds = bounds;
    
    // Vertical slider section
    auto vSliderSection = bottomBounds.removeFromLeft(150);
    vSliderLabel.setBounds(vSliderSection.removeFromTop(25));
    vSliderValueLabel.setBounds(vSliderSection.removeFromBottom(25));
    verticalSlider.setBounds(vSliderSection.reduced(20));
    
    bottomBounds.removeFromLeft(50); // spacing
    
    // Knob section
    auto knobSection = bottomBounds.removeFromLeft(150);
    knobLabel.setBounds(knobSection.removeFromTop(25));
    knobValueLabel.setBounds(knobSection.removeFromBottom(25));
    knobSlider.setBounds(knobSection.reduced(20));
}

void MainComponent::timerCallback()
{
    // Update UI from OSC messages (thread-safe)
    if (toggleNeedsUpdate.load())
    {
        toggleNeedsUpdate.store(false);
        bool state = newToggleValue.load() != 0;
        toggleButton.setToggleState(state, juce::dontSendNotification);
        toggleValueLabel.setText(state ? "ON" : "OFF", juce::dontSendNotification);
    }
    
    if (hsliderNeedsUpdate.load())
    {
        hsliderNeedsUpdate.store(false);
        float value = newHSliderValue.load();
        horizontalSlider.setValue(value, juce::dontSendNotification);
        hSliderValueLabel.setText(juce::String(value, 2), juce::dontSendNotification);
    }
    
    if (vsliderNeedsUpdate.load())
    {
        vsliderNeedsUpdate.store(false);
        float value = newVSliderValue.load();
        verticalSlider.setValue(value, juce::dontSendNotification);
        vSliderValueLabel.setText(juce::String(value, 2), juce::dontSendNotification);
    }
    
    if (knobNeedsUpdate.load())
    {
        knobNeedsUpdate.store(false);
        float value = newKnobValue.load();
        knobSlider.setValue(value, juce::dontSendNotification);
        knobValueLabel.setText(juce::String(value, 2), juce::dontSendNotification);
    }
}

// Static OSC handler implementations
int MainComponent::toggleHandler(const char *path, const char *types, lo_arg **argv, 
                                 int argc, lo_message msg, void *user_data)
{
    MainComponent* component = static_cast<MainComponent*>(user_data);
    if (component && argc > 0)
    {
        int value = argv[0]->i;
        component->newToggleValue.store(value);
        component->toggleNeedsUpdate.store(true);
        std::cout << "OSC /toggle received: " << value << std::endl;
    }
    return 0;
}

int MainComponent::hsliderHandler(const char *path, const char *types, lo_arg **argv, 
                                  int argc, lo_message msg, void *user_data)
{
    MainComponent* component = static_cast<MainComponent*>(user_data);
    if (component && argc > 0)
    {
        float value = argv[0]->f;
        value = juce::jlimit(0.0f, 1.0f, value);
        component->newHSliderValue.store(value);
        component->hsliderNeedsUpdate.store(true);
        std::cout << "OSC /hslider received: " << value << std::endl;
    }
    return 0;
}

int MainComponent::vsliderHandler(const char *path, const char *types, lo_arg **argv, 
                                  int argc, lo_message msg, void *user_data)
{
    MainComponent* component = static_cast<MainComponent*>(user_data);
    if (component && argc > 0)
    {
        float value = argv[0]->f;
        value = juce::jlimit(0.0f, 1.0f, value);
        component->newVSliderValue.store(value);
        component->vsliderNeedsUpdate.store(true);
        std::cout << "OSC /vslider received: " << value << std::endl;
    }
    return 0;
}

int MainComponent::knobHandler(const char *path, const char *types, lo_arg **argv, 
                               int argc, lo_message msg, void *user_data)
{
    MainComponent* component = static_cast<MainComponent*>(user_data);
    if (component && argc > 0)
    {
        float value = argv[0]->f;
        value = juce::jlimit(0.0f, 1.0f, value);
        component->newKnobValue.store(value);
        component->knobNeedsUpdate.store(true);
        std::cout << "OSC /knob received: " << value << std::endl;
    }
    return 0;
}

void MainComponent::errorHandler(int num, const char *msg, const char *path)
{
    std::cerr << "OSC Error " << num << " in path " << (path ? path : "unknown") << ": " << msg << std::endl;
}

void MainComponent::sendOscMessage(const char* address, const char* types, ...)
{
    if (!oscClient)
    {
        std::cerr << "Error: OSC client not initialized" << std::endl;
        return;
    }
    
    va_list args;
    va_start(args, types);
    
    lo_message msg = lo_message_new();
    
    // Build message based on type string
    for (const char* t = types; *t != '\0'; ++t)
    {
        switch (*t)
        {
            case 'i':
                lo_message_add_int32(msg, va_arg(args, int));
                break;
            case 'f':
                lo_message_add_float(msg, static_cast<float>(va_arg(args, double)));
                break;
            case 's':
                lo_message_add_string(msg, va_arg(args, char*));
                break;
            default:
                std::cerr << "Unsupported OSC type: " << *t << std::endl;
                break;
        }
    }
    
    va_end(args);
    
    // Send message
    int result = lo_send_message(oscClient, address, msg);
    
    if (result == -1)
    {
        std::cerr << "Error sending OSC message to " << address << std::endl;
    }
    else
    {
        std::cout << "Sent OSC: " << address << " " << types << std::endl;
    }
    
    lo_message_free(msg);
}

void MainComponent::loadConfiguration()
{
    // Load saved configuration or use defaults
    oscTargetHost = properties->getValue("oscTargetHost", "127.0.0.1");
    oscTargetPort = properties->getIntValue("oscTargetPort", 7770);
    
    std::cout << "Loaded configuration: " << oscTargetHost << ":" << oscTargetPort << std::endl;
}

void MainComponent::saveConfiguration()
{
    properties->setValue("oscTargetHost", oscTargetHost);
    properties->setValue("oscTargetPort", oscTargetPort);
    properties->saveIfNeeded();
    
    std::cout << "Saved configuration: " << oscTargetHost << ":" << oscTargetPort << std::endl;
}

void MainComponent::applyConfiguration()
{
    // Free existing client if any
    if (oscClient)
    {
        lo_address_free(oscClient);
        oscClient = nullptr;
    }
    
    // Create new OSC client with configured values
    char target_port_str[16];
    snprintf(target_port_str, sizeof(target_port_str), "%d", oscTargetPort);
    oscClient = lo_address_new(oscTargetHost.toRawUTF8(), target_port_str);
    
    if (oscClient)
    {
        std::cout << "OSC Client initialized, sending to: " << oscTargetHost 
                  << ":" << oscTargetPort << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Failed to create OSC client" << std::endl;
    }
}

bool MainComponent::validateIPAddress(const juce::String& ip)
{
    // Simple IP address validation
    juce::StringArray parts;
    parts.addTokens(ip, ".", "");
    
    if (parts.size() != 4)
        return false;
    
    for (const auto& part : parts)
    {
        if (part.isEmpty())
            return false;
        
        // Check that the string only contains digits
        for (int i = 0; i < part.length(); ++i)
        {
            if (!juce::CharacterFunctions::isDigit(part[i]))
                return false;
        }
        
        // Now safe to convert to int
        int num = part.getIntValue();
        if (num < 0 || num > 255)
            return false;
        
        // Reject leading zeros (except for "0" itself)
        if (part.length() > 1 && part[0] == '0')
            return false;
    }
    
    return true;
}

bool MainComponent::validatePort(const juce::String& portStr)
{
    if (portStr.isEmpty())
        return false;
    
    // Check that the string only contains digits
    for (int i = 0; i < portStr.length(); ++i)
    {
        if (!juce::CharacterFunctions::isDigit(portStr[i]))
            return false;
    }
    
    // Now safe to convert to int
    int port = portStr.getIntValue();
    return port >= 1 && port <= 65535;
}

void MainComponent::onApplyButtonClicked()
{
    // Don't allow saving if using command-line config
    if (useCommandLineConfig)
    {
        statusLabel.setText("Cannot save: Using command-line configuration", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
        return;
    }
    
    juce::String address = addressEditor.getText();
    juce::String portStr = portEditor.getText();
    
    // Validate inputs
    if (address.isEmpty())
    {
        statusLabel.setText("Error: Address cannot be empty", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
        return;
    }
    
    if (!validatePort(portStr))
    {
        statusLabel.setText("Error: Invalid port number (1-65535)", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
        return;
    }
    
    // Special case: allow "localhost" as valid address
    if (address != "localhost" && !validateIPAddress(address))
    {
        statusLabel.setText("Error: Invalid IP address format", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
        return;
    }
    
    // Update configuration
    oscTargetHost = address;
    oscTargetPort = portStr.getIntValue();
    
    // Save and apply
    saveConfiguration();
    applyConfiguration();
    
    statusLabel.setText("Configuration applied successfully!", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::green);
}

