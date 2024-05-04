/*
  ==============================================================================

    Oscillator.h
    Created: 2 May 2024 6:53:17pm
    Author:  MacJay

    Sine Oscillator
  ==============================================================================
*/

#pragma once

#include <cmath>

//  Calculate pi
const double PI = std::atan(1.0) * 4;
//  Calculate 2*pi
const double TWO_PI = 2 * PI;

class Oscillator {
public:
    float amplitude;
    float inc;
    float phase;
    
//    Variant digital resonator.
    float sin0;
    float sin1;
    float dsin;
    
    void reset()
    {
//        Cosine wave
        phase = 0.0f;
        
        //    Variant digital resonator.
        sin0 = amplitude * std::sin(phase * TWO_PI);
        sin1 = amplitude * std::sin((phase - inc) * TWO_PI);
        dsin = 2.0f * std::cos(inc * TWO_PI);

    }
    
//    Outputs audio data (sine tone)
    float nextSample()
    {
        /*
        phase += inc;
//        It takes sampleRate/freq samples for phase to count from 0 to 1.
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
//        Sine needs as an argument an angle in radians
//        phase shows the current position of sine
        return amplitude * std::sin(TWO_PI * phase);
         */
        
//            Variant digital resonator.
//            An alternative that saves computing time by the sine function
        float sinx = dsin * sin0 - sin1; sin1 = sin0;
        sin0 = sinx;
        return sinx;
    }
};
