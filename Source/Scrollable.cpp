/*
  ==============================================================================

    Scrollable.cpp
    Created: 24 Jun 2024 3:18:33pm
    Author:  MacJay

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Scrollable.h"

//==============================================================================
Scrollable::Scrollable()
{
    outputLevelKnob.label = "Level";
    filterResoKnob.label = "Reso";
    oscMixKnob.label = "Osc Mix";
    oscTuneKnob.label = "Osc Tune";
    oscFineKnob.label = "Osc Fine";
    glideRateKnob.label = "Glide Rate";
    glideBendKnob.label = "Glide Bend";
    glideModeKnob.label = "Glide Mode";
    filterFreqKnob.label = "Frequency";
    filterEnvKnob.label = "Envelope";
    filterLFOKnob.label = "LFO";
    filterVelocityKnob.label = "Velocity";
    filterAttackKnob.label = "Attack";
    filterDecayKnob.label = "Decay";
    filterSustainKnob.label = "Sustain";
    filterReleaseKnob.label = "Release";
    envAttackKnob.label = "Attack";
    envDecayKnob.label = "Decay";
    envSustainKnob.label = "Sustain";
    envReleaseKnob.label = "Release";
    lfoRateKnob.label = "LFO Rate";
    vibratoKnob.label = "Vibrato";
    noiseKnob.label = "Noise";
    octaveKnob.label = "Octave";
    tuningKnob.label = "Tuning";

    polyModeButton.setButtonText("Poly");
    polyModeButton.setClickingTogglesState(true);
    polyModeButton.setButtonText("Mono");
    polyModeButton.setClickingTogglesState(true);
    
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
    addAndMakeVisible(glideModeKnob);
    addAndMakeVisible(polyModeButton);
    
//    Initialize and add the labels.
    oscillatorLabel.setText("Oscillator", juce::dontSendNotification);
    oscillatorLabel.setJustificationType(juce::Justification::centredLeft);
    oscillatorLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(oscillatorLabel);
    
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centredLeft);
    filterLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(filterLabel);
    
    fenvLabel.setText("Envelope Filter", juce::dontSendNotification);
    fenvLabel.setJustificationType(juce::Justification::centredLeft);
    fenvLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(fenvLabel);
    
    envLabel.setText("Envelope Volume", juce::dontSendNotification);
    envLabel.setJustificationType(juce::Justification::centredLeft);
    envLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(envLabel);
    
    modLabel.setText("Modulation", juce::dontSendNotification);
    modLabel.setJustificationType(juce::Justification::centredLeft);
    modLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(modLabel);

    outLabel.setText("Master", juce::dontSendNotification);
    outLabel.setJustificationType(juce::Justification::centredTop);
    outLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(outLabel);


}

Scrollable::~Scrollable()
{
}

void Scrollable::paint (juce::Graphics& g)
{
    
}

void Scrollable::resized()
{
    const int margin = 10;
    const int labelHeight = 20;
    const int knobWidth = 90;
    const int knobHeight = 120;
    const int buttonHeight = 30;
    const int buttonWidth = 60;
    const int spacing = 10;
   
// Create a working area within the editor window with margins
    auto bounds = getLocalBounds().reduced(margin);

//    Oscillator
    oscArea = bounds.removeFromTop(knobHeight + labelHeight);
    oscillatorLabel.setBounds(oscArea.removeFromTop(labelHeight));
    oscMixKnob.setBounds(oscArea.removeFromLeft(knobWidth).reduced(spacing));
    oscTuneKnob.setBounds(oscArea.removeFromLeft(knobWidth).reduced(spacing));
    oscFineKnob.setBounds(oscArea.removeFromLeft(knobWidth).reduced(spacing));
    noiseKnob.setBounds(oscArea.removeFromLeft(knobWidth).reduced(spacing));
    octaveKnob.setBounds(oscArea.removeFromLeft(knobWidth).reduced(spacing));
    tuningKnob.setBounds(oscArea.removeFromLeft(knobWidth).reduced(spacing));
//    Filter
    filterArea = bounds.removeFromTop(knobHeight + labelHeight);
    filterLabel.setBounds(filterArea.removeFromTop(labelHeight));
    filterResoKnob.setBounds(filterArea.removeFromLeft(knobWidth).reduced(spacing));
    filterFreqKnob.setBounds(filterArea.removeFromLeft(knobWidth).reduced(spacing));
    filterLFOKnob.setBounds(filterArea.removeFromLeft(knobWidth).reduced(spacing));
    filterVelocityKnob.setBounds(filterArea.removeFromLeft(knobWidth).reduced(spacing));
    filterEnvKnob.setBounds(filterArea.removeFromLeft(knobWidth).reduced(spacing));
//    Filter envelope
    fenvArea = bounds.removeFromTop(knobHeight + labelHeight);
    fenvLabel.setBounds(fenvArea.removeFromTop(labelHeight));
    filterAttackKnob.setBounds(fenvArea.removeFromLeft(knobWidth).reduced(spacing));
    filterDecayKnob.setBounds(fenvArea.removeFromLeft(knobWidth).reduced(spacing));
    filterSustainKnob.setBounds(fenvArea.removeFromLeft(knobWidth).reduced(spacing));
    filterReleaseKnob.setBounds(fenvArea.removeFromLeft(knobWidth).reduced(spacing));
//    Envelope
    envArea = bounds.removeFromTop(knobHeight + labelHeight);
    envLabel.setBounds(envArea.removeFromTop(labelHeight));
    envAttackKnob.setBounds(envArea.removeFromLeft(knobWidth).reduced(spacing));
    envDecayKnob.setBounds(envArea.removeFromLeft(knobWidth).reduced(spacing));
    envSustainKnob.setBounds(envArea.removeFromLeft(knobWidth).reduced(spacing));
    envReleaseKnob.setBounds(envArea.removeFromLeft(knobWidth).reduced(spacing));
    
//    Modulation
    modArea = bounds.removeFromTop(knobHeight + labelHeight);
    modLabel.setBounds(modArea.removeFromTop(labelHeight));
    glideModeKnob.setBounds(modArea.removeFromLeft(knobWidth).reduced(spacing));
    glideRateKnob.setBounds(modArea.removeFromLeft(knobWidth).reduced(spacing));
    glideBendKnob.setBounds(modArea.removeFromLeft(knobWidth).reduced(spacing));
    lfoRateKnob.setBounds(modArea.removeFromLeft(knobWidth).reduced(spacing));
    vibratoKnob.setBounds(modArea.removeFromLeft(knobWidth).reduced(spacing));
        
    int outputLevelKnobX = envReleaseKnob.getRight() - (3 * spacing) +
                            (2 * envReleaseKnob.getWidth());
    int outputLevelKnobY = envReleaseKnob.getY();

    outArea = juce::Rectangle<int>(outputLevelKnobX, outputLevelKnobY - labelHeight, envReleaseKnob.getWidth(), envReleaseKnob.getHeight() + labelHeight);
    outLabel.setBounds(outArea.removeFromTop(labelHeight));
    outputLevelKnob.setBounds(outArea);

//       polyModeButton.setBounds(rowArea.removeFromLeft(buttonWidth).reduced(spacing));
       
}
