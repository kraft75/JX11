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
    float freq;
    float sampleRate;
    float phaseOffset;
    int sampleIndex;
    
    void reset()
    {
        sampleIndex = 0;
    }
    
//    Outputs audio data (sine tone)
    float nextSample()
    {
        float output = amplitude * std::sin(TWO_PI * sampleIndex * freq / sampleRate + phaseOffset);

        sampleIndex += 1;
        
        return output;
    }
};
