/*******************************************************************************************
*
*   flappy - a very simple flappy bird clone in a one-night boredom spree
*
*   This game has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Violet McKinney <vi@violet.wtf>
*
********************************************************************************************/

#include "raylib.h"
#include <stdbool.h>

const int WIDTH = 800;
const int HEIGHT = 800;

const int TILE_SIZE = 25;

const int TILE_WIDTH = WIDTH / TILE_SIZE;
const int TILE_HEIGHT = HEIGHT / TILE_SIZE;

const int TILE_NIL = 0;
const int TILE_PIPE = 1;
const int TILE_BIRD = 2;
const int TILE_DEAD_BIRD = 3;

const int MAX_TILE_VALUE = TILE_DEAD_BIRD;

const int PIPE_WIDTH = 2;

const Color TILE_COLOURS[] = 
{ 
    // TILE_NIL (unused)
    WHITE,

    // TILE_PIPE
    GREEN,

    // TILE_BIRD
    YELLOW,

    // TILE_DEAD_BIRD
    RED,
};

void InitTestMap(int tiles[TILE_WIDTH][TILE_HEIGHT]) 
{
    tiles[0][0] = 1;
    tiles[0][99] = 1;
    tiles[99][0] = 1;
    tiles[99][99] = 1;
}

bool PipeIsNear(int tiles[TILE_WIDTH][TILE_HEIGHT], int x) 
{
    for (int modX = -1; modX <= 1; modX++) 
    {
        if (modX != -1 || x != 0) 
        {
            // Incase hole is at 0
            for (int y = 0; y < 3; y++) 
            {
                if (tiles[x + modX][y] == TILE_PIPE) 
                {
                    return true;
                }
            }
        }
    }

    return false;
}

int GetRandomPipeX(int tiles[TILE_WIDTH][TILE_HEIGHT], int startX) {
    int posX = GetRandomValue(startX, TILE_WIDTH - 2);

    if (PipeIsNear(tiles, posX)) 
    {
        return GetRandomPipeX(tiles, startX);
    }

    return posX;
}

void GenPipes(int tiles[TILE_WIDTH][TILE_HEIGHT], int startX, int amount) 
{
    for (int i = 0; i < amount; i++) 
    {
        int posX = GetRandomPipeX(tiles, startX);
        int holeY = GetRandomValue(0, TILE_HEIGHT - 1);

        for (int x = 0; x < PIPE_WIDTH; x++) 
        {
            for (int y = 0; y < TILE_HEIGHT; y++) 
            {
                if ((y != holeY && y != holeY + 1) || (holeY == 1 && y != 0)) 
                {
                    tiles[posX + x][y] = TILE_PIPE;
                }
            }
        }
    }
}

bool Every(int delta, int ticks) 
{
    return ticks != 0 && ticks % delta == 0;
}

// Returns whether bird is still alive
bool ScrollScreen(int tiles[TILE_WIDTH][TILE_HEIGHT])
{
    bool alive = true;

    for (int x = 0; x < TILE_WIDTH - 1; x++) 
    {
        for (int y = 0; y < TILE_HEIGHT; y++)
        {
            // Bird collision
            if (tiles[x][y] == TILE_BIRD) 
            {
                if (tiles[x + 1][y] == TILE_PIPE)
                {
                    tiles[x][y] = TILE_DEAD_BIRD;
                    alive = false;
                }
            } else if (tiles[x + 1][y] != TILE_BIRD) 
            {
                tiles[x][y] = tiles[x + 1][y];
            }
        }
    }

    // Clear last x row
    for (int y = 0; y < TILE_HEIGHT; y++) 
    {
        tiles[TILE_WIDTH - 1][y] = TILE_NIL;
    }

    return alive;
}

int main(void)
{
    bool go = true;
    bool isInPipe = false;

    int ticks = 0;
    int score = 0;
    int waitCounter = 0;
    int tiles[TILE_WIDTH][TILE_HEIGHT];
    int jumps = 1;

    int birdX = 1;
    int birdY = TILE_HEIGHT / 2;

    // Zero it out, which must be done for some reason
    for (int x = 0; x < TILE_WIDTH; x++) 
    {
        for (int y = 0; y < TILE_HEIGHT; y++) 
        {
            tiles[x][y] = 0;
        }
    }

    // InitTestMap(tiles);

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(WIDTH, HEIGHT, "flappy");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    GenPipes(tiles, 7, 2);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        if (ticks == 60) 
        {
            ticks = 0;
        } else 
        {
            ticks++;
        }
        
        if (go)
        {
            if (Every(20, ticks)) 
            {
                tiles[birdX][birdY] = TILE_NIL; // Remove bird
                birdY++;

                if (birdY == TILE_HEIGHT) {
                    go = false;
                }

                if (jumps > 1) 
                {
                    jumps--;
                }
            }

            if (IsKeyPressed(KEY_SPACE)) 
            {
                tiles[birdX][birdY] = TILE_NIL; // Remove bird
                birdY -= 1;

                if (tiles[birdX][birdY] == TILE_PIPE) {
                    go = false;
                    score--;
                }

                jumps = 3;
            }

            if (go)
            {
                tiles[birdX][birdY] = TILE_BIRD;
            } else {
                tiles[birdX][birdY] = TILE_DEAD_BIRD;
            }

            if (Every(30, ticks)) 
            {
                if (!ScrollScreen(tiles)) 
                {
                    go = false;
                    score--;
                }

                waitCounter++;
            }

            if (waitCounter == 10) 
            {
                waitCounter = 0;
                GenPipes(tiles, TILE_WIDTH - 5, 1);
            }

            // Score
            bool isStartingPipe = false;

            if (birdY == TILE_HEIGHT - 1 || birdY == TILE_HEIGHT - 2) 
            {
                isStartingPipe = 
                    tiles[birdX][birdY - 1] == TILE_PIPE ||
                    tiles[birdX][birdY - 2] == TILE_PIPE;
            } else
            {
                isStartingPipe = 
                    tiles[birdX][birdY + 1] == TILE_PIPE ||
                    tiles[birdX][birdY + 2] == TILE_PIPE;
            }

            if (isStartingPipe)
            {
                if (!isInPipe)
                {
                    isInPipe = true;
                    score++;
                }
            } else
            {
                isInPipe = false;
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(SKYBLUE);

        for (int x = 0; x < TILE_WIDTH; x++) 
        {
            for (int y = 0; y < TILE_HEIGHT; y++) 
            {
                int tile = tiles[x][y];

                if (tile != TILE_NIL) 
                {
                    DrawRectangle(
                        x * TILE_SIZE, 
                        y * TILE_SIZE, 
                        TILE_SIZE, 
                        TILE_SIZE, 
                        TILE_COLOURS[tile]
                    );
                }
            }
        }

        char buffer[31];
        
        if (go)
        {
            sprintf(buffer, "score: %d", score);
        } else
        {
            sprintf(buffer, "score: %d - you died!", score);
        }

        DrawText(buffer, 0, 0, 32, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
