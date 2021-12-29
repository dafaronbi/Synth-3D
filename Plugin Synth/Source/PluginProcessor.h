/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "synth.h"
#include "personalDS.h"

//==============================================================================
/**
*/
class PluginSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PluginSynthAudioProcessor();
    ~PluginSynthAudioProcessor() override;

    //==============================================================================
    void addVSTParam();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    //Update synth paramters from gui thread
    void updateSyntheParameters(synth_parameters);
    synth_parameters getParams();
    

private:
    //synthesizer class
    juce::Synthesiser synth;
    
    //synth parameter values
    synth_parameters param;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSynthAudioProcessor)
    
    //synthesizer parameter variables
    juce::AudioParameterInt* osc1_wavShape;
    juce::AudioParameterInt*  osc2_wavShape;
    juce::AudioParameterInt* osc3_wavShape;
    juce::AudioParameterFloat* osc1_gain;
    juce::AudioParameterFloat* osc2_gain;
    juce::AudioParameterFloat* osc3_gain;
    juce::AudioParameterInt* osc1_az;
    juce::AudioParameterInt* osc2_az;
    juce::AudioParameterInt* osc3_az;
    juce::AudioParameterFloat* osc1_distance;
    juce::AudioParameterFloat* osc2_distance;
    juce::AudioParameterFloat* osc3_distance;
    juce::AudioParameterInt* filter1_type;
    juce::AudioParameterInt* filter2_type;
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
