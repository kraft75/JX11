/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JX11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void JX11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JX11AudioProcessor::getProgramName (int index)
{
    return {};
}

void JX11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
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
//    Map the value from 0%-100% to 0-1. Thread-safe operation.
        float noiseMix = noiseParam->get() / 100.0f;
//    Squaring the parameter to become logarithmic. Human hearing.
        noiseMix *= noiseMix;
//    The audio rendering happens in class Synth. Times 0.06 sets the
//    maximum noise level roughly to -24dB for flavoring the sound.
        synth.noiseMix = noiseMix * 0.06f;
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
    /* Terminal output test
    char s[16];
    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    DBG(s);
     */
    
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
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 700);
    return editor;
}

//==============================================================================
void JX11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
    
    DBG(apvts.copyState().toXmlString());
}

void JX11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<<#class Tp#>>(<#unique_ptr<Tp, Dp> &&u#>)
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11AudioProcessor();
}
