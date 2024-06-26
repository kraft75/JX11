/*
  ==============================================================================

    Scrollable.h
    Created: 24 Jun 2024 3:18:33pm
    Author:  MacJay

    To make a customized editor scrollable in JUCE, you can use the Viewport
    class, which allows you to create a scrollable area.
 
    Steps to Make a Scrollable Editor:
    1. Create a container component for the scrollable content.
    2. Wrap the container in a Viewport.
    3. Add the Viewport to your editor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotaryKnob.h"

//==============================================================================
/*
*/
class Scrollable  : public juce::Component
{
public:
    Scrollable();
    ~Scrollable() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    RotaryKnob outputLevelKnob;
    RotaryKnob filterResoKnob;
    RotaryKnob filterFreqKnob;
    RotaryKnob filterEnvKnob;
    RotaryKnob filterLFOKnob;
    RotaryKnob filterVelocityKnob;
    RotaryKnob oscMixKnob;
    RotaryKnob oscTuneKnob;
    RotaryKnob oscFineKnob;
    RotaryKnob glideRateKnob;
    RotaryKnob glideBendKnob;
    RotaryKnob glideModeKnob;
    RotaryKnob filterAttackKnob;
    RotaryKnob filterDecayKnob;
    RotaryKnob filterSustainKnob;
    RotaryKnob filterReleaseKnob;
    RotaryKnob envAttackKnob;
    RotaryKnob envDecayKnob;
    RotaryKnob envSustainKnob;
    RotaryKnob envReleaseKnob;
    RotaryKnob lfoRateKnob;
    RotaryKnob vibratoKnob;
    RotaryKnob noiseKnob;
    RotaryKnob octaveKnob;
    RotaryKnob tuningKnob;

//    Toggle buttons between Poly and Mono mode
    juce::TextButton polyModeButton;
    
//    Labels / Areas for aggregation of the knob types
    juce::Label oscillatorLabel;
    juce::Rectangle<int> oscArea;
    
    juce::Rectangle<int> filterArea;
    juce::Label filterLabel;
    
    juce::Rectangle<int> fenvArea;
    juce::Label fenvLabel;
    
    juce::Rectangle<int> envArea;
    juce::Label envLabel;
    
    juce::Rectangle<int> modArea;
    juce::Label modLabel;
    
    juce::Rectangle<int> outArea;
    juce::Label outLabel;
    
    

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Scrollable)
};
