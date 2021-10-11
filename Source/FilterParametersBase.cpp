/*
  ==============================================================================

    FilterParametersBase.cpp
    Created: 10 Oct 2021 9:44:34pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "FilterParametersBase.h"


String getParamNameGain(int filterNum)
{
    String str;
    
    str << "Filter_";
    str << filterNum;
    str << "_gain";
    
    return str;
}

String getParamNameQuality(int filterNum)
{
    String str;
    
    str << "Filter_";
    str << filterNum;
    str << "_quality";
    
    return str;
}

String getParamNameFreq(int filterNum)
{
    String str;
    
    str << "Filter_";
    str << filterNum;
    str << "_freq";
    
    return str;
}

String getParamNameType(int filterNum)
{
    String str;
    
    str << "Filter_";
    str << filterNum;
    str << "_type";
    
    return str;
}
