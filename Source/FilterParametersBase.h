/*
  ==============================================================================

    FilterParametersBase.h
    Created: 10 Oct 2021 9:44:34pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "FilterInfo.h"


struct FilterParametersBase
{
    float frequency { 100.f };
    bool bypassed { false };
    float quality { 1.0 };
    double sampleRate { 0 };
};

struct FilterParameters : FilterParametersBase
{
    FilterParameters () {}
    // Copy Constructor
    FilterParameters (const FilterParameters& params)
    {
        frequency = params.frequency;
        bypassed = params.bypassed;
        quality = params.quality;
        sampleRate = params.sampleRate;
        filterType = params.filterType;
        gainInDb = params.gainInDb;
    }
    
    bool isEqual (const FilterParameters& params)
    {
        return  (frequency != params.frequency) ||
                (bypassed != params.bypassed) ||
                (quality != params.quality) ||
                (sampleRate != params.sampleRate) ||
                (filterType != params.filterType) ||
                (gainInDb != params.gainInDb);
    }
    
    // Member Variables
    FilterInfo::FilterType filterType { FilterInfo::FilterType::LowPass };
    float gainInDb { 0 };
    
    
};

struct HighCutLowCutParameters : FilterParametersBase
{
    HighCutLowCutParameters () {}
    
    // Copy Constructor
    HighCutLowCutParameters (const HighCutLowCutParameters& params)
    {
        frequency = params.frequency;
        bypassed = params.bypassed;
        quality = params.quality;
        sampleRate = params.sampleRate;
        order = params.order;
        isLowcut = params.isLowcut;
    }
    
    bool isEqual (const HighCutLowCutParameters& params)
    {
        return  (frequency != params.frequency) ||
                (bypassed != params.bypassed) ||
                (quality != params.quality) ||
                (sampleRate != params.sampleRate) ||
                (order != params.order) ||
                (isLowcut != params.isLowcut);
    }
    
    // Member Variables
    int order { 1 };
    bool isLowcut { true };
};


