/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

//New name of the top-level XML element that contains separate child
//elements for the <Parameter> section as well as the learned MIDI CC number.
static const juce::Identifier pluginTag = "PLUGIN";
static const juce::Identifier extraTag = "EXTRA";
static const juce::Identifier midiCCAttribute = "midiCC";


//==============================================================================
JX11AudioProcessor::JX11AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
//              Grabs the parameter with the identifier
//              ParameterID::... from the APVTS and casts it
//              to juce::AudioParameter....
    
    Utils::castParameter(apvts, ParameterID::oscMix, oscMixParam);
    Utils::castParameter(apvts, ParameterID::oscTune, oscTuneParam);
    Utils::castParameter(apvts, ParameterID::oscFine, oscFineParam);
    Utils::castParameter(apvts, ParameterID::glideMode, glideModeParam);
    Utils::castParameter(apvts, ParameterID::glideRate, glideRateParam);
    Utils::castParameter(apvts, ParameterID::glideBend, glideBendParam);
    Utils::castParameter(apvts, ParameterID::filterFreq, filterFreqParam);
    Utils::castParameter(apvts, ParameterID::filterReso, filterResoParam);
    Utils::castParameter(apvts, ParameterID::filterEnv, filterEnvParam);
    Utils::castParameter(apvts, ParameterID::filterLFO, filterLFOParam);
    Utils::castParameter(apvts, ParameterID::filterVelocity, filterVelocityParam);
    Utils::castParameter(apvts, ParameterID::filterAttack, filterAttackParam);
    Utils::castParameter(apvts, ParameterID::filterDecay, filterDecayParam);
    Utils::castParameter(apvts, ParameterID::filterSustain, filterSustainParam);
    Utils::castParameter(apvts, ParameterID::filterRelease, filterReleaseParam);
    Utils::castParameter(apvts, ParameterID::envAttack, envAttackParam);
    Utils::castParameter(apvts, ParameterID::envDecay, envDecayParam);
    Utils::castParameter(apvts, ParameterID::envSustain, envSustainParam);
    Utils::castParameter(apvts, ParameterID::envRelease, envReleaseParam);
    Utils::castParameter(apvts, ParameterID::lfoRate, lfoRateParam);
    Utils::castParameter(apvts, ParameterID::vibrato, vibratoParam);
    Utils::castParameter(apvts, ParameterID::noise, noiseParam);
    Utils::castParameter(apvts, ParameterID::octave, octaveParam);
    Utils::castParameter(apvts, ParameterID::tuning, tuningParam);
    Utils::castParameter(apvts, ParameterID::outputLevel, outputLevelParam);
    Utils::castParameter(apvts, ParameterID::polyMode, polyModeParam);
    
    
    createPrograms();
    setCurrentProgram(0);
    
//    Add the listener.
//    Connecting the valueTreePropertyChanged
//    method to the APVTS.
    apvts.state.addListener(this);
    
}

JX11AudioProcessor::~JX11AudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String JX11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JX11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JX11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JX11AudioProcessor::getNumPrograms()
{
//    Number of presets(programs)
    return int(presets.size());
}

int JX11AudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void JX11AudioProcessor::setCurrentProgram (int index)
{
    currentProgram = index;
 
//    Knobs
//    AudioParameterFloat and AudioParameterChoice objects.
    juce::RangedAudioParameter *params[NUM_PARAMS] = {
            oscMixParam,
            oscTuneParam,
            oscFineParam,
            glideModeParam,
            glideRateParam,
            glideBendParam,
            filterFreqParam,
            filterResoParam,
            filterEnvParam,
            filterLFOParam,
            filterVelocityParam,
            filterAttackParam,
            filterDecayParam,
            filterSustainParam,
            filterReleaseParam,
            envAttackParam,
            envDecayParam,
            envSustainParam,
            envReleaseParam,
            lfoRateParam,
            vibratoParam,
            noiseParam,
            octaveParam,
            tuningParam,
            outputLevelParam,
            polyModeParam,
    };
    
//    Preset object extracted from the vector
    const Preset& preset = presets[index];
    
//     It loops through the 26 elements in the Preset
//    object’s params array and assigns their values
//    to the appropriate AudioParameterFloat and
//    AudioParameterChoice objects.
    for (int i = 0; i < NUM_PARAMS; ++i) {
        
//     Call setValueNotifyingHost on the audio parameter object,
//     in order to inform the host application that the parameter
//     has changed.
//     Internally JUCE uses the values 0.0 – 1.0.
//     Calling convertTo0to1 converts the parameter’s
//     real value into that range.
        params[i]->setValueNotifyingHost(params[i]->convertTo0to1(preset.param[i]));
    }
    
//    If the synth doesn't stop playing, still playing
//    notes cause  an effect on changing presets.
//    Parameter values could have big jumps, which leads
//    for example to infinite audio values.
    reset();
}

