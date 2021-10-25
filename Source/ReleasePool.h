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
    
    void add(Ptr ptr)
    {
        // Toggle addedToFifo to true
        
        // To check if on messagethread: use [static bool existsAndIsCurrentThread () noexcept]
    }
    
    void timerCallback() override
    {
        // If successfully added
        if ( addedToFifo.compareAndSetBool(false, true) )
        {
            Ptr ptr;
            
            while ( releaseFifo.pull(ptr) )
            {
                // Check if ptr points to real object
                if (ptr != nullptr)
                {
                    // Check if ptr already exists in the pool
                    addIfNotAlreadyThere(ptr);
                    ptr.reset();
                }
            }
        }

        // Delete Everything in the pool with a RefCount <= 1
        
        
    }
    
private:
    void addIfNotAlreadyThere(Ptr ptr)
    {
        auto alreadyExists = [ptr] (Ptr poolPtr) {return ptr == poolPtr;};
        
        if ( std::find_if(deletionPool.begin(), deletionPool.end(), alreadyExists) == deletionPool.end() )
        {
            // Doesn't exist, add to pool
            deletionPool.push_back(ptr);
        }
    }
    std::vector<Ptr> deletionPool;
    Fifo<Ptr, 1024> releaseFifo;
    juce::Atomic<bool> addedToFifo;
};
