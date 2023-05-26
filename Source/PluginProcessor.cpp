/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DTUsedParameters.h"
#include "DTInterfaceDefines.h"
#include "DTAudioFunctions.h"
#include "DTHelperFunctions.h"


//==============================================================================
DelayTrollAudioProcessor::DelayTrollAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), tree(*this, nullptr, "PARAMETER", createParameterLayout()),
                        mAtomicHord(DTParameterDefaultValue[kParam_MinDelayTime],
                                    DTParameterDefaultValue[kParam_MaxDelayTime],
                                    static_cast<int>(DTNonModulatableDefaultValues[kNMod_NumbOfDels])),
                        mAtomicTimeProcessor(mAtomicTrolls, mAtomicHord, mUpdatePaused, mAssignableParams, mNonModulatableParameters, mModManagers, mIIRFilters, mLastSampleRate),
                        mMultiShift(mAtomicTrolls, mAtomicHord), mMultiShiftRight(mAtomicTrolls, mAtomicHord),
                        mMinTimeBPMManager(mBPM, mTimeSigNum), mMaxTimeBPMManager(mBPM, mTimeSigNum),
                        mMaxTimeQuantRef(mMaxTimeBPMManager.getQuantTime()), mMinTimeQuantRef(mMinTimeBPMManager.getQuantTime())
#endif
{
   
    DTModulationUpdateInTicks = static_cast<double>(Time::getHighResolutionTicksPerSecond()) * static_cast<double>(DTModulationUpdateFrequency) / 1000.0;
    
    DBG("TicksPerSecond: "<< Time::getHighResolutionTicksPerSecond());
    
    mBPM.setValue(var(mPositionInfo->getBpm().orFallback(120.0)));\
    
    mPositionInfo->setTimeSignature(juce::AudioPlayHead::TimeSignature());
   
    mTimeSigNum.setValue(var(4));
    
    mPlayheadTimeInSamples = mPositionInfo->getTimeInSamples().orFallback(0);
    
    
    //fill the wrapper with the different types of curves
    for(int i = 0 ; i < kMod_TotalNumOfMods; i++){
        if(i<DTnumOfMods){
            mModManagers.add(new BezierPlotter(mDTmodArray[i], (float)DTmodArraySize, 0.f, i, mBPM, mTimeSigNum));
        }
        else if(i<DTnumOfMods*2){
            mModManagers.add(new LFOPlotter(mDTmodArray[i], (float)DTmodArraySize, 0.5f, i,  mBPM, mTimeSigNum));
        }
        else{
            mModManagers.add(new BezierPlotter(mDTmodArray[i], (float)DTmodArraySize, 0.f, i,  mBPM, mTimeSigNum));
        }
    }
    
    for(int i = 0 ; i < kAss_TotalNumOFAssignableParams; i++){
        mAssignableParams.add(new DTModulation::DTAssignableParam(i,
                                                                  *static_cast<AudioParameterFloat*>(tree.getParameter(StringRef(DTAssignableParamsID[i])))));

        mAssignableParams[i]->modulationManagers.clear();
    }
    
    mNonModulatableParameters.clear();
    mNonModulatableParameters.ensureStorageAllocated(kNMod_TotalNumNModParam);
    for (int i = 0; i<kNMod_TotalNumNModParam; i++) {
        mNonModulatableParameters.add(new Value(DTNonModulatableDefaultValues[i]));
    }
    
    //Setting up nodes for the Bezier Curves note that the list of bezier defalt nodes is shorter
    for(int i = kMod_EQLowPassBez; i < kMod_TotalNumOfMods; i++){
        dynamic_cast<BezierPlotter*>(mModManagers[i])->setNodes(DTBezierDefaultNodes[i - (DTnumOfMods*2)]);
    }

    
    //initialize our delay trolls
    //just make 100 so memory doesnt need to be alocated later
    mIIRFilters.ensureStorageAllocated(maxNumberOfTrolls);
    mDelayLines.ensureStorageAllocated(maxNumberOfTrolls);
    
    
    for(int j =0; j < maxNumberOfTrolls; j++)
    {
        mDelayLines.add(new DTDelay());
        mIIRFilters.add(new dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>);
    }
    
    Array<float> trollAttributes;
    
    for (int att = 0; att < TrollParams::kTroll_TotalNumOfAttributes; att++) {
        trollAttributes.add(TrollParams::DefaultTrollAttributeValues[att]);
    }
    
    
    for (int troll = 0; troll < DTTotalNumberOfTrolls; troll++) {
        mAtomicTrolls.add(new AtomicTroll(trollAttributes));
    }
    
    mLastSampleRate = 44100.f;
    
    //initalize display values
    for(int i = 0 ; i < kDisVal_TotalNumDisplayValues; i++){
        mDisplayValues.set(DTDisplayValueIDs[i], DTDisplayValueDefault[i]);
    }
    
    mShiftedBuffers.setSize(48, 2048);
    mIsRunning = false;
    mPresetManager = std::make_unique<DTPresetManager>(this);
    
    mLeftInputLevel = 0.f;
    mRightInputLevel = 0.f;
    mLeftOutputLevel = 0.f;
    mRightOutputLevel = 0.f;
    
    mIsProcessorFinished = true;
    
    mMinTimeBPMManager.initalizeBPMInfo();
    mMaxTimeBPMManager.initalizeBPMInfo();
    
    mMinTimeBPMManager.setNoteFractionsFromTime(DTParameterDefaultValue[kParam_MinDelayTime]);
    mMaxTimeBPMManager.setNoteFractionsFromTime(DTParameterDefaultValue[kParam_MaxDelayTime]);
    
    
    mAtomicHord.setHordNoteFractionRange(mMinTimeBPMManager.getNoteFractionInt(), mMaxTimeBPMManager.getNoteFractionInt());
    
    mIsPlayheadInitalized = false;
    
    mUpdateMin = false;
    mUpdateMax = false;
   
    mIsUpdated.store(true);
    mUpdatePaused.store(false);
    mIsTimerRunning.store(false);
    mIsRangeUpdateNeeded.store(false);
    mIsMultUpdateNeeded.store(false);
    mIsTargetUpdateNeeded.store(false);
    
    mAtomicTimeProcessor.resizeMult(static_cast<int>(DTNonModulatableDefaultValues[kNMod_NumbOfDels]));
    
    tree.getParameter(DTParameterID[kParam_MinDelayTime])->addListener(this);
    tree.getParameter(DTParameterID[kParam_MaxDelayTime])->addListener(this);
    tree.getParameter(DTParameterID[kParam_OutputLevel])->addListener(this);
    tree.getParameter(DTParameterID[kParam_Feedback])->addListener(this);
    tree.getParameter(DTParameterID[kParam_GravetyExpo])->addListener(this);
    mIsLoading.store(false);
   
}

