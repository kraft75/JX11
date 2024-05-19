/*
  ==============================================================================

    Synth.cpp
    Created: 1 May 2024 9:50:18pm
    Author:  MacJay

  ==============================================================================
*/

#include "Synth.h"

static const float ANALOG = 0.002f;

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
//    reset Voice and Generator instance.
//    Defy start value for Pitch bend
    for (int i = 0; i < MAX_VOICES; ++i) {
        voices[i].reset();
    }
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
    for (int i = 0; i < MAX_VOICES; ++i) {
        Voice& voice = voices[i];
//        Check if key is pressed.
        if (voice.env.isActive()) {
            voice.osc1.period = voice.period * pitchBend;
            voice.osc2.period = voice.osc1.period * detune;
        }
    }
    
//    Loop thru the samples. If there were MIDI messages
//    this will be less than the total number of samples in the block.
    for (int sample = 0; sample < sampleCount; ++sample) {
//        Next output from noise generator multiplied by
//        the parameter noise (noiseMix).
        float noise = noiseGen.nextValue() * noiseMix;
        
//        Output for the left and right speaker
        float outputLeft = 0.0f;
        float outputRight = 0.0f;
        
        for (int i = 0; i < MAX_VOICES; ++i) {
            Voice& voice = voices[i];
//        Check if key is pressed.
            if (voice.env.isActive()) {
//            Audio data with added noise.
                float output = voice.render(noise);
//            Sample is mixed into the left/right
//            channel output using panLeft/panRight
//            amount.
                outputLeft += output * voice.panLeft;
                outputRight += output * voice.panRight;
            }
        }

        
//        Write output values into the respective audio buffer
        if (outputBufferRight != nullptr) {
            outputBufferLeft[sample] = outputLeft;
            outputBufferRight[sample] = outputRight;
        } else {
//            Case mono: left and right values need to be combined
//            into a mono sample. No stereo.
            outputBufferLeft[sample] = (outputLeft + outputRight) * .5f;
        }
    }
    
    for (int i = 0; i < MAX_VOICES; ++i) {
        Voice& voice = voices[i];
        if (!voice.env.isActive()) {
            voice.env.reset();
        }
    }
    
    
//        Mutes the audio for values beyond -2.0f and 2.0f
    earProtect.protectYourEars(outputBufferLeft, sampleCount);
    earProtect.protectYourEars(outputBufferRight, sampleCount);
}
//==============================================================================

void Synth::startVoice(int v, int note, int velocity)
{
    float period = calcPeriod(v, note);
    Voice& voice = voices[v];
    
//    Settings of the envelope attributes
//    ------------------------------------------------------------------
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
//    Setting private members level and target
    env.attack();
//    ------------------------------------------------------------------
//    Oscillator settings
//    ------------------------------------------------------------------

    voice.osc1.amplitude = (velocity / 127.0f) * 0.5f;
//    No reset. Emulating the behavior of an analogue
//    hardware.
//    voice.osc1.reset();
    voice.osc2.amplitude = voice.osc1.amplitude * oscMix;
//    voice.osc2.reset();
    
    voice.period = period;
    voice.note = note;
    voice.updatePanning();
}

void Synth::noteON(int note, int velocity)
{

//    0 is mono mode.
    int v = 0;
    
//    polyphonic
    if (numVoices > 1) {
        v = findFreeVoice();
    }
    
    startVoice(v, note, velocity);
}

int Synth::findFreeVoice() const 
{
    int v = 0;
// Random threshold for initialization
    float l = 100.0f;
    
    for (int i = 0; i < MAX_VOICES; ++i) {
//        Find the voice with the lowest envelope level.
//        Ignores voices in the attack stage.
        if (voices[i].env.level < l && !voices[i].env.isInAttack()) {
            l = voices[i].env.level;
            v = i;
        }
    }
    return v;
}

void Synth::noteOff(int note)
{
    for (int i = 0; i < MAX_VOICES; ++i) {
        Voice& voice = voices[i];
//    Checking all voices to see which is playing
//    the actual note.
        if (voice.note == note) {
//            Begin the release phase in the envelope.
            voice.release();
//            Note Off event was received for this voice.
//            Voice will keep playing until the note faded out.
            voice.note = 0;
        }
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
float Synth::calcPeriod(int v, int note) const
{
//    Period in samples instead of Hz. p. 225
//    from freq to period.
//    replace pow(x, y) with exp(y * log(x)). Exp is faster.
//    ANALOG and v emulates random detuning. I.e, temperature.
    float period = tune * std::exp(-0.05776226505f * (float(note) + ANALOG * float(v)));
    
//    BLIT-based oscillator may not work reliably if
//    the period is too small.
//    A prevention is to guarantee a period with
//    with at least 6 samples.
    while (period < .6f || (period * detune) < .6f) {
        period += period;
    }
    
    return period;
}

