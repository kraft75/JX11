/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Synth.h"
#include "Preset.h"

//  juce::ParameterID is a parameter identifier that
//  combines a juce::String object with a version number
//  The version number ensures compatibility with future
//  plug-in updates. juce::String object gives each parameter
//  a unique name. Instead of hardcoding the string identifier
//  we use a preprocessor which automatically generates them.
namespace ParameterID {
#define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);
    PARAMETER_ID(oscMix)
    PARAMETER_ID(oscTune)
    PARAMETER_ID(oscFine)
    PARAMETER_ID(glideMode)
    PARAMETER_ID(glideRate)
    PARAMETER_ID(glideBend)
    PARAMETER_ID(filterFreq)
    PARAMETER_ID(filterReso)
    PARAMETER_ID(filterEnv)
    PARAMETER_ID(filterLFO)
    PARAMETER_ID(filterVelocity)
    PARAMETER_ID(filterAttack)
    PARAMETER_ID(filterDecay)
    PARAMETER_ID(filterSustain)
    PARAMETER_ID(filterRelease)
    PARAMETER_ID(envAttack)
    PARAMETER_ID(envDecay)
    PARAMETER_ID(envSustain)
    PARAMETER_ID(envRelease)
    PARAMETER_ID(lfoRate)
    PARAMETER_ID(vibrato)
    PARAMETER_ID(noise)
    PARAMETER_ID(octave)
    PARAMETER_ID(tuning)
    PARAMETER_ID(outputLevel)
    PARAMETER_ID(polyMode)
    #undef PARAMETER_ID
}

//==============================================================================
/**
*/
class JX11AudioProcessor  : public juce::AudioProcessor,
                            // Listens to parameter changes.
                            // Only changes when the value alters.
                            // It prevents unecessary computing.
                            private juce::ValueTree::Listener
{
public:
//    Determing whether the plug-in is
//    currently in learning mode or not.
    std::atomic<bool> midiLearn;
    //==============================================================================
    JX11AudioProcessor();
    ~JX11AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
//    This method is used to save the state of the audio processor
//    (including all parameters) into a memory block
//    (raw data, XML or ValueTree), which can be serialized
//    to save presets or for plugin state persistence.
    void getStateInformation (juce::MemoryBlock& destData) override;
    
//    This method is used to restore the state of the audio processor
//    (including all parameters) from a memory block.
//    This is typically used when loading presets or restoring
//    the plugin state.
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
//    AudioProcessorValueTreeState owns the parameters.
//    Save/restore plug-in state. Eases connection with GUI.
//    Adding the instance apvts.
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
//    Linking Processor and Editor in order to show the presets in
//    the customized GUI.
    const std::vector<Preset>& getPresets() const { return presets; }
    std::vector<std::string> getPresetNames() const;
//    int getPreset()

private:
    //==============================================================================
//    Adding the MIDI CC number to the XML.
//    A roundabout way because Juce doesn't guarantee
//    that getStateInformation and setStateInformation
//    will be called on any thread in particular.
    std::atomic<uint8_t> midiLearnCC;
    
//    Informs the audio thread (processBlock) about
//    parameter changes, which are then calculated.
//    When apvts notifies the listener a parameter
//    change, the listener set the atomic variable
//    true.
    std::atomic<bool> parametersChanged {false};
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        parametersChanged.store(true);
    }
    
//    Calculations to the new parameter values.
    void update();
    
    /*
        For MIDI Messages. Splitting the buffer in smaller pieces (p.81)
     */
    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);
    void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);
    
