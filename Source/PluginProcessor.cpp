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
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Pfmcpp_project10AudioProcessor::~Pfmcpp_project10AudioProcessor()
{
}

//==============================================================================
const juce::String Pfmcpp_project10AudioProcessor::getName() const
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

const juce::String Pfmcpp_project10AudioProcessor::getProgramName (int index)
{
    return {};
}

void Pfmcpp_project10AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Pfmcpp_project10AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    
    // prepare FilterParamsbase Structs
    filterParams.sampleRate = sampleRate;
    highCutLowCutParams.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    auto& leftCoeffs = leftChain.get<0>();
    auto& rightCoeffs = rightChain.get<0>();

    auto currentFilterType = (FilterInfo::FilterType)apvts.getRawParameterValue(getTypeParamName(0))->load();
    
    // Update highCutLowCutParams according to apvts to test
    highCutLowCutParams.frequency = apvts.getRawParameterValue(getFreqParamName(0))->load();
    highCutLowCutParams.quality = apvts.getRawParameterValue(getQualityParamName(0))->load();
    highCutLowCutParams.isLowcut = currentFilterType == FilterInfo::LowPass;
    highCutLowCutParams.order = 1;
    highCutLowCutParams.bypassed = (bool)apvts.getRawParameterValue(getBypassParamName(0))->load();
    
    old_highCutLowCutParams = highCutLowCutParams;
    
    // Update filterParams according to apvts to test
    filterParams.frequency = apvts.getRawParameterValue(getFreqParamName(0))->load();
    filterParams.quality = apvts.getRawParameterValue(getQualityParamName(0))->load();
    filterParams.filterType = currentFilterType;
    filterParams.gainInDb = apvts.getRawParameterValue(getGainParamName(0))->load();
    filterParams.bypassed = (bool)apvts.getRawParameterValue(getBypassParamName(0))->load();
    
    old_filterParams = filterParams;
    
    if ((currentFilterType == FilterInfo::HighPass) || (currentFilterType == FilterInfo::LowPass))
    {
        leftCoeffs = CoefficientsMaker<float>::calcCutCoefficients(highCutLowCutParams)[0];
        rightCoeffs = CoefficientsMaker<float>::calcCutCoefficients(highCutLowCutParams)[0];
    }
    else
    {
        leftCoeffs = CoefficientsMaker<float>::calcFilterCoefficients(filterParams);
        rightCoeffs = CoefficientsMaker<float>::calcFilterCoefficients(filterParams);
    }
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

void Pfmcpp_project10AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    // Check for bypass
    if ((bool)apvts.getRawParameterValue(getBypassParamName(0))->load()) return;
    
    // Check for Parameters changing
    // Check type of filter so you know which filter param struct to use
    auto currentFilterType = (FilterInfo::FilterType)apvts.getRawParameterValue(getTypeParamName(0))->load();
    if ((currentFilterType == FilterInfo::HighPass) || (currentFilterType == FilterInfo::LowPass))
    {
        // check if anything has changed
        // if changed, calc new Coeffs
        highCutLowCutParams.frequency = apvts.getRawParameterValue(getFreqParamName(0))->load();
        highCutLowCutParams.quality = apvts.getRawParameterValue(getQualityParamName(0))->load();
        highCutLowCutParams.isLowcut = currentFilterType == FilterInfo::LowPass;
        highCutLowCutParams.order = 1;
        highCutLowCutParams.bypassed = (bool)apvts.getRawParameterValue(getBypassParamName(0))->load();
        
        if (highCutLowCutParams.isEqual(old_highCutLowCutParams))
        {
            old_highCutLowCutParams = highCutLowCutParams;
            
            auto& leftCoeffs = leftChain.get<0>();
            auto& rightCoeffs = rightChain.get<0>();
            
            leftCoeffs = CoefficientsMaker<float>::calcCutCoefficients(highCutLowCutParams)[0];
            rightCoeffs = CoefficientsMaker<float>::calcCutCoefficients(highCutLowCutParams)[0];
        }
    }
    else
    {
        filterParams.frequency = apvts.getRawParameterValue(getFreqParamName(0))->load();
        filterParams.quality = apvts.getRawParameterValue(getQualityParamName(0))->load();
        filterParams.filterType = (FilterInfo::FilterType)apvts.getRawParameterValue(getTypeParamName(0))->load();
        filterParams.gainInDb = apvts.getRawParameterValue(getGainParamName(0))->load();
        filterParams.bypassed = (bool)apvts.getRawParameterValue(getBypassParamName(0))->load();
        // check if anything has changed
        // if changed, calc new Coeffs
        if (filterParams.isEqual(old_filterParams))
        {
            old_filterParams = filterParams;
            
            auto& leftCoeffs = leftChain.get<0>();
            auto& rightCoeffs = rightChain.get<0>();
            
            leftCoeffs = CoefficientsMaker<float>::calcFilterCoefficients(filterParams);
            rightCoeffs = CoefficientsMaker<float>::calcFilterCoefficients(filterParams);
        }
    }
    
    // Process
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext (leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext (rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool Pfmcpp_project10AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Pfmcpp_project10AudioProcessor::createEditor()
{
//    return new Pfmcpp_project10AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Pfmcpp_project10AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
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
    
    const int numFilters = 1;
    
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
        
        juce::StringArray stringArray;
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
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project10AudioProcessor();
}
