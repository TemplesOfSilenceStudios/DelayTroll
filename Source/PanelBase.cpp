/*
  ==============================================================================

    PanelBase.cpp
    Created: 22 Apr 2019 4:42:48pm
    Author:  Sam Ferguson

  ==============================================================================
*/


#include "PanelBase.h"

//==============================================================================
PanelBase::PanelBase(DelayTrollAudioProcessor& p): mProcessor(p)
{
    mLookAndFeel = std::make_unique<DTLookAndFeel>();
    //setLookAndFeel(mLookAndFeel.get());
    LookAndFeel::setDefaultLookAndFeel(mLookAndFeel.get());

}

PanelBase::~PanelBase()
{
    LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void PanelBase::paint (Graphics& g)
{
    
}

void PanelBase::resized()
{

}
