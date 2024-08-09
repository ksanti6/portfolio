/*!***************************************************************************
\file    Tiled.cpp
\author  Kiara Santiago
\par     DP email: email has be redacted for portfolio purposes
\par     Course: GAM 250
\par     Copyright © 2021 - 2022 DigiPen (USA) Corporation.
\date    2-5-2022

\brief
  This is where the Tiled implementation lives.

  -ArrayAssert
    private function - json helper function

  -FixJsonPath
    private function - json helper function

  -ReadTiledJson
    reads in all the data needed from Tiled Json File

  -CreateTiledObjects
    Right now only created the BG tiles

  -TileFactoryBuild
    Tiled vers of the game object factory build

  -GetWidthHeightOfMap
    this gives the w and h of the entire image created by the tiles
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////INCLUDES///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "Tiled.h"              //header file
#include "JSON.h"               //to read in the data
#include <vector>               //to store the data for each tileset
#include "GameObjectManager.h"  //to store game objects
#include "Transform.h"          //to change game object translations
#include "Sprite.h"             //to change game object frame indices
#include "ScenesManager.h"		//to store gameobjects(scene)'
#include "Scenes.h"

//////////////////////////////////////////////////////////////////////////////
///////////////////////////PRIVATE VARIABLES//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#pragma region Variables

std::vector<TileSetInfo> AllTileInfo;    //will hold the info for each tileset we read in
JSON* TiledDataReader;                   //will be the json object to read in with
std::vector<std::vector<int>> LayerDatas;//this will hold the arrays of data used to stitch the image together
int TiledMapWidth;                       //this is the width aka num of chunks on x axis                          
int TiledMapHeight;                      //this is the height aka num of chunks on y axis

#pragma endregion Variables
//////////////////////////////////////////////////////////////////////////////
///////////////////////////PRIVATE FUNCTIONS//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#pragma region PrivateFunction
/*!***************************************************************************
\brief
  Json helper function - asserts and gets

\param d
  json document we read from

\param n
  name of member we are searching for/getting

\return
  returns array that is gotten (or crashs trying)
*****************************************************************************/
const rapidjson::Value& ArrayAssert(rapidjson::Document d, const char* n)
{
    assert(d[n].IsArray());
    return d[n].GetArray();
}

/*!***************************************************************************
\brief
  appends file path to the start of the given string

\param s
  rest of the file path - given from tiled json

\return
  full file path
*****************************************************************************/
//one to append the correct file path start
std::string FixJsonPath(std::string s)
{
    return "./TiledJsonData/" + s;
}

