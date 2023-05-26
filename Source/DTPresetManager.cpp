/*
  ==============================================================================

    KAPPresetManager.cpp
    Created: 7 Apr 2019 4:08:39pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTPresetManager.h"
#include "DTUsedParameters.h"


//setting up the file structure stuff
//=====================================================================================================
static const String pluginName = "Delay Troll";
static const String manuName = "Temples Of Silence Studios";

#if JUCE_WINDOWS
    static const String directorySeperator = "\\";
    static const String factoryPresetDirectory = File::getSpecialLocation(
                                                                          File::SpecialLocationType::commonApplicationDataDirectory).getChildFile(manuName).getChildFile(pluginName).getFullPathName();
const String DTPresetManager::mUserPresetDirectory{
    File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getChildFile(manuName).getChildFile(pluginName).getFullPathName()};
#elif JUCE_MAC
    static const String directorySeperator = "/";
    static const String factoryPresetDirectory="/Library/Audio/Presets/Temples Of Silence Studios/Delay Troll";
const String DTPresetManager::mUserPresetDirectory{
    File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getChildFile("Audio").getChildFile("Presets").getChildFile(manuName).getChildFile(pluginName).getFullPathName()};
#endif
//=====================================================================================================

DTPresetManager::DTPresetManager(AudioProcessor* inProcessor)
: mCurrentPresetIsSaved(false),
mCurrentPresetFactory(false),
mCurrentPresetName("untitled"),
mProcessor(inProcessor)
{
    mInitalizedIndexNumber = -1;
    if(!File(mUserPresetDirectory).exists())
    {
        File(mUserPresetDirectory).createDirectory();
    }
    
    storeLocalPresets();
    
}
DTPresetManager::~DTPresetManager()
{
    
}

void DTPresetManager::getXmlForPreset(XmlElement* inElement, OwnedArray<Value>& _nonModValues)
{
    
    auto& parameters = mProcessor->getParameters();
    
    for(int i = 0 ; i < parameters.size(); i++){
        
        AudioProcessorParameterWithID* parameter = static_cast<AudioProcessorParameterWithID*>(parameters.getUnchecked(i));
        
        inElement->setAttribute(parameter->paramID,
                                parameter->getValue());
    }
    
    for(int i = 0; i<_nonModValues.size(); i++)
    {
        inElement->setAttribute(Identifier(DTNonModParamIDs[i]), static_cast<int>(_nonModValues[i]->getValue()));
    }
    
}
void DTPresetManager::loadPresetForXml(XmlElement* inElement, OwnedArray<Value>& _nonModValues)
{
    mCurrentPresetXml = inElement;
    
     DBG("begin loading at manager");
    
    auto& parameters = mProcessor->getParameters();
    
    for(int i = 0; i<_nonModValues.size(); i++)
    {
        const String ValueID = DTNonModParamIDs[i];
        _nonModValues[i]->setValue(var(mCurrentPresetXml->getIntAttribute(StringRef(ValueID))));
        //_nonModValues[i]->getValueSource().sendChangeMessage(true);
    }

    for (auto* param : parameters) {
        
        const String paramID = static_cast<AudioProcessorParameterWithID*>(param)->paramID;
        
        float value = mCurrentPresetXml->getDoubleAttribute(paramID);
        
        
        param->beginChangeGesture();
        param->setValueNotifyingHost(value);
        param->endChangeGesture();
        //param->setValue(value);
        
        //param->sendValueChangedMessageToListeners(value);

    }//param loop
    

     DBG("end loading at manager");
}

int DTPresetManager::getNumberOfPresets(bool _includeUserPresets)
{
   //scan storage to get how many files
    return _includeUserPresets ? mLocalPresets.size(): mNumberofFactoryPresets;
}

String DTPresetManager::getPresetName(int inPresetIndex)
{
    //get the name of a preset to display in combobox
    return mLocalPresets[inPresetIndex].getFileNameWithoutExtension();
}

void DTPresetManager::createNewPreset()
{
    //set all values back to default and get new preset going
//    for(auto* p : mProcessor->getParameters())
//    {
//        p->setValueNotifyingHost(p->getDefaultValue());
//    }
    
//    auto& parameters = mProcessor->getParameters();
//
//    for(int i = 0 ; i < parameters.size(); i++){
//
//        AudioProcessorParameterWithID* parameter = static_cast<AudioProcessorParameterWithID*>(parameters.getUnchecked(i));
//
//        const float defaultValue = parameter->getDefaultValue();
//
//        parameter->setValueNotifyingHost(defaultValue);
//
//    }
    
    if(mInitalizedIndexNumber != -1 && mInitalizedIndexNumber < mLocalPresets.size()){
        loadPreset(mInitalizedIndexNumber);
    }
    else{
        jassertfalse;
    }
    
    //mCurrentPresetIsSaved = false;
    //mCurrentPresetName = "Untitled";
    
}

void DTPresetManager::savePreset()
{
    //overrite current preset
    
    MemoryBlock destinationData;
    mProcessor->getStateInformation(destinationData);
    
    mCurrentlyLoadedPreset.deleteFile();
    

    
    mCurrentlyLoadedPreset.appendData(destinationData.getData(), destinationData.getSize());
    
    mCurrentPresetIsSaved = true;
}

void DTPresetManager::deletePreset(int _indexToRemove)
{
    bool success = mCurrentlyLoadedPreset.deleteFile();
    
    if(!success)
        jassertfalse;
    
    jassert(_indexToRemove >= mNumberofFactoryPresets);
    
    mLocalPresets.remove(_indexToRemove);
    mCurrentPresetIsSaved = false;
    mCurrentPresetName = "Untitled";
}

void DTPresetManager::saveAsPreset(String inPresetName)
{
    File presetFile = File(mUserPresetDirectory + directorySeperator + inPresetName + PRESET_FILE_EXTENTION);
    
    if(!presetFile.exists()){
        presetFile.create();
    }
    else{
        presetFile.deleteFile();
    }
    
    MemoryBlock destinationData;
    mProcessor->getStateInformation(destinationData);
    
    
    presetFile.appendData(destinationData.getData(),
                          destinationData.getSize());
    
    mCurrentPresetIsSaved = true;
    mCurrentPresetFactory = false;
    mCurrentPresetName = inPresetName;
    mCurrentlyLoadedPreset = presetFile;
    
    storeLocalPresets();
    
    

}

void DTPresetManager::loadPreset(int inPresetIndex)
{
    //load the preset
    //DBG("Loading at preset manager index: "<<inPresetIndex);
    
    //DBG("With name: "<<mLocalPresets[inPresetIndex].getFileName());
  
    mCurrentlyLoadedPreset = mLocalPresets[inPresetIndex];
    
    MemoryBlock presetBinary;
    
    if(mCurrentlyLoadedPreset.loadFileAsData(presetBinary))
    {
        mCurrentPresetIsSaved = true;
        mCurrentPresetFactory = inPresetIndex < mNumberofFactoryPresets? true: false;
        mCurrentPresetName = getPresetName(inPresetIndex);
        mProcessor->setStateInformation(presetBinary.getData(),
                                        static_cast<int>(presetBinary.getSize()));
    }
}

bool DTPresetManager::getIsCurrentPresetSaved()
{
    //check to see if the preset already exists or we need a new preset name
    return mCurrentPresetIsSaved;
}

bool DTPresetManager::getIsCurrentPresetFactory()
{
    return mCurrentPresetFactory;
}

String DTPresetManager::getCurrentPresetName()
{
    //return the name of the current preset
    return mCurrentPresetName;
}

void DTPresetManager::storeLocalPresets(){
    //store presets that are found on disk so we can know what exists
    mLocalPresets.clear();
    //std::unordered_set<char> allowedValues = {'O', 'B', 'S','Z'};
    //char c ='A';
    //mDefaultPresets.clear();

    for (DirectoryEntry entry : RangedDirectoryIterator (File (factoryPresetDirectory),
                                                         false,
                                                         "*"+static_cast<String>(PRESET_FILE_EXTENTION),
                                                         File::TypesOfFileToFind::findFiles))
    {
        File preset = entry.getFile();
        
        String fileName = preset.getFileName();
        
        if(fileName == "O-Initialized.dtf"){
            //DBG("FOUND INITALIZED PRESET WITH INDEX OF: "<<mLocalPresets.size());
            mInitalizedIndexNumber = mLocalPresets.size();
        }
        
        //c = preset.getFileName()[0];
        if(fileName.indexOfAnyOf("OBSZ")==0 && fileName[1]=='-'){
            mLocalPresets.add(preset);
        }
        else{
            DBG("Found an Interloper: "<<preset.getFileName());
        }
    }
    mNumberofFactoryPresets = mLocalPresets.size();
    
    for (DirectoryEntry entry : RangedDirectoryIterator (File (mUserPresetDirectory),
                                                         false,
                                                         "*"+static_cast<String>(PRESET_FILE_EXTENTION),
                                                         File::TypesOfFileToFind::findFiles))
    {
        File preset = entry.getFile();
        mLocalPresets.add(preset);
    }
    
    
}
