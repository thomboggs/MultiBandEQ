/*
  ==============================================================================

    FilterHelperFunctions.h
    Created: 11 Oct 2021 7:44:14pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


// Free Functions to create Parameter Names
String getGainParamName(int filterNum);
String getQualityParamName(int filterNum);
String getFreqParamName(int filterNum);
String getTypeParamName(int filterNum);
String getBypassParamName(int filterNum);
