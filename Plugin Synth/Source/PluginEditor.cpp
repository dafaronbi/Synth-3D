/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginSynthAudioProcessorEditor::PluginSynthAudioProcessorEditor (PluginSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    
    //add child components
    addChildComponent(oMenu);
    addChildComponent(fMenu);
    addChildComponent(aMenu);
    
    //initialize ceenters

    addAndMakeVisible(nBar);

    //only oscilattior menu is on by default
    oMenu.setVisible(true);

    //add listeners to sub-components
    nBar.addChangeListener(this);
    oMenu.addChangeListener(this);
    fMenu.addChangeListener(this);
    aMenu.addChangeListener(this);

    //set default window size
    setSize(500, 500);
    setResizable(false, false);

    //current Angle at begining of phase
    currentAngle1 = 0;
    currentAngle2 = 0;
    currentAngle3 = 0;

    //current Sample at begining
    currentSample1 = 0;
    currentSample2 = 0;
    currentSample3 = 0;


    //initialize filter variables
    cutoff_freq1 = fMenu.filter1_cuttoff_freq.getValue();
    cutoff_freq2 =  fMenu.filter2_cuttoff_freq.getValue();
    resonance1 =  fMenu.filter1_resonance.getValue();
    resonance2 = fMenu.filter2_resonance.getValue();
    f_attack = fMenu.filter_attack.getValue();
    f_decay = fMenu.filter_decay.getValue();
    f_sustain = fMenu.filter_sustain.getValue();
    f_release = fMenu.filter_release.getValue();

    //silent at first
    audible = false;

}

PluginSynthAudioProcessorEditor::~PluginSynthAudioProcessorEditor()
{
    nBar.removeChangeListener(this);
    oMenu.removeChangeListener(this);
    fMenu.removeChangeListener(this);
    aMenu.removeChangeListener(this);
}

//==============================================================================
void PluginSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("", getLocalBounds(), juce::Justification::centred, 1);
}

void PluginSynthAudioProcessorEditor::resized()
{
    //get the local area
    auto area = getLocalBounds();

    auto nav_width = getWidth() < 160 ? getWidth() /3 : 160;

    //set areas of things
    nBar.setBounds(area.removeFromLeft(nav_width));    fMenu.setBounds(area);
    oMenu.setBounds(area);
    aMenu.setBounds(area);
}

void PluginSynthAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
    {
        if(source == &nBar){
            oMenu.setVisible(false);
            fMenu.setVisible(false);
            aMenu.setVisible(false);
            
            switch(nBar.get_button_click()){
                case 0:
                    oMenu.setVisible(true);
                    break;
                case 1:
                    fMenu.setVisible(true);
                    break;
                case 2:
                    aMenu.setVisible(true);
                    break;
            };
            repaint();
        }
    

        //set parameters to be passed into synth
        if(source == &oMenu){
            
            param.osc1_wavShape = oMenu.osc1_wav_shape.getSelectedId();
            param.osc2_wavShape = oMenu.osc2_wav_shape.getSelectedId();
            param.osc3_wavShape = oMenu.osc3_wav_shape.getSelectedId();
            
            param.osc1_gain = oMenu.osc1_gain.getValue();
            param.osc2_gain = oMenu.osc2_gain.getValue();
            param.osc3_gain =  oMenu.osc3_gain.getValue();
            
            param.osc1_az = oMenu.osc1_pan.getAzimuth();
            param.osc2_az = oMenu.osc2_pan.getAzimuth();
            param.osc3_az = oMenu.osc3_pan.getAzimuth();
            
            param.osc1_distance = oMenu.osc1_pan.getDistance();
            param.osc2_distance = oMenu.osc2_pan.getDistance();
            param.osc3_distance = oMenu.osc3_pan.getDistance();
            
            DBG("o1 az "<< param.osc1_az);
            DBG("o2 az "<< param.osc2_az);
            DBG("o3 az "<< param.osc3_az);
            DBG("o1 dist "<< param.osc1_distance);
            DBG("o2 dist "<< param.osc2_distance);
            DBG("o3 dist "<< param.osc3_distance);
            
            
        }
        
        if(source == &fMenu){
        
            param.filter1_type = fMenu.filter1_type.getSelectedId();
            param.filter2_type = fMenu.filter2_type.getSelectedId();
            
            param.filter1_cuttoff = fMenu.filter1_cuttoff_freq.getValue();
            param.filter2_cuttoff = fMenu.filter2_cuttoff_freq.getValue();
            
            param.filter1_resonance = fMenu.filter1_resonance.getValue();
            param.filter2_resonance  = fMenu.filter2_resonance.getValue();
            
            param.filter_attack = fMenu.filter_attack.getValue();
            param.filter_decay = fMenu.filter_decay.getValue();
            param.filter_sustain = fMenu.filter_sustain.getValue();
            param.filter_release = fMenu.filter_release.getValue();
        }
        
        if(source == &aMenu){
            
            param.total_gain  = aMenu.amp_total_gain.getValue();
            
            param.amp_attack = aMenu.amp_attack.getValue();
            param.amp_decay = aMenu.amp_decay.getValue();
            param.amp_sustain = aMenu.amp_sustain.getValue();
            param.amp_release = aMenu.amp_release.getValue();
        }
    
    audioProcessor.updateSyntheParameters(param);
        
    }

