//basic window
#include "raylib.h" 
#include "raymath.h"
#include <string.h>

#define screenWidth 1100
#define screenHeight 1000
#define gridWidth 1000
#define gridHeight 1000

const float scaleX = (float) screenWidth / gridWidth;
const float scaleY = (float) screenHeight / gridHeight;

bool grid[gridWidth][gridHeight] = { false };  // needs to be a bool for game of life, dead or alive
bool nextGrid[gridWidth][gridHeight] = { false }; // buffer grid to update grid at same time

typedef struct GridView {
    Vector2 offset; //how far we are from (0,0)
    float zoom;
} GridView;

Vector2 GridToScreen (Vector2 gridCoords, GridView view) {
    Vector2 screenCoords = { 
        (gridCoords.x * scaleX * view.zoom) + view.offset.x,
        (gridCoords.y * scaleY * view.zoom) + view.offset.y,
     };
    
    return screenCoords;
}

Vector2 ScreenToGrid(Vector2 screenCoords, GridView view) {
    Vector2 gridCoords = { 
        (screenCoords.x - view.offset.x) / (scaleX * view.zoom), 
        (screenCoords.y - view.offset.y) / (scaleY * view.zoom),
    };

    return gridCoords;
}

int countNeighbors (int i, int j) {
    int neighborCount = 0;
    // check boundaries
    if (i > 0 && j > 0 && i < gridWidth - 1 && j < gridHeight - 1) {
        if(grid[i-1][j])
            neighborCount++;
        if(grid[i-1][j-1])
            neighborCount++;
        if(grid[i][j-1])
            neighborCount++;
        if(grid[i+1][j])
            neighborCount++;
        if(grid[i+1][j+1])
            neighborCount++;
        if(grid[i][j+1])
            neighborCount++;
        if(grid[i+1][j-1]) 
            neighborCount++;
        if(grid[i-1][j+1]) 
            neighborCount++;
    }
    return neighborCount;
}

void updateGrid() {
    // clear nextGrid
    memset(nextGrid, 0, sizeof(nextGrid));


    for (int i = 0; i < gridWidth; i++) {
        for (int j = 0; j < gridHeight; j++) {

            int neighborCount = countNeighbors(i, j);
            
            if (grid[i][j]) {
                // game of life rules
                if (neighborCount == 2 || neighborCount == 3)
                    nextGrid[i][j] = true;
            } else {
                //resurrection rules
                //any dead cell with exactly three neighbors becomes alive
                if(neighborCount == 3)
                    nextGrid[i][j] = true;
            }
        }
    }
    // update cells in one go
    memcpy(grid, nextGrid, sizeof(grid));
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "Conway's Game Of life");
    SetTargetFPS(60);

    bool isPaused = false;
    float updateTimer = 0.0f;
    float updateInterval = 0.5f;
    int generations = 0;

    GridView view = {
        .offset = (Vector2) { 
            screenWidth / 2.0f - (gridWidth * scaleX * 50.0f) / 2.0f,
            screenHeight / 2.0f - (gridHeight * scaleY * 50.0f) / 2.0f,
        },
        .zoom = 50.0f,
    };

    while(!WindowShouldClose()) {
        // this returns either a positive float, negative float, or 0.0 depending on wheel movement
        float wheel = GetMouseWheelMove();
        // if it returned anything other than 0
        if (wheel != 0) {
            Vector2 mousePos = GetMousePosition();
            // store mouse pos
            Vector2 worldPos = ScreenToGrid(mousePos, view);

            // zoom (change float to mess around with zoom speed, also * view.zoom so it's fast at zoomed out, slow zoomed in)
            view.zoom += wheel * 0.1f * view.zoom;
            // stop infinite zoom out
            // 0.1 is zoom out 10%
            if (view.zoom < 10.0f)
                view.zoom = 10.0f;
            // 5.0 is zoom in 500%
            if (view.zoom > 200.0f)
                view.zoom = 200.0f;

            // get new screen point after zoom
            Vector2 newScreenPos = GridToScreen(worldPos, view);

            // adjust offset
            view.offset.x += mousePos.x - newScreenPos.x;
            view.offset.y += mousePos.y - newScreenPos.y;
        }

        // panning with middle mouse button
        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            view.offset.x += delta.x;
            view.offset.y += delta.y;
        }

        // optional keyboard panning
        float panSpeed = 10.0f;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) 
            view.offset.y += panSpeed;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) 
            view.offset.y -= panSpeed;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) 
            view.offset.x += panSpeed;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) 
            view.offset.x -= panSpeed;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = GetMousePosition();
            Vector2 worldPos = ScreenToGrid(pos, view);

            int x = (int) worldPos.x;
            int y = (int) worldPos.y;
            // making sure mouse pos is within grid boundaries
            if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                grid[x][y] = true;
            }
        }

        // center grid on key press C
        if (IsKeyPressed(KEY_C)) {
            view.offset.x = (screenWidth - (gridWidth * scaleX * view.zoom)) / 2.0f;
            view.offset.y = (screenHeight - (gridHeight * scaleY * view.zoom)) / 2.0f;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            isPaused = !isPaused;
        }

        if (!isPaused) {
            updateTimer += GetFrameTime();
            if (updateTimer >= updateInterval) {
                updateGrid();
                updateTimer = 0.0f;
                generations++;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);

            //DrawLineV()
            for(int x = 0; x <= gridWidth; x++) {
                Vector2 start = GridToScreen((Vector2) { x , 0 }, view);
                Vector2 end = GridToScreen((Vector2) { x , gridHeight }, view);

                DrawLineV(start, end, LIGHTGRAY);
            }   

            for(int y = 0; y <= gridHeight; y++) {
                Vector2 start = GridToScreen((Vector2) { 0 , y }, view);
                Vector2 end = GridToScreen((Vector2) { gridWidth , y }, view);

                DrawLineV(start, end, LIGHTGRAY);
            }
            for (int i = 0; i < sizeof(grid) / sizeof(grid[0]); i++) {
                for (int j = 0; j < sizeof(grid[0]) / sizeof(grid[0][0]); j++) {
                    if (grid[i][j]) {
                        // fill in cells if alive (true)
                        Vector2 recPos = GridToScreen((Vector2) { i , j }, view);
                        Vector2 recSize = { scaleX * view.zoom, scaleY * view.zoom };
                        DrawRectangleV(recPos, recSize, WHITE);
                    }
                }
            }

            DrawText(isPaused ? "PAUSED" : "RUNNING", 10, 10, 20, WHITE);
            DrawText(TextFormat("GENERATION: %d", generations), screenWidth - 200, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;       
}