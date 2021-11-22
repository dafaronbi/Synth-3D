/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "extraComponents.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PluginSynthAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::ChangeListener, public juce::Button::Listener
{
public:
    PluginSynthAudioProcessorEditor (PluginSynthAudioProcessor&);
    ~PluginSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void buttonClicked (juce::Button* button) override;
    void updateAngle1Delta();
    void updateAngle2Delta();
    void updateAngle3Delta();
    void updateFilter1Values();
    void updateFilter2Values();
    void updateEnvelopes();
    void start_adsr();
    void start_adsr_save();
    float synth();
    float osc();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginSynthAudioProcessor& audioProcessor;
    
    // gui segments
    Nav_Bar nBar;
    Filter_Menu fMenu;
    Oscillator_Menu oMenu;
    amplifier_Menu aMenu;
    juce::TextButton save_button;
    juce::TextButton test_audio;
    juce::File save_directory;
    juce::Random random;
    bool audible;
    double sample_rate;
    
    //current Angle
    double currentAngle1;
    double currentAngle2;
    double currentAngle3;
    
    //current frequency from sliders
    double currentFrequency1;
    double currentFrequency2;
    double currentFrequency3;
    
    //Angle to update for  desired frequency
    double angleDelta1;
    double angleDelta2;
    double angleDelta3;
    
    //current  sample
    double currentSample1;
    double currentSample2;
    double currentSample3;
    
    //filter variables
    double cutoff_freq1;
    double cutoff_freq2;
    double resonance1;
    double resonance2;
    double f_attack;
    double f_decay;
    double f_sustain;
    double f_release;
    int filter1_type;
    int filter2_type;
    
    //IIR filter variables
    juce::IIRFilter filter1;
    juce::IIRFilter filter2;
    
    //amplifier variables
    double a_total_gain;
    double a_attack;
    double a_decay;
    double a_sustain;
    double a_release;
    
    //ASDRs
    juce::ADSR f_adsr;
    juce::ADSR a_adsr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSynthAudioProcessorEditor)
};
