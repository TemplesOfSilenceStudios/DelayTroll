/*
  ==============================================================================

    MainModMenuPanel.h
    Created: 22 Apr 2019 6:14:12pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DTButton.h"
#include "DTTabbedButtonBar.h"
#include "DTParamComboBox.h"
#include "DTLookAndFeel.h"
//==============================================================================
/**Displays Menue options handleing presets and visability of the modulation section
*/
class MainModMenuPanel    : public PanelBase, public Button::Listener, public ComboBox::Listener
{
public:
    MainModMenuPanel(DelayTrollAudioProcessor& p);
    ~MainModMenuPanel();

    void paint (Graphics&) override;
    void resized() override;
    
    void addButtonListener(Button::Listener* inListener, int _buttonIndex);
    void removeButtonListener(Button::Listener* inListener, int _buttonIndex);
    
    /**expands the display to reveil the modulation section*/
    void buttonClicked (Button* b) override;
    
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    
    /**handles a change to the plot number that user wants to view*/
    void updatePlotNumber(int _newPlotNum);
    
    /**returns a pointer to the preset manager*/
    DTPresetManager* getPresetManagerPtr(){return mProcessor.getPresetManager();}
    
    /**gets the index of the currently loaded save*/
    int getSelectedItemIndex() {return mPresetDisplay->getSelectedItemIndex();}
    
    /**gets the ID of the currently loaded save*/
    int getSelectedItemID(){return mPresetDisplay->getSelectedId();}
    
    /** updates the combobox filling it with the variouse presets in the preset manager */
    void updatePresetComboBox();
    
    int getSelectedModulation(){return mButtonBar.getCurrentTabIndex();}
    
    bool isInitalizing(){return mIsInitalizingSettings;}
    bool isDeleting(){return mIsDeletingPreset;}
    
    void resetButtonBools(){ mIsInitalizingSettings = false; mIsDeletingPreset = false;}
    
private:
    
    struct AlertBoxResultChosen
    {
        void operator() (int result) const noexcept
        {
            if(result==1 && menuPanel != nullptr){
                
                if(menuPanel->isInitalizing()){
                    menuPanel->getPresetManagerPtr()->createNewPreset();
                }
                else if(menuPanel->getSelectedItemIndex()!= -1 && menuPanel->isDeleting()){
                    menuPanel->getPresetManagerPtr()->deletePreset(menuPanel->getSelectedItemID()-1);
                }
                
                menuPanel->updatePresetComboBox();
            }
            
            if(menuPanel !=nullptr)
                menuPanel->resetButtonBools();
            
            menuPanel->exitModalState(result);
        }
        WeakReference<MainModMenuPanel> menuPanel;
    };
    
    
    //std::unique_ptr<DTLookAndFeel> mLookAndFeel;
    /** displays a pop up menu for naming a new preset */
    void displaySaveAsPopup();
    
    const Image mLeftImageOn, mLeftImageOff, mRightImageOn, mRightImageOff;
    const Image mEjectImageOn, mEjectImageOff;
    
    DTButton mExpandButton;
    DTButton mLeftArrow;
    DTButton mRightArrow;
    Label mPlotNum;
    Label mPlotNumGlow;
    const Image mButtonBarImage;
    const Image mButtonBarGlImage;
    DTTabbedButtonBar mButtonBar;
    DrawableImage mButtonBarGl;

    
    NamedValueSet& mDisplayVal;
    Identifier mDisValID;
    int mPlotNumber;
    
    const Image mNewImageOn, mNewImageOff, mSaveImageOn, mSaveImageOff, mDeleteImageOn, mDeleteImageOff;
    
    std::unique_ptr<DTButton> mNewPreset, mSaveAsPreset, mDeletePreset;
    
    std::unique_ptr<DTParamComboBox> mPresetDisplay;
    
    PopupMenu mZipperSubMenu;
    PopupMenu mSweepSubMenu;
    PopupMenu mBubblerSubMenu;
    PopupMenu mOtherSubMenu;
    PopupMenu mUserPresetsSubMenu;
    
    const Image mPresetImage;
    const Image mPresetGlImage;
    const Image mSmallDisplayImage;
    const Image mSmallDisplayGlImage;
    DrawableImage mSmallDisplay;
    DrawableImage mSmallDisplayGl;
    DrawableImage mPresetGl;
    
    DrawableRectangle mGlow;
    
    //std::unique_ptr<DTButton> mNewPreset, mSaveAsPreset, mDeletePreset;
    std::unique_ptr<FileChooser> mFileChooser;
    std::unique_ptr<AlertWindow> mDeleteAlert;
    std::unique_ptr<AlertWindow> mInitAlert;
    
    bool mIsInitalizingSettings = false;
    bool mIsDeletingPreset = false;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainModMenuPanel)
    JUCE_DECLARE_WEAK_REFERENCEABLE (MainModMenuPanel)
};
