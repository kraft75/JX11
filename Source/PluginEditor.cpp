/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor (JX11AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
//    Makes it a rotary slider.
    outputLevelKnob.label = "Level";
    filterResoKnob.label = "Reso";
    oscMixKnob.label = "Osc Mix";
    oscTuneKnob.label = "Osc Tune";
    oscFineKnob.label = "Osc Fine";
    glideRateKnob.label = "Glide Rate";
    glideBendKnob.label = "Glide Bend";
    filterFreqKnob.label = "Filter Freq";
    filterEnvKnob.label = "Filter Env";
    filterLFOKnob.label = "Filter LFO";
    filterVelocityKnob.label = "Filter Vel";
    filterAttackKnob.label = "Filter Attack";
    filterDecayKnob.label = "Filter Decay";
    filterSustainKnob.label = "Filter Sustain";
    filterReleaseKnob.label = "Filter Release";
    envAttackKnob.label = "Env Attack";
    envDecayKnob.label = "Env Decay";
    envSustainKnob.label = "Env Sustain";
    envReleaseKnob.label = "Env Release";
    lfoRateKnob.label = "LFO Rate";
    vibratoKnob.label = "Vibrato";
    noiseKnob.label = "Noise";
    octaveKnob.label = "Octave";
    tuningKnob.label = "Tuning";

    polyModeButton.setButtonText("Poly");
    polyModeButton.setClickingTogglesState(true);
    
    glideModeButton.setButtonText("On");
    glideModeButton.setClickingTogglesState(true);
    
//    Put it into the editor window.
    addAndMakeVisible(outputLevelKnob);
    addAndMakeVisible(filterResoKnob);
    addAndMakeVisible(filterFreqKnob);
    addAndMakeVisible(filterEnvKnob);
    addAndMakeVisible(filterLFOKnob);
    addAndMakeVisible(filterVelocityKnob);
    addAndMakeVisible(filterAttackKnob);
    addAndMakeVisible(filterDecayKnob);
    addAndMakeVisible(filterSustainKnob);
    addAndMakeVisible(filterSustainKnob);
    addAndMakeVisible(filterReleaseKnob);
    addAndMakeVisible(envAttackKnob);
    addAndMakeVisible(envDecayKnob);
    addAndMakeVisible(envSustainKnob);
    addAndMakeVisible(envReleaseKnob);
    addAndMakeVisible(lfoRateKnob);
    addAndMakeVisible(vibratoKnob);
    addAndMakeVisible(noiseKnob);
    addAndMakeVisible(octaveKnob);
    addAndMakeVisible(tuningKnob);
    addAndMakeVisible(oscMixKnob);
    addAndMakeVisible(oscTuneKnob);
    addAndMakeVisible(oscFineKnob);
    addAndMakeVisible(glideRateKnob);
    addAndMakeVisible(glideBendKnob);
    addAndMakeVisible(polyModeButton);
    addAndMakeVisible(glideModeButton);



    



    
//    Enabling the look-and-feel class.
    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);
    
    setSize (600, 400);
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void JX11AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JX11AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::Rectangle r(20, 20, 100, 120);
    outputLevelKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    filterResoKnob.setBounds(r);
    
    
    polyModeButton.setSize(80, 30);
    polyModeButton.setCentrePosition(r.withX(r.getRight()).getCentre());
    
   
}
