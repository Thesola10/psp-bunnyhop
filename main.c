/*******************************************************************************************
*
*   raylib [textures] example - Bunnymark
*
*   Example originally created with raylib 1.6, last time updated with raylib 2.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <raylib.h>

#include "controller.h"
#include "level.h"
#include "entity.h"

PSP_MODULE_INFO("bunnymark", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define ATTR_PSP_WIDTH 480
#define ATTR_PSP_HEIGHT 272

SceCtrlData pad;


int flag=1;
int xflag=0;
int x;
int y;

#define MAX_BUNNIES        50000    // 50K bunnies limit

// This is the maximum amount of elements (quads) per batch
// NOTE: This value is defined in [rlgl] module and can be changed there
#define MAX_BATCH_ELEMENTS  8192

typedef struct Bunny {
    Vector2 position;
    Vector2 speed;
    Color color;
} Bunny;


char up, down, left, right;

int _flush_cache() {
    return 0;
}


#define TILESIZE 16

void printTile(Texture2D tileset, int tileId, Vector2 where)
{
    int workId = tileId - 1;

    int tilex = workId % (tileset.width / TILESIZE);
    int tiley = workId / (tileset.width / TILESIZE);

    Rectangle subTile = (Rectangle) {
        .width = TILESIZE,
        .height = TILESIZE,
        .x = tilex * TILESIZE,
        .y = tiley * TILESIZE
    };

    if (!tileId) return;

    DrawTextureRec(tileset, subTile, where, WHITE);
}


extern bhop_Level level_lapinou;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = ATTR_PSP_WIDTH;
    const int screenHeight = ATTR_PSP_HEIGHT;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - bunnymark");

    x=screenWidth/2;
    y=screenHeight/2;

    Rectangle boxB= { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f, 6, 6 };
    // Load bunny texture
    Texture2D texBunny = LoadTexture("host0:/textures/bunnymark/wabbit_alpha.png");

    Image imgBg = LoadImage("host0:/textures/bg-fs8.png");
    Texture2D texBg = LoadTextureFromImage(imgBg);

    bhop_loadLevelTileset(LoadTexture("host0:/textures/spriteset-fs8.png"));

    level_lapinou.terrainLayer.lifetime = 0;
    level_lapinou.decorLayer.lifetime = 0;


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    bhop_ButtonMap bm = {
        .onCross = bhop_$ButtonEvent({ xflag = pressed; }),
        .onCircle = bhop_$ButtonEvent({
                    bhop_Entity *player = bhop_Level_getPlayerEntity(&level_lapinou);
                    if (playerTiming.cdSouth || playerTiming.cdEast || playerTiming.cdWest)
                        player->velocity.y = -10.0f;
                }),
        .onStart = bhop_$ButtonEvent({ flag = 0; }),
        .onUp = bhop_$ButtonEvent({ up = pressed; }),
        .onDown = bhop_$ButtonEvent({ down = pressed; }),
        .onLeft = bhop_$ButtonEvent({ left = pressed; }),
        .onRight = bhop_$ButtonEvent({ right = pressed; }),
    };

    bhop_ButtonMap_load(&bm);

    // Main game loop
    while (flag && !WindowShouldClose()) {
        // Update
        bhop_scanButtons();

        bhop_updateEntities(&level_lapinou);

        bhop_Entity *player = bhop_Level_getPlayerEntity(&level_lapinou);
        // Update player-controlled-box (box02)
        boxB.x = x - boxB.width/2;
        boxB.y = y - boxB.height/2;
        //----------------------------------------------------------------------------------
        if (up)  y = y - 6;
        if (down) y = y + 6;
        if (left) {
            if (player->collider & bhop_EntityCollider_$SOUTH)
                player->velocity.x = -4.5f;
            else if (player->velocity.x > -4.0f)
                player->velocity.x -= 0.4f;
        }
        if (right) {
            if (player->collider & bhop_EntityCollider_$SOUTH)
                player->velocity.x = 4.5f;
            else if (player->velocity.x < 4.0f)
                player->velocity.x += 0.4f;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

# define DrawLayer$(tex) \
            DrawTextureRec( tex , \
                    (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight }, \
                    (Vector2) { 0, 0 }, WHITE);

            ClearBackground(RAYWHITE);
            //DrawTexture(bg.texture, 0, 0, WHITE);

            DrawLayer$(texBg);

            DrawLayer$(bhop_Level_getDecorTexture(&level_lapinou));

            DrawRectangleRec(boxB, BLUE);

            bhop_Level_drawEntities(&level_lapinou);

            DrawLayer$(bhop_Level_getTerrainTexture(&level_lapinou));

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
        //
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadTexture(texBunny);    // Unload bunny texture


    UnloadTexture(texBg);

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
