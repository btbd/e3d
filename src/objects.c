E3D_OBJECT *E3D_Object(unsigned int type) {
  E3D_OBJECT *object = (E3D_OBJECT *)malloc(E3D_OBJECT_SIZE);

  memset(object, 0, E3D_OBJECT_SIZE);
  object->type = type;

  return object;
}

E3D_SCENE *E3D_Scene() {
  E3D_SCENE *scene = (E3D_SCENE *)malloc(E3D_SCENE_SIZE);

  scene->object_count = 0;
  scene->alloc = _E3D_SCENE_DEFAULT_ALLOC;

  scene->objects =
      (E3D_OBJECT **)malloc(_E3D_SCENE_DEFAULT_ALLOC * E3D_OBJECT_SIZE);

  return scene;
}

E3D_CAMERA *E3D_Camera() {
  E3D_CAMERA *camera = (E3D_CAMERA *)malloc(E3D_CAMERA_SIZE);

  memset(camera, 0, E3D_CAMERA_SIZE);

  return camera;
}

void E3D_ScenePush(E3D_SCENE *scene, E3D_OBJECT *object) {
  if (scene->object_count >= scene->alloc) {
    scene->alloc *= 2;
    realloc(scene->objects, scene->alloc);
  }

  scene->objects[scene->object_count++] = object;
}

void _E3D_PlanesPush(_E3D_PLANES *planes, float x,  float y,  float z,
                                          float x1, float y1, float z1,
                                          float x2, float y2, float z2)
{
  if (planes->count >= planes->alloc) {
    planes->alloc *= 2;
    realloc(planes->planes, planes->alloc);
  }

  _E3D_PLANE *p = &planes->planes[planes->count++];

  p->points[0].x = x;  p->points[0].y = y;  p->points[0].z = z;
  p->points[1].x = x1; p->points[1].y = y1; p->points[1].z = z1;
  p->points[2].x = x2; p->points[2].y = y2; p->points[2].z = z2;
}
