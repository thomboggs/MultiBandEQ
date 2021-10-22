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
     Loop through filters of the chain to check for parameter change
     */
    
    for (auto filterIndex = 0; filterIndex < chainLength; ++filterIndex)
    {
        // Check for not bypassed
        if ( !static_cast<bool>( apvts.getParameter( getBypassParamName(filterIndex) ) ) )
        {
//            leftChain.setBypassed<filterIndex>(false);
            setChainBypass<filterIndex>(leftChain, true);
            rightChain.setBypassed<filterIndex>(false);
//            setChainBypass(leftChain, filterIndex, true);
            
            // LowCut
            if (filterIndex == 0)
            {
                auto tempHighCutLowCutParams = getCutParams(0);
                if (currentCutParams != tempHighCutLowCutParams)
                {
                    // if changed, calc new Coeffs
                    currentCutParams = tempHighCutLowCutParams;
                    updateCutCoefficients(currentCutParams, filterIndex);
                    continue;
                }
            }
            
            // HighCut
            if (filterIndex == (chainLength - 1) )
            {
                auto tempHighCutLowCutParams = getCutParams(chainLength-1);
                if (currentCutParams != tempHighCutLowCutParams)
                {
                    // if changed, calc new Coeffs
                    currentCutParams = tempHighCutLowCutParams;
                    updateCutCoefficients(currentCutParams, filterIndex);
                    continue;
                }
            }
            
            // Middle Filter Params
            auto tempFilterParams = getFilterParams(0);
            
            if (currentFilterParams != tempFilterParams)
            {
                // if changed, calc new Coeffs
                currentFilterParams = tempFilterParams;
                updateFilterCoefficients(currentFilterParams, filterIndex);
            }
            
            
        }
        else
        {
            leftChain.setBypassed<filterIndex>(true);
//            setChainBypass(leftChain, filterIndex, true);
            rightChain.setBypassed<filterIndex>(true);
//            setChainBypass(leftChain, filterIndex, true);
        }
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

void Pfmcpp_project11AudioProcessor::updateCutCoefficients(const HighCutLowCutParameters& params, const int filterIndex)
{
    auto& leftFilter = leftChain.get<filterIndex>();
    auto& rightFilter = rightChain.get<filterIndex>();
    

    *leftFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
    *rightFilter.coefficients = *CoefficientsMaker<float>::calcCutCoefficients(params)[0];
}

void Pfmcpp_project11AudioProcessor::updateFilterCoefficients(const FilterParameters& params, const int filterIndex)
{
    auto& leftFilter = leftChain.get<filterIndex>();
    auto& rightFilter = rightChain.get<filterIndex>();
    
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
    params.order = 1;
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.quality = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getTypeParamName(bandNum))))
    {
        params.isLowcut = ( static_cast<FilterInfo::FilterType>(p->getIndex()) == FilterInfo::LowPass  );
        
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
    
    
    juce::StringArray stringArray;
    
    // Orders 1 through 8
    for (auto i=1; i<9; ++i)
    {
        stringArray.add(juce::String(i));
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getTypeParamName(filterNum),
                                                            getTypeParamName(filterNum),
                                                            stringArray,
                                                            0));
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project11AudioProcessor();
}
