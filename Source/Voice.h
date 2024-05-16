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

struct Voice {
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
    }
    
//    Time to release the note
    void release()
    {
        env.release();
    }
    
//    Get the next sample from the oscillator
    float render(float input) {
        float sample1 = osc1.nextSample();
        float sample2 = osc2.nextSample();
//        .997f acts like a low-pass filter preventin an offset
//        Output from second osc is subtracted from the first.
        saw = saw * .997f + sample1 - sample2;
        
//        Noise added to the oscillator
        float output = saw + input;
        
        float envelope = env.nextValue();

//        Osc value with noise multiplied
//         by the current envelope level.
        return output * envelope;
    }
};
