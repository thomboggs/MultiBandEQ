/*
  ==============================================================================

    FilterHelperFunctions.cpp
    Created: 11 Oct 2021 7:44:14pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "FilterHelperFunctions.h"


String getParamPrefix(int filterNum, String paramType)
{
    String str;
    
    str << "Filter_";
    str << filterNum;
    str << "_";
    str << paramType;
    return str;
}


String getGainParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Gain");
}

String getQualityParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Quality");
}

String getFreqParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Freq");
}

String getTypeParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Type");
}

String getBypassParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Bypass");
}
