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

struct Voice {
//    Register the note and velocity
//    of the most recently pressed key
    int note;
    int velocity;
    
//    On initialization of the plug-in
//    reset note and velocity
    void reset() {
        note = 0;
        velocity = 0;
    }
};