const juce::String JX11AudioProcessor::getProgramName (int index)
{
    return {presets[index].name};
}

void JX11AudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
//    Not supported by JX11
}

void JX11AudioProcessor::createPrograms()
{
    presets.emplace_back("Init", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 100.00f, 15.00f, 50.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 0.00f, 50.00f, 100.00f, 30.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("5th Sweep Pad", 100.00f, -7.00f, -6.30f, 1.00f, 32.00f, 0.00f, 90.00f, 60.00f, -76.00f, 0.00f, 0.00f, 90.00f, 89.00f, 90.00f, 73.00f, 0.00f, 50.00f, 100.00f, 71.00f, 0.81f, 30.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Echo Pad [SA]", 88.00f, 0.00f, 0.00f, 0.00f, 49.00f, 0.00f, 46.00f, 76.00f, 38.00f, 10.00f, 38.00f, 100.00f, 86.00f, 76.00f, 57.00f, 30.00f, 80.00f, 68.00f, 66.00f, 0.79f, -74.00f, 25.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Space Chimes [SA]", 88.00f, 0.00f, 0.00f, 0.00f, 49.00f, 0.00f, 49.00f, 82.00f, 32.00f, 8.00f, 78.00f, 85.00f, 69.00f, 76.00f, 47.00f, 12.00f, 22.00f, 55.00f, 66.00f, 0.89f, -32.00f, 0.00f, 2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Solid Backing", 100.00f, -12.00f, -18.70f, 0.00f, 35.00f, 0.00f, 30.00f, 25.00f, 40.00f, 0.00f, 26.00f, 0.00f, 35.00f, 0.00f, 25.00f, 0.00f, 50.00f, 100.00f, 30.00f, 0.81f, 0.00f, 50.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Velocity Backing [SA]", 41.00f, 0.00f, 9.70f, 0.00f, 8.00f, -1.68f, 49.00f, 1.00f, -32.00f, 0.00f, 86.00f, 61.00f, 87.00f, 100.00f, 93.00f, 11.00f, 48.00f, 98.00f, 32.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Rubber Backing [ZF]", 29.00f, 12.00f, -5.60f, 0.00f, 18.00f, 5.06f, 35.00f, 15.00f, 54.00f, 14.00f, 8.00f, 0.00f, 42.00f, 13.00f, 21.00f, 0.00f, 56.00f, 0.00f, 32.00f, 0.20f, 16.00f, 22.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("808 State Lead", 100.00f, 7.00f, -7.10f, 2.00f, 34.00f, 12.35f, 65.00f, 63.00f, 50.00f, 16.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 17.00f, 50.00f, 100.00f, 3.00f, 0.81f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Mono Glide", 0.00f, -12.00f, 0.00f, 2.00f, 46.00f, 0.00f, 51.00f, 0.00f, 0.00f, 0.00f, -100.00f, 0.00f, 30.00f, 0.00f, 25.00f, 37.00f, 50.00f, 100.00f, 38.00f, 0.81f, 24.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Detuned Techno Lead", 84.00f, 0.00f, -17.20f, 2.00f, 41.00f, -0.15f, 54.00f, 1.00f, 16.00f, 21.00f, 34.00f, 0.00f, 9.00f, 100.00f, 25.00f, 20.00f, 85.00f, 100.00f, 30.00f, 0.83f, -82.00f, 40.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Hard Lead [SA]", 71.00f, 12.00f, 0.00f, 0.00f, 24.00f, 36.00f, 56.00f, 52.00f, 38.00f, 19.00f, 40.00f, 100.00f, 14.00f, 65.00f, 95.00f, 7.00f, 91.00f, 100.00f, 15.00f, 0.84f, -34.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Bubble", 0.00f, -12.00f, -0.20f, 0.00f, 71.00f, -0.00f, 23.00f, 77.00f, 60.00f, 32.00f, 26.00f, 40.00f, 18.00f, 66.00f, 14.00f, 0.00f, 38.00f, 65.00f, 16.00f, 0.48f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Monosynth", 62.00f, -12.00f, 0.00f, 1.00f, 35.00f, 0.02f, 64.00f, 39.00f, 2.00f, 65.00f, -100.00f, 7.00f, 52.00f, 24.00f, 84.00f, 13.00f, 30.00f, 76.00f, 21.00f, 0.58f, -40.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Moogcury Lite", 81.00f, 24.00f, -9.80f, 1.00f, 15.00f, -0.97f, 39.00f, 17.00f, 38.00f, 40.00f, 24.00f, 0.00f, 47.00f, 19.00f, 37.00f, 0.00f, 50.00f, 20.00f, 33.00f, 0.38f, 6.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Gangsta Whine", 0.00f, 0.00f, 0.00f, 2.00f, 44.00f, 0.00f, 41.00f, 46.00f, 0.00f, 0.00f, -100.00f, 0.00f, 0.00f, 100.00f, 25.00f, 15.00f, 50.00f, 100.00f, 32.00f, 0.81f, -2.00f, 0.00f, 2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Higher Synth [ZF]", 48.00f, 0.00f, -8.80f, 0.00f, 0.00f, 0.00f, 50.00f, 47.00f, 46.00f, 30.00f, 60.00f, 0.00f, 10.00f, 0.00f, 7.00f, 0.00f, 42.00f, 0.00f, 22.00f, 0.21f, 18.00f, 16.00f, 2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("303 Saw Bass", 0.00f, 0.00f, 0.00f, 1.00f, 49.00f, 0.00f, 55.00f, 75.00f, 38.00f, 35.00f, 0.00f, 0.00f, 56.00f, 0.00f, 56.00f, 0.00f, 80.00f, 100.00f, 24.00f, 0.26f, -2.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("303 Square Bass", 75.00f, 0.00f, 0.00f, 1.00f, 49.00f, 0.00f, 55.00f, 75.00f, 38.00f, 35.00f, 0.00f, 14.00f, 49.00f, 0.00f, 39.00f, 0.00f, 80.00f, 100.00f, 24.00f, 0.26f, -2.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Analog Bass", 100.00f, -12.00f, -10.90f, 1.00f, 19.00f, 0.00f, 30.00f, 51.00f, 70.00f, 9.00f, -100.00f, 0.00f, 88.00f, 0.00f, 21.00f, 0.00f, 50.00f, 100.00f, 46.00f, 0.81f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Analog Bass 2", 100.00f, -12.00f, -10.90f, 0.00f, 19.00f, 13.44f, 48.00f, 43.00f, 88.00f, 0.00f, 60.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 61.00f, 100.00f, 32.00f, 0.81f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Low Pulses", 97.00f, -12.00f, -3.30f, 0.00f, 35.00f, 0.00f, 80.00f, 40.00f, 4.00f, 0.00f, 0.00f, 0.00f, 77.00f, 0.00f, 25.00f, 0.00f, 50.00f, 100.00f, 30.00f, 0.81f, -68.00f, 0.00f, -2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Sine Infra-Bass", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 33.00f, 76.00f, 6.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 0.00f, 55.00f, 25.00f, 30.00f, 0.81f, 4.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Wobble Bass [SA]", 100.00f, -12.00f, -8.80f, 0.00f, 82.00f, 0.21f, 72.00f, 47.00f, -32.00f, 34.00f, 64.00f, 20.00f, 69.00f, 100.00f, 15.00f, 9.00f, 50.00f, 100.00f, 7.00f, 0.81f, -8.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Squelch Bass", 100.00f, -12.00f, -8.80f, 0.00f, 35.00f, 0.00f, 67.00f, 70.00f, -48.00f, 0.00f, 0.00f, 48.00f, 69.00f, 100.00f, 15.00f, 0.00f, 50.00f, 100.00f, 7.00f, 0.81f, -8.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Rubber Bass [ZF]", 49.00f, -12.00f, 1.60f, 1.00f, 35.00f, 0.00f, 36.00f, 15.00f, 50.00f, 20.00f, 0.00f, 0.00f, 38.00f, 0.00f, 25.00f, 0.00f, 60.00f, 100.00f, 22.00f, 0.19f, 0.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Soft Pick Bass", 37.00f, 0.00f, 7.80f, 0.00f, 22.00f, 0.00f, 33.00f, 47.00f, 42.00f, 16.00f, 18.00f, 0.00f, 0.00f, 0.00f, 25.00f, 4.00f, 58.00f, 0.00f, 22.00f, 0.15f, -12.00f, 33.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Fretless Bass", 50.00f, 0.00f, -14.40f, 1.00f, 34.00f, 0.00f, 51.00f, 0.00f, 16.00f, 0.00f, 34.00f, 0.00f, 9.00f, 0.00f, 25.00f, 20.00f, 85.00f, 0.00f, 30.00f, 0.81f, 40.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Whistler", 23.00f, 0.00f, -0.70f, 0.00f, 35.00f, 0.00f, 33.00f, 100.00f, 0.00f, 0.00f, 0.00f, 0.00f, 29.00f, 0.00f, 25.00f, 68.00f, 39.00f, 58.00f, 36.00f, 0.81f, 28.00f, 38.00f, 2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Very Soft Pad", 39.00f, 0.00f, -4.90f, 2.00f, 12.00f, 0.00f, 35.00f, 78.00f, 0.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 35.00f, 50.00f, 80.00f, 70.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Pizzicato", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 23.00f, 20.00f, 50.00f, 0.00f, 0.00f, 0.00f, 22.00f, 0.00f, 25.00f, 0.00f, 47.00f, 0.00f, 30.00f, 0.81f, 0.00f, 80.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Synth Strings", 100.00f, 0.00f, -7.10f, 0.00f, 0.00f, -0.97f, 42.00f, 26.00f, 50.00f, 14.00f, 38.00f, 0.00f, 67.00f, 55.00f, 97.00f, 82.00f, 70.00f, 100.00f, 42.00f, 0.84f, 34.00f, 30.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Synth Strings 2", 75.00f, 0.00f, -3.80f, 0.00f, 49.00f, 0.00f, 55.00f, 16.00f, 38.00f, 8.00f, -60.00f, 76.00f, 29.00f, 76.00f, 100.00f, 46.00f, 80.00f, 100.00f, 39.00f, 0.79f, -46.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Leslie Organ", 0.00f, 0.00f, 0.00f, 0.00f, 13.00f, -0.38f, 38.00f, 74.00f, 8.00f, 20.00f, -100.00f, 0.00f, 55.00f, 52.00f, 31.00f, 0.00f, 17.00f, 73.00f, 28.00f, 0.87f, -52.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Click Organ", 50.00f, 12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 44.00f, 50.00f, 30.00f, 16.00f, -100.00f, 0.00f, 0.00f, 18.00f, 0.00f, 0.00f, 75.00f, 80.00f, 0.00f, 0.81f, -2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Hard Organ", 89.00f, 19.00f, -0.90f, 0.00f, 35.00f, 0.00f, 51.00f, 62.00f, 8.00f, 0.00f, -100.00f, 0.00f, 37.00f, 0.00f, 100.00f, 4.00f, 8.00f, 72.00f, 4.00f, 0.77f, -2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Bass Clarinet", 100.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 51.00f, 10.00f, 0.00f, 11.00f, 0.00f, 0.00f, 0.00f, 0.00f, 25.00f, 35.00f, 65.00f, 65.00f, 32.00f, 0.79f, -2.00f, 20.00f, -1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Trumpet", 0.00f, 0.00f, 0.00f, 1.00f, 6.00f, 0.00f, 57.00f, 0.00f, -36.00f, 15.00f, 0.00f, 21.00f, 15.00f, 0.00f, 25.00f, 24.00f, 60.00f, 80.00f, 10.00f, 0.75f, 10.00f, 25.00f, 1.00f, 0.00f, 0.00f, 0.00f);
   presets.emplace_back("Soft Horn", 12.00f, 19.00f, 1.90f, 0.00f, 35.00f, 0.00f, 50.00f, 21.00f, -42.00f, 12.00f, 20.00f, 0.00f, 35.00f, 36.00f, 25.00f, 8.00f, 50.00f, 100.00f, 27.00f, 0.83f, 2.00f, 10.00f, -1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Brass Section", 43.00f, 12.00f, -7.90f, 0.00f, 28.00f, -0.79f, 50.00f, 0.00f, 18.00f, 0.00f, 0.00f, 24.00f, 16.00f, 91.00f, 8.00f, 17.00f, 50.00f, 80.00f, 45.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Synth Brass", 40.00f, 0.00f, -6.30f, 0.00f, 30.00f, -3.07f, 39.00f, 15.00f, 50.00f, 0.00f, 0.00f, 39.00f, 30.00f, 82.00f, 25.00f, 33.00f, 74.00f, 76.00f, 41.00f, 0.81f, -6.00f, 23.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Detuned Syn Brass [ZF]", 68.00f, 0.00f, 31.80f, 0.00f, 31.00f, 0.50f, 26.00f, 7.00f, 70.00f, 0.00f, 32.00f, 0.00f, 83.00f, 0.00f, 5.00f, 0.00f, 75.00f, 54.00f, 32.00f, 0.76f, -26.00f, 29.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Power PWM", 100.00f, -12.00f, -8.80f, 0.00f, 35.00f, 0.00f, 82.00f, 13.00f, 50.00f, 0.00f, -100.00f, 24.00f, 30.00f, 88.00f, 34.00f, 0.00f, 50.00f, 100.00f, 48.00f, 0.71f, -26.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Water Velocity [SA]", 76.00f, 0.00f, -1.40f, 0.00f, 49.00f, 0.00f, 87.00f, 67.00f, 100.00f, 32.00f, -82.00f, 95.00f, 56.00f, 72.00f, 100.00f, 4.00f, 76.00f, 11.00f, 46.00f, 0.88f, 44.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Ghost [SA]", 75.00f, 0.00f, -7.10f, 2.00f, 16.00f, -0.00f, 38.00f, 58.00f, 50.00f, 16.00f, 62.00f, 0.00f, 30.00f, 40.00f, 31.00f, 37.00f, 50.00f, 100.00f, 54.00f, 0.85f, 66.00f, 43.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Soft E.Piano", 31.00f, 0.00f, -0.20f, 0.00f, 35.00f, 0.00f, 34.00f, 26.00f, 6.00f, 0.00f, 26.00f, 0.00f, 22.00f, 0.00f, 39.00f, 0.00f, 80.00f, 0.00f, 44.00f, 0.81f, 2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Thumb Piano", 72.00f, 15.00f, 50.00f, 0.00f, 35.00f, 0.00f, 37.00f, 47.00f, 8.00f, 0.00f, 0.00f, 0.00f, 45.00f, 0.00f, 39.00f, 0.00f, 39.00f, 0.00f, 48.00f, 0.81f, 20.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Steel Drums [ZF]", 81.00f, 12.00f, -12.00f, 0.00f, 18.00f, 2.30f, 40.00f, 30.00f, 8.00f, 17.00f, -20.00f, 0.00f, 42.00f, 23.00f, 47.00f, 12.00f, 48.00f, 0.00f, 49.00f, 0.53f, -28.00f, 34.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Car Horn", 57.00f, -1.00f, -2.80f, 0.00f, 35.00f, 0.00f, 46.00f, 0.00f, 36.00f, 0.00f, 0.00f, 46.00f, 30.00f, 100.00f, 23.00f, 30.00f, 50.00f, 100.00f, 31.00f, 1.00f, -24.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Helicopter", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 8.00f, 36.00f, 38.00f, 100.00f, 0.00f, 100.00f, 100.00f, 0.00f, 100.00f, 96.00f, 50.00f, 100.00f, 92.00f, 0.97f, 0.00f, 100.00f, -2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Arctic Wind", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 16.00f, 85.00f, 0.00f, 28.00f, 0.00f, 37.00f, 30.00f, 0.00f, 25.00f, 89.00f, 50.00f, 100.00f, 89.00f, 0.24f, 0.00f, 100.00f, 2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Thip", 100.00f, -7.00f, 0.00f, 0.00f, 35.00f, 0.00f, 0.00f, 100.00f, 94.00f, 0.00f, 0.00f, 2.00f, 20.00f, 0.00f, 20.00f, 0.00f, 46.00f, 0.00f, 30.00f, 0.81f, 0.00f, 78.00f, 0.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Synth Tom", 0.00f, -12.00f, 0.00f, 0.00f, 76.00f, 24.53f, 30.00f, 33.00f, 52.00f, 0.00f, 36.00f, 0.00f, 59.00f, 0.00f, 59.00f, 10.00f, 50.00f, 0.00f, 50.00f, 0.81f, 0.00f, 70.00f, -2.00f, 0.00f, 0.00f, 1.00f);
   presets.emplace_back("Squelchy Frog", 50.00f, -5.00f, -7.90f, 2.00f, 77.00f, -36.00f, 40.00f, 65.00f, 90.00f, 0.00f, 0.00f, 33.00f, 50.00f, 0.00f, 25.00f, 0.00f, 70.00f, 65.00f, 18.00f, 0.32f, 100.00f, 0.00f, -2.00f, 0.00f, 0.00f, 1.00f);
}

std::vector<std::string> JX11AudioProcessor::getPresetNames() const
{
    std::vector<std::string> names;
    for (const auto& preset : presets) {
        names.push_back(preset.name);
    }
    return names;
}
//==============================================================================
void JX11AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
//    Synth instance reacts to changes
//    In JX11 they don't allocate, but still worth setting up
    synth.allocateResources(sampleRate, samplesPerBlock);
    
//    Forces update() to be executed when processBlock()
//    is called the first time.
//    The synth will initialized with the initial state of
//    the parameters.
    parametersChanged.store(true);
    
    reset();
}

