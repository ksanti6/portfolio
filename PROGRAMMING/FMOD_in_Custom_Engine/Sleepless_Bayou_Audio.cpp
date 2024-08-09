/*!***************************************************************************
\file    audio.cpp
\author  Kiara Santiago
\par     DP email: email has be redacted for portfolio purposes
\par     Course: GAM 250
\par     Copyright � 2021 - 2022 DigiPen (USA) Corporation.
\date    7-26-2022 

*****************************************************************************/
#include "audio.h"                       //header
#include "FMOD/inc/fmod_errors.h"        //error checking
#include "JSON.h"                        //deserialization
#include <iostream>                      //console printfings
#include <vector>                        //to store variables in
#include <cstdlib>                       //for random help

//to store all the info pertaining to a particular sound
struct soundInfo
{
    std::string _name;                   //to refer it by in json file and in engine
    std::string _path;                   //file path for loading
    FMOD::Sound* _sound;                 //actual sound
    FMOD::Channel* _channel;             //its own channel to live in
    bool firstPlay = true;
};

//////////////////////////////////////////////////////////////////////////////
// global variables - 
FMOD::System* theSystem;                 //core system

std::vector<soundInfo> backgroundSounds; //where all the looping tracks live (and their related info)
std::vector<soundInfo> effectSounds;     //where all the one shot tracks live (and their related info)


FMOD::ChannelGroup* backgroundGroup;     //grouping for the looping tracks
FMOD::ChannelGroup* monsterGroup;        //grouping for the one shot tracks particularly for the monsters
FMOD::ChannelGroup* effectGroup;         //grouping for all other one shot tracks


float backgroundVolume;                  //volume the background track will be
float monsterVolume;                     //volume the monster sounds will be
float effectVolume;                      //volume all other sound effects will be


JSON* AudioReader;                       //used to read in data from json

bool areMuted = false;                   //for muting functionality

//////////////////////////////////////////////////////////////////////////////
//private functions -

//deserialization of audio - reads in from the json
void AudioRead(void);

//loads and unloads the sounds from the read
void LoadAllSounds(void);
void UnloadAllSounds(void);

//load and unload a single sound
FMOD::Sound* LoadSound(const char* name, FMOD_MODE isLooping);
void UnloadSound(FMOD::Sound* theSound);

//if theres an fmod error we will know
void ErrorPrint(FMOD_RESULT errorVal);

//find function
int FindSound(std::string name, std::string type);

//returns random int [min, max)
int GetRandomIntHelper(int min, int max);

//////////////////////////////////////////////////////////////////////////////
//imlementation -

/*!***************************************************************************
\brief
  Initialize the private variables for FMOD and start up the system
*****************************************************************************/
void AudioInit(void)
{
    //the error enum var
    FMOD_RESULT errorResult;

    //create the system
    errorResult = FMOD::System_Create(&theSystem);
    ErrorPrint(errorResult);

    //make sure the sound is stereo
    //errorResult = theSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
    //ErrorPrint(errorResult);

    //init the system
    errorResult = theSystem->init(400, FMOD_INIT_NORMAL, 0);
    ErrorPrint(errorResult);

    //create a channel groups
    errorResult = theSystem->createChannelGroup("backgroundChannelGroup", &backgroundGroup);
    ErrorPrint(errorResult);
    errorResult = theSystem->createChannelGroup("effectChannelGroup", &effectGroup);
    ErrorPrint(errorResult);
    errorResult = theSystem->createChannelGroup("monsterChannelGroup", &monsterGroup);
    ErrorPrint(errorResult);

    //read in from the json file the file names
    //and load then in
    AudioRead();

    //try to set the volumes (does this actually work?)
    backgroundGroup->setVolume(backgroundVolume);
    effectGroup->setVolume(effectVolume);
    monsterGroup->setVolume(monsterVolume);
}

/*!***************************************************************************
\brief
  Update FMOD - it needs to tick once per game update
*****************************************************************************/
void AudioUpdate(void)
{
    //error check var
    FMOD_RESULT updateResult;

    //update
    updateResult = theSystem->update();
    ErrorPrint(updateResult);
}

/*!***************************************************************************
\brief
  shut down the system and release stuff created in the init func
*****************************************************************************/
void AudioShutdown(void)
{
    FMOD_RESULT shutResult;

    //make sure to shut down / release in reverse order

    //get rid of the channel groups
    shutResult = monsterGroup->release();
    ErrorPrint(shutResult);
    shutResult = effectGroup->release();
    ErrorPrint(shutResult);
    shutResult = backgroundGroup->release();
    ErrorPrint(shutResult);

    //shutdown the system
    shutResult = theSystem->release();
    ErrorPrint(shutResult);
}


