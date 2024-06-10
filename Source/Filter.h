/*
  ==============================================================================

    Filter.h
    Created: 2 Jun 2024 6:24:41pm
    Author:  MacJay

 
    Resonant low-pass filter.
    A second order State Variable Filter (SVF) filter .
    ********************************************************************************
    Detailed explanation of the render-method:
 
    float v3 = x - ic2eq;
 
    In this line, the variable v3 is calculated as the difference between the input sample x
    and the internal state ic2eq. Here's a more detailed breakdown of
    what this means and why it is done:

    Key Concepts:

    Input Sample (x):
    This is the current input value to the filter. In a real-time audio processing context,
    x would be the audio sample currently being processed.
 
    Internal State (ic2eq):
    ic2eq represents an internal state of the filter, specifically the state related to the
    second integrator in the filter's architecture.
    This state is part of the feedback mechanism that keeps track of the previous output of the filter,
    enabling the filter to maintain its memory and produce a continuous signal.
 
    Calculation:

    The expression x - ic2eq calculates the difference between the current input and the state of
    the filter's second integrator.
    This difference (v3) serves as a part of the intermediate calculation to determine how much
    the input signal deviates from the current state of the filter.
    This deviation is important because it helps adjust the filter's response based on both the
    new input and the stored internal state, ensuring the filter reacts appropriately
    to changes in the input signal.
 
    Role in the Filter:

    In the context of a State Variable Filter (SVF), the input signal is processed through
    various stages (integrators and feedback paths) to produce different types of filtered
    outputs (low-pass, high-pass, band-pass, etc.).
    The calculation of v3 is the first step in processing the input signal through these stages.
    It sets up the subsequent calculations (for v1 and v2) by providing a value that reflects
    the new input adjusted by the current state of the filter.
    This helps in dynamically adjusting the filter's behavior, allowing it to adapt to the
    input signal while maintaining stability and continuity.
    By calculating v3 in this way, the filter ensures that the new input is considered in the
    context of the previous output (stored in ic2eq), which is crucial for the filter's feedback
    mechanism and overall performance.
    ********************************************************************************
    float v1 = a1 * ic1eq + a2 * v3;
    This line calculates the intermediate variable v1, which represents the output of the
    first integrator in the filter's architecture. Here's a more detailed explanation:

    Key Concepts:

    Internal State (ic1eq):
    ic1eq represents the internal state associated with the first integrator of the filter.
    This state variable holds the accumulated value from previous input samples, allowing the
    filter to maintain continuity and stability.
 
    Filter Coefficients (a1 and a2):
    a1 and a2 are precomputed coefficients that determine the behavior of the filter.
    These coefficients are calculated based on the filter's cutoff frequency and
    resonance (Q factor) in the updateCoefficients method.
    a1 is related to the normalization of the filter to ensure it has a proper response.
    a2 is related to the interaction between the input and the internal state to create
    the desired filter effect.
 
    Intermediate Value (v3):
    v3 is the difference between the current input sample x and the internal state ic2eq,
    calculated in the previous line.
 
    Calculation:

    The expression a1 * ic1eq + a2 * v3 combines the current state of the first
    integrator (ic1eq) with a scaled version of the intermediate value v3.

    a1 * ic1eq:
    This term scales the internal state of the first integrator by the coefficient a1.
    It reflects how much of the previous state should influence the current output.
 
    a2 * v3:
    This term scales the intermediate value v3 by the coefficient a2.
    It reflects how much the current input (adjusted by the internal state of the second integrator)
    should influence the current output.
 
    Role in the Filter:

    The calculation of v1 as a1 * ic1eq + a2 * v3 effectively integrates the input signal while
    considering the previous state of the filter.
    This integration process is a key part of the filter's operation, allowing it to accumulate input
    over time and produce a smooth, continuous output.
    v1 serves as the output of the first integrator stage and is used in the subsequent calculations
    for the second integrator and updating internal states.
 
    Purpose:

    This line ensures that the filter not only responds to the current input signal but also maintains
    a memory of past inputs through the internal state ic1eq.
    By combining the scaled internal state and the scaled input difference, the filter achieves a
    balanced and stable response, crucial for effective filtering.
    Overall, the calculation of v1 is a critical step in the digital SVF algorithm, ensuring that
    the filter accurately integrates the input signal and maintains stability through its internal state.
    ********************************************************************************
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;
    This line calculates the intermediate variable v2, which represents the output of the second
    integrator in the filter's architecture.
    
    Key Concepts:

    Internal States (ic1eq and ic2eq):
    ic1eq is the internal state associated with the first integrator of the filter, representing
    its accumulated value from previous input samples.
    ic2eq is the internal state associated with the second integrator, holding the accumulated
    value from previous states and inputs, crucial for maintaining the filter's stability and continuity.
 
    Filter Coefficients (a2 and a3):
    a2 and a3 are precomputed coefficients that determine the behavior of the filter.
    These coefficients are calculated based on the filter's cutoff frequency and
    resonance (Q factor) in the updateCoefficients method.
    a2 is used to scale the internal state of the first integrator (ic1eq).
    a3 is used to scale the intermediate value v3.
 
    Intermediate Value (v3):
    v3 is the difference between the current input sample x and the internal state ic2eq,
    calculated earlier in the process.
 
    Calculation:

    The expression ic2eq + a2 * ic1eq + a3 * v3 combines the current state of the
    second integrator (ic2eq) with scaled versions of the first integrator's state (ic1eq)
    and the intermediate value v3.

    ic2eq:
    This term represents the current state of the second integrator, providing a base value for
    the calculation.
    It reflects the accumulated value up to the previous input sample.
 
    a2 * ic1eq:
    This term scales the internal state of the first integrator by the coefficient a2.
    It integrates the contribution of the first integrator's state to the second integrator's output.
 
    a3 * v3:
    This term scales the intermediate value v3 by the coefficient a3.
    It integrates the contribution of the input difference (adjusted by the second integrator's state)
    to the second integrator's output.
 
    Role in the filter:
 
    The calculation of v2 as ic2eq + a2 * ic1eq + a3 * v3 effectively integrates the output of the
    first integrator and the input signal through the second integrator.
    This integration process is crucial for producing the desired filtered signal, allowing the
    filter to accumulate input over time and generate a smooth, continuous output.
    v2 serves as the final output of the second integrator stage and represents the overall output
    of the filter for the current input sample.
 
    Purpose:

    This line ensures that the filter produces an output (v2) that reflects both the current input signal
    and the accumulated states of the integrators (ic1eq and ic2eq).
    By combining these terms, the filter achieves a balanced and stable response, crucial for effective filtering.
    v2 is used as the final output of the filter for the given input sample x and is also used to
    update the internal states for the next iteration.
    Overall, the calculation of v2 is a critical step in the digital SVF algorithm, ensuring that the filter
    accurately integrates the input signal through both integrators and maintains stability through its internal states.









 
 
  ==============================================================================
*/

