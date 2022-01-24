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
    
    //update components  based on parameter values
    updateParameters();
    
    //timer call back every tenth of a second
    startTimer (10);


    //initialize filter variables
    cutoff_freq1 = fMenu.filter1_cuttoff_freq.getValue();
    cutoff_freq2 =  fMenu.filter2_cuttoff_freq.getValue();
    resonance1 =  fMenu.filter1_resonance.getValue();
    resonance2 = fMenu.filter2_resonance.getValue();
    f_attack = fMenu.filter_attack.getValue();
    f_decay = fMenu.filter_decay.getValue();
    f_sustain = fMenu.filter_sustain.getValue();
    f_release = fMenu.filter_release.getValue();
    

}

void  PluginSynthAudioProcessorEditor::timerCallback (){
    
    //update components  based on parameter values
    updateParameters();
    audioProcessor.updateSyntheParameters();
    
}

PluginSynthAudioProcessorEditor::~PluginSynthAudioProcessorEditor()
{
    nBar.removeChangeListener(this);
    oMenu.removeChangeListener(this);
    fMenu.removeChangeListener(this);
    aMenu.removeChangeListener(this);
}

void PluginSynthAudioProcessorEditor::updateParameters()
{
    //get the parameters from the audio processor
    juce::AudioProcessorValueTreeState* param = &audioProcessor.param;
    
    //set oscillator values
    oMenu.osc1_wav_shape.setSelectedId(*param->getRawParameterValue("osc1wavShape"));
    oMenu.osc2_wav_shape.setSelectedId(*param->getRawParameterValue("osc2wavShape"));
    oMenu.osc3_wav_shape.setSelectedId(*param->getRawParameterValue("osc3wavShape"));
    
    oMenu.osc1_frequency.setValue(*param->getRawParameterValue("osc1freqOff"));
    oMenu.osc2_frequency.setValue(*param->getRawParameterValue("osc2freqOff"));
    oMenu.osc3_frequency.setValue(*param->getRawParameterValue("osc3freqOff"));
    
    oMenu.osc1_gain.setValue(*param->getRawParameterValue("osc1gain"));
    oMenu.osc2_gain.setValue(*param->getRawParameterValue("osc2gain"));
    oMenu.osc3_gain.setValue(*param->getRawParameterValue("osc3gain"));
    
    oMenu.osc1_pan.setPosition(*param->getRawParameterValue("osc1az"), *param->getRawParameterValue("osc1distance"));
    oMenu.osc2_pan.setPosition(*param->getRawParameterValue("osc2az"), *param->getRawParameterValue("osc2distance"));
    oMenu.osc3_pan.setPosition(*param->getRawParameterValue("osc3az"), *param->getRawParameterValue("osc3distance"));
    
    //set filter values
    fMenu.filter1_type.setSelectedId(*param->getRawParameterValue("filter1type"));
    fMenu.filter2_type.setSelectedId(*param->getRawParameterValue("filter2type"));
    
    fMenu.filter1_cuttoff_freq.setValue(*param->getRawParameterValue("filter1cuttoff"));
    fMenu.filter2_cuttoff_freq.setValue(*param->getRawParameterValue("filter2cuttoff"));
    
    fMenu.filter1_resonance.setValue(*param->getRawParameterValue("filter1resonance"));
    fMenu.filter2_resonance.setValue(*param->getRawParameterValue("filter2resonance"));
    
    fMenu.filter_attack.setValue(*param->getRawParameterValue("filterAttack"));
    fMenu.filter_decay.setValue(*param->getRawParameterValue("filterDecay"));
    fMenu.filter_sustain.setValue(*param->getRawParameterValue("filterSustain"));
    fMenu.filter_release.setValue(*param->getRawParameterValue("filterRelease"));
    
    //set amplifier values
    aMenu.amp_total_gain.setValue(*param->getRawParameterValue("totalGain"));
    
    aMenu.amp_attack.setValue(*param->getRawParameterValue("ampAttack"));
    aMenu.amp_decay.setValue(*param->getRawParameterValue("ampDecay"));
    aMenu.amp_sustain.setValue(*param->getRawParameterValue("ampSustain"));
    aMenu.amp_release.setValue(*param->getRawParameterValue("ampRelease"));
    
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
    
        //get the parameters from the audio processor
        juce::AudioProcessorValueTreeState* param = &audioProcessor.param;
    

        //set parameters to be passed into synth
        if(source == &oMenu){
            
            *param->getRawParameterValue("osc1wavShape") = oMenu.osc1_wav_shape.getSelectedId();
            *param->getRawParameterValue("osc2wavShape") = oMenu.osc2_wav_shape.getSelectedId();
            *param->getRawParameterValue("osc3wavShape") = oMenu.osc3_wav_shape.getSelectedId();
            
            *param->getRawParameterValue("osc1freqOff") = oMenu.osc1_frequency.getValue();
            *param->getRawParameterValue("osc2freqOff") = oMenu.osc2_frequency.getValue();
            *param->getRawParameterValue("osc3freqOff") = oMenu.osc3_frequency.getValue();
            
            *param->getRawParameterValue("osc1gain") = oMenu.osc1_gain.getValue();
            *param->getRawParameterValue("osc2gain") = oMenu.osc2_gain.getValue();
            *param->getRawParameterValue("osc3gain") =  oMenu.osc3_gain.getValue();
            
            *param->getRawParameterValue("osc1az") = oMenu.osc1_pan.getAzimuth();
            *param->getRawParameterValue("osc2az") = oMenu.osc2_pan.getAzimuth();
            *param->getRawParameterValue("osc3az") = oMenu.osc3_pan.getAzimuth();
            
            *param->getRawParameterValue("osc1distance") = oMenu.osc1_pan.getDistance();
            *param->getRawParameterValue("osc2distance") = oMenu.osc2_pan.getDistance();
            *param->getRawParameterValue("osc3distance") = oMenu.osc3_pan.getDistance();
            
            
        }
        
        if(source == &fMenu){
        
            *param->getRawParameterValue("filter1type") = fMenu.filter1_type.getSelectedId();
            *param->getRawParameterValue("filter2type") = fMenu.filter2_type.getSelectedId();
            
            *param->getRawParameterValue("filter1cuttoff") = fMenu.filter1_cuttoff_freq.getValue();
            *param->getRawParameterValue("filter2cuttoff") = fMenu.filter2_cuttoff_freq.getValue();
            
            *param->getRawParameterValue("filter1resonance") = fMenu.filter1_resonance.getValue();
            *param->getRawParameterValue("filter2resonance")  = fMenu.filter2_resonance.getValue();
            
            *param->getRawParameterValue("filterAttack") = fMenu.filter_attack.getValue();
            *param->getRawParameterValue("filterDecay") = fMenu.filter_decay.getValue();
            *param->getRawParameterValue("filterSustain") = fMenu.filter_sustain.getValue();
            *param->getRawParameterValue("filterRelease") = fMenu.filter_release.getValue();
        }
        
        if(source == &aMenu){
            
            *param->getRawParameterValue("totalGain")  = aMenu.amp_total_gain.getValue();
            
            *param->getRawParameterValue("ampAttack") = aMenu.amp_attack.getValue();
            *param->getRawParameterValue("ampDecay") = aMenu.amp_decay.getValue();
            *param->getRawParameterValue("ampSustain") = aMenu.amp_sustain.getValue();
            *param->getRawParameterValue("ampRelease") = aMenu.amp_release.getValue();
            
        }
    
    audioProcessor.updateSyntheParameters();
        
    }







