#include "impl/OpenALSoundEnvironment.h"
#include "impl/XmlPugi.h"

namespace hpl
{

cOpenALSoundEnvironment::cOpenALSoundEnvironment ( ) /*:
                                        mfDensity(1.0f),
                                        mfDiffusion(1.0f),
                                        mfGain(0.32f),
                                        mfGainHF(0.89f),
                                        mfGainLF(0.0f),
                                        mfDecayTime(1.49f),
                                        mfDecayHFRatio(0.83f),
                                        mfDecayLFRatio(1.0f),
                                        mfReflectionsGain(0.05f),
                                        mfReflectionsDelay(0.007f),
                                        mfLateReverbGain(1.25f),
                                        mfLateReverbDelay(0.011f),
                                        mfEchoTime(0.25f),
                                        mfEchoDepth(0.0f),
                                        mfModulationTime(0.25f),
                                        mfModulationDepth(0.0f),
                                        mfAirAbsorptionGainHF(0.994f),
                                        mfHFReference(5000.0f),
                                        mfLFReference(250.0f),
                                        mfRoomRolloffFactor(0.0f),
                                        mbDecayHFLimit(true)*/
{
}

cOpenALSoundEnvironment::~cOpenALSoundEnvironment()
{
}

bool cOpenALSoundEnvironment::CreateFromFile(const tString &asFile)
{
    tString strType;
    pugi::xml_document doc;
    if (!Xml::LoadFile(doc, cString::To16Char(asFile)))
    {
        return false;
    }

    pugi::xml_node pMain = doc.child("SoundEnvironment").child("Main");
    if (pMain)
    {
        strType = pMain.attribute("Type").value();
        mstrName = pMain.attribute("Name").value();
    }


    float *pfTemp;

    pugi::xml_node pParams = doc.child("SoundEnvironment").child("Parameters");

    if ( (!pParams) || (strType.compare("OpenAL") != 0) )
    {
        return false;
    }

    mfDensity = cString::ToFloat(Xml::Attr(pParams, "Density"), 0);
    mfDiffusion = cString::ToFloat(Xml::Attr(pParams, "Diffusion"), 0);
    mfGain = cString::ToFloat(Xml::Attr(pParams, "Gain"), 0);
    mfGainHF = cString::ToFloat(Xml::Attr(pParams, "GainHF"), 0);
    mfGainLF = cString::ToFloat(Xml::Attr(pParams, "GainLF"), 0);
    mfDecayTime = cString::ToFloat(Xml::Attr(pParams, "DecayTime"), 0);
    mfDecayHFRatio = cString::ToFloat (Xml::Attr(pParams, "DecayHFRatio"), 0);
    mfDecayLFRatio = cString::ToFloat (Xml::Attr(pParams, "DecayLFRatio"), 0);
    mfReflectionsGain = cString::ToFloat(Xml::Attr(pParams, "ReflectionsGain"), 0);
    mfReflectionsDelay = cString::ToFloat(Xml::Attr(pParams, "ReflectionsDelay"), 0);
    pfTemp = cString::ToVector3f(Xml::Attr(pParams, "ReflectionsPan"), cVector3f(0)).v;
    mfReflectionsPan[0] = pfTemp[0];
    mfReflectionsPan[1] = pfTemp[1];
    mfReflectionsPan[2] = pfTemp[2];
    mfLateReverbGain = cString::ToFloat(Xml::Attr(pParams, "LateReverbGain"), 0);
    mfLateReverbDelay = cString::ToFloat(Xml::Attr(pParams, "LateReverbDelay"), 0);
    pfTemp = cString::ToVector3f(Xml::Attr(pParams, "LateReverbPan"), cVector3f(0)).v;
    mfLateReverbPan[0] = pfTemp[0];
    mfLateReverbPan[1] = pfTemp[1];
    mfLateReverbPan[2] = pfTemp[2];
    mfEchoTime = cString::ToFloat(Xml::Attr(pParams, "EchoTime"), 0);
    mfEchoDepth = cString::ToFloat(Xml::Attr(pParams, "EchoDepth"), 0);
    mfModulationTime = cString::ToFloat(Xml::Attr(pParams, "ModulationTime"), 0);
    mfModulationDepth = cString::ToFloat(Xml::Attr(pParams, "ModulationDepth"), 0);
    mfAirAbsorptionGainHF = cString::ToFloat(Xml::Attr(pParams, "AirAbsorptionGainHF"), 0);
    mfHFReference = cString::ToFloat(Xml::Attr(pParams, "HFReference"), 0);
    mfLFReference = cString::ToFloat(Xml::Attr(pParams, "LFReference"), 0);
    mfRoomRolloffFactor = cString::ToFloat(Xml::Attr(pParams, "RoomRolloffFactor"), 0);
    mbDecayHFLimit = cString::ToInt(Xml::Attr(pParams, "DecayHFLimit"), 0);

    return true;
}

};