void JX11AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    synth.deallocateResources();
}

void JX11AudioProcessor::reset()
{
//    Call synth reset() from main class
    synth.reset();
    
//    Informs the smoother about the initial settting of the
//    output level.
    synth.outputLevelSmoother.setCurrentAndTargetValue(
            juce::Decibels::decibelsToGain(outputLevelParam->get()));
    
    midiLearn = false;
    midiLearnCC = synth.resoCC;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JX11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JX11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
//    We can access midiLearnCC variable from any
//    thread, since it’s atomic.
    synth.resoCC = midiLearnCC;
    
//    Thread-safe check, whether parametersChanged is true.
    bool expected = true;
//    if parametersChanged is set by the UI thread to true
//    it calls the update method and automatically the audio
//    thread sets parametersChanged to false.
//    isNonRealtime() deals with adding automation to the
//    parameters. If the DAW works in offline mode it is faster
//    than realtime. Preventing loss of parameter changes, we always
//    update if isNonRealtime() is true. p.175
    if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        update();
    }
    
//    Processing midi messages by timestamps.
//    Therefore, split them by events.
    splitBufferByEvents(buffer, midiMessages);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

void JX11AudioProcessor::update()
{
    
//    Noise
//    --------------------------------------------------------------------------

//    Maps the value from 0%-100% to 0-1. Thread-safe operation.
        float noiseMix = noiseParam->get() / 100.0f;
    
//    Squaring the parameter to become logarithmic. Human hearing.
        noiseMix *= noiseMix;
    
//    The audio rendering happens in class Synth. Times 0.06 sets the
//    maximum noise level roughly to -24dB for flavoring the sound.
        synth.noiseMix = noiseMix * 0.06f;
//    --------------------------------------------------------------------------

//    Envelope
//    --------------------------------------------------------------------------

//    Actual sample rate
    float sampleRate = float(getSampleRate());
    
//    First version
    
//    EnvDecay param measures in 0-100%. Conversion to 0-1.
//    This example defies the decay time of 100% at 5 sec.
//    float decayTime = envDecayParam->get() / 100.0f * 5.0f;
//    In the digital audio domain time is measured in samples.
//    For instance, a sampleRate of 44,1kHz produces in 1 sec
//    44100 samples. The decayTime is a multiplier for time
//    in samples.
//    float decaySamples = sampleRate * decayTime;
//    The multiplier induces the function exp() to
//    decrease from 1 ( Level = 100%) to
//    0 (SILENCE) in a certain amount of time.
//    Due to the human hearing the lower
//    threshold is expressed in dB.
//    synth.envDecay = std::exp(std::log(SILENCE) / decaySamples);
    
//    Actual second version
    
//    Scale time values in sec to the corresponding
//    time values in samples.
    float inverseSampleRate = 1.0f / sampleRate;
    
    
    synth.envAttack =
        std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * envAttackParam->get()));
    
    synth.envDecay =
        std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * envDecayParam->get()));
    
    synth.envSustain = envSustainParam->get() / 100.0f;
    
    float envRelease = envReleaseParam->get();
    
    if (envRelease < 1.0f) {
        synth.envRelease = 0.75f; // extra fast release
    }
    else {
        synth.envRelease = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * envRelease));
    }
