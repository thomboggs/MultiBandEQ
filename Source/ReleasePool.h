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
        deletionPool.reserve(capacity);
        
        // Start the timer
        startTimer(2000);
    }
    
    ~ReleasePool()
    {
        stopTimer();
    }
    
    void add(Ptr ptr)
    {
        // To check if on messagethread: use [static bool existsAndIsCurrentThread ()
        if ( juce::MessageManager::existsAndIsCurrentThread() )
        {
            addIfNotAlreadyThere(ptr);
            ptr.reset();
        }
        else
        {
            // Add to Fifo
            if (releaseFifo.push(ptr) )
            {
                // Toggle addedToFifo to true
                addedToFifo.set(true);
            }
            else
            {
                jassertfalse; // FIFO likely too small
            }
        }
    }
    
    void timerCallback() override
    {
        // If successfully added
        if ( addedToFifo.compareAndSetBool(false, true) )
        {
            Ptr ptr;
            
            // need to make this exchange
            while ( releaseFifo.exchange(std::move(ptr)) )
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
        deletionPool.erase( std::remove_if(deletionPool.begin(),
                                           deletionPool.end(),
                                           [] (Ptr ptr) { return ptr->getReferenceCount() <= 1; }),
                                   deletionPool.end() );
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
    static const size_t capacity { 1024 };
    std::vector<Ptr> deletionPool;
    Fifo<Ptr, capacity> releaseFifo;
    juce::Atomic<bool> addedToFifo;
};
