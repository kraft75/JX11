/*
  ==============================================================================

    Voice.h
    Created: 1 May 2024 9:49:48pm
    Author:  MacJay

    Voice is an object that manages a single playing note. In monophonic mode
    there is only one note playing at a time. On the contrary, the polyphonic
    mode has a Voice instance for every played note (e.g chords).
 
    The job of Voice is to produce the next output sample for a given note.
  ==============================================================================
*/

#pragma once

#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"

struct Voice {
//    Sets the filter’s cutoff based
//    on the pitch of the note
    float cutoff;
    
//    Register the note and velocity
//    of the most recently pressed key
    int note;
    
//    Envelope object
    Envelope env;
    
//    Sine oscillator
    Oscillator osc1;
    Oscillator osc2;
    
//    Turning BLIT into sawtooth wave
    float saw;
    
//    New detuned value by the oscillator
//    voice.osc2.period
//    Updating the period in processBlock
    float period;
    
//    Making the synth stereophonic.
//    Lower notes to the left speaker.
//    Vice versa higher notes.
    float panLeft, panRight;
    
//    SVF-filter
    Filter filter;
    
//    Target value for glide.
    float target;
    
//    The logic for performing the glide
//    happens in the Voice object.
//    A copy of Synth’s glideRate variable
//    so that the voice will be able to access it.
    float glideRate;
    
//    Saves the value of filterKeyTracking
    float filterMod;
    
//    Resonance value
    float filterQ;
    
//    Adding filter envelopes.
    Envelope filterEnv;
    float filterEnvDepth;
    
//    Amount of pitch bend in the filter modulation.
    float pitchBend;
    
//    On initialization of the plug-in
//    reset note and velocity
    void reset() {
        note = 0;
//        Reseting osc objects is optional.
//        Mimicking an analogue hardware
//        synthesizer should exclude a
//        reset.
        osc1.reset();
        osc2.reset();
        saw = 0.0f;
        env.reset();
        panLeft = 0.707f;
        panRight = 0.707f;
        filter.reset();
        filterEnv.reset();
    }
    
//    Time to release the note.
    void release()
    {
        env.release();
        filterEnv.release();
    }
    

//    Updating filter coefficients.
    void updateLFO()
    {
//       This is the one-pole filter formula that creates an
//       exponential transition curve between the two pitches.
        period += glideRate * (target - period);
        
//        Gets the filter envelope’s current level (0 - 1).
        float fenv = filterEnv.nextValue();
        
//        A multiplier that makes cutoff higher or lower.
//        From exp(-1.5) = 0.22× to exp(6.5) = 665×.
//        Freq = 18,75% equals the cutoff frequency.
//        Freq = 33% is on the pitch of the note.
//        Freq = 90%. No influence of the filter.
//        Multiply by filter env.
        float modulatedCutoff = cutoff * std::exp(filterMod + filterEnvDepth * fenv) / pitchBend;
//        Limit the cutoff to a reasonable range between
//        30 Hz and 20000 Hz.
        modulatedCutoff = std::clamp(modulatedCutoff, 30.0f, 20000.0f);
//        Updating coefficients.
        filter.updateCoefficients(modulatedCutoff, filterQ);
    }
    
//    Get the next sample from the oscillator
    float render(float input) 
    {
//        Renders both oscillators
        float sample1 = osc1.nextSample();
        float sample2 = osc2.nextSample();
        
//        .997f acts like a low-pass filter preventin an offset
//        Output from second osc is subtracted from the first.
        saw = saw * .997f + sample1 - sample2;
        
//        Noise added to the oscillator
        float output = saw + input;
        
//        Filters the output
        output = filter.render(output);
        
//        Envelope curve
        float envelope = env.nextValue();

//        Osc value with noise multiplied
//         by the current envelope level.
        return output * envelope;
    }
    
//    Giving panLeft/panRight appropriate values
//    Constant power panning makes the loudness always
//    appear the same. No matter at which position.
    void updatePanning()
    {
//        Converts a MIDI Note into a panning value from –1 to +1.
        float panning = std::clamp((note - 60.0f) / 24.0f, -1.0f, 1.0f);
//        A panning value of –1 adjusts the sound fully to the left
        panLeft = std::sin(PI_OVER_4 * (1.0f - panning));
//        Vice versa
        panRight = std::sin(PI_OVER_4 * (1.0f + panning));
    }
};
