void _E3D_Perspective(E3D_CAMERA *c, float *x, float *y, float *z, float *pz,
                      float ar)
{
  float f = 1 / tan(c->fov * E3D_PI / 180 / 2);

  *x = (f / ar);
  *y = f;
  *z = (c->zFar + c->zNear) / (c->zNear - c->zFar);
  *pz = (2 * c->zFar * c->zNear) / (c->zNear - c->zFar);
}

int _E3D_PInsideScreen(_E3D_POINT *p, float w, float h) {
  return p->x >= 0 && p->x < w && p->y >= 0 && p->y <= h;
}

void _E3D_P3DToP2D(_E3D_POINT *point, float mx, float my, float mz, float pz,
                   float hw, float hh)
{
  float w = (hw + hh) / 2;

  point->x = hw + (w * ((point->x * mx) / -point->z));
  point->y = hh + (w * ((point->y * my) / point->z));
  point->z = (point->z * mz) + pz;
}

void _E3D_Line(_E3D_PIXEL_BUFFER *b, int x0, int y0, int x1, int y1, DWORD c) {
  int dx = abs(x1 - x0),
      dy = abs(y1 - y0),
      sx = x0 < x1 ? 1 : -1,
      sy = y0 < y1 ? 1 : -1;

  int e = (dx > dy ? dx : -dy) / 2,
      e1;

  while (1) {
    _E3D_SetPixel(b, x0, y0, c);

    if (x0 == x1 && y0 == y1) {
      break;
    }

    e1 = e;

    if (e1 > -dx) {
      e -= dy;
      x0 += sx;
    }

    if (e1 < dy) {
      e += dx;
      y0 += sy;
    }
  }
}

// Currently not the fastest filling method, but it will do for now
void _E3D_FillPlane(_E3D_PIXEL_BUFFER *b, _E3D_PLANE *p, DWORD c) {
  float ymin = E3D_Min(E3D_Min(p->points[0].y, p->points[1].y), p->points[2].y),
        ymax = E3D_Max(E3D_Max(p->points[0].y, p->points[1].y), p->points[2].y);

  float xmin = E3D_Min(E3D_Min(p->points[0].x, p->points[1].x), p->points[2].x),
        xmax = E3D_Max(E3D_Max(p->points[0].x, p->points[1].x), p->points[2].x);

  float a = 0.5 * (-p->points[1].y * p->points[2].x + p->points[0].y *
                  (-p->points[1].x + p->points[2].x ) + p->points[0].x *
                  (p->points[1].y - p->points[2].y) + p->points[1].x *
                   p->points[2].y);
  float h = 1 / (2 * a);
  float k, l;

  int x;

  for (; ymin <= ymax; ymin++) {
    for (x = xmin; x <= xmax; x++) {
      k = h * (p->points[0].y * p->points[2].x - p->points[0].x * p->points[2].y
                  + (p->points[2].y - p->points[0].y) * x +
                      (p->points[0].x - p->points[2].x) * ymin);

      l = h * (p->points[0].x * p->points[1].y - p->points[0].y * p->points[1].x
                  + (p->points[0].y - p->points[1].y) * x +
                      (p->points[1].x - p->points[0].x) * ymin);

      if (k > 0 && l > 0 && 1 - k - l > 0) {
        _E3D_SetPixel(b, x, ymin, c);
      }
    }
  }
}

void E3D_Render(E3D_RENDERER *renderer) {
  if (!_E3D_WINDOW.ready) {
    return;
  }

  E3D_SCENE *scene     = renderer->scene;
  E3D_CAMERA *camera   = renderer->camera;
  E3D_OBJECT **objects = scene->objects;

  unsigned int hwidth  = _E3D_WINDOW.pixels.width / 2,
               hheight = _E3D_WINDOW.pixels.height / 2;

  float mulx, muly, mulz, pluz;
  _E3D_Perspective(camera, &mulx, &muly, &mulz, &pluz,
                  ((float)_E3D_WINDOW.pixels.width / _E3D_WINDOW.pixels.width));

  _E3D_PLANES *planes = &_E3D_WINDOW.planes;
  int o;

  for (o = 0; o < scene->object_count; o++) {
    _E3D_ObjectToPlanes(camera, planes, objects[o]);

    int p;
    for (p = 0; p < planes->count; p++) {
      _E3D_PLANE *plane = &planes->planes[p];

      _E3D_P3DToP2D(&plane->points[0], mulx, muly, mulz, pluz, hwidth, hheight);
      _E3D_P3DToP2D(&plane->points[1], mulx, muly, mulz, pluz, hwidth, hheight);
      _E3D_P3DToP2D(&plane->points[2], mulx, muly, mulz, pluz, hwidth, hheight);

      if (!_E3D_IsPlaneVisible(plane, camera)) {
        continue;
      }

      if (objects[o]->material.wireframe) {
        _E3D_Line(&_E3D_WINDOW.pixels, plane->points[2].x, plane->points[2].y,
                  plane->points[0].x, plane->points[0].y,
                  objects[o]->material.color);
        _E3D_Line(&_E3D_WINDOW.pixels, plane->points[0].x, plane->points[0].y,
                  plane->points[1].x, plane->points[1].y,
                  objects[o]->material.color);
        _E3D_Line(&_E3D_WINDOW.pixels, plane->points[1].x, plane->points[1].y,
                  plane->points[2].x, plane->points[2].y,
                  objects[o]->material.color);
      } else {
        _E3D_FillPlane(&_E3D_WINDOW.pixels, plane, objects[o]->material.color);
      }
    }

    planes->count = 0;
  }

  _E3D_DrawPixelBuffer(&_E3D_WINDOW.pixels);
  _E3D_ResetPixelBuffer(&_E3D_WINDOW.pixels);
}