#pragma endregion PrivateFunction
//////////////////////////////////////////////////////////////////////////////
////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#pragma region PublicFunctions
/*!***************************************************************************
\brief
  Reads all needed data from Tiled Json File
*****************************************************************************/
void ReadTiledJson(void)
{
    //get the json data
    //this creates the json object which holds all the functions in it
    //and the document (the document is private)
    TiledDataReader = new JSON("./TiledJsonData/TiledLevel.json");
    assert(TiledDataReader->getCurrentDocument().IsObject());

    //first we will read in the layers to see how many TileSetInfo objects
    //we will need
    assert(TiledDataReader->hasMember("layers"));
    const rapidjson::Value& AllLayers = ArrayAssert(TiledDataReader->getCurrentDocument(), "layers");

    //for each layer we will extract its data, thats it
    //since the h and w will be the same
    //this is an array of objects btw
    for (rapidjson::SizeType k = 0; k < AllLayers.Size(); ++k)
    {
        //is it an object like we said? does it possess data?
        assert(AllLayers[k].IsObject());
        assert(AllLayers[k].HasMember("data"));

        //cool (hopefully) get that array so we can use it
        assert(AllLayers[k]["data"].IsArray());
        const rapidjson::Value& TempDataArray = AllLayers[k]["data"].GetArray();

        std::vector<int> TempVector;

        //copy over data to slap in the actual vector bc stl is mean and wont
        //let me store arrays directly 
        for (rapidjson::SizeType j = 0; j < TempDataArray.Size(); ++j)
        {
            TempVector.push_back(TempDataArray[j].GetInt());
        }

        //now slap that into the boi (all holy vector god)
        LayerDatas.push_back(TempVector);

    }// end of for loop - reading the layer data

    //okay so now read in the general height and width
    //this is from a layer
    //double check these things below before grab
    assert(AllLayers[0].IsObject());
    assert(AllLayers[0].HasMember("height"));
    assert(AllLayers[0]["height"].IsInt());

    TiledMapHeight = AllLayers[0]["height"].GetInt();

    //repeat for width
    assert(AllLayers[0].IsObject());
    assert(AllLayers[0].HasMember("width"));
    assert(AllLayers[0]["width"].IsInt());

    TiledMapWidth = AllLayers[0]["width"].GetInt();

    //cool now we can forget about the layers and move onto the tilesets
    //this will also be an array of object(s) like the layers
    //lets double check this
    assert(TiledDataReader->getCurrentDocument().IsObject());

    //now try to get the tilesets
    assert(TiledDataReader->hasMember("tilesets"));
    const rapidjson::Value& AllTilesets = ArrayAssert(TiledDataReader->getCurrentDocument(), "tilesets");

    //now for each tileset we will extract its firstgid and source
    for (rapidjson::SizeType k = 0; k < AllTilesets.Size(); ++k)
    {
        //this is where we will put the info before shoving it into the vector
        TileSetInfo tempInfo;

        //double check this - its an object that possesses the firstgid
        //and the firstgid is an int
        assert(AllTilesets[k].IsObject());
        assert(AllTilesets[k].HasMember("firstgid"));
        assert(AllTilesets[k]["firstgid"].IsInt());

        //cool now take the int and put into temp
        tempInfo.FirstGid = AllTilesets[k]["firstgid"].GetInt();

        //and now repeat for the source
        assert(AllTilesets[k].HasMember("source"));
        assert(AllTilesets[k]["source"].IsString());

        //except we have to do something special for this string to get the
        //correct file path
        //so before we store it lets change it
        std::string tempString = AllTilesets[k]["source"].GetString();
        tempInfo.JsonFilePath = FixJsonPath(tempString);

        //thats all the info we need so slap that boi into the vector
        AllTileInfo.push_back(tempInfo);
    }

    //okay now we are done with that file so now to open the json files
    //from the tilesets (this code might have to change - rn i want to
    //reuse the json object i have but there isnt really any functionality
    //to safely do that so i might just have to create a new one for each
    //file if this doesnt work)

    //so for each tileset - open its json file and extract the correct info
    for (int k = 0; k < AllTileInfo.size(); ++k)
    {
        //do the switch to new file
        TiledDataReader->openJSONFILE(AllTileInfo[k].JsonFilePath.c_str());

        //confirm that the document is an object
        assert(TiledDataReader->getCurrentDocument().IsObject());

        //first item to get is the image path
        assert(TiledDataReader->hasMember("image"));
        AllTileInfo[k].ImageFilePath = TiledDataReader->getString("image");

        //next is to get the tilecount
        assert(TiledDataReader->hasMember("tilecount"));
        AllTileInfo[k].TileCount = TiledDataReader->getInt("tilecount");

        //that is all we need
    } //end of for loop

    //and that should be all the tiled info we will need for rn

}

