/*
  ==============================================================================

    Synth.h
    Created: 1 May 2024 9:50:18pm
    Author:  MacJay
 
    This class makes the actual sound

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Voice.h"
#include "NoiseGenerator.h"
#include "Utils.h"


class Synth {
    float sampleRate;
    
    void noteON(int, int);
    void noteOff(int);
    
//    Multiple voices for each note playing.
//    At the momemt monophonic
    Voice voice;
    
//    Mixing noise to the oscillator
    NoiseGenerator noiseGen;
    
//    Mutes the audio for ear protection
    Utils earProtect;
    
public:
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
    float calcPeriod(int) const;
    
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
};
