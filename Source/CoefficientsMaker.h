/*
  ==============================================================================

    CoefficientsMaker.h
    Created: 11 Oct 2021 7:36:51pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "FilterInfo.h"
#include "FilterParametersBase.h"


using Coefficients = dsp::IIR::Filter<float>::CoefficientsPtr;
using IIRCoeffs = dsp::IIR::Coefficients<float>;

//template <typename FloatType>
struct CoefficientsMaker
{
    static Coefficients calcFilterCoefficients (const FilterParameters& filterParams);
    static ReferenceCountedArray<IIRCoeffs> calcCutCoefficients (const HighCutLowCutParameters& cutParams);
    
private:
    static Coefficients calcCoefficientsHelper (const FilterInfo::FilterType& filterType, const float& freq, const float& quality, const float& gain, const double& sampleRate);
    
};

