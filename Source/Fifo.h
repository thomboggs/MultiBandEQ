/*
  ==============================================================================

    Fifo.h
    Created: 16 Oct 2021
    Author:  Thomas Boggs   

  ==============================================================================
*/

#pragma once
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"


//=============================================================================
/*
 Template Specialization For Push() Function
 */
using Coefficients = typename juce::dsp::IIR::Filter<float>::CoefficientsPtr;
using IIRCoeffs = juce::dsp::IIR::Coefficients<float>;

template <typename T>
struct IsReferenceCountedObjectPtr : std::false_type { };

template <>
struct IsReferenceCountedObjectPtr<Coefficients> : std::true_type { };

template <>
struct IsReferenceCountedObjectPtr<IIRCoeffs> : std::true_type { };


//=============================================================================
/*
 Fifo
 */
template <typename T, size_t Size>
struct Fifo
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
    
    /*
     If T is a ref-counted object:
     - is_same_v?? todo
     - Make sure that refcount doesn't go to 0
     
     Otherwise, push as normal.
     */
    bool push(const T& t)
    {
        auto writeHandle = fifo.write (1);
        
        if (writeHandle.blockSize1 > 0)
        {
            // Cast the writeindex to size_t, because the return type is int
            size_t index = static_cast<size_t>(writeHandle.startIndex1);
            
            if constexpr (IsReferenceCountedObjectPtr<T>::value)
            {
                auto currentBuf = myBuffers[index];
                jassert(currentBuf.getReferenceCount() > 1);
                
                myBuffers[index] = t;
                return true;
            }
            myBuffers[index] = t;
            return true;
        }
        return false;
    }
    
    bool pull(T& t)
    {
        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0)
        {
            // Cast the readindex to size_t, because the return type is int
            size_t index = static_cast<size_t>(readHandle.readIndex1);
            t = myBuffers[index];
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