void PluginSynthAudioProcessorEditor::buttonClicked (juce::Button* button)
    {
        if (button == &test_audio)
        {
            //make audio audible
            audible = true;
            
            //make adsr run
            start_adsr();
            
            //wait 1 second
            juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + 1000);
            
            audible = false;
        }
        
        if(button == &save_button)
        {
            
            juce::FileChooser myChooser ("Select an output file",
                           juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                           "*.wav");
            
//            if (myChooser.browseForFileToSave(true))
//                {
//
//                    save_directory = myChooser.getResult();
//
//
//                    //10 second buffer initialize
//                    juce::AudioBuffer<float> write_buffer;
//                    write_buffer.setSize(2,sample_rate*10);
//
//                    // Get a pointer to the start sample in the buffer for this audio output channel
//                    auto* leftBuffer  = write_buffer.getWritePointer (0, 0);
//                    auto* rightBuffer = write_buffer.getWritePointer (1, 0);
//
//                    //start envelope attack
//                    f_adsr.noteOn();
//                    a_adsr.noteOn();
//
//                    // Fill the required number of samples with noise between -0.125 and +0.125
//                    for (auto sample = 0; sample < write_buffer.getNumSamples()/2; ++sample){
//                        auto currentSample = synth();
//                        leftBuffer[sample] = currentSample;
//                        rightBuffer[sample] = currentSample;
//                    }
//
//                    //start envelope release
//                    f_adsr.noteOff();
//                    a_adsr.noteOff();
//
//                    for (auto sample = write_buffer.getNumSamples()/2; sample < write_buffer.getNumSamples(); ++sample){
//                        auto currentSample = synth();
//                        leftBuffer[sample] = currentSample;
//                        rightBuffer[sample] = currentSample;
//                    }
//
//                    juce::WavAudioFormat format;
//                    std::unique_ptr<juce::AudioFormatWriter> writer;
//                    writer.reset (format.createWriterFor (new juce::FileOutputStream (save_directory),
//                                                          (int)sample_rate,
//                                                          write_buffer.getNumChannels(),
//                                                          24,
//                                                          {},
//                                                          0));
//                    if (writer != nullptr)
//                        writer->writeFromAudioSampleBuffer (write_buffer, 0, write_buffer.getNumSamples());
//
//                }
            
        }
    }

