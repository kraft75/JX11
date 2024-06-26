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
    
    Other algorithms on p. 145
 
    Vibrato: We multiply the pitch by a sine wave that slowly oscillates between 0.95 and 1.05,
    to get the modulated sawtooth wave.
    One way to achieve vibrato is to change the period variable using the LFO.
    However, it’s cleaner to not apply the modulations to the period directly but use
    a new variable for this.

 
    The logic in squareWave sets the starting phase of the second oscillator so that its sinc peaks
    fall exactly halfway in between two sinc peaks from the first oscillator.
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
    
//    phase counts up to phaseMax
    float phaseMax;
    
//    DC Offset. Blob at 0 Hz. The subtraction
//    creates the sawtooth waveform
    float dc;
    
//     digital resonator version for sine oscillator.
    float sin0;
    float sin1;
    float dsin;

public:
    float period = 0.0f;
    float amplitude = 1.0f;
//    Current amount of modulation to be applied to the period.
//    1.0f means no vibrato.
    float modulation = 1.0f;
    
    void reset()
    {
        phase = 0.0f;
        inc = 0.0f;
        dc = 0.0f;
        
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
            float halfPeriod = period / 2.0f * modulation;
            
//            phaseMax variable holds the position of the midpoint
//            between the two impulse peaks expressed in samples.
//            std::floor fudges the value in order to prevent aliasing
            phaseMax = std::floor(.5f + halfPeriod) - .5f;
            
//            Signal’s average value. The average value needs to be divided
//            by the number of samples.
            dc = .5f * amplitude / phaseMax;
            
//            The sinc function is sin(phase * PI) / (phase * PI).
//            To avoid the permanent multiplication by PI we set
//            the unit of the phase, phaseMax and inc with an
//            multiplication by Pi.
            phaseMax *= PI;
            
//            Phase increment. Theoretically inc equals PI. Due to
//            the fudged midpoint it deviates slightly from PI.
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
//        Substract the DC-Offset to avoid clicks and pops.
        return output - dc;
    }
    
//    The squareWave function is used to set the phase of one
//    oscillator based on the phase of the other oscillator.
//    This method is called on osc2 when the voice starts
//    playing a new note.
    void squareWave(Oscillator& other, float newPeriod)
    {
//        In case the osc had been used already,
        reset();
        
//        Figuring out what the phase of the other oscillator is
//        and in which direction it’s going.
        if (other.inc > 0.0f) {
//            phase is set to a value that reflects other.phase
//            about other.phaseMax. This mirrors the phase value.
            phase = other.phaseMax + other.phaseMax - other.phase;
//            Inverting the direction of phase increment.
            inc = -other.inc;
        } else if (other.inc < 0.0f) {
//            Copies the values
            phase = other.phase;
            inc = other.inc;
        } else {
//            The other oscillator has not started yet,
//            and there’s no way of knowing where its peaks will be.
//            Guessing that inc is a value close to PI.
//            Initializing the phase for a new cycle.
            phase = -PI;
//            Which is a standard increment to start the wave.
            inc = PI;
        }
        
//        Shift the phase by half a period so that we’re halfway
//        in between the peaks of the other oscillator.
        phase += PI * newPeriod / 2.0f;
        phaseMax = phase;
    }
};
