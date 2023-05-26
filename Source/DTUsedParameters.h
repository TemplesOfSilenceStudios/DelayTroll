/*
  ==============================================================================

    DTUsedParameters.h
    Created: 18 Apr 2019 11:55:16am
    Author:  Sam Ferguson

 These are the parameters the user will be able to automate from the host
 
  ==============================================================================
*/

#pragma once

static const int DTTotalNumberOfTrolls{64};


enum DTParameter
{
    
    kParam_MinDelayTime = 0,
    kParam_MaxDelayTime,
    kParam_SmoothFactor,
    kParam_GravetyExpo,
    kParam_WetDry,
    kParam_Feedback,
    kParam_InputLevel,
    kParam_OutputLevel,
    kParam_TotalNumFloatParam,
    
};


enum DTBoolParameters
{
    kBParam_EQBypass = kParam_TotalNumFloatParam,
    kBParam_PitchBypass,
    kBParam_SoftClipperBypass,
    kBParam_TotalNumBoolParam,
};

enum DTNonModulatableParameters
{
    kNMod_QuantizationSelection = 0,
    kNMod_TimeControlMode,
    kNMod_NumbOfDels,
    kNMod_LevelManuelMode,
    kNMod_TimeManuelMode,
    kNMod_EQManuelMode,
    kNMod_PitchManuelMode,
    kNMod_PanManuelMode,
    kNMod_TotalNumNModParam,
};

static var DTNonModulatableDefaultValues[kNMod_TotalNumNModParam]
{
    var(4),         //QuantizationSelection
    var(1),         //TimeControlMode
    var(32),        //NumberOfDels
    var(2),         //TimeManuelMode
    var(2),         //LevelManuelMode
    var(2),         //EQManuelMode
    var(2),         //PitchManuelMode
    var(4),         //PanManuelMode
};

static String DTNonModParamNames[kNMod_TotalNumNModParam]
{
    "Quantization",            //QuantizationSelection
    "Mode",                    //TimeControlMode
    "Number Of Delays",        //number of delays
    "Manual Mode",             //TimeManuelMode
    "Manual Mode",             //LevelManuelMode
    "Manual Mode",             //EQManuelMode
    "Manual Mode",             //PitchManuelMode
    "Manual Mode",             //PanManuelMode
};

static String DTNonModParamIDs[kNMod_TotalNumNModParam]
{
    "QuantizationID",           //QuantizationSelection
    "TimeControlModeID",        //TimeControlMode
    "NumbOfDelsID",
    "TimeManuelModeID",         //TimeManuelMode
    "LevelManuelModeID",        //LevelManuelMode
    "EQManuelModeID",           //EQManuelMode
    "PitchManuelModeID",        //PitchManuelMode
    "PanManuelModeID",          //PanManuelMode
};



static ParameterID DTParameterIDNew[kBParam_TotalNumBoolParam] =
{
    ParameterID("MinDelayTime",1),
    ParameterID("MaxDelayTime",1),
    ParameterID("SmoothFactor",1),
    ParameterID("GravityExpo",1),
    ParameterID("WetDry",1),
    ParameterID("Feedback",1),
    ParameterID("InputLevel",1),
    ParameterID("OutputLevel",1),
    ParameterID("EQBypass",1),
    ParameterID("PitchBypass",1),
    ParameterID("SoftClipperBypass", 1),
    
};

static String DTParameterID[kBParam_TotalNumBoolParam] =
{
    "MinDelayTime",
    "MaxDelayTime",
    "SmoothFactor",
    "GravityExpo",
    "WetDry",
    "Feedback",
    "InputLevel",
    "OutputLevel",
    "EQBypass",
    "PitchBypass",
    "SoftClipperBypass",
};

static const String DTParameterName[kBParam_TotalNumBoolParam] =
{
    "Min Delay Time",
    "Max Delay Time",
    "Smooth Factor",
    "Gravity Expo",
    "Wet/Dry",
    "Feedback",
    "Input Level",
    "Output Level",
    "Bypass EQ",
    "Bypass Pitch",
    "Bypass Soft Clipper",
};



static StringArray DTTimeControlLabels("Gravity",
                                       "Manual Control",
                                       "Divisions",
                                       "BPMSync",
                                       "Quantized Delays");

static StringArray DTChoiceParameterStrings[5]{
    StringArray(                                                //kIParam_QuantizationSelection
                "1/1",
                "1/2",
                "1/3",
                "1/4",
                "1/5",
                "1/6",
                "1/8",
                "1/9",
                "1/16",
                "1/32",
                "1/64",
                "1/128"),
    StringArray(                                                //kIParam_TimeControlMode
                "Gravity",
                "Manual Control",
                "Divisions",
                "BPMSync",
                "Quantized Delays"),
    StringArray(                                                //Level and EQ Manuel Mode
                "Manual",
                "Bez"),
    StringArray(                                                //PitchManuelMode
                "Manual",
                "Bez",
                "Quantized"),
    StringArray(                                                //PanManuelMode
                "Manual",
                "Bez",
                "Manual Balance",
                "Bez Balance")
    
};

