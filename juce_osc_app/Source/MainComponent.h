#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_osc/juce_osc.h>
#include <iostream>

class MainComponent : public juce::Component, 
                      public juce::Timer,
                      public juce::OSCReceiver::Listener<juce::OSCReceiver::MessageLoopCallback>
{
public:
    MainComponent();
    MainComponent(const juce::String& cmdLineHost, int cmdLinePort);
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    // OSC Receiver callbacks
    void oscMessageReceived(const juce::OSCMessage& message) override;
    void oscBundleReceived(const juce::OSCBundle& bundle) override;

private:
    // OSC Server
    juce::OSCReceiver oscReceiver;
    static const int OSC_PORT = 7771;
    
    // OSC Client
    juce::OSCSender oscSender;
    juce::String oscTargetHost;
    int oscTargetPort;
    
    // Configuration persistence
    std::unique_ptr<juce::PropertiesFile> properties;
    void initializePropertiesFile();
    void loadConfiguration();
    void saveConfiguration();
    void applyConfiguration();
    void onApplyButtonClicked();
    void initializeComponent();
    bool validateIPAddress(const juce::String& ip);
    bool validatePort(const juce::String& portStr);
    
    // Helper method to send OSC messages
    void sendOscMessage(const juce::String& address, const juce::OSCMessage& message);
    
    // Configuration UI Components
    juce::Label configTitleLabel;
    juce::Label addressLabel;
    juce::TextEditor addressEditor;
    juce::Label portLabel;
    juce::TextEditor portEditor;
    juce::TextButton applyButton;
    juce::Label statusLabel;
    
    // UI Components
    juce::ToggleButton toggleButton;
    juce::Label toggleLabel;
    juce::Label toggleValueLabel;
    
    juce::Slider horizontalSlider;
    juce::Label hSliderLabel;
    juce::Label hSliderValueLabel;
    
    juce::Slider verticalSlider;
    juce::Label vSliderLabel;
    juce::Label vSliderValueLabel;
    
    juce::Slider knobSlider;
    juce::Label knobLabel;
    juce::Label knobValueLabel;
    
    // Update flags
    std::atomic<bool> toggleNeedsUpdate{false};
    std::atomic<bool> hsliderNeedsUpdate{false};
    std::atomic<bool> vsliderNeedsUpdate{false};
    std::atomic<bool> knobNeedsUpdate{false};
    
    std::atomic<int> newToggleValue{0};
    std::atomic<float> newHSliderValue{0.0f};
    std::atomic<float> newVSliderValue{0.0f};
    std::atomic<float> newKnobValue{0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
