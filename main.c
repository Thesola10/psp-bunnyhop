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
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <raylib.h>

#include "tileset.h"
#include "controller.h"
#include "level.h"
#include "entity.h"
#include "sound.h"

PSP_MODULE_INFO("bunnyhop", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define ATTR_PSP_WIDTH 480
#define ATTR_PSP_HEIGHT 272

#ifndef DATA_PREFIX
# define DATA_PREFIX "host0:"
#endif

SceCtrlData pad;


int flag=1;
int xflag=0;
int x;
int y;

extern Texture2D bhop_levelTileset;

int playerHealth = 3;

int playerScore = 0;

int playerCooldown = 0;

// This is the maximum amount of elements (quads) per batch
// NOTE: This value is defined in [rlgl] module and can be changed there
#define MAX_BATCH_ELEMENTS  8192

char up, down, left, right;

int _flush_cache() {
    return 0;
}


extern bhop_Level level_lapinou;


void bhop_drawHealth(Texture2D heart)
{
    for (int i = 0; i < playerHealth; i++)
        DrawTexture(heart, 10 + (20 * i), 10, WHITE);
}

void bhop_drawScore(void)
{
    Texture2D tileset = bhop_levelTileset;

    for (int i = 0; i < playerScore; i++)
        bhop_drawTile(tileset, bhop_Entity_EGG,
                (Vector2) { .x = 470 - (16 + (20 * i)), .y = 10 });
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = ATTR_PSP_WIDTH;
    const int screenHeight = ATTR_PSP_HEIGHT;

    InitWindow(screenWidth, screenHeight, "BunnyHop!");


    bhop_initSound();

    x=screenWidth/2;
    y=screenHeight/2;

    Texture2D texBg = LoadTexture(DATA_PREFIX "/textures/bg-fs8.png");

    Texture2D heart = LoadTexture(DATA_PREFIX "/textures/heart.png");

    Texture2D paf = LoadTexture(DATA_PREFIX "/textures/paf.png");

    Image tileset = LoadImage(DATA_PREFIX "/textures/spriteset.png");

    bhop_loadLevelTileset(tileset);


    level_lapinou.terrainLayer.lifetime = 0;
    level_lapinou.decorLayer.lifetime = 0;

    bhop_Level_load(&level_lapinou);

    bhop_Sound *jumpSound = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/jump.wav");
    bhop_Sound *collectSound = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/collect.wav");
    bhop_Sound *hitSound = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/hit.wav");
    bhop_Sound *bounceSound = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/bounce.wav");

    bhop_Sound *bgm = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/music.wav");

    bhop_Sound *lose = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/lose.wav");
    bhop_Sound *win = bhop_Sound_loadFromFile(DATA_PREFIX "/sounds/win.wav");

    bhop_Sound_loadBgm(bgm, 1);
    bhop_Player_loadOnBounce(bhop_$EntityEvent({
        bhop_Sound_play(bounceSound);
    }));

    bhop_Player_loadOnJump(bhop_$EntityEvent({
        target->velocity.y = -10.0f;
        bhop_Sound_play(jumpSound);
    }));

    bhop_Player_loadOnWallJump(bhop_$EntityEvent({
        target->velocity.y = -6.0f;
        bhop_Sound_play(jumpSound);
    }));

    bhop_Player_loadOnCollectCoins(bhop_$EntityEvent({
        bhop_Sound_play(collectSound);
        target->type = bhop_Entity_NULL;
        playerScore += 1;
    }));

    bhop_Player_loadOnHitEnemy(bhop_$EntityEvent({
        if (!playerCooldown) {
            bhop_Sound_play(hitSound);
            playerHealth--;
            playerCooldown = 10;
        }
    }));

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    bhop_ButtonMap bm = {
        .onCross = bhop_$ButtonEvent({ xflag = pressed; }),
        .onCircle = bhop_$ButtonEvent({
                    playerJumpCd = 4;
                }),
        .onStart = bhop_$ButtonEvent({ flag = 0; }),
        .onUp = bhop_$ButtonEvent({ up = pressed; }),
        .onDown = bhop_$ButtonEvent({ down = pressed; }),
        .onLeft = bhop_$ButtonEvent({ left = pressed; }),
        .onRight = bhop_$ButtonEvent({ right = pressed; }),
    };

    bhop_ButtonMap_load(&bm);

    int timer = 0;

    Vector2 losepos;

    // Main game loop
    while (flag && !WindowShouldClose()) {

        bhop_Level *lvl = bhop_getCurrentLevel();
        // Update
        bhop_Entity *player = bhop_Level_getPlayerEntity(lvl);

        bhop_refreshSound();
        bhop_scanButtons();

        if (playerScore >= 3) {
            if (timer == 0) {
                bhop_Sound_loadBgm(0, 0);
            }

            else if (timer == 30) {
                bhop_Sound_loadBgm(win, 0);
            }

            timer++;
        } else if (playerHealth) {

            bhop_updateEntities(lvl);

            if (playerCooldown)
                playerCooldown -= 1;

            if (left) {
                if (player->collider & bhop_EntityCollider_$SOUTH)
                    player->velocity.x = -4.5f;
                else if (player->velocity.x > -2.0f)
                    player->velocity.x -= 0.4f;
            }
            if (right) {
                if (player->collider & bhop_EntityCollider_$SOUTH)
                    player->velocity.x = 4.5f;
                else if (player->velocity.x < 2.0f)
                    player->velocity.x += 0.4f;
            }
        } else {

            if (timer == 0) {
                losepos = player->origin;
                bhop_Sound_loadBgm(0, 0);

            } else if (timer == 60) {
                bhop_Sound_loadBgm(lose, 0);
                player->velocity.y = -10.0f;
            } else if (timer == 360) {
                bhop_Level_load(&level_lapinou);
                timer = 0;
                playerHealth = 3;
                playerScore = 0;

                bhop_ButtonMap_load(&bm);
                bhop_Sound_loadBgm(bgm, 1);
                continue;
            }

            // Mario-style game over
            if (timer >= 60) {
                player->origin.y += player->velocity.y;
                player->velocity.y += 1.2f;
            }

            timer ++;

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

            DrawLayer$(bhop_Level_getDecorTexture(lvl));

            if (!playerHealth)
                DrawTexture(paf, losepos.x - 16, losepos.y - 32, WHITE);

            if (playerHealth)
                bhop_Level_drawEntities(lvl);
            DrawLayer$(bhop_Level_getTerrainTexture(lvl));

            if (!playerHealth)
                bhop_Level_drawEntities(lvl);

            bhop_drawHealth(heart);
            bhop_drawScore();

            //DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
        //
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------


    UnloadTexture(texBg);

    bhop_Sound_unload(hitSound);
    bhop_Sound_unload(bounceSound);
    bhop_Sound_unload(jumpSound);
    bhop_Sound_unload(collectSound);

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
