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
    // do nothing yet
}
//==============================================================================

void Synth::render(float** outputBuffers, int sampleCount) {
    // do nothing yet
}
//==============================================================================

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2) {
    // do nothing yet
}

