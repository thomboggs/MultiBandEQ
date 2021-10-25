/*
  ==============================================================================

    ReleasePool.h
    Created: 24 Oct 2021 9:00:14pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Fifo.h"


template<typename ReferenceCountedType>
struct ReleasePool : juce::Timer
{
    using Ptr = ReferenceCountedType;

    ReleasePool()
    {
        // set the size of the deletion pool
        deletionPool.reserve(1024);
        
        // Start the timer
        startTimer(2000);
    }
    
    ~ReleasePool()
    {
        stopTimer();
    }
    
    void add(Ptr ptr);
    void timerCallback() override;
    
private:
    void addIfNotAlreadyThere(Ptr ptr);
    std::vector<Ptr> deletionPool;
    Fifo<Ptr, 1024> releaseFifo;
    juce::Atomic<bool> addedToFifo;
};
