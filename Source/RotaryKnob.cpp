/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 18 Jun 2024 2:25:40pm
    Author:  MacJay

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RotaryKnob.h"

static constexpr int labelHeight = 15;
static constexpr int textBoxHeight = 20;
//==============================================================================
RotaryKnob::RotaryKnob()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
//    Get the rotary parameters to create customized
//    radians.
    auto rotaryParams = slider.getRotaryParameters();
    rotaryParams.startAngleRadians = 0.0f;
    rotaryParams.endAngleRadians = 1.5f * juce::MathConstants<float>::pi;
    slider.setRotaryParameters(rotaryParams);
    
//    Defying the slider type
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, textBoxHeight);
    addAndMakeVisible(slider);
    
    
    
//    Initial size which can be overridden by the layout code
//    in its parent component.
//    setBounds(0, 0, 100, 120);

}

RotaryKnob::~RotaryKnob()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void RotaryKnob::setTextValueSuffix(const juce::String& suffix)
{
    slider.setTextValueSuffix(suffix);
}

void RotaryKnob::paint (juce::Graphics& g)
{

//   clear the background.
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
//    Draws the label.
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    auto bounds = getLocalBounds();
    g.drawText (label, juce::Rectangle<int>{ 0, 0, bounds.getWidth(), labelHeight },
                juce::Justification::centred);
    
//    Drawing an outline around the entire component.
    g.setColour(juce::Colours::orange);
    g.drawRect(getLocalBounds(), 1);
 
}

void RotaryKnob::resized()
{
//    Getting a juce::Rectangle object with the current width and
//    height of the RotaryKnob component.
    auto bounds = getLocalBounds();
    
//    Define the area for the slider, leaving space for the label.
    auto sliderArea = bounds.removeFromTop(bounds.getHeight() - labelHeight);

//    Set the slider's bounds with the label space at the top.
    slider.setBounds(sliderArea.reduced(5));
}

void RotaryKnob::setTooltip(const juce::String& newTooltip)
{
        slider.setTooltip(newTooltip);
}
