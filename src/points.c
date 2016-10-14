void _E3D_PointRotate(_E3D_POINT *point, E3D_ROTATION *rotation,
                      float ox, float oy, float oz)
{
  float x = point->x,
        y = point->y,
        z = point->z;

  if (fmod(rotation->x, 360) != 0) {
    float rx = rotation->x * E3D_PI / 180;

    point->y = oy + (y - oy) * cos(rx) - (z - oz) * sin(rx);
    point->z = oz + (y - oy) * sin(rx) + (z - oz) * cos(rx);

    y = point->y;
    z = point->z;
  }

  if (fmod(rotation->y, 360) != 0) {
    float ry = rotation->y * E3D_PI / 180;

    point->z = oz + (z - oz) * cos(ry) - (x - ox) * sin(ry);
    point->x = ox + (z - oz) * sin(ry) + (x - ox) * cos(ry);

    x = point->x;
  }

  if (fmod(rotation->z, 360) != 0) {
    float rz = rotation->z * E3D_PI / 180;

    point->x = ox + (x - ox) * cos(rz) - (y - oy) * sin(rz);
    point->y = oy + (x - ox) * sin(rz) + (y - oy) * cos(rz);
  }
}

void _E3D_PointTransformCamera(_E3D_POINT *point, E3D_CAMERA *camera) {
  point->x -= camera->x;
  point->y -= camera->y;
  point->z -= camera->z;
}

void _E3D_PlaneRotate(_E3D_PLANE *p, E3D_ROTATION *r,
                      float ox, float oy, float oz)
{
  _E3D_PointRotate(&p->points[0], r, ox, oy, oz);
  _E3D_PointRotate(&p->points[1], r, ox, oy, oz);
  _E3D_PointRotate(&p->points[2], r, ox, oy, oz);
}

void _E3D_ObjectToPlanes(E3D_CAMERA *c, _E3D_PLANES *p, E3D_OBJECT *o) {
  float hwidth  = o->width / 2,
        hheight = o->height / 2,
        hdepth  = o->depth / 2;

  E3D_ROTATION cr = {
    .x = -c->rotation.x,
    .y = -c->rotation.y,
    .z = -c->rotation.z
  };

  switch (o->type) {
    case E3D_OBJECT_PLANE: {
      float x  = o->x - hwidth,
            x1 = o->x + hwidth,
            z  = o->z - hdepth,
            z1 = o->z + hdepth;

      _E3D_PlanesPush(p, x,  o->y, z1, x,  o->y, z, x1, o->y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      _E3D_PLANE *pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x, pre->points[0].y, pre->points[0].z,
        x1, o->y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      break;
    }
    case E3D_OBJECT_PRISM: {
      float x  = o->x - hwidth,
            x1 = o->x + hwidth,
            y  = o->y - hheight,
            y1 = o->y + hheight,
            z  = o->z - hdepth,
            z1 = o->z + hdepth;

      // Top

      _E3D_PlanesPush(p, x, y, z1, x, y, z, x1, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      _E3D_PLANE *pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x1, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      // Bottom

      _E3D_PlanesPush(p, x, y1, z1, x, y1, z, x1, y1, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
          pre->points[0].x,  pre->points[0].y, pre->points[0].z,
          x1, y1, z1,
          pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      // Left

      _E3D_PlanesPush(p, x, y1, z1, x, y1, z, x, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      // Right

      _E3D_PlanesPush(p, x1, y1, z1, x1, y1, z, x1, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x1, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      // Front

      _E3D_PlanesPush(p, x, y1, z1, x1, y1, z1, x1, y, z1);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x1, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      // Back

      _E3D_PlanesPush(p, x, y1, z, x1, y1, z, x1, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, c->x, c->y, c->z);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x1, y, z,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       o->x, o->y, o->z);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       c->x, c->y, c->z);

      break;
    }
  }
}