DelayTrollAudioProcessor::~DelayTrollAudioProcessor()
{
    
    releaseResources();
    tree.getParameter(DTParameterID[kParam_MinDelayTime])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_MaxDelayTime])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_OutputLevel])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_Feedback])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_GravetyExpo])->removeListener(this);
}

//==============================================================================
const String DelayTrollAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayTrollAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayTrollAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayTrollAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayTrollAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayTrollAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayTrollAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayTrollAudioProcessor::setCurrentProgram (int index)
{
}

const String DelayTrollAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayTrollAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DelayTrollAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    //Logger::writeToLog("isNonRealtime: " + String((int)isNonRealtime()));
    //mShouldRunTimer.store(true);
    //Logger::writeToLog("Starting Timer at prepare to paly");
    mIsNonRealTime = isNonRealtime();
    
    if(mIsNonRealTime){
        DBG("Prepairing non realtime bounce");
        findPosition();
        releaseResources();
        mTimeSinceLastModUpdate = 0.0;
    }
    else{
        DBG("Preparing Realtime");
        findPosition();
        //startTimer(DTModulationUpdateFrequency);
        //DBG("UpdateInTicks: "<< DTModulationUpdateInTicks);
        mCurrentTime = Time::getHighResolutionTicks();
        //DBG("CurrentTime: "<< mCurrentTime);
        //mScheduledTime = mCurrentTime;
        //DBG("mScheduledTime: "<< mScheduledTime);
        //DBG("Starting Timer==================");
        mDriftFlag = true;
        mDrift = 0;
        hiResTimerCallback();
    }
    
    mProcessorSpec.sampleRate = sampleRate;
    mProcessorSpec.numChannels = getTotalNumInputChannels();
    mProcessorSpec.maximumBlockSize = samplesPerBlock;
    mLastSampleRate = sampleRate;
    
    mShiftedBuffers.setSize(maxNumberOfTrolls * 2, samplesPerBlock);
    mShiftedBuffers.clear();
    mDelayBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    mDelayBuffer.clear();
    
    mMultiShift.prep(0, samplesPerBlock, mShiftedBuffers);
    
    if(getTotalNumInputChannels() > 1)
        mMultiShiftRight.prep(1, samplesPerBlock, mShiftedBuffers);
    
    mFilteredBlocks.clear();
    mDataForFilteredBlocks.clear();
    
    for(int i = 0 ; i < maxNumberOfTrolls; i++){
        mDelayLines[i]->prep(sampleRate, getTotalNumInputChannels());
        mDataForFilteredBlocks.add(new AudioBuffer<float>(getTotalNumInputChannels(), samplesPerBlock));
        mDataForFilteredBlocks[i]->clear();
        mFilteredBlocks.add(new dsp::AudioBlock<float>(*mDataForFilteredBlocks[i]));
       
        
        mIIRFilters[i]->reset();
        mIIRFilters[i]->prepare(mProcessorSpec);
        *mIIRFilters[i]->state = *dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 2000.f, 4.f);
        
    }
    mIsRunning = true;
    mIsProcessorFinished = true;
    
    //iir.reset();
    
    double sampleinSeconds = 1.0/sampleRate;
    mPlayheadTimeIntervalInSeconds = sampleinSeconds * static_cast<double>(samplesPerBlock);
    mExpectedTimeInSeconds = mPositionInfo->getTimeInSeconds().orFallback(0.0);
    isPaused = !mPositionInfo->getIsPlaying();
}

void DelayTrollAudioProcessor::blockSizeChanged(int samplesPerBlock)
{

    DBG("Block SIzed Changed");
    mProcessorSpec.maximumBlockSize = samplesPerBlock;
    mShiftedBuffers.setSize(maxNumberOfTrolls * 2, samplesPerBlock);
    mDelayBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);


    mFilteredBlocks.clear();
    
    for(int i = 0 ; i < maxNumberOfTrolls; i++){

        mDataForFilteredBlocks[i]->setSize(getTotalNumInputChannels(), samplesPerBlock);
        mFilteredBlocks.add(new dsp::AudioBlock<float>(*mDataForFilteredBlocks[i]));
        jassert( mFilteredBlocks[i]->getNumSamples() == samplesPerBlock);
        
    }
    
    double sampleinSeconds = 1.0/mProcessorSpec.sampleRate;
    mPlayheadTimeIntervalInSeconds = sampleinSeconds * static_cast<double>(samplesPerBlock);
    
}

void DelayTrollAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    DBG("Releasing Resources");
    for(auto& a: mDelayLines)
    {
        a->reset();
    }
    mDelayBuffer.clear();
    mFilteredBlocks.clear();
    mDataForFilteredBlocks.clear();
    mIsRunning = false;
    //mShouldRunTimer.store(false, std::memory_order_release);
    //Logger::writeToLog("mShouldRunTimer at releaseReasorces: " + String((int)mShouldRunTimer.load(std::memory_order_acquire)));
    
    //while(mTimerLock.test_and_set(std::memory_order_acquire)) {} // spin until the timer callback completes
    
    stopTimer();
    //Logger::writeToLog("Stopping Timer");

    DBG("STopping Timer==================");

    mIsProcessorFinished = true;
    mIsPlayheadInitalized = false;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayTrollAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayTrollAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
   
    jassert(mIsProcessorFinished);
    mIsProcessorFinished = false;
    
    //Buffer Management in case of live changes to samples per block
    if(mFilteredBlocks[0]->getNumSamples() != buffer.getNumSamples()){
        blockSizeChanged(buffer.getNumSamples());
    }
    
    //for ofline rendering we must manage the timer in the process block
    if(isNonRealtime()){
        //*************bad do division out of process block also only works because useres arnt adusting the plots in while bouncing
        if(mTimeSinceLastModUpdate >= static_cast<double>(DTModulationUpdateFrequency)/1000.0){
            hiResTimerCallback();
            //wrapp the timetracker
            mTimeSinceLastModUpdate -= static_cast<double>(DTModulationUpdateFrequency)/1000.0;
        }
        //advance the timetracker
        mTimeSinceLastModUpdate += mPlayheadTimeIntervalInSeconds;
    }
    
    if(!mUpdatePaused.load(std::memory_order_acquire))
    {
        mIsUpdated.store(false, std::memory_order_release);
        
        mAtomicHord.updateHordSizeToTarget();
        for (int i = 0; i<mAtomicHord.getCurrentHordSize(); i++) {
            mAtomicTrolls[i]->updateToTargets();
        }
        //if(eq_updated)
            //state= new state
        //jassert(!mUpdatePaused.load(std::memory_order_acquire));
        mIsUpdated.store(true, std::memory_order_release);
    }
    
    if(getPlayHead()->getPosition().hasValue())
        mPositionInfo = getPlayHead()->getPosition();
    
    if(!mIsPlayheadInitalized)
    {
        //DBG("INITIALIZING");
        initializePlayhead();
    }
    
    //This is making sure the modulation position is corrent and if paused and restarted at a different point the buffers are cleared
    if(!mPositionInfo->getIsPlaying() != isPaused){
        if(mPositionInfo->getIsPlaying()){
            isPaused = false;
            findPosition();
            if(!approximatelyEqual(static_cast<float>(mPositionInfo->getTimeInSeconds().orFallback(0)),
                                   static_cast<float>(mExpectedTimeInSeconds))){
                for(auto& line: mDelayLines)
                {
                    line->reset();
                }
                for(auto& Filter: mIIRFilters)
                {
                    Filter->reset();
                }
                DBG("Resumed position: "<<mPositionInfo->getTimeInSeconds().orFallback(0)<<" Expected: "<< mExpectedTimeInSeconds);
                mExpectedTimeInSeconds = mPositionInfo->getTimeInSeconds().orFallback(0);
            }
        }
        else{
            isPaused = true;
            mExpectedTimeInSeconds = mPositionInfo->getTimeInSeconds().orFallback(0);
        }
    }
    else if (!approximatelyEqual(static_cast<float>(mPositionInfo->getTimeInSeconds().orFallback(0)), static_cast<float>(mExpectedTimeInSeconds)))
    {
        //DBG("playhead moved===========================");
        if(!isPaused)
            findPosition();
        DBG("moved position: "<<mPositionInfo->getTimeInSeconds().orFallback(0)<<" Expected: "<< mExpectedTimeInSeconds);
        mExpectedTimeInSeconds = mPositionInfo->getTimeInSeconds().orFallback(0);
    }
    
    if(!isPaused)
        mExpectedTimeInSeconds += mPlayheadTimeIntervalInSeconds;
    
    
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    jassert(buffer.getMagnitude(0, buffer.getNumSamples()) <= 1.f);
    
    
    buffer.applyGain(mAssignableParams[KAss_InputLevel]->getNextValue());
    
    mLeftInputLevel = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    
    if(totalNumInputChannels > 1)
        mRightInputLevel = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
    
    if(!*tree.getRawParameterValue(DTParameterID[kBParam_PitchBypass])){
        mMultiShift.process(buffer);

        if(totalNumInputChannels > 1)
            mMultiShiftRight.process(buffer);
        
        dsp::AudioBlock<float> bigBlock(mShiftedBuffers);
        
        for(int i = 0 ; i < mAtomicHord.getCurrentHordSize(); i++){
            dsp::AudioBlock<float> littleBlock(bigBlock.getSubsetChannelBlock(i*2, 2));
            mFilteredBlocks[i]->copyFrom(littleBlock);
            if(!*tree.getRawParameterValue(DTParameterID[kBParam_EQBypass])){
                dsp::ProcessContextReplacing<float> context(*mFilteredBlocks.getUnchecked(i));
                mIIRFilters[i]->process(context);
            }
        }
    }
    else if(!*tree.getRawParameterValue(DTParameterID[kBParam_EQBypass]))
    {
        for(int i = 0 ; i < mAtomicHord.getCurrentHordSize(); i++){
            mFilteredBlocks[i]->copyFrom(buffer);
            
            dsp::ProcessContextReplacing<float> context(*mFilteredBlocks[i]);
            mIIRFilters[i]->process(context);
        }
    }
    else
    {
        for(int i = 0 ; i < mAtomicHord.getCurrentHordSize(); i++){
            mFilteredBlocks[i]->copyFrom(buffer);
        }
    }
   
    mDelayBuffer.clear();
    
    for(int i = 0 ; i < mAtomicHord.getCurrentHordSize(); i++){
        mDelayLines[i]->process(*mFilteredBlocks[i],
                                mAtomicTrolls[i]->mAttributes[TrollParams::kTroll_DelayTime]/1000.f,
                                mAssignableParams[kAss_Feedback]->getNextValue(),
                                mAssignableParams[kAss_SmoothFactor]->getNextValue(),
                                mAtomicTrolls[i]->mAttributes[TrollParams::kTroll_Amplitude],
                                mAtomicTrolls[i]->mAttributes[TrollParams::kTroll_LeftAtenuation],
                                mAtomicTrolls[i]->mAttributes[TrollParams::kTroll_RightAtenuation],
                                *tree.getRawParameterValue(DTParameterID[kBParam_SoftClipperBypass]),
                                mDelayBuffer,
                                mDelayBuffer.getNumSamples());
    }
   
    
    
    mDelayBuffer.applyGain(mAssignableParams[kAss_WetDry]->getNextValue());
    buffer.applyGain(1.0f - mAssignableParams[kAss_WetDry]->getNextValue());
    
    buffer.addFrom(0, 0, mDelayBuffer, 0, 0, buffer.getNumSamples());
    
    if(totalNumInputChannels > 1)
        buffer.addFrom(1, 0, mDelayBuffer, 1, 0, buffer.getNumSamples());
    
    buffer.applyGain(mAssignableParams[kAss_OutputLevel]->getNextValue());
    
    //hard clipping so the driver doesnt freak out
    if(buffer.getMagnitude(0, buffer.getNumSamples()) > 1.f){
        float* sample = buffer.getWritePointer(0);
        for(int i =0; i<buffer.getNumSamples();i++)
        {
            sample[i] = jlimit(-1.f,1.f,sample[i]);
        }//left channel
        if(totalNumInputChannels > 1){
            sample = buffer.getWritePointer(1);
            for(int i =0; i<buffer.getNumSamples();i++)
            {
                sample[i] = jlimit(-1.f,1.f,sample[i]);
            }
        }// right channel
    }// if hard clipping needed

   
    mLeftOutputLevel = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    
    if(totalNumInputChannels > 1)
        mRightOutputLevel = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
    
    mIsProcessorFinished = true;
    
}

