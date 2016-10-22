/* E3D is a 3D Graphic Library for C on Windows
 *
 * Link gdi32
 *    GCC: -lgdi32
 */

#include <math.h>
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <winuser.h>

#define E3D_PI 3.14159265358979323846

/* Pixel Buffer */

typedef struct {
   long width, height;
   DWORD *buffer;
} _E3D_PIXEL_BUFFER;

/* Objects */

enum {
  E3D_OBJECT_PLANE,
  E3D_OBJECT_PRISM
};

typedef struct {
  int wireframe;
  float opacity;
  DWORD color;
} E3D_MATERIAL;

typedef struct {
  float x, y, z;
} E3D_ROTATION;

typedef struct {
  unsigned int type;

  float x,     y,      z,
        width, height, depth;

  E3D_ROTATION rotation;
  E3D_MATERIAL material;
} E3D_OBJECT;

#define _E3D_SCENE_DEFAULT_ALLOC 100

#define E3D_RGB(r, g, b) RGB(b, g,r )

typedef struct {
  unsigned int object_count, alloc;
  E3D_OBJECT **objects;
} E3D_SCENE;

typedef struct {
  float x, y, z,
        fov, zNear, zFar;

  E3D_ROTATION rotation;
} E3D_CAMERA;

/* Renderer */

typedef struct {
  E3D_CAMERA *camera;
  E3D_SCENE *scene;

  int antialias;
} E3D_RENDERER;

typedef struct {
  float x, y, z, w;
} _E3D_POINT;

typedef struct {
  _E3D_POINT points[3];
} _E3D_PLANE;

#define _E3D_PLANES_DEFAULT_ALLOC 200

typedef struct {
  unsigned int count, alloc;
  _E3D_PLANE *planes;
} _E3D_PLANES;

/* Window */

typedef struct {
  int x, y, width, height;
} E3D_WINPROPS;

struct {
  _E3D_PIXEL_BUFFER pixels;
  _E3D_PLANES planes;
  HWND *hWnd;
  HDC *hDC;
  RECT lpRect;
  int ready;

  void (*onmousemove)(int, int);
  void (*onkeydown)(short);
  void (*onkeyup)(short);
  void (*onready)();
  void (*onclose)();
  void (*onupdate)();
} _E3D_WINDOW;

const unsigned int E3D_OBJECT_SIZE = sizeof(E3D_OBJECT),
                   E3D_SCENE_SIZE  = sizeof(E3D_SCENE),
                   E3D_CAMERA_SIZE = sizeof(E3D_CAMERA);

#include "math.c"
#include "objects.c"
#include "window.c"
#include "pixels.c"
#include "points.c"
#include "renderer.c"