/*!***************************************************************************
\brief
  Using the Tiled data just read in, create the BG tiles and store them,,,,
  also this calculates their positions and sets the correct frame index
  for the correct uv coords
*****************************************************************************/
void CreateTiledObjects(void)
{
    //name of the tiles
    std::string BackgroundName = "BackgroundTile";

    //will need to store the objects
    GameObjectManager GOM;
    GameObjectPtr object;
    int CurrentTile = 0;

    //what the positions calculations will be done in
    float posW = 0;
    float posH = 0;
    //this is the current scale of the tiles - taken and hard coded from the
    //json file - should that value change this will need to be updated
    Vector2D saveScale;
    Vector2DSet(&saveScale, 475.0f, 475.0f);
    Scene* tiled = nullptr;
    ScenesManager* SM = (ScenesManager*)ScenesManager::engine->GetISystem(ISystem::SystemNames::SysScenesManager);
    if (SM->GetSceneID("tiled") == nullptr)
    {
        tiled = new Scene("tiled");
        SM->Add(tiled);
    }
    else
    {
        return;
    }
    //traverse like 2d array
    //for the y values
    for (int k = 0; k < TiledMapHeight; ++k)
    {
        //for the x values
        for (int j = 0; j < TiledMapWidth; ++j)
        {

            CurrentTile = LayerDatas[0][k * TiledMapWidth + j] - 1;
            //build object and get it back
            object = TileFactoryBuild(BackgroundName);
            //if it exists
            if (object)
            {
                //do calcs
                //how to calc - assuming 0,0 is center and objects draw from center
                //starting from top left across and then down a spot until the (currently) 4x4 grid is filled
                // 01 02 | 03 04
                // 05 06 | 07 08
                // -------------
                // 09 10 | 11 12
                // 13 14 | 15 16
                //maybe should treat like a 2d array for loop then - ok done
                //okay important note - the tiles scale is half of its actual size (1275 / 2 = 637.5)
                // 1050 / 2 = 525( this is height and other is width) - this too can change
                //so if 0,0 is center and half the scale of a tile gets the center - subtract
                // 3/2 width to get first x, and add 1 width for each next x
                //same idea applies to height (but everytime we switch k value)
                // add 3/2 to get first y and then subtract 1 for each next y
                //^^^^^^^^^^^^^^^^^^^^^^^^^^^
                //above is original thought process - obvi some numbers are changed but u get the idea

                //access the objects transform
                TransformPtr tform = (TransformPtr)object->GameObjectGetComponent(Component::TypeEnum::ETransform);
                Vector2D scale = *tform->TransformGetScale();
                saveScale = scale;
                
                

                //calc the new position
                if (j == 0)
                { 
                    posW = scale.x * -9.0f;
                    if (k == 0)
                    {
                        posH = scale.y * 22.0f;
                    }   
                }
                else
                {
                    posW += scale.x * 2.0f;
                }

                //slap it back in
                Vector2D temp;
                Vector2DSet(&temp, posW, posH);
                tform->TransformSetTranslation(&temp);
                ///////////////////////////////////////////////////////////////////
                //do uv coord calcs
                //should just be changing the frame index 
                //get the sprite
                SpritePtr spt = (SpritePtr)object->GameObjectGetComponent(Component::TypeEnum::ESprite);
                //slap new value in 1 - 16
                spt->SpriteSetFrame(CurrentTile);


                //toss into void
                GOM.GameObjectManagerAdd(object);
                tiled->Add(object);
				//scene->Add(object);

                //onto the next tile 
                //++CurrentTile;


            }//end of if
        }//end of height

        posH -= saveScale.y * 2.0f;
        
    }//end of width
    
}

/*!***************************************************************************
\brief
  Tiled vers of the game object factory since that code wasnt friendly with 
  this concept?

\param name
  name of the object to create

\return
  the game object created
*****************************************************************************/
GameObjectPtr TileFactoryBuild(const std::string name)
{
    GameObjectManager GOM;

    //if there is no existing archteype
    if (GOM.GameObjectManagerGetArchetype((name.c_str())) == NULL)
    {
        //create the json file path with the gameobjects name
        char pathName[FILENAME_MAX] = "";
        sprintf_s(pathName, _countof(pathName), "Data/%s.json", (name.c_str()));

        //open the json file
        JSON json;
        json.openJSONFILE(pathName);

        //create the gameobject from that file
        GameObjectPtr gameObject;
        gameObject = json.GameObjectReadJS(NULL);

        //make an archetype for the gameobject
        GOM.GameObjectManagerAddArchetype(gameObject);
    }

    //if there is an existing archetype, or after the archteype has been created
    //make a clone of the archetype and return it
    GameObjectPtr clone = GOM.GameObjectManagerGetArchetype((name.c_str()));
    GameObjectPtr returnclone = clone->GameObjectClone();

    if (returnclone)
    {
        return returnclone;
    }
    
    return NULL;
}

/*!***************************************************************************
\brief
  gives the width and height of the image created by the 16 bg tiles

\return
  width and height values - x and y respectively
*****************************************************************************/
Vector2D GetWidthHeightOfMap(void)
{
    Vector2D saveScale;
    //this is hard coded name - is subject to change
    Vector2DSet(&saveScale, 475.0f * TiledMapWidth, 475.0f * TiledMapHeight);
    return saveScale;
}
#pragma endregion PublicFunctions