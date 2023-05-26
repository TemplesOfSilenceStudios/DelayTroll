/*
  ==============================================================================

    DTCheckBox.h
    Created: 27 Apr 2019 10:25:41am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class DTCheckBox    : public ToggleButton
{
public:
    DTCheckBox(String _label);
    ~DTCheckBox();


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTCheckBox)
};
