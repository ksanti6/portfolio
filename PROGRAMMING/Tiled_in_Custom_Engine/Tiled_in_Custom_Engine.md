# Using Tiled data in a Custom Engine
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
Use the output data from Tiled to create game objects.
## Process  
I started off by downloading Tiled and getting familiar with the software. I made a basic tilemap and exported variations of it to see how the JSON output file would change.  

From there I created a system to:
- read in the data from the JSON file
- create game objects based on the data
- place said objects on the correct position based on the data  

There was a lot of troubleshooting to be done with converting the position data from the JSON file to in game coordinates. After struggling on it myself for a little bit, I had a team member look at it with me and we got it working in no time.  

Once the system was in place, some final testing was done to ensure it worked in the build as well (not just debug mode).  

This system ended up only being used for displaying the background art of the game.
## What I Would Do Differently
The main thing I would do differently is get rid of all the hard coded "magic numbers". I think if I spent some more time on this feature I would be able to fix the code so there was no reliance on arbitrary numbers. This will allow for the code to scale better and be more easily applicable to other scenarios.
## What I Learned
I learned a lot about interpreting data for use in a game engine. I learned more about how the Tiled software functions.
## Links
[My Portfolio](https://github.com/ksanti6/portfolio)   
[Sleepless Bayou Project Overview](https://github.com/ksanti6/portfolio/blob/main/GAMES/Sleepless_Bayou/Sleepless_Bayou.md)