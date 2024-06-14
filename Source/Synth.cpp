/*
  ==============================================================================

    Synth.cpp
    Created: 1 May 2024 9:50:18pm
    Author:  MacJay

  ==============================================================================
*/

#include "Synth.h"

static const float ANALOG = 0.002f;
static const int SUSTAIN = -1;

Synth::Synth() : sampleRate(44100.0f) {}
//==============================================================================
void Synth::allocateResources(double sampleRate_, int samplesPerBlock)
{
    sampleRate = static_cast<float>(sampleRate_);
    
    for (int i = 0; i < MAX_VOICES; ++i) {
//        For each voice a filter.
//        Ccalculating the coefficients with the sample rate.
        voices[i].filter.sampleRate = sampleRate;
    }
}
//==============================================================================

void Synth::deallocateResources() 
{
// do nothing
}
//==============================================================================

void Synth::reset()
{
//    On initialization of the plug-in
//    reset Voice and Generator instance.
//    Defy start value for Pitch bend
    for (int i = 0; i < MAX_VOICES; ++i) {
        voices[i].reset();
    }
    
    noiseGen.reset();
    
    pitchBend = 1.0f;
    
    sustainPedalPressed = false;
    
    lfo = 0.0f;
    lfoStep = 0;
    
    resonanceCtl = 1.0f;
    
//    Smoothing time
    outputLevelSmoother.reset(sampleRate, 0.05f);
    
    modWheel = 0.0f;
//    No glide for the very first note played.
    
    lastNote = 0;
    
    pressure = 0.0f;
    
    filterCtl = 0.0f;
    
    filterZip = 0.0f;
}
//==============================================================================

void Synth::render(float** outputBuffers, int sampleCount) 
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
//    New detuned value by the oscillator
//    while the sound is playing.
//    Updating the period in processBlock
    for (int i = 0; i < MAX_VOICES; ++i) {
        Voice& voice = voices[i];
//        Check if key is pressed.
        if (voice.env.isActive()) {
            updatePeriod(voice);
            voice.glideRate = glideRate;
            voice.filterQ = filterQ * resonanceCtl;
            voice.pitchBend = pitchBend;
            voice.filterEnvDepth = filterEnvDepth;
        }
    }
    
//    Loop thru the samples. If there were MIDI messages
//    this will be less than the total number of samples in the block.
    for (int sample = 0; sample < sampleCount; ++sample) {
//        Adding vibrato
        updateLFO();
//        Next output from noise generator multiplied by
//        the parameter noise (noiseMix).
        float noise = noiseGen.nextValue() * noiseMix;
        
//        Output for the left and right speaker
        float outputLeft = 0.0f;
        float outputRight = 0.0f;
        
        for (int i = 0; i < MAX_VOICES; ++i) {
            Voice& voice = voices[i];
//        Check if key is pressed.
            if (voice.env.isActive()) {
//            Audio data with added noise.
                float output = voice.render(noise);
//            Sample is mixed into the left/right
//            channel output using panLeft/panRight
//            amount.
                outputLeft += output * voice.panLeft;
                outputRight += output * voice.panRight;
            }
        }
        
//        Adding all total volume.
//        The smoother performs a linear interpolation between
//        the previous value and the new value.
        float outputLevel = outputLevelSmoother.getNextValue();
        outputLeft *= outputLevel;
        outputRight *= outputLevel;

        
//        Write output values into the respective audio buffer.
        if (outputBufferRight != nullptr) {
            outputBufferLeft[sample] = outputLeft;
            outputBufferRight[sample] = outputRight;
        } else {
//            Case mono: left and right values need to be combined
//            into a mono sample. No stereo.
            outputBufferLeft[sample] = (outputLeft + outputRight) * .5f;
        }
    }
    
    for (int i = 0; i < MAX_VOICES; ++i) {
        Voice& voice = voices[i];
        if (!voice.env.isActive()) {
            voice.env.reset();
            voice.filter.reset();
        }
    }
    
    
//        Mutes the audio for values beyond -2.0f and 2.0f
    earProtect.protectYourEars(outputBufferLeft, sampleCount);
    earProtect.protectYourEars(outputBufferRight, sampleCount);
}

