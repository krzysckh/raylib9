/**********************************************************************************************
*
*   rlgl v5.0 - A multi-OpenGL abstraction layer with an immediate-mode style API
*
*   DESCRIPTION:
*       An abstraction layer for multiple OpenGL versions (1.1, 2.1, 3.3 Core, 4.3 Core, ES 2.0)
*       that provides a pseudo-OpenGL 1.1 immediate-mode style API (rlVertex, rlTranslate, rlRotate...)
*
*   ADDITIONAL NOTES:
*       When choosing an OpenGL backend different than OpenGL 1.1, some internal buffer are
*       initialized on rlglInit() to accumulate vertex data.
*
*       When an internal state change is required all the stored vertex data is renderer in batch,
*       additionally, rlDrawRenderBatchActive() could be called to force flushing of the batch.
*
*       Some resources are also loaded for convenience, here the complete list:
*          - Default batch (RLGL.defaultBatch): RenderBatch system to accumulate vertex data
*          - Default texture (RLGL.defaultTextureId): 1x1 white pixel R8G8B8A8
*          - Default shader (RLGL.State.defaultShaderId, RLGL.State.defaultShaderLocs)
*
*       Internal buffer (and resources) must be manually unloaded calling rlglClose().
*
*   CONFIGURATION:
*       #define GRAPHICS_API_OPENGL_11
*       #define GRAPHICS_API_OPENGL_21
*       #define GRAPHICS_API_OPENGL_33
*       #define GRAPHICS_API_OPENGL_43
*       #define GRAPHICS_API_OPENGL_ES2
*       #define GRAPHICS_API_OPENGL_ES3
*           Use selected OpenGL graphics backend, should be supported by platform
*           Those preprocessor defines are only used on rlgl module, if OpenGL version is
*           required by any other module, use rlGetVersion() to check it
*
*       #define RLGL_IMPLEMENTATION
*           Generates the implementation of the library into the included file.
*           If not defined, the library is in header only mode and can be included in other headers
*           or source files without problems. But only ONE file should hold the implementation.
*
*       #define RLGL_RENDER_TEXTURES_HINT
*           Enable framebuffer objects (fbo) support (enabled by default)
*           Some GPUs could not support them despite the OpenGL version
*
*       #define RLGL_SHOW_PF_DETAILS_INFO
*           Show OpenGL extensions and capabilities detailed logs on init
*
*       #define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT
*           Enable debug context (only available on OpenGL 4.3)
*
*       rlgl capabilities could be customized just defining some internal
*       values before library inclusion (default values listed):
*
*       #define RL_DEFAULT_BATCH_BUFFER_ELEMENTS   8192    // Default internal render batch elements limits
*       #define RL_DEFAULT_BATCH_BUFFERS              1    // Default number of batch buffers (multi-buffering)
*       #define RL_DEFAULT_BATCH_DRAWCALLS          256    // Default number of batch draw calls (by state changes: mode, texture)
*       #define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS    4    // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
*
*       #define RL_MAX_MATRIX_STACK_SIZE             32    // Maximum size of internal GETS(Matrix) stack
*       #define RL_MAX_SHADER_LOCATIONS              32    // Maximum number of shader locations supported
*       #define RL_CULL_DISTANCE_NEAR              0.01    // Default projection matrix near cull distance
*       #define RL_CULL_DISTANCE_FAR             1000.0    // Default projection matrix far cull distance
*
*       When loading a shader, the following vertex attributes and uniform
*       location names are tried to be set automatically:
*
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION     "vertexPosition"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD     "vertexTexCoord"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL       "vertexNormal"      // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR        "vertexColor"       // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT      "vertexTangent"     // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2    "vertexTexCoord2"   // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_MVP         "mvp"               // model-view-projection matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW        "matView"           // view matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION  "matProjection"     // projection matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL       "matModel"          // model matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL      "matNormal"         // normal matrix (transpose(inverse(matModelView)))
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR       "colDiffuse"        // color diffuse (base tint color, multiplied by texture color)
*       #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0  "texture0"          // texture0 (texture slot active 0)
*       #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1  "texture1"          // texture1 (texture slot active 1)
*       #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2  "texture2"          // texture2 (texture slot active 2)
*
*   DEPENDENCIES:
*      - OpenGL libraries (depending on platform and OpenGL version selected)
*      - PFAD OpenGL extensions loading library (only for OpenGL 3.3 Core, 4.3 Core)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2024 Ramon Santamaria (@raysan5)
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

#ifndef RLGL_H
#define RLGL_H

#ifndef RL_NO_STRUCT
#define MKSTRUCT(x,y) typedef struct x y x
#define GETS(x) x
#else
#define MKSTRUCT(x,y) typedef struct rl##x y rl##x
#define GETS(x) rl##x
#endif

#define RLGL_VERSION  "5.0"

// Function specifiers in case library is build/used as a shared library
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
// NOTE: visibility(default) attribute makes symbols "visible" when compiled with -fvisibility=hidden
#if defined(_WIN32) && defined(BUILD_LIBTYPE_SHARED)
#define RLAPI __declspec(dllexport)     // We are building the library as a Win32 shared library (.dll)
#elif defined(BUILD_LIBTYPE_SHARED)
#define RLAPI __attribute__((visibility("default"))) // We are building the library as a Unix shared library (.so/.dylib)
#elif defined(_WIN32) && defined(USE_LIBTYPE_SHARED)
#define RLAPI __declspec(dllimport)     // We are using the library as a Win32 shared library (.dll)
#endif

// Function specifiers definition
#ifndef RLAPI
#define RLAPI       // Functions defined as 'extern' by default (implicit specifiers)
#endif

// Support TRACELOG macros
#ifndef TRACELOG
#define TRACELOG(level, ...) (void)0
#define TRACELOGD(...) (void)0
#endif

// Allow custom memory allocators
#ifndef RL_MALLOC
#define RL_MALLOC(sz)     malloc(sz)
#endif
#ifndef RL_CALLOC
#define RL_CALLOC(n,sz)   calloc(n,sz)
#endif
#ifndef RL_REALLOC
#define RL_REALLOC(n,sz)  realloc(n,sz)
#endif
#ifndef RL_FREE
#define RL_FREE(p)        free(p)
#endif

#include "pixelforge.h"
#define pfGetString(x) (#x)
#define pfDisable(x) (void)"No."

// Security check in case no GRAPHICS_API_OPENGL_* defined
#if !defined(GRAPHICS_API_OPENGL_11) && \
    !defined(GRAPHICS_API_OPENGL_21) && \
    !defined(GRAPHICS_API_OPENGL_33) && \
    !defined(GRAPHICS_API_OPENGL_43) && \
    !defined(GRAPHICS_API_OPENGL_ES2) && \
    !defined(GRAPHICS_API_OPENGL_ES3)
#define GRAPHICS_API_OPENGL_33
#endif

// Security check in case multiple GRAPHICS_API_OPENGL_* defined
#if defined(GRAPHICS_API_OPENGL_11)
#if defined(GRAPHICS_API_OPENGL_21)
#undef GRAPHICS_API_OPENGL_21
#endif
#if defined(GRAPHICS_API_OPENGL_33)
#undef GRAPHICS_API_OPENGL_33
#endif
#if defined(GRAPHICS_API_OPENGL_43)
#undef GRAPHICS_API_OPENGL_43
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
#undef GRAPHICS_API_OPENGL_ES2
#endif
#endif

// OpenGL 2.1 uses most of OpenGL 3.3 Core functionality
// WARNING: Specific parts are checked with #if defines
#if defined(GRAPHICS_API_OPENGL_21)
#define GRAPHICS_API_OPENGL_33
#endif

// OpenGL 4.3 uses OpenGL 3.3 Core functionality
#if defined(GRAPHICS_API_OPENGL_43)
#define GRAPHICS_API_OPENGL_33
#endif

// OpenGL ES 3.0 uses OpenGL ES 2.0 functionality (and more)
#if defined(GRAPHICS_API_OPENGL_ES3)
#define GRAPHICS_API_OPENGL_ES2
#endif

// Support framebuffer objects by default
// NOTE: Some driver implementation do not support it, despite they should
#define RLGL_RENDER_TEXTURES_HINT

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------

// Default internal render batch elements limits
#ifndef RL_DEFAULT_BATCH_BUFFER_ELEMENTS
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
// This is the maximum amount of elements (quads) per batch
// NOTE: Be careful with text, every letter maps to a quad
#define RL_DEFAULT_BATCH_BUFFER_ELEMENTS  8192
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
// We reduce memory sizes for embedded systems (RPI and HTML5)
// NOTE: On HTML5 (emscripten) this is allocated on heap,
// by default it's only 16MB!...just take care...
#define RL_DEFAULT_BATCH_BUFFER_ELEMENTS  2048
#endif
#endif
#ifndef RL_DEFAULT_BATCH_BUFFERS
#define RL_DEFAULT_BATCH_BUFFERS                 1      // Default number of batch buffers (multi-buffering)
#endif
#ifndef RL_DEFAULT_BATCH_DRAWCALLS
#define RL_DEFAULT_BATCH_DRAWCALLS             256      // Default number of batch draw calls (by state changes: mode, texture)
#endif
#ifndef RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS
#define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS       4      // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
#endif

// Internal GETS(Matrix) stack
#ifndef RL_MAX_MATRIX_STACK_SIZE
#define RL_MAX_MATRIX_STACK_SIZE                32      // Maximum size of GETS(Matrix) stack
#endif

// Shader limits
#ifndef RL_MAX_SHADER_LOCATIONS
#define RL_MAX_SHADER_LOCATIONS                 32      // Maximum number of shader locations supported
#endif

// Projection matrix culling
#ifndef RL_CULL_DISTANCE_NEAR
#define RL_CULL_DISTANCE_NEAR                 0.01      // Default near cull distance
#endif
#ifndef RL_CULL_DISTANCE_FAR
#define RL_CULL_DISTANCE_FAR                1000.0      // Default far cull distance
#endif

// Texture parameters (equivalent to OpenGL defines)
#define RL_TEXTURE_WRAP_S                       0x2802      // PF_TEXTURE_WRAP_S
#define RL_TEXTURE_WRAP_T                       0x2803      // PF_TEXTURE_WRAP_T
#define RL_TEXTURE_MAG_FILTER                   0x2800      // PF_TEXTURE_MAG_FILTER
#define RL_TEXTURE_MIN_FILTER                   0x2801      // PF_TEXTURE_MIN_FILTER

#define RL_TEXTURE_FILTER_NEAREST               0x2600      // PF_NEAREST
#define RL_TEXTURE_FILTER_LINEAR                0x2601      // PF_LINEAR
#define RL_TEXTURE_FILTER_MIP_NEAREST           0x2700      // PF_NEAREST_MIPMAP_NEAREST
#define RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR    0x2702      // PF_NEAREST_MIPMAP_LINEAR
#define RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST    0x2701      // PF_LINEAR_MIPMAP_NEAREST
#define RL_TEXTURE_FILTER_MIP_LINEAR            0x2703      // PF_LINEAR_MIPMAP_LINEAR
#define RL_TEXTURE_FILTER_ANISOTROPIC           0x3000      // Anisotropic filter (custom identifier)
#define RL_TEXTURE_MIPMAP_BIAS_RATIO            0x4000      // Texture mipmap bias, percentage ratio (custom identifier)

#define RL_TEXTURE_WRAP_REPEAT                  0x2901      // PF_REPEAT
#define RL_TEXTURE_WRAP_CLAMP                   0x812F      // PF_CLAMP_TO_EDGE
#define RL_TEXTURE_WRAP_MIRROR_REPEAT           0x8370      // PF_MIRRORED_REPEAT
#define RL_TEXTURE_WRAP_MIRROR_CLAMP            0x8742      // PF_MIRROR_CLAMP_EXT

// GETS(Matrix) modes (equivalent to OpenGL)
#define RL_MODELVIEW                            0x1700      // PF_MODELVIEW
#define RL_PROJECTION                           0x1701      // PF_PROJECTION
#define RL_TEXTURE                              0x1702      // PF_TEXTURE

// Primitive assembly draw modes
#define RL_LINES                                0x0001      // PF_LINES
#define RL_TRIANGLES                            0x0004      // PF_TRIANGLES
#define RL_QUADS                                0x0007      // PF_QUADS

// PF equivalent data types
#define RL_UNSIGNED_BYTE                        0x1401      // PF_UNSIGNED_BYTE
#define RL_FLOAT                                0x1406      // PF_FLOAT

// PF buffer usage hint
#define RL_STREAM_DRAW                          0x88E0      // PF_STREAM_DRAW
#define RL_STREAM_READ                          0x88E1      // PF_STREAM_READ
#define RL_STREAM_COPY                          0x88E2      // PF_STREAM_COPY
#define RL_STATIC_DRAW                          0x88E4      // PF_STATIC_DRAW
#define RL_STATIC_READ                          0x88E5      // PF_STATIC_READ
#define RL_STATIC_COPY                          0x88E6      // PF_STATIC_COPY
#define RL_DYNAMIC_DRAW                         0x88E8      // PF_DYNAMIC_DRAW
#define RL_DYNAMIC_READ                         0x88E9      // PF_DYNAMIC_READ
#define RL_DYNAMIC_COPY                         0x88EA      // PF_DYNAMIC_COPY

// PF Shader type
#define RL_FRAGMENT_SHADER                      0x8B30      // PF_FRAGMENT_SHADER
#define RL_VERTEX_SHADER                        0x8B31      // PF_VERTEX_SHADER
#define RL_COMPUTE_SHADER                       0x91B9      // PF_COMPUTE_SHADER

// PF blending factors
#define RL_ZERO                                 0           // PF_ZERO
#define RL_ONE                                  1           // PF_ONE
#define RL_SRC_COLOR                            0x0300      // PF_SRC_COLOR
#define RL_ONE_MINUS_SRC_COLOR                  0x0301      // PF_ONE_MINUS_SRC_COLOR
#define RL_SRC_ALPHA                            0x0302      // PF_SRC_ALPHA
#define RL_ONE_MINUS_SRC_ALPHA                  0x0303      // PF_ONE_MINUS_SRC_ALPHA
#define RL_DST_ALPHA                            0x0304      // PF_DST_ALPHA
#define RL_ONE_MINUS_DST_ALPHA                  0x0305      // PF_ONE_MINUS_DST_ALPHA
#define RL_DST_COLOR                            0x0306      // PF_DST_COLOR
#define RL_ONE_MINUS_DST_COLOR                  0x0307      // PF_ONE_MINUS_DST_COLOR
#define RL_SRC_ALPHA_SATURATE                   0x0308      // PF_SRC_ALPHA_SATURATE
#define RL_CONSTANT_COLOR                       0x8001      // PF_CONSTANT_COLOR
#define RL_ONE_MINUS_CONSTANT_COLOR             0x8002      // PF_ONE_MINUS_CONSTANT_COLOR
#define RL_CONSTANT_ALPHA                       0x8003      // PF_CONSTANT_ALPHA
#define RL_ONE_MINUS_CONSTANT_ALPHA             0x8004      // PF_ONE_MINUS_CONSTANT_ALPHA

// PF blending functions/equations
#define RL_FUNC_ADD                             0x8006      // PF_FUNC_ADD
#define RL_MIN                                  0x8007      // PF_MIN
#define RL_MAX                                  0x8008      // PF_MAX
#define RL_FUNC_SUBTRACT                        0x800A      // PF_FUNC_SUBTRACT
#define RL_FUNC_REVERSE_SUBTRACT                0x800B      // PF_FUNC_REVERSE_SUBTRACT
#define RL_BLEND_EQUATION                       0x8009      // PF_BLEND_EQUATION
#define RL_BLEND_EQUATION_RGB                   0x8009      // PF_BLEND_EQUATION_RGB   // (Same as BLEND_EQUATION)
#define RL_BLEND_EQUATION_ALPHA                 0x883D      // PF_BLEND_EQUATION_ALPHA
#define RL_BLEND_DST_RGB                        0x80C8      // PF_BLEND_DST_RGB
#define RL_BLEND_SRC_RGB                        0x80C9      // PF_BLEND_SRC_RGB
#define RL_BLEND_DST_ALPHA                      0x80CA      // PF_BLEND_DST_ALPHA
#define RL_BLEND_SRC_ALPHA                      0x80CB      // PF_BLEND_SRC_ALPHA
#define RL_BLEND_COLOR                          0x8005      // PF_BLEND_COLOR

#define RL_READ_FRAMEBUFFER                     0x8CA8      // PF_READ_FRAMEBUFFER
#define RL_DRAW_FRAMEBUFFER                     0x8CA9      // PF_DRAW_FRAMEBUFFER

// Default shader vertex attribute locations
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION  0
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD  1
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL    2
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR     3
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT       4
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2 5
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
#include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool) && !defined(RL_BOOL_TYPE)
// Boolean type
typedef enum bool { false = 0, true = !false } bool;
#endif

#if !defined(RL_MATRIX_TYPE)
// GETS(Matrix), 4x4 components, column major, OpenGL style, right handed
typedef struct
GETS(Matrix)
{
  float m0, m4, m8, m12;      // GETS(Matrix) first row (4 components)
  float m1, m5, m9, m13;      // GETS(Matrix) second row (4 components)
  float m2, m6, m10, m14;     // GETS(Matrix) third row (4 components)
  float m3, m7, m11, m15;     // GETS(Matrix) fourth row (4 components)
}
GETS(Matrix);
#define RL_MATRIX_TYPE
#endif

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rlVertexBuffer {
  int elementCount;           // Number of elements in the buffer (QUADS)

  float *vertices;            // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
  float *texcoords;           // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
  float *normals;             // Vertex normal (XYZ - 3 components per vertex) (shader-location = 2)
  unsigned char *colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
  unsigned int *indices;      // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
  unsigned short *indices;    // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
#endif
  unsigned int vaoId;         // OpenGL Vertex Array Object id
  unsigned int vboId[5];      // OpenGL Vertex Buffer Objects id (5 types of vertex data)
} rlVertexBuffer;

// Draw call type
// NOTE: Only texture changes register a new draw, other state-change-related elements are not
// used at this moment (vaoId, shaderId, matrices), raylib just forces a batch draw call if any
// of those state-change happens (this is done in core module)
typedef struct rlDrawCall {
  int mode;                   // Drawing mode: LINES, TRIANGLES, QUADS
  int vertexCount;            // Number of vertex of the draw
  int vertexAlignment;        // Number of vertex required for index alignment (LINES, TRIANGLES)
  //unsigned int vaoId;       // Vertex array id to be used on the draw -> Using RLGL.currentBatch->vertexBuffer.vaoId
  //unsigned int shaderId;    // Shader id to be used on the draw -> Using RLGL.currentShaderId
  unsigned int textureId;     // Texture id to be used on the draw -> Use to create new draw call if changes

  //Matrix projection;        // Projection matrix for this draw -> Using RLGL.projection by default
  //Matrix modelview;         // Modelview matrix for this draw -> Using RLGL.modelview by default
} rlDrawCall;

// rlRenderBatch type
typedef struct rlRenderBatch {
  int bufferCount;            // Number of vertex buffers (multi-buffering support)
  int currentBuffer;          // Current buffer tracking in case of multi-buffering
  rlVertexBuffer *vertexBuffer; // Dynamic buffer(s) for vertex data

  rlDrawCall *draws;          // Draw calls array, depends on textureId
  int drawCounter;            // Draw calls counter
  float currentDepth;         // Current depth value for next draw
} rlRenderBatch;

// OpenGL version
typedef enum {
  RL_OPENGL_11 = 1,           // OpenGL 1.1
  RL_OPENGL_21,               // OpenGL 2.1 (GLSL 120)
  RL_OPENGL_33,               // OpenGL 3.3 (GLSL 330)
  RL_OPENGL_43,               // OpenGL 4.3 (using PFSL 330)
  RL_OPENGL_ES_20,            // OpenGL ES 2.0 (GLSL 100)
  RL_OPENGL_ES_30             // OpenGL ES 3.0 (GLSL 300 es)
} rlGlVersion;

// Trace log level
// NOTE: Organized by priority level
typedef enum {
  RL_LOG_ALL = 0,             // Display all logs
  RL_LOG_TRACE,               // Trace logging, intended for internal use only
  RL_LOG_DEBUG,               // Debug logging, used for internal debugging, it should be disabled on release builds
  RL_LOG_INFO,                // Info logging, used for program execution info
  RL_LOG_WARNING,             // Warning logging, used on recoverable failures
  RL_LOG_ERROR,               // Error logging, used on unrecoverable failures
  RL_LOG_FATAL,               // Fatal logging, used to abort program: exit(EXIT_FAILURE)
  RL_LOG_NONE                 // Disable logging
} rlTraceLogLevel;

// Texture pixel formats
// NOTE: Support depends on OpenGL version
#define RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE    PF_PIXELFORMAT_GRAYSCALE         // 8 bit per pixel (no alpha)
#define RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA  PF_PIXELFORMAT_GRAY_ALPHA        // 8*2 bpp (2 channels)
#define RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5      PF_PIXELFORMAT_R5G6B5            // 16 bpp
#define RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8      PF_PIXELFORMAT_R8G8B8            // 24 bpp
#define RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1    PF_PIXELFORMAT_R5G5B5A1          // 16 bpp (1 bit alpha)
#define RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4    PF_PIXELFORMAT_R4G4B4A4          // 16 bpp (4 bit alpha)
#define RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8    PF_PIXELFORMAT_R8G8B8A8          // 32 bpp
#define RL_PIXELFORMAT_UNCOMPRESSED_R32         PF_PIXELFORMAT_R32               // 32 bpp (1 channel - float)
#define RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32   PF_PIXELFORMAT_R32G32B32         // 32*3 bpp (3 channels - float)
#define RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32 PF_PIXELFORMAT_R32G32B32A32      // 32*4 bpp (4 channels - float)
#define RL_PIXELFORMAT_UNCOMPRESSED_R16         PF_PIXELFORMAT_R16               // 16 bpp (1 channel - half float)
#define RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16   PF_PIXELFORMAT_R16G16B16         // 16*3 bpp (3 channels - half float)
#define RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16 PF_PIXELFORMAT_R16G16B16A16      // 16*4 bpp (4 channels - half float)

#define  RL_PIXELFORMAT_COMPRESSED_DXT1_RGB  0
#define  RL_PIXELFORMAT_COMPRESSED_DXT1_RGBA 0
#define  RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA 0
#define  RL_PIXELFORMAT_COMPRESSED_DXT5_RGBA 0
#define  RL_PIXELFORMAT_COMPRESSED_ETC1_RGB 0
#define  RL_PIXELFORMAT_COMPRESSED_ETC2_RGB 0
#define  RL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA 0
#define  RL_PIXELFORMAT_COMPRESSED_PVRT_RGB 0
#define  RL_PIXELFORMAT_COMPRESSED_PVRT_RGBA 0
#define  RL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA 0
#define  RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA 0

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum {
  RL_TEXTURE_FILTER_POINT = 0,        // No filter, just pixel approximation
  RL_TEXTURE_FILTER_BILINEAR,         // Linear filtering
  RL_TEXTURE_FILTER_TRILINEAR,        // Trilinear filtering (linear with mipmaps)
  RL_TEXTURE_FILTER_ANISOTROPIC_4X,   // Anisotropic filtering 4x
  RL_TEXTURE_FILTER_ANISOTROPIC_8X,   // Anisotropic filtering 8x
  RL_TEXTURE_FILTER_ANISOTROPIC_16X,  // Anisotropic filtering 16x
} rlTextureFilter;

// Color blending modes (pre-defined)
typedef enum {
  RL_BLEND_ALPHA = 0,                 // Blend textures considering alpha (default)
  RL_BLEND_ADDITIVE,                  // Blend textures adding colors
  RL_BLEND_MULTIPLIED,                // Blend textures multiplying colors
  RL_BLEND_ADD_COLORS,                // Blend textures adding colors (alternative)
  RL_BLEND_SUBTRACT_COLORS,           // Blend textures subtracting colors (alternative)
  RL_BLEND_ALPHA_PREMULTIPLY,         // Blend premultiplied textures considering alpha
  RL_BLEND_CUSTOM,                    // Blend textures using custom src/dst factors (use rlSetBlendFactors())
  RL_BLEND_CUSTOM_SEPARATE            // Blend textures using custom src/dst factors (use rlSetBlendFactorsSeparate())
} rlBlendMode;

// Shader location point type
typedef enum {
  RL_SHADER_LOC_VERTEX_POSITION = 0,  // Shader location: vertex attribute: position
  RL_SHADER_LOC_VERTEX_TEXCOORD01,    // Shader location: vertex attribute: texcoord01
  RL_SHADER_LOC_VERTEX_TEXCOORD02,    // Shader location: vertex attribute: texcoord02
  RL_SHADER_LOC_VERTEX_NORMAL,        // Shader location: vertex attribute: normal
  RL_SHADER_LOC_VERTEX_TANGENT,       // Shader location: vertex attribute: tangent
  RL_SHADER_LOC_VERTEX_COLOR,         // Shader location: vertex attribute: color
  RL_SHADER_LOC_MATRIX_MVP,           // Shader location: matrix uniform: model-view-projection
  RL_SHADER_LOC_MATRIX_VIEW,          // Shader location: matrix uniform: view (camera transform)
  RL_SHADER_LOC_MATRIX_PROJECTION,    // Shader location: matrix uniform: projection
  RL_SHADER_LOC_MATRIX_MODEL,         // Shader location: matrix uniform: model (transform)
  RL_SHADER_LOC_MATRIX_NORMAL,        // Shader location: matrix uniform: normal
  RL_SHADER_LOC_VECTOR_VIEW,          // Shader location: vector uniform: view
  RL_SHADER_LOC_COLOR_DIFFUSE,        // Shader location: vector uniform: diffuse color
  RL_SHADER_LOC_COLOR_SPECULAR,       // Shader location: vector uniform: specular color
  RL_SHADER_LOC_COLOR_AMBIENT,        // Shader location: vector uniform: ambient color
  RL_SHADER_LOC_MAP_ALBEDO,           // Shader location: sampler2d texture: albedo (same as: RL_SHADER_LOC_MAP_DIFFUSE)
  RL_SHADER_LOC_MAP_METALNESS,        // Shader location: sampler2d texture: metalness (same as: RL_SHADER_LOC_MAP_SPECULAR)
  RL_SHADER_LOC_MAP_NORMAL,           // Shader location: sampler2d texture: normal
  RL_SHADER_LOC_MAP_ROUGHNESS,        // Shader location: sampler2d texture: roughness
  RL_SHADER_LOC_MAP_OCCLUSION,        // Shader location: sampler2d texture: occlusion
  RL_SHADER_LOC_MAP_EMISSION,         // Shader location: sampler2d texture: emission
  RL_SHADER_LOC_MAP_HEIGHT,           // Shader location: sampler2d texture: height
  RL_SHADER_LOC_MAP_CUBEMAP,          // Shader location: samplerCube texture: cubemap
  RL_SHADER_LOC_MAP_IRRADIANCE,       // Shader location: samplerCube texture: irradiance
  RL_SHADER_LOC_MAP_PREFILTER,        // Shader location: samplerCube texture: prefilter
  RL_SHADER_LOC_MAP_BRDF              // Shader location: sampler2d texture: brdf
} rlShaderLocationIndex;

#define RL_SHADER_LOC_MAP_DIFFUSE       RL_SHADER_LOC_MAP_ALBEDO
#define RL_SHADER_LOC_MAP_SPECULAR      RL_SHADER_LOC_MAP_METALNESS

// Shader uniform data type
typedef enum {
  RL_SHADER_UNIFORM_FLOAT = 0,        // Shader uniform type: float
  RL_SHADER_UNIFORM_VEC2,             // Shader uniform type: vec2 (2 float)
  RL_SHADER_UNIFORM_VEC3,             // Shader uniform type: vec3 (3 float)
  RL_SHADER_UNIFORM_VEC4,             // Shader uniform type: vec4 (4 float)
  RL_SHADER_UNIFORM_INT,              // Shader uniform type: int
  RL_SHADER_UNIFORM_IVEC2,            // Shader uniform type: ivec2 (2 int)
  RL_SHADER_UNIFORM_IVEC3,            // Shader uniform type: ivec3 (3 int)
  RL_SHADER_UNIFORM_IVEC4,            // Shader uniform type: ivec4 (4 int)
  RL_SHADER_UNIFORM_SAMPLER2D         // Shader uniform type: sampler2d
} rlShaderUniformDataType;

// Shader attribute data types
typedef enum {
  RL_SHADER_ATTRIB_FLOAT = 0,         // Shader attribute type: float
  RL_SHADER_ATTRIB_VEC2,              // Shader attribute type: vec2 (2 float)
  RL_SHADER_ATTRIB_VEC3,              // Shader attribute type: vec3 (3 float)
  RL_SHADER_ATTRIB_VEC4               // Shader attribute type: vec4 (4 float)
} rlShaderAttributeDataType;

// Framebuffer attachment type
// NOTE: By default up to 8 color channels defined, but it can be more
typedef enum {
  RL_ATTACHMENT_COLOR_CHANNEL0 = 0,       // Framebuffer attachment type: color 0
  RL_ATTACHMENT_COLOR_CHANNEL1 = 1,       // Framebuffer attachment type: color 1
  RL_ATTACHMENT_COLOR_CHANNEL2 = 2,       // Framebuffer attachment type: color 2
  RL_ATTACHMENT_COLOR_CHANNEL3 = 3,       // Framebuffer attachment type: color 3
  RL_ATTACHMENT_COLOR_CHANNEL4 = 4,       // Framebuffer attachment type: color 4
  RL_ATTACHMENT_COLOR_CHANNEL5 = 5,       // Framebuffer attachment type: color 5
  RL_ATTACHMENT_COLOR_CHANNEL6 = 6,       // Framebuffer attachment type: color 6
  RL_ATTACHMENT_COLOR_CHANNEL7 = 7,       // Framebuffer attachment type: color 7
  RL_ATTACHMENT_DEPTH = 100,              // Framebuffer attachment type: depth
  RL_ATTACHMENT_STENCIL = 200,            // Framebuffer attachment type: stencil
} rlFramebufferAttachType;

// Framebuffer texture attachment type
typedef enum {
  RL_ATTACHMENT_CUBEMAP_POSITIVE_X = 0,   // Framebuffer texture attachment type: cubemap, +X side
  RL_ATTACHMENT_CUBEMAP_NEGATIVE_X = 1,   // Framebuffer texture attachment type: cubemap, -X side
  RL_ATTACHMENT_CUBEMAP_POSITIVE_Y = 2,   // Framebuffer texture attachment type: cubemap, +Y side
  RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y = 3,   // Framebuffer texture attachment type: cubemap, -Y side
  RL_ATTACHMENT_CUBEMAP_POSITIVE_Z = 4,   // Framebuffer texture attachment type: cubemap, +Z side
  RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z = 5,   // Framebuffer texture attachment type: cubemap, -Z side
  RL_ATTACHMENT_TEXTURE2D = 100,          // Framebuffer texture attachment type: texture2d
  RL_ATTACHMENT_RENDERBUFFER = 200,       // Framebuffer texture attachment type: renderbuffer
} rlFramebufferAttachTextureType;

// Face culling mode
typedef enum {
  RL_CULL_FACE_FRONT = 0,
  RL_CULL_FACE_BACK
} rlCullMode;

//------------------------------------------------------------------------------------
// Functions Declaration - GETS(Matrix) operations
//------------------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

RLAPI void rlMatrixMode(int mode);                      // Choose the current matrix to be transformed
RLAPI void rlPushMatrix(void);                          // Push the current matrix to stack
RLAPI void rlPopMatrix(void);                           // Pop latest inserted matrix from stack
RLAPI void rlLoadIdentity(void);                        // Reset current matrix to identity matrix
RLAPI void rlTranslatef(float x, float y, float z);     // Multiply the current matrix by a translation matrix
RLAPI void rlRotatef(float angle, float x, float y, float z); // Multiply the current matrix by a rotation matrix
RLAPI void rlScalef(float x, float y, float z);         // Multiply the current matrix by a scaling matrix
RLAPI void rlMultMatrixf(const float *matf);            // Multiply the current matrix by another matrix
RLAPI void rlFrustum(double left, double right, double bottom, double top, double znear, double zfar);
RLAPI void rlOrtho(double left, double right, double bottom, double top, double znear, double zfar);
RLAPI void rlViewport(int x, int y, int width, int height); // Set the viewport area
RLAPI void rlSetClipPlanes(double nearPlane, double farPlane);    // Set clip planes distances
RLAPI double rlGetCullDistanceNear(void);               // Get cull plane distance near
RLAPI double rlGetCullDistanceFar(void);                // Get cull plane distance far

//------------------------------------------------------------------------------------
// Functions Declaration - Vertex level operations
//------------------------------------------------------------------------------------
RLAPI void rlBegin(int mode);                           // Initialize drawing mode (how to organize vertex)
RLAPI void rlEnd(void);                                 // Finish vertex providing
RLAPI void rlVertex2i(int x, int y);                    // Define one vertex (position) - 2 int
RLAPI void rlVertex2f(float x, float y);                // Define one vertex (position) - 2 float
RLAPI void rlVertex3f(float x, float y, float z);       // Define one vertex (position) - 3 float
RLAPI void rlTexCoord2f(float x, float y);              // Define one vertex (texture coordinate) - 2 float
RLAPI void rlNormal3f(float x, float y, float z);       // Define one vertex (normal) - 3 float
RLAPI void rlColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Define one vertex (color) - 4 byte
RLAPI void rlColor3f(float x, float y, float z);        // Define one vertex (color) - 3 float
RLAPI void rlColor4f(float x, float y, float z, float w); // Define one vertex (color) - 4 float

//------------------------------------------------------------------------------------
// Functions Declaration - OpenGL style functions (common to 1.1, 3.3+, ES2)
// NOTE: This functions are used to completely abstract raylib code from OpenGL layer,
// some of them are direct wrappers over OpenGL calls, some others are custom
//------------------------------------------------------------------------------------

// Vertex buffers state
RLAPI bool rlEnableVertexArray(unsigned int vaoId);     // Enable vertex array (VAO, if supported)
RLAPI void rlDisableVertexArray(void);                  // Disable vertex array (VAO, if supported)
RLAPI void rlEnableVertexBuffer(unsigned int id);       // Enable vertex buffer (VBO)
RLAPI void rlDisableVertexBuffer(void);                 // Disable vertex buffer (VBO)
RLAPI void rlEnableVertexBufferElement(unsigned int id); // Enable vertex buffer element (VBO element)
RLAPI void rlDisableVertexBufferElement(void);          // Disable vertex buffer element (VBO element)
RLAPI void rlEnableVertexAttribute(unsigned int index); // Enable vertex attribute index
RLAPI void rlDisableVertexAttribute(unsigned int index); // Disable vertex attribute index
#if defined(GRAPHICS_API_OPENGL_11)
RLAPI void rlEnableStatePointer(int vertexAttribType, void *buffer); // Enable attribute state pointer
RLAPI void rlDisableStatePointer(int vertexAttribType); // Disable attribute state pointer
#endif

// Textures state
RLAPI void rlActiveTextureSlot(int slot);               // Select and active a texture slot
RLAPI void rlEnableTexture(unsigned int id);            // Enable texture
RLAPI void rlDisableTexture(void);                      // Disable texture
RLAPI void rlEnableTextureCubemap(unsigned int id);     // Enable texture cubemap
RLAPI void rlDisableTextureCubemap(void);               // Disable texture cubemap
RLAPI void rlTextureParameters(unsigned int id, int param, int value); // Set texture parameters (filter, wrap)
RLAPI void rlCubemapParameters(unsigned int id, int param, int value); // Set cubemap parameters (filter, wrap)

// Shader state
RLAPI void rlEnableShader(unsigned int id);             // Enable shader program
RLAPI void rlDisableShader(void);                       // Disable shader program

// Framebuffer state
RLAPI void rlEnableFramebuffer(unsigned int id);        // Enable render texture (fbo)
RLAPI void rlDisableFramebuffer(void);                  // Disable render texture (fbo), return to default framebuffer
RLAPI unsigned int rlGetActiveFramebuffer(void);        // Get the currently active render texture (fbo), 0 for default framebuffer
RLAPI void rlActiveDrawBuffers(int count);              // Activate multiple draw color buffers
RLAPI void rlBlitFramebuffer(int srcX, int srcY, int srcWidth, int srcHeight, int dstX, int dstY, int dstWidth, int dstHeight, int bufferMask); // Blit active framebuffer to main framebuffer
RLAPI void rlBindFramebuffer(unsigned int target, unsigned int framebuffer); // Bind framebuffer (FBO)

// General render state
RLAPI void rlEnableColorBlend(void);                    // Enable color blending
RLAPI void rlDisableColorBlend(void);                   // Disable color blending
RLAPI void rlEnableDepthTest(void);                     // Enable depth test
RLAPI void rlDisableDepthTest(void);                    // Disable depth test
RLAPI void rlEnableDepthMask(void);                     // Enable depth write
RLAPI void rlDisableDepthMask(void);                    // Disable depth write
RLAPI void rlEnableBackfaceCulling(void);               // Enable backface culling
RLAPI void rlDisableBackfaceCulling(void);              // Disable backface culling
RLAPI void rlColorMask(bool r, bool g, bool b, bool a); // Color mask control
RLAPI void rlSetCullFace(int mode);                     // Set face culling mode
RLAPI void rlEnableScissorTest(void);                   // Enable scissor test
RLAPI void rlDisableScissorTest(void);                  // Disable scissor test
RLAPI void rlScissor(int x, int y, int width, int height); // Scissor test
RLAPI void rlEnableWireMode(void);                      // Enable wire mode
RLAPI void rlEnablePointMode(void);                     // Enable point mode
RLAPI void rlDisableWireMode(void);                     // Disable wire mode ( and point ) maybe rename
RLAPI void rlSetLineWidth(float width);                 // Set the line drawing width
RLAPI float rlGetLineWidth(void);                       // Get the line drawing width
RLAPI void rlEnableSmoothLines(void);                   // Enable line aliasing
RLAPI void rlDisableSmoothLines(void);                  // Disable line aliasing
RLAPI void rlEnableStereoRender(void);                  // Enable stereo rendering
RLAPI void rlDisableStereoRender(void);                 // Disable stereo rendering
RLAPI bool rlIsStereoRenderEnabled(void);               // Check if stereo render is enabled

RLAPI void rlClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Clear color buffer with color
RLAPI void rlClearScreenBuffers(void);                  // Clear used screen buffers (color and depth)
RLAPI void rlCheckErrors(void);                         // Check and log OpenGL error codes
RLAPI void rlSetBlendMode(int mode);                    // Set blending mode
RLAPI void rlSetBlendFactors(int pfSrcFactor, int pfDstFactor, int pfEquation); // Set blending mode factor and equation (using OpenGL factors)
RLAPI void rlSetBlendFactorsSeparate(int pfSrcRGB, int pfDstRGB, int pfSrcAlpha, int pfDstAlpha, int pfEqRGB, int pfEqAlpha); // Set blending mode factors and equations separately (using OpenGL factors)

//------------------------------------------------------------------------------------
// Functions Declaration - rlgl functionality
//------------------------------------------------------------------------------------
// rlgl initialization functions
RLAPI void rlglInit(int width, int height);             // Initialize rlgl (buffers, shaders, textures, states)
RLAPI void rlglClose(void);                             // De-initialize rlgl (buffers, shaders, textures)
RLAPI void rlLoadExtensions(void *loader);              // Load OpenGL extensions (loader function required)
RLAPI int rlGetVersion(void);                           // Get current OpenGL version
RLAPI void rlSetFramebufferWidth(int width);            // Set current framebuffer width
RLAPI int rlGetFramebufferWidth(void);                  // Get default framebuffer width
RLAPI void rlSetFramebufferHeight(int height);          // Set current framebuffer height
RLAPI int rlGetFramebufferHeight(void);                 // Get default framebuffer height

RLAPI unsigned int rlGetTextureIdDefault(void);         // Get default texture id
RLAPI unsigned int rlGetShaderIdDefault(void);          // Get default shader id
RLAPI int *rlGetShaderLocsDefault(void);                // Get default shader locations

// Render batch management
// NOTE: rlgl provides a default render batch to behave like OpenGL 1.1 immediate mode
// but this render batch API is exposed in case of custom batches are required
RLAPI rlRenderBatch rlLoadRenderBatch(int numBuffers, int bufferElements); // Load a render batch system
RLAPI void rlUnloadRenderBatch(rlRenderBatch batch);    // Unload render batch system
RLAPI void rlDrawRenderBatch(rlRenderBatch *batch);     // Draw render batch data (Update->Draw->Reset)
RLAPI void rlSetRenderBatchActive(rlRenderBatch *batch); // Set the active render batch for rlgl (NULL for default internal)
RLAPI void rlDrawRenderBatchActive(void);               // Update and draw internal render batch
RLAPI bool rlCheckRenderBatchLimit(int vCount);         // Check internal buffer overflow for a given number of vertex

RLAPI void rlSetTexture(unsigned int id);               // Set current texture for render batch and check buffers limits

//------------------------------------------------------------------------------------------------------------------------

// Vertex buffers management
RLAPI unsigned int rlLoadVertexArray(void);             // Load vertex array (vao) if supported
RLAPI unsigned int rlLoadVertexBuffer(const void *buffer, int size, bool dynamic); // Load a vertex buffer object
RLAPI unsigned int rlLoadVertexBufferElement(const void *buffer, int size, bool dynamic); // Load vertex buffer elements object
RLAPI void rlUpdateVertexBuffer(unsigned int bufferId, const void *data, int dataSize, int offset); // Update vertex buffer object data on GPU buffer
RLAPI void rlUpdateVertexBufferElements(unsigned int id, const void *data, int dataSize, int offset); // Update vertex buffer elements data on GPU buffer
RLAPI void rlUnloadVertexArray(unsigned int vaoId);     // Unload vertex array (vao)
RLAPI void rlUnloadVertexBuffer(unsigned int vboId);    // Unload vertex buffer object
RLAPI void rlSetVertexAttribute(unsigned int index, int compSize, int type, bool normalized, int stride, int offset); // Set vertex attribute data configuration
RLAPI void rlSetVertexAttributeDivisor(unsigned int index, int divisor); // Set vertex attribute data divisor
RLAPI void rlSetVertexAttributeDefault(int locIndex, const void *value, int attribType, int count); // Set vertex attribute default value, when attribute to provided
RLAPI void rlDrawVertexArray(int offset, int count);    // Draw vertex array (currently active vao)
RLAPI void rlDrawVertexArrayElements(int offset, int count, const void *buffer); // Draw vertex array elements
RLAPI void rlDrawVertexArrayInstanced(int offset, int count, int instances); // Draw vertex array (currently active vao) with instancing
RLAPI void rlDrawVertexArrayElementsInstanced(int offset, int count, const void *buffer, int instances); // Draw vertex array elements with instancing

// Textures management
RLAPI unsigned int rlLoadTexture(const void *data, int width, int height, int format, int mipmapCount); // Load texture data
RLAPI unsigned int rlLoadTextureDepth(int width, int height, bool useRenderBuffer); // Load depth texture/renderbuffer (to be attached to fbo)
RLAPI unsigned int rlLoadTextureCubemap(const void *data, int size, int format); // Load texture cubemap data
RLAPI void rlUpdateTexture(unsigned int id, int offsetX, int offsetY, int width, int height, int format, const void *data); // Update texture with new data on GPU
RLAPI void rlGetGlTextureFormats(int format, unsigned int *pfInternalFormat, unsigned int *pfFormat, unsigned int *pfType); // Get OpenGL internal formats
RLAPI const char *rlGetPixelFormatName(unsigned int format);              // Get name string for pixel format
RLAPI void rlUnloadTexture(unsigned int id);                              // Unload texture from GPU memory
RLAPI void rlGenTextureMipmaps(unsigned int id, int width, int height, int format, int *mipmaps); // Generate mipmap data for selected texture
RLAPI void *rlReadTexturePixels(unsigned int id, int width, int height, int format); // Read texture pixel data
RLAPI unsigned char *rlReadScreenPixels(int width, int height);           // Read screen pixel data (color buffer)

// Framebuffer management (fbo)
RLAPI unsigned int rlLoadFramebuffer(void);                               // Load an empty framebuffer
RLAPI void rlFramebufferAttach(unsigned int fboId, unsigned int texId, int attachType, int texType, int mipLevel); // Attach texture/renderbuffer to a framebuffer
RLAPI bool rlFramebufferComplete(unsigned int id);                        // Verify framebuffer is complete
RLAPI void rlUnloadFramebuffer(unsigned int id);                          // Delete framebuffer from GPU

// Shaders management
RLAPI unsigned int rlLoadShaderCode(const char *vsCode, const char *fsCode);    // Load shader from code strings
RLAPI unsigned int rlCompileShader(const char *shaderCode, int type);           // Compile custom shader and return shader id (type: RL_VERTEX_SHADER, RL_FRAGMENT_SHADER, RL_COMPUTE_SHADER)
RLAPI unsigned int rlLoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId); // Load custom shader program
RLAPI void rlUnloadShaderProgram(unsigned int id);                              // Unload shader program
RLAPI int rlGetLocationUniform(unsigned int shaderId, const char *uniformName); // Get shader location uniform
RLAPI int rlGetLocationAttrib(unsigned int shaderId, const char *attribName);   // Get shader location attribute
RLAPI void rlSetUniform(int locIndex, const void *value, int uniformType, int count); // Set shader value uniform
RLAPI void rlSetUniformMatrix(int locIndex, GETS(Matrix) mat);                        // Set shader value matrix
RLAPI void rlSetUniformSampler(int locIndex, unsigned int textureId);           // Set shader value sampler
RLAPI void rlSetShader(unsigned int id, int *locs);                             // Set shader currently active (id and locations)

// Compute shader management
RLAPI unsigned int rlLoadComputeShaderProgram(unsigned int shaderId);           // Load compute shader program
RLAPI void rlComputeShaderDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ); // Dispatch compute shader (equivalent to *draw* for graphics pipeline)

// Shader buffer storage object management (ssbo)
RLAPI unsigned int rlLoadShaderBuffer(unsigned int size, const void *data, int usageHint); // Load shader storage buffer object (SSBO)
RLAPI void rlUnloadShaderBuffer(unsigned int ssboId);                           // Unload shader storage buffer object (SSBO)
RLAPI void rlUpdateShaderBuffer(unsigned int id, const void *data, unsigned int dataSize, unsigned int offset); // Update SSBO buffer data
RLAPI void rlBindShaderBuffer(unsigned int id, unsigned int index);             // Bind SSBO buffer
RLAPI void rlReadShaderBuffer(unsigned int id, void *dest, unsigned int count, unsigned int offset); // Read SSBO buffer data (GPU->CPU)
RLAPI void rlCopyShaderBuffer(unsigned int destId, unsigned int srcId, unsigned int destOffset, unsigned int srcOffset, unsigned int count); // Copy SSBO data between buffers
RLAPI unsigned int rlGetShaderBufferSize(unsigned int id);                      // Get SSBO buffer size

// Buffer management
RLAPI void rlBindImageTexture(unsigned int id, unsigned int index, int format, bool readonly);  // Bind image texture

// GETS(Matrix) state management
RLAPI GETS(Matrix) rlGetMatrixModelview(void);                                  // Get internal modelview matrix
RLAPI GETS(Matrix) rlGetMatrixProjection(void);                                 // Get internal projection matrix
RLAPI GETS(Matrix) rlGetMatrixTransform(void);                                  // Get internal accumulated transform matrix
RLAPI GETS(Matrix) rlGetMatrixProjectionStereo(int eye);                        // Get internal projection matrix for stereo render (selected eye)
RLAPI GETS(Matrix) rlGetMatrixViewOffsetStereo(int eye);                        // Get internal view offset matrix for stereo render (selected eye)
RLAPI void rlSetMatrixProjection(GETS(Matrix) proj);                            // Set a custom projection matrix (replaces internal projection matrix)
RLAPI void rlSetMatrixModelview(GETS(Matrix) view);                             // Set a custom modelview matrix (replaces internal modelview matrix)
RLAPI void rlSetMatrixProjectionStereo(GETS(Matrix) right, GETS(Matrix) left);        // Set eyes projection matrices for stereo rendering
RLAPI void rlSetMatrixViewOffsetStereo(GETS(Matrix) right, GETS(Matrix) left);        // Set eyes view offsets matrices for stereo rendering

// Quick and dirty cube/quad buffers load->draw->unload
RLAPI void rlLoadDrawCube(void);     // Load and draw a cube
RLAPI void rlLoadDrawQuad(void);     // Load and draw a quad

#if defined(__cplusplus)
}
#endif

#endif // RLGL_H

/***********************************************************************************
*
*   RLGL IMPLEMENTATION
*
************************************************************************************/

