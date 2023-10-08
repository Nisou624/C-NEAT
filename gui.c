#include <raylib.h>

int main(void) {
    InitWindow(800, 600, "C-NEAT");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Za3ma GUI", 380, 280, 20, DARKGRAY);
        DrawCircleLines(100, 100, 24, GREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
