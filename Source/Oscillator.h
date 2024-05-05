/*
  ==============================================================================

    Oscillator.h
    Created: 2 May 2024 6:53:17pm
    Author:  MacJay

    Bandlimited sawtooth oscillator created with the
    BLIT (bandlimited impulse trains) method (p. 128)
 
 
    To create a perfect bandlimited impulse train,
    you’d calculate the full-length sinc function centered on the first peak,
    then calculate another sinc centered on the second peak,
    and add them up
  ==============================================================================
*/

#pragma once

#include <cmath>

const double PI = std::atan(1.0) * 4;
const double TWO_PI = 2 * PI;
const float PI_OVER_4 = PI / 4;

class Oscillator {
//    Keeps track of the sine wave.
    float phase;
//    Increments the phase. How quick it changes,
    float inc;
    float phaseMax;
    
//     digital resonator version for more effeciency.
    float sin0;
    float sin1;
    float dsin;

public:
    float period = 0.0f;
    float amplitude = 1.0f;
    
    void reset()
    {
        phase = 0.0f;
        inc = 0.0f;
        
//      digital resonator version for more effeciency.
        sin0 = 0.0f;
        sin1 = 0.0f;
        dsin = 0.0f;
    }
    
//    Outputs audio data (sine tone)
    float nextSample()
    {
        float output = 0.0f;
        
//        Updating phase.
//        First half of sinc function: inc is positive and phase increments
//        Second half: inc is negative and phase decrements
        phase += inc;
        
//        Oscillator should start a new impulse. Phase is samples times PI.
//        If phase is less than PI/4 the end of the previous impulse is reached.
//        Start an new impulse. The osc enters it once per period or
//        immediate with a new note.
        if (phase <= PI_OVER_4) {
//            Find the midpoint between the finished and new peak.
//            To keep things simple, the oscillator ignores any changes
//            to period until it starts the next cycle.
            float halfPeriod = period / 2.0f;
            
//            phaseMax variable holds the position of the midpoint
//            between the two impulse peaks expressed in samples.
//            std::floor fudges the value in order to prevent aliasing
            phaseMax = std::floor(.5f + halfPeriod) - .5f;
            phaseMax *= PI;
            
            inc = phaseMax / halfPeriod;
//            after the midpoint the phase counts down to the next peak
            phase = -phase;
            
//            digital resonator version for more sine oscillator.
            sin0 = amplitude * std::sin(phase);
            sin1 = amplitude * std::sin(phase - inc);
            dsin = 2.0f * std::cos(inc);
            
//            For std::sin(0) / 0 return 1.
//            Floats make (phase == 0.0) impossible. Therefore, the span.
            if (phase * phase > 1e-9) {
//                Output value for the sample
//                Calculates sinc function multiplied by the amplitude
//                output = amplitude * std::sin(phase) / phase;
                
//                digital resonator version for output value.
                output = sin0 / phase;
            } else {
                output = amplitude;
            }
//            The current sample is somewhere between the
//            previous peak and the next. Crossing the midpoint,
//            counting backwards to next peak. Second half in reverse.
        } else {
            
            
            if(phase > phaseMax) {
//                If the phase counter goes past the half-way point,
//                set phase to the maximum and invert the increment inc
                phase = phaseMax + phaseMax - phase;
                inc = -inc;
            }
             
//            digital resonator version for sine wave approximation.
            float sinp = dsin * sin0 - sin1;
            sin1 = sin0;
            sin0 = sinp;
            
//            digital resonator version for output value
            output = sinp / phase;
        }
        return output;
    }
};