//==============================================================================
bool DelayTrollAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayTrollAudioProcessor::createEditor()
{
    return new DelayTrollAudioProcessorEditor (*this);
}

//==============================================================================
void DelayTrollAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    //XmlElement preset("DT_StateInfo");
    XmlElement preset(tree.state.getType());
    XmlElement* presetBody = new XmlElement("DT_Preset");
    
    //DBG("Max TIme Value at Save: "<<tree.getParameter(DTParameterID[kParam_MaxDelayTime])->getCurrentValueAsText());
    
    mPresetManager->getXmlForPreset(presetBody, mNonModulatableParameters);
    
    preset.addChildElement(presetBody);
    
    int j = 0;
  

    for(auto* plotParam : mModManagers){
        
        preset.addChildElement(plotParam->getManagerXMLElement(j));
        
        if(auto* p = dynamic_cast<BezierPlotter*>(plotParam)){
            preset.addChildElement(p->getXMLElement(j++));
        }
        else if (auto* p = dynamic_cast<LFOPlotter*>(plotParam)){
            preset.addChildElement(p->getXMLElement(j++));
        }
    }
    
    preset.addChildElement(mMinTimeBPMManager.getManagerXMLElement(0));
    preset.addChildElement(mMaxTimeBPMManager.getManagerXMLElement(1));
    preset.addChildElement(mAtomicTimeProcessor.getTimeProcessorXMLElement());
    Identifier bpmID = "BPMID";
    
    preset.setAttribute(bpmID, static_cast<double>(mBPM.getValue()));
    
    copyXmlToBinary(preset, destData);
    
    preset.deleteAllChildElements();
    
}

void DelayTrollAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    tree.getParameter(DTParameterID[kParam_MinDelayTime])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_MaxDelayTime])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_OutputLevel])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_Feedback])->removeListener(this);
    tree.getParameter(DTParameterID[kParam_GravetyExpo])->removeListener(this);
    
    DBG("Loading==================================");
    mIsLoading.store(true, std::memory_order_seq_cst);
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    for(auto param : mAssignableParams)
    {
        param->modulationManagers.clear();
        param->isAttached = false;
        //DBG(param->paramID<<": reset");
    }
    
    
    if(xmlState != nullptr)
    {
        if(xmlState->hasTagName(tree.state.getType()))
        {
            int n = 0;
            
            forEachXmlChildElement(*xmlState, e)
            {
                //DBG("Tage Name: "<<e->getTagName());
                if(e->hasTagName("DT_Preset"))
                {
                    mPresetManager->loadPresetForXml(e, mNonModulatableParameters);
                }
                
                if(e->hasTagName("BPMManager0"))
                {
                    mMinTimeBPMManager.setManagerXMLElement(e);
                }
                
                if(e->hasTagName("BPMManager1"))
                {
                    mMaxTimeBPMManager.setManagerXMLElement(e);
                }

                
                String plotNumberString = "bezierPlot";
                plotNumberString += n;
                StringRef plotNumberStringRef = plotNumberString;
                
                String LFOplotNumberString = "LFOPlot";
                LFOplotNumberString += n;
                StringRef LFOplotNumberStringRef = LFOplotNumberString;
                
                String managerNumberString = "Manager";
                managerNumberString += n;
                StringRef managerNumberStringRef = managerNumberString;

                //get the manager if it exists
                if(e->hasTagName(managerNumberStringRef)){
                    if(e->getNumAttributes() >= 3){
                        mModManagers[n]->setManagerXMLElement(e);
                    }
                }
                
                //get the bazier plots
                if(e->hasTagName(plotNumberStringRef) && n < mModManagers.size())
                {
                    dynamic_cast<BezierPlotter*>(mModManagers[n])->setXMLElement(e);
                    n++;
                }
                //get the lfo plots
                if(e->hasTagName(LFOplotNumberStringRef))
                {
                    dynamic_cast<LFOPlotter*>(mModManagers[n])->setXMLElement(e);
                    n++;
                }
                
            }//for each element loop
        }//if has tag name tree state
        
    }//if nullptr


    StringRef bpmID = "BPMID";
      
    if(!mIsPlayheadInitalized)
    {
        mBPM.setValue(var(xmlState->getDoubleAttribute(bpmID)));
    }
    
    //updateTotalFractionSlots();
    //static_cast<var::VariantType_Int>(var(2))
    for(auto m : mModManagers)
    {
        for(auto param : m->getParamTypes())
        {
            //DBG("=========post load=======");
            //DBG(m->getManagerID()<<": " <<param);
            if(param != -1){
                mAssignableParams[param]->modulationManagers.set(m->getManagerID(), m);
                mAssignableParams[param]->isAttached = true;
                
                //DBG(mAssignableParams[param]->paramID<<": attatched");
            }
        }
    }
    
    findPosition();
    for (auto param : mAssignableParams) {
        if(!param->isAttached)
            param->setValue(*tree.getRawParameterValue(StringRef(param->paramID)));
    }
    
    
    if(getSyncStatus())
    {
        //DBG("SETTING TIMES AS IF THE PRESET LOADING IS SYNCED");
        if(!mAssignableParams[kAss_MinTime]->isAttached && !mAssignableParams[kAss_MaxTime]->isAttached){
            mAssignableParams[kAss_MinTime]->setValue(static_cast<float>(mMinTimeBPMManager.getQuantTime().getValue()));
            mAssignableParams[kAss_MaxTime]->setValue(static_cast<float>(mMaxTimeBPMManager.getQuantTime().getValue()));
        }
        
        //static_cast<RangedAudioParameter*>(&mAssignableParams[kAss_MinTime]->sharedOrigninalValue)->setValue(mMinTimeBPMManager.getQuantTime().getValue());
        //static_cast<RangedAudioParameter*>(&mAssignableParams[kAss_MaxTime]->sharedOrigninalValue)->setValue(mMaxTimeBPMManager.getQuantTime().getValue());
        //mAssignableParams[kAss_MinTime]->sharedOrigninalValue.setValue(mMinTimeBPMManager.getQuantTime().getValue());
        //mAssignableParams[kAss_MaxTime]->sharedOrigninalValue.setValue(mMaxTimeBPMManager.getQuantTime().getValue());
      
        tree.getParameterAsValue(DTParameterID[kParam_MinDelayTime]).setValue(mMinTimeBPMManager.getQuantTime().getValue());
        tree.getParameterAsValue(DTParameterID[kParam_MaxDelayTime]).setValue(mMaxTimeBPMManager.getQuantTime().getValue());
        
    }
    DTWaitForUpdate(mUpdatePaused, mIsUpdated);
    mAtomicHord.setTimeTarget(*tree.getRawParameterValue(DTParameterID[kParam_MinDelayTime]),*tree.getRawParameterValue(DTParameterID[kParam_MaxDelayTime]));
    mAtomicHord.setHordNoteFractionRange(mMinTimeBPMManager.getNoteFractionInt(), mMaxTimeBPMManager.getNoteFractionInt());
    mAtomicTimeProcessor.setTimeProcessorXMLElement(xmlState->getChildByName(StringRef("AtomicTimeProcessor")));
    //*tree.getRawParameterValue(DTParameterID[kParam_NumOfDelays]) = static_cast<float>(mAtomicTimeProcessor.getMults().size());
    //DBG("=============================================");

    
    xmlState->deleteAllChildElements();
    
    //DBG("MAX TIME BEFORE LOADACTIVATING: "<< tree.getParameter(DTParameterID[kParam_MaxDelayTime])->getValue());

    loadActivate();
    DBG("Loading end==================================");

    //I think change consumption is so the async updator doesnt run when we load a preset
    mChangeConsumption.store(5, std::memory_order_release);
    mIsLoading.store(false, std::memory_order_seq_cst);
