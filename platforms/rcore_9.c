#include <u.h>
#include <lib9.h>
#include <draw.h>
#include <event.h>
#include <fcntl.h>
#include <sys/times.h>

#include <ctype.h>

static uint xr, yr;

typedef struct {
  uchar *rgbuf;
} PlatformData;

extern CoreData CORE;
static PlatformData pdata = { 0 };

int InitPlatform(void);
bool InitGraphicsDevice(void);

bool
WindowShouldClose(void)
{
  if (CORE.Window.ready)
    return CORE.Window.shouldClose;
  return true;
}

void
ToggleFullscreen(void)
{
  TRACELOG(LOG_WARNING, "ToggleFullscreen() not available on target platform");
}

void
ToggleBorderlessWindowed(void)
{
  TRACELOG(LOG_WARNING, "ToggleBorderlessWindowed() not available on target platform");
}

void
MaximizeWindow(void)
{
  TRACELOG(LOG_WARNING, "MaximizeWindow() not available on target platform");
}

void
MinimizeWindow(void)
{
  TRACELOG(LOG_WARNING, "MinimizeWindow() not available on target platform");
}

// Set window state: not minimized/maximized
void
RestoreWindow(void)
{
  TRACELOG(LOG_WARNING, "RestoreWindow() not available on target platform");
}

// Set window configuration state using flags
void
SetWindowState(unsigned int flags)
{
  TRACELOG(LOG_WARNING, "SetWindowState() not available on target platform");
}

void
ClearWindowState(unsigned int flags)
{
  TRACELOG(LOG_WARNING, "ClearWindowState() not available on target platform");
}

void
SetWindowIcon(GETS(Image) image)
{
  TRACELOG(LOG_WARNING, "SetWindowIcon() not available on target platform");
}

void
SetWindowIcons(GETS(Image) *images, int count)
{
  TRACELOG(LOG_WARNING, "SetWindowIcons() not available on target platform");
}

void
SetWindowTitle(const char *title)
{
  CORE.Window.title = title;
}

void
SetWindowPosition(int x, int y)
{
  TRACELOG(LOG_WARNING, "SetWindowPosition() not available on target platform");
}

void
SetWindowMonitor(int monitor)
{
  TRACELOG(LOG_WARNING, "SetWindowMonitor() not available on target platform");
}

void
SetWindowMinSize(int width, int height)
{
  CORE.Window.screenMin.width = width;
  CORE.Window.screenMin.height = height;
}

void
SetWindowMaxSize(int width, int height)
{
  CORE.Window.screenMax.width = width;
  CORE.Window.screenMax.height = height;
}

void
SetWindowSize(int width, int height)
{
  TRACELOG(LOG_WARNING, "SetWindowSize() not available on target platform");
}

void
SetWindowOpacity(float opacity)
{
  TRACELOG(LOG_WARNING, "SetWindowOpacity() not available on target platform");
}

void
SetWindowFocused(void)
{
  TRACELOG(LOG_WARNING, "SetWindowFocused() not available on target platform");
}

void *
GetWindowHandle(void)
{
  TRACELOG(LOG_WARNING, "GetWindowHandle() not implemented on target platform");
  return NULL;
}

int
GetMonitorCount(void)
{
  TRACELOG(LOG_WARNING, "GetMonitorCount() not implemented on target platform");
  return 1;
}

// Get number of monitors
int
GetCurrentMonitor(void)
{
  TRACELOG(LOG_WARNING, "GetCurrentMonitor() not implemented on target platform");
  return 0;
}

GETS(Vector2)
GetMonitorPosition(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorPosition() not implemented on target platform");
  return (GETS(Vector2)) {
    0, 0
  };
}

// Get selected monitor width (currently used by monitor)
int
GetMonitorWidth(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorWidth() not implemented on target platform");
  return 0;
}

// Get selected monitor height (currently used by monitor)
int
GetMonitorHeight(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorHeight() not implemented on target platform");
  return 0;
}

// Get selected monitor physical width in millimetres
int
GetMonitorPhysicalWidth(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorPhysicalWidth() not implemented on target platform");
  return 0;
}

// Get selected monitor physical height in millimetres
int
GetMonitorPhysicalHeight(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorPhysicalHeight() not implemented on target platform");
  return 0;
}

// Get selected monitor refresh rate
int
GetMonitorRefreshRate(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorRefreshRate() not implemented on target platform");
  return 0;
}

