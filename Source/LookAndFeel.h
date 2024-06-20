/*
  ==============================================================================

    LookAndFeel.h
    Created: 18 Jun 2024 4:08:15pm
    Author:  MacJay

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4 {
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
    
public:
    LookAndFeel();
    
//    Draw customized sliders.
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
};
