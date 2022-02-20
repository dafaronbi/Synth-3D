/*
  ==============================================================================

    personalDS.h
    Created: 27 Nov 2021 9:27:50pm
    Author:  Daniel Faronbi
 
    Personal data structures needed for the project

  ==============================================================================
*/

struct synth_parameters {
    juce::AudioParameterChoice* osc1_wavShape;
    juce::AudioParameterChoice*  osc2_wavShape;
    juce::AudioParameterChoice* osc3_wavShape;
    juce::AudioParameterFloat* osc1_freqOff;
    juce::AudioParameterFloat* osc2_freqOff;
    juce::AudioParameterFloat* osc3_freqOff;
    juce::AudioParameterFloat* osc1_gain;
    juce::AudioParameterFloat* osc2_gain;
    juce::AudioParameterFloat* osc3_gain;
    juce::AudioParameterInt* osc1_az;
    juce::AudioParameterInt* osc2_az;
    juce::AudioParameterInt* osc3_az;
    juce::AudioParameterFloat* osc1_distance;
    juce::AudioParameterFloat* osc2_distance;
    juce::AudioParameterFloat* osc3_distance;
    juce::AudioParameterChoice* filter1_type;
    juce::AudioParameterChoice* filter2_type;
    juce::AudioParameterFloat* filter1_cuttoff;
    juce::AudioParameterFloat* filter2_cuttoff;
    juce::AudioParameterFloat* filter1_resonance;
    juce::AudioParameterFloat* filter2_resonance;
    juce::AudioParameterFloat* filter_attack;
    juce::AudioParameterFloat* filter_decay;
    juce::AudioParameterFloat* filter_sustain;
    juce::AudioParameterFloat* filter_release;
    juce::AudioParameterFloat* amp_attack;
    juce::AudioParameterFloat* amp_decay;
    juce::AudioParameterFloat* amp_sustain;
    juce::AudioParameterFloat* amp_release;
    juce::AudioParameterFloat* total_gain;
};

//struct synth_parameters {
//    int osc1_wavShape = 1;
//    int osc2_wavShape = 1;
//    int osc3_wavShape = 1;
//    float osc1_freqOff = 0;
//    float osc2_freqOff = 0;
//    float osc3_freqOff = 0;
//    float osc1_gain = 0;
//    float osc2_gain = 0;
//    float osc3_gain = 0;
//    int osc1_az = 0;
//    int osc2_az = 0;
//    int osc3_az = 0;
//    float osc1_distance = 0;
//    float osc2_distance = 0;
//    float osc3_distance = 0;
//    int filter1_type = 1;
//    int filter2_type = 1;
//    float filter1_cuttoff = 20000;
//    float filter2_cuttoff = 20000;
//    float filter1_resonance = 0;
//    float filter2_resonance = 0;
//    float filter_attack = 0;
//    float filter_decay = 0;
//    float filter_sustain = 1;
//    float filter_release = 0;
//    float amp_attack = 0;
//    float amp_decay = 0;
//    float amp_sustain = 1;
//    float amp_release = 0;
//    float total_gain = 0;
//};

#pragma once
