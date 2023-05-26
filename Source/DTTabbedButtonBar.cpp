/*
  ==============================================================================

    DTTabbedButtonBar.cpp
    Created: 22 Apr 2019 4:52:18pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTTabbedButtonBar.h"

DTTabbedButtonBar::DTTabbedButtonBar(const String& buttonLabel, const Image& _image)
:   juce::TabbedButtonBar(TabbedButtonBar::TabsAtTop), mbackgroundImage(_image)
{
    setSize(_image.getWidth(), _image.getHeight());
    setColour(TabbedButtonBar::ColourIds::tabOutlineColourId, Colours::transparentWhite);
    //setColour(TabbedButtonBar::ColourIds::frontOutlineColourId, Colours::black);
}

DTTabbedButtonBar::~DTTabbedButtonBar()
{
}



