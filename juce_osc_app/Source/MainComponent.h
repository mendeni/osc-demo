#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <lo/lo.h>

class MainComponent : public juce::Component, public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // OSC Server
    lo_server_thread oscServer;
    static const int OSC_PORT = 7771;
    
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
