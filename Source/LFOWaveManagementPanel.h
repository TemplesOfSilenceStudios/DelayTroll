/*
  ==============================================================================

    LFOWaveManagementPanel.h
    Created: 26 Apr 2019 4:13:38pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PanelBase.h"
#include "DTButton.h"
#include "DTParamComboBox.h"
#include "DTParamSlider.h"
#include "DTCheckBox.h"
#include "DTParamValueLabel.h"
#include "BPMSyncLabel.h"
//==============================================================================
/*Controls the parameters of a LFOPlotter
*/
class LFOWaveManagementPanel    : public PanelBase, public Button::Listener, public Slider::Listener, public ComboBox::Listener, public ChangeBroadcaster
{
public:
    LFOWaveManagementPanel(DelayTrollAudioProcessor& p, int _assigningIndex, int _width, int _height, ChangeListener* inListener);
    ~LFOWaveManagementPanel();
    /**adds a row of sliders and dropdowns to the GUI and increases its height*/
    void addRow(int _newRowIndex);
    /**removes all rows, decrements the number of rows and rebuilds the rows to that number*/
    void removeRow(int _rowIndex);

    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked (Button* b) override;

    void sliderValueChanged(Slider* slider) override;
    
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    

private:
    
    LFOPlotter& mLFOPlot;
    
    BPMManager& mBPMManager;
    const Image mPlusOn, mPlusOff;
    const Image mMinusOn, mMinusOff;
    
    DTButton mPlusButton;
    
    const Image mLFOSelectionImage;
    const Image mLFOSelectionGlImage;
    const Image mGearImage;
    const Image mPeriodLable;
    const Image mPhaseLable;
    const Image mAmpLable;
    
    
    OwnedArray<DTButton> mMinusButtons;
    OwnedArray<DTParamComboBox> mLFOSelection;
    OwnedArray<DrawableImage> mLFOSelectionGl;
    OwnedArray<DTParameterSlider> mPeriodSliders;
    OwnedArray<DrawableImage> mPeriodLables;
    OwnedArray<DrawableImage> mPhaseLables;
    OwnedArray<DrawableImage> mAmpLables;
    OwnedArray<DTParameterSlider> mAmplitudeSliders;
    OwnedArray<DTParameterSlider> mPhaseSliders;
    
    //how many rows of perameters are being modulated by this plot
    int mNumOfRows;
    
    int mAssigningIndex;
    
    Array<Rectangle<int>> sixboxes;
    Array<Rectangle<int>> mInitialBoxes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOWaveManagementPanel)
};
