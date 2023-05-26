/*
  ==============================================================================

    MainModMenuPanel.cpp
    Created: 22 Apr 2019 6:14:12pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "MainModMenuPanel.h"
#include "DTHelperFunctions.h"

//==============================================================================
MainModMenuPanel::MainModMenuPanel(DelayTrollAudioProcessor& p):
PanelBase(p),
mLeftImageOn(ImageCache::getFromMemory(BinaryData::Left_ArrowDown_png, BinaryData::Left_ArrowDown_pngSize)),
mLeftImageOff(ImageCache::getFromMemory(BinaryData::Left_ArrowUp_png, BinaryData::Left_ArrowUp_pngSize)),
mRightImageOn(ImageCache::getFromMemory(BinaryData::Right_ArrowDown_png, BinaryData::Right_ArrowDown_pngSize)),
mRightImageOff(ImageCache::getFromMemory(BinaryData::Right_ArrowUp_png, BinaryData::Right_ArrowUp_pngSize)),
mEjectImageOn(ImageCache::getFromMemory(BinaryData::EjectDown_png, BinaryData::EjectDown_pngSize)),
mEjectImageOff(ImageCache::getFromMemory(BinaryData::EjectUp_png, BinaryData::EjectUp_pngSize)),
mExpandButton(DTButtonNames[kButton_Expand], mEjectImageOn, mEjectImageOff),
mLeftArrow(DTButtonNames[kButton_Left], mLeftImageOn, mLeftImageOff),
mRightArrow(DTButtonNames[kButton_Right], mRightImageOn, mRightImageOff),
mButtonBarImage(ImageCache::getFromMemory(BinaryData::TabBarB_png, BinaryData::TabBarB_pngSize)),
mButtonBarGlImage(ImageCache::getFromMemory(BinaryData::TabBarBGl_png, BinaryData::TabBarBGl_pngSize)),
mButtonBar(DTButtonNames[kButton_ModulationTabs], mButtonBarImage),
mDisplayVal(mProcessor.getDisplayValues()),
mDisValID(DTDisplayValueIDs[kDisVal_bezierEditingIndex]),
mNewImageOn(ImageCache::getFromMemory(BinaryData::NewDown_png, BinaryData::NewDown_pngSize)),
mNewImageOff(ImageCache::getFromMemory(BinaryData::NewUp_png, BinaryData::NewUp_pngSize)),
mSaveImageOn(ImageCache::getFromMemory(BinaryData::SaveDown_png, BinaryData::SaveDown_pngSize)),
mSaveImageOff(ImageCache::getFromMemory(BinaryData::SaveUp_png, BinaryData::SaveUp_pngSize)),
mDeleteImageOn(ImageCache::getFromMemory(BinaryData::DeleteDown_png, BinaryData::DeleteDown_pngSize)),
mDeleteImageOff(ImageCache::getFromMemory(BinaryData::DeleteUp_png, BinaryData::DeleteUp_pngSize)),
mPresetImage(ImageCache::getFromMemory(BinaryData::DropDownB_png, BinaryData::DropDownB_pngSize)),
mPresetGlImage(ImageCache::getFromMemory(BinaryData::DropDownBGl_png, BinaryData::DropDownBGl_pngSize)),
mSmallDisplayImage(ImageCache::getFromMemory(BinaryData::SmallDisplay3_png, BinaryData::SmallDisplay3_pngSize)),
mSmallDisplayGlImage(ImageCache::getFromMemory(BinaryData::SmallDisplay3Gl_png, BinaryData::SmallDisplay3Gl_pngSize))
{
    setSize(MAIN_MOD_MENU_PANEL_WIDTH, MAIN_MOD_MENU_PANEL_HEIGHT);
    
    //mLookAndFeel = std::make_unique<DTLookAndFeel>();
    //mButtonBar.setLookAndFeel(LookAndFeel);
    Array<Rectangle<int>> thirds= DTsetSubBox(getLocalBounds(), 3, 1);
    Array<Rectangle<int>> TabControlBoxes = DTsetSubBox(thirds[0], 3, 1);
    Array<Rectangle<int>> TabSwitchBoxes = DTsetSubBox(thirds[2], 3, 1);
    //mRightArrow.setSize(mRightArrow.getWidth()/2, mRightArrow.getHeight()/2);
    mRightArrow.setBounds(TabSwitchBoxes[2]);

    
    addAndMakeVisible(&mRightArrow);
    
    mSmallDisplay.setSize(mSmallDisplayImage.getWidth(), mSmallDisplayImage.getHeight());
    mSmallDisplay.setImage(mSmallDisplayImage);
    mSmallDisplay.setCentrePosition(TabSwitchBoxes[1].getCentre());
    mSmallDisplay.setBoundingBox(mSmallDisplay.getBounds().toFloat());
    addAndMakeVisible(&mSmallDisplay);
    
    mSmallDisplayGl.setSize(mSmallDisplayGlImage.getWidth(), mSmallDisplayGlImage.getHeight());
    mSmallDisplayGl.setImage(mSmallDisplayGlImage);
    mSmallDisplayGl.setCentrePosition(TabSwitchBoxes[1].getCentre());
    mSmallDisplayGl.setBoundingBox(mSmallDisplayGl.getBounds().toFloat());
    mSmallDisplayGl.setAlwaysOnTop(true);
    addAndMakeVisible(&mSmallDisplayGl);
    
    mPlotNumGlow.setFont(Font(50));
    mPlotNumGlow.setJustificationType(Justification::centred);
    mPlotNumGlow.setColour(Label::ColourIds::textColourId, DTColourIds::TextGlow);
    mPlotNumGlow.setAlpha(0.5f);
    
    Parallelogram<float> glowrec(TabSwitchBoxes[1].toFloat().reduced(30.f, 22.f));
    mGlow.setRectangle(glowrec);
    mGlow.setCornerSize(Point<float>(25.f,25.f));
    mGlow.setAlpha(0.3f);
    mGlow.setFill(ColourGradient(   DTColourIds::TextGlow,
                                    TabSwitchBoxes[1].getCentre().toFloat(),
                                    DTColourIds::TextGlowTrans,
                                    TabSwitchBoxes[1].getCentre().toFloat().translated(0.f, 20.f),
                                    true));
    addAndMakeVisible(&mGlow);
    
    mPlotNum.setFont(Font(45));
    mPlotNum.setJustificationType(Justification::centred);
    mPlotNum.setColour(Label::ColourIds::textColourId, DTColourIds::SmlDisplayGlow);
    //mTextGlow.setGlowProperties(20.f, Colours::red);
   // mPlotNum.setComponentEffect(&mTextGlow);
    int tempNumber = mDisplayVal[mDisValID];
    mPlotNum.setText(String(++tempNumber), dontSendNotification);
    mPlotNumGlow.setText(String(tempNumber), dontSendNotification);
    //mPlotNum.setBounds(TabSwitchBoxes[1]);
    mPlotNum.setBounds(mSmallDisplay.getBoundsInParent());
    mPlotNumGlow.setBounds(mSmallDisplay.getBoundsInParent());
    //addAndMakeVisible(&mPlotNumGlow);
    addAndMakeVisible(&mPlotNum);
    
    mLeftArrow.setBounds(TabSwitchBoxes[0]);
    addAndMakeVisible(&mLeftArrow);

    Array<Rectangle<int>> sixthsRecs = DTsetSubBox(thirds[1], 3, 2);
    
    
    mNewPreset = std::make_unique<DTButton>("New", mNewImageOn, mNewImageOff);
    mNewPreset->setCentrePosition(sixthsRecs[3].getCentre().translated(29, -8));
    mNewPreset->addListener(this);
    addAndMakeVisible(*mNewPreset);
    
    mSaveAsPreset = std::make_unique<DTButton>("Save As", mSaveImageOn, mSaveImageOff);
    mSaveAsPreset->setCentrePosition(sixthsRecs[4].getCentre().translated(0, -8));
    mSaveAsPreset->addListener(this);
    addAndMakeVisible(*mSaveAsPreset);
    
    mDeletePreset = std::make_unique<DTButton>("Delete", mDeleteImageOn, mDeleteImageOff);
    mDeletePreset->setCentrePosition(sixthsRecs[5].getCentre().translated(-29, -8));
    mDeletePreset->addListener(this);
    addAndMakeVisible(*mDeletePreset);
    
    mPresetDisplay = std::make_unique<DTParamComboBox>("presets", mPresetImage);
    mPresetDisplay->setCentrePosition(DTextendRecto(sixthsRecs[0], sixthsRecs[2]).getCentre().translated(0, -4));
    mPresetDisplay->addListener(this);
    addAndMakeVisible(*mPresetDisplay);
    
    mPresetGl.setImage(mPresetGlImage);
    mPresetGl.setSize(mPresetGlImage.getWidth(), mPresetGlImage.getHeight());
    mPresetGl.setCentrePosition(mPresetDisplay->getBounds().getCentre());
    mPresetGl.setBoundingBox(mPresetGl.getBounds().toFloat());
    addAndMakeVisible(&mPresetGl);
    
    updatePresetComboBox();
    
    
    //mExpandButton.setBounds(TabControlBoxes[0].reduced(20));
    mExpandButton.setCentrePosition(TabControlBoxes[0].getCentre());
    //mExpandButton.setButtonText("<-");
    addAndMakeVisible(&mExpandButton);
    
    //mButtonBar.setBounds(DTextendRecto(TabControlBoxes[1], TabControlBoxes[2]).reduced(0, 20));
    mButtonBar.setCentrePosition(DTextendRecto(TabControlBoxes[1], TabControlBoxes[2]).getCentre());
    mButtonBar.addTab("     Bezier     ", Colours::transparentWhite, 0);
    mButtonBar.getTabButton(kModPanel_Bezier)->addListener(this);
    mButtonBar.addTab("       LFO       ", Colours::transparentWhite, 1);
    mButtonBar.getTabButton(kModPanel_LFO)->addListener(this);
    
    //mButtonBar.setCurrentTabIndex(mProcessor.getDisplayValues()[DTDisplayValueIDs[kDisVal_modulationCategoryMenu]]);
    mButtonBar.setCurrentTabIndex(0);
    addAndMakeVisible(&mButtonBar);
    
    mButtonBarGl.setImage(mButtonBarGlImage);
    mButtonBarGl.setSize(mButtonBarGlImage.getWidth(), mButtonBarGlImage.getHeight());
    mButtonBarGl.setCentrePosition(DTextendRecto(TabControlBoxes[1], TabControlBoxes[2]).getCentre());
    mButtonBarGl.setBoundingBox(mButtonBarGl.getBounds().toFloat());
    addAndMakeVisible(&mButtonBarGl);
    
    //mPresetManager = std::make_unique<DTPresetManager>(mProcessor.getPresetManager());
    
}

MainModMenuPanel::~MainModMenuPanel()
{
    mButtonBar.getTabButton(0)->removeListener(this);
    mButtonBar.getTabButton(1)->removeListener(this);
}

void MainModMenuPanel::paint (Graphics& g)
{
//g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}

void MainModMenuPanel::resized()
{
   
}

void MainModMenuPanel::addButtonListener(Button::Listener *inListener, int _buttonIndex)
{
    
    switch (_buttonIndex) {
        case kButton_Expand:
            mExpandButton.addListener(inListener);
            break;
        case kButton_Left:
            mLeftArrow.addListener(inListener);
            break;
        case kButton_Right:
             mRightArrow.addListener(inListener);
            break;
        case kButton_ModulationTabs:
            for(int i = 0 ; i < mButtonBar.getNumTabs(); i++){
                mButtonBar.getTabButton(i)->addListener(inListener);
            }
            break;
        default:
            break;
    }
   
    
}

void MainModMenuPanel::removeButtonListener(Button::Listener *inListener, int _buttonIndex)
{
    switch (_buttonIndex) {
        case kButton_Expand:
            mExpandButton.removeListener(inListener);
            break;
        case kButton_Left:
            mLeftArrow.removeListener(inListener);
            break;
        case kButton_Right:
            mRightArrow.removeListener(inListener);
            break;
        case kButton_ModulationTabs:
            for(int i = 0 ; i < mButtonBar.getNumTabs(); i++){
                mButtonBar.getTabButton(i)->removeListener(inListener);
            }
            break;
        default:
            break;
    }
}
void MainModMenuPanel::buttonClicked(juce::Button *b)
{
    /*look its probably over unnessisary and I could probably just check if b == mExpandButton.get()
     but I dont remember why i did this so strangly in the first place so this seems like the safer option */
    if(b != nullptr && b != mNewPreset.get() && b != mSaveAsPreset.get() && b != mDeletePreset.get())
    {
        if(!mExpandButton.getToggleState())
            mExpandButton.triggerClick();
    }
    
    DTPresetManager* presetManager = mProcessor.getPresetManager();
    
    if(b == mNewPreset.get()){
        mIsInitalizingSettings = true;
        mInitAlert->showOkCancelBox(MessageBoxIconType::QuestionIcon, "Initializing Settings", "Are you sure you want to Initialize the settings? All Parameters will be set to basic settings. This action cannot be undone.", "Initialize", "Cancel", {}, ModalCallbackFunction::create (AlertBoxResultChosen{this}));
    }
    else if (b == mSaveAsPreset.get()){
        displaySaveAsPopup();
    }
    else if (b == mDeletePreset.get()){
        mIsDeletingPreset = true;
        if(!presetManager->getIsCurrentPresetFactory() && mPresetDisplay->getSelectedItemIndex() != -1){
        mDeleteAlert->showOkCancelBox(MessageBoxIconType::QuestionIcon, "Deleting File", "Are you sure you want to delete this preset? This action cannot be undone.", "Delete", "Cancel", {}, ModalCallbackFunction::create (AlertBoxResultChosen{this}));
        }
    }
    
}
void MainModMenuPanel::updatePlotNumber(int _newPlotNum)
{
    mPlotNum.setText(String(_newPlotNum), dontSendNotification);
    mPlotNumGlow.setText(String(_newPlotNum), dontSendNotification);
}

