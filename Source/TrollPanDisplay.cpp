/*
  ==============================================================================

    TrollPanDisplay.cpp
    Created: 4 Aug 2020 2:54:18pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TrollPanDisplay.h"
#include "DTUsedParameters.h"
#include "DTModulationDefines.h"
#include "DTInterfaceDefines.h"
//==============================================================================
TrollPanDisplay::TrollPanDisplay(DelayTrollAudioProcessor &p, TrollTimeControlPanel& _ControlPanel):
TrollDisplayPanelBase(p, _ControlPanel, kNMod_PanManuelMode, DTManualControl::kManMode_Split, TrollParams::kTroll_LeftAtenuation, TrollParams::kTroll_RightAtenuation),
mLeftCurve(p, kMod_LeftPanBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight()/2.f),
mRightCurve(p, kMod_RightPanBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight()/2.f),
mBalCurve(p, kMod_BalPanBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight())
{
    mProcessor.addListener(this);
    
    setInterceptsMouseClicks(true, false);
    mDisplayComponent.setInterceptsMouseClicks(false, false);
    mDisplayComponent.setAlwaysOnTop(false);
    
    int modeSelection = p.getNonModParamAsInt(kNMod_PanManuelMode);

    mReset.onClick = [this](){
        if(mMode.getSelectedId() == kPanOP_BezBal){
            mBalCurve.getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_BalPan]);
            mBalCurve.resetNodes();
        }
        else{
            mLeftCurve.getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_LeftPan]);;
            mLeftCurve.resetNodes();
            mRightCurve.getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_RightPan]);;
            mRightCurve.resetNodes();
        }
        timerCallback();
    };
    
    addChildComponent(&mReset);
    addChildComponent(&mResetLable);
    
    //set what components we see
    switch (modeSelection) {
        case kPanOp_Bez:
            setAlwaysOnTop(true);
            setInterceptsMouseClicks(true, false);
            mLeftCurve.setVisible(true);
            mRightCurve.setVisible(true);
            mBalCurve.setVisible(false);
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
            
        case kPanOp_Manual:
            setAlwaysOnTop(false);
            setInterceptsMouseClicks(false, true);
            mLeftCurve.setVisible(false);
            mRightCurve.setVisible(false);
            mBalCurve.setVisible(false);
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_Split);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
            
        case kPanOp_ManBal:
            setAlwaysOnTop(false);
            setInterceptsMouseClicks(false, true);
            mLeftCurve.setVisible(false);
            mRightCurve.setVisible(false);
            mBalCurve.setVisible(false);
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_InvertedSplit);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
            
        case kPanOP_BezBal:
            setAlwaysOnTop(true);
            setInterceptsMouseClicks(true, false);
            mLeftCurve.setVisible(false);
            mRightCurve.setVisible(false);
            mBalCurve.setVisible(true);
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
            
        default:
            break;
    }

    
    mMode.addItemList(DTChoiceParameterStrings[4], 1);
    mMode.setSelectedId(modeSelection, dontSendNotification);
    mLastModeSelection = modeSelection;
    
    mMode.onChange = [this](){
        if(mMode.getSelectedId() == mLastModeSelection || !isVisible())
        {
            return;
        }
        else{
            mLastModeSelection = mMode.getSelectedId();
        }
        
        switch (mMode.getSelectedId()) {
            case kPanOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mLeftCurve.setVisible(true);
                mRightCurve.setVisible(true);
                mBalCurve.setVisible(false);
                setInterceptsMouseClicks(true, false);
                setAlwaysOnTop(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kPanOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_Split);
                mLeftCurve.setVisible(false);
                mRightCurve.setVisible(false);
                mBalCurve.setVisible(false);
                setInterceptsMouseClicks(false, true);
                setAlwaysOnTop(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
                
            case kPanOp_ManBal:
                setAlwaysOnTop(false);
                setInterceptsMouseClicks(false, true);
                mLeftCurve.setVisible(false);
                mRightCurve.setVisible(false);
                mBalCurve.setVisible(false);
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_InvertedSplit);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
                
            case kPanOP_BezBal:
                setAlwaysOnTop(true);
                setInterceptsMouseClicks(true, false);
                mLeftCurve.setVisible(false);
                mRightCurve.setVisible(false);
                mBalCurve.setVisible(true);
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;

            default:
                break;
        }
        
        timerCallback();
    };
    
    //this component will handle all mouse events and pass them to the correct bezier curve display component acordingly
    mLeftCurve.setTopLeftPosition(mInnerDisplayArea.getX(), mInnerDisplayArea.getY());
    addChildComponent(&mLeftCurve);
    mLeftCurve.setInterceptsMouseClicks(false, false);

    
    mRightCurve.setTopLeftPosition(mLeftCurve.getX(), mLeftCurve.getBottom());
    addChildComponent(&mRightCurve);
    mRightCurve.setInterceptsMouseClicks(false, false);
    
    mBalCurve.setTopLeftPosition(mInnerDisplayArea.getX(), mInnerDisplayArea.getY());
    addChildComponent(&mBalCurve);
    mBalCurve.setInterceptsMouseClicks(false, false);

    
    //affinetransform to handle scaling between the mouses position in this panel and where it should be in the child panel
    mTranslateLeft = AffineTransform::translation(Point<float>(0,0) - mInnerDisplayArea.getTopLeft().toFloat());
    mTranslateRight = AffineTransform::translation(Point<float>(0,0) - mRightCurve.getBounds().getTopLeft().toFloat());
}

TrollPanDisplay::~TrollPanDisplay()
{
    mProcessor.removeListener(this);
}

void TrollPanDisplay::paint (juce::Graphics& g)
{
    float x = mInnerDisplayArea.toFloat().getX();
    float rightX = mInnerDisplayArea.getRight();
    g.setFont(8.f);
    g.setOpacity(0.5f);

    if(mMode.getSelectedId() == kPanOp_Bez || mMode.getSelectedId() == kPanOp_Manual){
        for (int i = 0; i<DTNumberofDBMarkers; i++) {
            float y = pow(2.f, static_cast<float>(DTDBMarkers[i])/12.f);
            y/=2;
            float yRight = mInnerDisplayArea.toFloat().getCentreY() + y*mInnerDisplayArea.toFloat().getHeight();
            y=0.5-y;
            float yLeft = mInnerDisplayArea.toFloat().getY() + y*mInnerDisplayArea.toFloat().getHeight();
            
            
            g.drawLine(x, yRight, rightX, yRight);
            g.drawLine(x, yLeft, rightX, yLeft);
            if(i != 0){
                mTextRect.setCentre(mTextRect.getCentreX(), static_cast<int>(yRight));
                g.drawFittedText(String(DTDBMarkers[i]) + String("dB"), mTextRect, Justification::centredLeft, 1);
                mTextRect.setCentre(mTextRect.getCentreX(), static_cast<int>(yLeft));
                g.drawFittedText(String(DTDBMarkers[i]) + String("dB"), mTextRect, Justification::centredLeft, 1);
            }
        }
        g.drawLine(x, mInnerDisplayArea.getCentreY(), rightX, mInnerDisplayArea.getCentreY());
        mTextRect.setCentre(mTextRect.getCentreX(), mInnerDisplayArea.getCentreY());
        
        g.drawFittedText(String("-") + String(CharPointer_UTF8("∞")) + String("dB"), mTextRect, Justification::centredLeft, 1);
    }
    
    else{
        float index = 1;
        float scalar = mInnerDisplayArea.toFloat().getHeight()/(static_cast<float>(DTPanBalLabels.size()+1));
        for(auto text: DTPanBalLabels){
            float y = mInnerDisplayArea.getY() + index++ * scalar;
            
            g.drawLine(x, y, rightX, y);
            mTextRect.setCentre(mTextRect.getCentreX(), static_cast<int>(y));
            g.drawFittedText(text, mTextRect, Justification::centredLeft, 1);
        }
    }
    TrollDisplayPanelBase::paint(g);
}
void TrollPanDisplay::loadActivated(DelayTrollAudioProcessor* _p)
{
    if (&mProcessor == _p){
        
        if(isVisible()){
            mControlPanel.setBPMModeAtomic(mProcessor.getSyncStatus(), false);
            mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
            mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
            mQuantizationAmount.setSelectedId(mLastQuantSelection, dontSendNotification);
            mLastModeSelection = mProcessor.getNonModParamAsInt(kNMod_PanManuelMode);
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
            case kPanOp_Bez:
                setAlwaysOnTop(true);
                setInterceptsMouseClicks(true, false);
                mLeftCurve.setVisible(true);
                mRightCurve.setVisible(true);
                mBalCurve.setVisible(false);
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
                
            case kPanOp_Manual:
                setAlwaysOnTop(false);
                setInterceptsMouseClicks(false, true);
                mLeftCurve.setVisible(false);
                mRightCurve.setVisible(false);
                mBalCurve.setVisible(false);
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_Split);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
                
            case kPanOp_ManBal:
                setAlwaysOnTop(false);
                setInterceptsMouseClicks(false, true);
                mLeftCurve.setVisible(false);
                mRightCurve.setVisible(false);
                mBalCurve.setVisible(false);
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_InvertedSplit);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
                
            case kPanOP_BezBal:
                setAlwaysOnTop(true);
                setInterceptsMouseClicks(true, false);
                mLeftCurve.setVisible(false);
                mRightCurve.setVisible(false);
                mBalCurve.setVisible(true);
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
                
            default:
                break;
        }
    }//if processor == p
}

void TrollPanDisplay::mouseMove(const juce::MouseEvent &e)
{
    if(mMode.getSelectedId() == kPanOp_Manual||
       mMode.getSelectedId() == kPanOp_ManBal){
        return;
    }
    
    if(mInnerDisplayArea.contains(e.getPosition())){

        if(mMode.getSelectedId() == kPanOP_BezBal ){
            mPlotEditingNumber = kBezIndex_BalPlot;
        }
        else if(mLeftCurve.getBounds().contains(e.getPosition())){
            mLeftCurve.setPathColour(DTColourIds::LrgDisplayGlow);
            mRightCurve.setPathColour(DTColourIds::LrgDisplayDimGlow);
            mPlotEditingNumber = kBezIndex_LeftPlot;
        }
        else{
            mLeftCurve.setPathColour(DTColourIds::LrgDisplayDimGlow);
            mRightCurve.setPathColour(DTColourIds::LrgDisplayGlow);
            mPlotEditingNumber = kBezIndex_RightPlot;
        }
        
    }
    
}
void TrollPanDisplay::mouseDown(const MouseEvent& e)
{
    //inserts a new node
    startTimer(50);
    
    if(mMode.getSelectedId() == kPanOp_Manual||
       mMode.getSelectedId() == kPanOp_ManBal){
        return;
    }
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        switch (mPlotEditingNumber) {
                
            case kBezIndex_LeftPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mLeftCurve.mouseDown(eTranslated);}
                break;
                
            case kBezIndex_RightPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateRight);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mRightCurve.mouseDown(eTranslated);}
                break;
            
            case kBezIndex_BalPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mBalCurve.mouseDown(eTranslated);}
                break;
                
            default:
                break;
        }
    }
}

void TrollPanDisplay::mouseDrag(const MouseEvent& e)
{
    if(mMode.getSelectedId() == kPanOp_Manual||
       mMode.getSelectedId() == kPanOp_ManBal){
        return;
    }
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        switch (mPlotEditingNumber) {
                
            case kBezIndex_LeftPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mLeftCurve.mouseDrag(eTranslated);}
                break;
                
            case kBezIndex_RightPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateRight);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mRightCurve.mouseDrag(eTranslated);}
                break;
                
            case kBezIndex_BalPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mBalCurve.mouseDrag(eTranslated);}
                break;
                
            default:
                break;
        }
    }
}

void TrollPanDisplay::mouseDoubleClick(const MouseEvent& e)
{
    if(mMode.getSelectedId() == kPanOp_Manual||
       mMode.getSelectedId() == kPanOp_ManBal){
        return;
    }
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        switch (mPlotEditingNumber) {
                
            case kBezIndex_LeftPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mLeftCurve.mouseDoubleClick(eTranslated);}
                break;
                
            case kBezIndex_RightPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateRight);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mRightCurve.mouseDoubleClick(eTranslated);}
                break;
            
            case kBezIndex_BalPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mBalCurve.mouseDoubleClick(eTranslated);}
                break;
                
            default:
                break;
        }
    }
    timerCallback();
}
void TrollPanDisplay::mouseUp(const MouseEvent& e)
{
    
    if(mMode.getSelectedId() == kPanOp_Manual||
       mMode.getSelectedId() == kPanOp_ManBal){
        stopTimer();
        timerCallback();
        repaint();
        return;
    }
        
    
    if(mInnerDisplayArea.contains(e.getPosition())){
        
        switch (mPlotEditingNumber) {
                
            case kBezIndex_LeftPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mLeftCurve.mouseUp(eTranslated);}
                break;
                
            case kBezIndex_RightPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateRight);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mRightCurve.mouseUp(eTranslated);}
                break;
                
            case kBezIndex_BalPlot:{
                Point<float> translated = e.getPosition().toFloat().transformedBy(mTranslateLeft);
                MouseEvent eTranslated = e.withNewPosition(translated);
                mBalCurve.mouseUp(eTranslated);}
                break;
                
            default:
                break;
        }
    }
    
    stopTimer();
    timerCallback();
    repaint();
}
void TrollPanDisplay::timerCallback()
{
    if(isVisible()){
        if(mMode.getSelectedId() == kPanOp_Bez || mMode.getSelectedId() == kPanOP_BezBal)
            mProcessor.getAtomicTimeProcessor().processPan();
        
        mDisplayComponent.setLines();
        repaint();
    }
}

void TrollPanDisplay::setComponentVisability(int _controlMode)
{
    mLastModeSelection = _controlMode;
    mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
    mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
    mMode.setSelectedId(_controlMode, dontSendNotification);
    
    switch (_controlMode) {
        case kPanOp_Bez:
            setAlwaysOnTop(true);
            setInterceptsMouseClicks(true, false);
            mLeftCurve.setVisible(true);
            mRightCurve.setVisible(true);
            mBalCurve.setVisible(false);
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
            
        case kPanOp_Manual:
            setAlwaysOnTop(false);
            setInterceptsMouseClicks(false, true);
            mLeftCurve.setVisible(false);
            mRightCurve.setVisible(false);
            mBalCurve.setVisible(false);
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_Split);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
            
        case kPanOp_ManBal:
            setAlwaysOnTop(false);
            setInterceptsMouseClicks(false, true);
            mLeftCurve.setVisible(false);
            mRightCurve.setVisible(false);
            mBalCurve.setVisible(false);
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_InvertedSplit);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
            
        case kPanOP_BezBal:
            setAlwaysOnTop(true);
            setInterceptsMouseClicks(true, false);
            mLeftCurve.setVisible(false);
            mRightCurve.setVisible(false);
            mBalCurve.setVisible(true);
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
            
        default:
            break;
    }//swithc case
}
