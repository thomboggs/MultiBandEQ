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
    
    /*
     Low Cut
     */
    // Check for not bypassed
    bool filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(FilterPosition::LowCut)))->get();
    if ( !filterBypassed )
    {
        setChainBypass(false, FilterPosition::LowCut);
        
        auto tempHighCutLowCutParams = getCutParams(0);
        if (currentLowCutParams != tempHighCutLowCutParams)
        {
            // if changed, calc new Coeffs
//            DBG("Updating Coefficients");
            currentLowCutParams = tempHighCutLowCutParams;
            updateLowCutCoefficients(currentLowCutParams);
        }
    }
    else
    {
        setChainBypass(true, FilterPosition::LowCut);
    }
    
    /*
     Multi1
     */
    filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(FilterPosition::Multi1)))->get();
    if ( !filterBypassed )
    {
        setChainBypass(false, FilterPosition::Multi1);
        
        // Middle Filter Params
        auto tempFilterParams = getFilterParams(FilterPosition::Multi1);
        
        if (currentFilterParams != tempFilterParams)
        {
            // if changed, calc new Coeffs
            currentFilterParams = tempFilterParams;
            updateFilterCoefficients(currentFilterParams, FilterPosition::Multi1);
        }
    }
    else
    {
        setChainBypass(true, FilterPosition::Multi1);
    }
    
    /*
     High Cut Filter
     */
    filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(FilterPosition::HighCut)))->get();
    if ( !filterBypassed)
    {
        setChainBypass(false, FilterPosition::HighCut);
        
        auto tempHighCutLowCutParams = getCutParams(FilterPosition::HighCut);
        if (currentHighCutParams != tempHighCutLowCutParams)
        {
            // if changed, calc new Coeffs
            currentHighCutParams = tempHighCutLowCutParams;
            updateHighCutCoefficients(currentHighCutParams);
        }
    }
    else
    {
        setChainBypass(true, FilterPosition::HighCut);
    }
    
//
//    // Check for Parameters changing
//    // Check type of filter so you know which filter param struct to use
//    auto currentFilterType = (FilterInfo::FilterType)apvts.getRawParameterValue(getTypeParamName(0))->load();
//    if ((currentFilterType == FilterInfo::HighPass) || (currentFilterType == FilterInfo::LowPass))
//    {
//        // check if anything has changed
//        auto tempHighCutLowCutParams = getCutParams(0);
//
//        if (currentCutParams != tempHighCutLowCutParams)
//        {
//            // if changed, calc new Coeffs
//            currentCutParams = tempHighCutLowCutParams;
//            updateCutCoefficients(currentCutParams);
//        }
//    }
//    else
//    {
//        // check if anything has changed
//        auto tempFilterParams = getFilterParams(0);
//
//        if (currentFilterParams != tempFilterParams)
//        {
//            // if changed, calc new Coeffs
//            currentFilterParams = tempFilterParams;
//            updateFilterCoefficients(currentFilterParams);
//        }
//    }
//
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
//    return new Pfmcpp_project10AudioProcessorEditor (*this);
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
    
    
    
//    const int numFilters = 3;
    
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
        
//        layout.add(std::make_unique<juce::AudioParameterBool>(getBypassParamName(i),
//                                                              getBypassParamName(i),
//                                                              false));
//
//        layout.add(std::make_unique<juce::AudioParameterFloat>(getGainParamName(i),
//                                                               getGainParamName(i),
//                                                               juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
//                                                                   0.f));
//        layout.add(std::make_unique<juce::AudioParameterFloat>(getQualityParamName(i),
//                                                               getQualityParamName(i),
//                                                               juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
//                                                                   1.f));
//        layout.add(std::make_unique<juce::AudioParameterFloat>(getFreqParamName(i),
//                                                               getFreqParamName(i),
//                                                               juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
//                                                                   500.f));
//
//        juce::StringArray stringArray;
//        auto filterTypeMap = FilterInfo::getFilterTypeMap();
//        auto it = filterTypeMap.begin();
//
//        while (it != filterTypeMap.end())
//        {
//            stringArray.add(it->second);
//            it++;
//        }
//
//        layout.add(std::make_unique<juce::AudioParameterChoice>(getTypeParamName(i),
//                                                                getTypeParamName(i),
//                                                                stringArray,
//                                                                0));
    }
    
    return layout;
}


