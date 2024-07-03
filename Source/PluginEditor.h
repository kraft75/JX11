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
#include "Preset.h"
#include "Scrollable.h"

//==============================================================================
/**
*/
class JX11AudioProcessorEditor : public juce::AudioProcessorEditor,
                                 public juce::Button::Listener,
                                 public juce::ComboBox::Listener
{
public:
    JX11AudioProcessorEditor (JX11AudioProcessor&);
    ~JX11AudioProcessorEditor() override;

    //==============================================================================
//    Drawing the contents of the UI.
    void paint (juce::Graphics&) override;
//    Doing the layout of those contents.
    void resized() override;
    
//    Toggles the button text when pressed.
    void buttonClicked(juce::Button* button) override;
    
//    Reacts when a new preset is selected.
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
//    Updating the slider values based on the selected preset.
//    This method will retrieve the preset data from the audio processor
//    and apply it to the sliders.
    void applyPreset(int presetIndex);

private:
    
    JX11AudioProcessor& audioProcessor;
    
    juce::ComboBox presetSelector;

//    Creates a scrollable area for the component.
    juce::Viewport viewPort;
//    Content component that will be scrolled.
//    The contentComponent holds all the UI elements
//    and is displayed inside the viewPort.
    std::unique_ptr<juce::Component> contentComponent;

    
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
//    Track the text mode.
       bool isPolyMode;
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
    
    juce::Rectangle<int> polyArea;
    juce::Label polyLabel;
    
    juce::Label presetLabel;
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
    
    SliderAttachment glideModeAttachment { audioProcessor.apvts, ParameterID::glideMode.getParamID(),
        glideModeKnob.slider};
    
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
    
//    Helper functions.
    
//    Setting the bounds for a knob.
    void setKnobBounds(RotaryKnob& knob,
                       juce::Rectangle<int>& area,
                       int knobWidth,
                       int spacing);
    
    void setSectionBounds(juce::Label& label,
                          std::vector<RotaryKnob*>& knobs,
                          juce::Rectangle<int>& bounds,
                          int labelHeight,
                          int knobWidth,
                          int knobHeight,
                          int spacing);
    
//    Configuration of the labels.
    void configureLabel(juce::Label& label,
                        const juce::String& text,
                        const juce::Justification& justification,
                        float fontSize,
                        juce::Font::FontStyleFlags fontStyle);
    
    
   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JX11AudioProcessorEditor)
};
