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


    Bunny *bunnies = (Bunny *)malloc(MAX_BUNNIES*sizeof(Bunny));    // Bunnies array

    int bunniesCount = 0;           // Bunnies counter

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------



    bhop_ButtonMap bm = {
        .onCross = bhop_$ButtonEvent({ xflag = pressed; }),
        .onStart = bhop_$ButtonEvent({ flag = 0; }),
        .onUp = bhop_$ButtonEvent({ up = pressed; }),
        .onDown = bhop_$ButtonEvent({ down = pressed; }),
        .onLeft = bhop_$ButtonEvent({ left = pressed; }),
        .onRight = bhop_$ButtonEvent({ right = pressed; })
    };

    bhop_ButtonMap_load(&bm);

    // Main game loop
    while (flag && !WindowShouldClose())    
    {
        // Update
        bhop_scanButtons();
        // Update player-controlled-box (box02)
        boxB.x = x - boxB.width/2;
        boxB.y = y - boxB.height/2;
        //----------------------------------------------------------------------------------
        if (up)  y = y - 6;
        if (down) y = y + 6;
        if (left) x = x - 6;
        if (right) x = x + 6;
        if (xflag)
        {
            //xflag=0;
            // Create more bunnies
            for (int i = 0; i < 100; i++)
            {
                if (bunniesCount < MAX_BUNNIES)
                {
                    bunnies[bunniesCount].position.x = boxB.x;
                    bunnies[bunniesCount].position.y = boxB.y;
                    bunnies[bunniesCount].speed.x = (float)GetRandomValue(-250, 250)/60.0f;
                    bunnies[bunniesCount].speed.y = (float)GetRandomValue(-250, 250)/60.0f;
                    bunnies[bunniesCount].color = (Color){ GetRandomValue(50, 240),
                                                       GetRandomValue(80, 240),
                                                       GetRandomValue(100, 240), 255 };
                    bunniesCount++;
                }
            }
        }

        // Update bunnies
        for (int i = 0; i < bunniesCount; i++)
        {
            bunnies[i].position.x += bunnies[i].speed.x;
            bunnies[i].position.y += bunnies[i].speed.y;

            if (((bunnies[i].position.x + texBunny.width/2) > GetScreenWidth()) ||
                ((bunnies[i].position.x + texBunny.width/2) < 0)) bunnies[i].speed.x *= -1;
            if (((bunnies[i].position.y + texBunny.height/2) > GetScreenHeight()) ||
                ((bunnies[i].position.y + texBunny.height/2 - 40) < 0)) bunnies[i].speed.y *= -1;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTexture(texBg, 1, 1, WHITE);
            //DrawTexture(bg.texture, 0, 0, WHITE);
            /*DrawTextureRec(texBg,
                    (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight },
                    (Vector2) { 0, 0 }, WHITE);*/
            DrawRectangleRec(boxB, BLUE);

            for (int i = 0; i < bunniesCount; i++)
            {
                // NOTE: When internal batch buffer limit is reached (MAX_BATCH_ELEMENTS),
                // a draw call is launched and buffer starts being filled again;
                // before issuing a draw call, updated vertex data from internal CPU buffer is send to GPU...
                // Process of sending data is costly and it could happen that GPU data has not been completely
                // processed for drawing while new data is tried to be sent (updating current in-use buffers)
                // it could generates a stall and consequently a frame drop, limiting the number of drawn bunnies
                DrawTexture(texBunny, (int)bunnies[i].position.x, (int)bunnies[i].position.y, bunnies[i].color);
            }

            DrawRectangle(0, 0, screenWidth, 40, BLACK);
            DrawText(TextFormat("bunnies: %i", bunniesCount), 120, 15, 10, GREEN);
            DrawText(TextFormat("batched draw calls: %i", 1 + bunniesCount/MAX_BATCH_ELEMENTS), 200, 15, 10, MAROON);

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(bunnies);              // Unload bunnies data array

    UnloadTexture(texBunny);    // Unload bunny texture


    UnloadTexture(texBg);

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
