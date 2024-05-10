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
    Oscillator osc;
    
//    Turning BLIT into sawtooth wave
    float saw;
    
//    On initialization of the plug-in
//    reset note and velocity
    void reset() {
        note = 0;
        osc.reset();
        saw = 0.0f;
    }
    
//    Get the next sample from the oscillator
    float render(float input) {
        float sample = osc.nextSample();
//        .997f acts like a low-pass filter preventin an offset
        saw = saw * .997f + sample;
        
//        Noise added to the oscillator
        float output = saw + input;
        
        float envelope = env.nextValue();

//        Osc value with noise multiplied
//         by the current envelope level.
        
        return output * envelope;
    }
};
