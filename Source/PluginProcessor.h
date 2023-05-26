/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "BezierPlotter.h"
#include "LFOPlotter.h"
#include "DTMultiShift.h"
#include "DTDelay.h"
#include "DTPresetManager.h"
#include "DTConstraintSection.h"
#include "DTAssignableParam.h"
#include "DTAtomicTimeProcessor.h"
#include "DTModulationDefines.h"
#include "DTAtomicHord.h"


//==============================================================================
/**
*/
class DelayTrollAudioProcessor  : public AudioProcessor, public HighResolutionTimer, public ChangeBroadcaster, public AudioProcessorParameter::Listener, public AsyncUpdater
{
public:
    //==============================================================================
    DelayTrollAudioProcessor();
    ~DelayTrollAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    //MY SHIT
    //=============================================================================
    /**method to create perameters that will be tracked by the tree state*/
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    /**timer callback to handle changes to values*/
    void hiResTimerCallback() override;

    /**updates the modulation values for use by processor*/
    void updateModulations();
    
    /**handles automation changes*/
    void handleAsyncUpdate() override;
    
    /**snaps del times to proper interval values*/
    double snapDelTimes(AudioParameterFloat* _timeParam, float _intervalValue);
    
    float findMinRange();
    
    //Getters
    //==============================================================================
    
    /**get a refference to the modulation manager*/
    OwnedArray<ModManagment>& getModulationArray(){return mModManagers;};
    
    /**gets a refference to the assignable parameters*/
    OwnedArray<DTModulation::DTAssignableParam>& getAssignableParameters(){return mAssignableParams;};
    
    /**get a refference to the delay troll objects*/
    OwnedArray<AtomicTroll>& getAtomicTrolls(){return mAtomicTrolls;};
    
    /**get a refference to the "Hord" object that manages information about the Trolls as a group**/
    DTAtomicHord& getAtomicHord(){return mAtomicHord;};
    
    OwnedArray<Value>& getNonModParams(){return mNonModulatableParameters;};
    int getNonModParamAsInt(int _index){ return static_cast<int>(mNonModulatableParameters[_index]->getValue());};
    
    /**get a refference to the display Values so certain display settings are kept consistant*/
    NamedValueSet& getDisplayValues(){return mDisplayValues;};
    
    /** retunrs a pointer to the preset manager */
    DTPresetManager* getPresetManager(){return mPresetManager.get();};
    
    /** returns the current level from the input end Gain object */
    float getInputGainMeterLevel(int inChannel);
    
    /** returns the current level from the output end Gain object */
    float getOutputGainMeterLevel(int inChannel);
    
    /**retuns a refference to the BPM value set from the processor*/
    Value& getBPMValue(){return mBPM;};
    
    /**returns a refference to the bpmManagers, 0 for min 1 for max*/
    BPMManager& getBPMManager(int _ManagerID){ return _ManagerID == 0 ? mMinTimeBPMManager : mMaxTimeBPMManager;};
    
    /**returns true if the plugin is syncing to BPM or not*/
    bool getSyncStatus();
    
    std::atomic_bool& getGUIMovingStatus(){return mIsGUIMoving;};
    std::atomic_bool& getLoadingStatus(){return mIsLoading;};
    
    /**returns a refference to the Time Processor*/
    DTDSP::AtomicTimeProcessor& getAtomicTimeProcessor(){return mAtomicTimeProcessor;};
    
    /**returns true if the parameter at the given index is currently being modulated*/
    bool getModulationStatus(int _paramIndex);
    
    /**returns a refference to the thread safty atomic bools*/
    std::atomic_bool& getAtomicUpdateStatus(){return mIsUpdated;};
    std::atomic_bool& getAtomicPauseStatus(){return mUpdatePaused;};
    
    //TreeState for saving and tracking peramiters
    AudioProcessorValueTreeState tree;
    
    //Listeners
    //===============================================================================
    /**listener class to tell our guy when the user has loaded a new preset*/
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void loadActivated(DelayTrollAudioProcessor*) = 0;
    };
    
    /**add listener*/
    void addListener(Listener* listenerToAdd){m_listeners.add(listenerToAdd);};
    /**remove listener*/
    void removeListener(Listener* listenerToRemove){jassert(m_listeners.contains(listenerToRemove)); m_listeners.remove(listenerToRemove);};
    
    /**listener class to tell our guy when the user has loaded a new preset*/
    class BPMListener
    {
    public:
        virtual ~BPMListener() = default;
        virtual void BPMUpdated(DelayTrollAudioProcessor*) = 0;
    };
    
    /**add listener*/
    void addBPMListener(BPMListener* listenerToAdd){mBPMListeners.add(listenerToAdd);};
    /**remove listener*/
    void removeBPMListener(BPMListener* listenerToRemove){jassert(mBPMListeners.contains(listenerToRemove)); mBPMListeners.remove(listenerToRemove);};
    
    /**calling this function will bang the gui for an update when the BPM Changes*/
    void BPMUpdated();
    
    /**called by the host when it detects one of the modulatable parameters is changing*/
    void parameterValueChanged (int parameterIndex, float newValue)override;

    /**called by the host when it detects one of the modulatable parameters is about to chang or has finished changing*/
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting)override{
        DBG("Parameter: "<<parameterIndex<<" changing: "<<static_cast<int>(gestureIsStarting));
    };
    