void Synth::updateLFO() 
{
//    Statement is entered every 32 samples
    if (--lfoStep <= 0) {
        lfoStep = LFO_MAX;
        
//        Increment the LFO’s phase variable lfo.
        lfo += lfoInc;
//        When this exceeds PI
        if (lfo > PI) {
//            subtract TWO_PI to put lfo back to -PI.
//            The phase is kept somewhere between ±π
//            that the argument for std::sin isn't too large.
            lfo -= TWO_PI;
        }
        
//        Calculate the sine
        const float sine = std::sin(lfo);
        
//        Calculate vibrato amount.
//        Max value for vibrato is 0.05 and sin changes
//        between -1 and 1. This makes -0.05 and 0.05.
//        To achieve the final value of
//        vibratoMod(0.95 and 1.05) -> 2^−1/12 = 0.9439
//        2^1/12 = 1.0594. We add 1.0f.
//        Range including modWheel: 0.869 - 1.131.
        float vibratoMod = 1.0f + sine * (modWheel + vibrato);
//        Assigning the intensity for the PWM modulation
//        in the same way as vibrato.
        float pwm = 1.0f + sine * (modWheel + pwmDepth);
        
//      Distance from cutoff and LFO
        float filterMod = filterKeyTracking + filterCtl + (filterLFODepth + pressure) * sine;
        
//        Smoothed version of filterMod.
        filterZip += 0.005f * (filterMod - filterZip);
        
//        Add vibrato to modulation.
        for (int v = 0; v < MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            if (voice.env.isActive()) {
                voice.osc1.modulation = vibratoMod;
                voice.osc2.modulation = pwm;
                voice.filterMod = filterZip;
//                Get the new target period..
                voice.updateLFO();
//                ..and update it
                updatePeriod(voice);
            }
        }
    }
    
}
//==============================================================================

bool Synth::isPlayingLegatoStyle() const
{
    int held = 0;
    for (int i = 0; i < MAX_VOICES; ++i) {
//        This method simply counts how many of the currently playing
//        voices are for keys that are still held down.
//        How many voices did not get a Note Off event yet.
        if (voices[i].note > 0) {
            held += 1;
        }
    }
    return held > 0;
}

void Synth::startVoice(int v, int note, int velocity)
{
    float period = calcPeriod(v, note);
    Voice& voice = voices[v];
    
//    Settings of the envelope attributes
//    ------------------------------------------------------------------
//    Amplitude
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
//    Setting private members level and target
    env.attack();
    
//    Filter
    Envelope& filterEnv = voice.filterEnv;
    filterEnv.attackMultiplier = filterAttack;
    filterEnv.decayMultiplier = filterDecay;
    filterEnv.sustainLevel = filterSustain;
    filterEnv.releaseMultiplier = filterRelease;
    filterEnv.attack();
//    ------------------------------------------------------------------
//    Oscillator settings
//    ------------------------------------------------------------------

    voice.target = period;
    voice.note = note;
    
//    Calculates the distance of the new note
//    in semitones.
    int noteDistance = 0;
    if (lastNote > 0) {
//        0 -> Skip and noteDistance = 0.
//        2 -> always
//        if legato (1) it only happens when more
//        than on key is pressed.
        if ((glideMode == 2) || ((glideMode == 1) && isPlayingLegatoStyle())) {
            noteDistance = note - lastNote;

        }
    }
    
//    Set voice.period to the period to glide from.
//    Additional semitones from the Glide Bend parameter.
    voice.period = period * std::pow(1.059463094359f, float(noteDistance) - glideBend);
    
//    The period shouldn't be too small.
    if (voice.period < 6.0f)
    {
        voice.period = 6.0f;
    }
    
//    Assign the new note number to voice.note
//    and lastNote.
    lastNote = note;
    voice.note = note;
    voice.updatePanning();
    
//    Changing the velocity from a linear to a logarithmic curve.
    float vel = .004f * float((velocity + 64) * (velocity + 64)) - 8.0f;
    voice.osc1.amplitude = vel * volumeTrim;
    voice.osc2.amplitude = voice.osc1.amplitude * oscMix;
    
//    If the synth is in PWM mode..
    if (vibrato == 0.0f && pwmDepth > 0.0f) {
//        ..the second oscillator will now be phase-locked
//        to the first one creating a squarewave.
        voice.osc2.squareWave(voice.osc1, voice.period);
    }
//    ------------------------------------------------------------------
//    Filter  settings
//    ------------------------------------------------------------------
//    Cutoff frequency for each note
    voice.cutoff = sampleRate / (PI * period);
//    64 is the middle of the velocity range
//    which results in a multiplier of 1.0.
//    Same when velocitySensitivity is 0.
//    Range is 1/24 - 24. It’s  like ±55 semitones.
//    Up and down by four-and-a-half octaves.
    voice.cutoff *= std::exp(velocitySensitivity * float(velocity - 64));
    
    
    
    
//    Optional reset of the oscillators.
//    No reset emulates the behavior of
//    an analogue hardware synthesizer.
//    voice.osc1.reset();
//    voice.osc2.reset();
    
    
}