#if defined(RLGL_IMPLEMENTATION)

// Expose OpenGL functions from pfad in raylib
#if defined(BUILD_LIBTYPE_SHARED)
#define PFAD_API_CALL_EXPORT
#define PFAD_API_CALL_EXPORT_BUILD
#endif

#if defined(GRAPHICS_API_OPENGL_11)
#if defined(__APPLE__)
#include <OpenGL/gl.h>          // OpenGL 1.1 library for OSX
#include <OpenGL/glext.h>       // OpenGL extensions library
#else
// APIENTRY for OpenGL function pointer declarations is required
#if !defined(APIENTRY)
#if defined(_WIN32)
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif
// WINGDIAPI definition. Some Windows OpenGL headers need it
#if !defined(WINGDIAPI) && defined(_WIN32)
#define WINGDIAPI __declspec(dllimport)
#endif

#endif
#endif

#if defined(GRAPHICS_API_OPENGL_33)
#define PFAD_MALLOC RL_MALLOC
#define PFAD_FREE RL_FREE

#define PFAD_PF_IMPLEMENTATION
#include "external/glad.h"          // PFAD extensions loading library, includes OpenGL headers
#endif

#if defined(GRAPHICS_API_OPENGL_ES3)
#include <GLES3/gl3.h>              // OpenGL ES 3.0 library
#define PF_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>           // OpenGL ES 2.0 extensions library
#elif defined(GRAPHICS_API_OPENGL_ES2)
// NOTE: OpenGL ES 2.0 can be enabled on PLATFORM_DESKTOP,
// in that case, functions are loaded from a custom pfad for OpenGL ES 2.0
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_DESKTOP_SDL)
#define PFAD_GLES2_IMPLEMENTATION
#include "external/glad_gles2.h"
#else
#define PF_GLEXT_PROTOTYPES
//#include <EGL/egl.h>          // EGL library -> not required, platform layer
#include <GLES2/gl2.h>          // OpenGL ES 2.0 library
#include <GLES2/gl2ext.h>       // OpenGL ES 2.0 extensions library
#endif

// It seems OpenGL ES 2.0 instancing entry points are not defined on Raspberry Pi
// provided headers (despite being defined in official Khronos PFES2 headers)
#if defined(PLATFORM_DRM)
typedef void (PF_APIENTRYP PFNGLDRAWARRAYSINSTANCEDEXTPROC) (GLenum mode, PFint start, PFsizei count, PFsizei primcount);
typedef void (PF_APIENTRYP PFNGLDRAWELEMENTSINSTANCEDEXTPROC) (GLenum mode, PFsizei count, PFenum type, const void *indices, PFsizei primcount);
typedef void (PF_APIENTRYP PFNGLVERTEXATTRIBDIVISOREXTPROC) (GLuint index, PFuint divisor);
#endif
#endif

#include <stdlib.h>                     // Required for: malloc(), free()
#include <string.h>                     // Required for: strcmp(), strlen() [Used in rlglInit(), on extensions loading]
#include <math.h>                       // Required for: sqrtf(), sinf(), cosf(), floor(), log()

//----------------------------------------------------------------------------------
// Defines and Macros
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

#ifndef PF_SHADING_LANGUAGE_VERSION
#define PF_SHADING_LANGUAGE_VERSION         0x8B8C
#endif

#ifndef PF_COMPRESSED_RGB_S3TC_DXT1_EXT
#define PF_COMPRESSED_RGB_S3TC_DXT1_EXT     0x83F0
#endif
#ifndef PF_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define PF_COMPRESSED_RGBA_S3TC_DXT1_EXT    0x83F1
#endif
#ifndef PF_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define PF_COMPRESSED_RGBA_S3TC_DXT3_EXT    0x83F2
#endif
#ifndef PF_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define PF_COMPRESSED_RGBA_S3TC_DXT5_EXT    0x83F3
#endif
#ifndef PF_ETC1_RGB8_OES
#define PF_ETC1_RGB8_OES                    0x8D64
#endif
#ifndef PF_COMPRESSED_RGB8_ETC2
#define PF_COMPRESSED_RGB8_ETC2             0x9274
#endif
#ifndef PF_COMPRESSED_RGBA8_ETC2_EAC
#define PF_COMPRESSED_RGBA8_ETC2_EAC        0x9278
#endif
#ifndef PF_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define PF_COMPRESSED_RGB_PVRTC_4BPPV1_IMG  0x8C00
#endif
#ifndef PF_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define PF_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif
#ifndef PF_COMPRESSED_RGBA_ASTC_4x4_KHR
#define PF_COMPRESSED_RGBA_ASTC_4x4_KHR     0x93b0
#endif
#ifndef PF_COMPRESSED_RGBA_ASTC_8x8_KHR
#define PF_COMPRESSED_RGBA_ASTC_8x8_KHR     0x93b7
#endif

#ifndef PF_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define PF_MAX_TEXTURE_MAX_ANISOTROPY_EXT   0x84FF
#endif
#ifndef PF_TEXTURE_MAX_ANISOTROPY_EXT
#define PF_TEXTURE_MAX_ANISOTROPY_EXT       0x84FE
#endif

#ifndef PF_PROGRAM_POINT_SIZE
#define PF_PROGRAM_POINT_SIZE               0x8642
#endif

#ifndef PF_LINE_WIDTH
#define PF_LINE_WIDTH                       0x0B21
#endif

#if defined(GRAPHICS_API_OPENGL_11)
#define PF_UNSIGNED_SHORT_5_6_5             0x8363
#define PF_UNSIGNED_SHORT_5_5_5_1           0x8034
#define PF_UNSIGNED_SHORT_4_4_4_4           0x8033
#endif

#if defined(GRAPHICS_API_OPENGL_21)
#define PF_LUMINANCE                        0x1909
#define PF_LUMINANCE_ALPHA                  0x190A
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)
#define pfClearDepth                 pfClearDepthf
#if !defined(GRAPHICS_API_OPENGL_ES3)
#define PF_READ_FRAMEBUFFER         PF_FRAMEBUFFER
#define PF_DRAW_FRAMEBUFFER         PF_FRAMEBUFFER
#endif
#endif

// Default shader vertex attribute names to set location points
#ifndef RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION
#define RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION     "vertexPosition"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD     "vertexTexCoord"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL
#define RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL       "vertexNormal"      // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR
#define RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR        "vertexColor"       // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT      "vertexTangent"     // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2    "vertexTexCoord2"   // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2
#endif

