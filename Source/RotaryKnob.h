/*
  ==============================================================================

    RotaryKnob.h
    Created: 18 Jun 2024 2:25:40pm
    Author:  MacJay
 
    RotaryKnob subclass that combines the juce::Slider object with a label into
    a custom component.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class RotaryKnob  : public juce::Component
{
public:
    RotaryKnob();
    ~RotaryKnob() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::Slider slider;
//    Drawn inside the bounds of the RotaryKnob by
//    the paint method.
    juce::String label;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};
