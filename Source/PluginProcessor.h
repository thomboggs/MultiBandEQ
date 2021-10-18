/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterParametersBase.h"
#include "FilterInfo.h"


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

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout ();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Params", createParameterLayout() };
private:
    HighCutLowCutParameters currentCutParams;
    FilterParameters currentFilterParams;
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterChain = juce::dsp::ProcessorChain<Filter, Filter, Filter>;
    
    FilterChain leftChain, rightChain;
    
    void updateCutCoefficients (const HighCutLowCutParameters& params);
    void updateFilterCoefficients (const FilterParameters& params);
    
    HighCutLowCutParameters getCutParams (int bandNum);
    FilterParameters getFilterParams (int bandNum);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project11AudioProcessor)
};
