/*
  ==============================================================================

    CoefficientsMaker.cpp
    Created: 11 Oct 2021 7:36:51pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "CoefficientsMaker.h"


//using Coefficients = dsp::IIR::Filter<float>::CoefficientsPtr;
//template<typename FloatType>
Coefficients CoefficientsMaker::calcCoefficientsHelper (const FilterInfo::FilterType& filterType, const float& freq, const float& quality, const float& gain, const double& sampleRate)
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


Coefficients CoefficientsMaker::calcFilterCoefficients (const FilterParameters& filterParams)
{
    return calcCoefficientsHelper(filterParams.filterType, filterParams.frequency, filterParams.quality, filterParams.gainInDb, filterParams.sampleRate);
}


ReferenceCountedArray<IIRCoeffs> CoefficientsMaker::calcCutCoefficients(const HighCutLowCutParameters& cutParams)
{
    if (cutParams.isLowcut)
    {
        return dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(cutParams.frequency, cutParams.sampleRate, cutParams.order);
    }
    else
    {
        return dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(cutParams.frequency, cutParams.sampleRate, cutParams.order);
    }
}