//    sendChangeMessage();
    tree.getParameter(DTParameterID[kParam_MinDelayTime])->addListener(this);
    tree.getParameter(DTParameterID[kParam_MaxDelayTime])->addListener(this);
    tree.getParameter(DTParameterID[kParam_OutputLevel])->addListener(this);
    tree.getParameter(DTParameterID[kParam_Feedback])->addListener(this);
    tree.getParameter(DTParameterID[kParam_GravetyExpo])->addListener(this);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayTrollAudioProcessor();
}
AudioProcessorValueTreeState::ParameterLayout DelayTrollAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    for(int i = 0 ; i < kParam_TotalNumFloatParam; i++){
        params.push_back(std::make_unique<AudioParameterFloat>(DTParameterIDNew[i],
                                                               DTParameterName[i],
                                                               DTParameterNormalRanges[i],
                                                               DTParameterDefaultValue[i]));
    }
    
    
    for(int i = kParam_TotalNumFloatParam; i<kBParam_TotalNumBoolParam; i++){
        params.push_back(std::make_unique<AudioParameterBool>(DTParameterIDNew[i],
                                                              DTParameterName[i],
                                                              DTBoolParameterDefaltValue[i-kParam_TotalNumFloatParam]));
    }

    
    //the return type has a template constructor for vector itterators
    return {params.begin(), params.end()};
}


void DelayTrollAudioProcessor::hiResTimerCallback()
{
    
   // mTimerLock.test_and_set(std::memory_order_acquire); // acquire the lock
    
    //Logger::writeToLog("mShouldRunTimer at TimerCallback Start: " + String((int)mShouldRunTimer.load(std::memory_order_acquire)));
    
    //if(!mShouldRunTimer.load(std::memory_order_acquire)){
       // stopTimer();
       // mTimerLock.clear(std::memory_order_release); // release the lock
       // return;
   // }
    
    mIsTimerRunning.store(true, std::memory_order_release);
    
    //DBG("Timer=========");

    if(mDriftFlag){
        mScheduledTime = mCurrentTime + DTModulationUpdateInTicks;
        //DBG("Scheduled Time: "<< mScheduledTime);
        mNextTimerInterval = static_cast<int>((mScheduledTime - mCurrentTime) / (Time::getHighResolutionTicksPerSecond()/1000));
        //DBG("Next timer interval: "<<nextTimerInterval);
        mDriftFlag = false;
    }
    else{
        mCurrentTime = Time::getHighResolutionTicks();
        //DBG("CurrentTime: "<< mCurrentTime);
        mDrift = mCurrentTime - mScheduledTime;
        //DBG("Drift: "<< mDrift);
        mScheduledTime = mCurrentTime + DTModulationUpdateInTicks - mDrift;
        //DBG("Scheduled Time: "<< mScheduledTime);
        mNextTimerInterval = static_cast<int>((mScheduledTime - mCurrentTime) / (Time::getHighResolutionTicksPerSecond()/1000));
        //DBG("Next timer interval: "<<nextTimerInterval);
    }
    
    //incase the DAW doesnt provide a numerator
    int timeSignatureNumerator = mPositionInfo->getTimeSignature().hasValue() ? mPositionInfo->getTimeSignature()->numerator:4;
    //DBG("Timer running");
    
    if((mPositionInfo->getBpm().orFallback(120) != static_cast<double>(mBPM.getValue())) ||
       (timeSignatureNumerator != static_cast<int>(mTimeSigNum.getValue()))){
        
        if(mIsPlayheadInitalized && mPositionInfo->getBpm().orFallback(120) >= 5.0){
            mBPM.setValue(var(mPositionInfo->getBpm().orFallback(120)));
            
            //mBPM.getValueSource().sendChangeMessage(false);
            //DBG("BPM Updating From timer to: "<<static_cast<float>(mBPM.getValue()));
        }
        
        mTimeSigNum.setValue(var(timeSignatureNumerator));
        
//        if(mPositionInfo.timeInSamples < mPlayheadTimeInSamples && !mPositionInfo.isLooping){
//            DBG("position reset");
//            //clear buffers
//            //reset timers
//
//        }
        mPlayheadTimeInSamples = mPositionInfo->getTimeInSamples().orFallback(0);

        if(getSyncStatus()){
            
            mMinTimeBPMManager.updatePositionInfo();
            mMaxTimeBPMManager.updatePositionInfo();
            
//            mAssignableParams[kAss_MinTime]->setValue(static_cast<float>(mMinTimeQuantRef.getValue()));
//            mAssignableParams[kAss_MaxTime]->setValue(static_cast<float>(mMaxTimeQuantRef.getValue()));
            
            DTWaitForUpdate(mUpdatePaused, mIsUpdated);
            
            mAtomicHord.setHordNoteFractionRange(mMinTimeBPMManager.getNoteFractionInt(), mMaxTimeBPMManager.getNoteFractionInt());
            
            tree.getParameterAsValue(DTParameterID[kParam_MinDelayTime]).setValue(mMinTimeQuantRef.getValue());
            tree.getParameterAsValue(DTParameterID[kParam_MaxDelayTime]).setValue(mMaxTimeQuantRef.getValue());
            //tree.getParameterAsValue(DTParameterID[kParam_MinDelayTime]).setValue(mMinTimeQuantRef.getValue());
            
            mAtomicTimeProcessor.updateRange(static_cast<double>(mMinTimeQuantRef.getValue()), static_cast<double>(mMaxTimeQuantRef.getValue()));

        }//if synced
        
        for(auto manager: mModManagers)
        {
            if(manager->getBPMSyncStatus() && manager->getNumOfValues()){
                manager->getBPMManager().updatePositionInfo();
                manager->updateStepSizeByNoteFraction();
                //if(mIsPlayheadInitalized)
                    //DBG("New ms per cycle: "<< manager->getStepSizeAsMsPerCycle());
            }
        }
    }//if BMP orTime Sig Changes

   
    
    updateModulations();
    
    for(auto m : mModManagers){
        m->advancePlotPosition();
    }
    
    if(hasEditor() && !mIsNonRealTime)
        triggerAsyncUpdate();
    
    //Logger::writeToLog("mNextTimerInterval: " + String(mNextTimerInterval));
    //Logger::writeToLog("mIsNonRealTime: " + String((int)mIsNonRealTime));
    //Logger::writeToLog("mShouldRunTimer at TimerCallback End: " + String((int)mShouldRunTimer.load(std::memory_order_acquire)));
   
    if(!mIsNonRealTime)
        startTimer(mNextTimerInterval);
 
    mIsTimerRunning.store(false, std::memory_order_release);
    //mTimerLock.clear(std::memory_order_release); // release the lock
}