//    Pointers for each of the 26 parameters
    juce::AudioParameterFloat* oscMixParam;
    juce::AudioParameterFloat* oscTuneParam;
    juce::AudioParameterFloat* oscFineParam;
    juce::AudioParameterChoice* glideModeParam;
    juce::AudioParameterFloat* glideRateParam;
    juce::AudioParameterFloat* glideBendParam;
    juce::AudioParameterFloat* filterFreqParam;
    juce::AudioParameterFloat* filterResoParam;
    juce::AudioParameterFloat* filterEnvParam;
    juce::AudioParameterFloat* filterLFOParam;
    juce::AudioParameterFloat* filterVelocityParam;
    juce::AudioParameterFloat* filterAttackParam;
    juce::AudioParameterFloat* filterDecayParam;
    juce::AudioParameterFloat* filterSustainParam;
    juce::AudioParameterFloat* filterReleaseParam;
    juce::AudioParameterFloat* envAttackParam;
    juce::AudioParameterFloat* envDecayParam;
    juce::AudioParameterFloat* envSustainParam;
    juce::AudioParameterFloat* envReleaseParam;
    juce::AudioParameterFloat* lfoRateParam;
    juce::AudioParameterFloat* vibratoParam;
    juce::AudioParameterFloat* noiseParam;
    juce::AudioParameterFloat* octaveParam;
    juce::AudioParameterFloat* tuningParam;
    juce::AudioParameterFloat* outputLevelParam;
    juce::AudioParameterChoice* polyModeParam;
    
//    Instantiation of all the AudioParameterFloat
//    and AudioParameterChoice objects.
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
//        Helper object to construct aptvs
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
// -------------------------------------------------------------------------------------------------------------------------
                /*        Adding the parameters to the layout*/
        
//        Alternative: AudioParameterBool Poly on/off
        layout.add(std::make_unique<juce::AudioParameterChoice>(// Identifier
                                                                ParameterID::polyMode,
                                                                // Human readable name shown
                                                                // to the user by DAW
                                                                "Polyphony",
                                                                // List of choices. The default setting 1
                                                                // offers "Poly"
                                                                juce::StringArray{"Mono", "Poly"}, 1));
        
//        oscTune: Tuning the second oscillator up and down in semis
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscTune,
                                                               "Osc Tune",
                                                               // Allowed values for the parameter
                                                               // NormalisableRange maps internally from 0.0f -1.0f
                                                               juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
                                                               // Default value
                                                               -12.0f,
                                                               // Describes the label of the units
                                                               juce::AudioParameterFloatAttributes().withLabel("semi")));
        
//        oscFine: Fine tuning the second oscillator in cents (1/100th of a semitone)
//        Skew factor is more precision for tiny values by doing greater steps.
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscFine,
                                                               "Osc Fine",
                                                               // Min, Max, Smallest step size for adjustment, skew factor
//                                                             // true signals that skew happens from the center
                                                               juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3f, true),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("cent")));
        
//        Osc Mix: Mixes the second osc to the first. Shown as a ratio.
//        1. Lambda function converts float to a juce::String for display
//        in the UI.
        auto oscMixStringFromValue = [] (float value, int)
        {
            char s[16] = { 0 };
            snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value, 0.5f * value);
            return juce::String(s);
        };
//        Add to object layout. Call the lambda function from juce::AudioParameterFloatAttributes
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::oscMix,
                                                               "Osc Mix",
                                                               // The step size is determined by the UI
                                                               juce::NormalisableRange<float>(0.0f, 100.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes()
                                                               .withLabel("%")
//                                                              // Shows the ratio between osc1 and osc2
                                                               .withStringFromValueFunction(oscMixStringFromValue)));
        
//        Glide mode: On, off or glide when legato-style playing
        layout.add(std::make_unique<juce::AudioParameterChoice>(
                                                                ParameterID::glideMode,
                                                                "Glide Mode",
                                                                juce::StringArray { "Off", "Legato", "Always" },
                                                                0));
        
//        Glide rate: How long it takes to glide from one note to another
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::glideRate,
                                                               "Glide Rate",
                                                               juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
                                                               35.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        Glide bend: Adds an additional glide to any note
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::glideBend,
                                                               "Glide Bend",
                                                               juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("semi")));
        
//        Filter freq: One part of the Cut-off frequency
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterFreq,
                                                               "Filter Freq",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        Filter Reso: Resonance filter
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterReso,
                                                               "Filter Reso",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               15.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        Filter Env: Sets the intensity for the filter envelope
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterEnv,
                                                               "Filter Env",
                                                               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                               50.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
        
