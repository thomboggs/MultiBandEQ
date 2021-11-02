/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FilterHelperFunctions.h"





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
//    fcgFilter("My Thread");
//    using CutCoeffs = juce::dsp::IIR::Coefficients<float>;
//    FilterCoefficientGenerator fcgFilter =
//        FilterCoefficientGenerator<
//            juce::ReferenceCountedArray<CutCoeffs>,
//            HighCutLowCutParameters,
//            CoefficientsMaker<float>,
//            32>
//        (lowCutFifo , "LowCut Thread");
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
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
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
    
    updateParams();
    
    refreshFilters();

    // Process The Chain
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext (leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext (rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
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
        // This updates the apvts. In the processBlock, any apvts changes will be automatically applied.
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Pfmcpp_project11AudioProcessor::createParameterLayout ()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    /*
     I want to add the params for:
        - 1 LowCut Filter
            - Freq
            - Byp
            - Order
            - IsLowCut = True
        - 1 MultiType Filter
            - Freq
            - Byp
            - Quality
            - gain
        - 1 HighCut Filter
            - Freq
            - Byp
            - Order
            - IsLowCut = False
     */
    
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


template<typename ParamType>
ParamType Pfmcpp_project11AudioProcessor::getParams (int bandNum)
{
    ParamType params;
    
    params.sampleRate = getSampleRate();
    
    if constexpr (std::is_same_v<ParamType, HighCutLowCutParameters>)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getQualityParamName(bandNum))))
        {
            params.order = p->getIndex()+1;
        }
        
        if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getTypeParamName(bandNum))))
        {
            params.isLowcut = p->get();
        }
    }
    
    if constexpr (std::is_same_v<ParamType, FilterParameters>)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getTypeParamName(bandNum))))
        {
            params.filterType = static_cast<FilterInfo::FilterType>(p->getIndex());
        }

        if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getGainParamName(bandNum))))
        {
            params.gainInDb = p->get();
        }
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.quality = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    return params;
}


template<int Index>
void Pfmcpp_project11AudioProcessor::setChainBypass(const bool isBypassed)
{
    leftChain.template setBypassed<Index>(isBypassed);
    rightChain.template setBypassed<Index>(isBypassed);
}


void Pfmcpp_project11AudioProcessor::updateParams()
{
    /*
     Low Cut
     */
    updateFilterParams<FilterPosition::LowCut>(currentLowCutParams, leftLowCutFCG, rightLowCutFCG);
    
    /*
     Multi1
     */
    updateFilterParams<FilterPosition::Multi1>(currentFilterParams, leftFilterFCG, rightFilterFCG);
    
    /*
     High Cut Filter
     */
    updateFilterParams<FilterPosition::HighCut>(currentHighCutParams, leftHighCutFCG, rightHighCutFCG);
}


template<int Index, typename ParamType, typename FCG>
void Pfmcpp_project11AudioProcessor::updateFilterParams(ParamType& params, FCG& leftFCG, FCG& rightFCG)
{
    bool filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(Index)))->get();
    if ( !filterBypassed)
    {
        setChainBypass<Index>(false);
        
        auto tempParams = getParams<ParamType>(Index);
        if (params != tempParams)
        {
            // if changed, calc new Coeffs
            params = tempParams;

            leftFCG.changeParameters(params);
            rightFCG.changeParameters(params);
        }
    }
    else
    {
        setChainBypass<Index>(true);
    }
}


void Pfmcpp_project11AudioProcessor::refreshFilters()
{
    // Low Cut
    refreshCutFilter<FilterPosition::LowCut> (leftLowCutFifo, leftChain, leftLowCutReleasePool);

    refreshCutFilter<FilterPosition::LowCut> (rightLowCutFifo, rightChain, rightLowCutReleasePool);
    
    // Peak Filter
    refreshFilter ( leftFilterCoeffFifo, leftChain.get<FilterPosition::Multi1>(), leftFilterReleasePool);
    
    refreshFilter ( rightFilterCoeffFifo, rightChain.get<FilterPosition::Multi1>(), rightFilterReleasePool);
    
    // High Cut
    refreshCutFilter<FilterPosition::HighCut> (leftHighCutFifo, leftChain, leftHighCutReleasePool);

    refreshCutFilter<FilterPosition::HighCut> (rightHighCutFifo, rightChain, rightHighCutReleasePool);
}


template<int Index, typename FifoType, typename Chain, typename Pool>
void Pfmcpp_project11AudioProcessor::refreshCutFilter (FifoType& cutFifo, Chain& chain, Pool& cutPool)
{
    // Refresh Low Cut
    // Left
    if ( cutFifo.getNumAvailableForReading() > 0 )
    {
        auto& CutFilterChain = chain.template get<Index>();
        juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> tempArray {};
    
        if ( cutFifo.pull(tempArray) )
        {
            auto tempSize = tempArray.size();
            if ( tempSize > 0)
            {
                CutFilterChain.template setBypassed<0>(true);
                CutFilterChain.template setBypassed<1>(true);
                CutFilterChain.template setBypassed<2>(true);
                CutFilterChain.template setBypassed<3>(true);
  
                switch (tempSize)
                {
                    case 4:
                        update<3>(CutFilterChain, tempArray, cutPool);
                    case 3:
                        update<2>(CutFilterChain, tempArray, cutPool);
                    case 2:
                        update<1>(CutFilterChain, tempArray, cutPool);
                    case 1:
                        update<0>(CutFilterChain, tempArray, cutPool);
                }
            }
        }
    }
}


template<int Index, typename Link, typename ArrayType, typename Pool>
void Pfmcpp_project11AudioProcessor::update(Link& link, ArrayType& tempArray, Pool& pool)
{
    link.template setBypassed<Index>(false);
    *link.template get<Index>().coefficients = *tempArray[Index];
    pool.add(tempArray[Index]);
}


template<typename FifoType, typename ChainLink, typename Pool>
void Pfmcpp_project11AudioProcessor::refreshFilter (FifoType& filterFifo, ChainLink& link, Pool& filterPool)
{
    CoefficientsPtr ptr;
    
    if ( filterFifo.getNumAvailableForReading() > 0 )
    {
        if ( filterFifo.pull(ptr) )
        {
            filterPool.add(ptr);
            *(link.coefficients) = *ptr;
        }
    }
}





//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project11AudioProcessor();
}
