/*
  ==============================================================================

    Envelope.h
    Created: 9 May 2024 8:20:20pm
    Author:  MacJay

 
    1.The level variable holds the current envelope level.
    It will start at 1.0 or 100%.
    Every time nextValue() is called,
    level is multiplied by the value multiplier
    ( e.g 0.9999 for 2 sec fade out).
    It provides a smooth fade out in the release phase
    of the note.
 
    2.Calculate the multiplier (turn knob), instead of a fixed value
 
    3.Fade out at the sustain level
 
    4.Calculate the release time
 
    5. Set the attack
  ==============================================================================
*/

#pragma once

//Threshold. Lower limit for silence.
const float SILENCE = 0.0001f;

class Envelope {
//    Value of sustain level
        float target;
//    Fade out
    float multiplier;
public:
    float nextValue()
    {
//        Fade out.
//        2.level *= multiplier;(Fade out at SILENCE)
//        3. Formula:
//        level = (1 - multiplier) * target + multiplier * level
        level = multiplier * (level - target) + target;
        
//        5. Set the target of the attack to 2.0f in
//        order to make the curve steeper. Giving the
//        attack more punch.
//        Transition to the sustainLevel by setting the
//        target to the sustain level and using the decay
//        multiplier.
        if (level + target > 3.0f) {
            multiplier = decayMultiplier;
            target = sustainLevel;
        }
        return level;
    }
    
    void reset()
    {
        level = 0.0f;
        target = 0.0f;
        multiplier = 0.0f;
    }
    
    bool isInAttack() 
    {
        return target >= 2.0f;
    }
    
//    Setting target to 2.0f and activating
//    the attackMultiplier. level guarantees
//    that the initial attack level is above
//    SILENCE, which enables new notes to be
//    played.
    void attack()
    {
//        += assignment supports
//        legato-style playing.
        level += SILENCE + SILENCE;
        target = 2.0;
        multiplier = attackMultiplier;
    }
    
//    Only renders the voice if
//    the envelope is still active.
//    Otherwise, it stops the voice
//    playing when it breached the target.
//    Sets target to 0.
    bool isActive() const
    {
        return level > SILENCE;
    }
    
//    4.Sets the multiplier for
//    the release time.
    void release()
    {
        target = 0.0f;
        multiplier = releaseMultiplier;
    }
    
//    Start value = 1
    float level;
//    Degree of fading out
    float attackMultiplier;
    float decayMultiplier;
    float sustainLevel;
    float releaseMultiplier;
    

};
