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
    
    using Coefficients = typename juce::dsp::IIR::Filter<FloatType>::CoefficientsPtr;
    using IIRCoeffs = juce::dsp::IIR::Coefficients<FloatType>;
    
    static Coefficients calcCoefficientsHelper (const FilterInfo::FilterType& filterType, const float& freq, const float& quality, const float& gain, const double& sampleRate)
    {
        
        auto gainFactor = std::pow(10, (gain / 20.f));
        
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
                return IIRCoeffs::makeLowShelf(sampleRate, freq, quality, gainFactor);
            case FilterInfo::HighShelf:
                return IIRCoeffs::makeHighShelf(sampleRate, freq, quality, gainFactor);
            case FilterInfo::Peak:
                return IIRCoeffs::makePeakFilter(sampleRate, freq, quality, gainFactor);
        };
    }
    
    static Coefficients calcCoefficients (const FilterParameters& filterParams)
    {
        return calcCoefficientsHelper(filterParams.filterType, filterParams.frequency, filterParams.quality, filterParams.gain.getDb(), filterParams.sampleRate);
    }
    
    static juce::ReferenceCountedArray<IIRCoeffs> calcCoefficients (const HighCutLowCutParameters& cutParams)
    {
        // Replaced Direct calls to butterworthFilterDesign with the following functions with customQ
        if (cutParams.isLowcut)
            return customQLowCutCoefficients(cutParams.frequency, cutParams.sampleRate, cutParams.order, cutParams.quality);

        return customQHighCutCoefficients(cutParams.frequency, cutParams.sampleRate, cutParams.order, cutParams.quality);
        
        
        
    }
    
    static juce::ReferenceCountedArray<IIRCoeffs> customQHighCutCoefficients (const FloatType frequency, const double sampleRate, const int order, const FloatType targetQ)
    {
        using namespace juce;
        
        jassert (sampleRate > 0);
        jassert (frequency > 0 && frequency <= sampleRate * 0.5);
        jassert (order > 0);

        ReferenceCountedArray<dsp::IIR::Coefficients<FloatType>> arrayFilters;

        auto qFactor = pow( targetQ * sqrt(2.0), 1.0 / (FloatType)order );

        DBG(qFactor);
        
        if (order % 2 == 1)
        {
            arrayFilters.add (*dsp::IIR::Coefficients<FloatType>::makeFirstOrderLowPass (sampleRate, frequency));

            for (int i = 0; i < order / 2; ++i)
            {
                auto Q = qFactor / (2.0 * std::cos ((i + 1.0) * MathConstants<double>::pi / order));
                arrayFilters.add (*dsp::IIR::Coefficients<FloatType>::makeLowPass (sampleRate, frequency,
                                                                              static_cast<FloatType> (Q)));
            }
        }
        else
        {
            for (int i = 0; i < order / 2; ++i)
            {
                auto Q = qFactor / (2.0 * std::cos ((2.0 * i + 1.0) * MathConstants<double>::pi / (order * 2.0)));
                arrayFilters.add (*dsp::IIR::Coefficients<FloatType>::makeLowPass (sampleRate, frequency,
                                                                              static_cast<FloatType> (Q)));
            }
        }

        return arrayFilters;
      
    }
    
    static juce::ReferenceCountedArray<IIRCoeffs> customQLowCutCoefficients (const FloatType frequency, const double sampleRate, const int order, const FloatType targetQ)
    {
        using namespace juce;
        
        jassert (sampleRate > 0);
        jassert (frequency > 0 && frequency <= sampleRate * 0.5);
        jassert (order > 0);

        ReferenceCountedArray<dsp::IIR::Coefficients<FloatType>> arrayFilters;

        auto qFactor = pow( targetQ * sqrt(2.0), 1.0 / (FloatType)order );
        
        DBG(qFactor);
        
        if (order % 2 == 1)
        {
            arrayFilters.add (*dsp::IIR::Coefficients<FloatType>::makeFirstOrderHighPass (sampleRate, frequency));
        
            for (int i = 0; i < order / 2; ++i)
            {
                auto Q = qFactor / (2.0 * std::cos ((i + 1.0) * MathConstants<double>::pi / order));
                arrayFilters.add (*dsp::IIR::Coefficients<FloatType>::makeHighPass (sampleRate, frequency,
                                                                               static_cast<FloatType> (Q)));
            }
        }
        else
        {
            for (int i = 0; i < order / 2; ++i)
            {
                auto Q = qFactor / (2.0 * std::cos ((2.0 * i + 1.0) * MathConstants<double>::pi / (order * 2.0)));
                arrayFilters.add (*dsp::IIR::Coefficients<FloatType>::makeHighPass (sampleRate, frequency,
                                                                               static_cast<FloatType> (Q)));
            }
        }

        return arrayFilters;
    }
        
};