void Synth::restartMonoVoice(int note, int velocity)
{
    float period = calcPeriod(0, note);
    
    Voice& voice = voices[0];
    voice.target = period;
    
//    If gliding is disabled, it sets voice.period to
//    the same target value, which will skip the
//    glide effect.
    if (glideMode == 0)
    {
        voice.period = period;
    }
    
    voice.env.level += SILENCE + SILENCE;
    voice.note = note;
    voice.updatePanning();
    
    voice.cutoff = sampleRate / (period * PI);
    if (velocity > 0) {
        voice.cutoff *= std::exp(velocitySensitivity * float(velocity - 64));
    }

}

void Synth::noteON(int note, int velocity)
{
    if (ignoreVelocity) {
//        Disabling velocity modulation
//        by setting a fixed value.
        velocity = 80;
    } else {
//    Velocity sensitivity for the amplitude. Not an
//    official part of the original JX11.
//    Formula returns a value between 0 and 1.
        float sens = std::abs(velocitySensitivity) / 0.05;
        velocity *= sens;
    }
    


//    0 is mono mode.
    int v = 0;
    
//    monophonic
    if (numVoices == 1) {
//        The key for the previous note is still being held down.
//        No note off event, yet.
//        If the condition is false, play staccato.
        if (voices[0].note > 0) {
            shiftQueuedNotes();
//            Call the method for legato-style playing.
            restartMonoVoice(note, velocity);
            return;
        }
//        Otherwise polyphonic
    } else {
        v = findFreeVoice();
    }
    
    startVoice(v, note, velocity);
}

int Synth::findFreeVoice() const 
{
    int v = 0;
// Random threshold for initialization
    float l = 100.0f;
    
    for (int i = 0; i < MAX_VOICES; ++i) {
//        Find the voice with the lowest envelope level.
//        Ignores voices in the attack stage.
        if (voices[i].env.level < l && !voices[i].env.isInAttack()) {
            l = voices[i].env.level;
            v = i;
        }
    }
    return v;
}

void Synth::noteOff(int note)
{
//    Last note priority
//    Checks if the synth is mono and that the key that
//    got released is for the note that is currently playing
    if ((numVoices == 1) && voices[0].note == note) {
        int queuedNote = nextQueuedNote();
//        Is player still holding down any keys
        if (queuedNote > 0) {
//            Change the pitch
            restartMonoVoice(queuedNote, -1);
        }
    }
    
    for (int i = 0; i < MAX_VOICES; ++i) {
        Voice& voice = voices[i];
//    Checking all voices to see which is playing
//    the actual note.
        if (voice.note == note) {
            if (sustainPedalPressed) {
//                Key was released (pedal pressed)
//                but the voice will keep playing.
                voice.note = SUSTAIN;
            } else {
//            Begin the release phase in the envelope.
                voice.release();
//            Note Off event was received for this voice.
//            Voice will keep playing until the note faded out.
                voice.note = 0;
            }
        }
    }
}

void Synth::shiftQueuedNotes() 
{
    for (int tmp = MAX_VOICES; tmp > 0; --tmp) {
        voices[tmp].note = voices[tmp-1].note;
//        Prevents bugs caused by switching from
//        polyphonic to monophonic while holding
//        chords. Otherwise the other voices would
//        keep ringing.
        voices[tmp].release();
    }
}