// Get the human-readable, UTF-8 encoded name of the selected monitor
const char *
GetMonitorName(int monitor)
{
  TRACELOG(LOG_WARNING, "GetMonitorName() not implemented on target platform");
  return "";
}

// Get window position XY on monitor
GETS(Vector2)
GetWindowPosition(void)
{
  TRACELOG(LOG_WARNING, "GetWindowPosition() not implemented on target platform");
  return (GETS(Vector2)) {
    0, 0
  };
}

// Get window scale DPI factor for current monitor
GETS(Vector2)
GetWindowScaleDPI(void)
{
  TRACELOG(LOG_WARNING, "GetWindowScaleDPI() not implemented on target platform");
  return (GETS(Vector2)) {
    1.0f, 1.0f
  };
}

// Set clipboard text content
void
SetClipboardText(const char *text)
{
  TRACELOG(LOG_WARNING, "SetClipboardText() not implemented on target platform");
}

// Get clipboard text content
// NOTE: returned string is allocated and freed by GLFW
const char *
GetClipboardText(void)
{
  TRACELOG(LOG_WARNING, "GetClipboardText() not implemented on target platform");
  return NULL;
}

// Show mouse cursor
void
ShowCursor(void)
{
  CORE.Input.Mouse.cursorHidden = false;
}

// Hides mouse cursor
void
HideCursor(void)
{
  CORE.Input.Mouse.cursorHidden = true;
}

// Enables cursor (unlock cursor)
void
EnableCursor(void)
{
  // Set cursor position in the middle
  SetMousePosition(CORE.Window.screen.width/2, CORE.Window.screen.height/2);

  CORE.Input.Mouse.cursorHidden = false;
}

// Disables cursor (lock cursor)
void
DisableCursor(void)
{
  // Set cursor position in the middle
  SetMousePosition(CORE.Window.screen.width/2, CORE.Window.screen.height/2);

  CORE.Input.Mouse.cursorHidden = true;
}

void
redraw(void)
{
  xr = screen->r.min.x;
  yr = screen->r.min.y;

  int w = CORE.Window.screen.width, h = CORE.Window.screen.height;
  Rectangle r = Rect(0, 0, w, h);
  Image *i = allocimage(display, r, RGB24, 0, 0);
  if (i == nil) {
    sysfatal("allocimage: %r");
  }

  if (loadimage(i, i->r, (void*)pdata.rgbuf, w*h*3) < 0) {
    sysfatal("loadimage: %r");
  }

  //drawop(screen, Rect(xr, yr, w+xr, h+yr), i, nil, i->r.min, 0);
  draw(screen, Rect(xr, yr, w+xr, h+yr), i, nil, i->r.min);
  flushimage(display, 1);

  freeimage(i);
}

void
eresized(int new)
{
  if (new && getwindow(display, Refnone) < 0) {
    fprintf(stderr, "can't reattach to window");
    sysfatal("resize");
  }

  redraw();
}

void
SwapScreenBuffer(void)
{
  redraw();
}

vlong
_nsec(void)
{
  return times(nil);
}

// Get elapsed time measure in seconds since InitTimer()
double
GetTime(void)
{
  return (double)(_nsec() - (double)CORE.Time.base)/(double)1000;  // Elapsed time since InitTimer()
}