//    --------------------------------------------------------------------------

//    Oscillator Mix
//    --------------------------------------------------------------------------

//    Maps the value from 0%-100% to 0-1. Thread-safe operation.
    synth.oscMix = oscMixParam->get() / 100.0f;
    
//    Detuning the second oscillator
//    by cents and half steps.
    float semi = oscTuneParam->get();
//    A cent is 1/100th of a semitone
    float cent = oscFineParam->get();
    
//    Same as
//    float freq = 440.0f * std::exp2(float(note - 69) / 12.0f)
//    to calculate the pitch of any note. 2^1/12 equals 1.059463094359f.
//    semi + cent/100 gets the total amount of semitones. Prefer
//    multiplying versions due to processing. The negative
//    algebraic sign results in the vice versa tuning of the pitch.
    synth.detune = std::pow(1.059463094359f, -semi - 0.01f * cent);
//    --------------------------------------------------------------------------
    
//    Overall Tuning
//    --------------------------------------------------------------------------
    
//    2 octaves up or down
    float octave = octaveParam->get();
//    –100 to +100 cents
    float tuning = tuningParam->get();
    float tuneInSemi = -36.3763f - 12.0f * octave - tuning / 100.0f;
    
//    12 semitones in an ocatve and 100 cents
//    in a semitone
//    synth.tune = octave * 12.0f + tuning * .01f;
    