int Synth::nextQueuedNote()
{
    int held = 0;
    for (int i = MAX_VOICES - 1; i > 0; --i) {
        //        Array is iterated backwards in order to
        //        find the first active note.
        if (voices[i].note > 0) {
            //            Save the position of the note
            held = i;
        }
    }
        
//        When a pressed note is found
    if (held > 0) {
//            Save its value and return it
        int note = voices[held].note;
//            and erase it.
        voices[held].note = 0;
        return note;
    }
    return 0;
}


//==============================================================================
void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
//    Consider only the four highest bits (command)
    switch (data0& 0xF0) {
//            Note off
        case 0x80: {
            noteOff(data1& 0x7F);
            break;
        }
//            Note on
        case 0x90: {
            uint8_t note = data1& 0x7F;
            uint8_t velo = data2& 0x7F;
            
            if (velo > 0) {
                noteON(note, velo);
            } else {
                noteOff(note);
            }
            break;
        }
//            Pitch bend
        case 0xE0: {
//            In Pitch bend data1 and data2 are combined.
//            It results in a 14-bit number in a a range of
//            0 – 16383 if unsigned or –8192 to 8191 signed.
//            (data1 + 128 * data2 - 8192)
            
//            Range to make the pitch rise  in 2 semitones
//            2^2/12 = 1.12. Lower 2 semitones is 0.89.
//            By changing the period it is inverse.
//            Mapping: –8192 to 8191 into 1.12 – 0.89.
//            Origin formula: 2^2*(data/8192)/12
            pitchBend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            break;
        }
//            Channel aftertouch
        case 0xD0: {
//            Additional depth to the LFO.
//            Maps the pressure value to a parabolic curve
//            starting at 0.0 (position 0) up to 1.61 (position 127).
            pressure = 0.0001f * float(data1 * data1);
            break;
        }
//            Control change
        case 0xB0: {
            controlChange(data1, data2);
            break;
        }
    }
}

void Synth::controlChange(uint8_t data1, uint8_t data2)
{
//    data1 determines the type of controller
//    data 2 the value (0-127)
    switch (data1) {
//            Sustain pedal
        case 0x40: {
//            Puts sustainPedalPressed to the state
//            true if the value is 64 or more. Usually,
//            pedals output off (0) on (127). This calc-
//            ulation also considers pedals with contiuous
//            values.
            sustainPedalPressed = (data2 >= 64);
            
//            Release note if the pedal isn't pressed
//            anymore.
            if (!sustainPedalPressed) {
                noteOff(SUSTAIN);
            }
            break;
        }
//            Mod wheel
        case 0x01: {
//            Values converted to a parabolic curve in
//            order to gain more control over small values.
//            0 maps to 0 and 127 to 0.0806.
            modWheel = 0.000005f * float(data2 * data2);
            break;
        }
//            Resonance
        case 0x47: {
//            Maps the position of the controller (0 – 127)
//            to a linear curve between 1.0 and 5.7 which
//            add an extra boost to the value from the
//            Filter Reso parameter.
            resonanceCtl = 154.0f / float(154 - data2);
            break;
        }
//            Filter in positive direction.
//            Turns cutoff higher.
        case 0x4A: {
            filterCtl = .02f * float(data2);
            break;
        }
//            Filter in negative direction.
//            Turns cutoff lower:
        case 0x4B: {
            filterCtl = -.03f * float(data2);
            break;
        }
            
//            PANIC message. Synth stops playing all
//            notes immediately.
//            If a note got stuck.
        default:
            if (data1 >= 0x78) {
                for (int i = 0; i < MAX_VOICES; ++i) {
                    voices[i].reset();
                }
                sustainPedalPressed = false;
            }
            break;
    }
    
}

//==============================================================================
float Synth::calcPeriod(int v, int note) const
{
//    Period in samples instead of Hz. p. 225
//    from freq to period.
//    replace pow(x, y) with exp(y * log(x)). Exp is faster.
//    ANALOG and v emulates random detuning. I.e, temperature.
    float period = tune * std::exp(-0.05776226505f * (float(note) + ANALOG * float(v)));
    
//    BLIT-based oscillator may not work reliably if
//    the period is too small.
//    A prevention is to guarantee a period with
//    with at least 6 samples.
    while (period < .6f || (period * detune) < .6f) {
        period += period;
    }
    
    return period;
}

//==============================================================================