// Open URL with default system browser (if available)
// NOTE: This function is only safe to use if you control the URL given.
// A user could craft a malicious string performing another action.
// Only call this function yourself not with user input or make sure to check the string yourself.
// Ref: https://github.com/raysan5/raylib/issues/686
void
OpenURL(const char *url)
{
  // Security check to (partially) avoid malicious code on target platform
  if (strchr(url, '\'') != NULL) TRACELOG(LOG_WARNING, "SYSTEM: Provided URL could be potentially malicious, avoid [\'] character");
  else {
    // TODO:
  }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Inputs
//----------------------------------------------------------------------------------

// Set internal gamepad mappings
int
SetGamepadMappings(const char *mappings)
{
  TRACELOG(LOG_WARNING, "SetGamepadMappings() not implemented on target platform");
  return 0;
}

// Set mouse position XY
void
SetMousePosition(int x, int y)
{
  CORE.Input.Mouse.currentPosition = (GETS(Vector2)) {
    (float)x, (float)y
  };
  CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
}

// Set mouse cursor
void
SetMouseCursor(int cursor)
{
  TRACELOG(LOG_WARNING, "SetMouseCursor() not implemented on target platform");
}

// Register all input events
void
PollInputEvents(void)
{
#if defined(SUPPORT_GESTURES_SYSTEM)
  UpdateGestures();
#endif

  // Reset keys/chars pressed registered
  CORE.Input.Keyboard.keyPressedQueueCount = 0;
  CORE.Input.Keyboard.charPressedQueueCount = 0;

  // Reset key repeats
  for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;

  CORE.Input.Gamepad.lastButtonPressed = 0;

  for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];

  for (int i = 0; i < 260; i++) {
    CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
    CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;
    CORE.Input.Keyboard.currentKeyState[i] = 0;
  }

  int w = CORE.Window.screen.width, h = CORE.Window.screen.height;

  Event e;
  int key;
  while (ecanread(Emouse|Ekeyboard)) {
    key = event(&e);
    Mouse m = e.mouse;
    int mx = Clamp(m.xy.x-xr, 1, w);
    int my = Clamp(m.xy.y-yr, 1, h);
    SetMousePosition(mx, my);
    if (key == Emouse) {
      CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT] = CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_RIGHT] 
        = CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_MIDDLE] = 0;

      if (m.buttons & 1) CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT] = 1;
      if (m.buttons & 2) CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_RIGHT] = 1;
      if (m.buttons & 4) CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_MIDDLE] = 1;
    } else if (key == Ekeyboard) {
      e.kbdc = toupper(e.kbdc);
      switch (e.kbdc) {
      case 27:
        CORE.Input.Keyboard.currentKeyState[KEY_ESCAPE] = 1;
        break;
      case 61457:
        CORE.Input.Keyboard.currentKeyState[KEY_LEFT] = 1;
        break;
      case 61458:
        CORE.Input.Keyboard.currentKeyState[KEY_RIGHT] = 1;
        break;
      case 61454:
        CORE.Input.Keyboard.currentKeyState[KEY_UP] = 1;
        break;
      case 61488:
        CORE.Input.Keyboard.currentKeyState[KEY_DOWN] = 1;
        break;
      default:
        CORE.Input.Keyboard.currentKeyState[e.kbdc] = 1;
        break;
      }

      if (CORE.Input.Keyboard.currentKeyState[CORE.Input.Keyboard.exitKey])
        CORE.Window.shouldClose = 1;
    }
  }
}

static void
set_pixel(void *buf_, PFsizei index, PFcolor color)
{
  uchar* buf = buf_;
  buf[index*3]     = color.b;
  buf[(index*3)+1] = color.g;
  buf[(index*3)+2] = color.r;
}

static PFcolor
get_pixel(void *buf_, PFsizei index)
{
  uchar* buf = buf_;
  return (PFcolor) {
    buf[(index*3)+2],
    buf[(index*3)+1],
    buf[(index*3)],
    0xff
  };
}


// Initialize platform: graphics, inputs and more
int
InitPlatform(void)
{
  CORE.Window.fullscreen = false;
  CORE.Window.flags |= FLAG_FULLSCREEN_MODE;

  // TODO: This can change
  int w = CORE.Window.screen.width, h = CORE.Window.screen.height;
  int fd = open("/dev/wctl", O_RDWR);
  fprint(fd, "resize -r 10 10 %d %d\n", w+10, h+10);
  close(fd);
  pdata.rgbuf = malloc(w*h*3);

  PFcontext ctx = pfCreateContext(pdata.rgbuf, CORE.Window.screen.width, CORE.Window.screen.height, PF_PIXELFORMAT_R8G8B8A8);
  pfMakeCurrent(ctx);

  pfSetDefaultPixelGetter(get_pixel);
  pfSetDefaultPixelSetter(set_pixel);

  initdraw(nil, nil, argv0);
  einit(Emouse|Ekeyboard);

  redraw();

  CORE.Window.ready = true;

  CORE.Window.render.width = CORE.Window.screen.width;
  CORE.Window.render.height = CORE.Window.screen.height;
  CORE.Window.currentFbo.width = CORE.Window.render.width;
  CORE.Window.currentFbo.height = CORE.Window.render.height;

  CORE.Input.Mouse.offset.x = 0;
  CORE.Input.Mouse.offset.y = 0;
  CORE.Input.Mouse.scale.x  = 1;
  CORE.Input.Mouse.scale.y  = 1;

  CORE.Window.screenScale = MatrixScale(1.f, 1.f, 1.0f);
  InitTimer();

  CORE.Storage.basePath = GetWorkingDirectory();

  TRACELOG(LOG_INFO, "PLATFORM: plan9: Initialized successfully");

  return 0;
}

void
ClosePlatform(void)
{
}
