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
    synthVoice(synth_parameters param,int samplesPerBlock, int numChannels){
        
        synth_param = param;
        
        juce::dsp::ProcessSpec spec; // Delivers information to the DSP algorithm.
        
        // Three methods we need to implement to pass onto the DSP module
        spec.sampleRate = getSampleRate();
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = numChannels;
        
        
        convolve1.reset();
        convolve1.prepare(spec);
        
        convolve2.reset();
        convolve2.prepare(spec);
        
        convolve3.reset();
        convolve3.prepare(spec);
        
        //set sample rate of ADSR
        f_adsr.setSampleRate(getSampleRate());
        a_adsr.setSampleRate(getSampleRate());
        
        //update parameters
        updateFilter1Values();
        updateFilter2Values();
        updateEnvelopes();
        
        loadHRTF(synth_param.osc1_az,1);
        loadHRTF(synth_param.osc2_az,2);
        loadHRTF(synth_param.osc3_az,3);
        
        DBG("azi 1: " << synth_param.osc1_az);
        DBG("dist 1: " << synth_param.osc1_distance);
        DBG("azi 2: " << synth_param.osc2_az);
        DBG("dist 2: " << synth_param.osc2_distance);
        DBG("azi 3: " << synth_param.osc3_az);
        DBG("dist 3: " << synth_param.osc3_distance);

        
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
            
            //start ADSRs
            f_adsr.noteOn();
            a_adsr.noteOn();
            
        }
    
    void loadHRTF(int p, int osc) // p is parameter for HRTF, osc is choosing which osc
        {
            int dataSize1;
            const char * data1;
            int dataSize2;
            const char * data2;
            int dataSize3;
            const char * data3;
            
            // Comment:  We should ensure that p doesn't go above 359 by indexing
            int azi;
            
            if (osc == 1) {
                int azi1 = p; // Giving the value to a separate variable (in case you want to change values on another osc right after?)
                data1 = BinaryData::getNamedResource(BinaryData::namedResourceList[azi1], dataSize1);
                convolve1.loadImpulseResponse(data1, dataSize1, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                azi = azi1; // azi is returned to store the azimuth value for display
            }
            
            if (osc == 2) {
                int azi2 = p;
                data2 = BinaryData::getNamedResource(BinaryData::namedResourceList[azi2], dataSize2);
                convolve2.loadImpulseResponse(data2, dataSize2, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                azi = azi2;
            }
            
            if (osc == 3) {
                int azi3 = p;
                data3 = BinaryData::getNamedResource(BinaryData::namedResourceList[azi3], dataSize3);
                convolve3.loadImpulseResponse(data3, dataSize3, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                azi = azi3;
            }
            
    }
    
    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
            {
                
                juce::AudioBuffer<float> osc1(outputBuffer.getNumChannels(), numSamples);
                juce::AudioBuffer<float> osc2(outputBuffer.getNumChannels(), numSamples);
                juce::AudioBuffer<float> osc3(outputBuffer.getNumChannels(), numSamples);
                
                
                
                for(auto samp = 0; samp < numSamples; samp++){
                    for (auto chan = outputBuffer.getNumChannels(); --chan >= 0;){
                        osc1.setSample(chan,samp,osc(currentAngle,samp,1));
                        osc2.setSample(chan,samp,osc(currentAngle,samp,2));
                        osc3.setSample(chan,samp,osc(currentAngle,samp,3));
                        
                    }
                    currentAngle += angleDelta;
                    
                }
                
                
                juce::dsp::AudioBlock<float> block1(osc1);
                juce::dsp::ProcessContextReplacing<float> context1 (block1);
                convolve1.process(context1);
                
                juce::dsp::AudioBlock<float> block2(osc2);
                juce::dsp::ProcessContextReplacing<float> context2 (block2);
                convolve2.process(context2);
                
                juce::dsp::AudioBlock<float> block3(osc3);
                juce::dsp::ProcessContextReplacing<float> context3 (block3);
                convolve3.process(context3);
                
                for(auto samp = 0; samp < numSamples; samp++){
                    
                    //get next adsr sample
                    auto next_adsr = a_adsr.getNextSample();
                    
                    //add sample for each channel
                    for (auto chan = outputBuffer.getNumChannels(); --chan >= 0;){
                        
                        //get current cample from synth
                        auto currentSample= osc1.getSample(chan,samp) + osc2.getSample(chan,samp) + osc3.getSample(chan,samp);
                        
                        //apply adsr
                        currentSample *= next_adsr;
                        
                        //add to output buffer
                        outputBuffer.addSample (chan, samp, currentSample);
                    }
                    
                }
            }
    
    void stopNote (float velocity, bool allowTailOff) override
        {
            //turn off ADSRs
            f_adsr.noteOff();
            a_adsr.noteOff();

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
        
        fParams.attack = synth_param.amp_attack;
        fParams.decay = synth_param.amp_decay;
        fParams.sustain = synth_param.amp_sustain;
        fParams.release = synth_param.amp_release;
        
        aParams.attack = synth_param.filter_attack;
        aParams.decay = synth_param.filter_decay;
        aParams.sustain = synth_param.filter_sustain;
        aParams.release = synth_param.filter_release;
        
        f_adsr.setParameters(fParams);
        a_adsr.setParameters(aParams);
    }

    float synth(double currentAngle,int currentSample){
        
        //calculate gain from final gain  slider
        auto gain_total = juce::Decibels::decibelsToGain(a_total_gain);
        
        //generate audio  from oscillator
        auto audio = osc(currentAngle,currentSample,1);
        
        //apply filter1
//        audio = filter1.processSingleSampleRaw(audio);
        
        //apply filter2
//        audio = filter2.processSingleSampleRaw(audio);
        
        //add adsr
//        audio *= a_adsr.getNextSample();
        
        return (float) audio;

    }

    float osc(double currentAngle,int currentSample, int osc_sel){
        
//        auto gain1 = juce::Decibels::decibelsToGain(oMenu.osc1_gain.getValue());
//        auto gain2 = juce::Decibels::decibelsToGain(oMenu.osc2_gain.getValue());
//        auto gain3 = juce::Decibels::decibelsToGain(oMenu.osc3_gain.getValue());
        
        auto gain1 = juce::Decibels::decibelsToGain(synth_param.osc1_gain);
        auto gain2 = juce::Decibels::decibelsToGain(synth_param.osc2_gain);
        auto gain3 = juce::Decibels::decibelsToGain(synth_param.osc3_gain);
        
        float audio = 0;
        
        switch(osc_sel){
            case 1:
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
                return audio;
            case 2:
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
                return audio;
            case 3:
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
        
        return 0;
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
    
    // Convolution
    juce::dsp::Convolution convolve1;
    juce::dsp::Convolution convolve2;
    juce::dsp::Convolution convolve3;

};

#pragma once
