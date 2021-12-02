/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Pfmcpp_project11AudioProcessor::Pfmcpp_project11AudioProcessor()
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

Pfmcpp_project11AudioProcessor::~Pfmcpp_project11AudioProcessor()
{
}

//==============================================================================
const juce::String Pfmcpp_project11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Pfmcpp_project11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Pfmcpp_project11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Pfmcpp_project11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Pfmcpp_project11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Pfmcpp_project11AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Pfmcpp_project11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Pfmcpp_project11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Pfmcpp_project11AudioProcessor::getProgramName (int index)
{
    return {};
}

void Pfmcpp_project11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Pfmcpp_project11AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec, stereoSpec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    
    stereoSpec.maximumBlockSize = samplesPerBlock;
    stereoSpec.sampleRate = sampleRate;
    stereoSpec.numChannels = getTotalNumOutputChannels();
    
    inputTrim.prepare(stereoSpec);
    outputTrim.prepare(stereoSpec);
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    // Initialize Filters
    initializeFilters(sampleRate, 0.05f);
    
    // Reset Chains
    leftChain.reset();
    rightChain.reset();
}


void Pfmcpp_project11AudioProcessor::initializeFilters(const double sampleRate, const float rampTime)
{
    initializeFilter<0, HighCutLowCutParameters>(sampleRate, rampTime);
    initializeFilter<1, FilterParameters>(sampleRate, rampTime);
    initializeFilter<2, FilterParameters>(sampleRate, rampTime);
    initializeFilter<3, FilterParameters>(sampleRate, rampTime);
    initializeFilter<4, FilterParameters>(sampleRate, rampTime);
    initializeFilter<5, FilterParameters>(sampleRate, rampTime);
    initializeFilter<6, FilterParameters>(sampleRate, rampTime);
    initializeFilter<7, HighCutLowCutParameters>(sampleRate, rampTime);
}

void Pfmcpp_project11AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Pfmcpp_project11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Pfmcpp_project11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    updateFilterParams();
    
    updateTrimState();

    // Process The Chain
    juce::dsp::AudioBlock<float> block(buffer);
    
    // Process input Trim
    processTrim(inputTrim, block);

    // Process Filters
    auto maxChunkSize = 32;
    for (auto offset = 0; offset < block.getNumSamples();)
    {
        auto samplesToProcess = static_cast<int>(block.getNumSamples() - offset);
        auto chunkSize = samplesToProcess > maxChunkSize ? maxChunkSize : samplesToProcess;

        // Loop through filters and call performInnerLoopFilterUpdate()
        updateFilterState(chunkSize);

        // Process The Audio
        auto subBlock = block.getSubBlock(offset, chunkSize);
        auto leftSubBlock = subBlock.getSingleChannelBlock(0);
        auto rightSubBlock = subBlock.getSingleChannelBlock(1);

        juce::dsp::ProcessContextReplacing<float> leftContext (leftSubBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext (rightSubBlock);

        leftChain.process(leftContext);
        rightChain.process(rightContext);

        // Update Offset
        offset += chunkSize;
    }
    
    // Process Output Trim
    processTrim(outputTrim, block);
}


void Pfmcpp_project11AudioProcessor::updateFilterState(const int chunkSize)
{
    updateSingleFilterState<0>(true, chunkSize);
    updateSingleFilterState<1>(true, chunkSize);
    updateSingleFilterState<2>(true, chunkSize);
    updateSingleFilterState<3>(true, chunkSize);
    updateSingleFilterState<4>(true, chunkSize);
    updateSingleFilterState<5>(true, chunkSize);
    updateSingleFilterState<6>(true, chunkSize);
    updateSingleFilterState<7>(true, chunkSize);
}

void Pfmcpp_project11AudioProcessor::updateFilterParams()
{
    updateSingleFilterParams<0, HighCutLowCutParameters>();
    updateSingleFilterParams<1, FilterParameters>();
    updateSingleFilterParams<2, FilterParameters>();
    updateSingleFilterParams<3, FilterParameters>();
    updateSingleFilterParams<4, FilterParameters>();
    updateSingleFilterParams<5, FilterParameters>();
    updateSingleFilterParams<6, FilterParameters>();
    updateSingleFilterParams<7, HighCutLowCutParameters>();
}