void DelayTrollAudioProcessor::loadActivate()
{
    m_listeners.call([this](Listener& l) { l.loadActivated(this); });
}
void DelayTrollAudioProcessor::BPMUpdated()
{
    mBPMListeners.call([this](BPMListener& l) { l.BPMUpdated(this); });
}

float DelayTrollAudioProcessor::getInputGainMeterLevel(int inChannel)
{
    float inValuedB;
    if(inChannel == 0)
    {
        inValuedB = Decibels::gainToDecibels(mLeftInputLevel + 0.00001f, -48.f);
    }
    else{
        inValuedB = Decibels::gainToDecibels(mRightInputLevel + 0.00001f, -48.f);
    }
    return jmax((inValuedB + 48.0f) / 48.0f,0.f);
}
float DelayTrollAudioProcessor::getOutputGainMeterLevel(int inChannel)
{
    float inValuedB;
    if(inChannel == 0)
    {
        inValuedB = Decibels::gainToDecibels(mLeftOutputLevel + 0.00001f, -48.f);
    }
    else{
        inValuedB = Decibels::gainToDecibels(mRightOutputLevel + 0.00001f, -48.f);
    }
    return jmax((inValuedB + 48.0f) / 48.0f, 0.f);
}

void DelayTrollAudioProcessor::initializePlayhead(){
    
    /**AUDIO PROCESS THREAD STUFF ONLY PLEASE**/
    
    if(!mPositionInfo->getIsPlaying()){
        DBG("initalizing while paused");
    }
    
    if(mPositionInfo->getBpm().orFallback(120) >= 5.0 && static_cast<double>(mBPM.getValue()) != mPositionInfo->getBpm().orFallback(120)){
        mBPM.setValue(var(mPositionInfo->getBpm().orFallback(120)));
        DBG("Updating BPM from Initialization to: "<<static_cast<float>(mBPM.getValue()));
        
        mMinTimeBPMManager.updatePositionInfo();
        mMaxTimeBPMManager.updatePositionInfo();
        
        for(auto plot: mModManagers){
            if(plot->getBPMSyncStatus() && plot->getNumOfValues()){
                DBG("Plot: "<<plot->getManagerID());
                plot->getBPMManager().updatePositionInfo();
                plot->updateStepSizeByNoteFraction();
                DBG("New ms Per Cycle: "<<plot->getStepSizeAsMsPerCycle());
            }
        }
    }//updating BPM
    
    //DBG("BPM Initalized: "<<mPositionInfo->getBpm().orFallback(120));
    
    if(getSyncStatus()){
        float tempMinTime, tempMaxTime;
        
        mMinTimeBPMManager.updatePositionInfo();
        mMaxTimeBPMManager.updatePositionInfo();
        
        tempMinTime = static_cast<float>(mMinTimeQuantRef.getValue());
        
        tempMinTime = jmax(0.f, tempMinTime);
        
        //WE WILL HAVE TO SEE ABOUT THIS=============================================================================
        //getParameters()[kParam_MinDelayTime]->setValue(tempMinTime);
        tree.getParameterAsValue(DTParameterID[kParam_MinDelayTime]).setValue(tempMinTime);
        
        mAssignableParams[kAss_MinTime]->setValue(tempMinTime);
        
        //DelayTroll::m_hordMinTime = tempMinTime;
        
        tempMaxTime = static_cast<float>(mMaxTimeQuantRef.getValue());
        
        tempMaxTime = jmin(tempMaxTime, DTParameterNormalRanges[kParam_MaxDelayTime].getRange().getEnd());
        
//        DBG("WITH MAX TIME: "<< tempMaxTime);
//        DBG("Value : "<< static_cast<double>(tree.getParameterAsValue(DTParameterID[kParam_MaxDelayTime]).getValue()));
//        DBG("RAW VALUE : "<< tree.getRawParameterValue(DTParameterID[kParam_MaxDelayTime])->load());
//        DBG("Param Value : "<< tree.getParameter(DTParameterID[kParam_MaxDelayTime])->getValue());
//        DBG("GET PARAM : "<<getParameters()[kParam_MaxDelayTime]->getValue());
        //should be fine as this is a process thread change
        //getParameters()[kParam_MaxDelayTime]->setValue(tempMaxTime);
        tree.getParameterAsValue(DTParameterID[kParam_MaxDelayTime]).setValue(tempMaxTime);
        
        mAssignableParams[kAss_MaxTime]->setValue(tempMaxTime);
        
        //DelayTroll::m_hordMaxTime = tempMaxTime;
        mAtomicHord.setHordNoteFractionRange(mMinTimeBPMManager.getNoteFractionInt(), mMaxTimeBPMManager.getNoteFractionInt());
        mAtomicTimeProcessor.updateRange(static_cast<double>(mMinTimeQuantRef.getValue()), static_cast<double>(mMaxTimeQuantRef.getValue()));
        
        updateModulations();
        
        //mTimeProcessor.processModulated();
        //loadActivate();
    }
    mIsPlayheadInitalized = true;
}

