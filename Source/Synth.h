/*
  ==============================================================================

    Synth.h
    Created: 1 May 2024 9:50:18pm
    Author:  MacJay
 
    This class makes the actual sound
 
    P. 115
    1: (note − 69) determines the number of semitones this note is up or down from
     the A with note number 69.
  
    2:  calculate 2^(note−69)/12 to get the multiplier.
  
    3: Apply this multiplier to frequency 440Hz to get the pitch of the note.
  
    4: Add the overall tuning to 1.
    float freq = 440.0f * std::exp2((float(note - 69) + tune) / 12.0f);


  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Voice.h"
#include "NoiseGenerator.h"
#include "Utils.h"


class Synth {
public:
//    ------------------------------------------------------------------
//    Public methods
//    ------------------------------------------------------------------

    Synth();
    
//    Called before the host starts playing audio.
//    Comparable to PrepareToPlay()
    void allocateResources(double sampleRate, int samplesPerBlock);
    
//    Called after the audio finished. Comparable to
//    releaseResources().
    void deallocateResources();
    
//    reset state
    void reset();
    
//    render current block of audio. Making sound.
    void render(float** outputBuffers, int sampleCount);
    
//    Handling MIDI messages
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
    
//    Renders the period for a given MIDI note number.
    float calcPeriod(int, int) const;
    
//    Setting up the voice with envelope and oscillators
    void startVoice(int v, int note, int velocity);
    
//    Find free voices for chords.
//    When all voices are in use, the synthesizer needs to
//    perform voice stealing. It has to decide which voice
//    should stop playing its old note and start playing the new one.
    int findFreeVoice() const;
    
//    Processes various MIDI CC commands
    void controlChange(uint8_t data1, uint8_t data2);
    
//    ------------------------------------------------------------------
//    Public member variables
//    ------------------------------------------------------------------

//    Parameter noise.
    float noiseMix;
    
//    Parameter envelope phases.
    float envAttack;
    float envDecay;
    float envSustain;
    float envRelease;
    
//    Parameter which mixes two oscillators.
//    The amplitude of osc2 is a percentage
//    of osc1’s amplitude.
    float oscMix;
//    Shifts the period of the second oscillator
    float detune;
    
//    Parameter for overall tuning
//    Total amount of tuning measured in semitones
    float tune;
    
//    Pitch bend
//    Controlled by a MIDI message
    float pitchBend;
    
//    Maximum amount of polyphony
    static constexpr int MAX_VOICES = 8;
//    Choice between 1 or MAX_VOICES
    int numVoices;
    
//    Volume control preventig boosts
//    in a single voice.
    float volumeTrim;
    
//    Smoother for total output level
    juce::LinearSmoothedValue<float> outputLevelSmoother;
    
    
private:
    float sampleRate;
    
    void noteON(int, int);
    void noteOff(int);
    
//    Multiple voices for each note playing.
    std::array<Voice, MAX_VOICES>  voices;
    
//    Mixing noise to the oscillator
    NoiseGenerator noiseGen;
    
//    Mutes the audio for ear protection
    Utils earProtect;
    
//    Sustain pedal
    bool sustainPedalPressed;
    
};
