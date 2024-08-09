/*!***************************************************************************
\file    audio.h
\author  Kiara Santiago
\par     DP email: email has be redacted for portfolio purposes
\par     Course: GAM 250
\par     Copyright � 2021 - 2022 DigiPen (USA) Corporation.
\date    7-26-2022

*****************************************************************************/
#pragma once

#include "FMOD/inc/fmod.hpp"        //FMOD functions
#include <string>                   //string funcionality

//init, update, shutdown
void AudioInit(void);
void AudioUpdate(void);
void AudioShutdown(void);

//play the sound asked for
void PlaySounds(std::string name, std::string type);

//set the volume of a particular channel
void SetVolume(std::string type, float val);

void StopChannel(std::string type, std::string name);

void MuteChannel(std::string type, bool mute);

bool getIfMuted(void);