/*!***************************************************************************
\brief
  Helper function for reading in the arrays

\param d
  json document we are reading from

\param n
  name of the array we are looking for

\return
  the array found from json
*****************************************************************************/
const rapidjson::Value& DoAssertThing(rapidjson::Document d, const char* n)
{
    assert(d[n].IsArray());
    return d[n].GetArray();
}

/*!***************************************************************************
\brief
  Deserialization of audio - reads in from the json
*****************************************************************************/
void AudioRead(void)
{
    //get the json data
    AudioReader = new JSON("./Data/SoundsJS.json");
    assert(AudioReader->getCurrentDocument().IsObject());

    //background noise
    assert(AudioReader->hasMember("Background"));
    const rapidjson::Value& bgs = DoAssertThing(AudioReader->getCurrentDocument(), "Background");
    soundInfo temp;

    //gets all the names of all the background track names to read in
    for (rapidjson::SizeType k = 0; k < bgs.Size(); ++k)
    {
        temp._name = bgs[k].GetString();
        backgroundSounds.push_back(temp);
    }

    //gets the bg track file location names
    for (rapidjson::SizeType k = 0; k < bgs.Size(); ++k)
    {
        char temp[400];
        strncpy_s(temp, sizeof(temp), backgroundSounds[k]._name.c_str(), sizeof(backgroundSounds[k]._name));

        backgroundSounds[k]._path = AudioReader->getString(temp);
    }

    //sound effects
    assert(AudioReader->hasMember("Effect"));
    const rapidjson::Value& efts = DoAssertThing(AudioReader->getCurrentDocument(), "Effect");

    //gets all the names of all the sound effect names to read in
    for (rapidjson::SizeType k = 0; k < efts.Size(); ++k)
    {
        temp._name = efts[k].GetString();
        effectSounds.push_back(temp);
    }

    //gets the sound effect file location names
    for (rapidjson::SizeType k = 0; k < efts.Size(); ++k)
    {
        char temp[400];
        strncpy_s(temp, sizeof(temp), effectSounds[k]._name.c_str(), sizeof(effectSounds[k]._name));

        effectSounds[k]._path = AudioReader->getString(temp);
    }

    //load the sounds
    LoadAllSounds();

    // get volumes
    backgroundVolume = AudioReader->getFloat("BGVolume");
    effectVolume = AudioReader->getFloat("EFTVolume");
    monsterVolume = effectVolume + 0.2f;


}

/*!***************************************************************************
\brief
  Load in all the sounds you got from the json file
*****************************************************************************/
void LoadAllSounds(void)
{
    FMOD::Sound* tempsound;
    //load in the bg tracks
    for (int k = 0; k < backgroundSounds.size(); ++k)
    {
        tempsound = LoadSound(backgroundSounds[k]._path.c_str(), FMOD_LOOP_NORMAL);
        backgroundSounds[k]._sound = tempsound;
    }

    //load in the sound effects
    for (int k = 0; k < effectSounds.size(); ++k)
    {
        tempsound = LoadSound(effectSounds[k]._path.c_str(), FMOD_LOOP_OFF);
        effectSounds[k]._sound = tempsound;
    }
}

/*!***************************************************************************
\brief
  Unload all the sounds we loaded in from the json file
*****************************************************************************/
void UnloadAllSounds(void)
{
    //bg tracks
    for (int k = 0; backgroundSounds.size(); ++k)
    {
        UnloadSound(backgroundSounds[k]._sound);
    }

    //sound effects
    for (int k = 0; effectSounds.size(); ++k)
    {
        UnloadSound(effectSounds[k]._sound);
    }
}

/*!***************************************************************************
\brief
  Print to console if the enum is not OK - means theres an error

\param errorVal
  the FMOD enum to be evaluated
*****************************************************************************/
void ErrorPrint(FMOD_RESULT errorVal)
{
    if (errorVal != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", errorVal, FMOD_ErrorString(errorVal));
    }
}

/*!***************************************************************************
\brief
  Plays the sound that is passed in

\param theSound
  the sound variable to be played
*****************************************************************************/
void PlaySounds(std::string name, std::string type)
{
    //error check var
    FMOD_RESULT playResult = FMOD_OK;
    bool a = false;
    bool* answer = &a;
    FMOD_RESULT isplayRes;

    int s = FindSound(name, type);

    //find the sound
    if (!type.compare("Background"))
    {
        //its a bg sound
        playResult = theSystem->playSound(backgroundSounds[s]._sound, backgroundGroup, false, &backgroundSounds[s]._channel);
    }
    else
    {
        if (!effectSounds[s]._channel || effectSounds[s].firstPlay || !name.compare("FishingStart"))
        {
            goto label2;
        }

        isplayRes = effectSounds[s]._channel->isPlaying(answer);

        if (isplayRes != FMOD_OK)
        {
        label2:
            effectSounds[s].firstPlay = false;

            if (!type.compare("Effect"))
            {
                if (!name.compare("FishingStart"))
                {
                    int r = GetRandomIntHelper(1, 3);

                    switch (r)
                    {
                    case 1:
                        playResult = theSystem->playSound(effectSounds[s]._sound, effectGroup, false, &effectSounds[s]._channel);
                        break;
                    case 2:
                        playResult = theSystem->playSound(effectSounds[s+1]._sound, effectGroup, false, &effectSounds[s+1]._channel);
                        break;
                    }
                }
                else
                {
                    playResult = theSystem->playSound(effectSounds[s]._sound, effectGroup, false, &effectSounds[s]._channel);
                }
                
            }
            else if (!type.compare("Monster"))
            {
                playResult = theSystem->playSound(effectSounds[s]._sound, monsterGroup, false, &effectSounds[s]._channel);
            }
        }
    } 

    ErrorPrint(playResult);
}

