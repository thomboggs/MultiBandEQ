/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FilterHelperFunctions.h"
#include "CoefficientsMaker.h"



//==============================================================================
Pfmcpp_project10AudioProcessor::Pfmcpp_project10AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    FilterParameters fParams;
    HighCutLowCutParameters hclcParams;
        
    fParams.sampleRate = 44100;
    hclcParams.sampleRate = 44100;
    
    auto filterCoeffs = CoefficientsMaker::calcFilterCoefficients(fParams);
    auto hclcCoeffs = CoefficientsMaker::calcCutCoefficients(hclcParams);
    
    
}

Pfmcpp_project10AudioProcessor::~Pfmcpp_project10AudioProcessor()
{
}

//==============================================================================
const String Pfmcpp_project10AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Pfmcpp_project10AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Pfmcpp_project10AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Pfmcpp_project10AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Pfmcpp_project10AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Pfmcpp_project10AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Pfmcpp_project10AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Pfmcpp_project10AudioProcessor::setCurrentProgram (int index)
{
}

const String Pfmcpp_project10AudioProcessor::getProgramName (int index)
{
    return {};
}

void Pfmcpp_project10AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Pfmcpp_project10AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Pfmcpp_project10AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Pfmcpp_project10AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void Pfmcpp_project10AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//    }
}

//==============================================================================
bool Pfmcpp_project10AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Pfmcpp_project10AudioProcessor::createEditor()
{
//    return new Pfmcpp_project10AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Pfmcpp_project10AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Pfmcpp_project10AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout Pfmcpp_project10AudioProcessor::createParameterLayout ()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    const int numFilters = 4;
    
    for ( int i = 0; i < numFilters; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterBool>(getBypassParamName(i),
                                                              getBypassParamName(i),
                                                              false));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(getGainParamName(i),
                                                               getGainParamName(i),
                                                               juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                                   0.f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(getQualityParamName(i),
                                                               getQualityParamName(i),
                                                               juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                                   1.f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(getFreqParamName(i),
                                                               getFreqParamName(i),
                                                               juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                                   500.f));
        
        StringArray stringArray;
        auto filterTypeMap = FilterInfo::getFilterTypeMap();
        auto it = filterTypeMap.begin();
        
        while (it != filterTypeMap.end())
        {
//            DBG(it->second);
            stringArray.add(it->second);
            it++;
        }
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(getTypeParamName(i),
                                                                getTypeParamName(i),
                                                                stringArray,
                                                                0));
    }
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project10AudioProcessor();
}
