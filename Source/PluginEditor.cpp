/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor (JX11AudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), contentComponent(new juce::Component()), isPolyMode(false)
{
//    Enabling the look-and-feel class.
    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);
    
//    Labels for each knob.
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
    
//    Setting the suffix for each knob.
    outputLevelKnob.setTextValueSuffix(" dB");
    filterResoKnob.setTextValueSuffix(" %");
    oscMixKnob.setTextValueSuffix(" %");
    oscTuneKnob.setTextValueSuffix(" semi");
    oscFineKnob.setTextValueSuffix(" cent");
    glideRateKnob.setTextValueSuffix(" %");
    glideBendKnob.setTextValueSuffix(" semi");
    glideModeKnob.setTextValueSuffix("");
    filterFreqKnob.setTextValueSuffix(" %");
    filterEnvKnob.setTextValueSuffix(" %");
    filterLFOKnob.setTextValueSuffix(" %");
    filterVelocityKnob.setTextValueSuffix(" %");
    filterAttackKnob.setTextValueSuffix(" %");
    filterDecayKnob.setTextValueSuffix(" %");
    filterSustainKnob.setTextValueSuffix(" %");
    filterReleaseKnob.setTextValueSuffix(" %");
    envAttackKnob.setTextValueSuffix(" %");
    envDecayKnob.setTextValueSuffix(" %");
    envSustainKnob.setTextValueSuffix(" %");
    envReleaseKnob.setTextValueSuffix(" %");
    lfoRateKnob.setTextValueSuffix(" Hz");
    vibratoKnob.setTextValueSuffix(" %");
    noiseKnob.setTextValueSuffix(" %");
    octaveKnob.setTextValueSuffix(" octave");
    tuningKnob.setTextValueSuffix(" cent");
    
    polyModeButton.setButtonText(isPolyMode ? "Mono" : "Poly");
    polyModeButton.setClickingTogglesState(true);
    polyModeButton.addListener(this);
    midiLearnButton.setButtonText("Midi");
    midiLearnButton.addListener(this);
    
//      Add components to the contentComponent.
    contentComponent->addAndMakeVisible(oscMixKnob);
    contentComponent->addAndMakeVisible(oscTuneKnob);
    contentComponent->addAndMakeVisible(oscFineKnob);
    contentComponent->addAndMakeVisible(filterResoKnob);
    contentComponent->addAndMakeVisible(filterFreqKnob);
    contentComponent->addAndMakeVisible(filterEnvKnob);
    contentComponent->addAndMakeVisible(filterLFOKnob);
    contentComponent->addAndMakeVisible(filterVelocityKnob);
    contentComponent->addAndMakeVisible(polyModeButton);
    contentComponent->addAndMakeVisible(midiLearnButton);
    contentComponent->addAndMakeVisible(presetSelector);
    contentComponent->addAndMakeVisible(outputLevelKnob);
    contentComponent->addAndMakeVisible(filterAttackKnob);
    contentComponent->addAndMakeVisible(filterDecayKnob);
    contentComponent->addAndMakeVisible(filterSustainKnob);
    contentComponent->addAndMakeVisible(filterSustainKnob);
    contentComponent->addAndMakeVisible(filterReleaseKnob);
    contentComponent->addAndMakeVisible(envAttackKnob);
    contentComponent->addAndMakeVisible(envDecayKnob);
    contentComponent->addAndMakeVisible(envSustainKnob);
    contentComponent->addAndMakeVisible(envReleaseKnob);
    contentComponent->addAndMakeVisible(lfoRateKnob);
    contentComponent->addAndMakeVisible(vibratoKnob);
    contentComponent->addAndMakeVisible(noiseKnob);
    contentComponent->addAndMakeVisible(octaveKnob);
    contentComponent->addAndMakeVisible(tuningKnob);
    contentComponent->addAndMakeVisible(glideRateKnob);
    contentComponent->addAndMakeVisible(glideBendKnob);
    contentComponent->addAndMakeVisible(glideModeKnob);
    