//==============================================================================
void Pfmcpp_project11AudioProcessor::updateCutCoefficients(const HighCutLowCutParameters& params, FilterPosition pos)
{
    
//    auto& leftFilter = leftChain.template get<0>();
//    auto& rightFilter = rightChain.template get<0>();
    

//    *leftFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
//    *rightFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
}


void Pfmcpp_project11AudioProcessor::updateLowCutCoefficients(const HighCutLowCutParameters &params)
{
//    auto& leftFilter = leftChain.template get<FilterPosition::LowCut>();
//    auto& rightFilter = rightChain.template get<FilterPosition::LowCut>();
    
//    using IIRCoeffsFloat = juce::dsp::IIR::Coefficients<float>;
//    auto filterCoeffArray = std::make_unique<juce::ReferenceCountedArray<IIRCoeffsFloat>>(CoefficientsMaker<float>::calcCutCoefficients(params)) ;
//    auto filterCoeffArray = CoefficientsMaker<float>::calcCutCoefficients(params) ;
//    DBG(filterCoeffArray.size());
    
    
//    *leftFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
//    *rightFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
    auto& leftCutFilterChain = leftChain.template get<FilterPosition::LowCut>();
    auto& rightCutFilterChain = rightChain.template get<FilterPosition::LowCut>();
    
    auto lowCutCoeffArray = CoefficientsMaker<float>::calcCutCoefficients(params);
    
    /*
     - Set all cut filters to bypass
     - Switch statement through order
     - Fall through to unbypass and apply coeffs
     */
    leftCutFilterChain.setBypassed<0>(true);
    leftCutFilterChain.setBypassed<1>(true);
    leftCutFilterChain.setBypassed<2>(true);
    leftCutFilterChain.setBypassed<3>(true);
    
    rightCutFilterChain.setBypassed<0>(true);
    rightCutFilterChain.setBypassed<1>(true);
    rightCutFilterChain.setBypassed<2>(true);
    rightCutFilterChain.setBypassed<3>(true);
    
    switch (params.order)
    {
        case 8:
        case 7:
            leftCutFilterChain.setBypassed<3>(false);
            rightCutFilterChain.setBypassed<3>(false);
            *leftCutFilterChain.get<3>().coefficients = *lowCutCoeffArray[3];
            *rightCutFilterChain.get<3>().coefficients = *lowCutCoeffArray[3];
        case 6:
        case 5:
            leftCutFilterChain.setBypassed<2>(false);
            rightCutFilterChain.setBypassed<2>(false);
            *leftCutFilterChain.get<2>().coefficients = *lowCutCoeffArray[2];
            *rightCutFilterChain.get<2>().coefficients = *lowCutCoeffArray[2];
        case 4:
        case 3:
            leftCutFilterChain.setBypassed<1>(false);
            rightCutFilterChain.setBypassed<1>(false);
            *leftCutFilterChain.get<1>().coefficients = *lowCutCoeffArray[1];
            *rightCutFilterChain.get<1>().coefficients = *lowCutCoeffArray[1];
        case 2:
        case 1:
            leftCutFilterChain.setBypassed<0>(false);
            rightCutFilterChain.setBypassed<0>(false);
            *leftCutFilterChain.get<0>().coefficients = *lowCutCoeffArray[0];
            *rightCutFilterChain.get<0>().coefficients = *lowCutCoeffArray[0];
    }
}


