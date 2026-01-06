#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <lo/lo.h>
#include <iostream>

class MainComponent : public juce::Component, public juce::Timer
{
public:
    MainComponent();
    MainComponent(const juce::String& cmdLineHost, int cmdLinePort);
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // OSC Server
    lo_server_thread oscServer;
    static const int OSC_PORT = 7771;
    
    // OSC Client
    lo_address oscClient;
    juce::String oscTargetHost;
    int oscTargetPort;
    
    // Configuration persistence
    std::unique_ptr<juce::PropertiesFile> properties;
    bool useCommandLineConfig;
    void loadConfiguration();
    void saveConfiguration();
    void applyConfiguration();
    void initializeComponent();
    bool validateIPAddress(const juce::String& ip);
    bool validatePort(const juce::String& portStr);
    
    // Static handlers for OSC messages
    static int toggleHandler(const char *path, const char *types, lo_arg **argv, 
                            int argc, lo_message msg, void *user_data);
    static int hsliderHandler(const char *path, const char *types, lo_arg **argv, 
                             int argc, lo_message msg, void *user_data);
    static int vsliderHandler(const char *path, const char *types, lo_arg **argv, 
                             int argc, lo_message msg, void *user_data);
    static int knobHandler(const char *path, const char *types, lo_arg **argv, 
                          int argc, lo_message msg, void *user_data);
    static void errorHandler(int num, const char *msg, const char *path);
    
    // Helper method to send OSC messages
    void sendOscMessage(const char* address, const char* types, ...);
    
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
