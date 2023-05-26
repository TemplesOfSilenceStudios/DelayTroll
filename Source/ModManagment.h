/*
  ==============================================================================

    ModManagment.h
    Created: 19 Mar 2019 5:16:55pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "BPMManager.h"

/**Base class for managing variouse modulation curves*/
class ModManagment
{
public:
    
    /**Creates a new manager for the provided array with the provided arraySize*/
    ModManagment(double* const& Array, float _arraySize, int _managmentID, Value& _BPM, Value& _timeSigNumerator);
    virtual ~ModManagment(){};
    
    /**sets the ms value from Hz*/
    inline void setTimerFromHz(float _inputHz){ m_ms = static_cast<int>(1000.f/_inputHz/100.f);};
    
    /**attatches a Dummy value to the manager*/
    void attatchDummyValue(){
        mParamTypes.add(-1);
        mIsPlaceholder.add(true);};
    
    /**Removes a Value at a given intex*/
    inline void removeValue(int _index){
        mIsPlaceholder.remove(_index);
        mParamTypes.remove(_index);};
    
    /**updating the values when a user loads a new preset with the gui open*/
    inline void updateValueAttatchment(int _newType, int _index){
        mIsPlaceholder.set(_index, false);
        mParamTypes.set(_index, _newType);};
    
    /**clears all values*/
    inline void clearAllValues(){
        mIsPlaceholder.clear();
        mParamTypes.clear();};

    
    /**get the total number of values managing*/
    inline int getNumOfValues(){return mParamTypes.size();};
    
    /**gets weather the value is a dummy or not*/
    inline bool getPlaceholderStatus(int _index){return mIsPlaceholder[_index];};
    
    /**get the parameter type of at the given index*/
    inline int getParamAtIndex(int _arrayIndex){return mParamTypes[_arrayIndex];};
    
    /**get the depth value*/
    inline double getDepthValue(int _arrayIndex){return static_cast<double>(depthValues[_arrayIndex]->getValue());};
    
    /**returns a pointer to the depth value at a given index*/
    Value* getDepthValueObject(int _arrayIndex){return depthValues.getUnchecked(_arrayIndex);};
    
    /**sets the depth Value*/
    inline void setDepthValue(int _index, double _newVal){depthValues[_index]->setValue(_newVal);};
    
    /**get the size of the array*/
    inline float getarraySize(){return m_arraySize;};
    
    /**gets the polar value*/
    inline double getPolarValue(){return static_cast<double>(mPolar.getValue());}
    
    /**sets the polar value*/
    inline void setPolarValue(float _newVal){mPolar.setValue(_newVal);};
    
    /**clears the array*/
    inline void clearPlotArray() {for(int i = 0; i < static_cast<int>(m_arraySize); i++){m_plotArray[i] = 0;}};
    
    /**gets an xmlelement representing the Managers state*/
    XmlElement* getManagerXMLElement(int _managerNum);
    
    /**sets the Managers state via an xmlelement*/
    void setManagerXMLElement(XmlElement* _xmlElement);

    /**gets a refference to the plot position*/
    int& getPlotPossition(){return plotPossition;};
    
    /**gets the ID of the manager*/
    int& getManagerID(){return mManagerID;};
    
    /**gets the timer interval*/
    int& getTimerInterval(){return m_ms;};
    
    /**sets the timer interval*/
    void setTimerTinterval(int _inputTimerInterval){m_ms = _inputTimerInterval;};
    
    /**advances the plot position*/
    inline void advancePlotPosition()
    {
//        mPosition += mStepSize;
//        if(mPosition > m_arraySize - 1.f)
//        {
//            mPosition = mPosition - (m_arraySize - 1.f);
//        }
//        plotPossition = roundToInt(mPosition);
        
        mPosition += mStepSize;
        if(mPosition >= m_arraySize)
        {
            mPosition = mPosition - m_arraySize;
        }
        plotPossition = truncatePositiveToUnsignedInt(mPosition);
        
    }
    
    /**gets a refferene to the parameter types*/
    Array<int>& getParamTypes(){return mParamTypes;};
    
    /**updates the stepsize based on the incoming ms*/
    void updateStepSize(double _msPerCycle);
    
    void updateStepSizeByNoteFraction();
    
    /**gets the calculates Hz from mStepSize*/
    double getStepSizeAsMsPerCycle();
    
    void setPositionFromNotefractions(double _notefractionsSinceEdit);
    
    void setPosition(double _timeInMs);
    
    /**sets the Manager to BPM sync mode and calculates bar fractions if true */
    void setBPMSyncMode(bool _HzShouldSync);
    
    /**returns true if the manager is syncing its BPM*/
    inline bool getBPMSyncStatus(){return mBPMSyncMode;};
    
    
    inline BPMManager& getBPMManager(){return mBPMManager;};
    
    //some protected members our children need access to
protected:
    double * const m_plotArray;
    float m_arraySize;
    float mArratIndexEnd;
    int m_ms;
    int plotPossition;
    Value mPolar;
    
private:
    Array<int> mParamTypes;

    //holds the depth values of each perameter being modulated
    OwnedArray<Value> depthValues;
    
    //holds weather a value is a dummy or not
    Array<bool> mIsPlaceholder;
    
    int mManagerID;
    
    float mStepSize;
    float mPosition;
    
    BPMManager mBPMManager;

    //Value mBarFractions;
    //this represents the ID of the dropdown menu in the corisponding GUI, this is offten 1 more than any index it is used to refer too
    int mQuantizationSelection;
    bool mBPMSyncMode;
    var mMaxNumberOfBarFractions;
    
    //Value& mBPM;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModManagment)
};
