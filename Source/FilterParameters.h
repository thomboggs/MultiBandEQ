/*
  ==============================================================================

    FilterParameters.h
    Created: 10 Oct 2021 9:45:21pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "FilterParametersBase.h"
#include "FilterInfo.h"


struct FilterParameters : FilterParametersBase
{
    FilterInfo::FilterType filterType { FilterInfo::FilterType::LowPass };
    
    float gainInDb { 0 };
};