#ifndef RL_DEFAULT_SHADER_UNIFORM_NAME_MVP
#define RL_DEFAULT_SHADER_UNIFORM_NAME_MVP         "mvp"               // model-view-projection matrix
#endif
#ifndef RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW
#define RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW        "matView"           // view matrix
#endif
#ifndef RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION
#define RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION  "matProjection"     // projection matrix
#endif
#ifndef RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL
#define RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL       "matModel"          // model matrix
#endif
#ifndef RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL
#define RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL      "matNormal"         // normal matrix (transpose(inverse(matModelView))
#endif
#ifndef RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR
#define RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR       "colDiffuse"        // color diffuse (base tint color, multiplied by texture color)
#endif
#ifndef RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0  "texture0"          // texture0 (texture slot active 0)
#endif
#ifndef RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1  "texture1"          // texture1 (texture slot active 1)
#endif
#ifndef RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2  "texture2"          // texture2 (texture slot active 2)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
typedef struct rlglData {
  rlRenderBatch *currentBatch;            // Current render batch
  rlRenderBatch defaultBatch;             // Default internal render batch

  struct {
    int vertexCounter;                  // Current active render batch vertex counter (generic, used for all batches)
    float texcoordx, texcoordy;         // Current active texture coordinate (added on pfVertex*())
    float normalx, normaly, normalz;    // Current active normal (added on pfVertex*())
    unsigned char colorr, colorg, colorb, colora;   // Current active color (added on pfVertex*())

    int currentMatrixMode;              // Current matrix mode
    GETS(Matrix) *currentMatrix;              // Current matrix pointer
    GETS(Matrix) modelview;                   // Default modelview matrix
    GETS(Matrix) projection;                  // Default projection matrix
    GETS(Matrix) transform;                   // Transform matrix to be used with rlTranslate, rlRotate, rlScale
    bool transformRequired;             // Require transform matrix application to current draw-call vertex (if required)
    GETS(Matrix) stack[RL_MAX_MATRIX_STACK_SIZE];// GETS(Matrix) stack for push/pop
    int stackCounter;                   // GETS(Matrix) stack counter

    unsigned int defaultTextureId;      // Default texture used on shapes/poly drawing (required by shader)
    unsigned int activeTextureId[RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS];    // Active texture ids to be enabled on batch drawing (0 active by default)
    unsigned int defaultVShaderId;      // Default vertex shader id (used by default shader program)
    unsigned int defaultFShaderId;      // Default fragment shader id (used by default shader program)
    unsigned int defaultShaderId;       // Default shader program id, supports vertex color and diffuse texture
    int *defaultShaderLocs;             // Default shader locations pointer to be used on rendering
    unsigned int currentShaderId;       // Current shader id to be used on rendering (by default, defaultShaderId)
    int *currentShaderLocs;             // Current shader locations pointer to be used on rendering (by default, defaultShaderLocs)

    bool stereoRender;                  // Stereo rendering flag
    GETS(Matrix) projectionStereo[2];         // VR stereo rendering eyes projection matrices
    GETS(Matrix) viewOffsetStereo[2];         // VR stereo rendering eyes view offset matrices

    // Blending variables
    int currentBlendMode;               // Blending mode active
    int pfBlendSrcFactor;               // Blending source factor
    int pfBlendDstFactor;               // Blending destination factor
    int pfBlendEquation;                // Blending equation
    int pfBlendSrcFactorRGB;            // Blending source RGB factor
    int pfBlendDestFactorRGB;           // Blending destination RGB factor
    int pfBlendSrcFactorAlpha;          // Blending source alpha factor
    int pfBlendDestFactorAlpha;         // Blending destination alpha factor
    int pfBlendEquationRGB;             // Blending equation for RGB
    int pfBlendEquationAlpha;           // Blending equation for alpha
    bool pfCustomBlendModeModified;     // Custom blending factor and equation modification status

    int framebufferWidth;               // Current framebuffer width
    int framebufferHeight;              // Current framebuffer height

  } State;            // Renderer state
  struct {
    bool vao;                           // VAO support (OpenGL ES2 could not support VAO extension) (PF_ARB_vertex_array_object)
    bool instancing;                    // Instancing supported (PF_ANGLE_instanced_arrays, PF_EXT_draw_instanced + PF_EXT_instanced_arrays)
    bool texNPOT;                       // NPOT textures full support (PF_ARB_texture_non_power_of_two, PF_OES_texture_npot)
    bool texDepth;                      // Depth textures supported (PF_ARB_depth_texture, PF_OES_depth_texture)
    bool texDepthWebGL;                 // Depth textures supported WebGL specific (PF_WEBPF_depth_texture)
    bool texFloat32;                    // float textures support (32 bit per channel) (PF_OES_texture_float)
    bool texFloat16;                    // half float textures support (16 bit per channel) (PF_OES_texture_half_float)
    bool texCompDXT;                    // DDS texture compression support (PF_EXT_texture_compression_s3tc, PF_WEBPF_compressed_texture_s3tc, PF_WEBKIT_WEBPF_compressed_texture_s3tc)
    bool texCompETC1;                   // ETC1 texture compression support (PF_OES_compressed_ETC1_RGB8_texture, PF_WEBPF_compressed_texture_etc1)
    bool texCompETC2;                   // ETC2/EAC texture compression support (PF_ARB_ES3_compatibility)
    bool texCompPVRT;                   // PVR texture compression support (PF_IMG_texture_compression_pvrtc)
    bool texCompASTC;                   // ASTC texture compression support (PF_KHR_texture_compression_astc_hdr, PF_KHR_texture_compression_astc_ldr)
    bool texMirrorClamp;                // Clamp mirror wrap mode supported (PF_EXT_texture_mirror_clamp)
    bool texAnisoFilter;                // Anisotropic texture filtering support (PF_EXT_texture_filter_anisotropic)
    bool computeShader;                 // Compute shaders support (PF_ARB_compute_shader)
    bool ssbo;                          // Shader storage buffer object support (PF_ARB_shader_storage_buffer_object)

    float maxAnisotropyLevel;           // Maximum anisotropy level supported (minimum is 2.0f)
    int maxDepthBits;                   // Maximum bits for depth component

  } ExtSupported;     // Extensions supported flags
} rlglData;

typedef void *(*rlglLoadProc)(const char *name);   // OpenGL extension functions loader signature (same as PFADloadproc)

#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2

//----------------------------------------------------------------------------------
// pfobal Variables Definition
//----------------------------------------------------------------------------------
static double rlCullDistanceNear = RL_CULL_DISTANCE_NEAR;
static double rlCullDistanceFar = RL_CULL_DISTANCE_FAR;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
static rlglData RLGL = { 0 };
#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2

#if defined(GRAPHICS_API_OPENGL_ES2) && !defined(GRAPHICS_API_OPENGL_ES3)
// NOTE: VAO functionality is exposed through extensions (OES)
static PFNGLGENVERTEXARRAYSOESPROC pfGenVertexArrays = NULL;
static PFNGLBINDVERTEXARRAYOESPROC pfBindVertexArray = NULL;
static PFNGLDELETEVERTEXARRAYSOESPROC pfDeleteVertexArrays = NULL;

// NOTE: Instancing functionality could also be available through extension
static PFNGLDRAWARRAYSINSTANCEDEXTPROC pfDrawArraysInstanced = NULL;
static PFNGLDRAWELEMENTSINSTANCEDEXTPROC pfDrawElementsInstanced = NULL;
static PFNGLVERTEXATTRIBDIVISOREXTPROC pfVertexAttribDivisor = NULL;
#endif

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
static void rlLoadShaderDefault(void);      // Load default shader
static void rlUnloadShaderDefault(void);    // Unload default shader
#if defined(RLGL_SHOW_PF_DETAILS_INFO)
static const char *rlGetCompressedFormatName(int format); // Get compressed format official PF identifier name
#endif  // RLGL_SHOW_PF_DETAILS_INFO
#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2

static int rlGetPixelDataSize(int width, int height, int format);   // Get pixel data size in bytes (image or texture)

// Auxiliar matrix math functions
typedef struct rl_float16 {
  float v[16];
} rl_float16;
static rl_float16 rlMatrixToFloatV(GETS(Matrix) mat);             // Get float array of matrix data
#define rlMatrixToFloat(mat) (rlMatrixToFloatV(mat).v)      // Get float vector for GETS(Matrix)
static GETS(Matrix) rlMatrixIdentity(void);                       // Get identity matrix
static GETS(Matrix) rlMatrixMultiply(GETS(Matrix) left, GETS(Matrix) right);  // Multiply two matrices
static GETS(Matrix) rlMatrixTranspose(GETS(Matrix) mat);                // Transposes provided matrix
static GETS(Matrix) rlMatrixInvert(GETS(Matrix) mat);                   // Invert provided matrix

//----------------------------------------------------------------------------------
// Module Functions Definition - GETS(Matrix) operations
//----------------------------------------------------------------------------------

#if defined(GRAPHICS_API_OPENGL_11)
// Fallback to OpenGL 1.1 function calls
//---------------------------------------
void
rlMatrixMode(int mode)
{
  switch (mode) {
  case RL_PROJECTION:
    pfMatrixMode(PF_PROJECTION);
    break;
  case RL_MODELVIEW:
    pfMatrixMode(PF_MODELVIEW);
    break;
  case RL_TEXTURE:
    pfMatrixMode(PF_TEXTURE);
    break;
  default:
    break;
  }
}

void
rlFrustum(double left, double right, double bottom, double top, double znear, double zfar)
{
  pfFrustum(left, right, bottom, top, znear, zfar);
}

void
rlOrtho(double left, double right, double bottom, double top, double znear, double zfar)
{
  pfOrtho(left, right, bottom, top, znear, zfar);
}

void
rlPushMatrix(void)
{
  pfPushMatrix();
}
void
rlPopMatrix(void)
{
  pfPopMatrix();
}
void
rlLoadIdentity(void)
{
  pfLoadIdentity();
}
void
rlTranslatef(float x, float y, float z)
{
  pfTranslatef(x, y, z);
}
void
rlRotatef(float angle, float x, float y, float z)
{
  pfRotatef(angle, x, y, z);
}
void
rlScalef(float x, float y, float z)
{
  pfScalef(x, y, z);
}
void
rlMultMatrixf(const float *matf)
{
  pfMultMatrixf(matf);
}
#endif
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Choose the current matrix to be transformed
void
rlMatrixMode(int mode)
{
  if (mode == RL_PROJECTION) RLGL.State.currentMatrix = &RLGL.State.projection;
  else if (mode == RL_MODELVIEW) RLGL.State.currentMatrix = &RLGL.State.modelview;
  //else if (mode == RL_TEXTURE) // Not supported

  RLGL.State.currentMatrixMode = mode;
}

// Push the current matrix into RLGL.State.stack
void
rlPushMatrix(void)
{
  if (RLGL.State.stackCounter >= RL_MAX_MATRIX_STACK_SIZE) TRACELOG(RL_LOG_ERROR, "RLGL: GETS(Matrix) stack overflow (RL_MAX_MATRIX_STACK_SIZE)");

  if (RLGL.State.currentMatrixMode == RL_MODELVIEW) {
    RLGL.State.transformRequired = true;
    RLGL.State.currentMatrix = &RLGL.State.transform;
  }

  RLGL.State.stack[RLGL.State.stackCounter] = *RLGL.State.currentMatrix;
  RLGL.State.stackCounter++;
}

// Pop lattest inserted matrix from RLGL.State.stack
void
rlPopMatrix(void)
{
  if (RLGL.State.stackCounter > 0) {
    GETS(Matrix) mat = RLGL.State.stack[RLGL.State.stackCounter - 1];
    *RLGL.State.currentMatrix = mat;
    RLGL.State.stackCounter--;
  }

  if ((RLGL.State.stackCounter == 0) && (RLGL.State.currentMatrixMode == RL_MODELVIEW)) {
    RLGL.State.currentMatrix = &RLGL.State.modelview;
    RLGL.State.transformRequired = false;
  }
}

// Reset current matrix to identity matrix
void
rlLoadIdentity(void)
{
  *RLGL.State.currentMatrix = rlMatrixIdentity();
}

// Multiply the current matrix by a translation matrix
void
rlTranslatef(float x, float y, float z)
{
  GETS(Matrix) matTranslation = {
    1.0f, 0.0f, 0.0f, x,
    0.0f, 1.0f, 0.0f, y,
    0.0f, 0.0f, 1.0f, z,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  // NOTE: We transpose matrix with multiplication order
  *RLGL.State.currentMatrix = rlMatrixMultiply(matTranslation, *RLGL.State.currentMatrix);
}

// Multiply the current matrix by a rotation matrix
// NOTE: The provided angle must be in degrees
void
rlRotatef(float angle, float x, float y, float z)
{
  GETS(Matrix) matRotation = rlMatrixIdentity();

  // Axis vector (x, y, z) normalization
  float lengthSquared = x*x + y*y + z*z;
  if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f)) {
    float inverseLength = 1.0f/sqrtf(lengthSquared);
    x *= inverseLength;
    y *= inverseLength;
    z *= inverseLength;
  }

  // Rotation matrix generation
  float sinres = sinf(DEG2RAD*angle);
  float cosres = cosf(DEG2RAD*angle);
  float t = 1.0f - cosres;

  matRotation.m0 = x*x*t + cosres;
  matRotation.m1 = y*x*t + z*sinres;
  matRotation.m2 = z*x*t - y*sinres;
  matRotation.m3 = 0.0f;

  matRotation.m4 = x*y*t - z*sinres;
  matRotation.m5 = y*y*t + cosres;
  matRotation.m6 = z*y*t + x*sinres;
  matRotation.m7 = 0.0f;

  matRotation.m8 = x*z*t + y*sinres;
  matRotation.m9 = y*z*t - x*sinres;
  matRotation.m10 = z*z*t + cosres;
  matRotation.m11 = 0.0f;

  matRotation.m12 = 0.0f;
  matRotation.m13 = 0.0f;
  matRotation.m14 = 0.0f;
  matRotation.m15 = 1.0f;

  // NOTE: We transpose matrix with multiplication order
  *RLGL.State.currentMatrix = rlMatrixMultiply(matRotation, *RLGL.State.currentMatrix);
}

// Multiply the current matrix by a scaling matrix
void
rlScalef(float x, float y, float z)
{
  GETS(Matrix) matScale = {
    x, 0.0f, 0.0f, 0.0f,
    0.0f, y, 0.0f, 0.0f,
    0.0f, 0.0f, z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  // NOTE: We transpose matrix with multiplication order
  *RLGL.State.currentMatrix = rlMatrixMultiply(matScale, *RLGL.State.currentMatrix);
}

// Multiply the current matrix by another matrix
void
rlMultMatrixf(const float *matf)
{
  // GETS(Matrix) creation from array
  GETS(Matrix) mat = { matf[0], matf[4], matf[8], matf[12],
                       matf[1], matf[5], matf[9], matf[13],
                       matf[2], matf[6], matf[10], matf[14],
                       matf[3], matf[7], matf[11], matf[15]
                     };

  *RLGL.State.currentMatrix = rlMatrixMultiply(mat, *RLGL.State.currentMatrix);
}

// Multiply the current matrix by a perspective matrix generated by parameters
void
rlFrustum(double left, double right, double bottom, double top, double znear, double zfar)
{
  GETS(Matrix) matFrustum = { 0 };

  float rl = (float)(right - left);
  float tb = (float)(top - bottom);
  float fn = (float)(zfar - znear);

  matFrustum.m0 = ((float) znear*2.0f)/rl;
  matFrustum.m1 = 0.0f;
  matFrustum.m2 = 0.0f;
  matFrustum.m3 = 0.0f;

  matFrustum.m4 = 0.0f;
  matFrustum.m5 = ((float) znear*2.0f)/tb;
  matFrustum.m6 = 0.0f;
  matFrustum.m7 = 0.0f;

  matFrustum.m8 = ((float)right + (float)left)/rl;
  matFrustum.m9 = ((float)top + (float)bottom)/tb;
  matFrustum.m10 = -((float)zfar + (float)znear)/fn;
  matFrustum.m11 = -1.0f;

  matFrustum.m12 = 0.0f;
  matFrustum.m13 = 0.0f;
  matFrustum.m14 = -((float)zfar*(float)znear*2.0f)/fn;
  matFrustum.m15 = 0.0f;

  *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix, matFrustum);
}

// Multiply the current matrix by an orthographic matrix generated by parameters
void
rlOrtho(double left, double right, double bottom, double top, double znear, double zfar)
{
  // NOTE: If left-right and top-botton values are equal it could create a division by zero,
  // response to it is platform/compiler dependant
  GETS(Matrix) matOrtho = { 0 };

  float rl = (float)(right - left);
  float tb = (float)(top - bottom);
  float fn = (float)(zfar - znear);

  matOrtho.m0 = 2.0f/rl;
  matOrtho.m1 = 0.0f;
  matOrtho.m2 = 0.0f;
  matOrtho.m3 = 0.0f;
  matOrtho.m4 = 0.0f;
  matOrtho.m5 = 2.0f/tb;
  matOrtho.m6 = 0.0f;
  matOrtho.m7 = 0.0f;
  matOrtho.m8 = 0.0f;
  matOrtho.m9 = 0.0f;
  matOrtho.m10 = -2.0f/fn;
  matOrtho.m11 = 0.0f;
  matOrtho.m12 = -((float)left + (float)right)/rl;
  matOrtho.m13 = -((float)top + (float)bottom)/tb;
  matOrtho.m14 = -((float)zfar + (float)znear)/fn;
  matOrtho.m15 = 1.0f;

  *RLGL.State.currentMatrix = rlMatrixMultiply(*RLGL.State.currentMatrix, matOrtho);
}
#endif

// Set the viewport area (transformation from normalized device coordinates to window coordinates)
// NOTE: We store current viewport dimensions
void
rlViewport(int x, int y, int width, int height)
{
  pfViewport(x, y, width, height);
}

// Set clip planes distances
void
rlSetClipPlanes(double nearPlane, double farPlane)
{
  rlCullDistanceNear = nearPlane;
  rlCullDistanceFar = farPlane;
}

// Get cull plane distance near
double
rlGetCullDistanceNear(void)
{
  return rlCullDistanceNear;
}

// Get cull plane distance far
double
rlGetCullDistanceFar(void)
{
  return rlCullDistanceFar;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Vertex level operations
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_11)
// Fallback to OpenGL 1.1 function calls
//---------------------------------------
void
rlBegin(int mode)
{
  switch (mode) {
  case RL_LINES:
    pfBegin(PF_LINES);
    break;
  case RL_TRIANGLES:
    pfBegin(PF_TRIANGLES);
    break;
  case RL_QUADS:
    pfBegin(PF_QUADS);
    break;
  default:
    break;
  }
}

void
rlEnd()
{
  pfEnd();
}
void
rlVertex2i(int x, int y)
{
  pfVertex2i(x, y);
}
void
rlVertex2f(float x, float y)
{
  pfVertex2f(x, y);
}
void
rlVertex3f(float x, float y, float z)
{
  pfVertex3f(x, y, z);
}
void
rlTexCoord2f(float x, float y)
{
  pfTexCoord2f(x, y);
}
void
rlNormal3f(float x, float y, float z)
{
  pfNormal3f(x, y, z);
}
void
rlColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  pfColor4ub(r, g, b, a);
}
void
rlColor3f(float x, float y, float z)
{
  pfColor3f(x, y, z);
}
void
rlColor4f(float x, float y, float z, float w)
{
  pfColor4f(x, y, z, w);
}
#endif
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Initialize drawing mode (how to organize vertex)
void
rlBegin(int mode)
{
  // Draw mode can be RL_LINES, RL_TRIANGLES and RL_QUADS
  // NOTE: In all three cases, vertex are accumulated over default internal vertex buffer
  if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode != mode) {
    if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount > 0) {
      // Make sure current RLGL.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
      // that way, following QUADS drawing will keep aligned with index processing
      // It implies adding some extra alignment vertex at the end of the draw,
      // those vertex are not processed but they are considered as an additional offset
      // for the next set of vertex to be drawn
      if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_LINES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount : RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4);
      else if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? 1 : (4 - (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4)));
      else RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = 0;

      if (!rlCheckRenderBatchLimit(RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment)) {
        RLGL.State.vertexCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;
        RLGL.currentBatch->drawCounter++;
      }
    }

    if (RLGL.currentBatch->drawCounter >= RL_DEFAULT_BATCH_DRAWCALLS) rlDrawRenderBatch(RLGL.currentBatch);

    RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode = mode;
    RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount = 0;
    RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId = RLGL.State.defaultTextureId;
  }
}

// Finish vertex providing
void
rlEnd(void)
{
  // NOTE: Depth increment is dependant on rlOrtho(): z-near and z-far values,
  // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
  // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bits)
  RLGL.currentBatch->currentDepth += (1.0f/20000.0f);
}

// Define one vertex (position)
// NOTE: Vertex position data is the basic information required for drawing
void
rlVertex3f(float x, float y, float z)
{
  float tx = x;
  float ty = y;
  float tz = z;

  // Transform provided vector if required
  if (RLGL.State.transformRequired) {
    tx = RLGL.State.transform.m0*x + RLGL.State.transform.m4*y + RLGL.State.transform.m8*z + RLGL.State.transform.m12;
    ty = RLGL.State.transform.m1*x + RLGL.State.transform.m5*y + RLGL.State.transform.m9*z + RLGL.State.transform.m13;
    tz = RLGL.State.transform.m2*x + RLGL.State.transform.m6*y + RLGL.State.transform.m10*z + RLGL.State.transform.m14;
  }

  // WARNING: We can't break primitives when launching a new batch.
  // RL_LINES comes in pairs, RL_TRIANGLES come in groups of 3 vertices and RL_QUADS come in groups of 4 vertices.
  // We must check current draw.mode when a new vertex is required and finish the batch only if the draw.mode draw.vertexCount is %2, %3 or %4
  if (RLGL.State.vertexCounter > (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4 - 4)) {
    if ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_LINES) &&
        (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%2 == 0)) {
      // Reached the maximum number of vertices for RL_LINES drawing
      // Launch a draw call but keep current state for next vertices comming
      // NOTE: We add +1 vertex to the check for security
      rlCheckRenderBatchLimit(2 + 1);
    } else if ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) &&
               (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%3 == 0)) {
      rlCheckRenderBatchLimit(3 + 1);
    } else if ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_QUADS) &&
               (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4 == 0)) {
      rlCheckRenderBatchLimit(4 + 1);
    }
  }

  // Add vertices
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vertices[3*RLGL.State.vertexCounter] = tx;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vertices[3*RLGL.State.vertexCounter + 1] = ty;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].vertices[3*RLGL.State.vertexCounter + 2] = tz;

  // Add current texcoord
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].texcoords[2*RLGL.State.vertexCounter] = RLGL.State.texcoordx;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].texcoords[2*RLGL.State.vertexCounter + 1] = RLGL.State.texcoordy;

  // Add current normal
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].normals[3*RLGL.State.vertexCounter] = RLGL.State.normalx;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].normals[3*RLGL.State.vertexCounter + 1] = RLGL.State.normaly;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].normals[3*RLGL.State.vertexCounter + 2] = RLGL.State.normalz;

  // Add current color
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.State.vertexCounter] = RLGL.State.colorr;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.State.vertexCounter + 1] = RLGL.State.colorg;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.State.vertexCounter + 2] = RLGL.State.colorb;
  RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].colors[4*RLGL.State.vertexCounter + 3] = RLGL.State.colora;

  RLGL.State.vertexCounter++;
  RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount++;
}

// Define one vertex (position)
void
rlVertex2f(float x, float y)
{
  rlVertex3f(x, y, RLGL.currentBatch->currentDepth);
}

// Define one vertex (position)
void
rlVertex2i(int x, int y)
{
  rlVertex3f((float)x, (float)y, RLGL.currentBatch->currentDepth);
}

// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void
rlTexCoord2f(float x, float y)
{
  RLGL.State.texcoordx = x;
  RLGL.State.texcoordy = y;
}

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
void
rlNormal3f(float x, float y, float z)
{
  float normalx = x;
  float normaly = y;
  float normalz = z;
  if (RLGL.State.transformRequired) {
    normalx = RLGL.State.transform.m0*x + RLGL.State.transform.m4*y + RLGL.State.transform.m8*z;
    normaly = RLGL.State.transform.m1*x + RLGL.State.transform.m5*y + RLGL.State.transform.m9*z;
    normalz = RLGL.State.transform.m2*x + RLGL.State.transform.m6*y + RLGL.State.transform.m10*z;
  }
  float length = sqrtf(normalx*normalx + normaly*normaly + normalz*normalz);
  if (length != 0.0f) {
    float ilength = 1.0f / length;
    normalx *= ilength;
    normaly *= ilength;
    normalz *= ilength;
  }
  RLGL.State.normalx = normalx;
  RLGL.State.normaly = normaly;
  RLGL.State.normalz = normalz;
}

// Define one vertex (color)
void
rlColor4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
  RLGL.State.colorr = x;
  RLGL.State.colorg = y;
  RLGL.State.colorb = z;
  RLGL.State.colora = w;
}

// Define one vertex (color)
void
rlColor4f(float r, float g, float b, float a)
{
  rlColor4ub((unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255));
}

// Define one vertex (color)
void
rlColor3f(float x, float y, float z)
{
  rlColor4ub((unsigned char)(x*255), (unsigned char)(y*255), (unsigned char)(z*255), 255);
}

#endif

//--------------------------------------------------------------------------------------
// Module Functions Definition - OpenGL style functions (common to 1.1, 3.3+, ES2)
//--------------------------------------------------------------------------------------

// Set current texture to use
void
rlSetTexture(unsigned int id)
{
  if (id == 0) {
#if defined(GRAPHICS_API_OPENGL_11)
    rlDisableTexture();
#else
    // NOTE: If quads batch limit is reached, we force a draw call and next batch starts
    if (RLGL.State.vertexCounter >=
        RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4) {
      rlDrawRenderBatch(RLGL.currentBatch);
    }
#endif
  } else {
#if defined(GRAPHICS_API_OPENGL_11)
    rlEnableTexture(id);
#else
    if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId != id) {
      if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount > 0) {
        // Make sure current RLGL.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
        // that way, following QUADS drawing will keep aligned with index processing
        // It implies adding some extra alignment vertex at the end of the draw,
        // those vertex are not processed but they are considered as an additional offset
        // for the next set of vertex to be drawn
        if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_LINES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount : RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4);
        else if (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = ((RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount < 4)? 1 : (4 - (RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount%4)));
        else RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment = 0;

        if (!rlCheckRenderBatchLimit(RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment)) {
          RLGL.State.vertexCounter += RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexAlignment;

          RLGL.currentBatch->drawCounter++;
        }
      }

      if (RLGL.currentBatch->drawCounter >= RL_DEFAULT_BATCH_DRAWCALLS) rlDrawRenderBatch(RLGL.currentBatch);

      RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId = id;
      RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].vertexCount = 0;
    }
#endif
  }
}

// Select and active a texture slot
void
rlActiveTextureSlot(int slot)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfActiveTexture(PF_TEXTURE0 + slot);
#endif
}

// Enable texture
void
rlEnableTexture(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_11)
  pfEnable(PF_TEXTURE_2D);
#endif
  pfBindTexture(pfGetTexture(id));
}

// Disable texture
void
rlDisableTexture(void)
{
#if defined(GRAPHICS_API_OPENGL_11)
  pfDisable(PF_TEXTURE_2D);
#endif
  pfBindTexture(NULL);
}

// Enable texture cubemap
void
rlEnableTextureCubemap(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindTexture(PF_TEXTURE_CUBE_MAP, pfGetTexture(id));
#endif
}

// Disable texture cubemap
void
rlDisableTextureCubemap(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindTexture(PF_TEXTURE_CUBE_MAP, 0);
#endif
}

// Set texture parameters (wrap mode/filter mode)
void
rlTextureParameters(unsigned int id, int param, int value)
{
  pfBindTexture(pfGetTexture(id));

#if !defined(GRAPHICS_API_OPENGL_11)
  // Reset anisotropy filter, in case it was set
  pfTexParameterf(PF_TEXTURE_2D, PF_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
#endif

  switch (param) {
  case RL_TEXTURE_WRAP_S:
  case RL_TEXTURE_WRAP_T: {
    if (value == RL_TEXTURE_WRAP_MIRROR_CLAMP) {
#if !defined(GRAPHICS_API_OPENGL_11)
      if (RLGL.ExtSupported.texMirrorClamp) pfTexParameteri(PF_TEXTURE_2D, param, value);
      else TRACELOG(RL_LOG_WARNING, "GL: Clamp mirror wrap mode not supported (PF_MIRROR_CLAMP_EXT)");
#endif
    } else pfTexParameteri(PF_TEXTURE_2D, param, value);

  }
  break;
  case RL_TEXTURE_MAG_FILTER:
  case RL_TEXTURE_MIN_FILTER:
    pfTexParameteri(PF_TEXTURE_2D, param, value);
    break;
  case RL_TEXTURE_FILTER_ANISOTROPIC: {
#if !defined(GRAPHICS_API_OPENGL_11)
    if (value <= RLGL.ExtSupported.maxAnisotropyLevel) pfTexParameterf(PF_TEXTURE_2D, PF_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
    else if (RLGL.ExtSupported.maxAnisotropyLevel > 0.0f) {
      TRACELOG(RL_LOG_WARNING, "GL: Maximum anisotropic filter level supported is %iX", id, (int)RLGL.ExtSupported.maxAnisotropyLevel);
      pfTexParameterf(PF_TEXTURE_2D, PF_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
    } else TRACELOG(RL_LOG_WARNING, "GL: Anisotropic filtering not supported");
#endif
  }
  break;
#if defined(GRAPHICS_API_OPENGL_33)
  case RL_TEXTURE_MIPMAP_BIAS_RATIO:
    pfTexParameterf(PF_TEXTURE_2D, PF_TEXTURE_LOD_BIAS, value/100.0f);
#endif
  default:
    break;
  }

  pfBindTexture(NULL);
}

// Set cubemap parameters (wrap mode/filter mode)
void
rlCubemapParameters(unsigned int id, int param, int value)
{
#if !defined(GRAPHICS_API_OPENGL_11)
  pfBindTexture(PF_TEXTURE_CUBE_MAP, pfGetTexture(id));

  // Reset anisotropy filter, in case it was set
  pfTexParameterf(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

  switch (param) {
  case RL_TEXTURE_WRAP_S:
  case RL_TEXTURE_WRAP_T: {
    if (value == RL_TEXTURE_WRAP_MIRROR_CLAMP) {
      if (RLGL.ExtSupported.texMirrorClamp) pfTexParameteri(PF_TEXTURE_CUBE_MAP, param, value);
      else TRACELOG(RL_LOG_WARNING, "GL: Clamp mirror wrap mode not supported (PF_MIRROR_CLAMP_EXT)");
    } else pfTexParameteri(PF_TEXTURE_CUBE_MAP, param, value);

  }
  break;
  case RL_TEXTURE_MAG_FILTER:
  case RL_TEXTURE_MIN_FILTER:
    pfTexParameteri(PF_TEXTURE_CUBE_MAP, param, value);
    break;
  case RL_TEXTURE_FILTER_ANISOTROPIC: {
    if (value <= RLGL.ExtSupported.maxAnisotropyLevel) pfTexParameterf(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
    else if (RLGL.ExtSupported.maxAnisotropyLevel > 0.0f) {
      TRACELOG(RL_LOG_WARNING, "GL: Maximum anisotropic filter level supported is %iX", id, (int)RLGL.ExtSupported.maxAnisotropyLevel);
      pfTexParameterf(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
    } else TRACELOG(RL_LOG_WARNING, "GL: Anisotropic filtering not supported");
  }
  break;
#if defined(GRAPHICS_API_OPENGL_33)
  case RL_TEXTURE_MIPMAP_BIAS_RATIO:
    pfTexParameterf(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_LOD_BIAS, value/100.0f);
#endif
  default:
    break;
  }

  pfBindTexture(PF_TEXTURE_CUBE_MAP, 0);
#endif
}

// Enable shader program
void
rlEnableShader(unsigned int id)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
  pfUseProgram(id);
#endif
}

// Disable shader program
void
rlDisableShader(void)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
  pfUseProgram(0);
#endif
}

// Enable rendering to texture (fbo)
void
rlEnableFramebuffer(unsigned int id)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfBindFramebuffer(PF_FRAMEBUFFER, id);
#endif
}

// return the active render texture (fbo)
unsigned int
rlGetActiveFramebuffer(void)
{
  PFint fboId = 0;
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES3)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfGetIntegerv(PF_DRAW_FRAMEBUFFER_BINDING, &fboId);
#endif
  return fboId;
}

// Disable rendering to texture
void
rlDisableFramebuffer(void)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfBindFramebuffer(PF_FRAMEBUFFER, 0);
#endif
}

// Blit active framebuffer to main framebuffer
void
rlBlitFramebuffer(int srcX, int srcY, int srcWidth, int srcHeight, int dstX, int dstY, int dstWidth, int dstHeight, int bufferMask)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES3)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfBlitFramebuffer(srcX, srcY, srcWidth, srcHeight, dstX, dstY, dstWidth, dstHeight, bufferMask, PF_NEAREST);
#endif
}

