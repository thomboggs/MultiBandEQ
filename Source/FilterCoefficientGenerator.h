/*
  ==============================================================================

    FilterCoefficientGenerator.h
    Created: 23 Oct 2021 5:33:31pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

#include "FilterParametersBase.h"
#include "CoefficientsMaker.h"
#include "Fifo.h"



template <typename CoefficientType, typename ParamType, typename MakeFunction, size_t Size>
struct FilterCoefficientGenerator : juce::Thread
{
    FilterCoefficientGenerator()
    {
        startThread();
    }
    
    ~FilterCoefficientGenerator()
    {
        stopThread(100);
    }
    
    // changeParameters will push params into a Fifo<> that the run() function will consume.
    void changeParameters(ParamType params)
    {
        paramFifo.push(params);
        parametersChanged.set(true);
    }
    
    //
    void run() override
    {
        // If parametersChanged == true, compute new coefficients
        if ( parametersChanged.compareAndSetBool(false, true) )
        {
            ParamType params;
            while (paramFifo.pull(params))
            {
                if ( std::is_same_v<ParamType, HighCutLowCutParameters> )
                {
                    // Calc Cut Parameters
                    auto cutCoeffArray = CoefficientsMaker<float>::calcCutCoefficients(params);
                    // check if size > 0
                    if ( cutCoeffArray.size() > 0 )
                    {
                        // Do I need to std::move?
                        coefficientsFifo.push(cutCoeffArray);
                    }
                }
                else
                {
                    // Calc Filter Parameters
                    auto filterCoeffs = CoefficientsMaker<float>::calcFilterCoefficients(params);
                    // Check if not null
                    if (filterCoeffs.get() != nullptr)
                    {
                        // Do I need to std::move?
                        coefficientsFifo.push(filterCoeffs);
                    }
                }
            }
        }
        
        // Wait
        wait(10);
    }
    

private:
    juce::Atomic<bool> parametersChanged { false };
    
    Fifo<ParamType, Size> paramFifo;
    Fifo<CoefficientType, Size>& coefficientsFifo;

};
