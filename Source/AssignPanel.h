/*
  ==============================================================================

    AssignPanel.h
    Created: 23 Apr 2019 11:36:20am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DTButton.h"
#include "DTParamComboBox.h"
#include "DTParamSlider.h"


//==============================================================================
/**Handles the assigning modulation curves to parameters
*/
class AssignPanel    : public PanelBase, public Button::Listener, public ComboBox::Listener
{
public:
    AssignPanel(DelayTrollAudioProcessor& p, ModManagment& _modManager, int _width, int _height);
    ~AssignPanel();

    void paint (Graphics&) override;
    void resized() override;

    /**adds (appends) a default dummy row to the list of modulatable parameters assigned to this modulation*/
    void addRow(int _newRowIndex);
    /**removes a row at _rowIndex from the list of modulatable parameters assigned to this modulation*/
    void removeRow(int _rowIndex);
    
    /**overrides the button method to determine which row is being removed when a minus button is clicked */
    void buttonClicked (Button* b) override;
    
    /**overrides the combo box method to handle updates to the parameter that is being modulated, either setting it or changing it*/
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    
    
    
private:
    LookAndFeel_V4 mDefaultLookAndFeel;
    /**Reference to a ModManagment that corrisponds to the data displayed and manipulated, each plot displayed has its own ModManager*/
    ModManagment& mModManager;
    
    const Image mAssignmentsImage;
    const Image mAssignmentsGlImage;
    OwnedArray<DrawableImage> mAssignmentsGl;

    
    const Image mPlusImageOn, mPlusImageOff, mMinusImageOn, mMinusImageOff;
    
    DTButton mPlusButton;
    OwnedArray<DTButton> mMinusButtons;
    OwnedArray<DTParamComboBox> mAssignments;
    
    const Image mGearImage;
    const Image mDepthLable;
    
    /**Array of depth sliders to control how much the selected parameter is effected by the plot, when attached to a parameter the underlying Value object that this slider is referring to is owned by the mModManager*/
    OwnedArray<DTParameterSlider> mDepths;
    OwnedArray<DrawableImage> mDepthLables;

    
    /**Keeps track of the parameter types currently selected by the combo boxes*/
    Array<int> mLastAssignedType;
    Array<bool> mIsSelected;

    int mNumOfRows;

    Array<bool> mAssigned;
    
    /**A group of rectangles representing the layout of an individual row of components, moving down each time a row is added*/
    Array<Rectangle<int>> mThreeBoxes;
    Array<Rectangle<int>> mInitialBoxes;
    
    BubbleMessageComponent mBubble;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AssignPanel)
};
