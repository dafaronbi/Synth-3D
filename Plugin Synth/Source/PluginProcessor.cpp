/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginSynthAudioProcessor::PluginSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "Parameters", addVSTParam())
{
    
    //add all parameters as available for vst
//    addVSTParam();
    
    //initialize synth voices and sound
    for (auto i = 0; i < 4; ++i)
        synth.addVoice (new synthVoice(param));

    synth.addSound (new synthSound());
    
    
}

PluginSynthAudioProcessor::~PluginSynthAudioProcessor()
{
}

void PluginSynthAudioProcessor::addVSTParam()
{
    //choices to pass in when initializing variables
    auto wavChoices = {"","Sin", "Saw", "Square", "Triangle", "Noise"};
    auto filterChoices = {"LowPass", "HighPass", "BandPass", "BandReject"};
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    parameters.push_back( new juce::AudioParameterChoice("osc1wavShape", "Oscillator 1 Wav Shape", wavChoices, 1));
    parameters.push_back( new juce::AudioParameterChoice("osc2wavShape", "Oscillator 2 Wav Shape", wavChoices, 1));
    parameters.push_back( new juce::AudioParameterChoice("osc3wavShape", "Oscillator 3 Wav Shape", wavChoices, 1));
    
    parameters.push_back( new juce::AudioParameterFloat("osc1freqOff", "Oscillator 1 Frequency Offset", juce::NormalisableRange<float> (-100.0f, 100.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("osc2freqOff", "Oscillator 2 Frequency Offset", juce::NormalisableRange<float> (-100.0f, 100.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("osc3freqOff", "Oscillator 3 Frequency Offset", juce::NormalisableRange<float> (-100.0f, 100.0f), 0));
    
    parameters.push_back( new juce::AudioParameterFloat("osc1gain", "Oscillator 1 Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    parameters.pushback( new juce::AudioParameterFloat("osc2gain", "Oscillator 2 Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    parameters.pushback( new juce::AudioParameterFloat("osc3gain", "Oscillator 3 Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    
    parameters.push_back( new juce::AudioParameterInt("osc1az", "Oscillator 1 Azimuth", 0, 359, 0));
    parameters.push_back( new juce::AudioParameterInt("osc2az", "Oscillator 2 Azimuth", 0, 359, 0));
    parameters.push_back( new juce::AudioParameterInt("osc3az", "Oscillator 3 Azimuth", 0, 359, 0));
    
    parameters.push_back( new juce::AudioParameterFloat("osc1dist", "Oscillator 1 Distance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("osc2dist", "Oscillator 2 Distance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    parameters.pushback( new juce::AudioParameterFloat("osc3dist", "Oscillator 3 Distance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    
    parameters.push_back( new juce::AudioParameterChoice("filter1type", "Filter 1 Type", filterChoices, 1));
    parameters.push_back( new juce::AudioParameterChoice("filter2type", "Filter 2 Type", filterChoices, 1));
    
    parameters.push_back( new juce::AudioParameterFloat("filter1cuttoff", "Filter 1 Cuttoff Frequency", juce::NormalisableRange<float> (20.0f, 20000.0f), 20000.0f));
    parameters.push_back( new juce::AudioParameterFloat("filter2cuttoff", "Filter 2 Cuttoff Frequency", juce::NormalisableRange<float> (20.0f, 20000.0f), 20000.0f));
    
    parameters.push_back( new juce::AudioParameterFloat("filter1resonance", "Filter 1 Resonance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("filter2resonance", "Filter 2 Resonance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    
    parameters.push_back( new juce::AudioParameterFloat("filterAttack", "Filter Attack", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("filterDecay", "Filter Decay", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("filterSustain", "Filter Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 1));
    parameters.push_back( new juce::AudioParameterFloat("filterRelease", "Filter Release", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    
    parameters.push_back( new juce::AudioParameterFloat("ampAttack", "Amplifier Attack", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("ampDecay", "Amplifier Decay", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.push_back( new juce::AudioParameterFloat("ampSustain", "Amplifier Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 1));
    parameters.push_back( new juce::AudioParameterFloat("ampRelease", "Amplifier Release", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    
    parameters.push_back( new juce::AudioParameterFloat("totalGain", "total Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    
    return {parameters.begin(), parameters.end()};

}

//==============================================================================
const juce::String PluginSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginSynthAudioProcessor::setCurrentProgram (int index)
{
    
}

const juce::String PluginSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //set synth sample rate
    synth.setCurrentPlaybackSampleRate (sampleRate);
    
    //set sample rate and number of channels for each synthesizer voice
    for (auto i = 0; i < 4; ++i){                // [1]
        auto voice = (synthVoice*)synth.getVoice(i);
        voice->prepaterToPlay(samplesPerBlock,getTotalNumOutputChannels());
    }
        
}


void PluginSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PluginSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    synth.renderNextBlock (buffer, midiMessages,
                           0, buffer.getNumSamples());
    
    

    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        // ..do something to the data...
    }
}

//==============================================================================
bool PluginSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginSynthAudioProcessor::createEditor()
{
    return new PluginSynthAudioProcessorEditor (*this);
}

//==============================================================================
void PluginSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("ParamTutorial"));
    xml->setAttribute ("osc1_wavShape", (int) *param.osc1_wavShape);
    xml->setAttribute ("osc2_wavShape", (int) *param.osc2_wavShape);
    xml->setAttribute ("osc3_wavShape", (int) *param.osc3_wavShape);
    xml->setAttribute ("osc1_freqOff", (float) *param.osc1_freqOff);
    xml->setAttribute ("osc2_freqOff", (float) *param.osc2_freqOff);
    xml->setAttribute ("osc3_freqOff", (float) *param.osc3_freqOff);
    xml->setAttribute ("osc1_gain", (float) *param.osc1_gain);
    xml->setAttribute ("osc2_gain", (float) *param.osc2_gain);
    xml->setAttribute ("osc3_gain", (float) *param.osc3_gain);
    xml->setAttribute ("osc1_az", (float) *param.osc1_az);
    xml->setAttribute ("osc2_az", (float) *param.osc2_az);
    xml->setAttribute ("osc3_az", (float) *param.osc3_az);
    xml->setAttribute ("osc1_distance", (float) *param.osc1_distance);
    xml->setAttribute ("osc2_distance", (float) *param.osc2_distance);
    xml->setAttribute ("osc3_distance", (float) *param.osc3_distance);
    xml->setAttribute ("filter1_type", (int) *param.filter1_type);
    xml->setAttribute ("filter2_type", (int) *param.filter2_type);
    xml->setAttribute ("filter1_cuttoff", (float) *param.filter1_cuttoff);
    xml->setAttribute ("filter2_cuttoff", (float) *param.filter2_cuttoff);
    xml->setAttribute ("filter1_resonance", (float) *param.filter1_resonance);
    xml->setAttribute ("filter2_resonance", (float) *param.filter2_resonance);
    xml->setAttribute ("filter_attack", (float) *param.filter_attack);
    xml->setAttribute ("filter_decay", (float) *param.filter_decay);
    xml->setAttribute ("filter_sustain", (float) *param.filter_sustain);
    xml->setAttribute ("filter_release", (float) *param.filter_release);
    xml->setAttribute ("amp_attack", (float) *param.amp_attack);
    xml->setAttribute ("amp_decay", (float) *param.amp_decay);
    xml->setAttribute ("amp_sustain", (float) *param.amp_sustain);
    xml->setAttribute ("amp_release", (float) *param.amp_release);
    xml->setAttribute ("total_gain", (float) *param.total_gain);
    copyXmlToBinary (*xml, destData);
}

void PluginSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
            if (xmlState.get() != nullptr)
            {
                if (xmlState->hasTagName ("ParamTutorial"))
                {
                    xmlState->getIntAttribute ("osc1_wavShape", 1);
                    xmlState->getIntAttribute ("osc2_wavShape", 1);
                    xmlState->getIntAttribute ("osc3_wavShape", 1);
                    xmlState->getDoubleAttribute ("osc1_freqOff", 0);
                    xmlState->getDoubleAttribute ("osc2_freqOff", 0);
                    xmlState->getDoubleAttribute ("osc3_freqOff", 0);
                    xmlState->getDoubleAttribute ("osc1_gain", 0);
                    xmlState->getDoubleAttribute ("osc2_gain", 0);
                    xmlState->getDoubleAttribute ("osc3_gain", 0);
                    xmlState->getDoubleAttribute ("osc1_az", 0);
                    xmlState->getDoubleAttribute ("osc2_az", 0);
                    xmlState->getDoubleAttribute ("osc3_az", 0);
                    xmlState->getDoubleAttribute ("osc1_distance", 0);
                    xmlState->getDoubleAttribute ("osc2_distance", 0);
                    xmlState->getDoubleAttribute ("osc3_distance", 0);
                    xmlState->getIntAttribute ("filter1_type", 0);
                    xmlState->getIntAttribute ("filter2_type", 0);
                    xmlState->getDoubleAttribute ("filter1_cuttoff", 1);
                    xmlState->getDoubleAttribute ("filter2_cuttoff", 1);
                    xmlState->getDoubleAttribute ("filter1_resonance", 0);
                    xmlState->getDoubleAttribute ("filter2_resonance", 0);
                    xmlState->getDoubleAttribute ("filter_attack", 0);
                    xmlState->getDoubleAttribute ("filter_decay", 0);
                    xmlState->getDoubleAttribute ("filter_sustain", 1);
                    xmlState->getDoubleAttribute ("filter_release", 0);
                    xmlState->getDoubleAttribute ("amp_attack", 0);
                    xmlState->getDoubleAttribute ("amp_decay", 0);
                    xmlState->getDoubleAttribute ("amp_sustain", 1);
                    xmlState->getDoubleAttribute ("amp_release", 0);
                    xmlState->getDoubleAttribute ("total_gain", 0);
                }
            }
}

void PluginSynthAudioProcessor::updateSyntheParameters(synth_parameters p)
{
    //set synth parameters
    param  = p;
    
    //update parameters for each synthesizer voice
    for (auto i = 0; i < 4; ++i){
        auto voice = (synthVoice*)synth.getVoice(i);
        voice->updateParameters(param);
    }
}

synth_parameters PluginSynthAudioProcessor::getParams()
{
    return param;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginSynthAudioProcessor();
}