//    New method:
//    It combines the sample rate, the tuning in octaves and cents,
//    and the reference pitch of 8.1758 Hz, into a single number.
    synth.tune = sampleRate * std::exp(0.05776226505f * tuneInSemi);
//    --------------------------------------------------------------------------
    
//    Polyphony
//    --------------------------------------------------------------------------
//    Choice of polyphony (Synth::MAX_VOICES) or not (1)
    synth.numVoices = (polyModeParam->getIndex() == 0) ? 1 : Synth::MAX_VOICES;
//    --------------------------------------------------------------------------
//    Volume
//    --------------------------------------------------------------------------
//    Volume trim(see Filter)
    

    
//    Total volume
//    Does a linear interpolation over 0.05 seconds from the
//    current value to this new target.
    synth.outputLevelSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputLevelParam->get()));

//    --------------------------------------------------------------------------
//    Modulation (Sensitivity, LFO, Vibrato, PWM, Glide)
//    --------------------------------------------------------------------------
            /*    Velocity sensitivity  */
    float filterVelocity = filterVelocityParam->get();
//    Changes the dynamic range of the sound to 0 dB
    if (filterVelocity < -90.0f) {
//        Sets ignoreVelocity to true when the parameter says OFF. 
        synth.velocitySensitivity = 0.0f;
        synth.ignoreVelocity = true;
    } else {
        synth.velocitySensitivity = 0.0005f * filterVelocity;
        synth.ignoreVelocity = false;
    }
    
            /*    LFO   */
