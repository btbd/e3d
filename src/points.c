int _E3D_IsPlaneVisible(_E3D_PLANE *p, E3D_CAMERA *c) {
  int i, t;

  for (i = t = 0; i < 3; i++) {
    if (p->points[i].z < c->zNear || p->points[i].z > c->zFar) {
      return 0;
    }

    if (p->points[i].x < 0 || p->points[i].x >= _E3D_WINDOW.pixels.width ||
        p->points[i].y < 0 || p->points[i].y >= _E3D_WINDOW.pixels.height)
    {
      t++;
    }
  }

  return t < 3;
}

// Cohen-Sutherland Clipping Algorithm
int _E3D_OutCode(float x, float y, int w, int h) {
  int c = 0;

  if (x < 0) {
    c |= 1;
  } else if (x > w) {
    c |= 2;
  }

  if (y < 0) {
    c |= 4;
  } else if (y > h) {
    c |= 8;
  }

  return c;
}

int _E3D_ClipLine(float *x0, float *y0, float *x1, float *y1, int vw, int vh) {
  int o0 = _E3D_OutCode(*x0, *y0, vw, vh),
      o1 = _E3D_OutCode(*x1, *y1, vw, vh);

  int a = 0;

  while (1) {
    if (!(o0 | o1)) {
      a = 1;
      break;
    } else if (o0 & o1) {
      break;
    } else {
      int x, y,
          o = o0 ? o0 : o1;

      if (o & 8) {
        x = *x0 + (*x1 - *x0) * (vh - *y0) / (*y1 - *y0);
        y = vh;
      } else if (o & 4) {
        x = *x0 + (*x1 - *x0) * (0 - *y0) / (*y1 - *y0);
        y = 0;
      } else if (o & 2) {
        y = *y0 + (*y1 - *y0) * (vw - *x0) / (*x1 - *x0);
        x = vw;
      } else if (o & 1) {
        y = *y0 + (*y1 - *y0) * (0 - *x0) / (*x1 - *x0);
        x = 0;
      }

      if (o == o0) {
        *x0 = x;
        *y0 = y;
        o0 = _E3D_OutCode(*x0, *y0, vw, vh);
      } else {
        *x1 = x;
        *y1 = y;
        o1 = _E3D_OutCode(*x1, *y1, vw, vh);
      }
    }
  }

  return a;
}

int _E3D_ClipPlane(_E3D_PLANE *p) {
  int i, n;
  int v = 0;

  for (i = 0, n = 2; i < 3; n = i++) {
    v += _E3D_ClipLine(&p->points[i].x, &p->points[i].y,
                       &p->points[n].x, &p->points[n].y,
                       E3D_GetWidth(), E3D_GetHeight());
  }

  return v != 0;
}

void _E3D_PointRotate(_E3D_POINT *point, E3D_ROTATION *rotation,
                      float ox, float oy, float oz)
{
  float x = point->x,
        y = point->y,
        z = point->z;

  if (fmod(rotation->y, 360) != 0) {
    float ry = rotation->y * E3D_PI / 180;

    point->z = oz + (z - oz) * cos(ry) - (x - ox) * sin(ry);
    point->x = ox + (z - oz) * sin(ry) + (x - ox) * cos(ry);

    x = point->x;
    z = point->z;
  }

  if (fmod(rotation->z, 360) != 0) {
    float rz = rotation->z * E3D_PI / 180;

    point->x = ox + (x - ox) * cos(rz) - (y - oy) * sin(rz);
    point->y = oy + (x - ox) * sin(rz) + (y - oy) * cos(rz);

    y = point->y;
  }

  if (fmod(rotation->x, 360) != 0) {
    float rx = rotation->x * E3D_PI / 180;

    point->y = oy + (y - oy) * cos(rx) - (z - oz) * sin(rx);
    point->z = oz + (y - oy) * sin(rx) + (z - oz) * cos(rx);
  }
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

  float ox = o->x - c->x;
  float oy = o->y - c->y;
  float oz = o->z - c->z;

  switch (o->type) {
    case E3D_OBJECT_PLANE: {
      float x  = ox - hwidth,
            x1 = ox + hwidth,
            z  = oz - hdepth,
            z1 = oz + hdepth;

      _E3D_PlanesPush(p, x,  oy, z1, x,  oy, z, x1, oy, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      _E3D_PLANE *pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x, pre->points[0].y, pre->points[0].z,
        x1, oy, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr,
                       0, 0, 0);

      break;
    }
    case E3D_OBJECT_PRISM: {
      float x  = ox - hwidth,
            x1 = ox + hwidth,
            y  = oy - hheight,
            y1 = oy + hheight,
            z  = oz - hdepth,
            z1 = oz + hdepth;

      // Top

      _E3D_PlanesPush(p, x, y1, z1, x, y1, z, x1, y1, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      _E3D_PLANE *pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x1, y1, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr, 0, 0, 0);

      // Bottom

      _E3D_PlanesPush(p, x, y, z1, x, y, z, x1, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
          pre->points[0].x,  pre->points[0].y, pre->points[0].z,
          x1, y, z1,
          pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr, 0, 0, 0);

      // Left

      _E3D_PlanesPush(p, x, y1, z1, x, y1, z, x, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr, 0, 0, 0);

      // Right

      _E3D_PlanesPush(p, x1, y1, z1, x1, y1, z, x1, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x1, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr, 0, 0, 0);

      // Front

      _E3D_PlanesPush(p, x, y1, z1, x1, y1, z1, x1, y, z1);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x, y, z1,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr, 0, 0, 0);

      // Back

      _E3D_PlanesPush(p, x, y1, z, x1, y1, z, x1, y, z);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &o->rotation,
                       ox, oy, oz);
      _E3D_PlaneRotate(&p->planes[p->count - 1], &cr, 0, 0, 0);

      pre = &p->planes[p->count - 1];

      _E3D_PlanesPush(p,
        pre->points[0].x,  pre->points[0].y, pre->points[0].z,
        x, y, z,
        pre->points[2].x,  pre->points[2].y, pre->points[2].z
      );

      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &o->rotation,
                       ox, oy, oz);
      _E3D_PointRotate(&p->planes[p->count - 1].points[1], &cr, 0, 0, 0);

      break;
    }
  }
}
