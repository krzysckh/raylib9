#include "raylib.h"
#include "rlgl.h"
#include "m.h"

#define PF_COMMON_IMPL
#undef DEG2RAD
#include "../PixelForge/examples/common.h"

void redraw(void);

int main(void)
{
    InitWindow(600, 600, "balsak");
    PFMvec3 camPos = { -2.0f, 1.5f, -2.0f };
    float timer = 0;

    while (1)
    {
        PollInputEvents();
        if (IsKeyPressed('Q')) break;
        // Update camera position
        camPos[0] = 2.0f * cosf(timer);
        camPos[2] = 2.0f * sinf(timer);
        timer += 2.0f * 0.032f;

        // Clear the destination buffer (frontBuffer)
        pfClear(PF_COLOR_BUFFER_BIT | PF_DEPTH_BUFFER_BIT);

        // Draw something on each iteration of the main loop
        PF_Begin3D(600, 600, 60.0);
            PF_Update3D(camPos[0], camPos[1], camPos[2], 0, 0, 0);
            PF_DrawCube(1.0f);
        PF_End3D();
        redraw();
    }

    CloseWindow();

    return 0;
}