//    The sample rate for the LFO is 32 times lower than the audio’s sample rate
    const float inverseUpdateRate = inverseSampleRate * synth.LFO_MAX;
//    Sets the frequency.
//    Maps 0 – 1 parameter value to 0.0183 Hz – 20.086 Hz, or roughly 0.02 Hz to 20 Hz.
    float lfoRate = std::exp(7.0f * lfoRateParam->get() - 4.0f);
//    inc = freq / sampleRate
    synth.lfoInc = lfoRate * inverseUpdateRate * float(TWO_PI);
    
            /*    Vibrato/PWM  */
//    The Vibrato parameter goes between –100% and +100%.
    float vibrato = vibratoParam->get() / 200.0f;
//    Vibrato is a parabolic curve from 0% (0) to 100% (0.05).
    synth.vibrato = .2f * vibrato * vibrato;
//    Sets the new pwmDepth variable to the value of synth.vibrato.
    synth.pwmDepth = synth.vibrato;
    if (vibrato < 0.0f) {
//        If vibrato is negative,
//        PWM mode should be used instead of the vibrato effect.
//        Turning off regular vibrato:
        synth.vibrato = 0.0f;
    }
    
            /*   Glide   */
    
//    0=off, 1=legato-style, 2=always
    synth.glideMode = glideModeParam->getIndex();
    
