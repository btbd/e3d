#include <stdio.h>
#include "e3d.h"

E3D_RENDERER renderer;
long lastUpdate;

void onready() {
  printf("Window Created\n\n");

  printf("Width: %dpx\nHeight: %dpx\n", E3D_GetWidth(), E3D_GetHeight());
}

void onclose() {
  exit(0);
}

void onmousemove(int x, int y) {
  renderer.camera->rotation.y -= (float)x * 0.25;
  renderer.camera->rotation.x -= (float)y * 0.25;

  E3D_SetCursorPos(0, 0);
}

void update() {
  if (E3D_GetTime() < lastUpdate + 17) {
    return;
  }

  if (E3D_KeyIsDown(E3D_GetKeyCode('W'))) {
    float angle = E3D_DegToRad(renderer.camera->rotation.y);

    renderer.camera->z -= cos(angle);
    renderer.camera->x -= sin(angle);
  } else if (E3D_KeyIsDown(E3D_GetKeyCode('S'))) {
    float angle = E3D_DegToRad(renderer.camera->rotation.y);

    renderer.camera->z += cos(angle);
    renderer.camera->x += sin(angle);
  }

  if (E3D_KeyIsDown(E3D_GetKeyCode('A'))) {
    float angle = E3D_DegToRad(renderer.camera->rotation.y + 90);

    renderer.camera->z -= cos(angle);
    renderer.camera->x -= sin(angle);
  } else if (E3D_KeyIsDown(E3D_GetKeyCode('D'))) {
    float angle = E3D_DegToRad(renderer.camera->rotation.y + 90);

    renderer.camera->z += cos(angle);
    renderer.camera->x += sin(angle);
  }

  E3D_Render(&renderer);

  lastUpdate = E3D_GetTime();
}

int main(int argc, char *argv[]) {
  E3D_HideCursor();

  E3D_WINPROPS props = {
    .x = 0,
    .y = 0,
    .width = 800,
    .height = 600
  };

  E3D_OnWindowReady(&onready);
  E3D_OnWindowClose(&onclose);
  E3D_OnMouseMove(&onmousemove);

  E3D_SCENE *scene   = E3D_Scene();
  E3D_CAMERA *camera = E3D_Camera();

  camera->zNear = 1;
  camera->zFar = 1000;
  camera->fov = 90;

  E3D_OBJECT *box  = E3D_Object(E3D_OBJECT_PRISM);

  box->width = 25;
  box->depth = 25;
  box->height = 25;
  box->z = -100;
  box->material.wireframe = 1;

  box->material.color = E3D_RGB(255, 133, 0);

  E3D_ScenePush(scene, box);

  renderer.scene = scene;
  renderer.camera = camera;

  E3D_OnUpdate(&update);
  E3D_CreateWindow("Example", &props);

  return 0;
}
