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

class Synth {
    float sampleRate;
    
    void noteON(int, int);
    void noteOff(int);
    
//    Multiple voices for each note playing.
//    At the momemt monophonic
    Voice voice;
    
public:
    Synth();
    
//    Called before the host starts playing audio.
//    Comparable to PrepareToPlay()
    void allocateResources(double sampleRate, int samplesPerBlock);
    
//    Called after the audio finished. Comparable to
//    releaseResources()
    void deallocateResources();
    
//    reset state
    void reset();
    
//    render current block of audio
    void render(float** outputBuffers, int sampleCount);
    
//    Handling MIDI messages
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
};
