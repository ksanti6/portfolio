# Integrating FMOD into a Custom Engine
**Contents**  
[1. Info](#info)  
[2. Goal](#goal)  
[3. Process](#process)  
[4. What I Would Do Differently](#what-i-would-do-differently)  
[5. What I Learned](#what-i-learned)  
[6. Links](#links)  

## Info
**Date:** Spring 2022   
**Class:** GAM 250 (sophmore game project class)  
**Where:** DigiPen Institute of Technology  

This was done in a custom C++ engine.
## Goal
Using FMOD Core API, be able to do these basic things:  
- play looping tracks
- play one shot effects
- pause all audio
- adjust the volume of the audio
## Process
For the initial integration of the API into the engine I relied on TAs in my class to help me ensure that the files were linked up correctly to the project.  
From there I spent a lot of time reading the API's documentation and testing my code to make sure it worked in debug mode, release mode, and in the build.  

I started first with getting it to initialization, update, and shutdown properly. Then I moved onto being able to play a sound effect and then a looping track.  

Once that was done I worked on deserializing the data associated with the system. Instead of hard coding the locations of the audio files I wanted to play, I placed the file paths into a JSON file and read them in after the audio system initialized.  
Since this is a very small game I was able to load in all the audio files at that stage and use them whenever needed.  

After that, I moved onto the remaining features and placing the audio triggers into the gameplay code.
## What I Would Do Differently
I would like to work more with the fade in and fade out of the audio. I think there is a lot of polishing that can be done on this project and that would be where I would start.  
To go along with that I would want to implement spatial audio. While it may not be super noticable in a 2D game like this one, I think it would add a lot to the ambience, which is important in a horror game.
## What I Learned
I learned how to link APIs into a Visual Studio project.  
I learned how to do a basic implementation of FMOD Core API.
## Links
[My Portfolio](https://github.com/ksanti6/portfolio)   
[Sleepless Bayou Project Overview](https://github.com/ksanti6/portfolio/blob/main/GAMES/Sleepless_Bayou/Sleepless_Bayou.md)