//    Initialize and add the labels.
    configureLabel(oscillatorLabel, "Oscillator",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(filterLabel, "Filter",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(polyLabel, "Polyphony",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(midiLabel, "MIDI learn",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(presetLabel, "Presets",
                   juce::Justification::centredTop, 15.0f, juce::Font::bold);
    configureLabel(fenvLabel, "Envelope Filter",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(envLabel, "Envelope Volume",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(modLabel, "Modulation",
                   juce::Justification::centredLeft, 15.0f, juce::Font::bold);
    configureLabel(outLabel, "Master",
                   juce::Justification::centredTop, 15.0f, juce::Font::bold);
    
//      Add labels to the contentComponent.
    contentComponent->addAndMakeVisible(oscillatorLabel);
    contentComponent->addAndMakeVisible(filterLabel);
    contentComponent->addAndMakeVisible(polyLabel);
    contentComponent->addAndMakeVisible(presetLabel);
    contentComponent->addAndMakeVisible(envLabel);
    contentComponent->addAndMakeVisible(modLabel);
    contentComponent->addAndMakeVisible(outLabel);
    contentComponent->addAndMakeVisible(fenvLabel);
    contentComponent->addAndMakeVisible(presetSelector);
    contentComponent->addAndMakeVisible(midiLabel);

//    Fetch preset names from audioProcessor as
//    std::vector<std::string>
    std::vector<std::string> presetNames = audioProcessor.getPresetNames();
//    Convert std::vector<std::string> to StringArray
    juce::StringArray presetNamesArray;
    for (const auto& name : presetNames) {
        presetNamesArray.add(name);
    }

//    Use StringArray with addItemList() in your GUI
//    component (presetSelector)
    presetSelector.addItemList(presetNamesArray, 1);
//    Register presetSelector as a listener
    presetSelector.addListener(this);
    
//    Add the contentComponent to the viewport
    viewPort.setViewedComponent(contentComponent.get(), true);
    addAndMakeVisible(viewPort);

//    Set the size of the main editor.
    setSize (550, 600);
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    polyModeButton.removeListener(this); 
    midiLearnButton.removeListener(this);
    audioProcessor.midiLearn = false;
}

//==============================================================================
void JX11AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(188, 198, 204));

}

void JX11AudioProcessorEditor::resized()

{
    DBG("Resized called");
//      Define margins and spacing
    const int margin = 10;
    const int labelHeight = 20;
    const int knobWidth = 90;
    const int knobHeight = 120;
    const int buttonHeight = 30;
    const int buttonWidth = 60;
    const int presetHeight = 30;
    const int presetWidth = 100;
    const int spacing = 10;
   

//    Create a working area within the editor window with margins.
    viewPort.setBounds(getLocalBounds());
    
//    Set the size of the content component to be
//    larger than the viewport to enable scrolling.
    contentComponent->setSize(600, 800);
    
//       Create a working area within the editor window with margins
   auto bounds = contentComponent->getLocalBounds().reduced(margin);


//    Oscillator
    std::vector<RotaryKnob*> oscillatorKnobs = { &oscMixKnob, &oscTuneKnob,
        &oscFineKnob, &noiseKnob, &octaveKnob, &tuningKnob };
    setSectionBounds(oscillatorLabel, oscillatorKnobs, bounds, labelHeight,
                     knobWidth, knobHeight, spacing);


//    Filter
    std::vector<RotaryKnob*> filterKnobs = { &filterResoKnob, &filterFreqKnob,
        &filterLFOKnob, &filterVelocityKnob, &filterEnvKnob };
    setSectionBounds(filterLabel, filterKnobs, bounds, labelHeight,
                     knobWidth, knobHeight, spacing);
    
//    Filter envelope
    std::vector<RotaryKnob*> fEnvKnobs = { &filterAttackKnob, &filterDecayKnob,
        &filterSustainKnob, &filterReleaseKnob };
    setSectionBounds(fenvLabel, fEnvKnobs, bounds, labelHeight,
                     knobWidth, knobHeight, spacing);

    
//    Envelope
    std::vector<RotaryKnob*> envKnobs = { &envAttackKnob, &envDecayKnob,
        &envSustainKnob, &envReleaseKnob };
    setSectionBounds(envLabel, envKnobs, bounds, labelHeight,
                     knobWidth, knobHeight, spacing);

    
//    Modulation
    std::vector<RotaryKnob*> modKnobs = { &glideModeKnob, &glideRateKnob,
        &glideBendKnob, &lfoRateKnob, &vibratoKnob };
    setSectionBounds(modLabel, modKnobs, bounds, labelHeight,
                     knobWidth, knobHeight, spacing);

    
     
//    Master Volume
//    x- and y-coordinates for the outputLevelKnob.
    int outputLevelKnobX = envReleaseKnob.getRight() - (3 * spacing) +
                            (2 * envReleaseKnob.getWidth());
    int outputLevelKnobY = envReleaseKnob.getY();
//    Positioning
    outArea = juce::Rectangle<int>(outputLevelKnobX, outputLevelKnobY - labelHeight,
                    envReleaseKnob.getWidth(), envReleaseKnob.getHeight() + labelHeight);
    outLabel.setBounds(outArea.removeFromTop(labelHeight));
    outputLevelKnob.setBounds(outArea);
    
//    Polyphony
    polyLabel.setBounds(glideModeKnob.getX(), glideModeKnob.getBottom() + spacing,
                        knobWidth, labelHeight);
    polyModeButton.setBounds(polyLabel.getX(), polyLabel.getBottom() + spacing,
                             buttonWidth, buttonHeight);

//    Presets
    int polyLabelX = polyLabel.getRight() - (3 * spacing) +
                     (2 * envReleaseKnob.getWidth());
    int polyLabelY = glideBendKnob.getBottom() + spacing;
    
    presetLabel.setBounds(polyLabelX, polyLabelY, knobWidth, labelHeight);
    presetSelector.setBounds(presetLabel.getX(), presetLabel.getBottom() + spacing,
                             presetWidth, presetHeight);
    
//    Midi learn
    midiLabel.setBounds(vibratoKnob.getX(), vibratoKnob.getBottom() + spacing,
                        knobWidth, labelHeight);
    midiLearnButton.setBounds(midiLabel.getX(), midiLabel.getBottom() + spacing, buttonWidth, buttonHeight);

}

//Helper Functions

void JX11AudioProcessorEditor::setKnobBounds(RotaryKnob& knob, juce::Rectangle<int>& area, int knobWidth, int spacing)
{
        knob.setBounds(area.removeFromLeft(knobWidth).reduced(spacing));
}

void JX11AudioProcessorEditor::setSectionBounds(juce::Label& label, std::vector<RotaryKnob*>& knobs,
                      juce::Rectangle<int>& bounds, int labelHeight,
                      int knobWidth, int knobHeight, int spacing)
{
    
    auto sectionArea = bounds.removeFromTop(knobHeight + labelHeight);
    label.setBounds(sectionArea.removeFromTop(labelHeight));
    
    for (auto* knob : knobs) {
        setKnobBounds(*knob, sectionArea, knobWidth, spacing);
    }
     
    
}

void JX11AudioProcessorEditor::configureLabel(juce::Label& label,
                    const juce::String& text,
                    const juce::Justification& justification,
                    float fontSize,
                    juce::Font::FontStyleFlags fontStyle)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(justification);
    label.setFont(juce::Font(fontSize, fontStyle));
}


//Listener Functions

void JX11AudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &polyModeButton) {
//        Toggle the mode and update the button text
        isPolyMode = !isPolyMode;
        
        if (isPolyMode) {
                polyModeButton.setButtonText("Mono");
        } else {
                polyModeButton.setButtonText("Poly");
        }
    }
    
    if (button == &midiLearnButton) {
//        If MIDI Learn button is pressed, it should be
//        disabled until a MIDI CC code is received.
//        So it can’t click it more than once.
        button->setButtonText("Waiting...");
        button->setEnabled(false);
        audioProcessor.midiLearn = true;
        
//        The timer will start running and call
//        the timerCallback function ten times
//        per second. It happens on the JUCE
//        message thread which enables a safe
//        approach to any UI component.
        startTimerHz(10);
    }
}

void JX11AudioProcessorEditor::timerCallback()
{
//    If midiLearn is set back to false by the audio thread.
    if (!audioProcessor.midiLearn) {
//        It stops the timer...
        stopTimer();
//        ...and restores the button.
        midiLearnButton.setButtonText("MIDI");
        midiLearnButton.setEnabled(true);
    }
}

void JX11AudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetSelector) {
//        Getting the index of the selected preset.
        int selectedPresetIndex = presetSelector.getSelectedItemIndex();
        if (selectedPresetIndex >= 0) {
//            Let the audioprocessor set the appropiate setting.
            applyPreset(selectedPresetIndex);
        }
    }
}

void JX11AudioProcessorEditor::applyPreset(int presetIndex) 
{
//    It sets the actual preset.
    audioProcessor.setCurrentProgram(presetIndex);
}

