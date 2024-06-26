/**********************************************************************************************
*
*   raylib v5.1-dev - A simple and easy-to-use library to enjoy videogames programming (www.raylib.com)
*
*   FEATURES:
*       - NO external dependencies, all required libraries included with raylib
*       - Multiplatform: Windows, Linux, FreeBSD, OpenBSD, NetBSD, DragonFly,
*                        MacOS, Haiku, Android, Raspberry Pi, DRM native, HTML5.
*       - Written in plain C code (C99) in PascalCase/camelCase notation
*       - Hardware accelerated with OpenGL (1.1, 2.1, 3.3, 4.3 or ES2 - choose at compile)
*       - Unique OpenGL abstraction layer (usable as standalone module): [rlgl]
*       - Multiple GETS(Font)s formats supported (TTF, XNA fonts, AngelCode fonts)
*       - Outstanding texture formats support, including compressed formats (DXT, ETC, ASTC)
*       - Full 3d support for 3d Shapes, Models, Billboards, Heightmaps and more!
*       - Flexible GETS(Material)s system, supporting classic maps and PBR maps
*       - Animated 3D models supported (skeletal bones animation) (IQM)
*       - GETS(Shader)s support, including Model shaders and Postprocessing shaders
*       - Powerful math module for Vector, GETS(Matrix) and Quaternion operations: [raymath]
*       - Audio loading and playing with streaming support (WAV, OGG, MP3, FLAC, XM, MOD)
*       - VR stereo rendering with configurable HMD device parameters
*       - Bindings to multiple programming languages available!
*
*   NOTES:
*       - One default GETS(Font) is loaded on InitWindow()->LoadFontDefault() [core, text]
*       - One default Texture2D is loaded on rlglInit(), 1x1 white pixel R8G8B8A8 [rlgl] (OpenGL 3.3 or ES2)
*       - One default GETS(Shader) is loaded on rlglInit()->rlLoadShaderDefault() [rlgl] (OpenGL 3.3 or ES2)
*       - One default RenderBatch is loaded on rlglInit()->rlLoadRenderBatch() [rlgl] (OpenGL 3.3 or ES2)
*
*   DEPENDENCIES (included):
*       [rcore] rglfw (Camilla Löwy - github.com/glfw/glfw) for window/context management and input (PLATFORM_DESKTOP)
*       [rlgl] glad (David Herberth - github.com/Dav1dde/glad) for OpenGL 3.3 extensions loading (PLATFORM_DESKTOP)
*       [raudio] miniaudio (David Reid - github.com/mackron/miniaudio) for audio device/context management
*
*   OPTIONAL DEPENDENCIES (included):
*       [rcore] msf_gif (Miles Fogle) for GIF recording
*       [rcore] sinfl (Micha Mettke) for DEFLATE decompression algorithm
*       [rcore] sdefl (Micha Mettke) for DEFLATE compression algorithm
*       [rtextures] stb_image (Sean Barret) for images loading (BMP, TGA, PNG, JPEG, HDR...)
*       [rtextures] stb_image_write (Sean Barret) for image writing (BMP, TGA, PNG, JPG)
*       [rtextures] stb_image_resize (Sean Barret) for image resizing algorithms
*       [rtext] stb_truetype (Sean Barret) for ttf fonts loading
*       [rtext] stb_rect_pack (Sean Barret) for rectangles packing
*       [rmodels] par_shapes (Philip Rideout) for parametric 3d shapes generation
*       [rmodels] tinyobj_loader_c (Syoyo Fujita) for models loading (OBJ, MTL)
*       [rmodels] cgltf (Johannes Kuhlmann) for models loading (glTF)
*       [rmodels] Model3D (bzt) for models loading (M3D, https://bztsrc.gitlab.io/model3d)
*       [raudio] dr_wav (David Reid) for WAV audio file loading
*       [raudio] dr_flac (David Reid) for FLAC audio file loading
*       [raudio] dr_mp3 (David Reid) for MP3 audio file loading
*       [raudio] stb_vorbis (Sean Barret) for OGG audio loading
*       [raudio] jar_xm (Joshua Reisenauer) for XM audio module loading
*       [raudio] jar_mod (Joshua Reisenauer) for MOD audio module loading
*
*
*   LICENSE: zlib/libpng
*
*   raylib is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RAYLIB_H
#define RAYLIB_H

#ifndef RL_NO_STRUCT
#define MKSTRUCT(x,y) typedef struct x y x
#define GETS(x) x
#else
#define MKSTRUCT(x,y) typedef struct rl##x y rl##x
#define GETS(x) rl##x
#endif

//#include <stdarg.h>     // Required for: va_list - Only used by TraceLogCallback

#define RAYLIB_VERSION_MAJOR 5
#define RAYLIB_VERSION_MINOR 1
#define RAYLIB_VERSION_PATCH 0
#define RAYLIB_VERSION  "5.1-dev"

#ifndef RLAPI
#define RLAPI       // Functions defined as 'extern' by default (implicit specifiers)
#endif

#include "m.h"

//----------------------------------------------------------------------------------
// Some basic Defines
//----------------------------------------------------------------------------------
#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI/180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f/PI)
#endif

// Allow custom memory allocators
// NOTE: Require recompiling raylib sources
#ifndef RL_MALLOC
#define RL_MALLOC(sz)       malloc(sz)
#endif
#ifndef RL_CALLOC
#define RL_CALLOC(n,sz)     calloc(n,sz)
#endif
#ifndef RL_REALLOC
#define RL_REALLOC(ptr,sz)  realloc(ptr,sz)
#endif
#ifndef RL_FREE
#define RL_FREE(ptr)        free(ptr)
#endif

// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized with { }
// This is called aggregate initialization (C++11 feature)
#define CLITERAL(type)      (type)

// Some compilers (mostly macos clang) default to C++98,
// where aggregate initialization can't be used
// So, give a more clear error stating how to fix this

// NOTE: We set some defines with some data types declared by raylib
// Other modules (raymath, rlgl) also require some of those types, so,
// to be able to use those other modules as standalone (not depending on raylib)
// this defines are very useful for internal check and avoid type (re)definitions
#define RL_COLOR_TYPE
#define RL_RECTANGLE_TYPE
#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_QUATERNION_TYPE
#define RL_MATRIX_TYPE

MKSTRUCT(Color, {
  unsigned char r;        // GETS(Color) red value
  unsigned char g;        // GETS(Color) green value
  unsigned char b;        // GETS(Color) blue value
  unsigned char a;        // GETS(Color) alpha value
} );


// Some Basic GETS(Color)s
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
static inline GETS(Color) LIGHTGRAY  = { 200, 200, 200, 255 }   ; // Light Gray
static inline GETS(Color) GRAY       = { 130, 130, 130, 255 }   ; // Gray
static inline GETS(Color) DARKGRAY   = { 80, 80, 80, 255 }      ; // Dark Gray
static inline GETS(Color) YELLOW     = { 253, 249, 0, 255 }     ; // Yellow
static inline GETS(Color) GOLD       = { 255, 203, 0, 255 }     ; // Gold
static inline GETS(Color) ORANGE     = { 255, 161, 0, 255 }     ; // Orange
static inline GETS(Color) PINK       = { 255, 109, 194, 255 }   ; // Pink
static inline GETS(Color) RED        = { 230, 41, 55, 255 }     ; // Red
static inline GETS(Color) MAROON     = { 190, 33, 55, 255 }     ; // Maroon
static inline GETS(Color) GREEN      = { 0, 228, 48, 255 }      ; // Green
static inline GETS(Color) LIME       = { 0, 158, 47, 255 }      ; // Lime
static inline GETS(Color) DARKGREEN  = { 0, 117, 44, 255 }      ; // Dark Green
static inline GETS(Color) SKYBLUE    = { 102, 191, 255, 255 }   ; // Sky Blue
static inline GETS(Color) BLUE       = { 0, 121, 241, 255 }     ; // Blue
static inline GETS(Color) DARKBLUE   = { 0, 82, 172, 255 }      ; // Dark Blue
static inline GETS(Color) PURPLE     = { 200, 122, 255, 255 }   ; // Purple
static inline GETS(Color) VIOLET     = { 135, 60, 190, 255 }    ; // Violet
static inline GETS(Color) DARKPURPLE = { 112, 31, 126, 255 }    ; // Dark Purple
static inline GETS(Color) BEIGE      = { 211, 176, 131, 255 }   ; // Beige
static inline GETS(Color) BROWN      = { 127, 106, 79, 255 }    ; // Brown
static inline GETS(Color) DARKBROWN  = { 76, 63, 47, 255 }      ; // Dark Brown
static inline GETS(Color) WHITE      = { 255, 255, 255, 255 }   ; // White
static inline GETS(Color) BLACK      = { 0, 0, 0, 255 }         ; // Black
static inline GETS(Color) BLANK      = { 0, 0, 0, 0 }           ; // Blank (Transparent)
static inline GETS(Color) MAGENTA    = { 255, 0, 255, 255 }     ; // Magenta
static inline GETS(Color) RAYWHITE   = { 245, 245, 245, 255 }   ; // My own White (raylib logo)

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------
// Boolean type
#define true 1
#define false 0
#define bool short

// GETS(Vector2), 2 components
MKSTRUCT(Vector2, {
  float x;                // Vector x component
  float y;                // Vector y component
});

// GETS(Vector3), 3 components
MKSTRUCT(Vector3,  {
  float x;                // Vector x component
  float y;                // Vector y component
  float z;                // Vector z component
});

// GETS(Vector4), 4 components
MKSTRUCT(Vector4, {
  float x;                // Vector x component
  float y;                // Vector y component
  float z;                // Vector z component
  float w;                // Vector w component
});

// Quaternion, 4 components (GETS(Vector4) alias)
typedef GETS(Vector4) GETS(Quaternion);

// GETS(Matrix), 4x4 components, column major, OpenGL style, right-handed
MKSTRUCT(Matrix, {
  float m0;
  float m4;
  float m8;
  float m12;
  float m1;
  float m5;
  float m9;
  float m13;
  float m2;
  float m6;
  float m10;
  float m14;
  float m3;
  float m7;
  float m11;
  float m15;
});

// GETS(Color), 4 components, R8G8B8A8 (32bit)
// GETS(Rectangle), 4 components
MKSTRUCT(Rectangle, {
  float x;                // GETS(Rectangle) top-left corner position x
  float y;                // GETS(Rectangle) top-left corner position y
  float width;            // GETS(Rectangle) width
  float height;           // GETS(Rectangle) height
});

// GETS(Image), pixel data stored in CPU memory (RAM)
MKSTRUCT(Image, {
  void *data;             // GETS(Image) raw data
  int width;              // GETS(Image) base width
  int height;             // GETS(Image) base height
  int mipmaps;            // Mipmap levels, 1 by default
  int format;             // Data format (PixelFormat type)
});

// Texture, tex data stored in GPU memory (VRAM)
MKSTRUCT(Texture,  {
  unsigned int id;        // OpenGL texture id
  int width;              // Texture base width
  int height;             // Texture base height
  int mipmaps;            // Mipmap levels, 1 by default
  int format;             // Data format (PixelFormat type)
});

// Texture2D, same as Texture
typedef GETS(Texture) GETS(Texture2D);

// TextureCubemap, same as Texture
typedef GETS(Texture) GETS(TextureCubemap);

// RenderTexture, fbo for texture rendering
MKSTRUCT(RenderTexture, {
  unsigned int id;        // OpenGL framebuffer object id
  GETS(Texture) texture;        // GETS(Color) buffer attachment texture
  GETS(Texture) depth;          // Depth buffer attachment texture
});

// GETS(RenderTexture2D), same as RenderTexture
typedef GETS(RenderTexture) GETS(RenderTexture2D);

// GETS(NPatchInfo), n-patch layout info
MKSTRUCT(NPatchInfo, {
  GETS(Rectangle) source;       // Texture source rectangle
  int left;               // Left border offset
  int top;                // Top border offset
  int right;              // Right border offset
  int bottom;             // Bottom border offset
  int layout;             // Layout of the n-patch: 3x3, 1x3 or 3x1
} );

// GETS(GlyphInfo), font characters glyphs info
MKSTRUCT(GlyphInfo, {
  int value;              // Character value (Unicode)
  int offsetX;            // Character offset X when drawing
  int offsetY;            // Character offset Y when drawing
  int advanceX;           // Character advance position X
  GETS(Image) image;            // Character image data
} );

// GETS(Font), font texture and GETS(GlyphInfo) array data
MKSTRUCT(Font, {
  int baseSize;           // Base size (default chars height)
  int glyphCount;         // Number of glyph characters
  int glyphPadding;       // Padding around the glyph characters
  GETS(Texture2D) texture;      // Texture atlas containing the glyphs
  GETS(Rectangle) *recs;        // GETS(Rectangle)s in texture for the glyphs
  GETS(GlyphInfo) *glyphs;      // Glyphs info data
} );

// Camera, defines position/orientation in 3d space
MKSTRUCT(Camera3D, {
  GETS(Vector3) position;       // Camera position
  GETS(Vector3) target;         // Camera target it looks-at
  GETS(Vector3) up;             // Camera up vector (rotation over its axis)
  float fovy;             // Camera field-of-view aperture in Y (degrees) in perspective, used as near plane width in orthographic
  int projection;         // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
} );

typedef GETS(Camera3D) GETS(Camera);    // Camera type fallback, defaults to GETS(GETS(Camera3D))

// GETS(GETS(Camera2D)), defines position/orientation in 2d space
MKSTRUCT(Camera2D, {
  GETS(Vector2) offset;         // Camera offset (displacement from target)
  GETS(Vector2) target;         // Camera target (rotation and zoom origin)
  float rotation;         // Camera rotation in degrees
  float zoom;             // Camera zoom (scaling), should be 1.0f by default
} );

// GETS(Mesh), vertex data and vao/vbo
MKSTRUCT(Mesh, {
  int vertexCount;        // Number of vertices stored in arrays
  int triangleCount;      // Number of triangles stored (indexed or not)

  // Vertex attributes data
  float *vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
  float *texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
  float *texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
  float *normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
  float *tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
  unsigned char *colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
  unsigned short *indices;    // Vertex indices (in case vertex data comes indexed)

  // Animation vertex data
  float *animVertices;    // Animated vertex positions (after bones transformations)
  float *animNormals;     // Animated normals (after bones transformations)
  unsigned char *boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning)
  float *boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)

  // OpenGL identifiers
  unsigned int vaoId;     // OpenGL Vertex Array Object id
  unsigned int *vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)
} );

// GETS(Shader)
MKSTRUCT(Shader, {
  unsigned int id;        // GETS(Shader) program id
  int *locs;              // GETS(Shader) locations array (RL_MAX_SHADER_LOCATIONS)
} );

// GETS(GETS(Material)Map)
MKSTRUCT(MaterialMap, {
    GETS(Texture2D) texture;      // GETS(Material) map texture
    GETS(Color) color;            // GETS(Material) map color
  float value;            // GETS(Material) map value
} );

// GETS(Material), includes shader and maps
MKSTRUCT(Material, {
  GETS(Shader) shader;          // GETS(Material) shader
  GETS(MaterialMap) *maps;      // GETS(Material) maps array (MAX_MATERIAL_MAPS)
  float params[4];        // GETS(Material) generic parameters (if required)
} );

// GETS(Transform), vertex transformation data
MKSTRUCT(Transform, {
  GETS(Vector3) translation;    // Translation
  GETS(Quaternion) rotation;    // Rotation
  GETS(Vector3) scale;          // Scale
} );

// Bone, skeletal animation bone
MKSTRUCT(BoneInfo, {
  char name[32];          // Bone name
  int parent;             // Bone parent
} );

// Model, meshes, materials and animation data
MKSTRUCT(Model, {
  GETS(Matrix) transform;       // Local transform matrix

  int meshCount;          // Number of meshes
  int materialCount;      // Number of materials
  GETS(Mesh) *meshes;           // GETS(Mesh)es array
  GETS(Material) *materials;    // GETS(Material)s array
  int *meshMaterial;      // GETS(Mesh) material number

  // Animation data
  int boneCount;          // Number of bones
  GETS(BoneInfo) *bones;        // Bones information (skeleton)
  GETS(Transform) *bindPose;    // Bones base transformation (pose)
});

// ModelAnimation
MKSTRUCT(ModelAnimation, {
  int boneCount;          // Number of bones
  int frameCount;         // Number of animation frames
  GETS(BoneInfo) *bones;        // Bones information (skeleton)
  GETS(Transform) **framePoses; // Poses array by frame
  char name[32];          // Animation name
});

// Ray, ray for raycasting
MKSTRUCT(Ray, {
  GETS(Vector3) position;       // Ray position (origin)
  GETS(Vector3) direction;      // Ray direction
});

// GETS(RayCollision), ray hit information
MKSTRUCT(RayCollision, {
  bool hit;               // Did the ray hit something?
  float distance;         // Distance to the nearest hit
  GETS(Vector3) point;          // Point of the nearest hit
  GETS(Vector3) normal;         // Surface normal of hit
});

// GETS(BoundingBox)
MKSTRUCT(BoundingBox, {
  GETS(Vector3) min;            // Minimum vertex box-corner
  GETS(Vector3) max;            // Maximum vertex box-corner
});

// GETS(Wave), audio wave data
MKSTRUCT(Wave, {
  unsigned int frameCount;    // Total number of frames (considering channels)
  unsigned int sampleRate;    // Frequency (samples per second)
  unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
  unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
  void *data;                 // Buffer data pointer
});

// Opaque structs declaration
// NOTE: Actual structs are defined internally in raudio module
typedef struct rAudioBuffer rAudioBuffer;
typedef struct rAudioProcessor rAudioProcessor;

// GETS(AudioStream), custom audio stream
MKSTRUCT(AudioStream, {
  rAudioBuffer *buffer;       // Pointer to internal data used by the audio system
  rAudioProcessor *processor; // Pointer to internal data processor, useful for audio effects

  unsigned int sampleRate;    // Frequency (samples per second)
  unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
  unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
});

// GETS(Sound)
MKSTRUCT(Sound, {
  GETS(AudioStream) stream;         // Audio stream
  unsigned int frameCount;    // Total number of frames (considering channels)
});

// Music, audio stream, anything longer than ~10 seconds should be streamed
MKSTRUCT(Music, {
  GETS(AudioStream) stream;         // Audio stream
  unsigned int frameCount;    // Total number of frames (considering channels)
  bool looping;               // Music looping enable

  int ctxType;                // Type of music context (audio filetype)
  void *ctxData;              // Audio context data, depends on type
});

// GETS(VrDeviceInfo), Head-Mounted-Display device parameters
MKSTRUCT(VrDeviceInfo, {
  int hResolution;                // Horizontal resolution in pixels
  int vResolution;                // Vertical resolution in pixels
  float hScreenSize;              // Horizontal size in meters
  float vScreenSize;              // Vertical size in meters
  float eyeToScreenDistance;      // Distance between eye and display in meters
  float lensSeparationDistance;   // Lens separation distance in meters
  float interpupillaryDistance;   // IPD (distance between pupils) in meters
  float lensDistortionValues[4];  // Lens distortion constant parameters
  float chromaAbCorrection[4];    // Chromatic aberration correction parameters
});

// GETS(VrStereoConfig), VR stereo rendering configuration for simulator
MKSTRUCT(VrStereoConfig, {
  GETS(Matrix) projection[2];           // VR projection matrices (per eye)
  GETS(Matrix) viewOffset[2];           // VR view offset matrices (per eye)
  float leftLensCenter[2];        // VR left lens center
  float rightLensCenter[2];       // VR right lens center
  float leftScreenCenter[2];      // VR left screen center
  float rightScreenCenter[2];     // VR right screen center
  float scale[2];                 // VR distortion scale
  float scaleIn[2];               // VR distortion scale in
});

// File path list
MKSTRUCT(FilePathList, {
  unsigned int capacity;          // Filepaths max entries
  unsigned int count;             // Filepaths entries count
  char **paths;                   // Filepaths entries
} );

// Automation event
MKSTRUCT(AutomationEvent, {
  unsigned int frame;             // Event frame
  unsigned int type;              // Event type (GETS(AutomationEvent)Type)
  int params[4];                  // Event parameters (if required)
} );

// Automation event list
MKSTRUCT(AutomationEventList, {
  unsigned int capacity;          // Events max entries (MAX_AUTOMATION_EVENTS)
  unsigned int count;             // Events entries count
  GETS(AutomationEvent) *events;        // Events entries
});


//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------
// System/Window config flags
// NOTE: Every bit registers one state (use it with bit masks)
// By default all flags are set to 0
typedef enum {
  FLAG_VSYNC_HINT         = 0x00000040,   // Set to try enabling V-Sync on GPU
  FLAG_FULLSCREEN_MODE    = 0x00000002,   // Set to run program in fullscreen
  FLAG_WINDOW_RESIZABLE   = 0x00000004,   // Set to allow resizable window
  FLAG_WINDOW_UNDECORATED = 0x00000008,   // Set to disable window decoration (frame and buttons)
  FLAG_WINDOW_HIDDEN      = 0x00000080,   // Set to hide window
  FLAG_WINDOW_MINIMIZED   = 0x00000200,   // Set to minimize window (iconify)
  FLAG_WINDOW_MAXIMIZED   = 0x00000400,   // Set to maximize window (expanded to monitor)
  FLAG_WINDOW_UNFOCUSED   = 0x00000800,   // Set to window non focused
  FLAG_WINDOW_TOPMOST     = 0x00001000,   // Set to window always on top
  FLAG_WINDOW_ALWAYS_RUN  = 0x00000100,   // Set to allow windows running while minimized
  FLAG_WINDOW_TRANSPARENT = 0x00000010,   // Set to allow transparent framebuffer
  FLAG_WINDOW_HIGHDPI     = 0x00002000,   // Set to support HighDPI
  FLAG_WINDOW_MOUSE_PASSTHROUGH = 0x00004000, // Set to support mouse passthrough, only supported when FLAG_WINDOW_UNDECORATED
  FLAG_BORDERLESS_WINDOWED_MODE = 0x00008000, // Set to run program in borderless windowed mode
  FLAG_MSAA_4X_HINT       = 0x00000020,   // Set to try enabling MSAA 4X
  FLAG_INTERLACED_HINT    = 0x00010000    // Set to try enabling interlaced video format (for V3D)
} ConfigFlags;

// Trace log level
// NOTE: Organized by priority level
typedef enum {
  LOG_ALL = 0,        // Display all logs
  LOG_TRACE,          // Trace logging, intended for internal use only
  LOG_DEBUG,          // Debug logging, used for internal debugging, it should be disabled on release builds
  LOG_INFO,           // Info logging, used for program execution info
  LOG_WARNING,        // Warning logging, used on recoverable failures
  LOG_ERROR,          // Error logging, used on unrecoverable failures
  LOG_FATAL,          // Fatal logging, used to abort program: exit(EXIT_FAILURE)
  LOG_NONE            // Disable logging
} TraceLogLevel;

// Keyboard keys (US keyboard layout)
// NOTE: Use GetKeyPressed() to allow redefining
// required keys for alternative layouts
typedef enum {
  KEY_NULL            = 0,        // Key: NULL, used for no key pressed
  // Alphanumeric keys
  KEY_APOSTROPHE      = 39,       // Key: '
  KEY_COMMA           = 44,       // Key: ,
  KEY_MINUS           = 45,       // Key: -
  KEY_PERIOD          = 46,       // Key: .
  KEY_SLASH           = 47,       // Key: /
  KEY_ZERO            = 48,       // Key: 0
  KEY_ONE             = 49,       // Key: 1
  KEY_TWO             = 50,       // Key: 2
  KEY_THREE           = 51,       // Key: 3
  KEY_FOUR            = 52,       // Key: 4
  KEY_FIVE            = 53,       // Key: 5
  KEY_SIX             = 54,       // Key: 6
  KEY_SEVEN           = 55,       // Key: 7
  KEY_EIGHT           = 56,       // Key: 8
  KEY_NINE            = 57,       // Key: 9
  KEY_SEMICOLON       = 59,       // Key: ;
  KEY_EQUAL           = 61,       // Key: =
  KEY_A               = 65,       // Key: A | a
  KEY_B               = 66,       // Key: B | b
  KEY_C               = 67,       // Key: C | c
  KEY_D               = 68,       // Key: D | d
  KEY_E               = 69,       // Key: E | e
  KEY_F               = 70,       // Key: F | f
  KEY_G               = 71,       // Key: G | g
  KEY_H               = 72,       // Key: H | h
  KEY_I               = 73,       // Key: I | i
  KEY_J               = 74,       // Key: J | j
  KEY_K               = 75,       // Key: K | k
  KEY_L               = 76,       // Key: L | l
  KEY_M               = 77,       // Key: M | m
  KEY_N               = 78,       // Key: N | n
  KEY_O               = 79,       // Key: O | o
  KEY_P               = 80,       // Key: P | p
  KEY_Q               = 81,       // Key: Q | q
  KEY_R               = 82,       // Key: R | r
  KEY_S               = 83,       // Key: S | s
  KEY_T               = 84,       // Key: T | t
  KEY_U               = 85,       // Key: U | u
  KEY_V               = 86,       // Key: V | v
  KEY_W               = 87,       // Key: W | w
  KEY_X               = 88,       // Key: X | x
  KEY_Y               = 89,       // Key: Y | y
  KEY_Z               = 90,       // Key: Z | z
  KEY_LEFT_BRACKET    = 91,       // Key: [
  KEY_BACKSLASH       = 92,       // Key: '\'
  KEY_RIGHT_BRACKET   = 93,       // Key: ]
  KEY_GRAVE           = 96,       // Key: `
  // Function keys
  KEY_SPACE           = 32,       // Key: Space
  KEY_ESCAPE          = 256,      // Key: Esc
  KEY_ENTER           = 257,      // Key: Enter
  KEY_TAB             = 258,      // Key: Tab
  KEY_BACKSPACE       = 259,      // Key: Backspace
  KEY_INSERT          = 260,      // Key: Ins
  KEY_DELETE          = 261,      // Key: Del
  KEY_RIGHT           = 262,      // Key: Cursor right
  KEY_LEFT            = 263,      // Key: Cursor left
  KEY_DOWN            = 264,      // Key: Cursor down
  KEY_UP              = 265,      // Key: Cursor up
  KEY_PAGE_UP         = 266,      // Key: Page up
  KEY_PAGE_DOWN       = 267,      // Key: Page down
  KEY_HOME            = 268,      // Key: Home
  KEY_END             = 269,      // Key: End
  KEY_CAPS_LOCK       = 280,      // Key: Caps lock
  KEY_SCROLL_LOCK     = 281,      // Key: Scroll down
  KEY_NUM_LOCK        = 282,      // Key: Num lock
  KEY_PRINT_SCREEN    = 283,      // Key: Print screen
  KEY_PAUSE           = 284,      // Key: Pause
  KEY_F1              = 290,      // Key: F1
  KEY_F2              = 291,      // Key: F2
  KEY_F3              = 292,      // Key: F3
  KEY_F4              = 293,      // Key: F4
  KEY_F5              = 294,      // Key: F5
  KEY_F6              = 295,      // Key: F6
  KEY_F7              = 296,      // Key: F7
  KEY_F8              = 297,      // Key: F8
  KEY_F9              = 298,      // Key: F9
  KEY_F10             = 299,      // Key: F10
  KEY_F11             = 300,      // Key: F11
  KEY_F12             = 301,      // Key: F12
  KEY_LEFT_SHIFT      = 340,      // Key: Shift left
  KEY_LEFT_CONTROL    = 341,      // Key: Control left
  KEY_LEFT_ALT        = 342,      // Key: Alt left
  KEY_LEFT_SUPER      = 343,      // Key: Super left
  KEY_RIGHT_SHIFT     = 344,      // Key: Shift right
  KEY_RIGHT_CONTROL   = 345,      // Key: Control right
  KEY_RIGHT_ALT       = 346,      // Key: Alt right
  KEY_RIGHT_SUPER     = 347,      // Key: Super right
  KEY_KB_MENU         = 348,      // Key: KB menu
  // Keypad keys
  KEY_KP_0            = 320,      // Key: Keypad 0
  KEY_KP_1            = 321,      // Key: Keypad 1
  KEY_KP_2            = 322,      // Key: Keypad 2
  KEY_KP_3            = 323,      // Key: Keypad 3
  KEY_KP_4            = 324,      // Key: Keypad 4
  KEY_KP_5            = 325,      // Key: Keypad 5
  KEY_KP_6            = 326,      // Key: Keypad 6
  KEY_KP_7            = 327,      // Key: Keypad 7
  KEY_KP_8            = 328,      // Key: Keypad 8
  KEY_KP_9            = 329,      // Key: Keypad 9
  KEY_KP_DECIMAL      = 330,      // Key: Keypad .
  KEY_KP_DIVIDE       = 331,      // Key: Keypad /
  KEY_KP_MULTIPLY     = 332,      // Key: Keypad *
  KEY_KP_SUBTRACT     = 333,      // Key: Keypad -
  KEY_KP_ADD          = 334,      // Key: Keypad +
  KEY_KP_ENTER        = 335,      // Key: Keypad Enter
  KEY_KP_EQUAL        = 336,      // Key: Keypad =
  // Android key buttons
  KEY_BACK            = 4,        // Key: Android back button
  KEY_MENU            = 5,        // Key: Android menu button
  KEY_VOLUME_UP       = 24,       // Key: Android volume up button
  KEY_VOLUME_DOWN     = 25        // Key: Android volume down button
} KeyboardKey;

// Add backwards compatibility support for deprecated names
#define MOUSE_LEFT_BUTTON   MOUSE_BUTTON_LEFT
#define MOUSE_RIGHT_BUTTON  MOUSE_BUTTON_RIGHT
#define MOUSE_MIDDLE_BUTTON MOUSE_BUTTON_MIDDLE

// Mouse buttons
typedef enum {
  MOUSE_BUTTON_LEFT    = 0,       // Mouse button left
  MOUSE_BUTTON_RIGHT   = 1,       // Mouse button right
  MOUSE_BUTTON_MIDDLE  = 2,       // Mouse button middle (pressed wheel)
  MOUSE_BUTTON_SIDE    = 3,       // Mouse button side (advanced mouse device)
  MOUSE_BUTTON_EXTRA   = 4,       // Mouse button extra (advanced mouse device)
  MOUSE_BUTTON_FORWARD = 5,       // Mouse button forward (advanced mouse device)
  MOUSE_BUTTON_BACK    = 6,       // Mouse button back (advanced mouse device)
} MouseButton;

// Mouse cursor
typedef enum {
  MOUSE_CURSOR_DEFAULT       = 0,     // Default pointer shape
  MOUSE_CURSOR_ARROW         = 1,     // Arrow shape
  MOUSE_CURSOR_IBEAM         = 2,     // Text writing cursor shape
  MOUSE_CURSOR_CROSSHAIR     = 3,     // Cross shape
  MOUSE_CURSOR_POINTING_HAND = 4,     // Pointing hand cursor
  MOUSE_CURSOR_RESIZE_EW     = 5,     // Horizontal resize/move arrow shape
  MOUSE_CURSOR_RESIZE_NS     = 6,     // Vertical resize/move arrow shape
  MOUSE_CURSOR_RESIZE_NWSE   = 7,     // Top-left to bottom-right diagonal resize/move arrow shape
  MOUSE_CURSOR_RESIZE_NESW   = 8,     // The top-right to bottom-left diagonal resize/move arrow shape
  MOUSE_CURSOR_RESIZE_ALL    = 9,     // The omnidirectional resize/move cursor shape
  MOUSE_CURSOR_NOT_ALLOWED   = 10     // The operation-not-allowed shape
} MouseCursor;

// Gamepad buttons
typedef enum {
  GAMEPAD_BUTTON_UNKNOWN = 0,         // Unknown button, just for error checking
  GAMEPAD_BUTTON_LEFT_FACE_UP,        // Gamepad left DPAD up button
  GAMEPAD_BUTTON_LEFT_FACE_RIGHT,     // Gamepad left DPAD right button
  GAMEPAD_BUTTON_LEFT_FACE_DOWN,      // Gamepad left DPAD down button
  GAMEPAD_BUTTON_LEFT_FACE_LEFT,      // Gamepad left DPAD left button
  GAMEPAD_BUTTON_RIGHT_FACE_UP,       // Gamepad right button up (i.e. PS3: Triangle, Xbox: Y)
  GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,    // Gamepad right button right (i.e. PS3: Circle, Xbox: B)
  GAMEPAD_BUTTON_RIGHT_FACE_DOWN,     // Gamepad right button down (i.e. PS3: Cross, Xbox: A)
  GAMEPAD_BUTTON_RIGHT_FACE_LEFT,     // Gamepad right button left (i.e. PS3: Square, Xbox: X)
  GAMEPAD_BUTTON_LEFT_TRIGGER_1,      // Gamepad top/back trigger left (first), it could be a trailing button
  GAMEPAD_BUTTON_LEFT_TRIGGER_2,      // Gamepad top/back trigger left (second), it could be a trailing button
  GAMEPAD_BUTTON_RIGHT_TRIGGER_1,     // Gamepad top/back trigger right (first), it could be a trailing button
  GAMEPAD_BUTTON_RIGHT_TRIGGER_2,     // Gamepad top/back trigger right (second), it could be a trailing button
  GAMEPAD_BUTTON_MIDDLE_LEFT,         // Gamepad center buttons, left one (i.e. PS3: Select)
  GAMEPAD_BUTTON_MIDDLE,              // Gamepad center buttons, middle one (i.e. PS3: PS, Xbox: XBOX)
  GAMEPAD_BUTTON_MIDDLE_RIGHT,        // Gamepad center buttons, right one (i.e. PS3: Start)
  GAMEPAD_BUTTON_LEFT_THUMB,          // Gamepad joystick pressed button left
  GAMEPAD_BUTTON_RIGHT_THUMB          // Gamepad joystick pressed button right
} GamepadButton;

// Gamepad axis
typedef enum {
  GAMEPAD_AXIS_LEFT_X        = 0,     // Gamepad left stick X axis
  GAMEPAD_AXIS_LEFT_Y        = 1,     // Gamepad left stick Y axis
  GAMEPAD_AXIS_RIGHT_X       = 2,     // Gamepad right stick X axis
  GAMEPAD_AXIS_RIGHT_Y       = 3,     // Gamepad right stick Y axis
  GAMEPAD_AXIS_LEFT_TRIGGER  = 4,     // Gamepad back trigger left, pressure level: [1..-1]
  GAMEPAD_AXIS_RIGHT_TRIGGER = 5      // Gamepad back trigger right, pressure level: [1..-1]
} GamepadAxis;

// GETS(Material) map index
typedef enum {
  MATERIAL_MAP_ALBEDO = 0,        // Albedo material (same as: MATERIAL_MAP_DIFFUSE)
  MATERIAL_MAP_METALNESS,         // Metalness material (same as: MATERIAL_MAP_SPECULAR)
  MATERIAL_MAP_NORMAL,            // Normal material
  MATERIAL_MAP_ROUGHNESS,         // Roughness material
  MATERIAL_MAP_OCCLUSION,         // Ambient occlusion material
  MATERIAL_MAP_EMISSION,          // Emission material
  MATERIAL_MAP_HEIGHT,            // Heightmap material
  MATERIAL_MAP_CUBEMAP,           // Cubemap material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
  MATERIAL_MAP_IRRADIANCE,        // Irradiance material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
  MATERIAL_MAP_PREFILTER,         // Prefilter material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
  MATERIAL_MAP_BRDF               // Brdf material
} MaterialMapIndex;

#define MATERIAL_MAP_DIFFUSE      MATERIAL_MAP_ALBEDO
#define MATERIAL_MAP_SPECULAR     MATERIAL_MAP_METALNESS

// GETS(Shader) location index
typedef enum {
  SHADER_LOC_VERTEX_POSITION = 0, // GETS(Shader) location: vertex attribute: position
  SHADER_LOC_VERTEX_TEXCOORD01,   // GETS(Shader) location: vertex attribute: texcoord01
  SHADER_LOC_VERTEX_TEXCOORD02,   // GETS(Shader) location: vertex attribute: texcoord02
  SHADER_LOC_VERTEX_NORMAL,       // GETS(Shader) location: vertex attribute: normal
  SHADER_LOC_VERTEX_TANGENT,      // GETS(Shader) location: vertex attribute: tangent
  SHADER_LOC_VERTEX_COLOR,        // GETS(Shader) location: vertex attribute: color
  SHADER_LOC_MATRIX_MVP,          // GETS(Shader) location: matrix uniform: model-view-projection
  SHADER_LOC_MATRIX_VIEW,         // GETS(Shader) location: matrix uniform: view (camera transform)
  SHADER_LOC_MATRIX_PROJECTION,   // GETS(Shader) location: matrix uniform: projection
  SHADER_LOC_MATRIX_MODEL,        // GETS(Shader) location: matrix uniform: model (transform)
  SHADER_LOC_MATRIX_NORMAL,       // GETS(Shader) location: matrix uniform: normal
  SHADER_LOC_VECTOR_VIEW,         // GETS(Shader) location: vector uniform: view
  SHADER_LOC_COLOR_DIFFUSE,       // GETS(Shader) location: vector uniform: diffuse color
  SHADER_LOC_COLOR_SPECULAR,      // GETS(Shader) location: vector uniform: specular color
  SHADER_LOC_COLOR_AMBIENT,       // GETS(Shader) location: vector uniform: ambient color
  SHADER_LOC_MAP_ALBEDO,          // GETS(Shader) location: sampler2d texture: albedo (same as: SHADER_LOC_MAP_DIFFUSE)
  SHADER_LOC_MAP_METALNESS,       // GETS(Shader) location: sampler2d texture: metalness (same as: SHADER_LOC_MAP_SPECULAR)
  SHADER_LOC_MAP_NORMAL,          // GETS(Shader) location: sampler2d texture: normal
  SHADER_LOC_MAP_ROUGHNESS,       // GETS(Shader) location: sampler2d texture: roughness
  SHADER_LOC_MAP_OCCLUSION,       // GETS(Shader) location: sampler2d texture: occlusion
  SHADER_LOC_MAP_EMISSION,        // GETS(Shader) location: sampler2d texture: emission
  SHADER_LOC_MAP_HEIGHT,          // GETS(Shader) location: sampler2d texture: height
  SHADER_LOC_MAP_CUBEMAP,         // GETS(Shader) location: samplerCube texture: cubemap
  SHADER_LOC_MAP_IRRADIANCE,      // GETS(Shader) location: samplerCube texture: irradiance
  SHADER_LOC_MAP_PREFILTER,       // GETS(Shader) location: samplerCube texture: prefilter
  SHADER_LOC_MAP_BRDF             // GETS(Shader) location: sampler2d texture: brdf
} ShaderLocationIndex;

#define SHADER_LOC_MAP_DIFFUSE      SHADER_LOC_MAP_ALBEDO
#define SHADER_LOC_MAP_SPECULAR     SHADER_LOC_MAP_METALNESS

// GETS(Shader) uniform data type
typedef enum {
  SHADER_UNIFORM_FLOAT = 0,       // GETS(Shader) uniform type: float
  SHADER_UNIFORM_VEC2,            // GETS(Shader) uniform type: vec2 (2 float)
  SHADER_UNIFORM_VEC3,            // GETS(Shader) uniform type: vec3 (3 float)
  SHADER_UNIFORM_VEC4,            // GETS(Shader) uniform type: vec4 (4 float)
  SHADER_UNIFORM_INT,             // GETS(Shader) uniform type: int
  SHADER_UNIFORM_IVEC2,           // GETS(Shader) uniform type: ivec2 (2 int)
  SHADER_UNIFORM_IVEC3,           // GETS(Shader) uniform type: ivec3 (3 int)
  SHADER_UNIFORM_IVEC4,           // GETS(Shader) uniform type: ivec4 (4 int)
  SHADER_UNIFORM_SAMPLER2D        // GETS(Shader) uniform type: sampler2d
} ShaderUniformDataType;

// GETS(Shader) attribute data types
typedef enum {
  SHADER_ATTRIB_FLOAT = 0,        // GETS(Shader) attribute type: float
  SHADER_ATTRIB_VEC2,             // GETS(Shader) attribute type: vec2 (2 float)
  SHADER_ATTRIB_VEC3,             // GETS(Shader) attribute type: vec3 (3 float)
  SHADER_ATTRIB_VEC4              // GETS(Shader) attribute type: vec4 (4 float)
} ShaderAttributeDataType;

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum {
  PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
  PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,    // 8*2 bpp (2 channels)
  PIXELFORMAT_UNCOMPRESSED_R5G6B5,        // 16 bpp
  PIXELFORMAT_UNCOMPRESSED_R8G8B8,        // 24 bpp
  PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,      // 16 bpp (1 bit alpha)
  PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,      // 16 bpp (4 bit alpha)
  PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,      // 32 bpp
  PIXELFORMAT_UNCOMPRESSED_R32,           // 32 bpp (1 channel - float)
  PIXELFORMAT_UNCOMPRESSED_R32G32B32,     // 32*3 bpp (3 channels - float)
  PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,  // 32*4 bpp (4 channels - float)
  PIXELFORMAT_UNCOMPRESSED_R16,           // 16 bpp (1 channel - half float)
  PIXELFORMAT_UNCOMPRESSED_R16G16B16,     // 16*3 bpp (3 channels - half float)
  PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,  // 16*4 bpp (4 channels - half float)
  PIXELFORMAT_COMPRESSED_DXT1_RGB,        // 4 bpp (no alpha)
  PIXELFORMAT_COMPRESSED_DXT1_RGBA,       // 4 bpp (1 bit alpha)
  PIXELFORMAT_COMPRESSED_DXT3_RGBA,       // 8 bpp
  PIXELFORMAT_COMPRESSED_DXT5_RGBA,       // 8 bpp
  PIXELFORMAT_COMPRESSED_ETC1_RGB,        // 4 bpp
  PIXELFORMAT_COMPRESSED_ETC2_RGB,        // 4 bpp
  PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,   // 8 bpp
  PIXELFORMAT_COMPRESSED_PVRT_RGB,        // 4 bpp
  PIXELFORMAT_COMPRESSED_PVRT_RGBA,       // 4 bpp
  PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,   // 8 bpp
  PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA    // 2 bpp
} PixelFormat;

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum {
  TEXTURE_FILTER_POINT = 0,               // No filter, just pixel approximation
  TEXTURE_FILTER_BILINEAR,                // Linear filtering
  TEXTURE_FILTER_TRILINEAR,               // Trilinear filtering (linear with mipmaps)
  TEXTURE_FILTER_ANISOTROPIC_4X,          // Anisotropic filtering 4x
  TEXTURE_FILTER_ANISOTROPIC_8X,          // Anisotropic filtering 8x
  TEXTURE_FILTER_ANISOTROPIC_16X,         // Anisotropic filtering 16x
} TextureFilter;

// Texture parameters: wrap mode
typedef enum {
  TEXTURE_WRAP_REPEAT = 0,                // Repeats texture in tiled mode
  TEXTURE_WRAP_CLAMP,                     // Clamps texture to edge pixel in tiled mode
  TEXTURE_WRAP_MIRROR_REPEAT,             // Mirrors and repeats the texture in tiled mode
  TEXTURE_WRAP_MIRROR_CLAMP               // Mirrors and clamps to border the texture in tiled mode
} TextureWrap;

// Cubemap layouts
typedef enum {
  CUBEMAP_LAYOUT_AUTO_DETECT = 0,         // Automatically detect layout type
  CUBEMAP_LAYOUT_LINE_VERTICAL,           // Layout is defined by a vertical line with faces
  CUBEMAP_LAYOUT_LINE_HORIZONTAL,         // Layout is defined by a horizontal line with faces
  CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR,     // Layout is defined by a 3x4 cross with cubemap faces
  CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE,     // Layout is defined by a 4x3 cross with cubemap faces
  CUBEMAP_LAYOUT_PANORAMA                 // Layout is defined by a panorama image (equirrectangular map)
} CubemapLayout;

// GETS(Font) type, defines generation method
typedef enum {
  FONT_DEFAULT = 0,               // Default font generation, anti-aliased
  FONT_BITMAP,                    // Bitmap font generation, no anti-aliasing
  FONT_SDF                        // SDF font generation, requires external shader
} FontType;

// GETS(Color) blending modes (pre-defined)
typedef enum {
  BLEND_ALPHA = 0,                // Blend textures considering alpha (default)
  BLEND_ADDITIVE,                 // Blend textures adding colors
  BLEND_MULTIPLIED,               // Blend textures multiplying colors
  BLEND_ADD_COLORS,               // Blend textures adding colors (alternative)
  BLEND_SUBTRACT_COLORS,          // Blend textures subtracting colors (alternative)
  BLEND_ALPHA_PREMULTIPLY,        // Blend premultiplied textures considering alpha
  BLEND_CUSTOM,                   // Blend textures using custom src/dst factors (use rlSetBlendFactors())
  BLEND_CUSTOM_SEPARATE           // Blend textures using custom rgb/alpha separate src/dst factors (use rlSetBlendFactorsSeparate())
} BlendMode;

// Gesture
// NOTE: Provided as bit-wise flags to enable only desired gestures
typedef enum {
  GESTURE_NONE        = 0,        // No gesture
  GESTURE_TAP         = 1,        // Tap gesture
  GESTURE_DOUBLETAP   = 2,        // Double tap gesture
  GESTURE_HOLD        = 4,        // Hold gesture
  GESTURE_DRAG        = 8,        // Drag gesture
  GESTURE_SWIPE_RIGHT = 16,       // Swipe right gesture
  GESTURE_SWIPE_LEFT  = 32,       // Swipe left gesture
  GESTURE_SWIPE_UP    = 64,       // Swipe up gesture
  GESTURE_SWIPE_DOWN  = 128,      // Swipe down gesture
  GESTURE_PINCH_IN    = 256,      // Pinch in gesture
  GESTURE_PINCH_OUT   = 512       // Pinch out gesture
} Gesture;

// Camera system modes
typedef enum {
  CAMERA_CUSTOM = 0,              // Camera custom, controlled by user (UpdateCamera() does nothing)
  CAMERA_FREE,                    // Camera free mode
  CAMERA_ORBITAL,                 // Camera orbital, around target, zoom supported
  CAMERA_FIRST_PERSON,            // Camera first person
  CAMERA_THIRD_PERSON             // Camera third person
} CameraMode;

// Camera projection
typedef enum {
  CAMERA_PERSPECTIVE = 0,         // Perspective projection
  CAMERA_ORTHOGRAPHIC             // Orthographic projection
} CameraProjection;

// N-patch layout
typedef enum {
  NPATCH_NINE_PATCH = 0,          // Npatch layout: 3x3 tiles
  NPATCH_THREE_PATCH_VERTICAL,    // Npatch layout: 1x3 tiles
  NPATCH_THREE_PATCH_HORIZONTAL   // Npatch layout: 3x1 tiles
} NPatchLayout;

// Callbacks to hook some internal functions
// WARNING: These callbacks are intended for advanced users
typedef void (*TraceLogCallback)(int logLevel, const char *text, ...);  // Logging: Redirect trace log messages
typedef unsigned char *(*LoadFileDataCallback)(const char *fileName, int *dataSize);    // FileIO: Load binary data
typedef bool (*SaveFileDataCallback)(const char *fileName, void *data, int dataSize);   // FileIO: Save binary data
typedef char *(*LoadFileTextCallback)(const char *fileName);            // FileIO: Load text data
typedef bool (*SaveFileTextCallback)(const char *fileName, char *text); // FileIO: Save text data

//------------------------------------------------------------------------------------
// Global Variables Definition
//------------------------------------------------------------------------------------
// It's lonely here...

//------------------------------------------------------------------------------------
// Window and Graphics Device Functions (Module: core)
//------------------------------------------------------------------------------------

// Window-related functions
RLAPI void InitWindow(int width, int height, const char *title);  // Initialize window and OpenGL context
RLAPI void CloseWindow(void);                                     // Close window and unload OpenGL context
RLAPI bool WindowShouldClose(void);                               // Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
RLAPI bool IsWindowReady(void);                                   // Check if window has been initialized successfully
RLAPI bool IsWindowFullscreen(void);                              // Check if window is currently fullscreen
RLAPI bool IsWindowHidden(void);                                  // Check if window is currently hidden (only PLATFORM_DESKTOP)
RLAPI bool IsWindowMinimized(void);                               // Check if window is currently minimized (only PLATFORM_DESKTOP)
RLAPI bool IsWindowMaximized(void);                               // Check if window is currently maximized (only PLATFORM_DESKTOP)
RLAPI bool IsWindowFocused(void);                                 // Check if window is currently focused (only PLATFORM_DESKTOP)
RLAPI bool IsWindowResized(void);                                 // Check if window has been resized last frame
RLAPI bool IsWindowState(unsigned int flag);                      // Check if one specific window flag is enabled
RLAPI void SetWindowState(unsigned int flags);                    // Set window configuration state using flags (only PLATFORM_DESKTOP)
RLAPI void ClearWindowState(unsigned int flags);                  // Clear window configuration state flags
RLAPI void ToggleFullscreen(void);                                // Toggle window state: fullscreen/windowed (only PLATFORM_DESKTOP)
RLAPI void ToggleBorderlessWindowed(void);                        // Toggle window state: borderless windowed (only PLATFORM_DESKTOP)
RLAPI void MaximizeWindow(void);                                  // Set window state: maximized, if resizable (only PLATFORM_DESKTOP)
RLAPI void MinimizeWindow(void);                                  // Set window state: minimized, if resizable (only PLATFORM_DESKTOP)
RLAPI void RestoreWindow(void);                                   // Set window state: not minimized/maximized (only PLATFORM_DESKTOP)
RLAPI void SetWindowIcon(GETS(Image) image);                            // Set icon for window (single image, RGBA 32bit, only PLATFORM_DESKTOP)
RLAPI void SetWindowIcons(GETS(Image) *images, int count);              // Set icon for window (multiple images, RGBA 32bit, only PLATFORM_DESKTOP)
RLAPI void SetWindowTitle(const char *title);                     // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
RLAPI void SetWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
RLAPI void SetWindowMonitor(int monitor);                         // Set monitor for the current window
RLAPI void SetWindowMinSize(int width, int height);               // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
RLAPI void SetWindowMaxSize(int width, int height);               // Set window maximum dimensions (for FLAG_WINDOW_RESIZABLE)
RLAPI void SetWindowSize(int width, int height);                  // Set window dimensions
RLAPI void SetWindowOpacity(float opacity);                       // Set window opacity [0.0f..1.0f] (only PLATFORM_DESKTOP)
RLAPI void SetWindowFocused(void);                                // Set window focused (only PLATFORM_DESKTOP)
RLAPI void *GetWindowHandle(void);                                // Get native window handle
RLAPI int GetScreenWidth(void);                                   // Get current screen width
RLAPI int GetScreenHeight(void);                                  // Get current screen height
RLAPI int GetRenderWidth(void);                                   // Get current render width (it considers HiDPI)
RLAPI int GetRenderHeight(void);                                  // Get current render height (it considers HiDPI)
RLAPI int GetMonitorCount(void);                                  // Get number of connected monitors
RLAPI int GetCurrentMonitor(void);                                // Get current connected monitor
RLAPI GETS(Vector2) GetMonitorPosition(int monitor);                    // Get specified monitor position
RLAPI int GetMonitorWidth(int monitor);                           // Get specified monitor width (current video mode used by monitor)
RLAPI int GetMonitorHeight(int monitor);                          // Get specified monitor height (current video mode used by monitor)
RLAPI int GetMonitorPhysicalWidth(int monitor);                   // Get specified monitor physical width in millimetres
RLAPI int GetMonitorPhysicalHeight(int monitor);                  // Get specified monitor physical height in millimetres
RLAPI int GetMonitorRefreshRate(int monitor);                     // Get specified monitor refresh rate
RLAPI GETS(Vector2) GetWindowPosition(void);                            // Get window position XY on monitor
RLAPI GETS(Vector2) GetWindowScaleDPI(void);                            // Get window scale DPI factor
RLAPI const char *GetMonitorName(int monitor);                    // Get the human-readable, UTF-8 encoded name of the specified monitor
RLAPI void SetClipboardText(const char *text);                    // Set clipboard text content
RLAPI const char *GetClipboardText(void);                         // Get clipboard text content
RLAPI void EnableEventWaiting(void);                              // Enable waiting for events on EndDrawing(), no automatic event polling
RLAPI void DisableEventWaiting(void);                             // Disable waiting for events on EndDrawing(), automatic events polling

// Cursor-related functions
RLAPI void ShowCursor(void);                                      // Shows cursor
RLAPI void HideCursor(void);                                      // Hides cursor
RLAPI bool IsCursorHidden(void);                                  // Check if cursor is not visible
RLAPI void EnableCursor(void);                                    // Enables cursor (unlock cursor)
RLAPI void DisableCursor(void);                                   // Disables cursor (lock cursor)
RLAPI bool IsCursorOnScreen(void);                                // Check if cursor is on the screen

// Drawing-related functions
  RLAPI void ClearBackground(GETS(Color) color);                          // Set background color (framebuffer clear color)
RLAPI void BeginDrawing(void);                                    // Setup canvas (framebuffer) to start drawing
RLAPI void EndDrawing(void);                                      // End canvas drawing and swap buffers (double buffering)
RLAPI void BeginMode2D(GETS(Camera2D) camera);                          // Begin 2D mode with custom camera (2D)
RLAPI void EndMode2D(void);                                       // Ends 2D mode with custom camera
RLAPI void BeginMode3D(GETS(Camera3D) camera);                          // Begin 3D mode with custom camera (3D)
RLAPI void EndMode3D(void);                                       // Ends 3D mode and returns to default 2D orthographic mode
RLAPI void BeginTextureMode(GETS(RenderTexture2D) target);              // Begin drawing to render texture
RLAPI void EndTextureMode(void);                                  // Ends drawing to render texture
RLAPI void BeginShaderMode(GETS(Shader) shader);                        // Begin custom shader drawing
RLAPI void EndShaderMode(void);                                   // End custom shader drawing (use default shader)
RLAPI void BeginBlendMode(int mode);                              // Begin blending mode (alpha, additive, multiplied, subtract, custom)
RLAPI void EndBlendMode(void);                                    // End blending mode (reset to default: alpha blending)
RLAPI void BeginScissorMode(int x, int y, int width, int height); // Begin scissor mode (define screen area for following drawing)
RLAPI void EndScissorMode(void);                                  // End scissor mode
RLAPI void BeginVrStereoMode(GETS(VrStereoConfig) config);              // Begin stereo rendering (requires VR simulator)
RLAPI void EndVrStereoMode(void);                                 // End stereo rendering (requires VR simulator)

// VR stereo config functions for VR simulator
RLAPI GETS(VrStereoConfig) LoadVrStereoConfig(GETS(VrDeviceInfo) device);     // Load VR stereo config for VR simulator device parameters
RLAPI void UnloadVrStereoConfig(GETS(VrStereoConfig) config);           // Unload VR stereo config

// GETS(Shader) management functions
// NOTE: GETS(Shader) functionality is not available on OpenGL 1.1
RLAPI GETS(Shader) LoadShader(const char *vsFileName, const char *fsFileName);   // Load shader from files and bind default locations
RLAPI GETS(Shader) LoadShaderFromMemory(const char *vsCode, const char *fsCode); // Load shader from code strings and bind default locations
RLAPI bool IsShaderReady(GETS(Shader) shader);                                   // Check if a shader is ready
RLAPI int GetShaderLocation(GETS(Shader) shader, const char *uniformName);       // Get shader uniform location
RLAPI int GetShaderLocationAttrib(GETS(Shader) shader, const char *attribName);  // Get shader attribute location
RLAPI void SetShaderValue(GETS(Shader) shader, int locIndex, const void *value, int uniformType);               // Set shader uniform value
RLAPI void SetShaderValueV(GETS(Shader) shader, int locIndex, const void *value, int uniformType, int count);   // Set shader uniform value vector
RLAPI void SetShaderValueMatrix(GETS(Shader) shader, int locIndex, GETS(Matrix) mat);         // Set shader uniform value (matrix 4x4)
RLAPI void SetShaderValueTexture(GETS(Shader) shader, int locIndex, GETS(Texture2D) texture); // Set shader uniform value for texture (sampler2d)
RLAPI void UnloadShader(GETS(Shader) shader);                                    // Unload shader from GPU memory (VRAM)

// Screen-space-related functions
#define GetMouseRay GetScreenToWorldRay     // Compatibility hack for previous raylib versions
  RLAPI GETS(Ray) GetScreenToWorldRay(GETS(Vector2) position, GETS(Camera) camera);         // Get a ray trace from screen position (i.e mouse)
  RLAPI GETS(Ray) GetScreenToWorldRayEx(GETS(Vector2) position, GETS(Camera) camera, int width, int height); // Get a ray trace from screen position (i.e mouse) in a viewport
  RLAPI GETS(Vector2) GetWorldToScreen(GETS(Vector3) position, GETS(Camera) camera);        // Get the screen space position for a 3d world space position
  RLAPI GETS(Vector2) GetWorldToScreenEx(GETS(Vector3) position, GETS(Camera) camera, int width, int height); // Get size position for a 3d world space position
RLAPI GETS(Vector2) GetWorldToScreen2D(GETS(Vector2) position, GETS(Camera2D) camera);    // Get the screen space position for a 2d camera world space position
RLAPI GETS(Vector2) GetScreenToWorld2D(GETS(Vector2) position, GETS(Camera2D) camera);    // Get the world space position for a 2d camera screen space position
  RLAPI GETS(Matrix) GetCameraMatrix(GETS(Camera) camera);                            // Get camera transform matrix (view matrix)
RLAPI GETS(Matrix) GetCameraMatrix2D(GETS(Camera2D) camera);                        // Get camera 2d transform matrix

// Timing-related functions
RLAPI void SetTargetFPS(int fps);                                 // Set target FPS (maximum)
RLAPI float GetFrameTime(void);                                   // Get time in seconds for last frame drawn (delta time)
RLAPI double GetTime(void);                                       // Get elapsed time in seconds since InitWindow()
RLAPI int GetFPS(void);                                           // Get current FPS

// Custom frame control functions
// NOTE: Those functions are intended for advanced users that want full control over the frame processing
// By default EndDrawing() does this job: draws everything + SwapScreenBuffer() + manage frame timing + PollInputEvents()
// To avoid that behaviour and control frame processes manually, enable in config.h: SUPPORT_CUSTOM_FRAME_CONTROL
RLAPI void SwapScreenBuffer(void);                                // Swap back buffer with front buffer (screen drawing)
RLAPI void PollInputEvents(void);                                 // Register all input events
RLAPI void WaitTime(double seconds);                              // Wait for some time (halt program execution)

// Random values generation functions
RLAPI void SetRandomSeed(unsigned int seed);                      // Set the seed for the random number generator
RLAPI int GetRandomValue(int min, int max);                       // Get a random value between min and max (both included)
RLAPI int *LoadRandomSequence(unsigned int count, int min, int max); // Load random values sequence, no values repeated
RLAPI void UnloadRandomSequence(int *sequence);                   // Unload random values sequence

// Misc. functions
RLAPI void TakeScreenshot(const char *fileName);                  // Takes a screenshot of current screen (filename extension defines format)
RLAPI void SetConfigFlags(unsigned int flags);                    // Setup init configuration flags (view FLAGS)
RLAPI void OpenURL(const char *url);                              // Open URL with default system browser (if available)

// NOTE: Following functions implemented in module [utils]
//------------------------------------------------------------------
RLAPI void TraceLog(int logLevel, const char *text, ...);         // Show trace log messages (LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR...)
RLAPI void SetTraceLogLevel(int logLevel);                        // Set the current threshold (minimum) log level
RLAPI void *MemAlloc(unsigned int size);                          // Internal memory allocator
RLAPI void *MemRealloc(void *ptr, unsigned int size);             // Internal memory reallocator
RLAPI void MemFree(void *ptr);                                    // Internal memory free

// Set custom callbacks
// WARNING: Callbacks setup is intended for advanced users
RLAPI void SetTraceLogCallback(TraceLogCallback callback);         // Set custom trace log
RLAPI void SetLoadFileDataCallback(LoadFileDataCallback callback); // Set custom file binary data loader
RLAPI void SetSaveFileDataCallback(SaveFileDataCallback callback); // Set custom file binary data saver
RLAPI void SetLoadFileTextCallback(LoadFileTextCallback callback); // Set custom file text data loader
RLAPI void SetSaveFileTextCallback(SaveFileTextCallback callback); // Set custom file text data saver

// Files management functions
RLAPI unsigned char *LoadFileData(const char *fileName, int *dataSize); // Load file data as byte array (read)
RLAPI void UnloadFileData(unsigned char *data);                   // Unload file data allocated by LoadFileData()
RLAPI bool SaveFileData(const char *fileName, void *data, int dataSize); // Save data to file from byte array (write), returns true on success
RLAPI bool ExportDataAsCode(const unsigned char *data, int dataSize, const char *fileName); // Export data to code (.h), returns true on success
RLAPI char *LoadFileText(const char *fileName);                   // Load text data from file (read), returns a '\0' terminated string
RLAPI void UnloadFileText(char *text);                            // Unload file text data allocated by LoadFileText()
RLAPI bool SaveFileText(const char *fileName, char *text);        // Save text data to file (write), string must be '\0' terminated, returns true on success
//------------------------------------------------------------------

// File system functions
RLAPI bool FileExists(const char *fileName);                      // Check if file exists
RLAPI bool DirectoryExists(const char *dirPath);                  // Check if a directory path exists
RLAPI bool IsFileExtension(const char *fileName, const char *ext); // Check file extension (including point: .png, .wav)
RLAPI int GetFileLength(const char *fileName);                    // Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
RLAPI const char *GetFileExtension(const char *fileName);         // Get pointer to extension for a filename string (includes dot: '.png')
RLAPI const char *GetFileName(const char *filePath);              // Get pointer to filename for a path string
RLAPI const char *GetFileNameWithoutExt(const char *filePath);    // Get filename string without extension (uses static string)
RLAPI const char *GetDirectoryPath(const char *filePath);         // Get full path for a given fileName with path (uses static string)
RLAPI const char *GetPrevDirectoryPath(const char *dirPath);      // Get previous directory path for a given path (uses static string)
RLAPI const char *GetWorkingDirectory(void);                      // Get current working directory (uses static string)
RLAPI const char *GetApplicationDirectory(void);                  // Get the directory of the running application (uses static string)
RLAPI bool ChangeDirectory(const char *dir);                      // Change working directory, return true on success
RLAPI bool IsPathFile(const char *path);                          // Check if a given path is a file or a directory
RLAPI bool IsFileNameValid(const char *fileName);                 // Check if fileName is valid for the platform/OS
  RLAPI GETS(FilePathList) LoadDirectoryFiles(const char *dirPath);       // Load directory filepaths
  RLAPI GETS(FilePathList) LoadDirectoryFilesEx(const char *basePath, const char *filter, bool scanSubdirs); // Load directory filepaths with extension filtering and recursive directory scan
  RLAPI void UnloadDirectoryFiles(GETS(FilePathList) files);              // Unload filepaths
RLAPI bool IsFileDropped(void);                                   // Check if a file has been dropped into window
  RLAPI GETS(FilePathList) LoadDroppedFiles(void);                        // Load dropped filepaths
  RLAPI void UnloadDroppedFiles(GETS(FilePathList) files);                // Unload dropped filepaths
RLAPI long GetFileModTime(const char *fileName);                  // Get file modification time (last write time)

// Compression/Encoding functionality
RLAPI unsigned char *CompressData(const unsigned char *data, int dataSize, int *compDataSize);        // Compress data (DEFLATE algorithm), memory must be MemFree()
RLAPI unsigned char *DecompressData(const unsigned char *compData, int compDataSize, int *dataSize);  // Decompress data (DEFLATE algorithm), memory must be MemFree()
RLAPI char *EncodeDataBase64(const unsigned char *data, int dataSize, int *outputSize);               // Encode data to Base64 string, memory must be MemFree()
RLAPI unsigned char *DecodeDataBase64(const unsigned char *data, int *outputSize);                    // Decode Base64 string data, memory must be MemFree()

// Automation events functionality
  RLAPI GETS(AutomationEventList) LoadAutomationEventList(const char *fileName);                // Load automation events list from file, NULL for empty list, capacity = MAX_AUTOMATION_EVENTS
  RLAPI void UnloadAutomationEventList(GETS(AutomationEventList) list);                         // Unload automation events list from file
  RLAPI bool ExportAutomationEventList(GETS(AutomationEventList) list, const char *fileName);   // Export automation events list as text file
  RLAPI void SetAutomationEventList(GETS(AutomationEventList) *list);                           // Set automation event list to record to
RLAPI void SetAutomationEventBaseFrame(int frame);                                      // Set automation event internal base frame to start recording
RLAPI void StartAutomationEventRecording(void);                                         // Start recording automation events (GETS(AutomationEvent)List must be set)
RLAPI void StopAutomationEventRecording(void);                                          // Stop recording automation events
RLAPI void PlayAutomationEvent(GETS(AutomationEvent) event);                                  // Play a recorded automation event

//------------------------------------------------------------------------------------
// Input Handling Functions (Module: core)
//------------------------------------------------------------------------------------

// Input-related functions: keyboard
RLAPI bool IsKeyPressed(int key);                             // Check if a key has been pressed once
RLAPI bool IsKeyPressedRepeat(int key);                       // Check if a key has been pressed again (Only PLATFORM_DESKTOP)
RLAPI bool IsKeyDown(int key);                                // Check if a key is being pressed
RLAPI bool IsKeyReleased(int key);                            // Check if a key has been released once
RLAPI bool IsKeyUp(int key);                                  // Check if a key is NOT being pressed
RLAPI int GetKeyPressed(void);                                // Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty
RLAPI int GetCharPressed(void);                               // Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty
RLAPI void SetExitKey(int key);                               // Set a custom key to exit program (default is ESC)

// Input-related functions: gamepads
RLAPI bool IsGamepadAvailable(int gamepad);                                        // Check if a gamepad is available
RLAPI const char *GetGamepadName(int gamepad);                                     // Get gamepad internal name id
RLAPI bool IsGamepadButtonPressed(int gamepad, int button);                        // Check if a gamepad button has been pressed once
RLAPI bool IsGamepadButtonDown(int gamepad, int button);                           // Check if a gamepad button is being pressed
RLAPI bool IsGamepadButtonReleased(int gamepad, int button);                       // Check if a gamepad button has been released once
RLAPI bool IsGamepadButtonUp(int gamepad, int button);                             // Check if a gamepad button is NOT being pressed
RLAPI int GetGamepadButtonPressed(void);                                           // Get the last gamepad button pressed
RLAPI int GetGamepadAxisCount(int gamepad);                                        // Get gamepad axis count for a gamepad
RLAPI float GetGamepadAxisMovement(int gamepad, int axis);                         // Get axis movement value for a gamepad axis
RLAPI int SetGamepadMappings(const char *mappings);                                // Set internal gamepad mappings (SDL_GameControllerDB)
RLAPI void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor);    // Set gamepad vibration for both motors

// Input-related functions: mouse
RLAPI bool IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
RLAPI bool IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
RLAPI bool IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
RLAPI bool IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed
RLAPI int GetMouseX(void);                                    // Get mouse position X
RLAPI int GetMouseY(void);                                    // Get mouse position Y
RLAPI GETS(Vector2) GetMousePosition(void);                         // Get mouse position XY
RLAPI GETS(Vector2) GetMouseDelta(void);                            // Get mouse delta between frames
RLAPI void SetMousePosition(int x, int y);                    // Set mouse position XY
RLAPI void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
RLAPI void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
RLAPI float GetMouseWheelMove(void);                          // Get mouse wheel movement for X or Y, whichever is larger
RLAPI GETS(Vector2) GetMouseWheelMoveV(void);                       // Get mouse wheel movement for both X and Y
RLAPI void SetMouseCursor(int cursor);                        // Set mouse cursor

// Input-related functions: touch
RLAPI int GetTouchX(void);                                    // Get touch position X for touch point 0 (relative to screen size)
RLAPI int GetTouchY(void);                                    // Get touch position Y for touch point 0 (relative to screen size)
RLAPI GETS(Vector2) GetTouchPosition(int index);                    // Get touch position XY for a touch point index (relative to screen size)
RLAPI int GetTouchPointId(int index);                         // Get touch point identifier for given index
RLAPI int GetTouchPointCount(void);                           // Get number of touch points

//------------------------------------------------------------------------------------
// Gestures and Touch Handling Functions (Module: rgestures)
//------------------------------------------------------------------------------------
RLAPI void SetGesturesEnabled(unsigned int flags);      // Enable a set of gestures using flags
RLAPI bool IsGestureDetected(unsigned int gesture);     // Check if a gesture have been detected
RLAPI int GetGestureDetected(void);                     // Get latest detected gesture
RLAPI float GetGestureHoldDuration(void);               // Get gesture hold time in milliseconds
RLAPI GETS(Vector2) GetGestureDragVector(void);               // Get gesture drag vector
RLAPI float GetGestureDragAngle(void);                  // Get gesture drag angle
RLAPI GETS(Vector2) GetGesturePinchVector(void);              // Get gesture pinch delta
RLAPI float GetGesturePinchAngle(void);                 // Get gesture pinch angle

//------------------------------------------------------------------------------------
// Camera System Functions (Module: rcamera)
//------------------------------------------------------------------------------------
  RLAPI void UpdateCamera(GETS(Camera) *camera, int mode);      // Update camera position for selected mode
  RLAPI void UpdateCameraPro(GETS(Camera) *camera, GETS(Vector3) movement, GETS(Vector3) rotation, float zoom); // Update camera movement/rotation

//------------------------------------------------------------------------------------
// Basic Shapes Drawing Functions (Module: shapes)
//------------------------------------------------------------------------------------
// Set texture and rectangle to be used on shapes drawing
// NOTE: It can be useful when using basic shapes and one single font,
// defining a font char white rectangle would allow drawing everything in a single draw call
  RLAPI void SetShapesTexture(GETS(Texture2D) texture, GETS(Rectangle) source);       // Set texture and rectangle to be used on shapes drawing
  RLAPI GETS(Texture2D) GetShapesTexture(void);                                 // Get texture that is used for shapes drawing
RLAPI GETS(Rectangle) GetShapesTextureRectangle(void);                        // Get texture source rectangle that is used for shapes drawing

// Basic shapes drawing functions
RLAPI void DrawPixel(int posX, int posY, GETS(Color) color);                                                   // Draw a pixel
RLAPI void DrawPixelV(GETS(Vector2) position, GETS(Color) color);                                                    // Draw a pixel (Vector version)
RLAPI void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, GETS(Color) color);                // Draw a line
RLAPI void DrawLineV(GETS(Vector2) startPos, GETS(Vector2) endPos, GETS(Color) color);                                     // Draw a line (using gl lines)
RLAPI void DrawLineEx(GETS(Vector2) startPos, GETS(Vector2) endPos, float thick, GETS(Color) color);                       // Draw a line (using triangles/quads)
RLAPI void DrawLineStrip(const GETS(Vector2) *points, int pointCount, GETS(Color) color);                            // Draw lines sequence (using gl lines)
RLAPI void DrawLineBezier(GETS(Vector2) startPos, GETS(Vector2) endPos, float thick, GETS(Color) color);                   // Draw line segment cubic-bezier in-out interpolation
RLAPI void DrawCircle(int centerX, int centerY, float radius, GETS(Color) color);                              // Draw a color-filled circle
RLAPI void DrawCircleSector(GETS(Vector2) center, float radius, float startAngle, float endAngle, int segments, GETS(Color) color);      // Draw a piece of a circle
RLAPI void DrawCircleSectorLines(GETS(Vector2) center, float radius, float startAngle, float endAngle, int segments, GETS(Color) color); // Draw circle sector outline
RLAPI void DrawCircleGradient(int centerX, int centerY, float radius, GETS(Color) color1, GETS(Color) color2);       // Draw a gradient-filled circle
RLAPI void DrawCircleV(GETS(Vector2) center, float radius, GETS(Color) color);                                       // Draw a color-filled circle (Vector version)
RLAPI void DrawCircleLines(int centerX, int centerY, float radius, GETS(Color) color);                         // Draw circle outline
RLAPI void DrawCircleLinesV(GETS(Vector2) center, float radius, GETS(Color) color);                                  // Draw circle outline (Vector version)
RLAPI void DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, GETS(Color) color);             // Draw ellipse
RLAPI void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, GETS(Color) color);        // Draw ellipse outline
RLAPI void DrawRing(GETS(Vector2) center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, GETS(Color) color); // Draw ring
RLAPI void DrawRingLines(GETS(Vector2) center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, GETS(Color) color);    // Draw ring outline
RLAPI void DrawRectangle(int posX, int posY, int width, int height, GETS(Color) color);                        // Draw a color-filled rectangle
RLAPI void DrawRectangleV(GETS(Vector2) position, GETS(Vector2) size, GETS(Color) color);                                  // Draw a color-filled rectangle (Vector version)
RLAPI void DrawRectangleRec(GETS(Rectangle) rec, GETS(Color) color);                                                 // Draw a color-filled rectangle
RLAPI void DrawRectanglePro(GETS(Rectangle) rec, GETS(Vector2) origin, float rotation, GETS(Color) color);                 // Draw a color-filled rectangle with pro parameters
RLAPI void DrawRectangleGradientV(int posX, int posY, int width, int height, GETS(Color) color1, GETS(Color) color2);// Draw a vertical-gradient-filled rectangle
RLAPI void DrawRectangleGradientH(int posX, int posY, int width, int height, GETS(Color) color1, GETS(Color) color2);// Draw a horizontal-gradient-filled rectangle
RLAPI void DrawRectangleGradientEx(GETS(Rectangle) rec, GETS(Color) col1, GETS(Color) col2, GETS(Color) col3, GETS(Color) col4);       // Draw a gradient-filled rectangle with custom vertex colors
RLAPI void DrawRectangleLines(int posX, int posY, int width, int height, GETS(Color) color);                   // Draw rectangle outline
RLAPI void DrawRectangleLinesEx(GETS(Rectangle) rec, float lineThick, GETS(Color) color);                            // Draw rectangle outline with extended parameters
RLAPI void DrawRectangleRounded(GETS(Rectangle) rec, float roundness, int segments, GETS(Color) color);              // Draw rectangle with rounded edges
RLAPI void DrawRectangleRoundedLines(GETS(Rectangle) rec, float roundness, int segments, GETS(Color) color);         // Draw rectangle lines with rounded edges
RLAPI void DrawRectangleRoundedLinesEx(GETS(Rectangle) rec, float roundness, int segments, float lineThick, GETS(Color) color); // Draw rectangle with rounded edges outline
RLAPI void DrawTriangle(GETS(Vector2) v1, GETS(Vector2) v2, GETS(Vector2) v3, GETS(Color) color);                                // Draw a color-filled triangle (vertex in counter-clockwise order!)
RLAPI void DrawTriangleLines(GETS(Vector2) v1, GETS(Vector2) v2, GETS(Vector2) v3, GETS(Color) color);                           // Draw triangle outline (vertex in counter-clockwise order!)
RLAPI void DrawTriangleFan(const GETS(Vector2) *points, int pointCount, GETS(Color) color);                          // Draw a triangle fan defined by points (first vertex is the center)
RLAPI void DrawTriangleStrip(const GETS(Vector2) *points, int pointCount, GETS(Color) color);                        // Draw a triangle strip defined by points
RLAPI void DrawPoly(GETS(Vector2) center, int sides, float radius, float rotation, GETS(Color) color);               // Draw a regular polygon (Vector version)
RLAPI void DrawPolyLines(GETS(Vector2) center, int sides, float radius, float rotation, GETS(Color) color);          // Draw a polygon outline of n sides
RLAPI void DrawPolyLinesEx(GETS(Vector2) center, int sides, float radius, float rotation, float lineThick, GETS(Color) color); // Draw a polygon outline of n sides with extended parameters

// Splines drawing functions
RLAPI void DrawSplineLinear(const GETS(Vector2) *points, int pointCount, float thick, GETS(Color) color);                  // Draw spline: Linear, minimum 2 points
RLAPI void DrawSplineBasis(const GETS(Vector2) *points, int pointCount, float thick, GETS(Color) color);                   // Draw spline: B-Spline, minimum 4 points
RLAPI void DrawSplineCatmullRom(const GETS(Vector2) *points, int pointCount, float thick, GETS(Color) color);              // Draw spline: Catmull-Rom, minimum 4 points
RLAPI void DrawSplineBezierQuadratic(const GETS(Vector2) *points, int pointCount, float thick, GETS(Color) color);         // Draw spline: Quadratic Bezier, minimum 3 points (1 control point): [p1, c2, p3, c4...]
RLAPI void DrawSplineBezierCubic(const GETS(Vector2) *points, int pointCount, float thick, GETS(Color) color);             // Draw spline: Cubic Bezier, minimum 4 points (2 control points): [p1, c2, c3, p4, c5, c6...]
RLAPI void DrawSplineSegmentLinear(GETS(Vector2) p1, GETS(Vector2) p2, float thick, GETS(Color) color);                    // Draw spline segment: Linear, 2 points
RLAPI void DrawSplineSegmentBasis(GETS(Vector2) p1, GETS(Vector2) p2, GETS(Vector2) p3, GETS(Vector2) p4, float thick, GETS(Color) color); // Draw spline segment: B-Spline, 4 points
RLAPI void DrawSplineSegmentCatmullRom(GETS(Vector2) p1, GETS(Vector2) p2, GETS(Vector2) p3, GETS(Vector2) p4, float thick, GETS(Color) color); // Draw spline segment: Catmull-Rom, 4 points
RLAPI void DrawSplineSegmentBezierQuadratic(GETS(Vector2) p1, GETS(Vector2) c2, GETS(Vector2) p3, float thick, GETS(Color) color); // Draw spline segment: Quadratic Bezier, 2 points, 1 control point
RLAPI void DrawSplineSegmentBezierCubic(GETS(Vector2) p1, GETS(Vector2) c2, GETS(Vector2) c3, GETS(Vector2) p4, float thick, GETS(Color) color); // Draw spline segment: Cubic Bezier, 2 points, 2 control points

// Spline segment point evaluation functions, for a given t [0.0f .. 1.0f]
RLAPI GETS(Vector2) GetSplinePointLinear(GETS(Vector2) startPos, GETS(Vector2) endPos, float t);                           // Get (evaluate) spline point: Linear
RLAPI GETS(Vector2) GetSplinePointBasis(GETS(Vector2) p1, GETS(Vector2) p2, GETS(Vector2) p3, GETS(Vector2) p4, float t);              // Get (evaluate) spline point: B-Spline
RLAPI GETS(Vector2) GetSplinePointCatmullRom(GETS(Vector2) p1, GETS(Vector2) p2, GETS(Vector2) p3, GETS(Vector2) p4, float t);         // Get (evaluate) spline point: Catmull-Rom
RLAPI GETS(Vector2) GetSplinePointBezierQuad(GETS(Vector2) p1, GETS(Vector2) c2, GETS(Vector2) p3, float t);                     // Get (evaluate) spline point: Quadratic Bezier
RLAPI GETS(Vector2) GetSplinePointBezierCubic(GETS(Vector2) p1, GETS(Vector2) c2, GETS(Vector2) c3, GETS(Vector2) p4, float t);        // Get (evaluate) spline point: Cubic Bezier

// Basic shapes collision detection functions
RLAPI bool CheckCollisionRecs(GETS(Rectangle) rec1, GETS(Rectangle) rec2);                                           // Check collision between two rectangles
RLAPI bool CheckCollisionCircles(GETS(Vector2) center1, float radius1, GETS(Vector2) center2, float radius2);        // Check collision between two circles
RLAPI bool CheckCollisionCircleRec(GETS(Vector2) center, float radius, GETS(Rectangle) rec);                         // Check collision between circle and rectangle
RLAPI bool CheckCollisionPointRec(GETS(Vector2) point, GETS(Rectangle) rec);                                         // Check if point is inside rectangle
RLAPI bool CheckCollisionPointCircle(GETS(Vector2) point, GETS(Vector2) center, float radius);                       // Check if point is inside circle
RLAPI bool CheckCollisionPointTriangle(GETS(Vector2) point, GETS(Vector2) p1, GETS(Vector2) p2, GETS(Vector2) p3);               // Check if point is inside a triangle
RLAPI bool CheckCollisionPointPoly(GETS(Vector2) point, const GETS(Vector2) *points, int pointCount);                // Check if point is within a polygon described by array of vertices
RLAPI bool CheckCollisionLines(GETS(Vector2) startPos1, GETS(Vector2) endPos1, GETS(Vector2) startPos2, GETS(Vector2) endPos2, GETS(Vector2) *collisionPoint); // Check the collision between two lines defined by two points each, returns collision point by reference
RLAPI bool CheckCollisionPointLine(GETS(Vector2) point, GETS(Vector2) p1, GETS(Vector2) p2, int threshold);                // Check if point belongs to line created between two points [p1] and [p2] with defined margin in pixels [threshold]
RLAPI bool CheckCollisionCircleLine(GETS(Vector2) center, float radius, GETS(Vector2) p1, GETS(Vector2) p2);               // Check if circle collides with a line created betweeen two points [p1] and [p2]
RLAPI GETS(Rectangle) GetCollisionRec(GETS(Rectangle) rec1, GETS(Rectangle) rec2);                                         // Get collision rectangle for two rectangles collision

//------------------------------------------------------------------------------------
// Texture Loading and Drawing Functions (Module: textures)
//------------------------------------------------------------------------------------

// GETS(Image) loading functions
// NOTE: These functions do not require GPU access
RLAPI GETS(Image) LoadImage(const char *fileName);                                                             // Load image from file into CPU memory (RAM)
RLAPI GETS(Image) LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize);       // Load image from RAW file data
RLAPI GETS(Image) LoadImageSvg(const char *fileNameOrString, int width, int height);                           // Load image from SVG file data or string with specified size
RLAPI GETS(Image) LoadImageAnim(const char *fileName, int *frames);                                            // Load image sequence from file (frames appended to image.data)
RLAPI GETS(Image) LoadImageAnimFromMemory(const char *fileType, const unsigned char *fileData, int dataSize, int *frames); // Load image sequence from memory buffer
RLAPI GETS(Image) LoadImageFromMemory(const char *fileType, const unsigned char *fileData, int dataSize);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
  RLAPI GETS(Image) LoadImageFromTexture(GETS(Texture2D) texture);                                                     // Load image from GPU texture data
RLAPI GETS(Image) LoadImageFromScreen(void);                                                                   // Load image from screen buffer and (screenshot)
RLAPI bool IsImageReady(GETS(Image) image);                                                                    // Check if an image is ready
RLAPI void UnloadImage(GETS(Image) image);                                                                     // Unload image from CPU memory (RAM)
RLAPI bool ExportImage(GETS(Image) image, const char *fileName);                                               // Export image data to file, returns true on success
RLAPI unsigned char *ExportImageToMemory(GETS(Image) image, const char *fileType, int *fileSize);              // Export image to memory buffer
RLAPI bool ExportImageAsCode(GETS(Image) image, const char *fileName);                                         // Export image as code file defining an array of bytes, returns true on success

// GETS(Image) generation functions
RLAPI GETS(Image) GenImageColor(int width, int height, GETS(Color) color);                                           // Generate image: plain color
RLAPI GETS(Image) GenImageGradientLinear(int width, int height, int direction, GETS(Color) start, GETS(Color) end);        // Generate image: linear gradient, direction in degrees [0..360], 0=Vertical gradient
RLAPI GETS(Image) GenImageGradientRadial(int width, int height, float density, GETS(Color) inner, GETS(Color) outer);      // Generate image: radial gradient
RLAPI GETS(Image) GenImageGradientSquare(int width, int height, float density, GETS(Color) inner, GETS(Color) outer);      // Generate image: square gradient
RLAPI GETS(Image) GenImageChecked(int width, int height, int checksX, int checksY, GETS(Color) col1, GETS(Color) col2);    // Generate image: checked
RLAPI GETS(Image) GenImageWhiteNoise(int width, int height, float factor);                                     // Generate image: white noise
RLAPI GETS(Image) GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);           // Generate image: perlin noise
RLAPI GETS(Image) GenImageCellular(int width, int height, int tileSize);                                       // Generate image: cellular algorithm, bigger tileSize means bigger cells
RLAPI GETS(Image) GenImageText(int width, int height, const char *text);                                       // Generate image: grayscale image from text data

// GETS(Image) manipulation functions
RLAPI GETS(Image) ImageCopy(GETS(Image) image);                                                                      // Create an image duplicate (useful for transformations)
RLAPI GETS(Image) ImageFromImage(GETS(Image) image, GETS(Rectangle) rec);                                                  // Create an image from another image piece
RLAPI GETS(Image) ImageText(const char *text, int fontSize, GETS(Color) color);                                      // Create an image from text (default font)
RLAPI GETS(Image) ImageTextEx(GETS(Font) font, const char *text, float fontSize, float spacing, GETS(Color) tint);         // Create an image from text (custom sprite font)
RLAPI void ImageFormat(GETS(Image) *image, int newFormat);                                                     // Convert image data to desired format
RLAPI void ImageToPOT(GETS(Image) *image, GETS(Color) fill);                                                         // Convert image to POT (power-of-two)
RLAPI void ImageCrop(GETS(Image) *image, GETS(Rectangle) crop);                                                      // Crop an image to a defined rectangle
RLAPI void ImageAlphaCrop(GETS(Image) *image, float threshold);                                                // Crop image depending on alpha value
RLAPI void ImageAlphaClear(GETS(Image) *image, GETS(Color) color, float threshold);                                  // Clear alpha channel to desired color
RLAPI void ImageAlphaMask(GETS(Image) *image, GETS(Image) alphaMask);                                                // Apply alpha mask to image
RLAPI void ImageAlphaPremultiply(GETS(Image) *image);                                                          // Premultiply alpha channel
RLAPI void ImageBlurGaussian(GETS(Image) *image, int blurSize);                                                // Apply Gaussian blur using a box blur approximation
RLAPI void ImageKernelConvolution(GETS(Image) *image, float *kernel, int kernelSize);                          // Apply Custom Square image convolution kernel
RLAPI void ImageResize(GETS(Image) *image, int newWidth, int newHeight);                                       // Resize image (Bicubic scaling algorithm)
RLAPI void ImageResizeNN(GETS(Image) *image, int newWidth,int newHeight);                                      // Resize image (Nearest-Neighbor scaling algorithm)
RLAPI void ImageResizeCanvas(GETS(Image) *image, int newWidth, int newHeight, int offsetX, int offsetY, GETS(Color) fill); // Resize canvas and fill with color
RLAPI void ImageMipmaps(GETS(Image) *image);                                                                   // Compute all mipmap levels for a provided image
RLAPI void ImageDither(GETS(Image) *image, int rBpp, int gBpp, int bBpp, int aBpp);                            // Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
RLAPI void ImageFlipVertical(GETS(Image) *image);                                                              // Flip image vertically
RLAPI void ImageFlipHorizontal(GETS(Image) *image);                                                            // Flip image horizontally
RLAPI void ImageRotate(GETS(Image) *image, int degrees);                                                       // Rotate image by input angle in degrees (-359 to 359)
RLAPI void ImageRotateCW(GETS(Image) *image);                                                                  // Rotate image clockwise 90deg
RLAPI void ImageRotateCCW(GETS(Image) *image);                                                                 // Rotate image counter-clockwise 90deg
RLAPI void ImageColorTint(GETS(Image) *image, GETS(Color) color);                                                    // Modify image color: tint
RLAPI void ImageColorInvert(GETS(Image) *image);                                                               // Modify image color: invert
RLAPI void ImageColorGrayscale(GETS(Image) *image);                                                            // Modify image color: grayscale
RLAPI void ImageColorContrast(GETS(Image) *image, float contrast);                                             // Modify image color: contrast (-100 to 100)
RLAPI void ImageColorBrightness(GETS(Image) *image, int brightness);                                           // Modify image color: brightness (-255 to 255)
RLAPI void ImageColorReplace(GETS(Image) *image, GETS(Color) color, GETS(Color) replace);                                  // Modify image color: replace color
RLAPI GETS(Color) *LoadImageColors(GETS(Image) image);                                                               // Load color data from image as a GETS(Color) array (RGBA - 32bit)
RLAPI GETS(Color) *LoadImagePalette(GETS(Image) image, int maxPaletteSize, int *colorCount);                         // Load colors palette from image as a GETS(Color) array (RGBA - 32bit)
RLAPI void UnloadImageColors(GETS(Color) *colors);                                                             // Unload color data loaded with LoadGETS(Image)Colors()
RLAPI void UnloadImagePalette(GETS(Color) *colors);                                                            // Unload colors palette loaded with LoadGETS(Image)Palette()
RLAPI GETS(Rectangle) GetImageAlphaBorder(GETS(Image) image, float threshold);                                       // Get image alpha border rectangle
RLAPI GETS(Color) GetImageColor(GETS(Image) image, int x, int y);                                                    // Get image pixel color at (x, y) position

// GETS(Image) drawing functions
// NOTE: GETS(Image) software-rendering functions (CPU)
RLAPI void ImageClearBackground(GETS(Image) *dst, GETS(Color) color);                                                // Clear image background with given color
RLAPI void ImageDrawPixel(GETS(Image) *dst, int posX, int posY, GETS(Color) color);                                  // Draw pixel within an image
RLAPI void ImageDrawPixelV(GETS(Image) *dst, GETS(Vector2) position, GETS(Color) color);                                   // Draw pixel within an image (Vector version)
RLAPI void ImageDrawLine(GETS(Image) *dst, int startPosX, int startPosY, int endPosX, int endPosY, GETS(Color) color); // Draw line within an image
RLAPI void ImageDrawLineV(GETS(Image) *dst, GETS(Vector2) start, GETS(Vector2) end, GETS(Color) color);                          // Draw line within an image (Vector version)
RLAPI void ImageDrawLineEx(GETS(Image) *dst, GETS(Vector2) start, GETS(Vector2) end, int thick, GETS(Color) color);              // Draw a line defining thickness within an image
RLAPI void ImageDrawCircle(GETS(Image) *dst, int centerX, int centerY, int radius, GETS(Color) color);               // Draw a filled circle within an image
RLAPI void ImageDrawCircleV(GETS(Image) *dst, GETS(Vector2) center, int radius, GETS(Color) color);                        // Draw a filled circle within an image (Vector version)
RLAPI void ImageDrawCircleLines(GETS(Image) *dst, int centerX, int centerY, int radius, GETS(Color) color);          // Draw circle outline within an image
RLAPI void ImageDrawCircleLinesV(GETS(Image) *dst, GETS(Vector2) center, int radius, GETS(Color) color);                   // Draw circle outline within an image (Vector version)
RLAPI void ImageDrawRectangle(GETS(Image) *dst, int posX, int posY, int width, int height, GETS(Color) color);       // Draw rectangle within an image
RLAPI void ImageDrawRectangleV(GETS(Image) *dst, GETS(Vector2) position, GETS(Vector2) size, GETS(Color) color);                 // Draw rectangle within an image (Vector version)
RLAPI void ImageDrawRectangleRec(GETS(Image) *dst, GETS(Rectangle) rec, GETS(Color) color);                                // Draw rectangle within an image
RLAPI void ImageDrawRectangleLines(GETS(Image) *dst, GETS(Rectangle) rec, int thick, GETS(Color) color);                   // Draw rectangle lines within an image
RLAPI void ImageDrawTriangle(GETS(Image) *dst, GETS(Vector2) v1, GETS(Vector2) v2, GETS(Vector2) v3, GETS(Color) color);               // Draw triangle within an image
RLAPI void ImageDrawTriangleEx(GETS(Image) *dst, GETS(Vector2) v1, GETS(Vector2) v2, GETS(Vector2) v3, GETS(Color) c1, GETS(Color) c2, GETS(Color) c3); // Draw triangle with interpolated colors within an image
RLAPI void ImageDrawTriangleLines(GETS(Image) *dst, GETS(Vector2) v1, GETS(Vector2) v2, GETS(Vector2) v3, GETS(Color) color);          // Draw triangle outline within an image
RLAPI void ImageDrawTriangleFan(GETS(Image) *dst, GETS(Vector2) *points, int pointCount, GETS(Color) color);               // Draw a triangle fan defined by points within an image (first vertex is the center)
RLAPI void ImageDrawTriangleStrip(GETS(Image) *dst, GETS(Vector2) *points, int pointCount, GETS(Color) color);             // Draw a triangle strip defined by points within an image
RLAPI void ImageDraw(GETS(Image) *dst, GETS(Image) src, GETS(Rectangle) srcRec, GETS(Rectangle) dstRec, GETS(Color) tint);             // Draw a source image within a destination image (tint applied to source)
RLAPI void ImageDrawText(GETS(Image) *dst, const char *text, int posX, int posY, int fontSize, GETS(Color) color);   // Draw text (using default font) within an image (destination)
RLAPI void ImageDrawTextEx(GETS(Image) *dst, GETS(Font) font, const char *text, GETS(Vector2) position, float fontSize, float spacing, GETS(Color) tint); // Draw text (custom sprite font) within an image (destination)

// Texture loading functions
// NOTE: These functions require GPU access
  RLAPI GETS(Texture2D) LoadTexture(const char *fileName);                                                       // Load texture from file into GPU memory (VRAM)
  RLAPI GETS(Texture2D) LoadTextureFromImage(GETS(Image) image);                                                       // Load texture from image data
  RLAPI GETS(TextureCubemap) LoadTextureCubemap(GETS(Image) image, int layout);                                        // Load cubemap from image, multiple image cubemap layouts supported
RLAPI GETS(RenderTexture2D) LoadRenderTexture(int width, int height);                                          // Load texture for rendering (framebuffer)
  RLAPI bool IsTextureReady(GETS(Texture2D) texture);                                                            // Check if a texture is ready
  RLAPI void UnloadTexture(GETS(Texture2D) texture);                                                             // Unload texture from GPU memory (VRAM)
RLAPI bool IsRenderTextureReady(GETS(RenderTexture2D) target);                                                 // Check if a render texture is ready
RLAPI void UnloadRenderTexture(GETS(RenderTexture2D) target);                                                  // Unload render texture from GPU memory (VRAM)
  RLAPI void UpdateTexture(GETS(Texture2D) texture, const void *pixels);                                         // Update GPU texture with new data
  RLAPI void UpdateTextureRec(GETS(Texture2D) texture, GETS(Rectangle) rec, const void *pixels);                       // Update GPU texture rectangle with new data

// Texture configuration functions
  RLAPI void GenTextureMipmaps(GETS(Texture2D) *texture);                                                        // Generate GPU mipmaps for a texture
  RLAPI void SetTextureFilter(GETS(Texture2D) texture, int filter);                                              // Set texture scaling filter mode
  RLAPI void SetTextureWrap(GETS(Texture2D) texture, int wrap);                                                  // Set texture wrapping mode

// Texture drawing functions
  RLAPI void DrawTexture(GETS(Texture2D) texture, int posX, int posY, GETS(Color) tint);                               // Draw a Texture2D
  RLAPI void DrawTextureV(GETS(Texture2D) texture, GETS(Vector2) position, GETS(Color) tint);                                // Draw a Texture2D with position defined as GETS(Vector2)
  RLAPI void DrawTextureEx(GETS(Texture2D) texture, GETS(Vector2) position, float rotation, float scale, GETS(Color) tint);  // Draw a Texture2D with extended parameters
  RLAPI void DrawTextureRec(GETS(Texture2D) texture, GETS(Rectangle) source, GETS(Vector2) position, GETS(Color) tint);            // Draw a part of a texture defined by a rectangle
  RLAPI void DrawTexturePro(GETS(Texture2D) texture, GETS(Rectangle) source, GETS(Rectangle) dest, GETS(Vector2) origin, float rotation, GETS(Color) tint); // Draw a part of a texture defined by a rectangle with 'pro' parameters
  RLAPI void DrawTextureNPatch(GETS(Texture2D) texture, GETS(NPatchInfo) nPatchInfo, GETS(Rectangle) dest, GETS(Vector2) origin, float rotation, GETS(Color) tint); // Draws a texture (or part of it) that stretches or shrinks nicely

// GETS(Color)/pixel related functions
RLAPI bool ColorIsEqual(GETS(Color) col1, GETS(Color) col2);                            // Check if two colors are equal
RLAPI GETS(Color) Fade(GETS(Color) color, float alpha);                                 // Get color with alpha applied, alpha goes from 0.0f to 1.0f
RLAPI int ColorToInt(GETS(Color) color);                                          // Get hexadecimal value for a GETS(Color) (0xRRGGBBAA)
RLAPI GETS(Vector4) ColorNormalize(GETS(Color) color);                                  // Get GETS(Color) normalized as float [0..1]
RLAPI GETS(Color) ColorFromNormalized(GETS(Vector4) normalized);                        // Get GETS(Color) from normalized values [0..1]
RLAPI GETS(Vector3) ColorToHSV(GETS(Color) color);                                      // Get HSV values for a GETS(Color), hue [0..360], saturation/value [0..1]
RLAPI GETS(Color) ColorFromHSV(float hue, float saturation, float value);         // Get a GETS(Color) from HSV values, hue [0..360], saturation/value [0..1]
RLAPI GETS(Color) ColorTint(GETS(Color) color, GETS(Color) tint);                             // Get color multiplied with another color
RLAPI GETS(Color) ColorBrightness(GETS(Color) color, float factor);                     // Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
RLAPI GETS(Color) ColorContrast(GETS(Color) color, float contrast);                     // Get color with contrast correction, contrast values between -1.0f and 1.0f
RLAPI GETS(Color) ColorAlpha(GETS(Color) color, float alpha);                           // Get color with alpha applied, alpha goes from 0.0f to 1.0f
RLAPI GETS(Color) ColorAlphaBlend(GETS(Color) dst, GETS(Color) src, GETS(Color) tint);              // Get src alpha-blended into dst color with tint
RLAPI GETS(Color) GetColor(unsigned int hexValue);                                // Get GETS(Color) structure from hexadecimal value
RLAPI GETS(Color) GetPixelColor(void *srcPtr, int format);                        // Get GETS(Color) from a source pixel pointer of certain format
RLAPI void SetPixelColor(void *dstPtr, GETS(Color) color, int format);            // Set color formatted into destination pixel pointer
RLAPI int GetPixelDataSize(int width, int height, int format);              // Get pixel data size in bytes for certain format

//------------------------------------------------------------------------------------
// GETS(Font) Loading and Text Drawing Functions (Module: text)
//------------------------------------------------------------------------------------

// GETS(Font) loading/unloading functions
RLAPI GETS(Font) GetFontDefault(void);                                                            // Get the default GETS(Font)
RLAPI GETS(Font) LoadFont(const char *fileName);                                                  // Load font from file into GPU memory (VRAM)
RLAPI GETS(Font) LoadFontEx(const char *fileName, int fontSize, int *codepoints, int codepointCount); // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set
RLAPI GETS(Font) LoadFontFromImage(GETS(Image) image, GETS(Color) key, int firstChar);                        // Load font from GETS(Image) (XNA style)
RLAPI GETS(Font) LoadFontFromMemory(const char *fileType, const unsigned char *fileData, int dataSize, int fontSize, int *codepoints, int codepointCount); // Load font from memory buffer, fileType refers to extension: i.e. '.ttf'
RLAPI bool IsFontReady(GETS(Font) font);                                                          // Check if a font is ready
RLAPI GETS(GlyphInfo) *LoadFontData(const unsigned char *fileData, int dataSize, int fontSize, int *codepoints, int codepointCount, int type); // Load font data for further use
RLAPI GETS(Image) GenImageFontAtlas(const GETS(GlyphInfo) *glyphs, GETS(Rectangle) **glyphRecs, int glyphCount, int fontSize, int padding, int packMethod); // Generate image font atlas using chars info
RLAPI void UnloadFontData(GETS(GlyphInfo) *glyphs, int glyphCount);                               // Unload font chars info data (RAM)
RLAPI void UnloadFont(GETS(Font) font);                                                           // Unload font from GPU memory (VRAM)
RLAPI bool ExportFontAsCode(GETS(Font) font, const char *fileName);                               // Export font as code file, returns true on success

// Text drawing functions
RLAPI void DrawFPS(int posX, int posY);                                                     // Draw current FPS
RLAPI void DrawText(const char *text, int posX, int posY, int fontSize, GETS(Color) color);       // Draw text (using default font)
RLAPI void DrawTextEx(GETS(Font) font, const char *text, GETS(Vector2) position, float fontSize, float spacing, GETS(Color) tint); // Draw text using font and additional parameters
RLAPI void DrawTextPro(GETS(Font) font, const char *text, GETS(Vector2) position, GETS(Vector2) origin, float rotation, float fontSize, float spacing, GETS(Color) tint); // Draw text using GETS(Font) and pro parameters (rotation)
RLAPI void DrawTextCodepoint(GETS(Font) font, int codepoint, GETS(Vector2) position, float fontSize, GETS(Color) tint); // Draw one character (codepoint)
RLAPI void DrawTextCodepoints(GETS(Font) font, const int *codepoints, int codepointCount, GETS(Vector2) position, float fontSize, float spacing, GETS(Color) tint); // Draw multiple character (codepoint)

// Text font info functions
RLAPI void SetTextLineSpacing(int spacing);                                                 // Set vertical line spacing when drawing with line-breaks
RLAPI int MeasureText(const char *text, int fontSize);                                      // Measure string width for default font
RLAPI GETS(Vector2) MeasureTextEx(GETS(Font) font, const char *text, float fontSize, float spacing);    // Measure string size for GETS(Font)
RLAPI int GetGlyphIndex(GETS(Font) font, int codepoint);                                          // Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
RLAPI GETS(GlyphInfo) GetGlyphInfo(GETS(Font) font, int codepoint);                                     // Get glyph font info data for a codepoint (unicode character), fallback to '?' if not found
RLAPI GETS(Rectangle) GetGlyphAtlasRec(GETS(Font) font, int codepoint);                                 // Get glyph rectangle in font atlas for a codepoint (unicode character), fallback to '?' if not found

// Text codepoints management functions (unicode characters)
RLAPI char *LoadUTF8(const int *codepoints, int length);                // Load UTF-8 text encoded from codepoints array
RLAPI void UnloadUTF8(char *text);                                      // Unload UTF-8 text encoded from codepoints array
RLAPI int *LoadCodepoints(const char *text, int *count);                // Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
RLAPI void UnloadCodepoints(int *codepoints);                           // Unload codepoints data from memory
RLAPI int GetCodepointCount(const char *text);                          // Get total number of codepoints in a UTF-8 encoded string
RLAPI int GetCodepoint(const char *text, int *codepointSize);           // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI int GetCodepointNext(const char *text, int *codepointSize);       // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI int GetCodepointPrevious(const char *text, int *codepointSize);   // Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI const char *CodepointToUTF8(int codepoint, int *utf8Size);        // Encode one codepoint into UTF-8 byte array (array length returned as parameter)

// Text strings management functions (no UTF-8 strings, only byte chars)
// NOTE: Some strings allocate memory internally for returned strings, just be careful!
RLAPI int TextCopy(char *dst, const char *src);                                             // Copy one string to another, returns bytes copied
RLAPI bool TextIsEqual(const char *text1, const char *text2);                               // Check if two text string are equal
RLAPI unsigned int TextLength(const char *text);                                            // Get text length, checks for '\0' ending
RLAPI const char *TextFormat(const char *text, ...);                                        // Text formatting with variables (sprintf() style)
RLAPI const char *TextSubtext(const char *text, int position, int length);                  // Get a piece of a text string
RLAPI char *TextReplace(const char *text, const char *replace, const char *by);             // Replace text string (WARNING: memory must be freed!)
RLAPI char *TextInsert(const char *text, const char *insert, int position);                 // Insert text in a position (WARNING: memory must be freed!)
RLAPI const char *TextJoin(const char **textList, int count, const char *delimiter);        // Join text strings with delimiter
RLAPI const char **TextSplit(const char *text, char delimiter, int *count);                 // Split text into multiple strings
RLAPI void TextAppend(char *text, const char *append, int *position);                       // Append text at specific position and move cursor!
RLAPI int TextFindIndex(const char *text, const char *find);                                // Find first text occurrence within a string
RLAPI const char *TextToUpper(const char *text);                      // Get upper case version of provided string
RLAPI const char *TextToLower(const char *text);                      // Get lower case version of provided string
RLAPI const char *TextToPascal(const char *text);                     // Get Pascal case notation version of provided string
RLAPI const char *TextToSnake(const char *text);                      // Get Snake case notation version of provided string
RLAPI const char *TextToCamel(const char *text);                      // Get Camel case notation version of provided string

RLAPI int TextToInteger(const char *text);                            // Get integer value from text (negative values not supported)
RLAPI float TextToFloat(const char *text);                            // Get float value from text (negative values not supported)

//------------------------------------------------------------------------------------
// Basic 3d Shapes Drawing Functions (Module: models)
//------------------------------------------------------------------------------------

// Basic geometric 3D shapes drawing functions
RLAPI void DrawLine3D(GETS(Vector3) startPos, GETS(Vector3) endPos, GETS(Color) color);                                    // Draw a line in 3D world space
RLAPI void DrawPoint3D(GETS(Vector3) position, GETS(Color) color);                                                   // Draw a point in 3D space, actually a small line
RLAPI void DrawCircle3D(GETS(Vector3) center, float radius, GETS(Vector3) rotationAxis, float rotationAngle, GETS(Color) color); // Draw a circle in 3D world space
RLAPI void DrawTriangle3D(GETS(Vector3) v1, GETS(Vector3) v2, GETS(Vector3) v3, GETS(Color) color);                              // Draw a color-filled triangle (vertex in counter-clockwise order!)
RLAPI void DrawTriangleStrip3D(const GETS(Vector3) *points, int pointCount, GETS(Color) color);                      // Draw a triangle strip defined by points
RLAPI void DrawCube(GETS(Vector3) position, float width, float height, float length, GETS(Color) color);             // Draw cube
RLAPI void DrawCubeV(GETS(Vector3) position, GETS(Vector3) size, GETS(Color) color);                                       // Draw cube (Vector version)
RLAPI void DrawCubeWires(GETS(Vector3) position, float width, float height, float length, GETS(Color) color);        // Draw cube wires
RLAPI void DrawCubeWiresV(GETS(Vector3) position, GETS(Vector3) size, GETS(Color) color);                                  // Draw cube wires (Vector version)
RLAPI void DrawSphere(GETS(Vector3) centerPos, float radius, GETS(Color) color);                                     // Draw sphere
RLAPI void DrawSphereEx(GETS(Vector3) centerPos, float radius, int rings, int slices, GETS(Color) color);            // Draw sphere with extended parameters
RLAPI void DrawSphereWires(GETS(Vector3) centerPos, float radius, int rings, int slices, GETS(Color) color);         // Draw sphere wires
RLAPI void DrawCylinder(GETS(Vector3) position, float radiusTop, float radiusBottom, float height, int slices, GETS(Color) color); // Draw a cylinder/cone
RLAPI void DrawCylinderEx(GETS(Vector3) startPos, GETS(Vector3) endPos, float startRadius, float endRadius, int sides, GETS(Color) color); // Draw a cylinder with base at startPos and top at endPos
RLAPI void DrawCylinderWires(GETS(Vector3) position, float radiusTop, float radiusBottom, float height, int slices, GETS(Color) color); // Draw a cylinder/cone wires
RLAPI void DrawCylinderWiresEx(GETS(Vector3) startPos, GETS(Vector3) endPos, float startRadius, float endRadius, int sides, GETS(Color) color); // Draw a cylinder wires with base at startPos and top at endPos
RLAPI void DrawCapsule(GETS(Vector3) startPos, GETS(Vector3) endPos, float radius, int slices, int rings, GETS(Color) color); // Draw a capsule with the center of its sphere caps at startPos and endPos
RLAPI void DrawCapsuleWires(GETS(Vector3) startPos, GETS(Vector3) endPos, float radius, int slices, int rings, GETS(Color) color); // Draw capsule wireframe with the center of its sphere caps at startPos and endPos
RLAPI void DrawPlane(GETS(Vector3) centerPos, GETS(Vector2) size, GETS(Color) color);                                      // Draw a plane XZ
  RLAPI void DrawRay(GETS(Ray) ray, GETS(Color) color);                                                                // Draw a ray line
RLAPI void DrawGrid(int slices, float spacing);                                                          // Draw a grid (centered at (0, 0, 0))

//------------------------------------------------------------------------------------
// Model 3d Loading and Drawing Functions (Module: models)
//------------------------------------------------------------------------------------

// Model management functions
  RLAPI GETS(Model) LoadModel(const char *fileName);                                                // Load model from files (meshes and materials)
  RLAPI GETS(Model) LoadModelFromMesh(GETS(Mesh) mesh);                                                   // Load model from generated mesh (default material)
  RLAPI bool IsModelReady(GETS(Model) model);                                                       // Check if a model is ready
  RLAPI void UnloadModel(GETS(Model) model);                                                        // Unload model (including meshes) from memory (RAM and/or VRAM)
  RLAPI GETS(BoundingBox) GetModelBoundingBox(GETS(Model) model);                                         // Compute model bounding box limits (considers all meshes)

// Model drawing functions
  RLAPI void DrawModel(GETS(Model) model, GETS(Vector3) position, float scale, GETS(Color) tint);               // Draw a model (with texture if set)
  RLAPI void DrawModelEx(GETS(Model) model, GETS(Vector3) position, GETS(Vector3) rotationAxis, float rotationAngle, GETS(Vector3) scale, GETS(Color) tint); // Draw a model with extended parameters
  RLAPI void DrawModelWires(GETS(Model) model, GETS(Vector3) position, float scale, GETS(Color) tint);          // Draw a model wires (with texture if set)
  RLAPI void DrawModelWiresEx(GETS(Model) model, GETS(Vector3) position, GETS(Vector3) rotationAxis, float rotationAngle, GETS(Vector3) scale, GETS(Color) tint); // Draw a model wires (with texture if set) with extended parameters
RLAPI void DrawBoundingBox(GETS(BoundingBox) box, GETS(Color) color);                                   // Draw bounding box (wires)
  RLAPI void DrawBillboard(GETS(Camera) camera, GETS(Texture2D) texture, GETS(Vector3) position, float size, GETS(Color) tint);   // Draw a billboard texture
  RLAPI void DrawBillboardRec(GETS(Camera) camera, GETS(Texture2D) texture, GETS(Rectangle) source, GETS(Vector3) position, GETS(Vector2) size, GETS(Color) tint); // Draw a billboard texture defined by source
  RLAPI void DrawBillboardPro(GETS(Camera) camera, GETS(Texture2D) texture, GETS(Rectangle) source, GETS(Vector3) position, GETS(Vector3) up, GETS(Vector2) size, GETS(Vector2) origin, float rotation, GETS(Color) tint); // Draw a billboard texture defined by source and rotation

// GETS(Mesh) management functions
RLAPI void UploadMesh(GETS(Mesh) *mesh, bool dynamic);                                            // Upload mesh vertex data in GPU and provide VAO/VBO ids
RLAPI void UpdateMeshBuffer(GETS(Mesh) mesh, int index, const void *data, int dataSize, int offset); // Update mesh vertex data in GPU for a specific buffer index
RLAPI void UnloadMesh(GETS(Mesh) mesh);                                                           // Unload mesh data from CPU and GPU
RLAPI void DrawMesh(GETS(Mesh) mesh, GETS(Material) material, GETS(Matrix) transform);                        // Draw a 3d mesh with material and transform
RLAPI void DrawMeshInstanced(GETS(Mesh) mesh, GETS(Material) material, const GETS(Matrix) *transforms, int instances); // Draw multiple mesh instances with material and different transforms
RLAPI GETS(BoundingBox) GetMeshBoundingBox(GETS(Mesh) mesh);                                            // Compute mesh bounding box limits
RLAPI void GenMeshTangents(GETS(Mesh) *mesh);                                                     // Compute mesh tangents
RLAPI bool ExportMesh(GETS(Mesh) mesh, const char *fileName);                                     // Export mesh data to file, returns true on success
RLAPI bool ExportMeshAsCode(GETS(Mesh) mesh, const char *fileName);                               // Export mesh as code file (.h) defining multiple arrays of vertex attributes

// GETS(Mesh) generation functions
RLAPI GETS(Mesh) GenMeshPoly(int sides, float radius);                                            // Generate polygonal mesh
RLAPI GETS(Mesh) GenMeshPlane(float width, float length, int resX, int resZ);                     // Generate plane mesh (with subdivisions)
RLAPI GETS(Mesh) GenMeshCube(float width, float height, float length);                            // Generate cuboid mesh
RLAPI GETS(Mesh) GenMeshSphere(float radius, int rings, int slices);                              // Generate sphere mesh (standard sphere)
RLAPI GETS(Mesh) GenMeshHemiSphere(float radius, int rings, int slices);                          // Generate half-sphere mesh (no bottom cap)
RLAPI GETS(Mesh) GenMeshCylinder(float radius, float height, int slices);                         // Generate cylinder mesh
RLAPI GETS(Mesh) GenMeshCone(float radius, float height, int slices);                             // Generate cone/pyramid mesh
RLAPI GETS(Mesh) GenMeshTorus(float radius, float size, int radSeg, int sides);                   // Generate torus mesh
RLAPI GETS(Mesh) GenMeshKnot(float radius, float size, int radSeg, int sides);                    // Generate trefoil knot mesh
RLAPI GETS(Mesh) GenMeshHeightmap(GETS(Image) heightmap, GETS(Vector3) size);                                 // Generate heightmap mesh from image data
RLAPI GETS(Mesh) GenMeshCubicmap(GETS(Image) cubicmap, GETS(Vector3) cubeSize);                               // Generate cubes-based map mesh from image data

// GETS(Material) loading/unloading functions
RLAPI GETS(Material) *LoadMaterials(const char *fileName, int *materialCount);                    // Load materials from model file
RLAPI GETS(Material) LoadMaterialDefault(void);                                                   // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
RLAPI bool IsMaterialReady(GETS(Material) material);                                              // Check if a material is ready
RLAPI void UnloadMaterial(GETS(Material) material);                                               // Unload material from GPU memory (VRAM)
  RLAPI void SetMaterialTexture(GETS(Material) *material, int mapType, GETS(Texture2D) texture);          // Set texture for a material map type (MATERIAL_MAP_DIFFUSE, MATERIAL_MAP_SPECULAR...)
  RLAPI void SetModelMeshMaterial(GETS(Model) *model, int meshId, int materialId);                  // Set material for a mesh

// Model animations loading/unloading functions
  RLAPI GETS(ModelAnimation) *LoadModelAnimations(const char *fileName, int *animCount);            // Load model animations from file
  RLAPI void UpdateModelAnimation(GETS(Model) model, GETS(ModelAnimation) anim, int frame);               // Update model animation pose
  RLAPI void UnloadModelAnimation(GETS(ModelAnimation) anim);                                       // Unload animation data
  RLAPI void UnloadModelAnimations(GETS(ModelAnimation) *animations, int animCount);                // Unload animation array data
  RLAPI bool IsModelAnimationValid(GETS(Model) model, GETS(ModelAnimation) anim);                         // Check model animation skeleton match

// Collision detection functions
RLAPI bool CheckCollisionSpheres(GETS(Vector3) center1, float radius1, GETS(Vector3) center2, float radius2);   // Check collision between two spheres
RLAPI bool CheckCollisionBoxes(GETS(BoundingBox) box1, GETS(BoundingBox) box2);                                 // Check collision between two bounding boxes
RLAPI bool CheckCollisionBoxSphere(GETS(BoundingBox) box, GETS(Vector3) center, float radius);                  // Check collision between box and sphere
  RLAPI GETS(RayCollision) GetRayCollisionSphere(GETS(Ray) ray, GETS(Vector3) center, float radius);                    // Get collision info between ray and sphere
  RLAPI GETS(RayCollision) GetRayCollisionBox(GETS(Ray) ray, GETS(BoundingBox) box);                                    // Get collision info between ray and box
  RLAPI GETS(RayCollision) GetRayCollisionMesh(GETS(Ray) ray, GETS(Mesh) mesh, GETS(Matrix) transform);                       // Get collision info between ray and mesh
  RLAPI GETS(RayCollision) GetRayCollisionTriangle(GETS(Ray) ray, GETS(Vector3) p1, GETS(Vector3) p2, GETS(Vector3) p3);            // Get collision info between ray and triangle
  RLAPI GETS(RayCollision) GetRayCollisionQuad(GETS(Ray) ray, GETS(Vector3) p1, GETS(Vector3) p2, GETS(Vector3) p3, GETS(Vector3) p4);    // Get collision info between ray and quad

//------------------------------------------------------------------------------------
// Audio Loading and Playing Functions (Module: audio)
//------------------------------------------------------------------------------------
typedef void (*GETS(AudioCallback))(void *bufferData, unsigned int frames);

// Audio device management functions
RLAPI void InitAudioDevice(void);                                     // Initialize audio device and context
RLAPI void CloseAudioDevice(void);                                    // Close the audio device and context
RLAPI bool IsAudioDeviceReady(void);                                  // Check if audio device has been initialized successfully
RLAPI void SetMasterVolume(float volume);                             // Set master volume (listener)
RLAPI float GetMasterVolume(void);                                    // Get master volume (listener)

// GETS(Wave)/Sound loading/unloading functions
RLAPI GETS(Wave) LoadWave(const char *fileName);                            // Load wave data from file
RLAPI GETS(Wave) LoadWaveFromMemory(const char *fileType, const unsigned char *fileData, int dataSize); // Load wave from memory buffer, fileType refers to extension: i.e. '.wav'
RLAPI bool IsWaveReady(GETS(Wave) wave);                                    // Checks if wave data is ready
RLAPI GETS(Sound) LoadSound(const char *fileName);                          // Load sound from file
RLAPI GETS(Sound) LoadSoundFromWave(GETS(Wave) wave);                             // Load sound from wave data
RLAPI GETS(Sound) LoadSoundAlias(GETS(Sound) source);                             // Create a new sound that shares the same sample data as the source sound, does not own the sound data
RLAPI bool IsSoundReady(GETS(Sound) sound);                                 // Checks if a sound is ready
RLAPI void UpdateSound(GETS(Sound) sound, const void *data, int sampleCount); // Update sound buffer with new data
RLAPI void UnloadWave(GETS(Wave) wave);                                     // Unload wave data
RLAPI void UnloadSound(GETS(Sound) sound);                                  // Unload sound
RLAPI void UnloadSoundAlias(GETS(Sound) alias);                             // Unload a sound alias (does not deallocate sample data)
RLAPI bool ExportWave(GETS(Wave) wave, const char *fileName);               // Export wave data to file, returns true on success
RLAPI bool ExportWaveAsCode(GETS(Wave) wave, const char *fileName);         // Export wave sample data to code (.h), returns true on success

// GETS(Wave)/Sound management functions
RLAPI void PlaySound(GETS(Sound) sound);                                    // Play a sound
RLAPI void StopSound(GETS(Sound) sound);                                    // Stop playing a sound
RLAPI void PauseSound(GETS(Sound) sound);                                   // Pause a sound
RLAPI void ResumeSound(GETS(Sound) sound);                                  // Resume a paused sound
RLAPI bool IsSoundPlaying(GETS(Sound) sound);                               // Check if a sound is currently playing
RLAPI void SetSoundVolume(GETS(Sound) sound, float volume);                 // Set volume for a sound (1.0 is max level)
RLAPI void SetSoundPitch(GETS(Sound) sound, float pitch);                   // Set pitch for a sound (1.0 is base level)
RLAPI void SetSoundPan(GETS(Sound) sound, float pan);                       // Set pan for a sound (0.5 is center)
RLAPI GETS(Wave) WaveCopy(GETS(Wave) wave);                                       // Copy a wave to a new wave
RLAPI void WaveCrop(GETS(Wave) *wave, int initFrame, int finalFrame);       // Crop a wave to defined frames range
RLAPI void WaveFormat(GETS(Wave) *wave, int sampleRate, int sampleSize, int channels); // Convert wave data to desired format
RLAPI float *LoadWaveSamples(GETS(Wave) wave);                              // Load samples data from wave as a 32bit float data array
RLAPI void UnloadWaveSamples(float *samples);                         // Unload samples data loaded with LoadWaveSamples()

// Music management functions
  RLAPI GETS(Music) LoadMusicStream(const char *fileName);                    // Load music stream from file
  RLAPI GETS(Music) LoadMusicStreamFromMemory(const char *fileType, const unsigned char *data, int dataSize); // Load music stream from data
  RLAPI bool IsMusicReady(GETS(Music) music);                                 // Checks if a music stream is ready
  RLAPI void UnloadMusicStream(GETS(Music) music);                            // Unload music stream
  RLAPI void PlayMusicStream(GETS(Music) music);                              // Start music playing
  RLAPI bool IsMusicStreamPlaying(GETS(Music) music);                         // Check if music is playing
  RLAPI void UpdateMusicStream(GETS(Music) music);                            // Updates buffers for music streaming
  RLAPI void StopMusicStream(GETS(Music) music);                              // Stop music playing
  RLAPI void PauseMusicStream(GETS(Music) music);                             // Pause music playing
  RLAPI void ResumeMusicStream(GETS(Music) music);                            // Resume playing paused music
  RLAPI void SeekMusicStream(GETS(Music) music, float position);              // Seek music to a position (in seconds)
  RLAPI void SetMusicVolume(GETS(Music) music, float volume);                 // Set volume for music (1.0 is max level)
  RLAPI void SetMusicPitch(GETS(Music) music, float pitch);                   // Set pitch for a music (1.0 is base level)
  RLAPI void SetMusicPan(GETS(Music) music, float pan);                       // Set pan for a music (0.5 is center)
  RLAPI float GetMusicTimeLength(GETS(Music) music);                          // Get music time length (in seconds)
  RLAPI float GetMusicTimePlayed(GETS(Music) music);                          // Get current music time played (in seconds)

// GETS(AudioStream) management functions
RLAPI GETS(AudioStream) LoadAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels); // Load audio stream (to stream raw audio pcm data)
RLAPI bool IsAudioStreamReady(GETS(AudioStream) stream);                    // Checks if an audio stream is ready
RLAPI void UnloadAudioStream(GETS(AudioStream) stream);                     // Unload audio stream and free memory
RLAPI void UpdateAudioStream(GETS(AudioStream) stream, const void *data, int frameCount); // Update audio stream buffers with data
RLAPI bool IsAudioStreamProcessed(GETS(AudioStream) stream);                // Check if any audio stream buffers requires refill
RLAPI void PlayAudioStream(GETS(AudioStream) stream);                       // Play audio stream
RLAPI void PauseAudioStream(GETS(AudioStream) stream);                      // Pause audio stream
RLAPI void ResumeAudioStream(GETS(AudioStream) stream);                     // Resume audio stream
RLAPI bool IsAudioStreamPlaying(GETS(AudioStream) stream);                  // Check if audio stream is playing
RLAPI void StopAudioStream(GETS(AudioStream) stream);                       // Stop audio stream
RLAPI void SetAudioStreamVolume(GETS(AudioStream) stream, float volume);    // Set volume for audio stream (1.0 is max level)
RLAPI void SetAudioStreamPitch(GETS(AudioStream) stream, float pitch);      // Set pitch for audio stream (1.0 is base level)
RLAPI void SetAudioStreamPan(GETS(AudioStream) stream, float pan);          // Set pan for audio stream (0.5 is centered)
RLAPI void SetAudioStreamBufferSizeDefault(int size);                 // Default size for new audio streams
RLAPI void SetAudioStreamCallback(GETS(AudioStream) stream, GETS(AudioCallback) callback); // Audio thread callback to request new data

RLAPI void AttachAudioStreamProcessor(GETS(AudioStream) stream, GETS(AudioCallback) processor); // Attach audio stream processor to stream, receives the samples as 'float'
RLAPI void DetachAudioStreamProcessor(GETS(AudioStream) stream, GETS(AudioCallback) processor); // Detach audio stream processor from stream

RLAPI void AttachAudioMixedProcessor(GETS(AudioCallback) processor); // Attach audio stream processor to the entire audio pipeline, receives the samples as 'float'
RLAPI void DetachAudioMixedProcessor(GETS(AudioCallback) processor); // Detach audio stream processor from the entire audio pipeline

#endif // RAYLIB_H
