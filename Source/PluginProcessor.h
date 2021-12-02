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
    static void createTrimParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout);
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
    
    using CoefficientsPtr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    using CutCoeffs = juce::dsp::IIR::Coefficients<float>;
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilterChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    using CutFilter = FilterLink<CutFilterChain, juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>>;
    using SingleFilter = FilterLink<Filter, CoefficientsPtr, FilterParameters, CoefficientsMaker<float>>;
    
    using Chain = juce::dsp::ProcessorChain<CutFilter,
                                            SingleFilter,
                                            SingleFilter,
                                            SingleFilter,
                                            SingleFilter,
                                            SingleFilter,
                                            SingleFilter,
                                            CutFilter>;
    static const int chainLength { 8 };
    
    Chain leftChain, rightChain;
    
    juce::dsp::Gain<float> inputTrim, outputTrim;

    void initializeFilters (const double sampleRate, const float rampTime);
    
    template <int Index, typename ParamType>
    void initializeFilter (const double sampleRate, const float rampTime);
    
    template <typename ParamType>
    ParamType getParams (const int bandNum, const double sampleRate);
    
    template <int Index>
    void setChainBypass(const bool isBypassed);
    
    void updateFilterParams ();
    
    template <int Index, typename ParamType>
    void updateSingleFilterParams ();
    
    void updateFilterState (const int chunkSize);
    
    template <int Index>
    void updateSingleFilterState (const bool onRealTimeThread, const int chunkSize);
    
    void updateTrimState();
    
    void processTrim(juce::dsp::Gain<float>& gain, juce::dsp::AudioBlock<float>& block);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project11AudioProcessor)
};


//==============================================================================
/*
        Template Function Definition
 */
//==============================================================================

template<typename ParamType>
ParamType Pfmcpp_project11AudioProcessor::getParams (const int bandNum, const double sampleRate)
{
    ParamType params;
    
    params.sampleRate = sampleRate;
    
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
            params.gain.setDb(p->get());
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


template <int Index, typename ParamType>
void Pfmcpp_project11AudioProcessor::initializeFilter (const double sampleRate, const float rampTime)
{
    auto tempCutParams = getParams<ParamType>(Index, sampleRate);
    leftChain.get<Index>().initialize(tempCutParams, rampTime, false, sampleRate);
    rightChain.get<Index>().initialize(tempCutParams, rampTime, false, sampleRate);
    
}


template <int Index, typename ParamType>
void Pfmcpp_project11AudioProcessor::updateSingleFilterParams ()
{
    auto sampleRate = getSampleRate();
    leftChain.get<Index>().performPreloopUpdate(getParams<ParamType>(Index, sampleRate));
    rightChain.get<Index>().performPreloopUpdate(getParams<ParamType>(Index, sampleRate));
}

template <int Index>
void Pfmcpp_project11AudioProcessor::updateSingleFilterState (const bool onRealTimeThread, const int chunkSize)
{
    leftChain.get<Index>().performInnerLoopFilterUpdate(onRealTimeThread, chunkSize);
    rightChain.get<Index>().performInnerLoopFilterUpdate(onRealTimeThread, chunkSize);
}
