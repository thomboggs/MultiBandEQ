/*
  ==============================================================================

    Decibel.h
    Created: 15 Oct 2021 7:28:19pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include <cmath>


template <typename FloatType>
struct Decibel
{
    // Constructors
    // default constructor
    Decibel () {};
    // a constructor that takes a FloatType parameter to initialize your member variable.
    Decibel (FloatType inputGainInDb) : gainInDb(inputGainInDb) {};
    
    // Rule of 3
    // Copy Constructor
    Decibel (const Decibel& decibel) : gainInDb(decibel.gainInDb) {};
    
    // Copy Assignment Operator
    Decibel& operator=(const Decibel& other)
    {
        if (this != &other)
        {
            gainInDb = other.gainInDb;
        }

        return *this;
    }
    
    // Destructor
    ~Decibel() {};
    
    // Math Operators
    Decibel& operator+=(const Decibel& other)
    {
        this->gainInDb = this->gainInDb + other.gainInDb;
        return *this;
    }
    
    Decibel& operator-=(const Decibel& other)
    {
        this->gainInDb = this->gainInDb - other.gainInDb;
        return *this;
    }
    
    Decibel& operator*=(const Decibel& other)
    {
        this->gainInDb = this->gainInDb * other.gainInDb;
        return *this;
    }
    
    Decibel& operator/=(const Decibel& other)
    {
        this->gainInDb = this->gainInDb / other.gainInDb;
        return *this;
    }
    
    
    friend Decibel operator+ (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb + rhs.gainInDb);
    }

    
    friend Decibel operator- (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb - rhs.gainInDb);
    }

    friend Decibel operator* (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb * rhs.gainInDb);
    }
    
    friend Decibel operator/ (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb / rhs.gainInDb);
    }

    // Comparison Operators - these should be friend functions
    friend bool operator==(const Decibel& lhs, const Decibel& rhs)
    {
        return lhs.gainInDb == rhs.gainInDb;
    }

    friend bool operator!=(const Decibel& lhs, const Decibel& rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator<(const Decibel& lhs, const Decibel& rhs)
    {
        return lhs.gainInDb < rhs.gainInDb;
    }

    friend bool operator>(const Decibel& lhs, const Decibel& rhs)
    {
        return lhs.gainInDb > rhs.gainInDb;
    }

    friend bool operator<=(const Decibel& lhs, const Decibel& rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const Decibel& lhs, const Decibel& rhs)
    {
        return !(lhs < rhs);
    }
    
    
    // Setters and getters
    FloatType getGain() const
    {
        return std::pow(10.f, gainInDb / 20.f);
    }

    FloatType getDb() const
    {
        return gainInDb;
    }

    void setGain(FloatType g)
    {
        gainInDb = 20.f * std::log10(g);
    }

    void setDb(FloatType db)
    {
        gainInDb = db;
    }
    
    
private:
    FloatType gainInDb {0.f};
};

//=========================================================================================
//template <typename FloatType>
//Decibel<FloatType> operator+ (const Decibel<FloatType>& lhs, const Decibel<FloatType>& rhs)
//{
//    return Decibel(lhs.gainInDb + rhs.gainInDb);
//}

//template <typename FloatType>
//Decibel<FloatType> operator- (const Decibel<FloatType>& lhs, const Decibel<FloatType>& rhs)
//{
//    return Decibel(lhs.gainInDb - rhs.gainInDb);
//}

//template <typename FloatType>
//Decibel<FloatType> operator* (const Decibel<FloatType>& lhs, const Decibel<FloatType>& rhs)
//{
//    return Decibel(lhs.gainInDb * rhs.gainInDb);
//}

//template <typename FloatType>
//Decibel<FloatType> operator/ (const Decibel<FloatType>& lhs, const Decibel<FloatType>& rhs)
//{
//    return Decibel(lhs.gainInDb / rhs.gainInDb);
//}

//friend bool operator==(const Decibel& lhs, const Decibel& rhs);
//
//friend bool operator!=(const Decibel& lhs, const Decibel& rhs);
//
//friend bool operator<(const Decibel& lhs, const Decibel& rhs);
//
//friend bool operator>(const Decibel& lhs, const Decibel& rhs);
//
//friend bool operator<=(const Decibel& lhs, const Decibel& rhs);
//
//friend bool operator>=(const Decibel& lhs, const Decibel& rhs);


/*
Implement a Decibel<FloatType> class

the purpose of this class is so you don't have to remember to convert decibels to gain units

It is essentially a wrapper around a FloatType member variable, which is private

========Implement:

DONE Rule of 3

DONE math operators for when this object is on the left side of the +=, -=, etc

DONE math operators for when this object is on the right side of the =: auto v = gain1 + gain2;   These should be friend functions

DONE comparison operators.  these should be friend functions

DONE FloatType getGain() const

DONE FloatType getDb() const

DONE setGain(FloatType g)

DONE setDb(FloatType db)

======== 2 constructors:

DONE default constructor

DONE a constructor that takes a FloatType parameter to initialize your member variable.

 
 
 
 
 
 
Why is this class being created:

This class will be used in your FilterParameters class.

It will also be used in the FilterLink class, as the template type for a juce::SmoothedValue<>  The use within SmoothedValue is why you must implement math operators and comparison operators.  The SmoothedValue instance will handle smoothing changes to the filter band’s gain setting.

*/
