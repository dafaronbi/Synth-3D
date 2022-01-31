/*
  ==============================================================================

    sliders.h
    Created: 6 Mar 2021 4:10:15pm
    Author:  Daniel Faronbi

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//special gain slider
class gain_slider : public juce::Slider{
public:
    gain_slider(){}
    
    juce::String getTextFromValue (double value) override
    {
        return juce::Decibels::toString (value);
    }
    
    double getValueFromText (const juce::String& text) override
    {
        auto minusInfinitydB = -100.0;
 
        auto decibelText = text.upToFirstOccurrenceOf ("dB", false, false).trim();    // [1]
 
        return decibelText.equalsIgnoreCase ("-INF") ? minusInfinitydB
                                                     : decibelText.getDoubleValue();  // [2]
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (gain_slider)
};
