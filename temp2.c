#include <stdio.h>

#include "raylib.h"

void
main(int argc, char **argv)
{
  InitWindow(500, 500, "helo");

  Texture2D t = LoadTexture("g.png");

  SetTargetFPS(30);
  while (!WindowShouldClose()) {
    if (IsKeyDown('Q')) break;
    Vector2 mp = GetMousePosition();
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawTexture(t, 0, 0, WHITE);
      DrawRectangleLines(0, 0, 172, 187, RED);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DrawCircleV(GetMousePosition(), 16, RED);
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        DrawCircleV(GetMousePosition(), 16, BLUE);
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        DrawCircleV(GetMousePosition(), 16, ORANGE);
    }
    EndDrawing();
  }
}
