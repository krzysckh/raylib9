#include <stdio.h>

#include "raylib.h"

void
main(int argc, char **argv)
{
  InitWindow(500, 500, "helo");

  SetTargetFPS(30);
  while (!WindowShouldClose()) {
    if (IsKeyDown('Q')) break;
    Vector2 mp = GetMousePosition();
    BeginDrawing();
      ClearBackground(GREEN);
      DrawRectangle(0, 0, 250, 250, BLUE);
      DrawCircle(0, 0, 20, RED);
      DrawLine(0, 0, (int)mp.x+1, (int)mp.y+1, WHITE);
      DrawText("Hello, World!", 0, 300, 22, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DrawCircle((int)mp.x+1, (int)mp.y+1, 20, ORANGE);
      if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        DrawCircle((int)mp.x+1, (int)mp.y+1, 20, BLUE);
      if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
        DrawCircle((int)mp.x+1, (int)mp.y+1, 20, YELLOW);
    EndDrawing();
  }
}