// Bind framebuffer object (fbo)
void
rlBindFramebuffer(unsigned int target, unsigned int framebuffer)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfBindFramebuffer(target, framebuffer);
#endif
}

// Activate multiple draw color buffers
// NOTE: One color buffer is always active by default
void
rlActiveDrawBuffers(int count)
{
#if ((defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES3)) && defined(RLGL_RENDER_TEXTURES_HINT))
  // NOTE: Maximum number of draw buffers supported is implementation dependant,
  // it can be queried with pfGet*() but it must be at least 8
  //GLint maxDrawBuffers = 0;
  //pfGetIntegerv(PF_MAX_DRAW_BUFFERS, &maxDrawBuffers);

  if (count > 0) {
    if (count > 8) TRACELOG(LOG_WARNING, "GL: Max color buffers limited to 8");
    else {
      unsigned int buffers[8] = {
#if defined(GRAPHICS_API_OPENGL_ES3)
        PF_COLOR_ATTACHMENT0_EXT,
        PF_COLOR_ATTACHMENT1_EXT,
        PF_COLOR_ATTACHMENT2_EXT,
        PF_COLOR_ATTACHMENT3_EXT,
        PF_COLOR_ATTACHMENT4_EXT,
        PF_COLOR_ATTACHMENT5_EXT,
        PF_COLOR_ATTACHMENT6_EXT,
        PF_COLOR_ATTACHMENT7_EXT,
#else
        PF_COLOR_ATTACHMENT0,
        PF_COLOR_ATTACHMENT1,
        PF_COLOR_ATTACHMENT2,
        PF_COLOR_ATTACHMENT3,
        PF_COLOR_ATTACHMENT4,
        PF_COLOR_ATTACHMENT5,
        PF_COLOR_ATTACHMENT6,
        PF_COLOR_ATTACHMENT7,
#endif
      };

#if defined(GRAPHICS_API_OPENGL_ES3)
      pfDrawBuffersEXT(count, buffers);
#else
      pfDrawBuffers(count, buffers);
#endif
    }
  } else TRACELOG(LOG_WARNING, "GL: One color buffer active by default");
#endif
}

//----------------------------------------------------------------------------------
// General render state configuration
//----------------------------------------------------------------------------------

// Enable color blending
void
rlEnableColorBlend(void)
{
  pfEnable(PF_BLEND);
}

// Disable color blending
void
rlDisableColorBlend(void)
{
  pfDisable(PF_BLEND);
}

// Enable depth test
void
rlEnableDepthTest(void)
{
  pfEnable(PF_DEPTH_TEST);
}

// Disable depth test
void
rlDisableDepthTest(void)
{
  pfDisable(PF_DEPTH_TEST);
}

// Enable depth write
void
rlEnableDepthMask(void)
{
  _pfDepthMask(PF_TRUE);
}

// Disable depth write
void
rlDisableDepthMask(void)
{
  _pfDepthMask(PF_FALSE);
}

// Enable backface culling
void
rlEnableBackfaceCulling(void)
{
  pfEnable(PF_CULL_FACE);
}

// Disable backface culling
void
rlDisableBackfaceCulling(void)
{
  pfDisable(PF_CULL_FACE);
}

// Set color mask active for screen read/draw
void
rlColorMask(bool r, bool g, bool b, bool a)
{
  _pfColorMask(r, g, b, a);
}

// Set face culling mode
void
rlSetCullFace(int mode)
{
  switch (mode) {
  case RL_CULL_FACE_BACK:
    pfCullFace(PF_BACK);
    break;
  case RL_CULL_FACE_FRONT:
    pfCullFace(PF_FRONT);
    break;
  default:
    break;
  }
}

// Enable scissor test
void
rlEnableScissorTest(void)
{
  pfEnable(PF_SCISSOR_TEST);
}

// Disable scissor test
void
rlDisableScissorTest(void)
{
  pfDisable(PF_SCISSOR_TEST);
}

// Scissor test
void
rlScissor(int x, int y, int width, int height)
{
  _pfScissor(x, y, width, height);
}

// Enable wire mode
void
rlEnableWireMode(void)
{
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
  // NOTE: pfPolygonMode() not available on OpenGL ES
  pfPolygonMode(PF_FRONT_AND_BACK, PF_LINE);
#endif
}

void
rlEnablePointMode(void)
{
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
  // NOTE: pfPolygonMode() not available on OpenGL ES
  pfPolygonMode(PF_FRONT_AND_BACK, PF_POINT);
  pfEnable(PF_PROGRAM_POINT_SIZE);
#endif
}
// Disable wire mode
void
rlDisableWireMode(void)
{
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
  // NOTE: pfPolygonMode() not available on OpenGL ES
  pfPolygonMode(PF_FRONT_AND_BACK, PF_FILL);
#endif
}

// Set the line drawing width
void
rlSetLineWidth(float width)
{
  pfLineWidth(width);
}

// Get the line drawing width
float
rlGetLineWidth(void)
{
  float width = 0;
  pfGetFloatv(PF_LINE_WIDTH, &width);
  return width;
}

// Enable line aliasing
void
rlEnableSmoothLines(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_11)
  pfEnable(PF_LINE_SMOOTH);
#endif
}

// Disable line aliasing
void
rlDisableSmoothLines(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_11)
  pfDisable(PF_LINE_SMOOTH);
#endif
}

// Enable stereo rendering
void
rlEnableStereoRender(void)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
  RLGL.State.stereoRender = true;
#endif
}

// Disable stereo rendering
void
rlDisableStereoRender(void)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
  RLGL.State.stereoRender = false;
#endif
}

// Check if stereo render is enabled
bool
rlIsStereoRenderEnabled(void)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
  return RLGL.State.stereoRender;
#else
  return false;
#endif
}

// Clear color buffer with color
void
rlClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  // Color values clamp to 0.0f(0) and 1.0f(255)
  float cr = (float)r/255;
  float cg = (float)g/255;
  float cb = (float)b/255;
  float ca = (float)a/255;

  pfClearColor(cr, cg, cb, ca);
}

// Clear used screen buffers (color and depth)
void
rlClearScreenBuffers(void)
{
  pfClear(PF_COLOR_BUFFER_BIT | PF_DEPTH_BUFFER_BIT);     // Clear used buffers: Color and Depth (Depth is used for 3D)
  //pfClear(PF_COLOR_BUFFER_BIT | PF_DEPTH_BUFFER_BIT | PF_STENCIL_BUFFER_BIT);     // Stencil buffer not used...
}

// Check and log OpenGL error codes
void
rlCheckErrors(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  int check = 1;
  while (check) {
    const PFenum err = pfGetError();
    switch (err) {
    case PF_NO_ERROR:
      check = 0;
      break;
    case 0x0500:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_INVALID_ENUM");
      break;
    case 0x0501:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_INVALID_VALUE");
      break;
    case 0x0502:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_INVALID_OPERATION");
      break;
    case 0x0503:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_STACK_OVERFLOW");
      break;
    case 0x0504:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_STACK_UNDERFLOW");
      break;
    case 0x0505:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_OUT_OF_MEMORY");
      break;
    case 0x0506:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: PF_INVALID_FRAMEBUFFER_OPERATION");
      break;
    default:
      TRACELOG(RL_LOG_WARNING, "GL: Error detected: Unknown error code: %x", err);
      break;
    }
  }
#endif
}

// Set blend mode
void
rlSetBlendMode(int mode)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if ((RLGL.State.currentBlendMode != mode) || ((mode == RL_BLEND_CUSTOM || mode == RL_BLEND_CUSTOM_SEPARATE) && RLGL.State.pfCustomBlendModeModified)) {
    rlDrawRenderBatch(RLGL.currentBatch);

    switch (mode) {
    case RL_BLEND_ALPHA:
      pfBlendFunc(PF_SRC_ALPHA, PF_ONE_MINUS_SRC_ALPHA);
      pfBlendEquation(PF_FUNC_ADD);
      break;
    case RL_BLEND_ADDITIVE:
      pfBlendFunc(PF_SRC_ALPHA, PF_ONE);
      pfBlendEquation(PF_FUNC_ADD);
      break;
    case RL_BLEND_MULTIPLIED:
      pfBlendFunc(PF_DST_COLOR, PF_ONE_MINUS_SRC_ALPHA);
      pfBlendEquation(PF_FUNC_ADD);
      break;
    case RL_BLEND_ADD_COLORS:
      pfBlendFunc(PF_ONE, PF_ONE);
      pfBlendEquation(PF_FUNC_ADD);
      break;
    case RL_BLEND_SUBTRACT_COLORS:
      pfBlendFunc(PF_ONE, PF_ONE);
      pfBlendEquation(PF_FUNC_SUBTRACT);
      break;
    case RL_BLEND_ALPHA_PREMULTIPLY:
      pfBlendFunc(PF_ONE, PF_ONE_MINUS_SRC_ALPHA);
      pfBlendEquation(PF_FUNC_ADD);
      break;
    case RL_BLEND_CUSTOM: {
      // NOTE: Using PF blend src/dst factors and PF equation configured with rlSetBlendFactors()
      pfBlendFunc(RLGL.State.pfBlendSrcFactor, RLGL.State.pfBlendDstFactor);
      pfBlendEquation(RLGL.State.pfBlendEquation);

    }
    break;
    case RL_BLEND_CUSTOM_SEPARATE: {
      // NOTE: Using PF blend src/dst factors and PF equation configured with rlSetBlendFactorsSeparate()
      pfBlendFuncSeparate(RLGL.State.pfBlendSrcFactorRGB, RLGL.State.pfBlendDestFactorRGB, RLGL.State.pfBlendSrcFactorAlpha, RLGL.State.pfBlendDestFactorAlpha);
      pfBlendEquationSeparate(RLGL.State.pfBlendEquationRGB, RLGL.State.pfBlendEquationAlpha);

    }
    break;
    default:
      break;
    }

    RLGL.State.currentBlendMode = mode;
    RLGL.State.pfCustomBlendModeModified = false;
  }
#endif
}

// Set blending mode factor and equation
void
rlSetBlendFactors(int pfSrcFactor, int pfDstFactor, int pfEquation)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if ((RLGL.State.pfBlendSrcFactor != pfSrcFactor) ||
      (RLGL.State.pfBlendDstFactor != pfDstFactor) ||
      (RLGL.State.pfBlendEquation != pfEquation)) {
    RLGL.State.pfBlendSrcFactor = pfSrcFactor;
    RLGL.State.pfBlendDstFactor = pfDstFactor;
    RLGL.State.pfBlendEquation = pfEquation;

    RLGL.State.pfCustomBlendModeModified = true;
  }
#endif
}

// Set blending mode factor and equation separately for RGB and alpha
void
rlSetBlendFactorsSeparate(int pfSrcRGB, int pfDstRGB, int pfSrcAlpha, int pfDstAlpha, int pfEqRGB, int pfEqAlpha)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if ((RLGL.State.pfBlendSrcFactorRGB != pfSrcRGB) ||
      (RLGL.State.pfBlendDestFactorRGB != pfDstRGB) ||
      (RLGL.State.pfBlendSrcFactorAlpha != pfSrcAlpha) ||
      (RLGL.State.pfBlendDestFactorAlpha != pfDstAlpha) ||
      (RLGL.State.pfBlendEquationRGB != pfEqRGB) ||
      (RLGL.State.pfBlendEquationAlpha != pfEqAlpha)) {
    RLGL.State.pfBlendSrcFactorRGB = pfSrcRGB;
    RLGL.State.pfBlendDestFactorRGB = pfDstRGB;
    RLGL.State.pfBlendSrcFactorAlpha = pfSrcAlpha;
    RLGL.State.pfBlendDestFactorAlpha = pfDstAlpha;
    RLGL.State.pfBlendEquationRGB = pfEqRGB;
    RLGL.State.pfBlendEquationAlpha = pfEqAlpha;

    RLGL.State.pfCustomBlendModeModified = true;
  }
#endif
}

//----------------------------------------------------------------------------------
// Module Functions Definition - OpenGL Debug
//----------------------------------------------------------------------------------
#if defined(RLGL_ENABLE_OPENGL_DEBUG_CONTEXT) && defined(GRAPHICS_API_OPENGL_43)
static void PFAPIENTRY
rlDebugMessageCallback(GLenum source, PFenum type, PFuint id, PFenum severity, PFsizei length, const PFchar *message, const void *userParam)
{
  // Ignore non-significant error/warning codes (NVidia drivers)
  // NOTE: Here there are the details with a sample output:
  // - #131169 - Framebuffer detailed info: The driver allocated storage for renderbuffer 2. (severity: low)
  // - #131185 - Buffer detailed info: Buffer object 1 (bound to PF_ELEMENT_ARRAY_BUFFER_ARB, usage hint is PF_ENUM_88e4)
  //             will use VIDEO memory as the source for buffer object operations. (severity: low)
  // - #131218 - Program/shader state performance warning: Vertex shader in program 7 is being recompiled based on PF state. (severity: medium)
  // - #131204 - Texture state usage warning: The texture object (0) bound to texture image unit 0 does not have
  //             a defined base level and cannot be used for texture mapping. (severity: low)
  if ((id == 131169) || (id == 131185) || (id == 131218) || (id == 131204)) return;

  const char *msgSource = NULL;
  switch (source) {
  case PF_DEBUG_SOURCE_API:
    msgSource = "API";
    break;
  case PF_DEBUG_SOURCE_WINDOW_SYSTEM:
    msgSource = "WINDOW_SYSTEM";
    break;
  case PF_DEBUG_SOURCE_SHADER_COMPILER:
    msgSource = "SHADER_COMPILER";
    break;
  case PF_DEBUG_SOURCE_THIRD_PARTY:
    msgSource = "THIRD_PARTY";
    break;
  case PF_DEBUG_SOURCE_APPLICATION:
    msgSource = "APPLICATION";
    break;
  case PF_DEBUG_SOURCE_OTHER:
    msgSource = "OTHER";
    break;
  default:
    break;
  }

  const char *msgType = NULL;
  switch (type) {
  case PF_DEBUG_TYPE_ERROR:
    msgType = "ERROR";
    break;
  case PF_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    msgType = "DEPRECATED_BEHAVIOR";
    break;
  case PF_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    msgType = "UNDEFINED_BEHAVIOR";
    break;
  case PF_DEBUG_TYPE_PORTABILITY:
    msgType = "PORTABILITY";
    break;
  case PF_DEBUG_TYPE_PERFORMANCE:
    msgType = "PERFORMANCE";
    break;
  case PF_DEBUG_TYPE_MARKER:
    msgType = "MARKER";
    break;
  case PF_DEBUG_TYPE_PUSH_GROUP:
    msgType = "PUSH_GROUP";
    break;
  case PF_DEBUG_TYPE_POP_GROUP:
    msgType = "POP_GROUP";
    break;
  case PF_DEBUG_TYPE_OTHER:
    msgType = "OTHER";
    break;
  default:
    break;
  }

  const char *msgSeverity = "DEFAULT";
  switch (severity) {
  case PF_DEBUG_SEVERITY_LOW:
    msgSeverity = "LOW";
    break;
  case PF_DEBUG_SEVERITY_MEDIUM:
    msgSeverity = "MEDIUM";
    break;
  case PF_DEBUG_SEVERITY_HIGH:
    msgSeverity = "HIGH";
    break;
  case PF_DEBUG_SEVERITY_NOTIFICATION:
    msgSeverity = "NOTIFICATION";
    break;
  default:
    break;
  }

  TRACELOG(LOG_WARNING, "GL: OpenGL debug message: %s", message);
  TRACELOG(LOG_WARNING, "    > Type: %s", msgType);
  TRACELOG(LOG_WARNING, "    > Source = %s", msgSource);
  TRACELOG(LOG_WARNING, "    > Severity = %s", msgSeverity);
}
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition - rlgl functionality
//----------------------------------------------------------------------------------

// Initialize rlgl: OpenGL extensions, default buffers/shaders/textures, OpenGL states
void
rlglInit(int width, int height)
{
  // Enable OpenGL debug context if required
#if defined(RLGL_ENABLE_OPENGL_DEBUG_CONTEXT) && defined(GRAPHICS_API_OPENGL_43)
  if ((pfDebugMessageCallback != NULL) && (pfDebugMessageControl != NULL)) {
    pfDebugMessageCallback(rlDebugMessageCallback, 0);
    // pfDebugMessageControl(PF_DEBUG_SOURCE_API, PF_DEBUG_TYPE_ERROR, PF_DEBUG_SEVERITY_HIGH, 0, 0, PF_TRUE);

    // Debug context options:
    //  - PF_DEBUG_OUTPUT - Faster version but not useful for breakpoints
    //  - PF_DEBUG_OUTPUT_SYNCHRONUS - Callback is in sync with errors, so a breakpoint can be placed on the callback in order to get a stacktrace for the PF error
    pfEnable(PF_DEBUG_OUTPUT);
    pfEnable(PF_DEBUG_OUTPUT_SYNCHRONOUS);
  }
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // Init default white texture
  unsigned char pixels[4] = { 255, 255, 255, 255 };   // 1 pixel RGBA (4 bytes)
  RLGL.State.defaultTextureId = rlLoadTexture(pixels, 1, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

  if (RLGL.State.defaultTextureId != 0) TRACELOG(RL_LOG_INFO, "TEXTURE: [ID %i] Default texture loaded successfully", RLGL.State.defaultTextureId);
  else TRACELOG(RL_LOG_WARNING, "TEXTURE: Failed to load default texture");

  // Init default Shader (customized for PF 3.3 and ES2)
  // Loaded: RLGL.State.defaultShaderId + RLGL.State.defaultShaderLocs
  rlLoadShaderDefault();
  RLGL.State.currentShaderId = RLGL.State.defaultShaderId;
  RLGL.State.currentShaderLocs = RLGL.State.defaultShaderLocs;

  // Init default vertex arrays buffers
  // Simulate that the default shader has the location RL_SHADER_LOC_VERTEX_NORMAL to bind the normal buffer for the default render batch
  RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL] = RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL;
  RLGL.defaultBatch = rlLoadRenderBatch(RL_DEFAULT_BATCH_BUFFERS, RL_DEFAULT_BATCH_BUFFER_ELEMENTS);
  RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL] = -1;
  RLGL.currentBatch = &RLGL.defaultBatch;

  // Init stack matrices (emulating OpenGL 1.1)
  for (int i = 0; i < RL_MAX_MATRIX_STACK_SIZE; i++) RLGL.State.stack[i] = rlMatrixIdentity();

  // Init internal matrices
  RLGL.State.transform = rlMatrixIdentity();
  RLGL.State.projection = rlMatrixIdentity();
  RLGL.State.modelview = rlMatrixIdentity();
  RLGL.State.currentMatrix = &RLGL.State.modelview;
#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2

  // Initialize OpenGL default states
  //----------------------------------------------------------
  // Init state: Depth test
  _pfDepthFunc(PF_LEQUAL);                                 // Type of depth testing to apply
  pfDisable(PF_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

  // Init state: Blending mode
  _pfBlendFunc(PF_SRC_ALPHA, PF_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
  pfEnable(PF_BLEND);                                     // Enable color blending (required to work with transparencies)

  // Init state: Culling
  // NOTE: All shapes/models triangles are drawn CCW
  pfCullFace(PF_BACK);                                    // Cull the back face (default)
  _pfFrontFace(PF_CCW);                                    // Front face are defined counter clockwise (default)
  pfEnable(PF_CULL_FACE);                                 // Enable backface culling

  // Init state: Cubemap seamless
#if defined(GRAPHICS_API_OPENGL_33)
  pfEnable(PF_TEXTURE_CUBE_MAP_SEAMLESS);                 // Seamless cubemaps (not supported on OpenGL ES 2.0)
#endif

#if defined(GRAPHICS_API_OPENGL_11)
  // Init state: Color hints (deprecated in OpenGL 3.0+)
  _pfHint(PF_PERSPECTIVE_CORRECTION_HINT, PF_NICEST);      // Improve quality of color and texture coordinate interpolation
  pfShadeModel(PF_SMOOTH);                                // Smooth shading between vertex (vertex colors interpolation)
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // Store screen size into pfobal variables
  RLGL.State.framebufferWidth = width;
  RLGL.State.framebufferHeight = height;

  TRACELOG(RL_LOG_INFO, "RLGL: Default OpenGL state initialized successfully");
  //----------------------------------------------------------
#endif

  // Init state: Color/Depth buffers clear
  pfClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
  pfClearDepth(1.0f);                                     // Set clear depth value (default)
  pfClear(PF_COLOR_BUFFER_BIT | PF_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)
}

// Vertex Buffer Object deinitialization (memory free)
void
rlglClose(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  rlUnloadRenderBatch(RLGL.defaultBatch);

  rlUnloadShaderDefault();          // Unload default shader

  pfDeleteTextures(1, &RLGL.State.defaultTextureId); // Unload default texture
  TRACELOG(RL_LOG_INFO, "TEXTURE: [ID %i] Default texture unloaded successfully", RLGL.State.defaultTextureId);
#endif
}

// Load OpenGL extensions
// NOTE: External loader function must be provided
void
rlLoadExtensions(void *loader)
{
#if defined(GRAPHICS_API_OPENGL_33)     // Also defined for GRAPHICS_API_OPENGL_21
  // NOTE: pfad is generated and contains only required OpenGL 3.3 Core extensions (and lower versions)
  if (gladLoadGL((GLADloadfunc)loader) == 0) TRACELOG(RL_LOG_WARNING, "GLAD: Cannot load OpenGL extensions");
  else TRACELOG(RL_LOG_INFO, "GLAD: OpenGL extensions loaded successfully");

  // Get number of supported extensions
  PFint numExt = 0;
  pfGetIntegerv(PF_NUM_EXTENSIONS, &numExt);
  TRACELOG(RL_LOG_INFO, "GL: Supported extensions count: %i", numExt);

#if defined(RLGL_SHOW_PF_DETAILS_INFO)
  // Get supported extensions list
  // WARNING: pfGetStringi() not available on OpenGL 2.1
  TRACELOG(RL_LOG_INFO, "GL: OpenGL extensions:");
  for (int i = 0; i < numExt; i++) TRACELOG(RL_LOG_INFO, "    %s", pfGetStringi(PF_EXTENSIONS, i));
#endif

#if defined(GRAPHICS_API_OPENGL_21)
  // Register supported extensions flags
  // Optional OpenGL 2.1 extensions
  RLGL.ExtSupported.vao = PFAD_PF_ARB_vertex_array_object;
  RLGL.ExtSupported.instancing = (GLAD_PF_EXT_draw_instanced && PFAD_PF_ARB_instanced_arrays);
  RLGL.ExtSupported.texNPOT = PFAD_PF_ARB_texture_non_power_of_two;
  RLGL.ExtSupported.texFloat32 = PFAD_PF_ARB_texture_float;
  RLGL.ExtSupported.texFloat16 = PFAD_PF_ARB_texture_float;
  RLGL.ExtSupported.texDepth = PFAD_PF_ARB_depth_texture;
  RLGL.ExtSupported.maxDepthBits = 32;
  RLGL.ExtSupported.texAnisoFilter = PFAD_PF_EXT_texture_filter_anisotropic;
  RLGL.ExtSupported.texMirrorClamp = PFAD_PF_EXT_texture_mirror_clamp;
#else
  // Register supported extensions flags
  // OpenGL 3.3 extensions supported by default (core)
  RLGL.ExtSupported.vao = true;
  RLGL.ExtSupported.instancing = true;
  RLGL.ExtSupported.texNPOT = true;
  RLGL.ExtSupported.texFloat32 = true;
  RLGL.ExtSupported.texFloat16 = true;
  RLGL.ExtSupported.texDepth = true;
  RLGL.ExtSupported.maxDepthBits = 32;
  RLGL.ExtSupported.texAnisoFilter = true;
  RLGL.ExtSupported.texMirrorClamp = true;
#endif

  // Optional OpenGL 3.3 extensions
  RLGL.ExtSupported.texCompASTC = PFAD_PF_KHR_texture_compression_astc_hdr && PFAD_PF_KHR_texture_compression_astc_ldr;
  RLGL.ExtSupported.texCompDXT = PFAD_PF_EXT_texture_compression_s3tc;  // Texture compression: DXT
  RLGL.ExtSupported.texCompETC2 = PFAD_PF_ARB_ES3_compatibility;        // Texture compression: ETC2/EAC
#if defined(GRAPHICS_API_OPENGL_43)
  RLGL.ExtSupported.computeShader = PFAD_PF_ARB_compute_shader;
  RLGL.ExtSupported.ssbo = PFAD_PF_ARB_shader_storage_buffer_object;
#endif

#endif  // GRAPHICS_API_OPENGL_33

#if defined(GRAPHICS_API_OPENGL_ES3)
  // Register supported extensions flags
  // OpenGL ES 3.0 extensions supported by default (or it should be)
  RLGL.ExtSupported.vao = true;
  RLGL.ExtSupported.instancing = true;
  RLGL.ExtSupported.texNPOT = true;
  RLGL.ExtSupported.texFloat32 = true;
  RLGL.ExtSupported.texFloat16 = true;
  RLGL.ExtSupported.texDepth = true;
  RLGL.ExtSupported.texDepthWebGL = true;
  RLGL.ExtSupported.maxDepthBits = 24;
  RLGL.ExtSupported.texAnisoFilter = true;
  RLGL.ExtSupported.texMirrorClamp = true;
  // TODO: Check for additional OpenGL ES 3.0 supported extensions:
  //RLGL.ExtSupported.texCompDXT = true;
  //RLGL.ExtSupported.texCompETC1 = true;
  //RLGL.ExtSupported.texCompETC2 = true;
  //RLGL.ExtSupported.texCompPVRT = true;
  //RLGL.ExtSupported.texCompASTC = true;
  //RLGL.ExtSupported.maxAnisotropyLevel = true;
  //RLGL.ExtSupported.computeShader = true;
  //RLGL.ExtSupported.ssbo = true;

#elif defined(GRAPHICS_API_OPENGL_ES2)

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_DESKTOP_SDL)
  // TODO: Support PFAD loader for OpenGL ES 3.0
  if (gladLoadGLES2((GLADloadfunc)loader) == 0) TRACELOG(RL_LOG_WARNING, "GLAD: Cannot load OpenGL ES2.0 functions");
  else TRACELOG(RL_LOG_INFO, "GLAD: OpenGL ES 2.0 loaded successfully");
#endif

  // Get supported extensions list
  PFint numExt = 0;
  const char **extList = RL_MALLOC(512*sizeof(const char *)); // Allocate 512 strings pointers (2 KB)
  const char *extensions = (const char *)pfGetString(PF_EXTENSIONS);  // One big const string

  // NOTE: We have to duplicate string because pfGetString() returns a const string
  int size = strlen(extensions) + 1;      // Get extensions string size in bytes
  char *extensionsDup = (char *)RL_CALLOC(size, sizeof(char));
  strcpy(extensionsDup, extensions);
  extList[numExt] = extensionsDup;

  for (int i = 0; i < size; i++) {
    if (extensionsDup[i] == ' ') {
      extensionsDup[i] = '\0';
      numExt++;
      extList[numExt] = &extensionsDup[i + 1];
    }
  }

  TRACELOG(RL_LOG_INFO, "GL: Supported extensions count: %i", numExt);

#if defined(RLGL_SHOW_PF_DETAILS_INFO)
  TRACELOG(RL_LOG_INFO, "GL: OpenGL extensions:");
  for (int i = 0; i < numExt; i++) TRACELOG(RL_LOG_INFO, "    %s", extList[i]);
#endif

  // Check required extensions
  for (int i = 0; i < numExt; i++) {
    // Check VAO support
    // NOTE: Only check on OpenGL ES, OpenGL 3.3 has VAO support as core feature
    if (strcmp(extList[i], (const char *)"PF_OES_vertex_array_object") == 0) {
      // The extension is supported by our hardware and driver, try to get related functions pointers
      // NOTE: emscripten does not support VAOs natively, it uses emulation and it reduces overall performance...
      pfGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)((rlglLoadProc)loader)("pfGenVertexArraysOES");
      pfBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)((rlglLoadProc)loader)("pfBindVertexArrayOES");
      pfDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)((rlglLoadProc)loader)("pfDeleteVertexArraysOES");
      //pfIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)loader("pfIsVertexArrayOES");     // NOTE: Fails in WebGL, omitted

      if ((pfGenVertexArrays != NULL) && (pfBindVertexArray != NULL) && (pfDeleteVertexArrays != NULL)) RLGL.ExtSupported.vao = true;
    }

    // Check instanced rendering support
    if (strcmp(extList[i], (const char *)"PF_ANGLE_instanced_arrays") == 0) {       // Web ANGLE
      pfDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((rlglLoadProc)loader)("pfDrawArraysInstancedANGLE");
      pfDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((rlglLoadProc)loader)("pfDrawElementsInstancedANGLE");
      pfVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISOREXTPROC)((rlglLoadProc)loader)("pfVertexAttribDivisorANGLE");

      if ((pfDrawArraysInstanced != NULL) && (pfDrawElementsInstanced != NULL) && (pfVertexAttribDivisor != NULL)) RLGL.ExtSupported.instancing = true;
    } else {
      if ((strcmp(extList[i], (const char *)"PF_EXT_draw_instanced") == 0) &&     // Standard EXT
          (strcmp(extList[i], (const char *)"PF_EXT_instanced_arrays") == 0)) {
        pfDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((rlglLoadProc)loader)("pfDrawArraysInstancedEXT");
        pfDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((rlglLoadProc)loader)("pfDrawElementsInstancedEXT");
        pfVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISOREXTPROC)((rlglLoadProc)loader)("pfVertexAttribDivisorEXT");

        if ((pfDrawArraysInstanced != NULL) && (pfDrawElementsInstanced != NULL) && (pfVertexAttribDivisor != NULL)) RLGL.ExtSupported.instancing = true;
      }
    }

    // Check NPOT textures support
    // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
    if (strcmp(extList[i], (const char *)"PF_OES_texture_npot") == 0) RLGL.ExtSupported.texNPOT = true;

    // Check texture float support
    if (strcmp(extList[i], (const char *)"PF_OES_texture_float") == 0) RLGL.ExtSupported.texFloat32 = true;
    if (strcmp(extList[i], (const char *)"PF_OES_texture_half_float") == 0) RLGL.ExtSupported.texFloat16 = true;

    // Check depth texture support
    if (strcmp(extList[i], (const char *)"PF_OES_depth_texture") == 0) RLGL.ExtSupported.texDepth = true;
    if (strcmp(extList[i], (const char *)"PF_WEBPF_depth_texture") == 0) RLGL.ExtSupported.texDepthWebGL = true;    // WebGL requires unsized internal format
    if (RLGL.ExtSupported.texDepthWebGL) RLGL.ExtSupported.texDepth = true;

    if (strcmp(extList[i], (const char *)"PF_OES_depth24") == 0) RLGL.ExtSupported.maxDepthBits = 24;   // Not available on WebGL
    if (strcmp(extList[i], (const char *)"PF_OES_depth32") == 0) RLGL.ExtSupported.maxDepthBits = 32;   // Not available on WebGL

    // Check texture compression support: DXT
    if ((strcmp(extList[i], (const char *)"PF_EXT_texture_compression_s3tc") == 0) ||
        (strcmp(extList[i], (const char *)"PF_WEBPF_compressed_texture_s3tc") == 0) ||
        (strcmp(extList[i], (const char *)"PF_WEBKIT_WEBPF_compressed_texture_s3tc") == 0)) RLGL.ExtSupported.texCompDXT = true;

    // Check texture compression support: ETC1
    if ((strcmp(extList[i], (const char *)"PF_OES_compressed_ETC1_RGB8_texture") == 0) ||
        (strcmp(extList[i], (const char *)"PF_WEBPF_compressed_texture_etc1") == 0)) RLGL.ExtSupported.texCompETC1 = true;

    // Check texture compression support: ETC2/EAC
    if (strcmp(extList[i], (const char *)"PF_ARB_ES3_compatibility") == 0) RLGL.ExtSupported.texCompETC2 = true;

    // Check texture compression support: PVR
    if (strcmp(extList[i], (const char *)"PF_IMG_texture_compression_pvrtc") == 0) RLGL.ExtSupported.texCompPVRT = true;

    // Check texture compression support: ASTC
    if (strcmp(extList[i], (const char *)"PF_KHR_texture_compression_astc_hdr") == 0) RLGL.ExtSupported.texCompASTC = true;

    // Check anisotropic texture filter support
    if (strcmp(extList[i], (const char *)"PF_EXT_texture_filter_anisotropic") == 0) RLGL.ExtSupported.texAnisoFilter = true;

    // Check clamp mirror wrap mode support
    if (strcmp(extList[i], (const char *)"PF_EXT_texture_mirror_clamp") == 0) RLGL.ExtSupported.texMirrorClamp = true;
  }

  // Free extensions pointers
  RL_FREE(extList);
  RL_FREE(extensionsDup);    // Duplicated string must be deallocated
