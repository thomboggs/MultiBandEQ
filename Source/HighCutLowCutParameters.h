/*
  ==============================================================================

    HighCutLowCutParameters.h
    Created: 10 Oct 2021 9:45:42pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "FilterParametersBase.h"


struct HighCutLowCutParameters : FilterParametersBase
{
    int order { 1 };
    bool isLowcut { true };
};
