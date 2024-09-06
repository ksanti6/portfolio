# Simple 2D (a code sample project)
**Contents**  
[1. Info](#info)  
[2. Goal](#goal)  
[3. Results](#results)   
[4. Process](#process)  
[5. What I Would Add to This Project](#what-i-would-add-to-this-project)  
[6. Links](#links)  
## Info
**Date:** September 2024  

Made in a custom C++ project.  
Made over a long weekend.  

A link to the project's repository is below in the "Links" section.  

## Goal
To make a short project that showcases my current programming skills.  
**Based on a coding sample prompt:**  
"Write a simple game that demonstrates several objects (with a complex relationship) interacting with each other (example: a mouse that explores a 2D maze looking for food, with a cat that tries to chase the mouse, but loses it under some circumstances)"

## Results
A short game in which the mouse (the player) must eat all the cheese in the level to win.  
The mouse loses if they lose all of their lives to the cats (enemies) in the level.  

Some features of this project are:  
- 4 Levels read in from txt files
- Graphics displayed using DirectX12
- Input and Window using GLFW
- Enemy pathing using A Star Algorithm  
- player score and lives output to console when changed  

## Process
### Where to Start?
Reading the prompt, I immediately thought of a Pac-Man style game, so I started to think of what are the most basic features I would need to achieve that.  
I didn't want to over-engineer this project as at the time of creation I had no plans to make a fully fledged game.  

Some of the core features I came up with were:  
- a playable character (the mouse)  
- an object that gives points that the player character would want (the cheese)  
- an enemy that would hinder the player's goal (the cat)  
- visual feedback (graphics!)  

### Graphics
This was my first project doing graphics code, so I wanted to keep it as simple as possible. My goal with graphics was to be able to draw textures and run with that.   
I chose to use **DirectX12** for the graphics API. It seems like a good portion of the industry uses DirectX for their graphics, so it felt like a safe choice to make.  

Something the graphics in this project cannot do in its current state, is write text on screen. When weighing the pros and cons of adding text support VS using the console, I ended up choosing printing to the console.  
I am not primarily a graphics programmer, so I decided to use the time I would've use on that on other areas of programming instead.  

### Enemy Pathing  
For the enemy pathing I decided to use the **A Star Algorithm.** This is a popular algorithm to determine the shortest path between two points on a 2D grid.  

When implementing this algorithm I chose to use the **Octile distance heuristic**. I consider movement in all 8 directions of a block, which means diagonal movement. Diagonal movement costs more (approximately the squareroot of 2), while straight movement costs 1.  

The majority of the time the enemy uses this algorithm to path to the player's location, but there is a random chance that it might start pathing towards a random location in a few block radius of itself instead. 

### Collision
The collision code is where I struggled the most on this project. Writing the detection code was simple enough, but the resolution was what stumped me. With a lot of trial and error, I was able to get a simple resolution working.  


This project has **Box to Box** collision for player VS cheese collision, player VS enemy collision and enemy VS wall collisions.  
Then I did **Circle to Box** collision for the player VS wall collision.  

I chose to do the player VS wall collisions differently because the player has more organic movement since it is controled through input.  
The circle collider on the player allows for better movement around corners and thus a better playing experience.  

## What I Would Add to This Project  
If I were to make this a fully fledged game project, what would I add? Below are a few things I would add to improve this project.  

**Refactor**   
First, before adding any new features, I would do some refactoring to make the project easier to scale. This would entail upgrading the project to use a component based game object system. Streamlining object creation is important if I want to add bigger and better levels (and objects) to the game.  
  
**Better Enemy Behavior**  
I would add in more nuance to the enemy behavior. I think adding in patrolling behavior and aggro radius would be a good place to start for this. The player would have to strategize more to deal with the enemies, and I think it would lead to a more engaging experience.  

**Visual and Audio Feedback**  
Adding in feedback when the player picked up cheese or lost a life to the enemy would be another good addition to this project.  

Positive feedback (like positive ding or cheery vfx) would reinforce the player's desire to continue to pick up more cheese. It would let them know they are doing a good thing.  

Conversely, there would need to be negative feedback to balance that out. Losing a live to the enemy is bad for the player, so that should be abundantly clear through visual and audio cues.  

## Links
[My Portfolio](https://github.com/ksanti6/portfolio)  
[Simple2D Repository](https://github.com/ksanti6/Simple2D)