void DelayTrollAudioProcessor::findPosition(){
    
    DBG("Finding Position");
    
    double timeInMs = mPositionInfo->getTimeInSeconds().orFallback(0.0) * 1000.0;
    //DBG("Resetting ModulationPosition With time of: "<<timeInMs);
    
    for(auto& mod: mModManagers){
        
        double notFractionsSinceEditTime = timeInMs/mod->getBPMManager().getNoteFractionsInMS();
       
        if(mod->getBPMSyncStatus() && mod->getNumOfValues())
            mod->setPositionFromNotefractions(notFractionsSinceEditTime);
        else if(mod->getNumOfValues())
            mod->setPosition(timeInMs);
    }
}

bool DelayTrollAudioProcessor::getSyncStatus(){
    if(getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_BPMSync ||
       getNonModParamAsInt(kNMod_TimeControlMode)  == kTimeOp_QuantizeDelays ){
        return true;
    }
    else{
        return false;
    }
    
}

bool DelayTrollAudioProcessor::getModulationStatus(int _paramIndex)
{
    return mAssignableParams[_paramIndex]->isAttached;
}

void DelayTrollAudioProcessor::updateModulations(){
    bool rangeNeedsUpdate = false;
    bool gravetyNeedsUpdate = false;
    float offset = 0.f;
    float newValue = 0.f;
    float limitValue = 0.f;
    float polarValue = 0.5f;
    for(auto param : mAssignableParams)
    {
        if(param->isAttached)
        {
            //DBG("Modulating: "<<DTAssignableParamsName[param->typeIndex]);
            offset = 0.f;
            for(auto modManager : param->modulationManagers)
            {
                polarValue = static_cast<float>(modManager->getPolarValue());
                if(param->typeIndex == kAss_Gravity || param->typeIndex == kAss_SmoothFactor){
                    //offset -= (PlotPossition - plolarValue) * Depth
                    offset -= (mDTmodArray[modManager->getManagerID()][modManager->getPlotPossition()] - polarValue) * static_cast<float>(modManager->getDepthValue(param->typeIndex));
                }
                else{
                    //offset += (PlotPossition - plolarValue) * Depth
                    offset += (mDTmodArray[modManager->getManagerID()][modManager->getPlotPossition()] - polarValue) * static_cast<float>(modManager->getDepthValue(param->typeIndex));
                }
            }
            
            param->ofset = offset;
            
            newValue = param->sharedOrigninalValue.get();
            
            newValue = (newValue - DTAssignParamRanges[param->typeIndex].getRange().getStart()) / DTAssignParamRanges[param->typeIndex].getRange().getLength();
            newValue = newValue + offset;
            newValue = jmax(0.f, newValue);
            newValue = jmin(1.f, newValue);
            newValue = DTAssignParamRanges[param->typeIndex].getRange().getStart() + newValue * DTAssignParamRanges[param->typeIndex].getRange().getLength();
            if(param->typeIndex == kAss_MaxTime)
            {

                limitValue = mAssignableParams[kAss_MinTime]->getNextValue();
                newValue = jmax(newValue,  limitValue + static_cast<float>(mAtomicHord.getAtomicHordSize().load(std::memory_order_acquire)));
                if(newValue > DTAssignParamRanges[param->typeIndex].getRange().getEnd()){
                    newValue = DTAssignParamRanges[param->typeIndex].getRange().getEnd();
                }
                rangeNeedsUpdate = true;
            }
            else if (param->typeIndex == kAss_MinTime)
            {
//                if(mAssignableParams[kAss_MaxTime]->isAttached)
//                    limitValue = static_cast<float>(mAssignableParams[kAss_MaxTime]->getNextValue());
//                else
//                    limitValue = mAssignableParams[kAss_MaxTime]->getNextValue();
                limitValue = mAssignableParams[kAss_MaxTime]->getNextValue();
                
                newValue = jmin(newValue,
                                limitValue - static_cast<float>(mAtomicHord.getAtomicHordSize().load(std::memory_order_acquire)));
                if(newValue < DTAssignParamRanges[param->typeIndex].getRange().getStart()){
                    newValue = DTAssignParamRanges[param->typeIndex].getRange().getStart();
                }
                rangeNeedsUpdate = true;
                //watch out for negative numbers
            }
            else if (param->typeIndex == kAss_Gravity)
            {
                gravetyNeedsUpdate = true;
            }
            param->modulatedValue = newValue;
            param->setValue(newValue);
        }//is attatched
    }//param loop

    int selection = getNonModParamAsInt(kNMod_TimeControlMode);
    if(rangeNeedsUpdate && selection != kTimeOp_QuantizeDelays){
        DTWaitForUpdate(mUpdatePaused, mIsUpdated);
        mAtomicTimeProcessor.modulateRange();
    }
    
    if (gravetyNeedsUpdate &&(selection == kTimeOp_Gravity || selection == kTimeOp_BPMSync)){
        DTWaitForUpdate(mUpdatePaused, mIsUpdated);
        mAtomicTimeProcessor.updateMult();
    }
}
 void DelayTrollAudioProcessor::handleAsyncUpdate()
{
     if(mIsTimerRunning.load(std::memory_order_acquire)){
         //DBG("Timer is Running Try Async later");
         triggerAsyncUpdate();
         return;
     }
     
     
     if(mIsRangeUpdateNeeded.load(std::memory_order_acquire)){
        AudioParameterFloat* minParam = dynamic_cast<AudioParameterFloat*>(tree.getParameter(DTParameterID[kParam_MinDelayTime]));
        AudioParameterFloat* maxParam = dynamic_cast<AudioParameterFloat*>(tree.getParameter(DTParameterID[kParam_MaxDelayTime]));
        std::atomic<float>* minPtr = tree.getRawParameterValue(DTParameterID[kParam_MinDelayTime]);
        std::atomic<float>* maxPtr = tree.getRawParameterValue(DTParameterID[kParam_MaxDelayTime]);
        double mIntervalValue = mMinTimeBPMManager.getNoteFractionsInMS();
        double minval;
        double maxval;
        //DBG("Handling Async with values of");
        //DBG("maxParam: "<<maxParam->get());
        //DBG("MaxPtr: "<<*maxPtr);
        if(getSyncStatus()){
            //snap values to correct ones
            //*****SOMETHING IS STRANGE HERE minval and maxval are snaped but the snapping is NEVER USED WHAT WAS I THINKNG???**********
            
            minval = snapDelTimes(minParam, mIntervalValue);
            maxval = snapDelTimes(maxParam, mIntervalValue);
            minval = minParam->get() >= maxParam->get() ? maxParam->get() - mIntervalValue : minParam->get();
            minval = jmax(0.0, minval);
            maxval = maxParam->get() <= minParam->get() ? minParam->get() + mIntervalValue : maxParam->get();
            jmin(maxval, static_cast<double>(DTParameterNormalRanges[kParam_MaxDelayTime].end));
            
            minval = minval + mIntervalValue >= maxval ? minval - mIntervalValue: minval;
            
            //AddProtection for quants here?
            
            int newnotfraction = roundToInt(static_cast<float>(minval/mIntervalValue));
            mMinTimeBPMManager.setNoteFractions(static_cast<float>(newnotfraction));
            newnotfraction = roundToInt(static_cast<float>(maxval/mIntervalValue));
            mMaxTimeBPMManager.setNoteFractions(static_cast<float>(newnotfraction));
            *minPtr = static_cast<float>(minval);
            *maxPtr = static_cast<float>(maxval);
            
            mAtomicHord.setHordNoteFractionRange(mMinTimeBPMManager.getNoteFractionInt(), mMaxTimeBPMManager.getNoteFractionInt());
        }//end sync true
        else{
            minval = minParam->get() >= maxParam->get() ? maxParam->get() - mAtomicHord.getAtomicHordSize().load(std::memory_order_acquire) : minParam->get();
            minval = jmax(0.0, minval);
            maxval = maxParam->get() <= minParam->get() ? minParam->get() + mAtomicHord.getAtomicHordSize().load(std::memory_order_acquire) : maxParam->get();
            jmin(maxval, static_cast<double>(DTParameterNormalRanges[kParam_MaxDelayTime].end));
            //DBG("MaxVal: "<<maxval);
            *minPtr = static_cast<float>(minval);
            *maxPtr = static_cast<float>(maxval);
        }//end sync false
        //if either min or max are modulating let the modulator update the range otherwise call it from here
        if(!mAssignableParams[kAss_MinTime]->isAttached && !mAssignableParams[kAss_MaxTime]->isAttached){
            DTWaitForUpdate(mUpdatePaused, mIsUpdated);
            mAtomicTimeProcessor.updateRange(static_cast<double>(*minPtr),
                                             static_cast<double>(*maxPtr));
            if(getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_QuantizeDelays){
                mAtomicTimeProcessor.hordSizeUpdate(mAtomicTimeProcessor.getMults().size());
            }
        }
        mIsRangeUpdateNeeded.store(false, std::memory_order_release);
    }//Range update
    else if (mIsMultUpdateNeeded.load(std::memory_order_acquire)){
        if(!mAssignableParams[kAss_Gravity]->isAttached){
            mAssignableParams[kAss_Gravity]->setValue(*tree.getRawParameterValue(DTParameterID[kParam_GravetyExpo]));
            DTWaitForUpdate(mUpdatePaused, mIsUpdated);
            mAtomicTimeProcessor.updateMult();
        }
        mIsMultUpdateNeeded.store(false, std::memory_order_release);
    }//mult update
    else if (mIsTargetUpdateNeeded.load(std::memory_order_acquire)){
        
        if(!mAssignableParams[kAss_Feedback]->isAttached){
            mAssignableParams[kAss_Feedback]->setValue(*tree.getRawParameterValue(DTParameterID[kParam_Feedback]));
        }
        
        if(!mAssignableParams[kAss_OutputLevel]->isAttached){
            mAssignableParams[kAss_OutputLevel]->setValue(*tree.getRawParameterValue(DTParameterID[kParam_OutputLevel]));
        }
        
        if(!mAssignableParams[KAss_InputLevel]->isAttached){
            mAssignableParams[KAss_InputLevel]->setValue(*tree.getRawParameterValue(DTParameterID[kParam_InputLevel]));
        }
        
        if(!mAssignableParams[kAss_WetDry]->isAttached){
            mAssignableParams[kAss_WetDry]->setValue(*tree.getRawParameterValue(DTParameterID[kParam_WetDry]));
        }
        
        if(!mAssignableParams[kAss_SmoothFactor]->isAttached){
            mAssignableParams[kAss_SmoothFactor]->setValue(*tree.getRawParameterValue(DTParameterID[kParam_SmoothFactor]));
        }
        
        
        mIsTargetUpdateNeeded.store(false, std::memory_order_release);
        
    }//target update
     
     if(hasEditor())
         mAtomicTimeProcessor.AtomicActivate();
    
}