#pragma once

class Filter {
    const float PI = 3.1415926535897932f;
//  Filter coefficients.
    float g, k, a1, a2, a3;
//    Internal state.
    float ic1eq, ic2eq;
    
public:
    float sampleRate;
    
//    A change of the cutoff frequency or the amount of
//    resonance results in a re-calculation of
//    the filter’s coefficients.
    void updateCoefficients(float cutoff, float Q) {
        g = std::tan(PI * cutoff / sampleRate);
        
//        k determines how peaky or broad the filter's
//        response will be at the cutoff frequency.
        k = 1.0f / Q;
        
//        The coefficients a1, a2, and a3 are used to
//        control the filter's behavior, determined by
//        the cutoff frequency and resonance (Q factor)
//        set in the updateCoefficients method.
//        a1 controlling the normalization of the filter.
        a1 = 1.0f / (1.0f + g * (g + k));
//        a2 is controlling the interaction between the
//        input signal and the internal state.
        a2 = g * a1;
//        a3 determines how much influence the current
//        input sample has on the second integrator's output.
        a3 = g * a2;
    }
    
    void reset() {
        g = 0.0f;
        k = 0.0f;
        a1 = 0.0f;
        a2 = 0.0f;
        a3 = 0.0f;
        ic1eq = 0.0f;
        ic2eq = 0.0f;
    }
    
//    The render method takes an input sample x and processes it
//    through the SVF algorithm to produce a filtered output.
    float render(float x) {
        float v3 = x - ic2eq;
//        v1 represents the output of the first integrator.
//        This line combines the state of the first integrator
//        with the scaled input (v3) to produce the new v1.
        float v1 = a1 * ic1eq + a2 * v3;
//        This line calculates v2, which represents the output
//        of the second integrator. The final output.
//        v2 combines the states of the first and second
//        integrators with the scaled input to produce the new v2.
        float v2 = ic2eq + a2 * ic1eq + a3 * v3;
        
//      Updating the internal states (ic1eq and ic2eq) to reflect
//      the current state of the filter after processing the input.
        ic1eq = 2.0f * v1 - ic1eq; // First integrator
        ic2eq = 2.0f * v2 - ic2eq; // Second integrator
        
        return v2;
    }

};