/*!***************************************************************************
\brief
  Loads in sound from file and creates a variable for it to live in

\param nameOfSound
  the file name of the sound

\return
  the sound variable created after loading in
*****************************************************************************/
FMOD::Sound* LoadSound(const char* name, FMOD_MODE isLooping)
{
    //error boi
    FMOD_RESULT loadResult;

    //the variable to be returned - where the sound will live
    FMOD::Sound* theSound;

    //do the load
    loadResult = theSystem->createSound(name, isLooping, 0, &theSound);
    ErrorPrint(loadResult);

    return theSound;

}

/*!***************************************************************************
\brief
  Unloads the sound via the variable it lived in - important for avoiding
  mem leaks

\param theSound
  the sound to be unloaded
*****************************************************************************/
void UnloadSound(FMOD::Sound* theSound)
{
    FMOD_RESULT unloadResult;

    unloadResult = theSound->release();

    ErrorPrint(unloadResult);
}


/*!***************************************************************************
\brief
  Change the volume

\param type
  which channel to set the volume of

\param val
  value to set the volume to
*****************************************************************************/
void SetVolume(std::string type, float val)
{
    if (!type.compare("Background"))
    {
        backgroundGroup->setVolume(val);
    }
    else if (!type.compare("Effect"))
    {
        effectGroup->setVolume(val);
        monsterGroup->setVolume(val + 0.2f);
    }
}



void StopChannel(std::string type, std::string name)
{
    //FMOD_RESULT res;

    if (!type.compare("Background"))
    {
        backgroundGroup->stop();
    }
    else if (!type.compare("Effect"))
    {
        if (!name.compare("None"))
        {
            effectGroup->stop();
            monsterGroup->stop();
        }
        else
        {
            int s = FindSound(name, type);
            effectSounds[s]._channel->stop();
        }
        
    }
    else if (!type.compare("Monster"))
    {
        if (!name.compare("None"))
        {
            monsterGroup->stop();
        }
        else
        {
            int s = FindSound(name, type);

            int rate; unsigned long long dspclock;
            theSystem->getSoftwareFormat(&rate, 0, 0);
            effectSounds[s]._channel->getDSPClock(0, &dspclock);

            effectSounds[s]._channel->addFadePoint(dspclock, effectVolume);
            effectSounds[s]._channel->addFadePoint(dspclock + (rate * 8), 0.0f);
            effectSounds[s]._channel->setDelay(0, dspclock + (rate * 8), true);
        } 
    }
    else if (!type.compare("All"))
    {
        backgroundGroup->stop();
        effectGroup->stop();
        monsterGroup->stop();
    }

}


void MuteChannel(std::string type, bool mute)
{
    //FMOD_RESULT res;

    if (!type.compare("Background"))
    {
        backgroundGroup->setMute(mute);
    }
    else if (!type.compare("Effect"))
    {
        effectGroup->setMute(mute);
        monsterGroup->setMute(mute);
    }
    else if (!type.compare("All"))
    {
        areMuted = mute;
        
        backgroundGroup->setMute(mute);
        effectGroup->setMute(mute);
        monsterGroup->setMute(mute);
    }
}

bool getIfMuted(void)
{
    return areMuted;
}


int FindSound(std::string name, std::string type)
{
    int returnValue = -1;

    if (!type.compare("Background"))
    {
        for (int k = 0; k < backgroundSounds.size(); ++k)
        {
            if (!name.compare(backgroundSounds[k]._name))
            {
                returnValue = k;
                goto label;
            }
        }
    }
    else if (!type.compare("Effect") || !type.compare("Monster"))
    {
        for (int k = 0; k < effectSounds.size(); ++k)
        {
            if (!name.compare(effectSounds[k]._name))
            {
                returnValue = k;
                goto label;
            }
        }
    }

label:
    return returnValue;
}

int GetRandomIntHelper(int min, int max)
{
    int temp = rand() % (max - min) + min;

    return temp;
}