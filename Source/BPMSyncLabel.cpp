/*
  ==============================================================================

    BPMSyncLabel.cpp
    Created: 3 Aug 2020 2:25:11pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BPMSyncLabel.h"
#include "DTInterfaceDefines.h"

//==============================================================================
BPMSyncLabel::BPMSyncLabel(BPMManager::BPMLabelValues& _labels):
mWholeNoteLabel("", _labels.mWholeNoteValue), mDenominatorLabel("", _labels.mDenominatorValue), mNumeratorLabel("", _labels.mNumeratorValue)
{
    addAndMakeVisible(&mWholeNoteLabel);
    addAndMakeVisible(&mDenominatorLabel);
    addAndMakeVisible(&mNumeratorLabel);
    
    mWholeNoteLabel.setOverriden(false);
    mDenominatorLabel.setOverriden(false);
    mNumeratorLabel.setOverriden(false);
    
    mWholeNoteLabel.setJustificationType(Justification::centred);
    mDenominatorLabel.setJustificationType(Justification::centred);
    mNumeratorLabel.setJustificationType(Justification::centred);
    
    mWholeNoteLabel.setEditable(false);
    mDenominatorLabel.setEditable(false);
    mNumeratorLabel.setEditable(false);
}
BPMSyncLabel::~BPMSyncLabel()
{
}

void BPMSyncLabel::paint (juce::Graphics& g)
{
    g.setColour(DTColourIds::SmlDisplayGlow);
    g.drawLine(static_cast<float>(getWidth()/2)+10.f, static_cast<float>(getHeight()/2), static_cast<float>(getWidth())-10.f, static_cast<float>(getHeight()/2), 1.f);
}

void BPMSyncLabel::resized()
{
    mWholeNoteLabel.setBounds(0, 0, getWidth()/2, getHeight());
    mNumeratorLabel.setBounds(mWholeNoteLabel.getRight(), 0, getWidth()/2, getHeight()/2);
    mDenominatorLabel.setBounds(mNumeratorLabel.getX(), mNumeratorLabel.getBottom(), getWidth()/2, getHeight()/2);

}
 void BPMSyncLabel::setNewValues(BPMManager::BPMLabelValues &_labels)
{
    mWholeNoteLabel.setUnerlyingValue(_labels.mWholeNoteValue);
    mDenominatorLabel.setUnerlyingValue(_labels.mDenominatorValue);
    mNumeratorLabel.setUnerlyingValue(_labels.mNumeratorValue);
}