void PluginSynthAudioProcessorEditor::updateAngle1Delta()
        {
            auto cyclesPerSample = currentFrequency1 / sample_rate;         // [2]
            angleDelta1 = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
        }

    void PluginSynthAudioProcessorEditor::updateAngle2Delta()
    {
        auto cyclesPerSample = currentFrequency2 / sample_rate;         // [2]
        angleDelta2 = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
    }

    void PluginSynthAudioProcessorEditor::updateAngle3Delta()
    {
        auto cyclesPerSample = currentFrequency3 / sample_rate;         // [2]
        angleDelta3 = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
    }

    void PluginSynthAudioProcessorEditor::updateFilter1Values(){
        
        //get cuttoff frequency from envelope
        double cutoff = cutoff_freq1;
        
        //select filter type and get coefficients
        switch(filter1_type){
            case 1:
                filter1.setCoefficients(juce::IIRCoefficients::makeLowPass(sample_rate, cutoff));
                break;
            case 2:
                filter1.setCoefficients(juce::IIRCoefficients::makeHighPass(sample_rate, cutoff));
                break;
            case 3:
                filter1.setCoefficients(juce::IIRCoefficients::makeBandPass(sample_rate, cutoff));
                break;
            case 4:
                filter1.setCoefficients(juce::IIRCoefficients::makeNotchFilter(sample_rate, cutoff));
                break;
            default:
                filter1.setCoefficients(juce::IIRCoefficients::makeLowPass(sample_rate, cutoff));
            
        }
        
        filter1.reset();
    }

    void PluginSynthAudioProcessorEditor::updateFilter2Values(){
        
        //get cuttoff frequency from envelope
        double cutoff = cutoff_freq2;
        
        //select filter type and get coefficients
        switch(filter2_type){
            case 1:
                filter2.setCoefficients(juce::IIRCoefficients::makeLowPass(sample_rate, cutoff));
                break;
            case 2:
                filter2.setCoefficients(juce::IIRCoefficients::makeHighPass(sample_rate, cutoff));
                break;
            case 3:
                filter2.setCoefficients(juce::IIRCoefficients::makeBandPass(sample_rate, cutoff));
                break;
            case 4:
                filter2.setCoefficients(juce::IIRCoefficients::makeNotchFilter(sample_rate, cutoff));
                break;
            default:
                filter2.setCoefficients(juce::IIRCoefficients::makeLowPass(sample_rate, cutoff));
        }
            
        filter2.reset();
    }

    void PluginSynthAudioProcessorEditor::updateEnvelopes(){
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

    void PluginSynthAudioProcessorEditor::start_adsr(){
        //start attack
        f_adsr.noteOn();
        a_adsr.noteOn();
        
        //wait 100ms
        juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + 1000);
        
        //start release
        f_adsr.noteOff();
        a_adsr.noteOff();
        
    }

    float PluginSynthAudioProcessorEditor::synth(){
        
        //calculate gain from final gain  slider
        auto gain_total = juce::Decibels::decibelsToGain(a_total_gain);
        
        //generate audio  from oscillator
        auto audio = osc();
        
        //apply filter1
        audio = filter1.processSingleSampleRaw(audio);
        
        //apply filter2
        audio = filter2.processSingleSampleRaw(audio);
        
        //add adsr
        audio *= a_adsr.getNextSample();
        
        return (float) audio * gain_total;

    }

    float PluginSynthAudioProcessorEditor::osc(){
        
        //update current angles
        currentAngle1 += angleDelta1;
        currentAngle2 += angleDelta2;
        currentAngle3 += angleDelta3;
        
        //update current samples
        currentSample1++;
        currentSample2++;
        currentSample3++;
        
        auto gain1 = juce::Decibels::decibelsToGain(oMenu.osc1_gain.getValue());
        auto gain2 = juce::Decibels::decibelsToGain(oMenu.osc2_gain.getValue());
        auto gain3 = juce::Decibels::decibelsToGain(oMenu.osc3_gain.getValue());
        
        float audio = 0;
        //add oscilator 1
        switch(oMenu.osc1_wav_shape.getSelectedId()){
            case 1:
                audio += std::sin (currentAngle1)*gain1;
                break;
            case 2:
                audio += (-2 / juce::MathConstants<double>::pi * std::atan(1 / std::tan(currentFrequency1 *juce::MathConstants<double>::pi* currentSample1/sample_rate)))*gain1;
                break;
            case 3:
                audio += std::sin (currentAngle1) > 0 ? 1*gain1 : -1*gain1;
                break;
            case 4:
                audio += (2 / juce::MathConstants<double>::pi * std::asin(std::sin(currentAngle1)))*gain1;
                break;
            case 5:
                audio += (2*random.nextFloat()-1)*gain1;
        }
        
        
        //add oscilator 2
        switch(oMenu.osc2_wav_shape.getSelectedId()){
            case 1:
                audio += std::sin (currentAngle2)*gain2;
                break;
            case 2:
                audio += (-2 / juce::MathConstants<double>::pi * std::atan(1 / std::tan(currentFrequency2 *juce::MathConstants<double>::pi* currentSample2/sample_rate)))*gain2;
                break;
            case 3:
                audio += std::sin (currentAngle2) > 0 ? 1*gain2 : -1*gain2;
                break;
            case 4:
                audio += (2 / juce::MathConstants<double>::pi * std::asin(std::sin(currentAngle2)))*gain2;
                break;
            case 5:
                audio += (2*random.nextFloat()-1)*gain2;
        }
        
        //add oscilator 3
        switch(oMenu.osc3_wav_shape.getSelectedId()){
            case 1:
                audio += std::sin (currentAngle3)*gain3;
                break;
            case 2:
                audio += (-2 / juce::MathConstants<double>::pi * std::atan(1 / std::tan(currentFrequency3 *juce::MathConstants<double>::pi* currentSample3/sample_rate)))*gain3;
                break;
            case 3:
                audio += std::sin (currentAngle3) > 0 ? 1*gain3 : -1*gain3;
                break;
            case 4:
                audio += (2 / juce::MathConstants<double>::pi * std::asin(std::sin(currentAngle3)))*gain3;
                break;
            case 5:
                audio += (2*random.nextFloat()-1)*gain3;
        }
        return audio;
        
    }
