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
    FilterInfo::FilterType filterType { FilterInfo::FilterType::LowPass };
    
    float gainInDb { 0 };
};

struct HighCutLowCutParameters : FilterParametersBase
{
    int order { 1 };
    bool isLowcut { true };
};