#endif  // GRAPHICS_API_OPENGL_ES2

  // Check OpenGL information and capabilities
  //------------------------------------------------------------------------------
  // Show current OpenGL and PFSL version
  TRACELOG(RL_LOG_INFO, "GL: OpenGL device information:");
  TRACELOG(RL_LOG_INFO, "    > Vendor:   %s", pfGetString(PF_VENDOR));
  TRACELOG(RL_LOG_INFO, "    > Renderer: %s", pfGetString(PF_RENDERER));
  TRACELOG(RL_LOG_INFO, "    > Version:  %s", pfGetString(PF_VERSION));
  TRACELOG(RL_LOG_INFO, "    > PFSL:     %s", pfGetString(PF_SHADING_LANGUAGE_VERSION));

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // NOTE: Anisotropy levels capability is an extension
#ifndef PF_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define PF_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif
  pfGetFloatv(PF_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &RLGL.ExtSupported.maxAnisotropyLevel);

#if defined(RLGL_SHOW_PF_DETAILS_INFO)
  // Show some OpenGL GPU capabilities
  TRACELOG(RL_LOG_INFO, "GL: OpenGL capabilities:");
  PFint capability = 0;
  pfGetIntegerv(PF_MAX_TEXTURE_SIZE, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_TEXTURE_SIZE: %i", capability);
  pfGetIntegerv(PF_MAX_CUBE_MAP_TEXTURE_SIZE, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_CUBE_MAP_TEXTURE_SIZE: %i", capability);
  pfGetIntegerv(PF_MAX_TEXTURE_IMAGE_UNITS, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_TEXTURE_IMAGE_UNITS: %i", capability);
  pfGetIntegerv(PF_MAX_VERTEX_ATTRIBS, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_VERTEX_ATTRIBS: %i", capability);
#if !defined(GRAPHICS_API_OPENGL_ES2)
  pfGetIntegerv(PF_MAX_UNIFORM_BLOCK_SIZE, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_UNIFORM_BLOCK_SIZE: %i", capability);
  pfGetIntegerv(PF_MAX_DRAW_BUFFERS, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_DRAW_BUFFERS: %i", capability);
  if (RLGL.ExtSupported.texAnisoFilter) TRACELOG(RL_LOG_INFO, "    PF_MAX_TEXTURE_MAX_ANISOTROPY: %.0f", RLGL.ExtSupported.maxAnisotropyLevel);
#endif
  pfGetIntegerv(PF_NUM_COMPRESSED_TEXTURE_FORMATS, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_NUM_COMPRESSED_TEXTURE_FORMATS: %i", capability);
  PFint *compFormats = (GLint *)RL_CALLOC(capability, sizeof(GLint));
  pfGetIntegerv(PF_COMPRESSED_TEXTURE_FORMATS, compFormats);
  for (int i = 0; i < capability; i++) TRACELOG(RL_LOG_INFO, "        %s", rlGetCompressedFormatName(compFormats[i]));
  RL_FREE(compFormats);

#if defined(GRAPHICS_API_OPENGL_43)
  pfGetIntegerv(PF_MAX_VERTEX_ATTRIB_BINDINGS, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_VERTEX_ATTRIB_BINDINGS: %i", capability);
  pfGetIntegerv(PF_MAX_UNIFORM_LOCATIONS, &capability);
  TRACELOG(RL_LOG_INFO, "    PF_MAX_UNIFORM_LOCATIONS: %i", capability);
#endif  // GRAPHICS_API_OPENGL_43
#else   // RLGL_SHOW_PF_DETAILS_INFO

  // Show some basic info about PF supported features
  if (RLGL.ExtSupported.vao) TRACELOG(RL_LOG_INFO, "GL: VAO extension detected, VAO functions loaded successfully");
  else TRACELOG(RL_LOG_WARNING, "GL: VAO extension not found, VAO not supported");
  if (RLGL.ExtSupported.texNPOT) TRACELOG(RL_LOG_INFO, "GL: NPOT textures extension detected, full NPOT textures supported");
  else TRACELOG(RL_LOG_WARNING, "GL: NPOT textures extension not found, limited NPOT support (no-mipmaps, no-repeat)");
  if (RLGL.ExtSupported.texCompDXT) TRACELOG(RL_LOG_INFO, "GL: DXT compressed textures supported");
  if (RLGL.ExtSupported.texCompETC1) TRACELOG(RL_LOG_INFO, "GL: ETC1 compressed textures supported");
  if (RLGL.ExtSupported.texCompETC2) TRACELOG(RL_LOG_INFO, "GL: ETC2/EAC compressed textures supported");
  if (RLGL.ExtSupported.texCompPVRT) TRACELOG(RL_LOG_INFO, "GL: PVRT compressed textures supported");
  if (RLGL.ExtSupported.texCompASTC) TRACELOG(RL_LOG_INFO, "GL: ASTC compressed textures supported");
  if (RLGL.ExtSupported.computeShader) TRACELOG(RL_LOG_INFO, "GL: Compute shaders supported");
  if (RLGL.ExtSupported.ssbo) TRACELOG(RL_LOG_INFO, "GL: Shader storage buffer objects supported");
#endif  // RLGL_SHOW_PF_DETAILS_INFO

#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2
}

// Get current OpenGL version
int
rlGetVersion(void)
{
  int pfVersion = 0;
#if defined(GRAPHICS_API_OPENGL_11)
  pfVersion = RL_OPENGL_11;
#endif
#if defined(GRAPHICS_API_OPENGL_21)
  pfVersion = RL_OPENGL_21;
#elif defined(GRAPHICS_API_OPENGL_43)
  pfVersion = RL_OPENGL_43;
#elif defined(GRAPHICS_API_OPENGL_33)
  pfVersion = RL_OPENGL_33;
#endif
#if defined(GRAPHICS_API_OPENGL_ES3)
  pfVersion = RL_OPENGL_ES_30;
#elif defined(GRAPHICS_API_OPENGL_ES2)
  pfVersion = RL_OPENGL_ES_20;
#endif

  return pfVersion;
}

// Set current framebuffer width
void
rlSetFramebufferWidth(int width)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  RLGL.State.framebufferWidth = width;
#endif
}

// Set current framebuffer height
void
rlSetFramebufferHeight(int height)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  RLGL.State.framebufferHeight = height;
#endif
}

// Get default framebuffer width
int
rlGetFramebufferWidth(void)
{
  int width = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  width = RLGL.State.framebufferWidth;
#endif
  return width;
}

// Get default framebuffer height
int
rlGetFramebufferHeight(void)
{
  int height = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  height = RLGL.State.framebufferHeight;
#endif
  return height;
}

// Get default internal texture (white texture)
// NOTE: Default texture is a 1x1 pixel UNCOMPRESSED_R8G8B8A8
unsigned int
rlGetTextureIdDefault(void)
{
  unsigned int id = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  id = RLGL.State.defaultTextureId;
#endif
  return id;
}

// Get default shader id
unsigned int
rlGetShaderIdDefault(void)
{
  unsigned int id = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  id = RLGL.State.defaultShaderId;
#endif
  return id;
}

// Get default shader locs
int *
rlGetShaderLocsDefault(void)
{
  int *locs = NULL;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  locs = RLGL.State.defaultShaderLocs;
#endif
  return locs;
}

// Render batch management
//------------------------------------------------------------------------------------------------
// Load render batch
rlRenderBatch
rlLoadRenderBatch(int numBuffers, int bufferElements)
{
  rlRenderBatch batch = { 0 };

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // Initialize CPU (RAM) vertex buffers (position, texcoord, color data and indexes)
  //--------------------------------------------------------------------------------------------
  batch.vertexBuffer = (rlVertexBuffer *)RL_MALLOC(numBuffers*sizeof(rlVertexBuffer));

  for (int i = 0; i < numBuffers; i++) {
    batch.vertexBuffer[i].elementCount = bufferElements;

    batch.vertexBuffer[i].vertices = (float *)RL_MALLOC(bufferElements*3*4*sizeof(float));        // 3 float by vertex, 4 vertex by quad
    batch.vertexBuffer[i].texcoords = (float *)RL_MALLOC(bufferElements*2*4*sizeof(float));       // 2 float by texcoord, 4 texcoord by quad
    batch.vertexBuffer[i].normals = (float *)RL_MALLOC(bufferElements*3*4*sizeof(float));        // 3 float by vertex, 4 vertex by quad
    batch.vertexBuffer[i].colors = (unsigned char *)RL_MALLOC(bufferElements*4*4*sizeof(unsigned char));   // 4 float by color, 4 colors by quad
#if defined(GRAPHICS_API_OPENGL_33)
    batch.vertexBuffer[i].indices = (unsigned int *)RL_MALLOC(bufferElements*6*sizeof(unsigned int));      // 6 int by quad (indices)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
    batch.vertexBuffer[i].indices = (unsigned short *)RL_MALLOC(bufferElements*6*sizeof(unsigned short));  // 6 int by quad (indices)
#endif

    for (int j = 0; j < (3*4*bufferElements); j++) batch.vertexBuffer[i].vertices[j] = 0.0f;
    for (int j = 0; j < (2*4*bufferElements); j++) batch.vertexBuffer[i].texcoords[j] = 0.0f;
    for (int j = 0; j < (3*4*bufferElements); j++) batch.vertexBuffer[i].normals[j] = 0.0f;
    for (int j = 0; j < (4*4*bufferElements); j++) batch.vertexBuffer[i].colors[j] = 0;

    int k = 0;

    // Indices can be initialized right now
    for (int j = 0; j < (6*bufferElements); j += 6) {
      batch.vertexBuffer[i].indices[j] = 4*k;
      batch.vertexBuffer[i].indices[j + 1] = 4*k + 1;
      batch.vertexBuffer[i].indices[j + 2] = 4*k + 2;
      batch.vertexBuffer[i].indices[j + 3] = 4*k;
      batch.vertexBuffer[i].indices[j + 4] = 4*k + 2;
      batch.vertexBuffer[i].indices[j + 5] = 4*k + 3;

      k++;
    }

    RLGL.State.vertexCounter = 0;
  }

  TRACELOG(RL_LOG_INFO, "RLGL: Render batch vertex buffers loaded successfully in RAM (CPU)");
  //--------------------------------------------------------------------------------------------

  // Upload to GPU (VRAM) vertex data and initialize VAOs/VBOs
  //--------------------------------------------------------------------------------------------
  for (int i = 0; i < numBuffers; i++) {
    if (RLGL.ExtSupported.vao) {
      // Initialize Quads VAO
      pfGenVertexArrays(1, &batch.vertexBuffer[i].vaoId);
      pfBindVertexArray(batch.vertexBuffer[i].vaoId);
    }

    // Quads - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    pfGenBuffers(1, &batch.vertexBuffer[i].vboId[0]);
    pfBindBuffer(PF_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[0]);
    pfBufferData(PF_ARRAY_BUFFER, bufferElements*3*4*sizeof(float), batch.vertexBuffer[i].vertices, PF_DYNAMIC_DRAW);
    pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_POSITION]);
    pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_POSITION], 3, PF_FLOAT, 0, 0, 0);

    // Vertex texcoord buffer (shader-location = 1)
    pfGenBuffers(1, &batch.vertexBuffer[i].vboId[1]);
    pfBindBuffer(PF_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[1]);
    pfBufferData(PF_ARRAY_BUFFER, bufferElements*2*4*sizeof(float), batch.vertexBuffer[i].texcoords, PF_DYNAMIC_DRAW);
    pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_TEXCOORD01]);
    pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_TEXCOORD01], 2, PF_FLOAT, 0, 0, 0);

    // Vertex normal buffer (shader-location = 2)
    pfGenBuffers(1, &batch.vertexBuffer[i].vboId[2]);
    pfBindBuffer(PF_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[2]);
    pfBufferData(PF_ARRAY_BUFFER, bufferElements*3*4*sizeof(float), batch.vertexBuffer[i].normals, PF_DYNAMIC_DRAW);
    pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL]);
    pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL], 3, PF_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    pfGenBuffers(1, &batch.vertexBuffer[i].vboId[3]);
    pfBindBuffer(PF_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[3]);
    pfBufferData(PF_ARRAY_BUFFER, bufferElements*4*4*sizeof(unsigned char), batch.vertexBuffer[i].colors, PF_DYNAMIC_DRAW);
    pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_COLOR]);
    pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_COLOR], 4, PF_UNSIGNED_BYTE, PF_TRUE, 0, 0);

    // Fill index buffer
    pfGenBuffers(1, &batch.vertexBuffer[i].vboId[4]);
    pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[4]);
#if defined(GRAPHICS_API_OPENGL_33)
    pfBufferData(PF_ELEMENT_ARRAY_BUFFER, bufferElements*6*sizeof(int), batch.vertexBuffer[i].indices, PF_STATIC_DRAW);
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
    pfBufferData(PF_ELEMENT_ARRAY_BUFFER, bufferElements*6*sizeof(short), batch.vertexBuffer[i].indices, PF_STATIC_DRAW);
#endif
  }

  TRACELOG(RL_LOG_INFO, "RLGL: Render batch vertex buffers loaded successfully in VRAM (GPU)");

  // Unbind the current VAO
  if (RLGL.ExtSupported.vao) pfBindVertexArray(0);
  //--------------------------------------------------------------------------------------------

  // Init draw calls tracking system
  //--------------------------------------------------------------------------------------------
  batch.draws = (rlDrawCall *)RL_MALLOC(RL_DEFAULT_BATCH_DRAWCALLS*sizeof(rlDrawCall));

  for (int i = 0; i < RL_DEFAULT_BATCH_DRAWCALLS; i++) {
    batch.draws[i].mode = RL_QUADS;
    batch.draws[i].vertexCount = 0;
    batch.draws[i].vertexAlignment = 0;
    //batch.draws[i].vaoId = 0;
    //batch.draws[i].shaderId = 0;
    batch.draws[i].textureId = RLGL.State.defaultTextureId;
    //batch.draws[i].RLGL.State.projection = rlMatrixIdentity();
    //batch.draws[i].RLGL.State.modelview = rlMatrixIdentity();
  }

  batch.bufferCount = numBuffers;    // Record buffer count
  batch.drawCounter = 1;             // Reset draws counter
  batch.currentDepth = -1.0f;         // Reset depth value
  //--------------------------------------------------------------------------------------------
#endif

  return batch;
}

// Unload default internal buffers vertex data from CPU and GPU
void
rlUnloadRenderBatch(rlRenderBatch batch)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // Unbind everything
  pfBindBuffer(PF_ARRAY_BUFFER, 0);
  pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, 0);

  // Unload all vertex buffers data
  for (int i = 0; i < batch.bufferCount; i++) {
    // Unbind VAO attribs data
    if (RLGL.ExtSupported.vao) {
      pfBindVertexArray(batch.vertexBuffer[i].vaoId);
      pfDisableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
      pfDisableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
      pfDisableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL);
      pfDisableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR);
      pfBindVertexArray(0);
    }

    // Delete VBOs from GPU (VRAM)
    pfDeleteBuffers(1, &batch.vertexBuffer[i].vboId[0]);
    pfDeleteBuffers(1, &batch.vertexBuffer[i].vboId[1]);
    pfDeleteBuffers(1, &batch.vertexBuffer[i].vboId[2]);
    pfDeleteBuffers(1, &batch.vertexBuffer[i].vboId[3]);
    pfDeleteBuffers(1, &batch.vertexBuffer[i].vboId[4]);

    // Delete VAOs from GPU (VRAM)
    if (RLGL.ExtSupported.vao) pfDeleteVertexArrays(1, &batch.vertexBuffer[i].vaoId);

    // Free vertex arrays memory from CPU (RAM)
    RL_FREE(batch.vertexBuffer[i].vertices);
    RL_FREE(batch.vertexBuffer[i].texcoords);
    RL_FREE(batch.vertexBuffer[i].normals);
    RL_FREE(batch.vertexBuffer[i].colors);
    RL_FREE(batch.vertexBuffer[i].indices);
  }

  // Unload arrays
  RL_FREE(batch.vertexBuffer);
  RL_FREE(batch.draws);
#endif
}

// Draw render batch
// NOTE: We require a pointer to reset batch and increase current buffer (multi-buffer)
void
rlDrawRenderBatch(rlRenderBatch *batch)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // Update batch vertex buffers
  //------------------------------------------------------------------------------------------------------------
  // NOTE: If there is not vertex data, buffers doesn't need to be updated (vertexCount > 0)
  // TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (use a change detector flag?)
  if (RLGL.State.vertexCounter > 0) {
    // Activate elements VAO
    if (RLGL.ExtSupported.vao) pfBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);

    // Vertex positions buffer
    pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[0]);
    pfBufferSubData(PF_ARRAY_BUFFER, 0, RLGL.State.vertexCounter*3*sizeof(float), batch->vertexBuffer[batch->currentBuffer].vertices);
    //pfBufferData(PF_ARRAY_BUFFER, sizeof(float)*3*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].vertices, PF_DYNAMIC_DRAW);  // Update all buffer

    // Texture coordinates buffer
    pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[1]);
    pfBufferSubData(PF_ARRAY_BUFFER, 0, RLGL.State.vertexCounter*2*sizeof(float), batch->vertexBuffer[batch->currentBuffer].texcoords);
    //pfBufferData(PF_ARRAY_BUFFER, sizeof(float)*2*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].texcoords, PF_DYNAMIC_DRAW); // Update all buffer

    // Normals buffer
    pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[2]);
    pfBufferSubData(PF_ARRAY_BUFFER, 0, RLGL.State.vertexCounter*3*sizeof(float), batch->vertexBuffer[batch->currentBuffer].normals);
    //pfBufferData(PF_ARRAY_BUFFER, sizeof(float)*3*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].normals, PF_DYNAMIC_DRAW); // Update all buffer

    // Colors buffer
    pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[3]);
    pfBufferSubData(PF_ARRAY_BUFFER, 0, RLGL.State.vertexCounter*4*sizeof(unsigned char), batch->vertexBuffer[batch->currentBuffer].colors);
    //pfBufferData(PF_ARRAY_BUFFER, sizeof(float)*4*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].colors, PF_DYNAMIC_DRAW);    // Update all buffer

    // NOTE: pfMapBuffer() causes sync issue.
    // If GPU is working with this buffer, pfMapBuffer() will wait(stall) until GPU to finish its job.
    // To avoid waiting (idle), you can call first pfBufferData() with NULL pointer before pfMapBuffer().
    // If you do that, the previous data in PBO will be discarded and pfMapBuffer() returns a new
    // allocated pointer immediately even if GPU is still working with the previous data.

    // Another option: map the buffer object into client's memory
    // Probably this code could be moved somewhere else...
    // batch->vertexBuffer[batch->currentBuffer].vertices = (float *)pfMapBuffer(PF_ARRAY_BUFFER, PF_READ_WRITE);
    // if (batch->vertexBuffer[batch->currentBuffer].vertices)
    // {
    // Update vertex data
    // }
    // pfUnmapBuffer(PF_ARRAY_BUFFER);

    // Unbind the current VAO
    if (RLGL.ExtSupported.vao) pfBindVertexArray(0);
  }
  //------------------------------------------------------------------------------------------------------------

  // Draw batch vertex buffers (considering VR stereo if required)
  //------------------------------------------------------------------------------------------------------------
  GETS(Matrix) matProjection = RLGL.State.projection;
  GETS(Matrix) matModelView = RLGL.State.modelview;

  int eyeCount = 1;
  if (RLGL.State.stereoRender) eyeCount = 2;

  for (int eye = 0; eye < eyeCount; eye++) {
    if (eyeCount == 2) {
      // Setup current eye viewport (half screen width)
      rlViewport(eye*RLGL.State.framebufferWidth/2, 0, RLGL.State.framebufferWidth/2, RLGL.State.framebufferHeight);

      // Set current eye view offset to modelview matrix
      rlSetMatrixModelview(rlMatrixMultiply(matModelView, RLGL.State.viewOffsetStereo[eye]));
      // Set current eye projection matrix
      rlSetMatrixProjection(RLGL.State.projectionStereo[eye]);
    }

    // Draw buffers
    if (RLGL.State.vertexCounter > 0) {
      // Set current shader and upload current MVP matrix
      pfUseProgram(RLGL.State.currentShaderId);

      // Create modelview-projection matrix and upload to shader
      GETS(Matrix) matMVP = rlMatrixMultiply(RLGL.State.modelview, RLGL.State.projection);
      pfUniformMatrix4fv(RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_MVP], 1, false, rlMatrixToFloat(matMVP));

      if (RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_PROJECTION] != -1) {
        pfUniformMatrix4fv(RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_PROJECTION], 1, false, rlMatrixToFloat(RLGL.State.projection));
      }

      // WARNING: For the following setup of the view, model, and normal matrices, it is expected that
      // transformations and rendering occur between rlPushMatrix and rlPopMatrix.

      if (RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_VIEW] != -1) {
        pfUniformMatrix4fv(RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_VIEW], 1, false, rlMatrixToFloat(RLGL.State.modelview));
      }

      if (RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_MODEL] != -1) {
        pfUniformMatrix4fv(RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_MODEL], 1, false, rlMatrixToFloat(RLGL.State.transform));
      }

      if (RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_NORMAL] != -1) {
        pfUniformMatrix4fv(RLGL.State.currentShaderLocs[RL_SHADER_LOC_MATRIX_NORMAL], 1, false, rlMatrixToFloat(rlMatrixTranspose(rlMatrixInvert(RLGL.State.transform))));
      }

      if (RLGL.ExtSupported.vao) pfBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);
      else {
        // Bind vertex attrib: position (shader-location = 0)
        pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[0]);
        pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_POSITION], 3, PF_FLOAT, 0, 0, 0);
        pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_POSITION]);

        // Bind vertex attrib: texcoord (shader-location = 1)
        pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[1]);
        pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_TEXCOORD01], 2, PF_FLOAT, 0, 0, 0);
        pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_TEXCOORD01]);

        // Bind vertex attrib: normal (shader-location = 2)
        pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[2]);
        pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL], 3, PF_FLOAT, 0, 0, 0);
        pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL]);

        // Bind vertex attrib: color (shader-location = 3)
        pfBindBuffer(PF_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[3]);
        pfVertexAttribPointer(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_COLOR], 4, PF_UNSIGNED_BYTE, PF_TRUE, 0, 0);
        pfEnableVertexAttribArray(RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_COLOR]);

        pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[4]);
      }

      // Setup some default shader values
      pfUniform4f(RLGL.State.currentShaderLocs[RL_SHADER_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
      pfUniform1i(RLGL.State.currentShaderLocs[RL_SHADER_LOC_MAP_DIFFUSE], 0);  // Active default sampler2D: texture0

      // Activate additional sampler textures
      // Those additional textures will be common for all draw calls of the batch
      for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) {
        if (RLGL.State.activeTextureId[i] > 0) {
          pfActiveTexture(PF_TEXTURE0 + 1 + i);
          pfBindTexture(PF_TEXTURE_2D, RLGL.State.activeTextureId[i]);
        }
      }

      // Activate default sampler2D texture0 (one texture is always active for default batch shader)
      // NOTE: Batch system accumulates calls by texture0 changes, additional textures are enabled for all the draw calls
      pfActiveTexture(PF_TEXTURE0);

      for (int i = 0, vertexOffset = 0; i < batch->drawCounter; i++) {
        // Bind current draw call texture, activated as PF_TEXTURE0 and Bound to sampler2D texture0 by default
        pfBindTexture(PF_TEXTURE_2D, batch->draws[i].textureId);

        if ((batch->draws[i].mode == RL_LINES) || (batch->draws[i].mode == RL_TRIANGLES)) pfDrawArrays(batch->draws[i].mode, vertexOffset, batch->draws[i].vertexCount);
        else {
#if defined(GRAPHICS_API_OPENGL_33)
          // We need to define the number of indices to be processed: elementCount*6
          // NOTE: The final parameter tells the GPU the offset in bytes from the
          // start of the index buffer to the location of the first index to process
          pfDrawElements(PF_TRIANGLES, batch->draws[i].vertexCount/4*6, PF_UNSIGNED_INT, (GLvoid *)(vertexOffset/4*6*sizeof(GLuint)));
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
          pfDrawElements(PF_TRIANGLES, batch->draws[i].vertexCount/4*6, PF_UNSIGNED_SHORT, (GLvoid *)(vertexOffset/4*6*sizeof(GLushort)));
#endif
        }

        vertexOffset += (batch->draws[i].vertexCount + batch->draws[i].vertexAlignment);
      }

      if (!RLGL.ExtSupported.vao) {
        pfBindBuffer(PF_ARRAY_BUFFER, 0);
        pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, 0);
      }

      pfBindTexture(PF_TEXTURE_2D, 0);    // Unbind textures
    }

    if (RLGL.ExtSupported.vao) pfBindVertexArray(0); // Unbind VAO

    pfUseProgram(0);    // Unbind shader program
  }

  // Restore viewport to default measures
  if (eyeCount == 2) rlViewport(0, 0, RLGL.State.framebufferWidth, RLGL.State.framebufferHeight);
  //------------------------------------------------------------------------------------------------------------

  // Reset batch buffers
  //------------------------------------------------------------------------------------------------------------
  // Reset vertex counter for next frame
  RLGL.State.vertexCounter = 0;

  // Reset depth for next draw
  batch->currentDepth = -1.0f;

  // Restore projection/modelview matrices
  RLGL.State.projection = matProjection;
  RLGL.State.modelview = matModelView;

  // Reset RLGL.currentBatch->draws array
  for (int i = 0; i < RL_DEFAULT_BATCH_DRAWCALLS; i++) {
    batch->draws[i].mode = RL_QUADS;
    batch->draws[i].vertexCount = 0;
    batch->draws[i].textureId = RLGL.State.defaultTextureId;
  }

  // Reset active texture units for next batch
  for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) RLGL.State.activeTextureId[i] = 0;

  // Reset draws counter to one draw for the batch
  batch->drawCounter = 1;
  //------------------------------------------------------------------------------------------------------------

  // Change to next buffer in the list (in case of multi-buffering)
  batch->currentBuffer++;
  if (batch->currentBuffer >= batch->bufferCount) batch->currentBuffer = 0;
