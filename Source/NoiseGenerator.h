/*
  ==============================================================================

    NoiseGenerator.h
    Created: 2 May 2024 2:20:09pm
    Author:  MacJay

 
    This class generates white noise.
  ==============================================================================
*/

#pragma once

class NoiseGenerator {
    unsigned int noiseSeed;
    
public:
    void reset()
    {
        noiseSeed = 22222;
    }
    
    float nextValue()
    {
//        Generate the next integer pseudorandom number.
        noiseSeed = noiseSeed * 196314165 + 907633515;
        
        /*
            Convert to a signed value.
            Since noiseSeed is an unsigned int,
            it can take on values between 0 and 4294967295.
         
            However, the lowest bits of the generated sequence are not very random,
            so we shift those out using noiseSeed >> 7 and only keep the 25 highest bits.
            The subtraction gives us signed integers in the range –16777216 to +16777215
         */
        int temp = int(noiseSeed >> 7) - 16777216;
        
        
//        Convert to a floating-point number between -1.0 and 1.0.
        return float(temp) / 16777216.0f;
        
    }
};
