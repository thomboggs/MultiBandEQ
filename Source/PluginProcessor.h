/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


namespace FilterInfo
{
    enum FilterType
    {
        FirstOrderLowPass,
        FirstOrderHighPass,
        FirstOrderAllPass,
        LowPass,
        HighPass,
        BandPass,
        Notch,
        AllPass,
        LowShelf,
        HighShelf,
        Peak
    };

    static std::map<FilterType, juce::String> FilterTypeMap
    {
        {FilterType::FirstOrderLowPass, "FirstOrderLowPass"},
        {FilterType::FirstOrderHighPass, "FirstOrderHighPass"},
        {FilterType::FirstOrderAllPass, "FirstOrderAllPass"},
        {FilterType::LowPass, "LowPass"},
        {FilterType::HighPass, "HighPass"},
        {FilterType::BandPass, "BandPass"},
        {FilterType::Notch, "Notch"},
        {FilterType::AllPass, "AllPass"},
        {FilterType::LowShelf, "LowShelf"},
        {FilterType::HighShelf, "HighShelf"},
        {FilterType::Peak, "Peak"},
    };
}




//==============================================================================
/**
*/
class Pfmcpp_project10AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Pfmcpp_project10AudioProcessor();
    ~Pfmcpp_project10AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessor)
};
