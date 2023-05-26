/*
  ==============================================================================

    DTArrowButton.h
    Created: 22 Apr 2019 6:49:00pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class DTArrowButton    : public ArrowButton
{
public:
    DTArrowButton(const String& _name);
    ~DTArrowButton();


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTArrowButton)
};
