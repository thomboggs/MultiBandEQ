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
    // LHS
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
    
    // RHS
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
