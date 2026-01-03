#include "MainComponent.h"

MainComponent::MainComponent()
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
    
    // Configure toggle button
    addAndMakeVisible(toggleButton);
    toggleButton.setButtonText("Toggle");
    toggleButton.onClick = [this] {
        std::cout << "Toggle clicked: " << (toggleButton.getToggleState() ? "ON" : "OFF") << std::endl;
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
        hSliderValueLabel.setText(juce::String(horizontalSlider.getValue(), 2), 
                                  juce::dontSendNotification);
        std::cout << "HSlider: " << horizontalSlider.getValue() << std::endl;
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
        vSliderValueLabel.setText(juce::String(verticalSlider.getValue(), 2), 
                                  juce::dontSendNotification);
        std::cout << "VSlider: " << verticalSlider.getValue() << std::endl;
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
        knobValueLabel.setText(juce::String(knobSlider.getValue(), 2), 
                              juce::dontSendNotification);
        std::cout << "Knob: " << knobSlider.getValue() << std::endl;
    };
    
    addAndMakeVisible(knobLabel);
    knobLabel.setText("Knob Control", juce::dontSendNotification);
    knobLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(knobValueLabel);
    knobValueLabel.setText("0.50", juce::dontSendNotification);
    knobValueLabel.setJustificationType(juce::Justification::centred);
    
    // Start timer for UI updates
    startTimer(50); // 20 FPS
    
    setSize(600, 500);
}

MainComponent::~MainComponent()
{
    if (oscServer)
    {
        lo_server_thread_stop(oscServer);
        lo_server_thread_free(oscServer);
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