void MainModMenuPanel::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if(comboBoxThatHasChanged == mPresetDisplay.get()){
        DTPresetManager* presetManager = mProcessor.getPresetManager();
        
        //const int index = mPresetDisplay->getSelectedItemIndex();
        const int index = mPresetDisplay->getSelectedId() - 1;
        
        presetManager->loadPreset(index);
    }
}

void MainModMenuPanel::updatePresetComboBox()
{
    DTPresetManager* presetManager = mProcessor.getPresetManager();
    //pointer to the root menue to add submenus to
    PopupMenu* root = mPresetDisplay->getRootMenu();
    String presetName = presetManager->getCurrentPresetName();
    const int numFactoryPresets = presetManager->getNumberOfPresets(false);
    
    //clear everything
    mPresetDisplay->clear(dontSendNotification);
    root->clear();
    mZipperSubMenu.clear();
    mSweepSubMenu.clear();
    mBubblerSubMenu.clear();
    mOtherSubMenu.clear();
    mUserPresetsSubMenu.clear();

    //add the factory presets, the file names start with a letter that denotes their category
    for(int i = 0 ; i < numFactoryPresets; i++){
        
        const char category = presetManager->getPresetName(i)[0];
        
        //DBG("Updating Preset ComboBox index: "<< i+1);
        //DBG("With Name: "<<presetManager->getPresetName(i).substring(2));
        
        switch (category) {
            case 'Z':
                mZipperSubMenu.addItem(i+1, presetManager->getPresetName(i).substring(2));
                break;
            case 'S':
                mSweepSubMenu.addItem(i+1, presetManager->getPresetName(i).substring(2));
                break;
            case 'B':
                mBubblerSubMenu.addItem(i+1, presetManager->getPresetName(i).substring(2));
                break;
            case 'O':
                mOtherSubMenu.addItem(i+1, presetManager->getPresetName(i).substring(2));
                break;
            default:
                jassertfalse;
                break;
        }
    }//done organizing factory presets into categories
    
    root->addSubMenu("Zippers", mZipperSubMenu);
    root->addSubMenu("Sweeps", mSweepSubMenu);
    root->addSubMenu("Bubblers", mBubblerSubMenu);
    root->addSubMenu("Other", mOtherSubMenu);
    
    mPresetDisplay->addSeparator();
    
    const int numPresets = presetManager->getNumberOfPresets(true);
    
    for(int i = numFactoryPresets; i< numPresets; i++){
        
        mUserPresetsSubMenu.addItem(i+1, presetManager->getPresetName(i));
    }
    root->addSubMenu("User Presets", mUserPresetsSubMenu);
    
    
    if(presetManager->getIsCurrentPresetFactory())
        mPresetDisplay->setText(presetManager->getCurrentPresetName().substring(2), dontSendNotification);
    else
        mPresetDisplay->setText(presetManager->getCurrentPresetName(), dontSendNotification);
}

void MainModMenuPanel::displaySaveAsPopup()
{
    
    String currentPresetName = mProcessor.getPresetManager()->getCurrentPresetName();

    mFileChooser = std::make_unique<FileChooser>(
                                                "Please enter the name of the preset to save",
                                                 mProcessor.getPresetManager()->mUserPresetDirectory,
                                                 "*" + static_cast<String>(PRESET_FILE_EXTENTION)
    );
    mFileChooser->launchAsync(FileBrowserComponent::saveMode, [&](const FileChooser& _chooser){
        const auto resultFile = _chooser.getResult();
//        DBG(resultFile.getFileNameWithoutExtension());
//        DBG(mProcessor.getPresetManager()->mPresetDirectory);
        if(resultFile.getFileNameWithoutExtension() != ""){
            mProcessor.getPresetManager()->saveAsPreset(resultFile.getFileNameWithoutExtension());
            updatePresetComboBox();
        }
    });
    
   
    
}
