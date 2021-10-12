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


template <typename FloatType>
struct CoefficientsMaker
{
    
    using Coefficients = typename dsp::IIR::Filter<FloatType>::CoefficientsPtr;
    using IIRCoeffs = dsp::IIR::Coefficients<FloatType>;
    
    static Coefficients calcCoefficientsHelper (const FilterInfo::FilterType& filterType, const float& freq, const float& quality, const float& gain, const double& sampleRate)
    {
        switch (filterType)
        {
            case FilterInfo::FirstOrderLowPass:
                return IIRCoeffs::makeFirstOrderLowPass(sampleRate, freq);
            case FilterInfo::FirstOrderHighPass:
                return IIRCoeffs::makeFirstOrderHighPass(sampleRate, freq);
            case FilterInfo::FirstOrderAllPass:
                return IIRCoeffs::makeFirstOrderAllPass(sampleRate, freq);
            case FilterInfo::LowPass:
                return IIRCoeffs::makeLowPass(sampleRate, freq, quality);
            case FilterInfo::HighPass:
                return IIRCoeffs::makeHighPass(sampleRate, freq, quality);
            case FilterInfo::BandPass:
                return IIRCoeffs::makeBandPass(sampleRate, freq, quality);
            case FilterInfo::Notch:
                return IIRCoeffs::makeNotch(sampleRate, freq, quality);
            case FilterInfo::AllPass:
                return IIRCoeffs::makeAllPass(sampleRate, freq, quality);
            case FilterInfo::LowShelf:
                return IIRCoeffs::makeLowShelf(sampleRate, freq, quality, gain);
            case FilterInfo::HighShelf:
                return IIRCoeffs::makeHighShelf(sampleRate, freq, quality, gain);
            case FilterInfo::Peak:
                return IIRCoeffs::makePeakFilter(sampleRate, freq, quality, gain);
        };
    }
    
    static Coefficients calcFilterCoefficients (const FilterParameters& filterParams)
    {
        return calcCoefficientsHelper(filterParams.filterType, filterParams.frequency, filterParams.quality, filterParams.gainInDb, filterParams.sampleRate);
    }
    
    static ReferenceCountedArray<IIRCoeffs> calcCutCoefficients (const HighCutLowCutParameters& cutParams)
    {
        if (cutParams.isLowcut)
            return dsp::FilterDesign<FloatType>::designIIRLowpassHighOrderButterworthMethod(cutParams.frequency, cutParams.sampleRate, cutParams.order);
        
        return dsp::FilterDesign<FloatType>::designIIRHighpassHighOrderButterworthMethod(cutParams.frequency, cutParams.sampleRate, cutParams.order);

    }
    
    
    
    
};

