/*
  ==============================================================================

    DTTabbedButtonBar.h
    Created: 22 Apr 2019 4:52:18pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
 */
class DTTabbedButtonBar    : public TabbedButtonBar
{
public:
    DTTabbedButtonBar(const String& buttonLabel, const Image& _image);
    ~DTTabbedButtonBar();
    
    
private:
    
    const Image& mbackgroundImage;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTTabbedButtonBar)
};
