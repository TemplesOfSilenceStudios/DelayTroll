/*
  ==============================================================================

    LFOWaveManagementPanel.cpp
    Created: 26 Apr 2019 4:13:38pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LFOWaveManagementPanel.h"
#include "DTModulationDefines.h"
#include "DTHelperFunctions.h"
#include "DTUsedParameters.h"


//==============================================================================
LFOWaveManagementPanel::LFOWaveManagementPanel(DelayTrollAudioProcessor& p, int _assigningIndex, int _width, int _height, ChangeListener* inListener):
PanelBase(p),
mLFOPlot(*(static_cast<LFOPlotter*>(p.getModulationArray()[_assigningIndex]))),
mBPMManager(mLFOPlot.getBPMManager()),
mPlusOn(ImageCache::getFromMemory(BinaryData::PlusDown_png, BinaryData::PlusDown_pngSize)),
mPlusOff(ImageCache::getFromMemory(BinaryData::PlusUp_png, BinaryData::PlusUp_pngSize)),
mMinusOn(ImageCache::getFromMemory(BinaryData::MinusDown_png, BinaryData::MinusDown_pngSize)),
mMinusOff(ImageCache::getFromMemory(BinaryData::MinusUp_png, BinaryData::MinusUp_pngSize)),
mPlusButton("+", mPlusOn, mPlusOff),
mLFOSelectionImage(ImageCache::getFromMemory(BinaryData::DropDownE_png, BinaryData::DropDownE_pngSize)),
mLFOSelectionGlImage(ImageCache::getFromMemory(BinaryData::DropDownEGl_png, BinaryData::DropDownEGl_pngSize)),
mGearImage(ImageCache::getFromMemory(BinaryData::GearE_png, BinaryData::GearE_pngSize)),
mPeriodLable(ImageCache::getFromMemory(BinaryData::PeriodLable_png, BinaryData::PeriodLable_pngSize)),
mPhaseLable(ImageCache::getFromMemory(BinaryData::PhaseLable_png, BinaryData::PhaseLable_pngSize)),
mAmpLable(ImageCache::getFromMemory(BinaryData::AmpLable_png, BinaryData::AmpLable_pngSize))
{
    
    addChangeListener(inListener);
    
    setSize(_width, _height);
    
    Rectangle<int> initialBounds(0,0,_width, _height);
    
    sixboxes = DTsetSubBox(initialBounds, 3, 2);
    sixboxes.set(0, sixboxes[0].withHeight(mLFOSelectionImage.getHeight()));
   
    sixboxes.set(1, sixboxes[1].withHeight(mLFOSelectionImage.getHeight()));
    sixboxes.set(2, sixboxes[2].withHeight(mLFOSelectionImage.getHeight()));
    
    sixboxes.set(3, Rectangle<int>(sixboxes[3].getX(), sixboxes[0].getBottom(), sixboxes[3].getWidth(), mGearImage.getWidth() + mGearImage.getWidth()/2 ));
   
    sixboxes.set(4, Rectangle<int>(sixboxes[4].getX(), sixboxes[1].getBottom(), sixboxes[4].getWidth(),  mGearImage.getWidth()+ mGearImage.getWidth()/2));
    sixboxes.set(5, Rectangle<int>(sixboxes[5].getX(), sixboxes[2].getBottom(), sixboxes[5].getWidth(),  mGearImage.getWidth()+ mGearImage.getWidth()/2));
    
    //sixboxes.set(6, Rectangle<int>(sixboxes[6].getX(), sixboxes[3].getBottom(), sixboxes[6].getWidth(), sixboxes[6].getHeight()));
    
    //sixboxes.set(7, Rectangle<int>(sixboxes[7].getX(), sixboxes[4].getBottom(), sixboxes[7].getWidth(), sixboxes[7].getHeight()));
    //sixboxes.set(8, Rectangle<int>(sixboxes[8].getX(), sixboxes[5].getBottom(), sixboxes[8].getWidth(), sixboxes[8].getHeight()));
    
    mInitialBoxes = sixboxes;

    
    mNumOfRows = mLFOPlot.getNumWaves();
    for(int i = 0 ; i < mNumOfRows; i++){
        addRow(i);
    }
    
    addAndMakeVisible(&mPlusButton);

    
    mPlusButton.onClick = [this]()
    {
        mLFOPlot.addWave(1.0f, 0.0f, 1.0f, kLFO_Sine);
        this->addRow(mNumOfRows);
        mNumOfRows++;
    };
    
}

LFOWaveManagementPanel::~LFOWaveManagementPanel()
{
}
void LFOWaveManagementPanel::addRow(int _newRowIndex)
{
    mMinusButtons.add(new DTButton("-", mMinusOn, mMinusOff));
   // mMinusButtons[_newRowIndex]->setBounds(sixboxes[0]);
    mMinusButtons[_newRowIndex]->setCentrePosition(sixboxes[0].getCentre().translated(0,3));
    mMinusButtons[_newRowIndex]->addListener(this);
    addAndMakeVisible(mMinusButtons[_newRowIndex]);
    
    mLFOSelection.add(new DTParamComboBox("LfoSelection", mLFOSelectionImage));
    
    for(int j = 0 ; j < kLFO_TotalNumOfLFOTypes; j++){
        mLFOSelection[_newRowIndex]->addItem(DTLFOTypeIDs[j], j+1);
    }
    //mLFOSelection[_newRowIndex]->setJustificationType(Justification::centred);
    mLFOSelection[_newRowIndex]->setTextWhenNothingSelected("Wave type");
    //mLFOSelection[i]->addListener(this);
    mLFOSelection[_newRowIndex]->setSelectedItemIndex(mLFOPlot.getWaveForm(_newRowIndex)->getWaveType());
    //mLFOSelection[_newRowIndex]->setBounds(DTextendRecto(sixboxes[1], sixboxes[2]));
    mLFOSelection[_newRowIndex]->setCentrePosition(DTextendRecto(sixboxes[1], sixboxes[2]).getCentre());
    mLFOSelection[_newRowIndex]->addListener(this);
    addAndMakeVisible(mLFOSelection[_newRowIndex]);
    
    mLFOSelectionGl.add(new DrawableImage(mLFOSelectionGlImage));
    mLFOSelectionGl[_newRowIndex]->setSize(mLFOSelectionGlImage.getWidth(), mLFOSelectionGlImage.getHeight());
    mLFOSelectionGl[_newRowIndex]->setCentrePosition(DTextendRecto(sixboxes[1], sixboxes[2]).getCentre());
    mLFOSelectionGl[_newRowIndex]->setBoundingBox(mLFOSelectionGl[_newRowIndex]->getBounds().toFloat());
    addAndMakeVisible(mLFOSelectionGl[_newRowIndex]);
    
    mPeriodLables.add(new DrawableImage());
    mPeriodLables[_newRowIndex]->setImage(mPeriodLable);
    mPeriodLables[_newRowIndex]->setSize(mPeriodLable.getWidth(), mPeriodLable.getHeight());
    mPeriodLables[_newRowIndex]->setCentrePosition(sixboxes[3].getCentre().translated(1,0));
    mPeriodLables[_newRowIndex]->setBoundingBox(mPeriodLables[_newRowIndex]->getBounds().toFloat());
    addAndMakeVisible(mPeriodLables[_newRowIndex]);
    
    float period = mLFOPlot.getWaveForm(_newRowIndex)->getPeriod();
    mPeriodSliders.add(new DTParameterSlider(NormalisableRange<double>(1.0f, 10.0f, 1), period, "Period", mGearImage));
    //mPeriodSliders[_newRowIndex]->setBounds(DTRemoveLabelBumper(sixboxes[3]));
    mPeriodSliders[_newRowIndex]->setCentrePosition(sixboxes[3].getCentre().translated(0,-10));
    mPeriodSliders[_newRowIndex]->addListener(this);
    addAndMakeVisible(mPeriodSliders[_newRowIndex]);
    
    mAmpLables.add(new DrawableImage());
    mAmpLables[_newRowIndex]->setImage(mAmpLable);
    mAmpLables[_newRowIndex]->setSize(mAmpLable.getWidth(), mAmpLable.getHeight());
    mAmpLables[_newRowIndex]->setCentrePosition(sixboxes[4].getCentre().translated(0,-1));
    mAmpLables[_newRowIndex]->setBoundingBox(mAmpLables[_newRowIndex]->getBounds().toFloat());
    addAndMakeVisible(mAmpLables[_newRowIndex]);
    
    float amp = mLFOPlot.getWaveForm(_newRowIndex)->getAmp();
    mAmplitudeSliders.add(new DTParameterSlider(NormalisableRange<double>(-1.0f, 1.0f), amp, "Amplitude", mGearImage));
    //mAmplitudeSliders[_newRowIndex]->setBounds(DTRemoveLabelBumper(sixboxes[4]));
    mAmplitudeSliders[_newRowIndex]->setCentrePosition(sixboxes[4].getCentre().translated(0,-10));
    mAmplitudeSliders[_newRowIndex]->addListener(this);
    addAndMakeVisible(mAmplitudeSliders[_newRowIndex]);
    
    mPhaseLables.add(new DrawableImage());
    mPhaseLables[_newRowIndex]->setImage(mPhaseLable);
    mPhaseLables[_newRowIndex]->setSize(mPhaseLable.getWidth(), mPhaseLable.getHeight());
    mPhaseLables[_newRowIndex]->setCentrePosition(sixboxes[5].getCentre().translated(0,-1));
    mPhaseLables[_newRowIndex]->setBoundingBox(mPhaseLables[_newRowIndex]->getBounds().toFloat());
    addAndMakeVisible(mPhaseLables[_newRowIndex]);
    
    float phase = mLFOPlot.getWaveForm(_newRowIndex)->getPhase();
    mPhaseSliders.add(new DTParameterSlider(NormalisableRange<double>(0.0f, 1.0f), phase, "Phase", mGearImage));
    //mPhaseSliders[_newRowIndex]->setBounds(DTRemoveLabelBumper(sixboxes[5]));
    mPhaseSliders[_newRowIndex]->setCentrePosition(sixboxes[5].getCentre().translated(0,-10));
    mPhaseSliders[_newRowIndex]->addListener(this);
    addAndMakeVisible(mPhaseSliders[_newRowIndex]);
    
    
    
    
    int translationHight = sixboxes[0].getHeight() + sixboxes[3].getHeight();
    for(int j = 0 ; j < 6; j++){

        sixboxes.set(j, sixboxes[j].translated(0, translationHight));
    }
    //sixboxes.removeRange(0, 9);
    
//    mPlusButton.setBounds(sixboxes[0]);
    mPlusButton.setCentrePosition(sixboxes[0].getCentre());
    setSize(getWidth(), mPlusButton.getBottom());

}
void LFOWaveManagementPanel::removeRow(int _rowIndex)
{
    mPeriodSliders.clear();
    mPeriodLables.clear();
    mAmplitudeSliders.clear();
    mAmpLables.clear();
    mPhaseSliders.clear();
    mPhaseLables.clear();
    mMinusButtons.clear();
    mLFOSelection.clear();
    mLFOSelectionGl.clear();
    mLFOPlot.removeWave(_rowIndex);
    mNumOfRows--;
    sixboxes = mInitialBoxes;
    for(int i = 0 ; i < mNumOfRows; i++){
        addRow(i);
    }
    
}
void LFOWaveManagementPanel::paint (Graphics& g)
{
//g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}

void LFOWaveManagementPanel::resized()
{

}
void LFOWaveManagementPanel::buttonClicked (Button* b)
{
    for(int i = 0 ; i < mMinusButtons.size(); i++){
        
        if(b == mMinusButtons[i]){
            if(mMinusButtons.size() > 1)
                removeRow(i);
        }
    }
    sendChangeMessage();
}
void LFOWaveManagementPanel::sliderValueChanged(juce::Slider* slider)
{
    for(int i = 0 ; i < mMinusButtons.size(); i++){
        if(slider == mPeriodSliders[i] || mAmplitudeSliders[i] || mPhaseSliders[i]){
            mLFOPlot.updateWave(i,
                                mPeriodSliders[i]->getValue(),
                                mAmplitudeSliders[i]->getValue(),
                                mPhaseSliders[i]->getValue(),
                                mLFOPlot.getWaveForm(i)->getWaveType());
            sendChangeMessage();
        }
    }
}
void LFOWaveManagementPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    for(int i = 0 ; i < mMinusButtons.size(); i++){
        if(comboBoxThatHasChanged == mLFOSelection[i]){
            mLFOPlot.updateWave(i,
                                mPeriodSliders[i]->getValue(),
                                mAmplitudeSliders[i]->getValue(),
                                mPhaseSliders[i]->getValue(),
                                comboBoxThatHasChanged->getSelectedItemIndex());
            sendChangeMessage();
        }
    }
}
