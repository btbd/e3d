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

 void update() {
   if (E3D_GetTime() < lastUpdate + 17) {
     return;
   }

   E3D_Render(&renderer);
   renderer.scene->objects[0]->rotation.y++;

   lastUpdate = E3D_GetTime();
 }

 int main(int argc, char *argv[]) {
   E3D_WINPROPS props = {
     .x = 0,
     .y = 0,
     .width = 600,
     .height = 600
   };

   E3D_OnWindowReady(&onready);
   E3D_OnWindowClose(&onclose);

   E3D_SCENE *scene   = E3D_Scene();
   E3D_CAMERA *camera = E3D_Camera();

   camera->zNear = 1;
   camera->zFar = 1000;
   camera->fov = 90;

   E3D_OBJECT *box  = E3D_Object(E3D_OBJECT_PRISM);

   box->width = 25;
   box->depth = 25;
   box->height = 25;
   box->x = 0;
   box->z = -100;
   box->rotation.y = 45;

   box->material.color = E3D_RGB(255, 0, 0);
   box->material.wireframe = 1;;

   E3D_ScenePush(scene, box);

   renderer.scene = scene;
   renderer.camera = camera;

   E3D_OnUpdate(&update);
   E3D_CreateWindow("Example", &props);

   return 0;
 }