void DelayTrollAudioProcessor::parameterValueChanged (int parameterIndex, float newValue){
    //DBG("Change ConsumptionSize: "<< mChangeConsumption.load());
    if(mChangeConsumption.load() != 0){
        mChangeConsumption--;
    }
    else if(mIsGUIMoving.load(std::memory_order_seq_cst)){
        return;
    }
    else if(parameterIndex == kParam_MinDelayTime || parameterIndex == kParam_MaxDelayTime)
    {
        //DBG("TriggeringAsync");
        mIsRangeUpdateNeeded.store(true, std::memory_order_release);
        triggerAsyncUpdate();
    }
    else if(parameterIndex == kParam_GravetyExpo){
        mIsMultUpdateNeeded.store(true, std::memory_order_release);
        triggerAsyncUpdate();
    }
    else if(parameterIndex == kParam_SmoothFactor   ||
            parameterIndex == kParam_WetDry         ||
            parameterIndex == kParam_Feedback       ||
            parameterIndex == kParam_InputLevel     ||
            parameterIndex == kParam_OutputLevel){
        mIsTargetUpdateNeeded.store(true, std::memory_order_release);
        triggerAsyncUpdate();
    }
}

double DelayTrollAudioProcessor::snapDelTimes(AudioParameterFloat* _timeParam, float _intervalValue){
    
    float rTime = remainder(_timeParam->get(), _intervalValue);
    
    if(rTime == 0.0)
        return static_cast<double>(_timeParam->get());
    
    if(rTime > _intervalValue/2.f)
        return static_cast<double>(_timeParam->get() + _intervalValue - rTime);
    else
        return static_cast<double>(_timeParam->get() - rTime);
}

float DelayTrollAudioProcessor::findMinRange(){
    return (static_cast<float>(getNonModParams()[kNMod_NumbOfDels]->getValue())-1.f) * static_cast<float>(mMinTimeBPMManager.getNoteFractionsInMS());
}
