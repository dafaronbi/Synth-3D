/*
  ==============================================================================

    synth.h
    Created: 22 Nov 2021 10:39:21pm
    Author:  Daniel Faronbi
    Use: generate audio from synthesizer

  ==============================================================================
*/

#include <JuceHeader.h>
#include "personalDS.h"


struct synthSound   : public juce::SynthesiserSound
{
    synthSound() {}
 
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};


class synthVoice : public juce::SamplerVoice
{
public:
    synthVoice(synth_parameters param){
        
        synth_param = param;
    }
    
    bool canPlaySound (juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<synthSound*> (sound) != nullptr;
        }
    
    void startNote (int midiNoteNumber, float velocity,
                        juce::SynthesiserSound*, int currentPitchWheelPosition) override
        {
            currentAngle = 0.0;
            level = velocity * 0.15;
            tailOff = 0.0;
     
            currentFrequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
            auto cyclesPerSample = currentFrequency / getSampleRate();
     
            angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
            updateFilter1Values();
            updateFilter2Values();
            updateEnvelopes();
        }
    
    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
        {
            if (angleDelta != 0.0)
            {
                if (tailOff > 0.0) // [7]
                {
                    while (--numSamples >= 0)
                    {
                        auto currentSample = (synth(currentAngle,startSample) * level * tailOff);
     
                        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                            outputBuffer.addSample (i, startSample, currentSample);
     
                        currentAngle += angleDelta;
                        ++startSample;
     
                        tailOff *= 0.99; // [8]
     
                        if (tailOff <= 0.005)
                        {
                            clearCurrentNote(); // [9]
     
                            angleDelta = 0.0;
                            break;
                        }
                    }
                }
                else
                {
                    while (--numSamples >= 0) // [6]
                    {
                        auto currentSample = (synth(currentAngle,startSample) * level);
     
                        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                            outputBuffer.addSample (i, startSample, currentSample);
     
                        currentAngle += angleDelta;
                        ++startSample;
                    }
                }
            }
        }
    
    void stopNote (float velocity, bool allowTailOff) override
        {
            if (allowTailOff)
            {
                if (tailOff == 0.0)
                    tailOff = 1.0;
            }
            else
            {
                clearCurrentNote();
                angleDelta = 0.0;
            }
        }
    