private:
    /**calling this function will bang the gui for an update when preset is loaded*/
    void loadActivate();
    
    /**initializes our Playhead related info when the user hits play, this information cannot be received befor this*/
    void initializePlayhead();
    
    /**sets the modManagers position based off the current playhead positioin*/
    void findPosition();
    
    /**handles inconsisitant blocksizes given by the DAW**/
    void blockSizeChanged(int samplesPerBlock);
    
    //display stuff
    //======================================
    float mLeftInputLevel, mRightInputLevel, mLeftOutputLevel, mRightOutputLevel;
    
    //wrapper for all the modulation curves
    OwnedArray<ModManagment> mModManagers;
    
    OwnedArray<DTModulation::DTAssignableParam> mAssignableParams;
    
    //keeps track of values so the GUI will remember what panels were visable during when the GUI last closed
    NamedValueSet mDisplayValues;
    
    //array to hold listeners
    ListenerList<Listener> m_listeners;
    
    //array to hold listeners
    ListenerList<BPMListener> mBPMListeners;
    
    bool mIsRunning;
    //Managers
    std::unique_ptr<DTPresetManager> mPresetManager;
    DTAtomicHord mAtomicHord;
    DTDSP::AtomicTimeProcessor mAtomicTimeProcessor;
    
    //processors
    //======================================
    std::atomic_bool mIsUpdated;
    std::atomic_bool mUpdatePaused;
    OwnedArray<AtomicTroll> mAtomicTrolls;
    std::atomic_bool mIsTimerRunning;
    
    //shift
    DTDSP::DTMultiShift mMultiShift;
    DTDSP::DTMultiShift mMultiShiftRight;
    AudioBuffer<float> mShiftedBuffers;
    
    //filter
    OwnedArray<dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>> mIIRFilters;
    OwnedArray<dsp::AudioBlock<float>> mFilteredBlocks;
    OwnedArray<AudioBuffer<float>> mDataForFilteredBlocks;
    
    //delay
    OwnedArray<DTDelay> mDelayLines;
    AudioBuffer<float> mDelayBuffer;
    
    //context
    dsp::ProcessSpec mProcessorSpec;
    double mLastSampleRate;
    int64 mPlayheadTimeInSamples;
    bool mUpdateMin, mUpdateMax;
    bool isPaused;
    double mExpectedTimeInSeconds;
    double mPlayheadTimeIntervalInSeconds;
    
    //BPM
    Value mBPM, mTimeSigNum;
    BPMManager mMinTimeBPMManager;
    BPMManager mMaxTimeBPMManager;
    Optional <juce::AudioPlayHead::PositionInfo> mPositionInfo;
    Value& mMaxTimeQuantRef, mMinTimeQuantRef;
    //arrays that modulate parameters, should be handled by mModManagers but accessed by processor
    double mDTmodArray[kMod_TotalNumOfMods][DTmodArraySize];
    
    //timer self correction memes
    int64 mCurrentTime = 0;
    int64 mScheduledTime = 0;
    int64 mDrift = 0;
    int64 DTModulationUpdateInTicks;
    bool mDriftFlag = false;
    int mNextTimerInterval = 0;
    
    
    //atomic management stuff
    //======================================
    std::atomic_bool mIsRangeUpdateNeeded;
    std::atomic_bool mIsMultUpdateNeeded;
    std::atomic_bool mIsTargetUpdateNeeded;
    
    OwnedArray<Value> mNonModulatableParameters;
    
    std::atomic_bool mIsLoading;
    std::atomic_bool mIsGUIMoving;
    std::atomic_int mChangeConsumption;
   
    //other
    bool mIsProcessorFinished;
    bool mIsPlayheadInitalized;
    
    bool mIsNonRealTime = false;
    double mTimeSinceLastModUpdate=0.0;
    
    //std::atomic<bool> mShouldRunTimer{true};
    //std::atomic_flag mTimerLock = ATOMIC_FLAG_INIT;
    
    
   
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayTrollAudioProcessor)
};
