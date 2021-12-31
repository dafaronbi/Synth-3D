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
    
    synthVoice(){
        
        //set default waveshape
        auto& osc_ob1 = osc1.template get<oscIndex>();
        auto& osc_ob2 = osc2.template get<oscIndex>();
        auto& osc_ob3 = osc3.template get<oscIndex>();
        
        osc_ob1.initialise(osc_sin, 128);
        osc_ob2.initialise(osc_sin, 128);
        osc_ob3.initialise(osc_sin, 128);
        
        //set default values
        updateParameters(synth_param);
        
    }
    
    float cent_offset(float note, float cent){
        
        //calulate the frequency offset from note from the cent value
        auto offset = std::pow(2, cent /1200);
        
        return note*offset;
    }
    
    //oscillator functions
    static float osc_sin(float x){
        return std::sin (x);
    }

    static float osc_saw(float x){
        return juce::jmap<float>(x, (-juce::MathConstants<double>::pi), (juce::MathConstants<double>::pi), -1, 1);
    }

    static float osc_square(float x){
        return std::sin (x) > 0 ? 1 : -1;
    }

    static float osc_triangle(float x){
        return (2 / juce::MathConstants<double>::pi * std::asin(std::sin(x)));
    }

    static float osc_noise(float x){
        //random generator
        juce::Random random;
        return (2*random.nextFloat()-1);
    }
    
    void prepaterToPlay(int samplesPerBlock, int numChannels){
        //spec for using dsp
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = getSampleRate();
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = numChannels;
        
        //initilize dsp  devices
        osc1.reset();
        osc1.prepare(spec);
        osc2.reset();
        osc2.prepare(spec);
        osc3.reset();
        osc3.prepare(spec);
        
        filter_gain.reset();
        filter_gain.prepare(spec);
        
        //set sample rate of ADSR
        f_adsr.setSampleRate(getSampleRate());
        a_adsr.setSampleRate(getSampleRate());
    }
    
    void updateParameters(synth_parameters param){
        
        //set  class parameter variable
        synth_param = param;
        
        //update parameters
        updateGain();
        updateFilter1Values();
        updateFilter2Values();
        updateEnvelopes();
        updateFreqOff();
        
        //only update oscillator if waveshape changed
        if(prevShape1 != synth_param.osc1_wavShape || prevShape2 != synth_param.osc2_wavShape ||   prevShape3 != synth_param.osc3_wavShape ){
            
            //set new previous shapes
            prevShape1 = synth_param.osc1_wavShape;
            prevShape2 = synth_param.osc2_wavShape;
            prevShape3 = synth_param.osc3_wavShape;
            
            //update oscillator shapes
            updateOscillators();
        }
        
        loadHRTF(synth_param.osc1_az,1);
        loadHRTF(synth_param.osc2_az,2);
        loadHRTF(synth_param.osc3_az,3);
        
    }
    
    void updateFreqOff(){
        
        //set oscilator frequency
        auto& osc_ob1 = osc1.template get<oscIndex>();
        auto& osc_ob2 = osc2.template get<oscIndex>();
        auto& osc_ob3 = osc3.template get<oscIndex>();
        
        osc_ob1.setFrequency(cent_offset(currentFrequency, synth_param.osc1_freqOff), false);
        osc_ob2.setFrequency(cent_offset(currentFrequency, synth_param.osc2_freqOff), false);
        osc_ob3.setFrequency(cent_offset(currentFrequency, synth_param.osc3_freqOff), false);
    }
    
    void updateOscillators(){
        
        auto& osc_ob1 = osc1.template get<oscIndex>();
        auto& osc_ob2 = osc2.template get<oscIndex>();
        auto& osc_ob3 = osc3.template get<oscIndex>();
        
        switch(synth_param.osc1_wavShape){
            case 1:
                osc_ob1.initialise(osc_sin, 128);
                break;
            case 2:
                osc_ob1.initialise(osc_saw, 128);
                break;
            case 3:
                osc_ob1.initialise(osc_square, 128);
                break;
            case 4:
                osc_ob1.initialise(osc_triangle, 128);
                break;
            case 5:
                osc_ob1.initialise(osc_noise, 0);
                break;
        }
    
        switch(synth_param.osc2_wavShape){
            case 1:
                osc_ob2.initialise(osc_sin, 128);
                break;
            case 2:
                osc_ob2.initialise(osc_saw, 128);
                break;
            case 3:
                osc_ob2.initialise(osc_square, 128);
                break;
            case 4:
                osc_ob2.initialise(osc_triangle, 128);
                break;
            case 5:
                osc_ob2.initialise(osc_noise, 0);
                break;
        }
    
        switch(synth_param.osc3_wavShape){
            case 1:
                osc_ob3.initialise(osc_sin, 128);
                break;
            case 2:
                osc_ob3.initialise(osc_saw, 128);
                break;
            case 3:
                osc_ob3.initialise(osc_square, 128);
                break;
            case 4:
                osc_ob3.initialise(osc_triangle, 128);
                break;
            case 5:
                osc_ob3.initialise(osc_noise, 0);
                break;
        }
        
    }
    
    void updateGain(){

        //get knob gain
        auto& gain_ob1 = osc1.template get<gainIndex>();
        auto& gain_ob2 = osc2.template get<gainIndex>();
        auto& gain_ob3 = osc3.template get<gainIndex>();

        //get distance gain
        auto& dis_ob1 = osc1.template get<disIndex>();
        auto& dis_ob2 = osc2.template get<disIndex>();
        auto& dis_ob3 = osc3.template get<disIndex>();

        //set knob gain
        gain_ob1.setGainDecibels(synth_param.osc1_gain);
        gain_ob2.setGainDecibels(synth_param.osc2_gain);
        gain_ob3.setGainDecibels(synth_param.osc3_gain);

        //set distance gain
        dis_ob1.setGainLinear((1-synth_param.osc1_distance)/2 + 0.5);
        dis_ob2.setGainLinear((1-synth_param.osc1_distance)/2 + 0.5);
        dis_ob3.setGainLinear((1-synth_param.osc1_distance)/2 + 0.5);
        
        //get totalgain from processor chain
        auto& totalGain_ob = filter_gain.template get<totalGainIndex>();
        
        //set total gain
        totalGain_ob.setGainDecibels(synth_param.total_gain);
        
    }
    
    bool canPlaySound (juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<synthSound*> (sound) != nullptr;
        }
    
    void startNote (int midiNoteNumber, float velocity,
                        juce::SynthesiserSound*, int currentPitchWheelPosition) override
        {
     
            currentFrequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
            
            //set oscilator frequency
            auto& osc_ob1 = osc1.template get<oscIndex>();
            auto& osc_ob2 = osc2.template get<oscIndex>();
            auto& osc_ob3 = osc3.template get<oscIndex>();
            
            osc_ob1.setFrequency(cent_offset(currentFrequency, synth_param.osc1_freqOff), false);
            osc_ob2.setFrequency(cent_offset(currentFrequency, synth_param.osc2_freqOff), false);
            osc_ob3.setFrequency(cent_offset(currentFrequency, synth_param.osc3_freqOff), false);
            
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
            
            auto& conv_ob1 = osc1.template get<convIndex>();
            auto& conv_ob2 = osc2.template get<convIndex>();
            auto& conv_ob3 = osc3.template get<convIndex>();
            
            // Comment:  We should ensure that p doesn't go above 359 by indexing
            int azi;
            
            if (osc == 1) {
                int azi1 = p; // Giving the value to a separate variable (in case you want to change values on another osc right after?)
                data1 = BinaryData::getNamedResource(BinaryData::namedResourceList[azi1], dataSize1);
                conv_ob1.loadImpulseResponse(data1, dataSize1, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                azi = azi1; // azi is returned to store the azimuth value for display
            }
            
            if (osc == 2) {
                int azi2 = p;
                data2 = BinaryData::getNamedResource(BinaryData::namedResourceList[azi2], dataSize2);
                conv_ob2.loadImpulseResponse(data2, dataSize2, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                azi = azi2;
            }
            
            if (osc == 3) {
                int azi3 = p;
                data3 = BinaryData::getNamedResource(BinaryData::namedResourceList[azi3], dataSize3);
                conv_ob3.loadImpulseResponse(data3, dataSize3, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                azi = azi3;
            }
            
    }
    
    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
            {
                //make new buffer for each oscillator
                juce::AudioBuffer<float> osc1_buf(outputBuffer.getNumChannels(), numSamples);
                juce::AudioBuffer<float> osc2_buf(outputBuffer.getNumChannels(), numSamples);
                juce::AudioBuffer<float> osc3_buf(outputBuffer.getNumChannels(), numSamples);
                
                //make a buffer for combined audio
                juce::AudioBuffer<float> combined(outputBuffer.getNumChannels(), numSamples);
                
                //clear new buffers
                osc1_buf.clear();
                osc2_buf.clear();
                osc3_buf.clear();
                
                combined.clear();
                
                //generate oscillator 1
                juce::dsp::AudioBlock<float> block1(osc1_buf);
                juce::dsp::ProcessContextReplacing<float> context1 (block1);
                osc1.process(context1);
                
                //generate oscillator 2
                juce::dsp::AudioBlock<float> block2(osc2_buf);
                juce::dsp::ProcessContextReplacing<float> context2 (block2);
                osc2.process(context2);
                
                //generate oscillator 3
                juce::dsp::AudioBlock<float> block3(osc3_buf);
                juce::dsp::ProcessContextReplacing<float> context3 (block3);
                osc3.process(context3);
                
                //add sample for each channel
                for (auto chan = outputBuffer.getNumChannels(); --chan >= 0;){
                    combined.addFrom(chan, 0, osc1_buf, chan, 0, numSamples);
                    combined.addFrom(chan, 0, osc2_buf, chan, 0, numSamples);
                    combined.addFrom(chan, 0, osc3_buf, chan, 0, numSamples);
                }
                
                //apply filter and gain
                juce::dsp::AudioBlock<float> blockComb(combined);
                juce::dsp::ProcessContextReplacing<float> contextComb (blockComb);
                filter_gain.process(contextComb);

                //apply adsr
                a_adsr.applyEnvelopeToBuffer(combined, 0, numSamples);
                
                for(auto samp = 0; samp < numSamples; samp++){

                    //add sample for each channel
                    for (auto chan = outputBuffer.getNumChannels(); --chan >= 0;){

                        //add to output buffer
                        outputBuffer.addSample (chan, samp, combined.getSample(chan,samp));
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
        
        //get filter from processor chain
        auto& filter1_ob = filter_gain.template get<filter1Index>();
        
        
        //select filter type and get coefficients
        switch(synth_param.filter1_type){
            case 1:
                filter1_ob.setMode(juce::dsp::LadderFilterMode::LPF12);
                filter1.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), synth_param.filter1_cuttoff));
                break;
            case 2:
                filter1_ob.setMode(juce::dsp::LadderFilterMode::HPF12);
                filter1.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), synth_param.filter1_cuttoff));
                break;
            case 3:
                filter1_ob.setMode(juce::dsp::LadderFilterMode::BPF12);
                filter1.setCoefficients(juce::IIRCoefficients::makeBandPass(getSampleRate(), synth_param.filter1_cuttoff));
                break;
            case 4:
                filter1_ob.setMode(juce::dsp::LadderFilterMode::BPF12);
                filter1.setCoefficients(juce::IIRCoefficients::makeNotchFilter(getSampleRate(), synth_param.filter1_cuttoff));
                break;
            default:
                filter1_ob.setMode(juce::dsp::LadderFilterMode::LPF12);
                filter1.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), synth_param.filter1_cuttoff));
            
        }
        
        //set cuttoff frequency and resonance
        filter1_ob.setCutoffFrequencyHz(synth_param.filter1_cuttoff);
        filter1_ob.setResonance(synth_param.filter1_resonance);
        
        filter1.reset();
    }

    void updateFilter2Values(){
        
        //get filter from processor chain
        auto& filter2_ob = filter_gain.template get<filter1Index>();
        
        //select filter type and get coefficients
        switch(synth_param.filter2_type){
            case 1:
                filter2_ob.setMode(juce::dsp::LadderFilterMode::LPF12);
                filter2.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), synth_param.filter2_cuttoff));
                break;
            case 2:
                filter2_ob.setMode(juce::dsp::LadderFilterMode::HPF12);
                filter2.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), synth_param.filter2_cuttoff));
                break;
            case 3:
                filter2_ob.setMode(juce::dsp::LadderFilterMode::BPF12);
                filter2.setCoefficients(juce::IIRCoefficients::makeBandPass(getSampleRate(), synth_param.filter2_cuttoff));
                break;
            case 4:
                filter2_ob.setMode(juce::dsp::LadderFilterMode::LPF12);
                filter2.setCoefficients(juce::IIRCoefficients::makeNotchFilter(getSampleRate(), synth_param.filter2_cuttoff));
                break;
            default:
                filter2_ob.setMode(juce::dsp::LadderFilterMode::LPF12);
                filter2.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), synth_param.filter2_cuttoff));
        }
        
        //set cuttoff frequency and resonance
        filter2_ob.setCutoffFrequencyHz(synth_param.filter2_cuttoff);
        filter2_ob.setResonance(synth_param.filter2_resonance);
            
        filter2.reset();
    }

    void updateEnvelopes(){
        
        //make parameter objects  with new parameter values
        juce::ADSR::Parameters fParams(synth_param.amp_attack,synth_param.amp_decay, synth_param.amp_sustain,synth_param.amp_release);
        juce::ADSR::Parameters aParams(synth_param.amp_attack,synth_param.amp_decay, synth_param.amp_sustain,synth_param.amp_release);
        
        f_adsr.setParameters(fParams);
        a_adsr.setParameters(aParams);
    }
    

    
private:
    //frequency variabls
    double currentFrequency = 0.0;
    
    //loaded in synth parameters
    synth_parameters synth_param;
    
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
    
    //previous osc values
    int prevShape1 = 1;
    int prevShape2 = 1;
    int prevShape3 = 1;
    
    //Oscillator Process chains
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Convolution, juce::dsp::Gain<float>, juce::dsp::Gain<float>> osc1;
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Convolution, juce::dsp::Gain<float>, juce::dsp::Gain<float>> osc2;
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Convolution, juce::dsp::Gain<float>, juce::dsp::Gain<float>> osc3;
    
    //enum to get each processor chain value
    enum{
        oscIndex,
        convIndex,
        gainIndex,
        disIndex,
    };
    
    //Filter and gain processing
    juce::dsp::ProcessorChain<juce::dsp::LadderFilter<float>, juce::dsp::LadderFilter<float>, juce::dsp::Gain<float>> filter_gain;
    
    //enum to get each processor chain value
    enum{
        filter1Index,
        filter2Index,
        totalGainIndex
    };
    
};

#pragma once
