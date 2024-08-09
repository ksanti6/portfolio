/*!***************************************************************************
\file    Tiled.h
\author  Kiara Santiago
\par     DP email: email has be redacted for portfolio purposes
\par     Course: GAM 250
\par     Copyright © 2021 - 2022 DigiPen (USA) Corporation.
\date    2-5-2022

\brief
  This is where the Tiled implementation lives.

  -ArrayAssert
    private function - json helper function (not in header)

  -FixJsonPath
    private function - json helper function (not in header)

  -ReadTiledJson
    reads in all the data needed from Tiled Json File

  -CreateTiledObjects
    Right now only created the BG tiles

  -TileFactoryBuild
    Tiled vers of the game object factory build

  -GetWidthHeightOfMap
    this gives the w and h of the entire image created by the tiles
*****************************************************************************/
#pragma once
#include <string>        //this is so I can use strings
#include "Vector2D.h"    //this is so i can pass along 2 values at once

typedef class GameObject* GameObjectPtr;

//container for each tileSets
struct TileSetInfo
{
    int FirstGid;               //starting index
    int TileCount;              //number of tiles in this set
    std::string ImageFilePath;  //location of the image tile set
    std::string JsonFilePath;   //location of its json tile set data
};


void ReadTiledJson(void);

void CreateTiledObjects(void);

GameObjectPtr TileFactoryBuild(const std::string name);

Vector2D GetWidthHeightOfMap(void);