void Pfmcpp_project11AudioProcessor::updateTrimState()
{
    // Input Trim
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Input Trim")))
    {
        inputTrim.setGainDecibels(p->get());
    }
    
    // Output Trim
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Output Trim")))
    {
        outputTrim.setGainDecibels(p->get());
    }
}

void Pfmcpp_project11AudioProcessor::processTrim(juce::dsp::Gain<float>& gain, juce::dsp::AudioBlock<float>& block)
{
    juce::dsp::ProcessContextReplacing<float> trimContext (block);
    gain.process(trimContext);
}

//==============================================================================


bool Pfmcpp_project11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Pfmcpp_project11AudioProcessor::createEditor()
{
//    return new Pfmcpp_project11AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}


//==============================================================================


void Pfmcpp_project11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void Pfmcpp_project11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid() )
    {
        apvts.replaceState(tree);
        // Initialize Filters
        initializeFilters(getSampleRate(), 0.05f);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Pfmcpp_project11AudioProcessor::createParameterLayout ()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    createTrimParams(layout);
    
    for ( int i = 0; i < chainLength; ++i)
    {
        // Add Low Cut Params to layout
        if (i == 0)
        {
            createCutParams(layout, i, true);
            continue;
        }
        // Add High Cut Params
        if ( i == (chainLength-1) )
        {
            createCutParams(layout, i, false);
            continue;
        }
        
        createFilterParamas(layout, i);
    }
    
    return layout;
}

//==============================================================================


void Pfmcpp_project11AudioProcessor::createCutParams(juce::AudioProcessorValueTreeState::ParameterLayout &layout, const int filterNum, const bool isLowCut)
{
    layout.add(std::make_unique<juce::AudioParameterBool>(getBypassParamName(filterNum),
                                                          getBypassParamName(filterNum),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getFreqParamName(filterNum),
                                                           getFreqParamName(filterNum),
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                                500.f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(getTypeParamName(filterNum),
                                                          getTypeParamName(filterNum),
                                                          isLowCut));
    
    juce::StringArray stringArray;
    
    // Orders 1 through 8
    for (auto i=1; i<9; ++i)
    {
        stringArray.add(juce::String(i));
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getQualityParamName(filterNum),
                                                            getQualityParamName(filterNum),
                                                            stringArray,
                                                            0));
}


void Pfmcpp_project11AudioProcessor::createFilterParamas(juce::AudioProcessorValueTreeState::ParameterLayout &layout, const int filterNum)
{
    layout.add(std::make_unique<juce::AudioParameterBool>(getBypassParamName(filterNum),
                                                          getBypassParamName(filterNum),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getGainParamName(filterNum),
                                                           getGainParamName(filterNum),
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                               0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(getQualityParamName(filterNum),
                                                           getQualityParamName(filterNum),
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                               1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(getFreqParamName(filterNum),
                                                           getFreqParamName(filterNum),
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                               500.f));
    
    juce::StringArray stringArray;
    auto filterTypeMap = FilterInfo::getFilterTypeMap();
    auto it = filterTypeMap.begin();
    
    while (it != filterTypeMap.end())
    {
        stringArray.add(it->second);
        it++;
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getTypeParamName(filterNum),
                                                            getTypeParamName(filterNum),
                                                            stringArray,
                                                            0));
}


void Pfmcpp_project11AudioProcessor::createTrimParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // Input Trim
    layout.add(std::make_unique<juce::AudioParameterFloat>("Input Trim",
                                                           "Input Trim",
                                                           juce::NormalisableRange<float>(-12.f, 12.f, 0.5f, 1.f),
                                                               0.f));
    
    // Output Trim
    layout.add(std::make_unique<juce::AudioParameterFloat>("Output Trim",
                                                           "Output Trim",
                                                           juce::NormalisableRange<float>(-12.f, 12.f, 0.5f, 1.f),
                                                               0.f));
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project11AudioProcessor();
}
