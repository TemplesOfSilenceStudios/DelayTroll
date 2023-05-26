/*
 ==============================================================================
 
 EQTrollDisplay.cpp
 Created: 19 Apr 2019 5:29:23pm
 Author:  Sam Ferguson
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "TrollEQDisplay.h"
#include "DTHelperFunctions.h"
#include "DTModulationDefines.h"
#include "DTInterfaceDefines.h"
#include "DTUsedParameters.h"

//==============================================================================
TrollEQDisplay::TrollEQDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel)
:   TrollDisplayPanelBase(p, _ControlPanel, kNMod_EQManuelMode, DTManualControl::kManMode_TopAndBottom, TrollParams::kTroll_LowPassFreq, TrollParams::kTroll_HighPassFreq),
displayValues(p.getDisplayValues()),
pathStroke(3.f, PathStrokeType::curved, PathStrokeType::butt),
mLowPassCurve(p, kMod_EQLowPassBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight()),
mHighPassCurve(p, kMod_EQHighPassBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight()),
mBandPassCurve(p, kMod_EQBPBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight()),
mBypass(p.tree, DTParameterID[kBParam_EQBypass], DTParameterName[kBParam_EQBypass],   mBypassImageOff, mBypassImageOn),
mBypassImage(ImageCache::getFromMemory(BinaryData::Bypass_Large_png, BinaryData::Bypass_Large_pngSize))
{
    mProcessor.addListener(this);
    
    mMode.addItemList(DTChoiceParameterStrings[2], 1);
    mMode.setSelectedId(p.getNonModParamAsInt(kNMod_EQManuelMode), dontSendNotification);

    
    setInterceptsMouseClicks(true, false);
    mDisplayComponent.setInterceptsMouseClicks(false, false);
    mDisplayComponent.setAlwaysOnTop(false);
    setAlwaysOnTop(true);
    
    mMode.onChange = [this](){
        if(mMode.getSelectedId() == mLastModeSelection || !isVisible())
        {
            return;
        }
        else{
            mLastModeSelection = mMode.getSelectedId();
        }
        
        switch (mMode.getSelectedId()) {
            case kDCOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mLowPassCurve.setVisible(true);
                mHighPassCurve.setVisible(true);
                setInterceptsMouseClicks(true, false);
                setAlwaysOnTop(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kDCOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mLowPassCurve.setVisible(false);
                mHighPassCurve.setVisible(false);
                setInterceptsMouseClicks(false, true);
                setAlwaysOnTop(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            default:
                break;
        }
        
        timerCallback();
    };
    
    //this component will handle all mouse events and pass them to the correct bezier curve display component acordingly
    mLowPassCurve.setBounds(mInnerDisplayArea);
    mLowPassCurve.addChangeListener(this);
    addChildComponent(&mLowPassCurve);
    mLowPassCurve.setInterceptsMouseClicks(false, false);
    
    mHighPassCurve.setBounds(mInnerDisplayArea);
    mHighPassCurve.addChangeListener(this);
    addChildComponent(&mHighPassCurve);
    mHighPassCurve.setInterceptsMouseClicks(false, false);
    
    mBandPassCurve.setBounds(mInnerDisplayArea);
    mBandPassCurve.addChangeListener(this);
    addChildComponent(&mBandPassCurve);
    mBandPassCurve.setInterceptsMouseClicks(false, false);
    
    mReset.onClick = [this](){
        mLowPassCurve.getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_EQLowPass]);
        mLowPassCurve.resetNodes();
        mHighPassCurve.getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_EQHighPass]);
        mHighPassCurve.resetNodes();
        timerCallback();
    };
    addChildComponent(&mReset);
    addChildComponent(&mResetLable);
    
    //set visability
    switch (mMode.getSelectedId()) {
        case kDCOp_Bez:
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mLowPassCurve.setVisible(true);
            mHighPassCurve.setVisible(true);
            setInterceptsMouseClicks(true, false);
            setAlwaysOnTop(true);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
        case kDCOp_Manual:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mLowPassCurve.setVisible(false);
            mHighPassCurve.setVisible(false);
            setInterceptsMouseClicks(false, true);
            setAlwaysOnTop(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        default:
            break;
    }
    
    //affinetransform to handle scaling between the mouses position in this panel and where it should be in the child panel
    mTranslate = AffineTransform::translation(Point<float>(0,0) - mInnerDisplayArea.getTopLeft().toFloat());
    
    //mBypass.setTopLeftPosition(mMode.getRight(), 0);
    mBypass.setClickingTogglesState(true);
    //mBypass.setCentrePosition(getLocalBounds().getCentreX(), (mMode.getHeight()/2) + 5);
    mBypass.setTopRightPosition(mQuantizationAmount.getX()-3, 0);
    mBypass.setToggleState(p.getParameters()[kBParam_EQBypass]->getValue(), NotificationType::dontSendNotification);

    //DBG("EQ Bypass: "<< p.getParameters()[kBParam_EQBypass]->getValue());
    

    
    addAndMakeVisible(&mBypass);
    
    mBypassDisplay.setImage(mBypassImage);
    
    mBypassDisplay.setBounds(mDisplayComponent.getBounds());
    mBypassDisplay.setBoundingBox(mBypassDisplay.getBounds().toFloat());
    
    mBypassDisplay.setVisible(mBypass.getToggleState());
    
    addChildComponent(&mBypassDisplay);
    
    
    
}

TrollEQDisplay::~TrollEQDisplay()
{
    mProcessor.removeListener(this);
}
void TrollEQDisplay::mouseMove(const juce::MouseEvent &e)
{
    if(mMode.getSelectedId() == kDCOp_Manual){
        return;
    }
    //*******************FIND WAY TO GIVE USER SOME BUFFERARROUND THE INNER BOX**************************
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslate);
        MouseEvent eTranslated = e.withNewPosition(translated);
        
        Path hpPath = mHighPassCurve.getBezPath();
        std::vector<Point<int>> hpNodes = mHighPassCurve.getNodes();
        Path lpPath = mLowPassCurve.getBezPath();
        std::vector<Point<int>> lpNodes = mLowPassCurve.getNodes();
        
        Point<float> pointOnPath;
        
        hpPath.getNearestPoint(eTranslated.getPosition().toFloat(), pointOnPath);
        float hpDistance = eTranslated.getPosition().toFloat().getDistanceFrom(pointOnPath);
        
        for(std::vector<Point<int>>::iterator it = hpNodes.begin(); it != hpNodes.end(); it++  )
        {
            float tempdistance = eTranslated.getPosition().toFloat().getDistanceFrom(it->toFloat());
            hpDistance = hpDistance <= tempdistance ? hpDistance : tempdistance;
        }
        
        lpPath.getNearestPoint(eTranslated.getPosition().toFloat(), pointOnPath);
        float lpDistance = eTranslated.getPosition().toFloat().getDistanceFrom(pointOnPath);
        
        for(std::vector<Point<int>>::iterator it = lpNodes.begin(); it != lpNodes.end(); it++  )
        {
            float tempdistance = eTranslated.getPosition().toFloat().getDistanceFrom(it->toFloat());
            lpDistance = lpDistance <= tempdistance ? lpDistance : tempdistance;
        }
        
        if (hpDistance >= lpDistance) {
            mLowPassCurve.setPathColour(DTColourIds::LrgDisplayGlow);
            mHighPassCurve.setPathColour(DTColourIds::LrgDisplayDimGlow);
            mPlotEditingNumber = kBezIndex_LowPassPlot;
        }
        else{
            mLowPassCurve.setPathColour(DTColourIds::LrgDisplayDimGlow);
            mHighPassCurve.setPathColour(DTColourIds::LrgDisplayGlow);
            mPlotEditingNumber = kBezIndex_HighPassPlot;
        }
    }
    
}
void TrollEQDisplay::mouseDown(const MouseEvent& e)
{
    //inserts a new node
    startTimer(50);
    
    if(mMode.getSelectedId() == kDCOp_Manual)
        return;
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslate);
        MouseEvent eTranslated = e.withNewPosition(translated);
        
        switch (mPlotEditingNumber) {
            case kBezIndex_BPPlot:
                mBandPassCurve.mouseDown(eTranslated);
                break;
                
            case kBezIndex_LowPassPlot:
                mLowPassCurve.mouseDown(eTranslated);
                break;
                
            case kBezIndex_HighPassPlot:
                mHighPassCurve.mouseDown(eTranslated);
                break;
                
            default:
                break;
        }
    }//if mouse is in display area
}

void TrollEQDisplay::mouseDrag(const MouseEvent& e)
{
    if(mMode.getSelectedId() == kDCOp_Manual)
        return;
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        std::vector<Point<int>> hpNodes = mHighPassCurve.getNodes();
        std::vector<Point<int>> lpNodes = mLowPassCurve.getNodes();
        
        Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslate);
        MouseEvent eTranslated = e.withNewPosition(translated);
        
        switch (mPlotEditingNumber) {
            case kBezIndex_BPPlot:
                mBandPassCurve.mouseDrag(eTranslated);
                break;
                
            case kBezIndex_LowPassPlot:
                if(mLowPassCurve.getDraggingNodeNumber() == 0 && eTranslated.getPosition().y >= hpNodes[0].y)
                    return;
                else if(mLowPassCurve.getDraggingNodeNumber() == lpNodes.size()-1 && eTranslated.getPosition().y >= hpNodes[hpNodes.size()-1].y)
                    return;
                
                mLowPassCurve.mouseDrag(eTranslated);
                break;
                
            case kBezIndex_HighPassPlot:
                if(mHighPassCurve.getDraggingNodeNumber() == 0 && eTranslated.getPosition().y <= lpNodes[0].y)
                    return;
                else if(mHighPassCurve.getDraggingNodeNumber() == hpNodes.size()-1 && eTranslated.getPosition().y <= lpNodes[lpNodes.size()-1].y)
                    return;
                
                mHighPassCurve.mouseDrag(eTranslated);
                break;
                
            default:
                break;
        }
    }
}

void TrollEQDisplay::mouseDoubleClick(const MouseEvent& e)
{
    if(mMode.getSelectedId() == kDCOp_Manual)
        return;
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslate);
        MouseEvent eTranslated = e.withNewPosition(translated);
        
        switch (mPlotEditingNumber) {
            case kBezIndex_BPPlot:
                mBandPassCurve.mouseDoubleClick(eTranslated);
                break;
                
            case kBezIndex_LowPassPlot:
                mLowPassCurve.mouseDoubleClick(eTranslated);
                break;
                
            case kBezIndex_HighPassPlot:
                mHighPassCurve.mouseDoubleClick(eTranslated);
                break;
                
            default:
                break;
        }
    }
    timerCallback();
}
void TrollEQDisplay::mouseUp(const MouseEvent& e)
{
    
    if(mMode.getSelectedId() == kDCOp_Manual){
        stopTimer();
        timerCallback();
        repaint();
        return;
    }
        
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslate);
        MouseEvent eTranslated = e.withNewPosition(translated);
        
        switch (mPlotEditingNumber) {
            case kBezIndex_BPPlot:
                mBandPassCurve.mouseUp(eTranslated);
                break;
                
            case kBezIndex_LowPassPlot:
                mLowPassCurve.mouseUp(eTranslated);
                break;
                
            case kBezIndex_HighPassPlot:
                mHighPassCurve.mouseUp(eTranslated);
                break;
                
            default:
                break;
        }
        
        
    }
    
    stopTimer();
    timerCallback();
    repaint();
}

void TrollEQDisplay::paint (Graphics& g)
{
    g.setOpacity(0.5f);
    float rightX = mInnerDisplayArea.toFloat().getRight();
    g.setFont(9.f);
    for (auto freq : DTFrequencyMarkers) {
       float y =  DTValueToPixels(static_cast<float>(freq),
                                TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_HighPassFreq],
                                mInnerDisplayArea.toFloat().getHeight()) + TROLL_DISPLAY_COMPONENT_BUMPER;
        
        g.drawLine(mInnerDisplayArea.toFloat().getX(), y, rightX, y);
        
        mTextRect.setCentre(mTextRect.getCentreX(), static_cast<int>(y));
        
        //g.drawText(String(freq) + String("Hz"), rightX, y-10.f, 40.f, 20.f, Justification::centredLeft);
        g.drawFittedText(String(freq) + String("Hz"), mTextRect, Justification::centredLeft, 1);
        
    }
    
    TrollDisplayPanelBase::paint(g);
    mBypassDisplay.setVisible(mBypass.getToggleState());
    
}

void TrollEQDisplay::loadActivated(DelayTrollAudioProcessor* _p)
{
    //DBG("Loading activated at EQ with a switch value of: "<<_p->getNonModParamAsInt(kNMod_EQManuelMode));
    
    if (&mProcessor == _p){
        
        if(isVisible()){
            mControlPanel.setBPMModeAtomic(mProcessor.getSyncStatus(), false);
            mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
            mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
            mQuantizationAmount.setSelectedId(mLastQuantSelection, dontSendNotification);
            mLastModeSelection = mProcessor.getNonModParamAsInt(kNMod_EQManuelMode);
            mMode.setSelectedId(mLastModeSelection, dontSendNotification);
            
            if(mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_Gravity ||
               mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_BPMSync) {
                mControlPanel.setGravetyEnabled(true);
            }
            else{
                mControlPanel.setGravetyEnabled(false);
            }
            
        }
        
        switch (mLastModeSelection) {
            case kDCOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mLowPassCurve.setVisible(true);
                mHighPassCurve.setVisible(true);
                setInterceptsMouseClicks(true, false);
                setAlwaysOnTop(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kDCOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mLowPassCurve.setVisible(false);
                mHighPassCurve.setVisible(false);
                setInterceptsMouseClicks(false, true);
                setAlwaysOnTop(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            default:
                break;
        }
    }//if processor adress matches _p
}

void TrollEQDisplay::setComponentVisability(int _controlMode)
{
    mLastModeSelection = _controlMode;
    mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
    mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
    mMode.setSelectedId(mLastModeSelection, dontSendNotification);
    
    switch (mLastModeSelection) {
        case kDCOp_Bez:
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mLowPassCurve.setVisible(true);
            mHighPassCurve.setVisible(true);
            setInterceptsMouseClicks(true, false);
            setAlwaysOnTop(true);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
        case kDCOp_Manual:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mLowPassCurve.setVisible(false);
            mHighPassCurve.setVisible(false);
            setInterceptsMouseClicks(false, true);
            setAlwaysOnTop(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        default:
            break;
    }
}
void TrollEQDisplay::timerCallback()
{
    if(isVisible()){
        mProcessor.getAtomicTimeProcessor().processEQ();
        
        mDisplayComponent.setLines();
        repaint();
    }
}
