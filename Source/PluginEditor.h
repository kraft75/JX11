/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class JX11AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    JX11AudioProcessorEditor (JX11AudioProcessor&);
    ~JX11AudioProcessorEditor() override;

    //==============================================================================
//    Drawing the contents of the UI.
    void paint (juce::Graphics&) override;
//    Doing the layout of those contents.
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JX11AudioProcessor& audioProcessor;
    
//    Custom Design of UI elements
    LookAndFeel globalLNF;
    
//    Knob for controlling the synth’s parameters.
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
    juce::TextButton glideModeButton;
    
//    Attachment object between a parameter from the APVTS and a slider/button.
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;
    
//    Constructing the attachment requires:
//    1. A reference to the APVTS.
//    2. ID for the parameter to look up the corresponding
//    AudioParameterFloat in the APVTS.
//    3. Always expects a reference to the Slider object.
    SliderAttachment outputLevelAttachment {audioProcessor.apvts, ParameterID::outputLevel.getParamID(), outputLevelKnob.slider};
    
    SliderAttachment filterResoAttachment { audioProcessor.apvts, ParameterID::filterReso.getParamID(), filterResoKnob.slider };
    
    SliderAttachment filterFreqAttachment { audioProcessor.apvts, ParameterID::filterFreq.getParamID(), filterFreqKnob.slider };
    
    SliderAttachment filterEnvAttachment { audioProcessor.apvts, ParameterID::filterEnv.getParamID(),
        filterEnvKnob.slider };
    
    SliderAttachment filterLFOAttachment { audioProcessor.apvts, ParameterID::filterLFO.getParamID(),
        filterLFOKnob.slider };
    
    SliderAttachment filterVelocityAttachment { audioProcessor.apvts, ParameterID::filterVelocity.getParamID(), filterVelocityKnob.slider };
    
    SliderAttachment filterAttackAttachment { audioProcessor.apvts,
        ParameterID::filterAttack.getParamID(), filterAttackKnob.slider };
    
    SliderAttachment filterDecayAttachment { audioProcessor.apvts, ParameterID::filterDecay.getParamID(), filterDecayKnob.slider };
    
    SliderAttachment filterSustainAttachment { audioProcessor.apvts, ParameterID::filterSustain.getParamID(), filterSustainKnob.slider };
    
    SliderAttachment filterReleaseAttachment { audioProcessor.apvts, ParameterID::filterRelease.getParamID(), filterReleaseKnob.slider };
    
    SliderAttachment oscMixAttachment { audioProcessor.apvts, ParameterID::oscMix.getParamID(),
        oscMixKnob.slider};
    
    SliderAttachment oscTuneAttachment { audioProcessor.apvts, ParameterID::oscTune.getParamID(),
        oscTuneKnob.slider};
    
    SliderAttachment oscFineAttachment { audioProcessor.apvts, ParameterID::oscFine.getParamID(),
        oscFineKnob.slider};
    
    SliderAttachment glideRateAttachment { audioProcessor.apvts, ParameterID::glideRate.getParamID(),
        glideRateKnob.slider};
    
    SliderAttachment glideBendAttachment { audioProcessor.apvts, ParameterID::glideBend.getParamID(),
        glideBendKnob.slider};
    
    SliderAttachment envAttackAttachment { audioProcessor.apvts, ParameterID::envAttack.getParamID(),
        envAttackKnob.slider};
    
    SliderAttachment envDecayAttachment { audioProcessor.apvts, ParameterID::envDecay.getParamID(),
        envDecayKnob.slider};
    
    SliderAttachment envSustainAttachment { audioProcessor.apvts, ParameterID::envSustain.getParamID(), envSustainKnob.slider};
    
    SliderAttachment envReleaseAttachment { audioProcessor.apvts, ParameterID::envRelease.getParamID(), envReleaseKnob.slider};
    
    SliderAttachment lfoRateAttachment { audioProcessor.apvts, ParameterID::lfoRate.getParamID(),
        lfoRateKnob.slider};
    
    SliderAttachment vibratoAttachment { audioProcessor.apvts, ParameterID::vibrato.getParamID(),
        vibratoKnob.slider};
    
    SliderAttachment noiseAttachment { audioProcessor.apvts, ParameterID::noise.getParamID(),
        noiseKnob.slider};
    
    SliderAttachment octaveAttachment { audioProcessor.apvts, ParameterID::octave.getParamID(),
        octaveKnob.slider};
    
    SliderAttachment tuningAttachment { audioProcessor.apvts, ParameterID::tuning.getParamID(),
        tuningKnob.slider};

    ButtonAttachment polyModeAttachment {audioProcessor.apvts, ParameterID::polyMode.getParamID(),
        polyModeButton};
    
    ButtonAttachment glideModeAttachment {audioProcessor.apvts, ParameterID::glideMode.getParamID(),
        glideModeButton};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JX11AudioProcessorEditor)
};
