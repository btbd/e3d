#include <stdio.h>
#include "e3d.h"

E3D_RENDERER renderer;

void onready() {
  printf("Window Created\n\n");

  printf("Width: %dpx\nHeight: %dpx\n", E3D_GetWidth(), E3D_GetHeight());
}

void onclose() {
  exit(0);
}

void onkeydown(short keycode) {
  switch (keycode) {
    // D
    case 68:
      renderer.camera->x++;
      break;
    // A
    case 65:
      renderer.camera->x--;
      break;
    // W
    case 87:
      renderer.camera->z--;
      break;
    // S
    case 83:
      renderer.camera->z++;
      break;
    // Shift
    case 16:
      renderer.camera->y--;
      break;
    // Space
    case 32:
      renderer.camera->y++;
      break;
    // Up arrow
    case 38:
      renderer.camera->rotation.x++;
      break;
    // Down arrow
    case 40:
      renderer.camera->rotation.x--;
      break;
    // Left arrow
    case 37:
      renderer.camera->rotation.y++;
      break;
    // Right arrow
    case 39:
      renderer.camera->rotation.y--;
      break;
  }
}

int main(int argc, char *argv[]) {
  E3D_WINPROPS props = {
    .x = 0,
    .y = 0,
    .width = 600,
    .height = 600
  };

  E3D_CreateWindow("Exaple", &props);
  E3D_OnWindowReady(&onready);
  E3D_OnWindowClose(&onclose);
  E3D_OnKeyDown(&onkeydown);

  E3D_SCENE *scene   = E3D_Scene();
  E3D_CAMERA *camera = E3D_Camera();

  camera->zNear = 1;
  camera->zFar = 1000;
  camera->fov = 75;

  E3D_OBJECT *box  = E3D_Object(E3D_OBJECT_PRISM);

  box->width = 25;
  box->depth = 25;
  box->height = 25;
  box->x = 0;
  box->z = -100;
  box->rotation.y = 45;

  box->material.color = E3D_RGB(255, 0, 0);

  E3D_ScenePush(scene, box);

  renderer.scene = scene;
  renderer.camera = camera;

  while (1) {
    E3D_Render(&renderer);
    box->rotation.x++;
    box->rotation.y++;

    Sleep(1);
  }

  return 0;
}
