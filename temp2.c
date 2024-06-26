#include <stdio.h>

#include "raylib.h"

void
main(int argc, char **argv)
{
  InitWindow(500, 500, "helo");

  Image i = LoadImage("g.png");
  Texture2D t = LoadTexture("g.png");
  ExportImage(i, "g.jpg");

  SetTargetFPS(30);
  while (!WindowShouldClose()) {
    if (IsKeyDown('Q')) break;
    Vector2 mp = GetMousePosition();
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawTexture(t, 0, 0, WHITE);
      DrawRectangleLines(0, 0, 172, 187, RED);
    }
    EndDrawing();
  }
}