//    glide speed as a percentage. glideRate as
//    coefficient for one-pole filter.
    float glideRate = glideRateParam->get();
    if (glideRate < 2.0f) {
        synth.glideRate = 1.0f;
    } else {
        synth.glideRate = 1.0f - std::exp(-inverseUpdateRate * std::exp(6.0f - 0.07f * glideRate));
    }
    
//    Range: 36 semitones to +36 semitones.
    synth.glideBend = glideBendParam->get();
    
//    --------------------------------------------------------------------------
//    Filter
//    --------------------------------------------------------------------------
//    Converting the percentage from 0 – 100% into the range –1.5 to 6.5.
//    The lower, the more sound get filtered.
    synth.filterKeyTracking = 0.08f * filterFreqParam->get() - 1.5f;
    
//    It creates an exponential curve that starts at
//    filterQ = 1 and goes up to filterQ = 20.
    float filterReso = filterResoParam->get() / 100.0f;
    synth.filterQ = std::exp(3.0f * filterReso);
    //    Velocity used to be (velocity / 127) * 0.5
    //    -> 0.5/127 = 0.00394. It's also the value for
    //    oscMix and noiseMix equals 0. Maximum amplitude of a
    //    single voice (-6dB).
        
    //    oscMix 100%: vT = 0.00384
    //    -> 20 × log10(0.00264/0.00384) = −3.25dB. Two waveforms with
    //    the same amplitude. Therefore, a decrease by -3dB.
        
    //    oscMix and noiseMix 100%: vt = 0.00084 -> -13dB
    //    Noise makes the perceived sound louder due to more freqs.
        synth.volumeTrim =
        0.0008f * (3.2f - synth.oscMix - 25.0f * synth.noiseMix) * (1.5f - 0.5f * filterReso);
    
    float filterLFO = filterLFOParam->get() / 100.0f;
//    Possible values go between 0 and 2.5.
    synth.filterLFODepth = 2.5f * filterLFO * filterLFO;
    
//    Filter envelopes
    synth.filterAttack = std::exp(-inverseUpdateRate *
                                    std::exp(5.5f - 0.075f * filterAttackParam->get()));
    synth.filterDecay = std::exp(-inverseUpdateRate *
                                    std::exp(5.5f - 0.075f * filterDecayParam->get()));
    float filterSustain = filterSustainParam->get() / 100.0f;
    synth.filterSustain = filterSustain * filterSustain;
    synth.filterRelease = std::exp(-inverseUpdateRate *
                                    std::exp(5.5f - 0.075f * filterReleaseParam->get()));
