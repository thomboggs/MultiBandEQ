/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterParametersBase.h"
#include "FilterHelperFunctions.h"
#include "CoefficientsMaker.h"
#include "FilterInfo.h"
#include "Fifo.h"
#include "FilterCoefficientGenerator.h"
#include "ReleasePool.h"
#include "FilterLink.h"


//==============================================================================
/**
*/
class Pfmcpp_project11AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Pfmcpp_project11AudioProcessor();
    ~Pfmcpp_project11AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // APVTS and Audio Parameter Creation
    static void createCutParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout, const int filterNum, const bool isLowCut);
    static void createFilterParamas (juce::AudioProcessorValueTreeState::ParameterLayout& layout, const int filterNum);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout ();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Params", createParameterLayout() };
    
private:
    
    enum FilterPosition
    {
        LowCut,
        Multi1,
        HighCut
    };
    
    HighCutLowCutParameters currentLowCutParams, currentHighCutParams;
    FilterParameters currentFilterParams;
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using FilterChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    static const int chainLength { 3 };
    
    FilterChain leftChain, rightChain;
    
    using CoefficientsPtr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    using CutCoeffs = juce::dsp::IIR::Coefficients<float>;
    
    Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32> LowCutFifo, HighCutFifo;
    Fifo<CoefficientsPtr, 32> FilterCoeffFifo;
    
    FilterCoefficientGenerator<CoefficientsPtr, FilterParameters, CoefficientsMaker<float>, 32> peakFilterFCG { FilterCoeffFifo , "Peak Filter Thread"};
    FilterCoefficientGenerator<juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>, 32> lowCutFCG {LowCutFifo , "LowCut Thread" };
    FilterCoefficientGenerator<juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>, 32> highCutFCG {HighCutFifo , "HighCut Thread" };
    
    ReleasePool<CoefficientsPtr> deletionPool { };
    
    template<typename ParamType>
    ParamType getParams (int bandNum);
    
    template<int Index>
    void setChainBypass(const bool isBypassed);
    
    void updateParams ();
    
    template<int Index, typename ParamType, typename FCG>
    void updateFilterParams(ParamType& params, FCG& filterFCG);
    
    void refreshFilters ();

    template<int Index, typename FifoType, typename Pool>
    void refreshCutFilter (FifoType& cutFifo, Pool& cutPool);
    
    template<int Index, typename Link, typename ArrayType, typename Pool>
    void update(Link& link, ArrayType& tempArray, Pool& pool);
    
    template<int Index, typename FifoType, typename Pool>
    void refreshFilter (FifoType& filterFifo, Pool& filterPool);
    
    // Test for FilterLink
//    FilterLink<Filter, CoefficientsPtr, FilterParameters, CoefficientsMaker<float>> testLink;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project11AudioProcessor)
};


//==============================================================================
/*
        Template Function Definition
 */
//==============================================================================
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
    updateFilterParams<FilterPosition::LowCut>(currentLowCutParams, lowCutFCG);
    
    /*
     Multi1
     */
    updateFilterParams<FilterPosition::Multi1>(currentFilterParams, peakFilterFCG);
    
    /*
     High Cut Filter
     */
    updateFilterParams<FilterPosition::HighCut>(currentHighCutParams, highCutFCG);
}


template<int Index, typename ParamType, typename FCG>
void Pfmcpp_project11AudioProcessor::updateFilterParams(ParamType& params, FCG& filterFCG)
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

            filterFCG.changeParameters(params);
        }
    }
    else
    {
        setChainBypass<Index>(true);
    }
}


template<int Index, typename FifoType, typename Pool>
void Pfmcpp_project11AudioProcessor::refreshCutFilter (FifoType& cutFifo, Pool& cutPool)
{
    // Refresh Low Cut
    // Left
    if ( cutFifo.getNumAvailableForReading() > 0 )
    {
        juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> tempArray {};
    
        if ( cutFifo.pull(tempArray) )
        {
            auto& leftCutFilter = leftChain.template get<Index>();
            auto& rightCutFilter = rightChain.template get<Index>();

            auto tempSize = tempArray.size();
            if ( tempSize > 0)
            {
                leftCutFilter.template setBypassed<0>(true);
                leftCutFilter.template setBypassed<1>(true);
                leftCutFilter.template setBypassed<2>(true);
                leftCutFilter.template setBypassed<3>(true);
                
                rightCutFilter.template setBypassed<0>(true);
                rightCutFilter.template setBypassed<1>(true);
                rightCutFilter.template setBypassed<2>(true);
                rightCutFilter.template setBypassed<3>(true);
  
                switch (tempSize)
                {
                    case 4:
                        update<3>(leftCutFilter, tempArray, cutPool);
                        update<3>(rightCutFilter, tempArray, cutPool);
                    case 3:
                        update<2>(leftCutFilter, tempArray, cutPool);
                        update<2>(rightCutFilter, tempArray, cutPool);
                    case 2:
                        update<1>(leftCutFilter, tempArray, cutPool);
                        update<1>(rightCutFilter, tempArray, cutPool);
                    case 1:
                        update<0>(leftCutFilter, tempArray, cutPool);
                        update<0>(rightCutFilter, tempArray, cutPool);
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


template<int Index, typename FifoType, typename Pool>
void Pfmcpp_project11AudioProcessor::refreshFilter (FifoType& filterFifo, Pool& filterPool)
{
    if ( filterFifo.getNumAvailableForReading() > 0 )
    {
        CoefficientsPtr ptr;
        
        if ( filterFifo.pull(ptr) )
        {
            filterPool.add(ptr);
            *(leftChain.get<Index>().coefficients) = *ptr;
            *(rightChain.get<Index>().coefficients) = *ptr;
        }
    }
}
