/*
  ==============================================================================

    Synth.cpp
    Created: 1 May 2024 9:50:18pm
    Author:  MacJay

  ==============================================================================
*/

#include "Synth.h"

Synth::Synth() : sampleRate(44100.0f) {}
//==============================================================================
void Synth::allocateResources(double sampleRate_, int samplesPerBlock) {
    sampleRate = static_cast<float>(sampleRate_);
}
//==============================================================================

void Synth::deallocateResources() {
// do nothing
}
//==============================================================================

void Synth::reset() {
//    On initialization of the plug-in
//       reset Voice instance
    voice.reset();
}
//==============================================================================

void Synth::render(float** outputBuffers, int sampleCount) {
    // do nothing yet
}
//==============================================================================

void Synth::noteON(int note, int velocity) {
//    Register recently played note and velocity
    voice.note = note;
    voice.velocity = velocity;
}

void Synth::noteOff(int note) {
//    Only if the released key is for the same note
    if (voice.note == note) {
        voice.note = 0;
        voice.velocity = 0;
    }
}
//==============================================================================
void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2) {
//    Consider only the four highest bits (command)
    switch (data0& 0xF0) {
//            Note off
        case 0x80:
            noteOff(data1& 0x7F);
            break;
//            Note on
        case 0x90:
            uint8_t note = data1& 0x7F;
            uint8_t velo = data2& 0x7F;
            
            if (velo > 0) {
                noteON(note, velo);
            } else {
                noteOff(note);
            }
            break;
            
        
    }
}

