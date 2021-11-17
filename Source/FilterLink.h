/*
  ==============================================================================

    FilterLink.h
    Created: 2 Nov 2021 9:50:47am
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ReleasePool.h"
#include "FilterCoefficientGenerator.h"
#include "Fifo.h"
#include "FilterParametersBase.h"
#include "FilterCoefficientGenerator.h"
#include "CoefficientsMaker.h"


//=============================================================================
/*
 Template Specialization To check if T is ReferenceCountedObjectPtr
 */
template <typename FloatType>
using Filter = juce::dsp::IIR::Filter<FloatType>;
//using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

template <typename T>
struct IsNotCutFilter : std::false_type { };

template <typename T>
struct IsNotCutFilter<Filter<T>> : std::true_type { };

//=============================================================================

template<typename FilterType, typename FifoDataType, typename ParamType, typename FunctionType>
struct FilterLink
{
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        filter.prepare(spec);
    }
    
    void reset()
    {
        filter.reset();
    }
    
    template<typename ContextType>
    void process(const ContextType& context)
    {
        filter.process(context);
    }
    
    //stuff for the juce::SmoothedValue instances.
    void updateSmootherTargets();
    void resetSmoothers(float rampTime);
    bool isSmoothing() const;
    void checkIfStillSmoothing();
    void advanceSmoothers(int numSamples);
    
    //stuff for updating the params
    void updateParams(const ParamType& params)
    {
        if (linkParams != params)
        {
            // if changed, calc new Coeffs
            linkParams = params;
            shouldComputeNewCoefficients = true;
        }
    }
    
    //stuff for updating the coefficients from processBlock, prepareToPlay, or setStateInformation
    void updateCoefficients(const FifoDataType& coefficients)
    {
        /*
         Depending on FilterType, Choose which provate function to call to update Coeffs
         */
        if constexpr (IsNotCutFilter<FilterType>::value)
        {
            updateFilterState(filter.coefficients, coefficients);
        }
        else
        {
            configureCutFilterChain(coefficients);
        }
    }
    
    void loadCoefficients(bool fromFifo)
    {
    /*
     loadCoefficients(fromFifo) does one of 2 things:

         either pulls coefficients from the fifo and passes them to updateCoefficients. This happens on the audio thread.

         uses the CoefficientMaker static functions directly to create coefficients and passes them to updateCoefficients.  This is called from a thread other than the audio thread.
     */
        
    /*
     if fromFifo:
        pull coeff from fifo and pass them to updateCoefficients
     else:
        use CoefficientMaker to create coefficients and pass them to updateCoefficients
     */
        
    /*
     
     TODO:
     1)
     */
        if (fromFifo)
        {
            if ( linkFifo.getNumAvailableForReading() > 0 )
            {
                FifoDataType ptr;
                
                if ( linkFifo.pull(ptr) )
                {
                    updateCoefficients(ptr);
                }
            }
        }
        else
        {
            // call static coeffmaker function dependng on filterType
            if constexpr (IsNotCutFilter<FilterType>::value)
            {
                updateCoefficients( calcFilterCoefficients(linkParams) );
            }
            else
            {
                updateCoefficients( calcCutCoefficients(linkParams) );
            }
        }
    }
    
    void generateNewCoefficientsIfNeeded()
    {
        if (shouldComputeNewCoefficients)
        {
            // This is currently not using the smoothers
            ParamType params = linkParams;
            
            // Send Params to the FCG
            linkFCG.changeParameters(params);
            
            // Before Leaving, reset flag
            shouldComputeNewCoefficients = false;
        }
    }
    
    //stuff for configuring the filter before processing
    void performPreloopUpdate(const ParamType& params);
    void performInnerLoopFilterUpdate(bool onRealTimeThread, int numSamplesToSkip);
    void initialize(const ParamType& params, float rampTime, bool onRealTimeThread, double sr);
private:
    static const int fifoSize = 32;
    
    // releasePool
    ReleasePool<FifoDataType> linkDeletionPool { };
    // Fifo
    Fifo<FifoDataType, fifoSize> linkFifo;
    // FCG
    FilterCoefficientGenerator<FifoDataType, ParamType, CoefficientsMaker<float>, fifoSize> linkFCG;
    // Params
    ParamType linkParams;
    // shouldComputeNewCoefficients flag
    bool shouldComputeNewCoefficients { false };
    // sampleRate
    double sampleRate { 0 };
    // FilterType
    FilterType filter;
    
    // SmoothedValue Instances
    // FreqSmoother
    // Quality Smoother
    // Gain Smoother
    
    
    
    //stuff for setting the coefficients of the FilterType instance.
    using Ptr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    
    void updateFilterState(Ptr& oldState, Ptr newState)
    {
        linkDeletionPool.add(newState);
        *(oldState) = *newState;
    }
    
    void configureCutFilterChain(const FifoDataType& coefficients)
    {
        auto tempSize = coefficients.size();
        if ( tempSize > 0)
        {
            filter.template setBypassed<0>(true);
            filter.template setBypassed<1>(true);
            filter.template setBypassed<2>(true);
            filter.template setBypassed<3>(true);
            
            switch (tempSize)
            {
                case 4:
                    filter.template setBypassed<3>(false);
                    updateFilterState(filter.template getIndex<3>().coefficients, coefficients[3]);
//                    update<3>(leftCutFilter, tempArray, cutPool);
                case 3:
                    filter.template setBypassed<2>(false);
                    updateFilterState(filter.template getIndex<2>().coefficients, coefficients[2]);
//                    update<2>(leftCutFilter, tempArray, cutPool);
                case 2:
                    filter.template setBypassed<1>(false);
                    updateFilterState(filter.template getIndex<1>().coefficients, coefficients[1]);
//                    update<1>(leftCutFilter, tempArray, cutPool);
                case 1:
                    filter.template setBypassed<0>(false);
                    updateFilterState(filter.template getIndex<0>().coefficients, coefficients[0]);
//                    update<0>(leftCutFilter, tempArray, cutPool);
            }
        }
    }
    
    
    
};
