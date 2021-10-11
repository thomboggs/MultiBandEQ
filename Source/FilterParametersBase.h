/*
  ==============================================================================

    FilterParametersBase.h
    Created: 10 Oct 2021 9:44:34pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once


struct FilterParametersBase
{
    float frequency { 100.f };
    bool bypassed { false };
    float quality { 1.0 };
    double sampleRate { 0 };
};
