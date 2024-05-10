/*
  ==============================================================================

    Envelope.h
    Created: 9 May 2024 8:20:20pm
    Author:  MacJay

 
    The level variable holds the current envelope level.
    It will start at 1.0 or 100%.
    Every time nextValue() is called,
    level is multiplied by the value multiplier
    ( e.g 0.9999 for 2 sec fade out).
    It provides a smooth fade out in the release phase
    of the note.
 
    Calculate the multiplier
  ==============================================================================
*/

#pragma once

//Threshold. Lower limit for silence.
const float SILENCE = 0.0001f;

class Envelope {
public:
    float nextValue()
    {
//        Fade out.
        level *= multiplier;
        
        return level;
    }
    
    float level;
    float multiplier;
};
