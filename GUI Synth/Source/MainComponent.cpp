#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    //add child components
    addChildComponent(oMenu);
    addChildComponent(fMenu);
    addChildComponent(aMenu);
    
    addAndMakeVisible(nBar);
    addAndMakeVisible(save_button);
    addAndMakeVisible(test_audio);
    
    //only oscilattior menu is on by default
    oMenu.setVisible(true);
    
    //add listeners to sub-components
    nBar.addChangeListener(this);
    oMenu.addChangeListener(this);
    fMenu.addChangeListener(this);
    aMenu.addChangeListener(this);
    
    //configure button text
    save_button.setButtonText ("Save Sample");
    test_audio.setButtonText("Test Audio");
    
    //add listener for button
    save_button.addListener(this);
    test_audio.addListener(this);
    
    //set default window size
    setSize(800, 600);
    
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

    // Specify the number of input and output channels that we want to open
    setAudioChannels (0, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    nBar.removeChangeListener(this);
    save_button.removeListener(this);
    test_audio.removeListener(this);
    oMenu.removeChangeListener(this);
    fMenu.removeChangeListener(this);
    aMenu.removeChangeListener(this);
    
    //end audio
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    juce::String message;
    message << "Preparing to play audio...\n";
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
    message << " sampleRate = " << sampleRate;
    juce::Logger::getCurrentLogger()->writeToLog (message);
    sample_rate = sampleRate;
    
    //update filteer settings
    updateFilter1Values();
    updateFilter2Values();
    
    //update envelope settings
    updateEnvelopes();
    
    //set sample rates of adsr
    f_adsr.setSampleRate(sample_rate);
    a_adsr.setSampleRate(sample_rate);
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    //play audio when button is pressed
    if(audible){

        // Get a pointer to the start sample in the buffer for this audio output channel
        auto* leftBuffer  = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        auto* rightBuffer = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);

        // Fill the required number of samples with noise between -0.125 and +0.125
        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample){
            auto currentSample = synth();
            leftBuffer[sample] = currentSample;
            rightBuffer[sample] = currentSample;
        }
        
//        filter1.processSamples(leftBuffer, bufferToFill.numSamples);
//        filter1.processSamples(rightBuffer, bufferToFill.numSamples);
    }
    else{
        
        bufferToFill.clearActiveBufferRegion();
        
    }
    
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::black);

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    //get the local area
    auto area = getLocalBounds();
    
    auto nav_width = getWidth() < 160 ? getWidth() /3 : 160;
    
    //set areas of things
    nBar.setBounds(area.removeFromLeft(nav_width));
    auto button_area = area.removeFromBottom(getHeight()/15);
    save_button.setBounds(button_area.removeFromRight(button_area.getWidth()/2));
    test_audio.setBounds(button_area);
    fMenu.setBounds(area);
    oMenu.setBounds(area);
    aMenu.setBounds(area);
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
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
        
        if(source == &oMenu){
            currentFrequency1 = oMenu.osc1_frequency.getValue();
            currentFrequency2 = oMenu.osc2_frequency.getValue();
            currentFrequency3 = oMenu.osc3_frequency.getValue();
            
            updateAngle1Delta();
            updateAngle2Delta();
            updateAngle3Delta();
            
            
        }
        
        if(source == &fMenu){
            //update variables of synth parameters
            cutoff_freq1 = fMenu.filter1_cuttoff_freq.getValue();
            cutoff_freq2 =  fMenu.filter2_cuttoff_freq.getValue();
            resonance1 =  fMenu.filter1_resonance.getValue();
            resonance2 = fMenu.filter2_resonance.getValue();
            f_attack = fMenu.filter_attack.getValue();
            f_decay = fMenu.filter_decay.getValue();
            f_sustain = fMenu.filter_sustain.getValue();
            f_release = fMenu.filter_release.getValue();
            filter1_type = fMenu.filter1_type.getSelectedId();
            filter2_type = fMenu.filter2_type.getSelectedId();
            
            //update dsp values
            updateFilter1Values();
            updateFilter2Values();
            updateEnvelopes();
        }
        
        if(source == &aMenu){
            //update variables of synth parameters
            a_total_gain = aMenu.amp_total_gain.getValue();
            a_attack = aMenu.amp_attack.getValue();
            a_decay = aMenu.amp_decay.getValue();
            a_sustain = aMenu.amp_sustain.getValue();
            a_release = aMenu.amp_release.getValue();
            
            //update dsp values
            updateEnvelopes();
        }
        
    }

void MainComponent::buttonClicked (juce::Button* button)
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
            
            if (myChooser.browseForFileToSave(true))
                {
                    
                    save_directory = myChooser.getResult();
                    
                    
                    //10 second buffer initialize
                    juce::AudioBuffer<float> write_buffer;
                    write_buffer.setSize(2,sample_rate*10);
                    
                    // Get a pointer to the start sample in the buffer for this audio output channel
                    auto* leftBuffer  = write_buffer.getWritePointer (0, 0);
                    auto* rightBuffer = write_buffer.getWritePointer (1, 0);
                    
                    //start envelope attack
                    f_adsr.noteOn();
                    a_adsr.noteOn();

                    // Fill the required number of samples with noise between -0.125 and +0.125
                    for (auto sample = 0; sample < write_buffer.getNumSamples()/2; ++sample){
                        auto currentSample = synth();
                        leftBuffer[sample] = currentSample;
                        rightBuffer[sample] = currentSample;
                    }
                    
                    //start envelope release
                    f_adsr.noteOff();
                    a_adsr.noteOff();
                    
                    for (auto sample = write_buffer.getNumSamples()/2; sample < write_buffer.getNumSamples(); ++sample){
                        auto currentSample = synth();
                        leftBuffer[sample] = currentSample;
                        rightBuffer[sample] = currentSample;
                    }
        
                    juce::WavAudioFormat format;
                    std::unique_ptr<juce::AudioFormatWriter> writer;
                    writer.reset (format.createWriterFor (new juce::FileOutputStream (save_directory),
                                                          (int)sample_rate,
                                                          write_buffer.getNumChannels(),
                                                          24,
                                                          {},
                                                          0));
                    if (writer != nullptr)
                        writer->writeFromAudioSampleBuffer (write_buffer, 0, write_buffer.getNumSamples());
                    
                }
            
        }
    }
    

    void MainComponent::updateAngle1Delta()
        {
            auto cyclesPerSample = currentFrequency1 / sample_rate;         // [2]
            angleDelta1 = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
        }

    void MainComponent::updateAngle2Delta()
    {
        auto cyclesPerSample = currentFrequency2 / sample_rate;         // [2]
        angleDelta2 = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
    }

    void MainComponent::updateAngle3Delta()
    {
        auto cyclesPerSample = currentFrequency3 / sample_rate;         // [2]
        angleDelta3 = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
    }

    void MainComponent::updateFilter1Values(){
        
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

    void MainComponent::updateFilter2Values(){
        
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

    void MainComponent::updateEnvelopes(){
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

    void MainComponent::start_adsr(){
        //start attack
        f_adsr.noteOn();
        a_adsr.noteOn();
        
        //wait 100ms
        juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + 1000);
        
        //start release
        f_adsr.noteOff();
        a_adsr.noteOff();
        
    }

    float MainComponent::synth(){
        
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

    float MainComponent::osc(){
        
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