#endif
}

// Set the active render batch for rlgl
void
rlSetRenderBatchActive(rlRenderBatch *batch)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  rlDrawRenderBatch(RLGL.currentBatch);

  if (batch != NULL) RLGL.currentBatch = batch;
  else RLGL.currentBatch = &RLGL.defaultBatch;
#endif
}

// Update and draw internal render batch
void
rlDrawRenderBatchActive(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  rlDrawRenderBatch(RLGL.currentBatch);    // NOTE: Stereo rendering is checked inside
#endif
}

// Check internal buffer overflow for a given number of vertex
// and force a rlRenderBatch draw call if required
bool
rlCheckRenderBatchLimit(int vCount)
{
  bool overflow = false;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if ((RLGL.State.vertexCounter + vCount) >=
      (RLGL.currentBatch->vertexBuffer[RLGL.currentBatch->currentBuffer].elementCount*4)) {
    overflow = true;

    // Store current primitive drawing mode and texture id
    int currentMode = RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode;
    int currentTexture = RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId;

    rlDrawRenderBatch(RLGL.currentBatch);    // NOTE: Stereo rendering is checked inside

    // Restore state of last batch so we can continue adding vertices
    RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].mode = currentMode;
    RLGL.currentBatch->draws[RLGL.currentBatch->drawCounter - 1].textureId = currentTexture;
  }
#endif

  return overflow;
}

// Textures data management
//-----------------------------------------------------------------------------------------
// Convert image data to OpenGL texture (returns id)
unsigned int
rlLoadTexture(const void *_data, int width, int height, int format, int mipmapCount)
{
  unsigned int id, sz = rlGetPixelDataSize(width, height, format);
  void *data = malloc(sz);
  memcpy(data, _data, sz);
  PFtexture *v = malloc(sizeof(PFtexture));
  *v = pfGenTexture(data, width, height, format);
  id = pfStoreTexture(v);

  TRACELOG(RL_LOG_INFO, "TEXTURE: [ID %d, PTR %p] Texture loaded successfully (%ix%i | %s)",
           id, v, v->width, v->height, rlGetPixelFormatName(format));

  return id;
}

// Load depth texture/renderbuffer (to be attached to fbo)
// WARNING: OpenGL ES 2.0 requires PF_OES_depth_texture and WebGL requires WEBPF_depth_texture extensions
unsigned int
rlLoadTextureDepth(int width, int height, bool useRenderBuffer)
{
  unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // In case depth textures not supported, we force renderbuffer usage
  if (!RLGL.ExtSupported.texDepth) useRenderBuffer = true;

  // NOTE: We let the implementation to choose the best bit-depth
  // Possible formats: PF_DEPTH_COMPONENT16, PF_DEPTH_COMPONENT24, PF_DEPTH_COMPONENT32 and PF_DEPTH_COMPONENT32F
  unsigned int pfInternalFormat = PF_DEPTH_COMPONENT;

#if (defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_ES3))
  // WARNING: WebGL platform requires unsized internal format definition (PF_DEPTH_COMPONENT)
  // while other platforms using OpenGL ES 2.0 require/support sized internal formats depending on the GPU capabilities
  if (!RLGL.ExtSupported.texDepthWebGL || useRenderBuffer) {
    if (RLGL.ExtSupported.maxDepthBits == 32) pfInternalFormat = PF_DEPTH_COMPONENT32_OES;
    else if (RLGL.ExtSupported.maxDepthBits == 24) pfInternalFormat = PF_DEPTH_COMPONENT24_OES;
    else pfInternalFormat = PF_DEPTH_COMPONENT16;
  }
#endif

  if (!useRenderBuffer && RLGL.ExtSupported.texDepth) {
    pfGenTextures(1, &id);
    pfBindTexture(PF_TEXTURE_2D, pfGetTexture(id));
    pfTexImage2D(PF_TEXTURE_2D, 0, pfInternalFormat, width, height, 0, PF_DEPTH_COMPONENT, PF_UNSIGNED_INT, NULL);

    pfTexParameteri(PF_TEXTURE_2D, PF_TEXTURE_MIN_FILTER, PF_NEAREST);
    pfTexParameteri(PF_TEXTURE_2D, PF_TEXTURE_MAG_FILTER, PF_NEAREST);
    pfTexParameteri(PF_TEXTURE_2D, PF_TEXTURE_WRAP_S, PF_CLAMP_TO_EDGE);
    pfTexParameteri(PF_TEXTURE_2D, PF_TEXTURE_WRAP_T, PF_CLAMP_TO_EDGE);

    pfBindTexture(PF_TEXTURE_2D, 0);

    TRACELOG(RL_LOG_INFO, "TEXTURE: Depth texture loaded successfully");
  } else {
    // Create the renderbuffer that will serve as the depth attachment for the framebuffer
    // NOTE: A renderbuffer is simpler than a texture and could offer better performance on embedded devices
    pfGenRenderbuffers(1, &id);
    pfBindRenderbuffer(PF_RENDERBUFFER, id);
    pfRenderbufferStorage(PF_RENDERBUFFER, pfInternalFormat, width, height);

    pfBindRenderbuffer(PF_RENDERBUFFER, 0);

    TRACELOG(RL_LOG_INFO, "TEXTURE: [ID %i] Depth renderbuffer loaded successfully (%i bits)", id, (RLGL.ExtSupported.maxDepthBits >= 24)? RLGL.ExtSupported.maxDepthBits : 16);
  }
#endif

  return id;
}

// Load texture cubemap
// NOTE: Cubemap data is expected to be 6 images in a single data array (one after the other),
// expected the following convention: +X, -X, +Y, -Y, +Z, -Z
unsigned int
rlLoadTextureCubemap(const void *data, int size, int format)
{
  unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  unsigned int dataSize = rlGetPixelDataSize(size, size, format);

  pfGenTextures(1, &id);
  pfBindTexture(PF_TEXTURE_CUBE_MAP, pfGetTexture(id));

  unsigned int pfInternalFormat, pfFormat, pfType;
  rlGetGlTextureFormats(format, &pfInternalFormat, &pfFormat, &pfType);

  if (pfInternalFormat != 0) {
    // Load cubemap faces
    for (unsigned int i = 0; i < 6; i++) {
      if (data == NULL) {
        if (format < RL_PIXELFORMAT_COMPRESSED_DXT1_RGB) {
          if ((format == RL_PIXELFORMAT_UNCOMPRESSED_R32) || (format == RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32)
              || (format == RL_PIXELFORMAT_UNCOMPRESSED_R16) || (format == RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16))
            TRACELOG(RL_LOG_WARNING, "TEXTURES: Cubemap requested format not supported");
          else pfTexImage2D(PF_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, pfInternalFormat, size, size, 0, pfFormat, pfType, NULL);
        } else TRACELOG(RL_LOG_WARNING, "TEXTURES: Empty cubemap creation does not support compressed format");
      } else {
        if (format < RL_PIXELFORMAT_COMPRESSED_DXT1_RGB) pfTexImage2D(PF_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, pfInternalFormat, size, size, 0, pfFormat, pfType, (unsigned char *)data + i*dataSize);
        else pfCompressedTexImage2D(PF_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, pfInternalFormat, size, size, 0, dataSize, (unsigned char *)data + i*dataSize);
      }

#if defined(GRAPHICS_API_OPENGL_33)
      if (format == RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE) {
        PFint swizzleMask[] = { PF_RED, PF_RED, PF_RED, PF_ONE };
        pfTexParameteriv(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_SWIZZLE_RGBA, swizzleMask);
      } else if (format == RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA) {
#if defined(GRAPHICS_API_OPENGL_21)
        PFint swizzleMask[] = { PF_RED, PF_RED, PF_RED, PF_ALPHA };
#elif defined(GRAPHICS_API_OPENGL_33)
        PFint swizzleMask[] = { PF_RED, PF_RED, PF_RED, PF_GREEN };
#endif
        pfTexParameteriv(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_SWIZZLE_RGBA, swizzleMask);
      }
#endif
    }
  }

  // Set cubemap texture sampling parameters
  pfTexParameteri(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_MIN_FILTER, PF_LINEAR);
  pfTexParameteri(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_MAG_FILTER, PF_LINEAR);
  pfTexParameteri(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_WRAP_S, PF_CLAMP_TO_EDGE);
  pfTexParameteri(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_WRAP_T, PF_CLAMP_TO_EDGE);
#if defined(GRAPHICS_API_OPENGL_33)
  pfTexParameteri(PF_TEXTURE_CUBE_MAP, PF_TEXTURE_WRAP_R, PF_CLAMP_TO_EDGE);  // Flag not supported on OpenGL ES 2.0
#endif

  pfBindTexture(PF_TEXTURE_CUBE_MAP, 0);
#endif

  if (id > 0) TRACELOG(RL_LOG_INFO, "TEXTURE: [ID %i] Cubemap texture loaded successfully (%ix%i)", id, size, size);
  else TRACELOG(RL_LOG_WARNING, "TEXTURE: Failed to load cubemap texture");

  return id;
}

// Update already loaded texture in GPU with new data
// NOTE: We don't know safely if internal texture format is the expected one...
void
rlUpdateTexture(unsigned int id, int offsetX, int offsetY, int width, int height, int format, const void *data)
{
  pfBindTexture(pfGetTexture(id));

  unsigned int pfInternalFormat, pfFormat, pfType;
  rlGetGlTextureFormats(format, &pfInternalFormat, &pfFormat, &pfType);

  if ((pfInternalFormat != 0) && (format < RL_PIXELFORMAT_COMPRESSED_DXT1_RGB)) {
    pfTexSubImage2D(PF_TEXTURE_2D, 0, offsetX, offsetY, width, height, pfFormat, pfType, data);
  } else TRACELOG(RL_LOG_WARNING, "TEXTURE: [ID %i] Failed to update for current texture format (%i)", id, format);
}

// Get OpenGL internal formats and data type from raylib PixelFormat
void
rlGetGlTextureFormats(int format, unsigned int *pfInternalFormat, unsigned int *pfFormat, unsigned int *pfType)
{
  *pfInternalFormat = 0;
  *pfFormat = 0;
  *pfType = 0;

  switch (format) {
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_21) || defined(GRAPHICS_API_OPENGL_ES2)
  // NOTE: on OpenGL ES 2.0 (WebGL), internalFormat must match format and options allowed are: PF_LUMINANCE, PF_RGB, PF_RGBA
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
    *pfInternalFormat = PF_LUMINANCE;
    *pfFormat = PF_LUMINANCE;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
    *pfInternalFormat = PF_LUMINANCE_ALPHA;
    *pfFormat = PF_LUMINANCE_ALPHA;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5:
    *pfInternalFormat = PF_RGB;
    *pfFormat = PF_RGB;
    *pfType = PF_UNSIGNED_SHORT_5_6_5;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8:
    *pfInternalFormat = PF_RGB;
    *pfFormat = PF_RGB;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
    *pfInternalFormat = PF_RGBA;
    *pfFormat = PF_RGBA;
    *pfType = PF_UNSIGNED_SHORT_5_5_5_1;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
    *pfInternalFormat = PF_RGBA;
    *pfFormat = PF_RGBA;
    *pfType = PF_UNSIGNED_SHORT_4_4_4_4;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
    *pfInternalFormat = PF_RGBA;
    *pfFormat = PF_RGBA;
    *pfType = PF_UNSIGNED_BYTE;
    break;
#if !defined(GRAPHICS_API_OPENGL_11)
#if defined(GRAPHICS_API_OPENGL_ES3)
  case RL_PIXELFORMAT_UNCOMPRESSED_R32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_R32F_EXT;
    *pfFormat = PF_RED_EXT;
    *pfType = PF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_RGB32F_EXT;
    *pfFormat = PF_RGB;
    *pfType = PF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_RGBA32F_EXT;
    *pfFormat = PF_RGBA;
    *pfType = PF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_R16F_EXT;
    *pfFormat = PF_RED_EXT;
    *pfType = PF_HALF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGB16F_EXT;
    *pfFormat = PF_RGB;
    *pfType = PF_HALF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGBA16F_EXT;
    *pfFormat = PF_RGBA;
    *pfType = PF_HALF_FLOAT;
    break;
#else
  case RL_PIXELFORMAT_UNCOMPRESSED_R32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_LUMINANCE;
    *pfFormat = PF_LUMINANCE;
    *pfType = PF_FLOAT;
    break;            // NOTE: Requires extension OES_texture_float
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_RGB;
    *pfFormat = PF_RGB;
    *pfType = PF_FLOAT;
    break;                  // NOTE: Requires extension OES_texture_float
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_RGBA;
    *pfFormat = PF_RGBA;
    *pfType = PF_FLOAT;
    break;             // NOTE: Requires extension OES_texture_float
#if defined(GRAPHICS_API_OPENGL_21)
  case RL_PIXELFORMAT_UNCOMPRESSED_R16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_LUMINANCE;
    *pfFormat = PF_LUMINANCE;
    *pfType = PF_HALF_FLOAT_ARB;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGB;
    *pfFormat = PF_RGB;
    *pfType = PF_HALF_FLOAT_ARB;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGBA;
    *pfFormat = PF_RGBA;
    *pfType = PF_HALF_FLOAT_ARB;
    break;
#else // defined(GRAPHICS_API_OPENGL_ES2)
  case RL_PIXELFORMAT_UNCOMPRESSED_R16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_LUMINANCE;
    *pfFormat = PF_LUMINANCE;
    *pfType = PF_HALF_FLOAT_OES;
    break;   // NOTE: Requires extension OES_texture_half_float
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGB;
    *pfFormat = PF_RGB;
    *pfType = PF_HALF_FLOAT_OES;
    break;         // NOTE: Requires extension OES_texture_half_float
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGBA;
    *pfFormat = PF_RGBA;
    *pfType = PF_HALF_FLOAT_OES;
    break;    // NOTE: Requires extension OES_texture_half_float
#endif
#endif
#endif
#elif defined(GRAPHICS_API_OPENGL_33)
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
    *pfInternalFormat = PF_R8;
    *pfFormat = PF_RED;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
    *pfInternalFormat = PF_RG8;
    *pfFormat = PF_RG;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5:
    *pfInternalFormat = PF_RGB565;
    *pfFormat = PF_RGB;
    *pfType = PF_UNSIGNED_SHORT_5_6_5;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8:
    *pfInternalFormat = PF_RGB8;
    *pfFormat = PF_RGB;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
    *pfInternalFormat = PF_RGB5_A1;
    *pfFormat = PF_RGBA;
    *pfType = PF_UNSIGNED_SHORT_5_5_5_1;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
    *pfInternalFormat = PF_RGBA4;
    *pfFormat = PF_RGBA;
    *pfType = PF_UNSIGNED_SHORT_4_4_4_4;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
    *pfInternalFormat = PF_RGBA8;
    *pfFormat = PF_RGBA;
    *pfType = PF_UNSIGNED_BYTE;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_R32F;
    *pfFormat = PF_RED;
    *pfType = PF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_RGB32F;
    *pfFormat = PF_RGB;
    *pfType = PF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
    if (RLGL.ExtSupported.texFloat32) *pfInternalFormat = PF_RGBA32F;
    *pfFormat = PF_RGBA;
    *pfType = PF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_R16F;
    *pfFormat = PF_RED;
    *pfType = PF_HALF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGB16F;
    *pfFormat = PF_RGB;
    *pfType = PF_HALF_FLOAT;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
    if (RLGL.ExtSupported.texFloat16) *pfInternalFormat = PF_RGBA16F;
    *pfFormat = PF_RGBA;
    *pfType = PF_HALF_FLOAT;
    break;
#endif
#if !defined(GRAPHICS_API_OPENGL_11)
  case RL_PIXELFORMAT_COMPRESSED_DXT1_RGB:
    if (RLGL.ExtSupported.texCompDXT) *pfInternalFormat = PF_COMPRESSED_RGB_S3TC_DXT1_EXT;
    break;
  case RL_PIXELFORMAT_COMPRESSED_DXT1_RGBA:
    if (RLGL.ExtSupported.texCompDXT) *pfInternalFormat = PF_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    break;
  case RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA:
    if (RLGL.ExtSupported.texCompDXT) *pfInternalFormat = PF_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    break;
  case RL_PIXELFORMAT_COMPRESSED_DXT5_RGBA:
    if (RLGL.ExtSupported.texCompDXT) *pfInternalFormat = PF_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    break;
  case RL_PIXELFORMAT_COMPRESSED_ETC1_RGB:
    if (RLGL.ExtSupported.texCompETC1) *pfInternalFormat = PF_ETC1_RGB8_OES;
    break;                      // NOTE: Requires OpenGL ES 2.0 or OpenGL 4.3
  case RL_PIXELFORMAT_COMPRESSED_ETC2_RGB:
    if (RLGL.ExtSupported.texCompETC2) *pfInternalFormat = PF_COMPRESSED_RGB8_ETC2;
    break;               // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
  case RL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA:
    if (RLGL.ExtSupported.texCompETC2) *pfInternalFormat = PF_COMPRESSED_RGBA8_ETC2_EAC;
    break;     // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
  case RL_PIXELFORMAT_COMPRESSED_PVRT_RGB:
    if (RLGL.ExtSupported.texCompPVRT) *pfInternalFormat = PF_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    break;    // NOTE: Requires PowerVR GPU
  case RL_PIXELFORMAT_COMPRESSED_PVRT_RGBA:
    if (RLGL.ExtSupported.texCompPVRT) *pfInternalFormat = PF_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    break;  // NOTE: Requires PowerVR GPU
  case RL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA:
    if (RLGL.ExtSupported.texCompASTC) *pfInternalFormat = PF_COMPRESSED_RGBA_ASTC_4x4_KHR;
    break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3
  case RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA:
    if (RLGL.ExtSupported.texCompASTC) *pfInternalFormat = PF_COMPRESSED_RGBA_ASTC_8x8_KHR;
    break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3
#endif
  default:
    TRACELOG(RL_LOG_WARNING, "TEXTURE: Current format not supported (%i)", format);
    break;
  }
}

// Unload texture from GPU memory
void
rlUnloadTexture(unsigned int id)
{
  pfDeleteTextures(1, &id);
}

// Generate mipmap data for selected texture
// NOTE: Only supports GPU mipmap generation
void
rlGenTextureMipmaps(unsigned int id, int width, int height, int format, int *mipmaps)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindTexture(PF_TEXTURE_2D, pfGetTexture(id));

  // Check if texture is power-of-two (POT)
  bool texIsPOT = false;

  if (((width > 0) && ((width & (width - 1)) == 0)) &&
      ((height > 0) && ((height & (height - 1)) == 0))) texIsPOT = true;

  if ((texIsPOT) || (RLGL.ExtSupported.texNPOT)) {
    //pfHint(PF_GENERATE_MIPMAP_HINT, PF_DONT_CARE);   // Hint for mipmaps generation algorithm: PF_FASTEST, PF_NICEST, PF_DONT_CARE
    pfGenerateMipmap(PF_TEXTURE_2D);    // Generate mipmaps automatically

#define MIN(a,b) (((a)<(b))? (a):(b))
#define MAX(a,b) (((a)>(b))? (a):(b))

    *mipmaps = 1 + (int)floor(log(MAX(width, height))/log(2));
    TRACELOG(RL_LOG_INFO, "TEXTURE: [ID %i] Mipmaps generated automatically, total: %i", id, *mipmaps);
  } else TRACELOG(RL_LOG_WARNING, "TEXTURE: [ID %i] Failed to generate mipmaps", id);

  pfBindTexture(PF_TEXTURE_2D, 0);
#else
  TRACELOG(RL_LOG_WARNING, "TEXTURE: [ID %i] GPU mipmap generation not supported", id);
#endif
}

// Read texture pixel data
void *
rlReadTexturePixels(unsigned int id, int width, int height, int format)
{
  void *pixels = NULL;

#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
  pfBindTexture(pfGetTexture(id));

  // NOTE: Using texture id, we can retrieve some texture info (but not on OpenGL ES 2.0)
  // Possible texture info: PF_TEXTURE_RED_SIZE, PF_TEXTURE_GREEN_SIZE, PF_TEXTURE_BLUE_SIZE, PF_TEXTURE_ALPHA_SIZE
  //int width, height, format;
  //pfGetTexLevelParameteriv(PF_TEXTURE_2D, 0, PF_TEXTURE_WIDTH, &width);
  //pfGetTexLevelParameteriv(PF_TEXTURE_2D, 0, PF_TEXTURE_HEIGHT, &height);
  //pfGetTexLevelParameteriv(PF_TEXTURE_2D, 0, PF_TEXTURE_INTERNAL_FORMAT, &format);

  // NOTE: Each row written to or read from by OpenGL pixel operations like pfGetTexImage are aligned to a 4 byte boundary by default, which may add some padding.
  // Use pfPixelStorei to modify padding with the PF_[UN]PACK_ALIGNMENT setting.
  // PF_PACK_ALIGNMENT affects operations that read from OpenGL memory (pfReadPixels, pfGetTexImage, etc.)
  // PF_UNPACK_ALIGNMENT affects operations that write to OpenGL memory (pfTexImage, etc.)
  pfPixelStorei(PF_PACK_ALIGNMENT, 1);

  unsigned int pfInternalFormat, pfFormat, pfType;
  rlGetGlTextureFormats(format, &pfInternalFormat, &pfFormat, &pfType);
  unsigned int size = rlGetPixelDataSize(width, height, format);

  if ((pfInternalFormat != 0) && (format < RL_PIXELFORMAT_COMPRESSED_DXT1_RGB)) {
    pixels = RL_MALLOC(size);
    pfGetTexImage(PF_TEXTURE_2D, 0, pfFormat, pfType, pixels);
  } else TRACELOG(RL_LOG_WARNING, "TEXTURE: [ID %i] Data retrieval not suported for pixel format (%i)", id, format);

  pfBindTexture(NULL);
#endif

#if defined(GRAPHICS_API_OPENGL_ES2)
  // pfGetTexImage() is not available on OpenGL ES 2.0
  // Texture width and height are required on OpenGL ES 2.0. There is no way to get it from texture id.
  // Two possible Options:
  // 1 - Bind texture to color fbo attachment and pfReadPixels()
  // 2 - Create an fbo, activate it, render quad with texture, pfReadPixels()
  // We are using Option 1, just need to care for texture format on retrieval
  // NOTE: This behaviour could be conditioned by graphic driver...
  unsigned int fboId = rlLoadFramebuffer();

  pfBindFramebuffer(PF_FRAMEBUFFER, fboId);
  pfBindTexture(PF_TEXTURE_2D, 0);

  // Attach our texture to FBO
  pfFramebufferTexture2D(PF_FRAMEBUFFER, PF_COLOR_ATTACHMENT0, PF_TEXTURE_2D, id, 0);

  // We read data as RGBA because FBO texture is configured as RGBA, despite binding another texture format
  pixels = (unsigned char *)RL_MALLOC(rlGetPixelDataSize(width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8));
  pfReadPixels(0, 0, width, height, PF_RGBA, PF_UNSIGNED_BYTE, pixels);

  pfBindFramebuffer(PF_FRAMEBUFFER, 0);

  // Clean up temporal fbo
  rlUnloadFramebuffer(fboId);
#endif

  return pixels;
}

// Read screen pixel data (color buffer)
unsigned char *
rlReadScreenPixels(int width, int height)
{
  unsigned char *screenData = (unsigned char *)RL_CALLOC(width*height*4, sizeof(unsigned char));

  // NOTE 1: pfReadPixels returns image flipped vertically -> (0,0) is the bottom left corner of the framebuffer
  // NOTE 2: We are getting alpha channel! Be careful, it can be transparent if not cleared properly!
  _pfReadPixels(0, 0, width, height, PF_RGBA, PF_UNSIGNED_BYTE, screenData);

  // Flip image vertically!
  unsigned char *imgData = (unsigned char *)RL_MALLOC(width*height*4*sizeof(unsigned char));

  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < (width*4); x++) {
      imgData[((height - 1) - y)*width*4 + x] = screenData[(y*width*4) + x];  // Flip line

      // Set alpha component value to 255 (no trasparent image retrieval)
      // NOTE: Alpha value has already been applied to RGB in framebuffer, we don't need it!
      if (((x + 1)%4) == 0) imgData[((height - 1) - y)*width*4 + x] = 255;
    }
  }

  RL_FREE(screenData);

  return imgData;     // NOTE: image data should be freed
}

