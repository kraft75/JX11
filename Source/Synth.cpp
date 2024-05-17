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
    pitchBend = 1.0f;
}
//==============================================================================

void Synth::render(float** outputBuffers, int sampleCount) 
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
//    New detuned value by the oscillator
//    while the sound is playing.
//    Updating the period in processBlock
    voice.osc1.period = voice.period * pitchBend;
    voice.osc2.period = voice.osc1.period * detune;
    
//    Loop thru the samples. If there were MIDI messages
//    this will be less than the total number of samples in the block.
    for (int sample = 0; sample < sampleCount; ++sample) {
//        Next output from noise generator multiplied by
//        the parameter noise (noiseMix).
        float noise = noiseGen.nextValue() * noiseMix;
        
        float output = 0.0f;
//        Check if key is pressed.
        if (voice.env.isActive()) {
//            Audio data with added noise.
            output = voice.render(noise);
        }
        
//        Write output values into the audio buffer
        outputBufferLeft[sample] = output;
        if (outputBufferRight != nullptr) {
            outputBufferRight[sample] = output;
        }
        
        if (!voice.env.isActive()) {
            voice.env.reset();
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
    
    /*  P. 115
        1: (note − 69) determines the number of semitones this note is up or down from
        the A with note number 69.
     
        2:  calculate 2^(note−69)/12 to get the multiplier.
     
        3: Apply this multiplier to frequency 440Hz to get the pitch of the note.
     
        4: Add the overall tuning to 1.
    */
//    float freq = 440.0f * std::exp2((float(note - 69) + tune) / 12.0f);
    
//    Settings of the oscillators attributes
//    ------------------------------------------------------------------
//    Activating the first oscillator
//    voice.period = sampleRate / freq;
    float period = calcPeriod(note);
    voice.period = period;
    voice.osc1.amplitude = (velocity / 127.0f) * 0.5f;
//    No reset emulates the behavior of an analogue
//    hardware
//    voice.osc1.reset();
    
//    Activating the second oscillator
    voice.osc2.amplitude = voice.osc1.amplitude * oscMix;
//    voice.osc2.reset();
//    ------------------------------------------------------------------

//    Settings of the envelope attributes
//    ------------------------------------------------------------------
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
//    Setting private members level and target
    env.attack();
}

void Synth::noteOff(int note)
{
//    Only if the released key is for the same note
    if (voice.note == note) {
        voice.release();
    }
}
//==============================================================================
void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
//    Consider only the four highest bits (command)
    switch (data0& 0xF0) {
//            Note off
        case 0x80: {
            noteOff(data1& 0x7F);
            break;
        }
//            Note on
        case 0x90: {
            uint8_t note = data1& 0x7F;
            uint8_t velo = data2& 0x7F;
            
            if (velo > 0) {
                noteON(note, velo);
            } else {
                noteOff(note);
            }
            break;
        }
//            Pitch bend
        case 0xE0: {
//            In Pitch bend data1 and data2 are combined.
//            It results in a 14-bit number in a a range of
//            0 – 16383 if unsigned or –8192 to 8191 signed.
//            (data1 + 128 * data2 - 8192)
            
//            Range to make the pitch rise  in 2 semitones
//            2^2/12 = 1.12. Lower 2 semitones is 0.89.
//            By changing the period it is inverse.
//            Mapping: –8192 to 8191 into 1.12 – 0.89.
//            Origin formula: 2^2*(data/8192)/12
            pitchBend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            break;
        }
        
    }
}
//==============================================================================
float Synth::calcPeriod(int note) const     
{
//    Period in samples instead of Hz. p. 225
//    from freq to period.
//    replace pow(x, y) with exp(y * log(x)). Exp is faster.
    float period = tune * std::exp(-0.05776226505f * float(note));
    
//    BLIT-based oscillator may not work reliably if
//    the period is too small.
//    A prevention is to guarantee a period with
//    with at least 6 samples.
    while (period < .6f || (period * detune) < .6f) {
        period += period;
    }
    
    return period;
}

