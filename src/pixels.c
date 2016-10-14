void _E3D_CreatePixelBuffer(_E3D_PIXEL_BUFFER *buffer, long w, long h) {
  buffer->width  = w;
  buffer->height = h;
  buffer->buffer = (DWORD *)malloc(w * h * sizeof(DWORD));
}

void _E3D_ResetPixelBuffer(_E3D_PIXEL_BUFFER *buffer) {
  memset(buffer->buffer, 0, buffer->width * buffer->height * sizeof(DWORD));
}

void _E3D_SetPixel(_E3D_PIXEL_BUFFER *buffer, int x, int y, DWORD rgb) {
  if (!(x >= 0 && x < buffer->width && y >= 0 && y < buffer->height)) {
    return;
  }

  buffer->buffer[x + (buffer->width * (buffer->height - y - 1))] = rgb;
}

void _E3D_DrawPixelBuffer(_E3D_PIXEL_BUFFER *buffer) {
  static BITMAPINFO bmi = {
    .bmiHeader.biSize          = sizeof(BITMAPINFO),
    .bmiHeader.biPlanes        = 1,
    .bmiHeader.biBitCount      = 32,
    .bmiHeader.biCompression   = BI_RGB,
    .bmiHeader.biSizeImage     = 0,
    .bmiHeader.biClrUsed       = 0,
    .bmiHeader.biClrImportant  = 0,
    .bmiHeader.biXPelsPerMeter = 2480,
    .bmiHeader.biYPelsPerMeter = 248
  };

  bmi.bmiHeader.biWidth  = buffer->width;
  bmi.bmiHeader.biHeight = buffer->height;

  SetDIBitsToDevice(*_E3D_WINDOW.hDC, 0, 0, buffer->width, buffer->height, 0, 0,
                    0, buffer->height, buffer->buffer, &bmi, DIB_RGB_COLORS);
}

void _E3D_ResizePixelBuffer(_E3D_PIXEL_BUFFER *buffer, long width,
                            long height)
{
  realloc(buffer->buffer, width * height * sizeof(DWORD));
}