//    Intensity of the envelope modulation.
//    Range from –100% to 100% mapping to –6.0 and +6.0.
    synth.filterEnvDepth = 0.06f * filterEnvParam->get();

}
//==============================================================================

void JX11AudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int bufferOffset = 0;
    
    for (const auto metadata : midiMessages) {
//        Render the audio that happens before this event (if any).
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }
        
//        Handle the event. Ignore MIDI messages such as sysex.
        if (metadata.numBytes <= 3) {
            juce::MidiMessage midiMessage = metadata.getMessage();
            uint8_t data1 = (midiMessage.getRawDataSize() >= 2) ? midiMessage.getRawData()[1] : 0;
            uint8_t data2 = (midiMessage.getRawDataSize() == 3) ? midiMessage.getRawData()[2] : 0;
            handleMIDI(midiMessage.getRawData()[0], data1, data2);
            
        }
    }
    
//    Render the audio after the last MIDI event. If there were no
//    MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment, bufferOffset);
    }
    
    midiMessages.clear();
}

void JX11AudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
//    Checks if midiLearn is true and for
//    command Control Change.
    if (midiLearn && ((data0 & 0xF0) == 0xB0)) {
        DBG("learned a MIDI CC");
//        Using the CC number (in data1) from
//        the message (data1).
        midiLearnCC = data1;
        midiLearn = false;
        return;
    }
    
//    Program Change
//    Checks if the command is 0xC,
//    which is Program Change.
    if ((data0 & 0xF0) == 0xC0) {
//        data1 byte contains the number of the program.
        if (data1 < presets.size()) {
            setCurrentProgram(data1);
        } 
    }
    
//    Outputs a sound by pressing MIDI keyboard
    synth.midiMessage(data0, data1, data2);
}

void JX11AudioProcessor::render(
                                juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset) 
{
//    Array of 2 float pointers for stereo
    float* outputBuffers[2] = {nullptr, nullptr};
    
//    Get a pointer to the audio data inside (getWritePointer)
//    AudioBuffer is split up due to MIDI time stamps
//    bufferOffset is the actual start of the buffer
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    
    if (getTotalNumInputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    
    synth.render(outputBuffers, sampleCount);
}

//==============================================================================
bool JX11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JX11AudioProcessor::createEditor()
{
    /* Automatic editor.
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 700);
    return editor;
     */
    
//    Customized editor.
//    Instantiating JX11AudioProcessorEditor and passing
//    it as a reference to the JX11AudioProcessor.
    return new JX11AudioProcessorEditor(*this);
}

//==============================================================================
void JX11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
//    Creates a new XML element with a tag name
//    to store the state.
    auto xml = std::make_unique<juce::XmlElement>(pluginTag);
    
//    The current state of the parameters (stored in apvts) is copied
//    and converted into an XML format.
    std::unique_ptr<juce::XmlElement> parametersXML(apvts.copyState().createXml());
//    The parameters XML is added as a child element
//    to the main XML element.
    xml->addChildElement(parametersXML.release());
    
//    Creates a new XML element with a tag name
//    to store the MIDI CC Number.
    auto extraXML = std::make_unique<juce::XmlElement>(extraTag);
//    Gives it an attribute named midiCC with the value of the
//    MIDI CC code that the user has learned (or the default 0x47).
    extraXML->setAttribute(midiCCAttribute, midiLearnCC);
    xml->addChildElement(extraXML.release());

    
//    The entire XML structure is converted into a
//    binary format and stored in destData
//    (a juce::MemoryBlock).
    copyXmlToBinary(*xml, destData);
    
//    Outputs the XML as a string to the debug console.
    DBG(xml->toString());
    
}

void JX11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
//    Parse binary data into an xml document.
//    Converts the binary data back into an XML structure.
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
//    Verifying that xml contains the parameter tag
    if (xml.get() != nullptr && xml->hasTagName(pluginTag)) {
//        Retrieves the child XML element that contains the parameters.
        if (auto* parametersXML = xml->getChildByName(apvts.state.getType())) {
//          Replaces the current parameter state with the state
//            defined in the XML. This is done in a thread-safe manner.
            apvts.replaceState(juce::ValueTree::fromXml(*parametersXML));
//            Signal processBlock() to call update() again.
//            Making sure the the values are up-to-date.
            parametersChanged.store(true);
        }
        
        if (auto* extraXML = xml->getChildByName(extraTag)) {
//            This looks for the <EXTRA midiCC="..."/> element,
//            reads the attribute..
            int midiCC = extraXML->getIntAttribute(midiCCAttribute);
//            ...and puts it into midiLearnCC.
            midiLearnCC = static_cast<uint8_t>(midiCC);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11AudioProcessor();
}
