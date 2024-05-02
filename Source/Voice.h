/*
  ==============================================================================

    Voice.h
    Created: 1 May 2024 9:49:48pm
    Author:  MacJay

    Voice is an object that manages a single playing note. In monophonic mode
    there is only one note playing at a time. On the contrary, the polyphonic
    mode has a Voice instance for every played note.
 
    The job of Voice is to produce the next output sample for a given note.
  ==============================================================================
*/

#pragma once

struct Voice {
    void reset() {
        
    }
};
