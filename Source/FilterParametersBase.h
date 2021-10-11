/*
  ==============================================================================

    FilterParametersBase.h
    Created: 10 Oct 2021 9:44:34pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


struct FilterParametersBase
{
    float frequency { 100.f };
    bool bypassed { false };
    float quality { 1.0 };
    double sampleRate { 0 };
};

// Free Functions to create Parameter Names
String getParamNameGain(int filterNum);
String getParamNameQuality(int filterNum);
String getParamNameFreq(int filterNum);
String getParamNameType(int filterNum);

