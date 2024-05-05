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
void Synth::allocateResources(double sampleRate_, int samplesPerBlock)
{
    sampleRate = static_cast<float>(sampleRate_);
}
//==============================================================================

void Synth::deallocateResources() 
{
// do nothing
}
//==============================================================================

void Synth::reset()
{
//    On initialization of the plug-in
//       reset Voice and Generator instance
    voice.reset();
    noiseGen.reset();
}
//==============================================================================

void Synth::render(float** outputBuffers, int sampleCount) 
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
//    Loop thru the samples. If there were MIDI messages
//    this will be less than the total number of samples in the block.
    for (int sample = 0; sample < sampleCount; ++sample) {
//        Next output from noise generator
//        float noise = noiseGen.nextValue();
        
        float output = 0.0f;
//        Check if key is pressed
        if (voice.note > 0) {
//            Audio data
            output = voice.render();
        }
        
//        Write output values into the audio buffer
        outputBufferLeft[sample] = output;
        if (outputBufferRight != nullptr) {
            outputBufferRight[sample] = output;
        }
        
//        Mutes the audio for values beyond -2.0f and 2.0f
        earProtect.protectYourEars(outputBufferLeft, sampleCount);
        earProtect.protectYourEars(outputBufferRight, sampleCount);

    }
}
//==============================================================================

void Synth::noteON(int note, int velocity)
{
//    Register recently played note and velocity
    voice.note = note;
    
    voice.osc.amplitude = (velocity / 127.0f) * 0.5f;
    
    /*  P. 115
        1: (note − 69) determines the number of semitones this note is up or down from
        the A with note number 69.
     
        2:  calculate 2^(note−69)/12 to get the multiplier.
     
        3: Apply this multiplier to frequency 440Hz to get the pitch of the note.
        
        
     */
    float freq = 440.0f * std::exp2(float(note - 69) / 12.0f);
    
    voice.osc.period = sampleRate / freq;
    voice.osc.reset();
}

void Synth::noteOff(int note)
{
//    Only if the released key is for the same note
    if (voice.note == note) {
        voice.note = 0;
    }
}
//==============================================================================
void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
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

