/*
  ==============================================================================

    Fifo.h
    Created: 16 Oct 2021 5:29:03pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"


template <typename T, size_t Size>
struct Fifo : std::false_type
{
    Fifo () : bufferSize(Size) {}
    
    size_t getSize() { return bufferSize; }
    
    //used when T is AudioBuffer<float>
    void prepare(int numSamples, int numChannels)
    {
        static_assert(std::is_same_v<T, juce::AudioBuffer<float>>, "Fifo Template Type should be juce::AudioBuffer<float> if using prepare with two arguments.");
        
        /*
         Steps to Prepare for AudioBuffer
         - Loop Through myBuffers
         - Initialize each audioBuffer to correct size
         - Clear Data inside each buffer in the array of buffers
         */
        for (auto& buffer : myBuffers)
        {
            buffer.setSize(numChannels,
                           numSamples,
                           false,
                           true,
                           true);
            buffer.clear();
        }
    }
    
    //used when T is std::vector<float>
    void prepare(size_t numElements)
    {
        static_assert(std::is_same_v<T, std::vector<float>>, "Fifo Template Type should be std::vector<float> if using prepare with one input argument.");
        
        /*
         Steps to prepare std::vector<float> buffertype
         - Loop through all the buffers in myBuffers
         - Set the vector to the correct size
         - initialize all data to 0's
         */
        for (auto& buffer : myBuffers)
        {
            buffer.clear(); // Leaves vector with size=0
            buffer.setSize(numElements, 0); // set vector to size numElements and init to 0
        }
    }
    
    bool push(const T& t)
    {
        /*
         If T is a ref-counted object:
            - Can be cast to RefCountedObjectPtr
         - Make sure that refcount doesn't go to 0
         
         Otherwise, push as normal.
         */
        auto writeHandle = fifo.write (1);
        
        if (writeHandle.blockSize1 > 0)
        {
            if (Fifo<T, Size>::value)
            {
                auto currentBuf = myBuffers[writeHandle.startIndex1];
                jassert(currentBuf.getReferenceCount() > 1);
                
                myBuffers[writeHandle.startIndex1] = t;
                return true;
            }
            
            myBuffers[writeHandle.startIndex1] = t;
            return true;
        }
        return false;
    }
    
    bool pull(T& t)
    {
        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0)
        {
            t = myBuffers[readHandle.readIndex1];
            return true;
        }
        return false;
    }
    
    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }
    
    int getAvailableSpace() const
    {
        return fifo.getFreeSpace();
    }

private:
    juce::AbstractFifo fifo { Size };
    std::array<T, Size> myBuffers;
    size_t bufferSize;
};

//==============================================================================
// Template specialization for ReferenceCountedObjects

using Coefficients = typename juce::dsp::IIR::Filter<float>::CoefficientsPtr;
template <size_t Size>
struct Fifo<Coefficients, Size> : std::true_type { };

using IIRCoeffs = juce::dsp::IIR::Coefficients<float>;
template <size_t Size>
struct Fifo<IIRCoeffs, Size> : std::true_type { };