// Framebuffer management (fbo)
//-----------------------------------------------------------------------------------------
// Load a framebuffer to be used for rendering
// NOTE: No textures attached
unsigned int
rlLoadFramebuffer(void)
{
  unsigned int fboId = 0;

#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfGenFramebuffers(1, &fboId);       // Create the framebuffer object
  pfBindFramebuffer(PF_FRAMEBUFFER, 0);   // Unbind any framebuffer
#endif

  return fboId;
}

// Attach color buffer texture to an fbo (unloads previous attachment)
// NOTE: Attach type: 0-Color, 1-Depth renderbuffer, 2-Depth texture
void
rlFramebufferAttach(unsigned int fboId, unsigned int texId, int attachType, int texType, int mipLevel)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfBindFramebuffer(PF_FRAMEBUFFER, fboId);

  switch (attachType) {
  case RL_ATTACHMENT_COLOR_CHANNEL0:
  case RL_ATTACHMENT_COLOR_CHANNEL1:
  case RL_ATTACHMENT_COLOR_CHANNEL2:
  case RL_ATTACHMENT_COLOR_CHANNEL3:
  case RL_ATTACHMENT_COLOR_CHANNEL4:
  case RL_ATTACHMENT_COLOR_CHANNEL5:
  case RL_ATTACHMENT_COLOR_CHANNEL6:
  case RL_ATTACHMENT_COLOR_CHANNEL7: {
    if (texType == RL_ATTACHMENT_TEXTURE2D) pfFramebufferTexture2D(PF_FRAMEBUFFER, PF_COLOR_ATTACHMENT0 + attachType, PF_TEXTURE_2D, texId, mipLevel);
    else if (texType == RL_ATTACHMENT_RENDERBUFFER) pfFramebufferRenderbuffer(PF_FRAMEBUFFER, PF_COLOR_ATTACHMENT0 + attachType, PF_RENDERBUFFER, texId);
    else if (texType >= RL_ATTACHMENT_CUBEMAP_POSITIVE_X) pfFramebufferTexture2D(PF_FRAMEBUFFER, PF_COLOR_ATTACHMENT0 + attachType, PF_TEXTURE_CUBE_MAP_POSITIVE_X + texType, texId, mipLevel);

  }
  break;
  case RL_ATTACHMENT_DEPTH: {
    if (texType == RL_ATTACHMENT_TEXTURE2D) pfFramebufferTexture2D(PF_FRAMEBUFFER, PF_DEPTH_ATTACHMENT, PF_TEXTURE_2D, texId, mipLevel);
    else if (texType == RL_ATTACHMENT_RENDERBUFFER)  pfFramebufferRenderbuffer(PF_FRAMEBUFFER, PF_DEPTH_ATTACHMENT, PF_RENDERBUFFER, texId);

  }
  break;
  case RL_ATTACHMENT_STENCIL: {
    if (texType == RL_ATTACHMENT_TEXTURE2D) pfFramebufferTexture2D(PF_FRAMEBUFFER, PF_STENCIL_ATTACHMENT, PF_TEXTURE_2D, texId, mipLevel);
    else if (texType == RL_ATTACHMENT_RENDERBUFFER)  pfFramebufferRenderbuffer(PF_FRAMEBUFFER, PF_STENCIL_ATTACHMENT, PF_RENDERBUFFER, texId);

  }
  break;
  default:
    break;
  }

  pfBindFramebuffer(PF_FRAMEBUFFER, 0);
#endif
}

// Verify render texture is complete
bool
rlFramebufferComplete(unsigned int id)
{
  bool result = false;

#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  pfBindFramebuffer(PF_FRAMEBUFFER, id);

  PFenum status = pfCheckFramebufferStatus(PF_FRAMEBUFFER);

  if (status != PF_FRAMEBUFFER_COMPLETE) {
    switch (status) {
    case PF_FRAMEBUFFER_UNSUPPORTED:
      TRACELOG(RL_LOG_WARNING, "FBO: [ID %i] Framebuffer is unsupported", id);
      break;
    case PF_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      TRACELOG(RL_LOG_WARNING, "FBO: [ID %i] Framebuffer has incomplete attachment", id);
      break;
#if defined(GRAPHICS_API_OPENGL_ES2)
    case PF_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      TRACELOG(RL_LOG_WARNING, "FBO: [ID %i] Framebuffer has incomplete dimensions", id);
      break;
#endif
    case PF_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      TRACELOG(RL_LOG_WARNING, "FBO: [ID %i] Framebuffer has a missing attachment", id);
      break;
    default:
      break;
    }
  }

  pfBindFramebuffer(PF_FRAMEBUFFER, 0);

  result = (status == PF_FRAMEBUFFER_COMPLETE);
#endif

  return result;
}

// Unload framebuffer from GPU memory
// NOTE: All attached textures/cubemaps/renderbuffers are also deleted
void
rlUnloadFramebuffer(unsigned int id)
{
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)) && defined(RLGL_RENDER_TEXTURES_HINT)
  // Query depth attachment to automatically delete texture/renderbuffer
  int depthType = 0, depthId = 0;
  pfBindFramebuffer(PF_FRAMEBUFFER, id);   // Bind framebuffer to query depth texture type
  pfGetFramebufferAttachmentParameteriv(PF_FRAMEBUFFER, PF_DEPTH_ATTACHMENT, PF_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depthType);

  // TODO: Review warning retrieving object name in WebGL
  // WARNING: WebGL: INVALID_ENUM: getFramebufferAttachmentParameter: invalid parameter name
  // https://registry.khronos.org/webgl/specs/latest/1.0/
  pfGetFramebufferAttachmentParameteriv(PF_FRAMEBUFFER, PF_DEPTH_ATTACHMENT, PF_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthId);

  unsigned int depthIdU = (unsigned int)depthId;
  if (depthType == PF_RENDERBUFFER) pfDeleteRenderbuffers(1, &depthIdU);
  else if (depthType == PF_TEXTURE) pfDeleteTextures(1, &depthIdU);

  // NOTE: If a texture object is deleted while its image is attached to the *currently bound* framebuffer,
  // the texture image is automatically detached from the currently bound framebuffer.

  pfBindFramebuffer(PF_FRAMEBUFFER, 0);
  pfDeleteFramebuffers(1, &id);

  TRACELOG(RL_LOG_INFO, "FBO: [ID %i] Unloaded framebuffer from VRAM (GPU)", id);
#endif
}

// Vertex data management
//-----------------------------------------------------------------------------------------
// Load a new attributes buffer
unsigned int
rlLoadVertexBuffer(const void *buffer, int size, bool dynamic)
{
  unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfGenBuffers(1, &id);
  pfBindBuffer(PF_ARRAY_BUFFER, id);
  pfBufferData(PF_ARRAY_BUFFER, size, buffer, dynamic? PF_DYNAMIC_DRAW : PF_STATIC_DRAW);
#endif

  return id;
}

// Load a new attributes element buffer
unsigned int
rlLoadVertexBufferElement(const void *buffer, int size, bool dynamic)
{
  unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfGenBuffers(1, &id);
  pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, id);
  pfBufferData(PF_ELEMENT_ARRAY_BUFFER, size, buffer, dynamic? PF_DYNAMIC_DRAW : PF_STATIC_DRAW);
#endif

  return id;
}

// Enable vertex buffer (VBO)
void
rlEnableVertexBuffer(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindBuffer(PF_ARRAY_BUFFER, id);
#endif
}

// Disable vertex buffer (VBO)
void
rlDisableVertexBuffer(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindBuffer(PF_ARRAY_BUFFER, 0);
#endif
}

// Enable vertex buffer element (VBO element)
void
rlEnableVertexBufferElement(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, id);
#endif
}

// Disable vertex buffer element (VBO element)
void
rlDisableVertexBufferElement(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, 0);
#endif
}

// Update vertex buffer with new data
// NOTE: dataSize and offset must be provided in bytes
void
rlUpdateVertexBuffer(unsigned int id, const void *data, int dataSize, int offset)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindBuffer(PF_ARRAY_BUFFER, id);
  pfBufferSubData(PF_ARRAY_BUFFER, offset, dataSize, data);
#endif
}

// Update vertex buffer elements with new data
// NOTE: dataSize and offset must be provided in bytes
void
rlUpdateVertexBufferElements(unsigned int id, const void *data, int dataSize, int offset)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfBindBuffer(PF_ELEMENT_ARRAY_BUFFER, id);
  pfBufferSubData(PF_ELEMENT_ARRAY_BUFFER, offset, dataSize, data);
#endif
}

// Enable vertex array object (VAO)
bool
rlEnableVertexArray(unsigned int vaoId)
{
  bool result = false;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if (RLGL.ExtSupported.vao) {
    pfBindVertexArray(vaoId);
    result = true;
  }
#endif
  return result;
}

// Disable vertex array object (VAO)
void
rlDisableVertexArray(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if (RLGL.ExtSupported.vao) pfBindVertexArray(0);
#endif
}

// Enable vertex attribute index
void
rlEnableVertexAttribute(unsigned int index)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfEnableVertexAttribArray(index);
#endif
}

// Disable vertex attribute index
void
rlDisableVertexAttribute(unsigned int index)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfDisableVertexAttribArray(index);
#endif
}

// Draw vertex array
void
rlDrawVertexArray(int offset, int count)
{
  pfDrawArrays(PF_TRIANGLES, offset, count);
}

// Draw vertex array elements
void
rlDrawVertexArrayElements(int offset, int count, const void *buffer)
{
  // NOTE: Added pointer math separately from function to avoid UBSAN complaining
  unsigned short *bufferPtr = (unsigned short *)buffer;
  if (offset > 0) bufferPtr += offset;

  pfDrawElements(PF_TRIANGLES, count, PF_UNSIGNED_SHORT, (const unsigned short *)bufferPtr);
}

// Draw vertex array instanced
void
rlDrawVertexArrayInstanced(int offset, int count, int instances)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfDrawArraysInstanced(PF_TRIANGLES, 0, count, instances);
#endif
}

// Draw vertex array elements instanced
void
rlDrawVertexArrayElementsInstanced(int offset, int count, const void *buffer, int instances)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // NOTE: Added pointer math separately from function to avoid UBSAN complaining
  unsigned short *bufferPtr = (unsigned short *)buffer;
  if (offset > 0) bufferPtr += offset;

  pfDrawElementsInstanced(PF_TRIANGLES, count, PF_UNSIGNED_SHORT, (const unsigned short *)bufferPtr, instances);
#endif
}

#if defined(GRAPHICS_API_OPENGL_11)
// Enable vertex state pointer
void
rlEnableStatePointer(int vertexAttribType, void *buffer)
{
  if (buffer != NULL) pfEnableClientState(vertexAttribType);
  switch (vertexAttribType) {
  case PF_VERTEX_ARRAY:
    pfVertexPointer(3, PF_FLOAT, 0, buffer);
    break;
  case PF_TEXTURE_COORD_ARRAY:
    _pfTexCoordPointer(2, PF_FLOAT, 0, buffer);
    break;
  case PF_NORMAL_ARRAY:
    if (buffer != NULL) pfNormalPointer(PF_FLOAT, 0, buffer);
    break;
  case PF_COLOR_ARRAY:
    if (buffer != NULL) pfColorPointer(4, PF_UNSIGNED_BYTE, 0, buffer);
    break;
  //case PF_INDEX_ARRAY: if (buffer != NULL) pfIndexPointer(PF_SHORT, 0, buffer); break; // Indexed colors
  default:
    break;
  }
}

// Disable vertex state pointer
void
rlDisableStatePointer(int vertexAttribType)
{
  pfDisableClientState(vertexAttribType);
}
#endif

// Load vertex array object (VAO)
unsigned int
rlLoadVertexArray(void)
{
  unsigned int vaoId = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if (RLGL.ExtSupported.vao) {
    pfGenVertexArrays(1, &vaoId);
  }
#endif
  return vaoId;
}

// Set vertex attribute
void
rlSetVertexAttribute(unsigned int index, int compSize, int type, bool normalized, int stride, int offset)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // NOTE: Data type could be: PF_BYTE, PF_UNSIGNED_BYTE, PF_SHORT, PF_UNSIGNED_SHORT, PF_INT, PF_UNSIGNED_INT
  // Additional types (depends on OpenGL version or extensions):
  //  - PF_HALF_FLOAT, PF_FLOAT, PF_DOUBLE, PF_FIXED,
  //  - PF_INT_2_10_10_10_REV, PF_UNSIGNED_INT_2_10_10_10_REV, PF_UNSIGNED_INT_10F_11F_11F_REV
  pfVertexAttribPointer(index, compSize, type, normalized, stride, (void *)offset);
#endif
}

// Set vertex attribute divisor
void
rlSetVertexAttributeDivisor(unsigned int index, int divisor)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfVertexAttribDivisor(index, divisor);
#endif
}

// Unload vertex array object (VAO)
void
rlUnloadVertexArray(unsigned int vaoId)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if (RLGL.ExtSupported.vao) {
    pfBindVertexArray(0);
    pfDeleteVertexArrays(1, &vaoId);
    TRACELOG(RL_LOG_INFO, "VAO: [ID %i] Unloaded vertex array data from VRAM (GPU)", vaoId);
  }
#endif
}

// Unload vertex buffer (VBO)
void
rlUnloadVertexBuffer(unsigned int vboId)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfDeleteBuffers(1, &vboId);
  //TRACELOG(RL_LOG_INFO, "VBO: Unloaded vertex data from VRAM (GPU)");
#endif
}

// Shaders management
//-----------------------------------------------------------------------------------------------
// Load shader from code strings
// NOTE: If shader string is NULL, using default vertex/fragment shaders
unsigned int
rlLoadShaderCode(const char *vsCode, const char *fsCode)
{
  unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  unsigned int vertexShaderId = 0;
  unsigned int fragmentShaderId = 0;

  // Compile vertex shader (if provided)
  if (vsCode != NULL) vertexShaderId = rlCompileShader(vsCode, PF_VERTEX_SHADER);
  // In case no vertex shader was provided or compilation failed, we use default vertex shader
  if (vertexShaderId == 0) vertexShaderId = RLGL.State.defaultVShaderId;

  // Compile fragment shader (if provided)
  if (fsCode != NULL) fragmentShaderId = rlCompileShader(fsCode, PF_FRAGMENT_SHADER);
  // In case no fragment shader was provided or compilation failed, we use default fragment shader
  if (fragmentShaderId == 0) fragmentShaderId = RLGL.State.defaultFShaderId;

  // In case vertex and fragment shader are the default ones, no need to recompile, we can just assign the default shader program id
  if ((vertexShaderId == RLGL.State.defaultVShaderId) && (fragmentShaderId == RLGL.State.defaultFShaderId)) id = RLGL.State.defaultShaderId;
  else {
    // One of or both shader are new, we need to compile a new shader program
    id = rlLoadShaderProgram(vertexShaderId, fragmentShaderId);

    // We can detach and delete vertex/fragment shaders (if not default ones)
    // NOTE: We detach shader before deletion to make sure memory is freed
    if (vertexShaderId != RLGL.State.defaultVShaderId) {
      // WARNING: Shader program linkage could fail and returned id is 0
      if (id > 0) pfDetachShader(id, vertexShaderId);
      pfDeleteShader(vertexShaderId);
    }
    if (fragmentShaderId != RLGL.State.defaultFShaderId) {
      // WARNING: Shader program linkage could fail and returned id is 0
      if (id > 0) pfDetachShader(id, fragmentShaderId);
      pfDeleteShader(fragmentShaderId);
    }

    // In case shader program loading failed, we assign default shader
    if (id == 0) {
      // In case shader loading fails, we return the default shader
      TRACELOG(RL_LOG_WARNING, "SHADER: Failed to load custom shader code, using default shader");
      id = RLGL.State.defaultShaderId;
    }
    /*
    else
    {
        // Get available shader uniforms
        // NOTE: This information is useful for debug...
        int uniformCount = -1;
        pfGetProgramiv(id, PF_ACTIVE_UNIFORMS, &uniformCount);

        for (int i = 0; i < uniformCount; i++)
        {
            int namelen = -1;
            int num = -1;
            char name[256] = { 0 };     // Assume no variable names longer than 256
            PFenum type = PF_ZERO;

            // Get the name of the uniforms
            pfGetActiveUniform(id, i, sizeof(name) - 1, &namelen, &num, &type, name);

            name[namelen] = 0;
            TRACELOGD("SHADER: [ID %i] Active uniform (%s) set at location: %i", id, name, pfGetUniformLocation(id, name));
        }
    }
    */
  }
#endif

  return id;
}

// Compile custom shader and return shader id
unsigned int
rlCompileShader(const char *shaderCode, int type)
{
  unsigned int shader = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  shader = pfCreateShader(type);
  pfShaderSource(shader, 1, &shaderCode, NULL);

  PFint success = 0;
  pfCompileShader(shader);
  pfGetShaderiv(shader, PF_COMPILE_STATUS, &success);

  if (success == PF_FALSE) {
    switch (type) {
    case PF_VERTEX_SHADER:
      TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to compile vertex shader code", shader);
      break;
    case PF_FRAGMENT_SHADER:
      TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to compile fragment shader code", shader);
      break;
      //case PF_GEOMETRY_SHADER:
#if defined(GRAPHICS_API_OPENGL_43)
    case PF_COMPUTE_SHADER:
      TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to compile compute shader code", shader);
      break;
#endif
    default:
      break;
    }

    int maxLength = 0;
    pfGetShaderiv(shader, PF_INFO_LOG_LENGTH, &maxLength);

    if (maxLength > 0) {
      int length = 0;
      char *log = (char *)RL_CALLOC(maxLength, sizeof(char));
      pfGetShaderInfoLog(shader, maxLength, &length, log);
      TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Compile error: %s", shader, log);
      RL_FREE(log);
    }
  } else {
    switch (type) {
    case PF_VERTEX_SHADER:
      TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Vertex shader compiled successfully", shader);
      break;
    case PF_FRAGMENT_SHADER:
      TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Fragment shader compiled successfully", shader);
      break;
      //case PF_GEOMETRY_SHADER:
#if defined(GRAPHICS_API_OPENGL_43)
    case PF_COMPUTE_SHADER:
      TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Compute shader compiled successfully", shader);
      break;
#endif
    default:
      break;
    }
  }
#endif

  return shader;
}

// Load custom shader strings and return program id
unsigned int
rlLoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId)
{
  unsigned int program = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  PFint success = 0;
  program = pfCreateProgram();

  pfAttachShader(program, vShaderId);
  pfAttachShader(program, fShaderId);

  // NOTE: Default attribute shader locations must be Bound before linking
  pfBindAttribLocation(program, RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
  pfBindAttribLocation(program, RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
  pfBindAttribLocation(program, RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL, RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
  pfBindAttribLocation(program, RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR, RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);
  pfBindAttribLocation(program, RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT, RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
  pfBindAttribLocation(program, RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);

  // NOTE: If some attrib name is no found on the shader, it locations becomes -1

  pfLinkProgram(program);

  // NOTE: All uniform variables are intitialised to 0 when a program links

  pfGetProgramiv(program, PF_LINK_STATUS, &success);

  if (success == PF_FALSE) {
    TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to link shader program", program);

    int maxLength = 0;
    pfGetProgramiv(program, PF_INFO_LOG_LENGTH, &maxLength);

    if (maxLength > 0) {
      int length = 0;
      char *log = (char *)RL_CALLOC(maxLength, sizeof(char));
      pfGetProgramInfoLog(program, maxLength, &length, log);
      TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Link error: %s", program, log);
      RL_FREE(log);
    }

    pfDeleteProgram(program);

    program = 0;
  } else {
    // Get the size of compiled shader program (not available on OpenGL ES 2.0)
    // NOTE: If PF_LINK_STATUS is PF_FALSE, program binary length is zero.
    //GLint binarySize = 0;
    //pfGetProgramiv(id, PF_PROGRAM_BINARY_LENGTH, &binarySize);

    TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Program shader loaded successfully", program);
  }
#endif
  return program;
}

// Unload shader program
void
rlUnloadShaderProgram(unsigned int id)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  pfDeleteProgram(id);

  TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Unloaded shader program data from VRAM (GPU)", id);
#endif
}

// Get shader location uniform
int
rlGetLocationUniform(unsigned int shaderId, const char *uniformName)
{
  int location = -1;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  location = pfGetUniformLocation(shaderId, uniformName);

  //if (location == -1) TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to find shader uniform: %s", shaderId, uniformName);
  //else TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Shader uniform (%s) set at location: %i", shaderId, uniformName, location);
#endif
  return location;
}

// Get shader location attribute
int
rlGetLocationAttrib(unsigned int shaderId, const char *attribName)
{
  int location = -1;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  location = pfGetAttribLocation(shaderId, attribName);

  //if (location == -1) TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to find shader attribute: %s", shaderId, attribName);
  //else TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Shader attribute (%s) set at location: %i", shaderId, attribName, location);
#endif
  return location;
}

// Set shader value uniform
void
rlSetUniform(int locIndex, const void *value, int uniformType, int count)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  switch (uniformType) {
  case RL_SHADER_UNIFORM_FLOAT:
    pfUniform1fv(locIndex, count, (float *)value);
    break;
  case RL_SHADER_UNIFORM_VEC2:
    pfUniform2fv(locIndex, count, (float *)value);
    break;
  case RL_SHADER_UNIFORM_VEC3:
    pfUniform3fv(locIndex, count, (float *)value);
    break;
  case RL_SHADER_UNIFORM_VEC4:
    pfUniform4fv(locIndex, count, (float *)value);
    break;
  case RL_SHADER_UNIFORM_INT:
    pfUniform1iv(locIndex, count, (int *)value);
    break;
  case RL_SHADER_UNIFORM_IVEC2:
    pfUniform2iv(locIndex, count, (int *)value);
    break;
  case RL_SHADER_UNIFORM_IVEC3:
    pfUniform3iv(locIndex, count, (int *)value);
    break;
  case RL_SHADER_UNIFORM_IVEC4:
    pfUniform4iv(locIndex, count, (int *)value);
    break;
  case RL_SHADER_UNIFORM_SAMPLER2D:
    pfUniform1iv(locIndex, count, (int *)value);
    break;
  default:
    TRACELOG(RL_LOG_WARNING, "SHADER: Failed to set uniform value, data type not recognized");
  }
#endif
}

// Set shader value attribute
void
rlSetVertexAttributeDefault(int locIndex, const void *value, int attribType, int count)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  switch (attribType) {
  case RL_SHADER_ATTRIB_FLOAT:
    if (count == 1) pfVertexAttrib1fv(locIndex, (float *)value);
    break;
  case RL_SHADER_ATTRIB_VEC2:
    if (count == 2) pfVertexAttrib2fv(locIndex, (float *)value);
    break;
  case RL_SHADER_ATTRIB_VEC3:
    if (count == 3) pfVertexAttrib3fv(locIndex, (float *)value);
    break;
  case RL_SHADER_ATTRIB_VEC4:
    if (count == 4) pfVertexAttrib4fv(locIndex, (float *)value);
    break;
  default:
    TRACELOG(RL_LOG_WARNING, "SHADER: Failed to set attrib default value, data type not recognized");
  }
#endif
}

// Set shader value uniform matrix
void
rlSetUniformMatrix(int locIndex, GETS(Matrix) mat)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  float matfloat[16] = {
    mat.m0, mat.m1, mat.m2, mat.m3,
    mat.m4, mat.m5, mat.m6, mat.m7,
    mat.m8, mat.m9, mat.m10, mat.m11,
    mat.m12, mat.m13, mat.m14, mat.m15
  };
  pfUniformMatrix4fv(locIndex, 1, false, matfloat);
#endif
}

// Set shader value uniform sampler
void
rlSetUniformSampler(int locIndex, unsigned int textureId)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // Check if texture is already active
  for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) {
    if (RLGL.State.activeTextureId[i] == textureId) {
      pfUniform1i(locIndex, 1 + i);
      return;
    }
  }

  // Register a new active texture for the internal batch system
  // NOTE: Default texture is always activated as PF_TEXTURE0
  for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) {
    if (RLGL.State.activeTextureId[i] == 0) {
      pfUniform1i(locIndex, 1 + i);              // Activate new texture unit
      RLGL.State.activeTextureId[i] = textureId; // Save texture id for binding on drawing
      break;
    }
  }
#endif
}

// Set shader currently active (id and locations)
void
rlSetShader(unsigned int id, int *locs)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  if (RLGL.State.currentShaderId != id) {
    rlDrawRenderBatch(RLGL.currentBatch);
    RLGL.State.currentShaderId = id;
    RLGL.State.currentShaderLocs = locs;
  }
#endif
}

// Load compute shader program
unsigned int
rlLoadComputeShaderProgram(unsigned int shaderId)
{
  unsigned int program = 0;

#if defined(GRAPHICS_API_OPENGL_43)
  PFint success = 0;
  program = pfCreateProgram();
  pfAttachShader(program, shaderId);
  pfLinkProgram(program);

  // NOTE: All uniform variables are intitialised to 0 when a program links

  pfGetProgramiv(program, PF_LINK_STATUS, &success);

  if (success == PF_FALSE) {
    TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to link compute shader program", program);

    int maxLength = 0;
    pfGetProgramiv(program, PF_INFO_LOG_LENGTH, &maxLength);

    if (maxLength > 0) {
      int length = 0;
      char *log = (char *)RL_CALLOC(maxLength, sizeof(char));
      pfGetProgramInfoLog(program, maxLength, &length, log);
      TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Link error: %s", program, log);
      RL_FREE(log);
    }

    pfDeleteProgram(program);

    program = 0;
  } else {
    // Get the size of compiled shader program (not available on OpenGL ES 2.0)
    // NOTE: If PF_LINK_STATUS is PF_FALSE, program binary length is zero.
    //GLint binarySize = 0;
    //pfGetProgramiv(id, PF_PROGRAM_BINARY_LENGTH, &binarySize);

    TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Compute shader program loaded successfully", program);
  }
#endif

  return program;
}

// Dispatch compute shader (equivalent to *draw* for graphics pilepine)
void
rlComputeShaderDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ)
{
#if defined(GRAPHICS_API_OPENGL_43)
  pfDispatchCompute(groupX, groupY, groupZ);
#endif
}

// Load shader storage buffer object (SSBO)
unsigned int
rlLoadShaderBuffer(unsigned int size, const void *data, int usageHint)
{
  unsigned int ssbo = 0;

#if defined(GRAPHICS_API_OPENGL_43)
  pfGenBuffers(1, &ssbo);
  pfBindBuffer(PF_SHADER_STORAGE_BUFFER, ssbo);
  pfBufferData(PF_SHADER_STORAGE_BUFFER, size, data, usageHint? usageHint : RL_STREAM_COPY);
  if (data == NULL) pfClearBufferData(PF_SHADER_STORAGE_BUFFER, PF_R8UI, PF_RED_INTEGER, PF_UNSIGNED_BYTE, NULL);    // Clear buffer data to 0
  pfBindBuffer(PF_SHADER_STORAGE_BUFFER, 0);
#endif

  return ssbo;
}

// Unload shader storage buffer object (SSBO)
void
rlUnloadShaderBuffer(unsigned int ssboId)
{
#if defined(GRAPHICS_API_OPENGL_43)
  pfDeleteBuffers(1, &ssboId);
#endif
}

// Update SSBO buffer data
void
rlUpdateShaderBuffer(unsigned int id, const void *data, unsigned int dataSize, unsigned int offset)
{
#if defined(GRAPHICS_API_OPENGL_43)
  pfBindBuffer(PF_SHADER_STORAGE_BUFFER, id);
  pfBufferSubData(PF_SHADER_STORAGE_BUFFER, offset, dataSize, data);
#endif
}

// Get SSBO buffer size
unsigned int
rlGetShaderBufferSize(unsigned int id)
{
  long long size = 0;

#if defined(GRAPHICS_API_OPENGL_43)
  pfBindBuffer(PF_SHADER_STORAGE_BUFFER, id);
  pfGetInteger64v(PF_SHADER_STORAGE_BUFFER_SIZE, &size);
#endif

  return (size > 0)? (unsigned int)size : 0;
}

// Read SSBO buffer data (GPU->CPU)
void
rlReadShaderBuffer(unsigned int id, void *dest, unsigned int count, unsigned int offset)
{
#if defined(GRAPHICS_API_OPENGL_43)
  pfBindBuffer(PF_SHADER_STORAGE_BUFFER, id);
  pfGetBufferSubData(PF_SHADER_STORAGE_BUFFER, offset, count, dest);
#endif
}

// Bind SSBO buffer
void
rlBindShaderBuffer(unsigned int id, unsigned int index)
{
#if defined(GRAPHICS_API_OPENGL_43)
  pfBindBufferBase(PF_SHADER_STORAGE_BUFFER, index, id);
#endif
}

