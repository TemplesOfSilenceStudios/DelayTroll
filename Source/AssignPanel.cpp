/*
  ==============================================================================

    AssignPanel.cpp
    Created: 23 Apr 2019 11:36:20am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "AssignPanel.h"
#include "DTHelperFunctions.h"
#include "DTModulationDefines.h"
#include "DTInterfaceDefines.h"
#include "DTUsedParameters.h"
//==============================================================================
AssignPanel::AssignPanel(DelayTrollAudioProcessor& p,  ModManagment& _modManager, int _width, int _height):
PanelBase(p),
mModManager(_modManager),
mAssignmentsImage(ImageCache::getFromMemory(BinaryData::DropDownD_png, BinaryData::DropDownD_pngSize)),
mAssignmentsGlImage(ImageCache::getFromMemory(BinaryData::DropDownDGl_png, BinaryData::DropDownDGl_pngSize)),
mPlusImageOn(ImageCache::getFromMemory(BinaryData::PlusDown_png, BinaryData::PlusDown_pngSize)),
mPlusImageOff(ImageCache::getFromMemory(BinaryData::PlusUp_png, BinaryData::PlusUp_pngSize)),
mMinusImageOn(ImageCache::getFromMemory(BinaryData::MinusDown_png, BinaryData::MinusDown_pngSize)),
mMinusImageOff(ImageCache::getFromMemory(BinaryData::MinusUp_png, BinaryData::MinusUp_pngSize)),
mPlusButton("+", mPlusImageOn, mPlusImageOff),
mGearImage(ImageCache::getFromMemory(BinaryData::GearD_png, BinaryData::GearD_pngSize)),
mDepthLable(ImageCache::getFromMemory(BinaryData::DepthLable_png, BinaryData::DepthLable_pngSize))
{
    
    setSize(_width, _height);
    
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    
    Rectangle<int> initialBounds(5, MODULATION_ASSIGN_PANEL_BUMPER, _width-5, _height - MODULATION_ASSIGN_PANEL_BUMPER);
    
    mThreeBoxes = DTsetSubBox(initialBounds, 4, 4);
    
    mInitialBoxes = mThreeBoxes;
    
    mNumOfRows = mModManager.getNumOfValues();
    
    addAndMakeVisible(&mPlusButton);
    
    for (int i = 0; i < kAss_TotalNumOFAssignableParams; i++) {
        mIsSelected.add(false);
    }
    
    
    if(mNumOfRows == 0){
        mAssigned.add(false);
        mModManager.attatchDummyValue();
        addRow(0);
        mNumOfRows++;
    }
    else{
        for(int i = 0 ; i < mNumOfRows; i++){
            mAssigned.add(!mModManager.getPlaceholderStatus(i));
            addRow(i);
        }
    }
    
    mPlusButton.onClick = [this]()
    {
        mAssigned.add(false);
        mModManager.attatchDummyValue();
        this->addRow(mNumOfRows);
        mNumOfRows++;
    };
    
}

AssignPanel::~AssignPanel()
{
    //mDepths[0]->setSliderStyle(nullptr);
}
void AssignPanel::addRow(int _newRowIndex)
{
    mMinusButtons.add(new DTButton("-", mMinusImageOn, mMinusImageOff));
    //mMinusButtons[_newRowIndex]->setBounds(mThreeBoxes[0]);
    mMinusButtons[_newRowIndex]->setCentrePosition(mThreeBoxes[0].getCentre().translated(-5, 0));
    mMinusButtons[_newRowIndex]->addListener(this);
    addAndMakeVisible(mMinusButtons[_newRowIndex]);
    
    mAssignments.add(new DTParamComboBox("Assignment", mAssignmentsImage));
    
    for(int j = 0 ; j < kAss_TotalNumOFAssignableParams; j++){
        mAssignments[_newRowIndex]->addItem(DTAssignableParamsName[j], j+1);
    }
    
    //mAssignments[_newRowIndex]->setJustificationType(Justification::centred);
    mAssignments[_newRowIndex]->setTextWhenNothingSelected("Parameter Assignment");

    if(mAssigned[_newRowIndex]){
        mAssignments[_newRowIndex]->setSelectedItemIndex(mModManager.getParamAtIndex(_newRowIndex));
        mIsSelected.set(mModManager.getParamAtIndex(_newRowIndex), true);
    }
    //mAssignments[_newRowIndex]->setBounds(DTextendRecto(mThreeBoxes[1], mThreeBoxes[2]));
    mAssignments[_newRowIndex]->setCentrePosition(DTextendRecto(mThreeBoxes[1], mThreeBoxes[2]).getCentre().translated(0, 0));
    mAssignments[_newRowIndex]->addListener(this);
    addAndMakeVisible(mAssignments[_newRowIndex]);
    
    mLastAssignedType.add(mAssignments[_newRowIndex]->getSelectedItemIndex());
    
    mAssignmentsGl.add(new DrawableImage(mAssignmentsGlImage));
    mAssignmentsGl[_newRowIndex]->setSize(mAssignmentsGlImage.getWidth(), mAssignmentsGlImage.getHeight());
    mAssignmentsGl[_newRowIndex]->setCentrePosition(DTextendRecto(mThreeBoxes[1], mThreeBoxes[2]).getCentre().translated(0, 0));
    mAssignmentsGl[_newRowIndex]->setBoundingBox(mAssignmentsGl[_newRowIndex]->getBounds().toFloat());
    addAndMakeVisible(mAssignmentsGl[_newRowIndex]);
    
    mDepthLables.add(new DrawableImage(mDepthLable));
    mDepthLables[_newRowIndex]->setSize(mDepthLable.getWidth()-2, mDepthLable.getHeight()-2);
    mDepthLables[_newRowIndex]->setCentrePosition(mThreeBoxes[3].getCentre().translated(4,-4));
    mDepthLables[_newRowIndex]->setBoundingBox(mDepthLables[_newRowIndex]->getBounds().toFloat());
    addAndMakeVisible(mDepthLables[_newRowIndex]);
    
    mDepths.add( new DTParameterSlider(mGearImage));
    mDepths[_newRowIndex]->setSliderStyle(Slider::RotaryVerticalDrag);
    mDepths[_newRowIndex]->setBounds(mThreeBoxes[3]);
    mDepths[_newRowIndex]->setCentrePosition(mThreeBoxes[3].getCentre().translated(5, 16));
    //mDepths[_newRowIndex]->setCentrePosition(mThreeBoxes[3].getCentre());
    mDepths[_newRowIndex]->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mDepths[_newRowIndex]->setRange(0.f, 1.f);
    
    //attached the underlying Value opject of the mDepth sliders to the ones in the mModManager the row is assigned to a parameter
    if(mAssigned[_newRowIndex]){
        mDepths[_newRowIndex]->getValueObject().referTo(*mModManager.getDepthValueObject(mAssignments[_newRowIndex]->getSelectedItemIndex()));
    }
    else{
        mDepths[_newRowIndex]->setValue(0.5);
    }

    //mDepths[_newRowIndex]->setLookAndFeel(&mDefaultLookAndFeel);
    addAndMakeVisible(mDepths[_newRowIndex]);
    
    //advance the mThreeBoxes by appending a new boxs to the end of the current boxes and then remove the old boxs
    for(int j = 0 ; j < 12; j++){
        Rectangle<int> tempBox(mThreeBoxes[j].getX(),
                               mThreeBoxes[j].getY() + mThreeBoxes[j].getHeight(),
                               mThreeBoxes[j].getWidth(),
                               mThreeBoxes[j].getHeight());
       
 
        mThreeBoxes.insert(12+j, tempBox);
    }
  
    mThreeBoxes.removeRange(0, 12);
    //mPlusButton.setBounds(mThreeBoxes[0]);
    mPlusButton.setCentrePosition(mThreeBoxes[0].getCentre().translated(-5, 0));
    setSize(getWidth(), mPlusButton.getBottom());
    
    for(auto boxes : mAssignments)
    {
        int index = 0;
        for(auto selected : mIsSelected)
        {
            boxes->setItemEnabled(index + 1, !selected);
            index++;
        }
        boxes->setItemEnabled(boxes->getSelectedId(), true);
    }
    
}
void AssignPanel::removeRow(int _rowIndex)
{

    mNumOfRows--;
    
    //if the mLastAssignedType == -1 then it's a dummy row and we dont need to deal the the assignableParam
    if(mLastAssignedType[_rowIndex] != -1){
        
        DTModulation::DTAssignableParam& assignableParam = *mProcessor.getAssignableParameters()[mLastAssignedType[_rowIndex]];
        if(mNumOfRows > 0){
            mModManager.removeValue(_rowIndex);
            mAssigned.remove(_rowIndex);
            assignableParam.modulationManagers.remove(mModManager.getManagerID());
            
        }
        else if(mNumOfRows == 0 && mAssigned[0] && _rowIndex == 0)
        {
            mModManager.removeValue(_rowIndex);
            mAssigned.set(_rowIndex, false);
            assignableParam.modulationManagers.remove(mModManager.getManagerID());
        }
        mIsSelected.set(mLastAssignedType[_rowIndex], false);
        
        //if there are nor more managers attatched to the parm then we need to clean some stuff up
        if (assignableParam.modulationManagers.size() == 0) {
            assignableParam.isAttached = false;
            assignableParam.modulatedValue  =var(assignableParam.sharedOrigninalValue.get());
            assignableParam.setValue(assignableParam.sharedOrigninalValue.get());
            
            mProcessor.parameterValueChanged(assignableParam.getParamIndex(), assignableParam.sharedOrigninalValue.get());
            
            if(assignableParam.typeIndex == kAss_MaxTime || assignableParam.typeIndex == kAss_MinTime)
            {
                mProcessor.BPMUpdated();
            }
        
        }//if no more managers left
    }//if was not a dummy row
    else{
        if(mNumOfRows > 0){
            mModManager.removeValue(_rowIndex);
            mAssigned.remove(_rowIndex);
        }
    }
    
    if(mNumOfRows < 0)
    {
        mNumOfRows = 0;
    }
    
    //rebuild all the rows from scratch
    mMinusButtons.clear();
    mAssignments.clear();
    mAssignmentsGl.clear();
    mLastAssignedType.clear();
    mDepths.clear();
    mDepthLables.clear();
    
    
    mThreeBoxes = mInitialBoxes;
    
    for(int i = 0 ; i < mNumOfRows; i++){
        addRow(i);
    }
    
    if(mNumOfRows == 0)
    {
        mModManager.attatchDummyValue();
        addRow(0);
        mNumOfRows++;
    }
    
}
void AssignPanel::paint (Graphics& g)
{

   // g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

}

void AssignPanel::resized()
{

}
void AssignPanel::buttonClicked (Button* b)
{
    for(int i = 0 ; i < mMinusButtons.size(); i++){
        
        if(b == mMinusButtons[i]){
                removeRow(i);
        }
    }
    //sendChangeMessage();
}


void AssignPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    
    for(int i = 0 ; i < mMinusButtons.size(); i++){
        if(comboBoxThatHasChanged == mAssignments[i]){
            
            DTModulation::DTAssignableParam& assignableParam = *mProcessor.getAssignableParameters()[mAssignments[i]->getSelectedItemIndex()];
            
            if(assignableParam.typeIndex == kAss_MinTime || assignableParam.typeIndex == kAss_MaxTime){
                assignableParam.isAttached = true;
                mProcessor.BPMUpdated();
            }
            
            //if the combo box had already been assigned we need to remove it from the last Assignable parameter as well as attatch it to the new one
            if(mAssigned[i])
            {
                
                for( auto selections : mAssignments)
                {
                    selections->setItemEnabled(mLastAssignedType[i] + 1, true);
                }
                mIsSelected.set(mLastAssignedType[i], false);
                
                DTModulation::DTAssignableParam& lastAssignedParam = *mProcessor.getAssignableParameters()[mLastAssignedType[i]];
               
                lastAssignedParam.modulationManagers.remove(mModManager.getManagerID());
                
                if(lastAssignedParam.modulationManagers.size() == 0)
                {
                    lastAssignedParam.isAttached = false;
                    lastAssignedParam.modulatedValue = lastAssignedParam.sharedOrigninalValue.get();
                    lastAssignedParam.setValue(lastAssignedParam.sharedOrigninalValue.get());
                    mProcessor.parameterValueChanged(lastAssignedParam.getParamIndex(), lastAssignedParam.sharedOrigninalValue.get());
                }
                
                assignableParam.modulationManagers.set(mModManager.getManagerID(), &mModManager);
                
                assignableParam.isAttached = true;
                
                mLastAssignedType.set(i, mAssignments[i]->getSelectedItemIndex());
                
                mModManager.updateValueAttatchment(mAssignments[i]->getSelectedItemIndex(), i);
                
                mModManager.setDepthValue(mAssignments[i]->getSelectedItemIndex(), mDepths[i]->getValue());
                mDepths[i]->getValueObject().referTo(*mModManager.getDepthValueObject(mAssignments[i]->getSelectedItemIndex()));
                
                for( auto selections : mAssignments)
                {
                    if(selections != mAssignments[i])
                        selections->setItemEnabled(assignableParam.typeIndex + 1, false);
                }
                mIsSelected.set(assignableParam.typeIndex, true);
                repaint();
            }
            else{

                assignableParam.modulationManagers.set(mModManager.getManagerID(), &mModManager);
                assignableParam.isAttached = true;
                mAssigned.set(i, true);
                
                mLastAssignedType.set(i, mAssignments[i]->getSelectedItemIndex());
     
                mModManager.updateValueAttatchment(mAssignments[i]->getSelectedItemIndex(), i);
                mModManager.setDepthValue(mAssignments[i]->getSelectedItemIndex(), mDepths[i]->getValue());
                
                mDepths[i]->getValueObject().referTo(*mModManager.getDepthValueObject(mAssignments[i]->getSelectedItemIndex()));
                
                
                for( auto selections : mAssignments)
                {
                    if(selections != mAssignments[i])
                        selections->setItemEnabled(assignableParam.typeIndex + 1, false);
                }
                
                mIsSelected.set(assignableParam.typeIndex, true);
                repaint();
            }//end if/else assigned
        }//end if combobox
    }//end minus for loop
}

