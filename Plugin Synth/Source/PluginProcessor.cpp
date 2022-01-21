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
                       )
                    #endif
                    , param(*this, nullptr, "Parameters", addVSTParam())
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

juce::AudioProcessorValueTreeState::ParameterLayout PluginSynthAudioProcessor::addVSTParam()
{
    //choices to pass in when initializing variables
    auto wavChoices = {"","Sin", "Saw", "Square", "Triangle", "Noise"};
    auto filterChoices = {"LowPass", "HighPass", "BandPass", "BandReject"};
    
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;
    
    parameters.add( std::make_unique<juce::AudioParameterChoice>("osc1wavShape", "Oscillator 1 Wav Shape", wavChoices, 1));
    parameters.add( std::make_unique<juce::AudioParameterChoice>("osc2wavShape", "Oscillator 2 Wav Shape", wavChoices, 1));
    parameters.add( std::make_unique<juce::AudioParameterChoice>("osc3wavShape", "Oscillator 3 Wav Shape", wavChoices, 1));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc1freqOff", "Oscillator 1 Frequency Offset", juce::NormalisableRange<float> (-100.0f, 100.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc2freqOff", "Oscillator 2 Frequency Offset", juce::NormalisableRange<float> (-100.0f, 100.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc3freqOff", "Oscillator 3 Frequency Offset", juce::NormalisableRange<float> (-100.0f, 100.0f), 0));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc1gain", "Oscillator 1 Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc2gain", "Oscillator 2 Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc3gain", "Oscillator 3 Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    
    parameters.add( std::make_unique<juce::AudioParameterInt>("osc1az", "Oscillator 1 Azimuth", 0, 359, 0));
    parameters.add( std::make_unique<juce::AudioParameterInt>("osc2az", "Oscillator 2 Azimuth", 0, 359, 0));
    parameters.add( std::make_unique<juce::AudioParameterInt>("osc3az", "Oscillator 3 Azimuth", 0, 359, 0));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc1dist", "Oscillator 1 Distance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc2dist", "Oscillator 2 Distance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("osc3dist", "Oscillator 3 Distance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    
    parameters.add( std::make_unique<juce::AudioParameterChoice>("filter1type", "Filter 1 Type", filterChoices, 1));
    parameters.add( std::make_unique<juce::AudioParameterChoice>("filter2type", "Filter 2 Type", filterChoices, 1));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filter1cuttoff", "Filter 1 Cuttoff Frequency", juce::NormalisableRange<float> (20.0f, 20000.0f), 20000.0f));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filter2cuttoff", "Filter 2 Cuttoff Frequency", juce::NormalisableRange<float> (20.0f, 20000.0f), 20000.0f));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filter1resonance", "Filter 1 Resonance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filter2resonance", "Filter 2 Resonance", juce::NormalisableRange<float> (0.0f, 1.0f), 0));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filterAttack", "Filter Attack", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filterDecay", "Filter Decay", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filterSustain", "Filter Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 1));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("filterRelease", "Filter Release", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("ampAttack", "Amplifier Attack", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("ampDecay", "Amplifier Decay", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("ampSustain", "Amplifier Sustain", juce::NormalisableRange<float> (0.0f, 1.0f), 1));
    parameters.add( std::make_unique<juce::AudioParameterFloat>("ampRelease", "Amplifier Release", juce::NormalisableRange<float> (0.0f, 5.0f), 0));
    
    parameters.add( std::make_unique<juce::AudioParameterFloat>("totalGain", "total Gain", juce::NormalisableRange<float> (-100.0f, 10.0f), 0));
    
    return parameters;

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
    
    auto state = param.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
            if (xmlState.get() != nullptr)
                if (xmlState->hasTagName (param.state.getType()))
                    param.replaceState (juce::ValueTree::fromXml (*xmlState));
}

void PluginSynthAudioProcessor::updateSyntheParameters(juce::AudioProcessorValueTreeState* p)
{
    //set synth parameters
    param.replaceState(p->copyState());
    
    //update parameters for each synthesizer voice
    for (auto i = 0; i < 4; ++i){
        auto voice = (synthVoice*)synth.getVoice(i);
        voice->updateParameters(&param);
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