// Copy SSBO buffer data
void
rlCopyShaderBuffer(unsigned int destId, unsigned int srcId, unsigned int destOffset, unsigned int srcOffset, unsigned int count)
{
#if defined(GRAPHICS_API_OPENGL_43)
  pfBindBuffer(PF_COPY_READ_BUFFER, srcId);
  pfBindBuffer(PF_COPY_WRITE_BUFFER, destId);
  pfCopyBufferSubData(PF_COPY_READ_BUFFER, PF_COPY_WRITE_BUFFER, srcOffset, destOffset, count);
#endif
}

// Bind image texture
void
rlBindImageTexture(unsigned int id, unsigned int index, int format, bool readonly)
{
#if defined(GRAPHICS_API_OPENGL_43)
  unsigned int pfInternalFormat = 0, pfFormat = 0, pfType = 0;

  rlGetGlTextureFormats(format, &pfInternalFormat, &pfFormat, &pfType);
  pfBindImageTexture(index, id, 0, 0, 0, readonly? PF_READ_ONLY : PF_READ_WRITE, pfInternalFormat);
#endif
}

// GETS(Matrix) state management
//-----------------------------------------------------------------------------------------
// Get internal modelview matrix
GETS(Matrix) rlGetMatrixModelview(void)
{
  GETS(Matrix) matrix = rlMatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_11)
  float mat[16];
  pfGetFloatv(PF_MODELVIEW_MATRIX, mat);
  matrix.m0 = mat[0];
  matrix.m1 = mat[1];
  matrix.m2 = mat[2];
  matrix.m3 = mat[3];
  matrix.m4 = mat[4];
  matrix.m5 = mat[5];
  matrix.m6 = mat[6];
  matrix.m7 = mat[7];
  matrix.m8 = mat[8];
  matrix.m9 = mat[9];
  matrix.m10 = mat[10];
  matrix.m11 = mat[11];
  matrix.m12 = mat[12];
  matrix.m13 = mat[13];
  matrix.m14 = mat[14];
  matrix.m15 = mat[15];
#else
  matrix = RLGL.State.modelview;
#endif
  return matrix;
}

// Get internal projection matrix
GETS(Matrix) rlGetMatrixProjection(void)
{
#if defined(GRAPHICS_API_OPENGL_11)
  float mat[16];
  pfGetFloatv(PF_PROJECTION_MATRIX,mat);
  GETS(Matrix) m;
  m.m0 = mat[0];
  m.m1 = mat[1];
  m.m2 = mat[2];
  m.m3 = mat[3];
  m.m4 = mat[4];
  m.m5 = mat[5];
  m.m6 = mat[6];
  m.m7 = mat[7];
  m.m8 = mat[8];
  m.m9 = mat[9];
  m.m10 = mat[10];
  m.m11 = mat[11];
  m.m12 = mat[12];
  m.m13 = mat[13];
  m.m14 = mat[14];
  m.m15 = mat[15];
  return m;
#else
  return RLGL.State.projection;
#endif
}

// Get internal accumulated transform matrix
GETS(Matrix) rlGetMatrixTransform(void)
{
  GETS(Matrix) mat = rlMatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  // TODO: Consider possible transform matrices in the RLGL.State.stack
  // Is this the right order? or should we start with the first stored matrix instead of the last one?
  //Matrix matStackTransform = rlMatrixIdentity();
  //for (int i = RLGL.State.stackCounter; i > 0; i--) matStackTransform = rlMatrixMultiply(RLGL.State.stack[i], matStackTransform);
  mat = RLGL.State.transform;
#endif
  return mat;
}

// Get internal projection matrix for stereo render (selected eye)
GETS(Matrix) rlGetMatrixProjectionStereo(int eye)
{
  GETS(Matrix) mat = rlMatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  mat = RLGL.State.projectionStereo[eye];
#endif
  return mat;
}

// Get internal view offset matrix for stereo render (selected eye)
GETS(Matrix) rlGetMatrixViewOffsetStereo(int eye)
{
  GETS(Matrix) mat = rlMatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  mat = RLGL.State.viewOffsetStereo[eye];
#endif
  return mat;
}

// Set a custom modelview matrix (replaces internal modelview matrix)
void
rlSetMatrixModelview(GETS(Matrix) view)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  RLGL.State.modelview = view;
#endif
}

// Set a custom projection matrix (replaces internal projection matrix)
void
rlSetMatrixProjection(GETS(Matrix) projection)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  RLGL.State.projection = projection;
#endif
}

// Set eyes projection matrices for stereo rendering
void
rlSetMatrixProjectionStereo(GETS(Matrix) right, GETS(Matrix) left)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  RLGL.State.projectionStereo[0] = right;
  RLGL.State.projectionStereo[1] = left;
#endif
}

// Set eyes view offsets matrices for stereo rendering
void
rlSetMatrixViewOffsetStereo(GETS(Matrix) right, GETS(Matrix) left)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  RLGL.State.viewOffsetStereo[0] = right;
  RLGL.State.viewOffsetStereo[1] = left;
#endif
}

// Load and draw a quad in NDC
void
rlLoadDrawQuad(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;

  float vertices[] = {
    // Positions         Texcoords
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
      1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
      1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    };

  // Gen VAO to contain VBO
  pfGenVertexArrays(1, &quadVAO);
  pfBindVertexArray(quadVAO);

  // Gen and fill vertex buffer (VBO)
  pfGenBuffers(1, &quadVBO);
  pfBindBuffer(PF_ARRAY_BUFFER, quadVBO);
  pfBufferData(PF_ARRAY_BUFFER, sizeof(vertices), &vertices, PF_STATIC_DRAW);

  // Bind vertex attributes (position, texcoords)
  pfEnableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
  pfVertexAttribPointer(RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, 3, PF_FLOAT, PF_FALSE, 5*sizeof(float), (void *)0); // Positions
  pfEnableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
  pfVertexAttribPointer(RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, 2, PF_FLOAT, PF_FALSE, 5*sizeof(float), (void *)(3*sizeof(float))); // Texcoords

  // Draw quad
  pfBindVertexArray(quadVAO);
  pfDrawArrays(PF_TRIANGLE_STRIP, 0, 4);
  pfBindVertexArray(0);

  // Delete buffers (VBO and VAO)
  pfDeleteBuffers(1, &quadVBO);
  pfDeleteVertexArrays(1, &quadVAO);
#endif
}

// Load and draw a cube in NDC
void
rlLoadDrawCube(void)
{
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
  unsigned int cubeVAO = 0;
  unsigned int cubeVBO = 0;

  float vertices[] = {
    // Positions          Normals               Texcoords
    -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
      1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
      1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
      -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
      1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
      1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
      1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
      -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
      -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
      -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
      -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
      1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
      1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
      1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
      1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
      1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
      -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
      1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
      1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
      1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
      -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
      -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
      -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
      1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
      1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
      1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
      -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
      -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f
    };

  // Gen VAO to contain VBO
  pfGenVertexArrays(1, &cubeVAO);
  pfBindVertexArray(cubeVAO);

  // Gen and fill vertex buffer (VBO)
  pfGenBuffers(1, &cubeVBO);
  pfBindBuffer(PF_ARRAY_BUFFER, cubeVBO);
  pfBufferData(PF_ARRAY_BUFFER, sizeof(vertices), vertices, PF_STATIC_DRAW);

  // Bind vertex attributes (position, normals, texcoords)
  pfBindVertexArray(cubeVAO);
  pfEnableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
  pfVertexAttribPointer(RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, 3, PF_FLOAT, PF_FALSE, 8*sizeof(float), (void *)0); // Positions
  pfEnableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL);
  pfVertexAttribPointer(RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL, 3, PF_FLOAT, PF_FALSE, 8*sizeof(float), (void *)(3*sizeof(float))); // Normals
  pfEnableVertexAttribArray(RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
  pfVertexAttribPointer(RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, 2, PF_FLOAT, PF_FALSE, 8*sizeof(float), (void *)(6*sizeof(float))); // Texcoords
  pfBindBuffer(PF_ARRAY_BUFFER, 0);
  pfBindVertexArray(0);

  // Draw cube
  pfBindVertexArray(cubeVAO);
  pfDrawArrays(PF_TRIANGLES, 0, 36);
  pfBindVertexArray(0);

  // Delete VBO and VAO
  pfDeleteBuffers(1, &cubeVBO);
  pfDeleteVertexArrays(1, &cubeVAO);
#endif
}

// Get name string for pixel format
const char *
rlGetPixelFormatName(unsigned int format)
{
  switch (format) {
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
    return "GRAYSCALE";
    break;         // 8 bit per pixel (no alpha)
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
    return "GRAY_ALPHA";
    break;       // 8*2 bpp (2 channels)
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5:
    return "R5G6B5";
    break;               // 16 bpp
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8:
    return "R8G8B8";
    break;               // 24 bpp
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
    return "R5G5B5A1";
    break;           // 16 bpp (1 bit alpha)
  case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
    return "R4G4B4A4";
    break;           // 16 bpp (4 bit alpha)
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
    return "R8G8B8A8";
    break;           // 32 bpp
  case RL_PIXELFORMAT_UNCOMPRESSED_R32:
    return "R32";
    break;                     // 32 bpp (1 channel - float)
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32:
    return "R32G32B32";
    break;         // 32*3 bpp (3 channels - float)
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
    return "R32G32B32A32";
    break;   // 32*4 bpp (4 channels - float)
  case RL_PIXELFORMAT_UNCOMPRESSED_R16:
    return "R16";
    break;                     // 16 bpp (1 channel - half float)
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16:
    return "R16G16B16";
    break;         // 16*3 bpp (3 channels - half float)
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
    return "R16G16B16A16";
    break;   // 16*4 bpp (4 channels - half float)
  default:
    return "Unsupported.";
    break;
  }
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Load default shader (just vertex positioning and texture coloring)
// NOTE: This shader program is used for internal buffers
// NOTE: Loaded: RLGL.State.defaultShaderId, RLGL.State.defaultShaderLocs
static void
rlLoadShaderDefault(void)
{
  RLGL.State.defaultShaderLocs = (int *)RL_CALLOC(RL_MAX_SHADER_LOCATIONS, sizeof(int));

  // NOTE: All locations must be reseted to -1 (no location)
  for (int i = 0; i < RL_MAX_SHADER_LOCATIONS; i++) RLGL.State.defaultShaderLocs[i] = -1;

  // Vertex shader directly defined, no external file required
  const char *defaultVShaderCode =
#if defined(GRAPHICS_API_OPENGL_21)
    "#version 120                       \n"
    "attribute vec3 vertexPosition;     \n"
    "attribute vec2 vertexTexCoord;     \n"
    "attribute vec4 vertexColor;        \n"
    "varying vec2 fragTexCoord;         \n"
    "varying vec4 fragColor;            \n"
#elif defined(GRAPHICS_API_OPENGL_33)
    "#version 330                       \n"
    "in vec3 vertexPosition;            \n"
    "in vec2 vertexTexCoord;            \n"
    "in vec4 vertexColor;               \n"
    "out vec2 fragTexCoord;             \n"
    "out vec4 fragColor;                \n"
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
    "#version 100                       \n"
    "precision mediump float;           \n"     // Precision required for OpenGL ES2 (WebGL) (on some browsers)
    "attribute vec3 vertexPosition;     \n"
    "attribute vec2 vertexTexCoord;     \n"
    "attribute vec4 vertexColor;        \n"
    "varying vec2 fragTexCoord;         \n"
    "varying vec4 fragColor;            \n"
#endif
    "uniform mat4 mvp;                  \n"
    "void main()                        \n"
    "{                                  \n"
    "    fragTexCoord = vertexTexCoord; \n"
    "    fragColor = vertexColor;       \n"
    "    pf_Position = mvp*vec4(vertexPosition, 1.0); \n"
    "}                                  \n";

  // Fragment shader directly defined, no external file required
  const char *defaultFShaderCode =
#if defined(GRAPHICS_API_OPENGL_21)
    "#version 120                       \n"
    "varying vec2 fragTexCoord;         \n"
    "varying vec4 fragColor;            \n"
    "uniform sampler2D texture0;        \n"
    "uniform vec4 colDiffuse;           \n"
    "void main()                        \n"
    "{                                  \n"
    "    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
    "    pf_FragColor = texelColor*colDiffuse*fragColor;      \n"
    "}                                  \n";
#elif defined(GRAPHICS_API_OPENGL_33)
    "#version 330       \n"
    "in vec2 fragTexCoord;              \n"
    "in vec4 fragColor;                 \n"
    "out vec4 finalColor;               \n"
    "uniform sampler2D texture0;        \n"
    "uniform vec4 colDiffuse;           \n"
    "void main()                        \n"
    "{                                  \n"
    "    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
    "    finalColor = texelColor*colDiffuse*fragColor;        \n"
    "}                                  \n";
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
  "#version 100                       \n"
  "precision mediump float;           \n"     // Precision required for OpenGL ES2 (WebGL)
  "varying vec2 fragTexCoord;         \n"
  "varying vec4 fragColor;            \n"
  "uniform sampler2D texture0;        \n"
  "uniform vec4 colDiffuse;           \n"
  "void main()                        \n"
  "{                                  \n"
  "    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
  "    pf_FragColor = texelColor*colDiffuse*fragColor;      \n"
  "}                                  \n";
#endif

  // NOTE: Compiled vertex/fragment shaders are not deleted,
  // they are kept for re-use as default shaders in case some shader loading fails
  RLGL.State.defaultVShaderId = rlCompileShader(defaultVShaderCode, PF_VERTEX_SHADER);     // Compile default vertex shader
  RLGL.State.defaultFShaderId = rlCompileShader(defaultFShaderCode, PF_FRAGMENT_SHADER);   // Compile default fragment shader

  RLGL.State.defaultShaderId = rlLoadShaderProgram(RLGL.State.defaultVShaderId, RLGL.State.defaultFShaderId);

  if (RLGL.State.defaultShaderId > 0) {
    TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Default shader loaded successfully", RLGL.State.defaultShaderId);

    // Set default shader locations: attributes locations
    RLGL.State.defaultShaderLocs[RL_SHADER_LOC_VERTEX_POSITION] = pfGetAttribLocation(RLGL.State.defaultShaderId, RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
    RLGL.State.defaultShaderLocs[RL_SHADER_LOC_VERTEX_TEXCOORD01] = pfGetAttribLocation(RLGL.State.defaultShaderId, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
    RLGL.State.defaultShaderLocs[RL_SHADER_LOC_VERTEX_COLOR] = pfGetAttribLocation(RLGL.State.defaultShaderId, RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);

    // Set default shader locations: uniform locations
    RLGL.State.defaultShaderLocs[RL_SHADER_LOC_MATRIX_MVP]  = pfGetUniformLocation(RLGL.State.defaultShaderId, RL_DEFAULT_SHADER_UNIFORM_NAME_MVP);
    RLGL.State.defaultShaderLocs[RL_SHADER_LOC_COLOR_DIFFUSE] = pfGetUniformLocation(RLGL.State.defaultShaderId, RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
    RLGL.State.defaultShaderLocs[RL_SHADER_LOC_MAP_DIFFUSE] = pfGetUniformLocation(RLGL.State.defaultShaderId, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);
  } else TRACELOG(RL_LOG_WARNING, "SHADER: [ID %i] Failed to load default shader", RLGL.State.defaultShaderId);
}

// Unload default shader
// NOTE: Unloads: RLGL.State.defaultShaderId, RLGL.State.defaultShaderLocs
static void
rlUnloadShaderDefault(void)
{
  pfUseProgram(0);

  pfDetachShader(RLGL.State.defaultShaderId, RLGL.State.defaultVShaderId);
  pfDetachShader(RLGL.State.defaultShaderId, RLGL.State.defaultFShaderId);
  pfDeleteShader(RLGL.State.defaultVShaderId);
  pfDeleteShader(RLGL.State.defaultFShaderId);

  pfDeleteProgram(RLGL.State.defaultShaderId);

  RL_FREE(RLGL.State.defaultShaderLocs);

  TRACELOG(RL_LOG_INFO, "SHADER: [ID %i] Default shader unloaded successfully", RLGL.State.defaultShaderId);
}

#if defined(RLGL_SHOW_PF_DETAILS_INFO)
// Get compressed format official PF identifier name
static const char *
rlGetCompressedFormatName(int format)
{
  switch (format) {
  // PF_EXT_texture_compression_s3tc
  case 0x83F0:
    return "PF_COMPRESSED_RGB_S3TC_DXT1_EXT";
    break;
  case 0x83F1:
    return "PF_COMPRESSED_RGBA_S3TC_DXT1_EXT";
    break;
  case 0x83F2:
    return "PF_COMPRESSED_RGBA_S3TC_DXT3_EXT";
    break;
  case 0x83F3:
    return "PF_COMPRESSED_RGBA_S3TC_DXT5_EXT";
    break;
  // PF_3DFX_texture_compression_FXT1
  case 0x86B0:
    return "PF_COMPRESSED_RGB_FXT1_3DFX";
    break;
  case 0x86B1:
    return "PF_COMPRESSED_RGBA_FXT1_3DFX";
    break;
  // PF_IMG_texture_compression_pvrtc
  case 0x8C00:
    return "PF_COMPRESSED_RGB_PVRTC_4BPPV1_IMG";
    break;
  case 0x8C01:
    return "PF_COMPRESSED_RGB_PVRTC_2BPPV1_IMG";
    break;
  case 0x8C02:
    return "PF_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG";
    break;
  case 0x8C03:
    return "PF_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG";
    break;
  // PF_OES_compressed_ETC1_RGB8_texture
  case 0x8D64:
    return "PF_ETC1_RGB8_OES";
    break;
  // PF_ARB_texture_compression_rgtc
  case 0x8DBB:
    return "PF_COMPRESSED_RED_RGTC1";
    break;
  case 0x8DBC:
    return "PF_COMPRESSED_SIGNED_RED_RGTC1";
    break;
  case 0x8DBD:
    return "PF_COMPRESSED_RG_RGTC2";
    break;
  case 0x8DBE:
    return "PF_COMPRESSED_SIGNED_RG_RGTC2";
    break;
  // PF_ARB_texture_compression_bptc
  case 0x8E8C:
    return "PF_COMPRESSED_RGBA_BPTC_UNORM_ARB";
    break;
  case 0x8E8D:
    return "PF_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB";
    break;
  case 0x8E8E:
    return "PF_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB";
    break;
  case 0x8E8F:
    return "PF_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB";
    break;
  // PF_ARB_ES3_compatibility
  case 0x9274:
    return "PF_COMPRESSED_RGB8_ETC2";
    break;
  case 0x9275:
    return "PF_COMPRESSED_SRGB8_ETC2";
    break;
  case 0x9276:
    return "PF_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
    break;
  case 0x9277:
    return "PF_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
    break;
  case 0x9278:
    return "PF_COMPRESSED_RGBA8_ETC2_EAC";
    break;
  case 0x9279:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";
    break;
  case 0x9270:
    return "PF_COMPRESSED_R11_EAC";
    break;
  case 0x9271:
    return "PF_COMPRESSED_SIGNED_R11_EAC";
    break;
  case 0x9272:
    return "PF_COMPRESSED_RG11_EAC";
    break;
  case 0x9273:
    return "PF_COMPRESSED_SIGNED_RG11_EAC";
    break;
  // PF_KHR_texture_compression_astc_hdr
  case 0x93B0:
    return "PF_COMPRESSED_RGBA_ASTC_4x4_KHR";
    break;
  case 0x93B1:
    return "PF_COMPRESSED_RGBA_ASTC_5x4_KHR";
    break;
  case 0x93B2:
    return "PF_COMPRESSED_RGBA_ASTC_5x5_KHR";
    break;
  case 0x93B3:
    return "PF_COMPRESSED_RGBA_ASTC_6x5_KHR";
    break;
  case 0x93B4:
    return "PF_COMPRESSED_RGBA_ASTC_6x6_KHR";
    break;
  case 0x93B5:
    return "PF_COMPRESSED_RGBA_ASTC_8x5_KHR";
    break;
  case 0x93B6:
    return "PF_COMPRESSED_RGBA_ASTC_8x6_KHR";
    break;
  case 0x93B7:
    return "PF_COMPRESSED_RGBA_ASTC_8x8_KHR";
    break;
  case 0x93B8:
    return "PF_COMPRESSED_RGBA_ASTC_10x5_KHR";
    break;
  case 0x93B9:
    return "PF_COMPRESSED_RGBA_ASTC_10x6_KHR";
    break;
  case 0x93BA:
    return "PF_COMPRESSED_RGBA_ASTC_10x8_KHR";
    break;
  case 0x93BB:
    return "PF_COMPRESSED_RGBA_ASTC_10x10_KHR";
    break;
  case 0x93BC:
    return "PF_COMPRESSED_RGBA_ASTC_12x10_KHR";
    break;
  case 0x93BD:
    return "PF_COMPRESSED_RGBA_ASTC_12x12_KHR";
    break;
  case 0x93D0:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR";
    break;
  case 0x93D1:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR";
    break;
  case 0x93D2:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR";
    break;
  case 0x93D3:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR";
    break;
  case 0x93D4:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR";
    break;
  case 0x93D5:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR";
    break;
  case 0x93D6:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR";
    break;
  case 0x93D7:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR";
    break;
  case 0x93D8:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR";
    break;
  case 0x93D9:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR";
    break;
  case 0x93DA:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR";
    break;
  case 0x93DB:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR";
    break;
  case 0x93DC:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR";
    break;
  case 0x93DD:
    return "PF_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR";
    break;
  default:
    return "PF_COMPRESSED_UNKNOWN";
    break;
  }
}
#endif  // RLGL_SHOW_PF_DETAILS_INFO

#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2

// Get pixel data size in bytes (image or texture)
// NOTE: Size depends on pixel format
static int
rlGetPixelDataSize(int width, int height, int format)
{
  int dataSize = 0;       // Size in bytes
  int bpp = 0;            // Bits per pixel

  switch (format) {
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
    bpp = 8;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5:
  case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
  case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
    bpp = 16;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
    bpp = 32;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8:
    bpp = 24;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32:
    bpp = 32;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32:
    bpp = 32*3;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
    bpp = 32*4;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16:
    bpp = 16;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16:
    bpp = 16*3;
    break;
  case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
    bpp = 16*4;
    break;
  default:
    break;
  }

  dataSize = width*height*bpp/8;  // Total data size in bytes

  // Most compressed formats works on 4x4 blocks,
  // if texture is smaller, minimum dataSize is 8 or 16
  if ((width < 4) && (height < 4)) {
    if ((format >= RL_PIXELFORMAT_COMPRESSED_DXT1_RGB) && (format < RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA)) dataSize = 8;
    else if ((format >= RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA) && (format < RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA)) dataSize = 16;
  }

  return dataSize;
}

// Auxiliar math functions

// Get float array of matrix data
static rl_float16
rlMatrixToFloatV(GETS(Matrix) mat)
{
  rl_float16 result = { 0 };

  result.v[0] = mat.m0;
  result.v[1] = mat.m1;
  result.v[2] = mat.m2;
  result.v[3] = mat.m3;
  result.v[4] = mat.m4;
  result.v[5] = mat.m5;
  result.v[6] = mat.m6;
  result.v[7] = mat.m7;
  result.v[8] = mat.m8;
  result.v[9] = mat.m9;
  result.v[10] = mat.m10;
  result.v[11] = mat.m11;
  result.v[12] = mat.m12;
  result.v[13] = mat.m13;
  result.v[14] = mat.m14;
  result.v[15] = mat.m15;

  return result;
}

// Get identity matrix
static
GETS(Matrix) rlMatrixIdentity(void)
{
  GETS(Matrix) result = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  return result;
}

// Get two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
static
GETS(Matrix) rlMatrixMultiply(GETS(Matrix) left, GETS(Matrix) right)
{
  GETS(Matrix) result = { 0 };

  result.m0 = left.m0*right.m0 + left.m1*right.m4 + left.m2*right.m8 + left.m3*right.m12;
  result.m1 = left.m0*right.m1 + left.m1*right.m5 + left.m2*right.m9 + left.m3*right.m13;
  result.m2 = left.m0*right.m2 + left.m1*right.m6 + left.m2*right.m10 + left.m3*right.m14;
  result.m3 = left.m0*right.m3 + left.m1*right.m7 + left.m2*right.m11 + left.m3*right.m15;
  result.m4 = left.m4*right.m0 + left.m5*right.m4 + left.m6*right.m8 + left.m7*right.m12;
  result.m5 = left.m4*right.m1 + left.m5*right.m5 + left.m6*right.m9 + left.m7*right.m13;
  result.m6 = left.m4*right.m2 + left.m5*right.m6 + left.m6*right.m10 + left.m7*right.m14;
  result.m7 = left.m4*right.m3 + left.m5*right.m7 + left.m6*right.m11 + left.m7*right.m15;
  result.m8 = left.m8*right.m0 + left.m9*right.m4 + left.m10*right.m8 + left.m11*right.m12;
  result.m9 = left.m8*right.m1 + left.m9*right.m5 + left.m10*right.m9 + left.m11*right.m13;
  result.m10 = left.m8*right.m2 + left.m9*right.m6 + left.m10*right.m10 + left.m11*right.m14;
  result.m11 = left.m8*right.m3 + left.m9*right.m7 + left.m10*right.m11 + left.m11*right.m15;
  result.m12 = left.m12*right.m0 + left.m13*right.m4 + left.m14*right.m8 + left.m15*right.m12;
  result.m13 = left.m12*right.m1 + left.m13*right.m5 + left.m14*right.m9 + left.m15*right.m13;
  result.m14 = left.m12*right.m2 + left.m13*right.m6 + left.m14*right.m10 + left.m15*right.m14;
  result.m15 = left.m12*right.m3 + left.m13*right.m7 + left.m14*right.m11 + left.m15*right.m15;

  return result;
}

// Transposes provided matrix
static
GETS(Matrix) rlMatrixTranspose(GETS(Matrix) mat)
{
  GETS(Matrix) result = { 0 };

  result.m0 = mat.m0;
  result.m1 = mat.m4;
  result.m2 = mat.m8;
  result.m3 = mat.m12;
  result.m4 = mat.m1;
  result.m5 = mat.m5;
  result.m6 = mat.m9;
  result.m7 = mat.m13;
  result.m8 = mat.m2;
  result.m9 = mat.m6;
  result.m10 = mat.m10;
  result.m11 = mat.m14;
  result.m12 = mat.m3;
  result.m13 = mat.m7;
  result.m14 = mat.m11;
  result.m15 = mat.m15;

  return result;
}

// Invert provided matrix
static
GETS(Matrix) rlMatrixInvert(GETS(Matrix) mat)
{
  GETS(Matrix) result = { 0 };

  // Cache the matrix values (speed optimization)
  float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
  float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
  float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
  float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

  float b00 = a00*a11 - a01*a10;
  float b01 = a00*a12 - a02*a10;
  float b02 = a00*a13 - a03*a10;
  float b03 = a01*a12 - a02*a11;
  float b04 = a01*a13 - a03*a11;
  float b05 = a02*a13 - a03*a12;
  float b06 = a20*a31 - a21*a30;
  float b07 = a20*a32 - a22*a30;
  float b08 = a20*a33 - a23*a30;
  float b09 = a21*a32 - a22*a31;
  float b10 = a21*a33 - a23*a31;
  float b11 = a22*a33 - a23*a32;

  // Calculate the invert determinant (inlined to avoid double-caching)
  float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

  result.m0 = (a11*b11 - a12*b10 + a13*b09)*invDet;
  result.m1 = (-a01*b11 + a02*b10 - a03*b09)*invDet;
  result.m2 = (a31*b05 - a32*b04 + a33*b03)*invDet;
  result.m3 = (-a21*b05 + a22*b04 - a23*b03)*invDet;
  result.m4 = (-a10*b11 + a12*b08 - a13*b07)*invDet;
  result.m5 = (a00*b11 - a02*b08 + a03*b07)*invDet;
  result.m6 = (-a30*b05 + a32*b02 - a33*b01)*invDet;
  result.m7 = (a20*b05 - a22*b02 + a23*b01)*invDet;
  result.m8 = (a10*b10 - a11*b08 + a13*b06)*invDet;
  result.m9 = (-a00*b10 + a01*b08 - a03*b06)*invDet;
  result.m10 = (a30*b04 - a31*b02 + a33*b00)*invDet;
  result.m11 = (-a20*b04 + a21*b02 - a23*b00)*invDet;
  result.m12 = (-a10*b09 + a11*b07 - a12*b06)*invDet;
  result.m13 = (a00*b09 - a01*b07 + a02*b06)*invDet;
  result.m14 = (-a30*b03 + a31*b01 - a32*b00)*invDet;
  result.m15 = (a20*b03 - a21*b01 + a22*b00)*invDet;

  return result;
}

#endif  // RLGL_IMPLEMENTATION