    void updateFilter1Values(){
        
        //get cuttoff frequency from envelope
        double cutoff = cutoff_freq1;
        
        //select filter type and get coefficients
        switch(filter1_type){
            case 1:
                filter1.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), cutoff));
                break;
            case 2:
                filter1.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), cutoff));
                break;
            case 3:
                filter1.setCoefficients(juce::IIRCoefficients::makeBandPass(getSampleRate(), cutoff));
                break;
            case 4:
                filter1.setCoefficients(juce::IIRCoefficients::makeNotchFilter(getSampleRate(), cutoff));
                break;
            default:
                filter1.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), cutoff));
            
        }
        
        filter1.reset();
    }

    void updateFilter2Values(){
        
        //get cuttoff frequency from envelope
        double cutoff = cutoff_freq2;
        
        //select filter type and get coefficients
        switch(filter2_type){
            case 1:
                filter2.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), cutoff));
                break;
            case 2:
                filter2.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), cutoff));
                break;
            case 3:
                filter2.setCoefficients(juce::IIRCoefficients::makeBandPass(getSampleRate(), cutoff));
                break;
            case 4:
                filter2.setCoefficients(juce::IIRCoefficients::makeNotchFilter(getSampleRate(), cutoff));
                break;
            default:
                filter2.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), cutoff));
        }
            
        filter2.reset();
    }

    void updateEnvelopes(){
        juce::ADSR::Parameters fParams;
        juce::ADSR::Parameters aParams;
        
        fParams.attack = f_attack;
        fParams.decay = f_decay;
        fParams.sustain = f_sustain;
        fParams.release = f_release;
        
        aParams.attack = a_attack;
        aParams.decay = a_decay;
        aParams.sustain = a_sustain;
        aParams.release = a_release;
        
        f_adsr.setParameters(fParams);
        a_adsr.setParameters(aParams);
    }

    void start_adsr(){
        //start attack
        f_adsr.noteOn();
        a_adsr.noteOn();
        
        //wait 100ms
        juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + 1000);
        
        //start release
        f_adsr.noteOff();
        a_adsr.noteOff();
        
    }

    float synth(double currentAngle,int currentSample){
        
        //calculate gain from final gain  slider
        auto gain_total = juce::Decibels::decibelsToGain(a_total_gain);
        
        //generate audio  from oscillator
        auto audio = osc(currentAngle,currentSample);
        
        //apply filter1
//        audio = filter1.processSingleSampleRaw(audio);
        
        //apply filter2
//        audio = filter2.processSingleSampleRaw(audio);
        
        //add adsr
//        audio *= a_adsr.getNextSample();
        
        return (float) audio * gain_total;

    }

    float osc(double currentAngle,int currentSample){
        
//        auto gain1 = juce::Decibels::decibelsToGain(oMenu.osc1_gain.getValue());
//        auto gain2 = juce::Decibels::decibelsToGain(oMenu.osc2_gain.getValue());
//        auto gain3 = juce::Decibels::decibelsToGain(oMenu.osc3_gain.getValue());
        
        auto gain1 = juce::Decibels::decibelsToGain(synth_param.osc1_gain);
        auto gain2 = juce::Decibels::decibelsToGain(synth_param.osc2_gain);
        auto gain3 = juce::Decibels::decibelsToGain(synth_param.osc3_gain);
        
        float audio = 0;
        //add oscilator 1
        switch(synth_param.osc1_wavShape){
            case 1:
                audio += std::sin (currentAngle)*gain1;
                break;
            case 2:
                audio += (-2 / juce::MathConstants<double>::pi * std::atan(1 / std::tan(currentFrequency *juce::MathConstants<double>::pi* currentSample/ getSampleRate())))*gain1;
                break;
            case 3:
                audio += std::sin (currentAngle) > 0 ? 1*gain1 : -1*gain1;
                break;
            case 4:
                audio += (2 / juce::MathConstants<double>::pi * std::asin(std::sin(currentAngle)))*gain1;
                break;
            case 5:
                audio += (2*random.nextFloat()-1)*gain1;
        }
        
        
        //add oscilator 2
        switch(synth_param.osc2_wavShape){
            case 1:
                audio += std::sin (currentAngle)*gain2;
                break;
            case 2:
                audio += (-2 / juce::MathConstants<double>::pi * std::atan(1 / std::tan(currentFrequency *juce::MathConstants<double>::pi* currentSample/ getSampleRate())))*gain2;
                break;
            case 3:
                audio += std::sin (currentAngle) > 0 ? 1*gain2 : -1*gain2;
                break;
            case 4:
                audio += (2 / juce::MathConstants<double>::pi * std::asin(std::sin(currentAngle)))*gain2;
                break;
            case 5:
                audio += (2*random.nextFloat()-1)*gain2;
        }
        
        //add oscilator 3
        switch(synth_param.osc3_wavShape){
            case 1:
                audio += std::sin (currentAngle)*gain3;
                break;
            case 2:
                audio += (-2 / juce::MathConstants<double>::pi * std::atan(1 / std::tan(currentFrequency *juce::MathConstants<double>::pi* currentSample/ getSampleRate())))*gain3;
                break;
            case 3:
                audio += std::sin (currentAngle) > 0 ? 1*gain3 : -1*gain3;
                break;
            case 4:
                audio += (2 / juce::MathConstants<double>::pi * std::asin(std::sin(currentAngle)))*gain3;
                break;
            case 5:
                audio += (2*random.nextFloat()-1)*gain3;
        }
        return audio;
        
    }
    

    
private:
    //frequency variabls
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0, currentFrequency = 0.0;
    
    //loaded in synth parameters
    synth_parameters synth_param;
    
    //filter variables
    double cutoff_freq1 = 4000;
    double cutoff_freq2 = 4000;
    double resonance1 = 0;
    double resonance2 = 0;
    double f_attack = 0;
    double f_decay = 0;
    double f_sustain = 0;
    double f_release = 0;
    int filter1_type = 0;
    int filter2_type = 0;
    
    //IIR filter variables
    juce::IIRFilter filter1;
    juce::IIRFilter filter2;
    
    //amplifier variables
    double a_total_gain = 0;
    double a_attack = 0;
    double a_decay = 0;
    double a_sustain = 1;
    double a_release = 0;
    
    //ASDRs
    juce::ADSR f_adsr;
    juce::ADSR a_adsr;
    
    //random from juce
    juce::Random random;

};

#pragma once