//        Filter LFO: Sets the intensity for the LFO modulation
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterLFO,
                                                               "Filter LFO",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        Filter Velocity: 0% disables velocity sensitivity for the filter modulation,
//        but still affects the amplitude of the notes. OFF mode disables velocity altogether.
//        All notes are equally loud.
        auto filterVelocityStringFromValue = [](float value, int) {
//            When slider is dragged all the way to left
            if (value < -90.0f)
                return juce::String("OFF");
            else
                return juce::String(value);
        };
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterVelocity,
                                                               "Velocity",
                                                               juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes()
                                                               .withLabel("%")
                                                               .withStringFromValueFunction(filterVelocityStringFromValue)));
        
//        The four following filters are ADSR parameters for
//        the filter envelope. 0% is fast, 100% is slow.
//        The sustainFilter is a percentage of the amplitude
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterAttack,
                                                               "Filter Attack",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterDecay,
                                                               "Filter Decay",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               30.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterSustain,
                                                               "Filter Sustain",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::filterRelease,
                                                               "Filter Release",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               25.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        ADSR parameters for the amplitude envelope
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::envAttack,
                                                               "Env Attack",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::envDecay,
                                                               "Env Decay",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               50.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        layout.add(std::make_unique<juce::AudioParameterFloat>( 
                                                               ParameterID::envSustain,
                                                               "Env Sustain",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               100.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>( 
                                                               ParameterID::envRelease,
                                                               "Env Release",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               30.0f,juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        LFO rate: Setting the frequency of the LFO. Lambda function deals with
//        mapping from float range values to Hz.
        auto lfoRateStringFromValue = [](float value, int) {
//            Mapping from 0 - 1 to a value in Hz.
            float lfoHz = std::exp(7.0f * value - 4.0f);
            
            return juce::String(lfoHz, 3);
        };
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::lfoRate,
                                                               "LFO Rate",
//                                                             // No arguments mean the parameter is
                                                               // a float between 0.0 and 1.0
                                                               juce::NormalisableRange<float>(),
                                                               0.81f,
                                                               juce::AudioParameterFloatAttributes()
                                                               .withLabel("Hz")
                                                               .withStringFromValueFunction(lfoRateStringFromValue)));
        
//        Vibrato: indicates the LFO how much modulation should be applied
        auto vibratoStringFromValue = [](float value, int) {
//            Turning the knob to the left creates a PWM. To the right vibrato
            if (value < 0.0f)
                return "PWM " + juce::String(-value, 1);
            else
                return juce::String(value, 1);
        };
        layout.add(std::make_unique<juce::AudioParameterFloat>( 
                                                               ParameterID::vibrato,
                                                               "Vibrato",
                                                               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes()
                                                               .withLabel("%")
                                                               .withStringFromValueFunction(vibratoStringFromValue)));
        
//        Noise: How much noise should be added to the signal
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::noise,
                                                               "Noise",
                                                               juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("%")));
        
//        Master tuning.Sets the overall tuning of the synth.
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::octave,
                                                               "Octave",
                                                               // Tunes the synth 2 octaves up and down.
                                                               // Useful for bass sound
                                                               juce::NormalisableRange<float>(-2.0f, 2.0f, 1.0f),
                                                               0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::tuning,
                                                               "Tuning",
                                                               // Fine tuning the synth by cents.
                                                               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("cent")));
        
//        Output level: Changes the output volume of the
//        synth from –24 to +6 dB.
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                               ParameterID::outputLevel,
                                                               "Output Level",
                                                               juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
                                                               0.0f,
                                                               juce::AudioParameterFloatAttributes().withLabel("dB")));
// -------------------------------------------------------------------------------------------------------------------------

        return layout;
    }
//    Synth obeject for rendering audio values
    Synth synth;
 
//    Fills up the presets vector.
    void createPrograms();
//    Contains all preset objects
    std::vector<Preset> presets;
//    Index of the currently selected preset
    int currentProgram;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JX11AudioProcessor)
};