void Pfmcpp_project11AudioProcessor::updateHighCutCoefficients(const HighCutLowCutParameters &params)
{
    auto& leftCutFilterChain = leftChain.template get<FilterPosition::HighCut>();
    auto& rightCutFilterChain = rightChain.template get<FilterPosition::HighCut>();
    
    auto highCutCoeffArray = CoefficientsMaker<float>::calcCutCoefficients(params);
    
    /*
     - Set all cut filters to bypass
     - Switch statement through order
     - Fall through to unbypass and apply coeffs
     */
    leftCutFilterChain.setBypassed<0>(true);
    leftCutFilterChain.setBypassed<1>(true);
    leftCutFilterChain.setBypassed<2>(true);
    leftCutFilterChain.setBypassed<3>(true);
    
    rightCutFilterChain.setBypassed<0>(true);
    rightCutFilterChain.setBypassed<1>(true);
    rightCutFilterChain.setBypassed<2>(true);
    rightCutFilterChain.setBypassed<3>(true);
    
    switch (params.order)
    {
        case 8:
        case 7:
            leftCutFilterChain.setBypassed<3>(false);
            rightCutFilterChain.setBypassed<3>(false);
            *leftCutFilterChain.get<3>().coefficients = *highCutCoeffArray[3];
            *rightCutFilterChain.get<3>().coefficients = *highCutCoeffArray[3];
        case 6:
        case 5:
            leftCutFilterChain.setBypassed<2>(false);
            rightCutFilterChain.setBypassed<2>(false);
            *leftCutFilterChain.get<2>().coefficients = *highCutCoeffArray[2];
            *rightCutFilterChain.get<2>().coefficients = *highCutCoeffArray[2];
        case 4:
        case 3:
            leftCutFilterChain.setBypassed<1>(false);
            rightCutFilterChain.setBypassed<1>(false);
            *leftCutFilterChain.get<1>().coefficients = *highCutCoeffArray[1];
            *rightCutFilterChain.get<1>().coefficients = *highCutCoeffArray[1];
        case 2:
        case 1:
            leftCutFilterChain.setBypassed<0>(false);
            rightCutFilterChain.setBypassed<0>(false);
            *leftCutFilterChain.get<0>().coefficients = *highCutCoeffArray[0];
            *rightCutFilterChain.get<0>().coefficients = *highCutCoeffArray[0];
    }

//    *leftFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
//    *rightFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
}


void Pfmcpp_project11AudioProcessor::updateFilterCoefficients(const FilterParameters& params, const int filterIndex)
{
    auto& leftFilter = leftChain.template get<FilterPosition::Multi1>();
    auto& rightFilter = rightChain.template get<FilterPosition::Multi1>();
    
    *leftFilter.coefficients = *CoefficientsMaker<float>::calcFilterCoefficients(params);
    *rightFilter.coefficients = *CoefficientsMaker<float>::calcFilterCoefficients(params);
}


FilterParameters Pfmcpp_project11AudioProcessor::getFilterParams(int bandNum)
{
    FilterParameters params;

    params.sampleRate = getSampleRate();
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.quality = p->get();
    }

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getTypeParamName(bandNum))))
    {
        params.filterType = static_cast<FilterInfo::FilterType>(p->getIndex());
    }

    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getGainParamName(bandNum))))
    {
        params.gainInDb = p->get();
    }
    
    return params;
}


HighCutLowCutParameters Pfmcpp_project11AudioProcessor::getCutParams(int bandNum)
{
    HighCutLowCutParameters params;
    
    params.sampleRate = getSampleRate();
    
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.order = p->getIndex()+1;
    }
//    params.order = 1;
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
//    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getQualityParamName(bandNum))))
//    {
//        params.quality = p->get();
//    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getTypeParamName(bandNum))))
    {
        params.isLowcut = p->get();
    }
    
    return params;
}


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

void Pfmcpp_project11AudioProcessor::setChainBypass(const bool isBypassed, FilterPosition pos)
{
    switch (pos)
    {
        case FilterPosition::LowCut:
            leftChain.template setBypassed<0>(isBypassed);
            rightChain.template setBypassed<0>(isBypassed);
            break;
        case FilterPosition::Multi1:
            leftChain.template setBypassed<1>(isBypassed);
            rightChain.template setBypassed<1>(isBypassed);
            break;
        case FilterPosition::HighCut:
            leftChain.template setBypassed<2>(isBypassed);
            rightChain.template setBypassed<2>(isBypassed);
            break;
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project11AudioProcessor();
}
