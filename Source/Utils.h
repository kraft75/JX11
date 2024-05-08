/*
  ==============================================================================

    Utils.h
    Created: 2 May 2024 6:01:52pm
    Author:  MacJay

 
    Ear protection! It checks if The audio values int the buffer are
    reasonable for the ears. If the value exceeds the threshold the out-
    put will be muted.
 
    Helper function for the constructor in PluginProcessor in order to
    grab the parameter.
  ==============================================================================
*/

#pragma once

class Utils {
public:
//    A shortcut for the class PluginProcessor.
    template<typename T>
    static void castParameter(juce::AudioProcessorValueTreeState& apvts,
                              const juce::ParameterID& id, T& destination)
    {
        destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
        jassert(destination);
    }
    
//    Ear protection.
    void protectYourEars(float* buffer, int sampleCount) {
        if (buffer == nullptr) { return; }
        
        bool firstWarning = true;
        for (int i = 0; i < sampleCount; ++i) {
            float x = buffer[i];
            bool silence = false;
            
            if (std::isnan(x)) {
                DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
                silence = true;
            } else if (std::isinf(x)) {
                DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
                silence = true;
            } else if (x < -2.0f || x > 2.0f) {
                DBG("!!! WARNING: sample out of range, silencing !!!");
                silence = true;
//                Exceedance tolerable
            } else if (x < -1.0f) {
                if (firstWarning) {
                    DBG("!!! WARNING: sample out of range, clamping !!!");
                    firstWarning = false;
                }
                buffer[i] = -1.0f;
//                Exceedance tolerable
            } else if (x > 1.0f) {
                if (firstWarning) {
                    DBG("!!! WARNING: sample out of range, clamping !!!");
                    firstWarning = false;
                }
                buffer[i] = 1.0f;
            }
            
            if (silence) {
//                Mutes audio
                memset(buffer, 0, sampleCount * sizeof(float));
                return;
            }
        }
    }
};