enum DTTimeControlOptions
{
  kTimeOp_Gravity = 1,
    kTimeOp_ManualControl,
    kTimeOp_Divisitions,
    kTimeOp_BPMSync,
    kTimeOp_QuantizeDelays,
    kTimeOp_TotalNumberofOptions,
};

enum DTDefaultControlOptions{
    kDCOp_Manual = 1,
    kDCOp_Bez,
};
enum DTPitchControlOptions{
    kPitchOp_Manual = 1,
    kPitchOp_Bez,
    kPitchOp_Quant,
};

enum DTPanControlOptions{
    kPanOp_Manual = 1,
    kPanOp_Bez,
    kPanOp_ManBal,
    kPanOP_BezBal,
};


enum DTBPMQuantization
{
    kQuant_1Beat = 0,
    kQuant_Half,
    kQuant_3rd,
    kQuant_4th,
    kQuant_5th,
    kQuant_6th,
    kQuant_8th,
    kQuant_9th,
    kQuant_16th,
    kQuant_32th,
    kQuant_64th,
    kQuant_128th,
    kQuant_TotalNumQuants,
};

static StringArray DTBPMQuantizationOptions("1/1",
                                            "1/2",
                                            "1/3",
                                            "1/4",
                                            "1/5",
                                            "1/6",
                                            "1/8",
                                            "1/9",
                                            "1/16",
                                            "1/32",
                                            "1/64",
                                            "1/128");

static int DTBPMQuantizationDenominators[kQuant_TotalNumQuants] =
{
1,      //    "1/1",
2,      //    "1/2",
3,      //    "1/3",
4,      //    "1/4",
5,      //    "1/5",
6,      //    "1/6",
8,      //    "1/8",
9,      //    "1/9",
16,     //    "1/16",
32,     //    "1/32",
64,     //    "1/64"
128,     //    "1/128"
};

static float DTBPMQuantizationMults[kQuant_TotalNumQuants] =
{
    1.f,      //    "1/1",
    2.f,      //    "1/2",
    3.f,      //    "1/3",
    4.f,      //    "1/4",
    5.f,      //    "1/5",
    6.f,      //    "1/6",
    8.f,      //    "1/8",
    9.f,      //    "1/9",
    16.f,     //    "1/16",
    32.f,     //    "1/32",
    64.f,     //    "1/64"
    128.f,    //    "1/128"
};

static float DTParameterDefaultValue[kParam_TotalNumFloatParam] =
{
    0.0f,        //    "Min Delay Time",
    2000.0f,    //     "Max Delay Time",
    0.001f,      //    "Smooth Factor",
    1.4f,        //    "Gravety Expo",
    0.5f,        //    "Wet/Dry",
    0.0f,        //    "Feedback",
    1.0f,        //    "Input Level",
    1.0f,        //    "Output Level",
};

static bool DTBoolParameterDefaltValue[kBParam_TotalNumBoolParam - kParam_TotalNumFloatParam] =
{
    true,   //EQBypass
    true,   //PitchBypass
    true,  //SoftClipperBypass
};

static NormalisableRange<float> DTParameterNormalRanges[kParam_TotalNumFloatParam] =
{
    NormalisableRange<float>(0.0f, 10000.0f, 1, 0.3f),      //    "Min Delay Time",
    NormalisableRange<float>(0.0f, 10000.0f, 1, 0.3f),      //    "Max Delay Time",
    NormalisableRange<float>(0.00001f, 0.002f,              //    "Smooth Factor",
                             [] (auto rangeStart, auto rangeEnd, auto normalised)
                                     { return jmap (normalised, rangeEnd, rangeStart); },
                             [] (auto rangeStart, auto rangeEnd, auto value)
                                     { return jmap (value, rangeEnd, rangeStart, 0.f, 1.f); }),
    NormalisableRange<float>(0.005f, 1.995f,                //    "Gravety Expo",
                             [] (auto rangeStart, auto rangeEnd, auto normalised)
                                     { return jmap (normalised, rangeEnd, rangeStart); },
                             [] (auto rangeStart, auto rangeEnd, auto value)
                                     { return jmap (value, rangeEnd, rangeStart, 0.f, 1.f); }),
    NormalisableRange<float>(0.0f, 1.0f),                   //    "Wet/Dry",
    NormalisableRange<float>(0.0f, 2.0f),                   //    "Feedback",
    NormalisableRange<float>(0.0f, 1.0f),                   //    "Input Level",
    NormalisableRange<float>(0.0f, 1.0f),                   //    "Output Level",
};

