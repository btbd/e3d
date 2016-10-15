int E3D_CreateWindow(char *, E3D_WINPROPS *);

LRESULT CALLBACK _E3D_WndProc(HWND, UINT, WPARAM, LPARAM);
void _E3D_WndLoop();
long E3D_GetWidth();
long E3D_GetHeight();
void _E3D_CreatePixelBuffer(_E3D_PIXEL_BUFFER *, long, long);

int E3D_CreateWindow(char *title, E3D_WINPROPS *props) {
  WNDCLASSEX wndClass = {
    .cbSize        = sizeof(WNDCLASSEX),
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = _E3D_WndProc,
    .cbClsExtra    = 0,
    .cbWndExtra    = 0,
    .hInstance     = GetModuleHandle(0),
    .hIcon         = LoadIcon(wndClass.hInstance,
                              MAKEINTRESOURCE(IDI_APPLICATION)),

    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszMenuName  = NULL,
    .lpszClassName = "win32app",
    .hIconSm       = LoadIcon(wndClass.hInstance,
                              MAKEINTRESOURCE(IDI_APPLICATION))

  };

  if (!RegisterClassEx(&wndClass)) {
    return;
  }

  int x, y, width, height;

  if (props == NULL) {
    x      = CW_USEDEFAULT;
    y      = CW_USEDEFAULT;
    width  = 480;
    height = 480;
  } else {
    x      = props->x;
    y      = props->y;
    width  = props->width;
    height = props->height;
  }

  DWORD style = WS_OVERLAPPEDWINDOW;
  HWND hWnd   = CreateWindow(wndClass.lpszClassName, title, style,
                             x, y, width, height, NULL, NULL,
                             GetModuleHandle(0), NULL);

  HDC hDC     = GetDC(hWnd);

  if (!hWnd || !hDC) {
    return;
  }

  ShowWindow(hWnd, 1);
  UpdateWindow(hWnd);

  _E3D_WINDOW.hWnd = &hWnd;
  _E3D_WINDOW.hDC  = &hDC;

  while(!IsWindowVisible(hWnd)) {
    Sleep(1);
  }

  _E3D_CreatePixelBuffer(&_E3D_WINDOW.pixels, E3D_GetWidth(), E3D_GetHeight());

  _E3D_WINDOW.planes.count = 0;
  _E3D_WINDOW.planes.alloc = _E3D_PLANES_DEFAULT_ALLOC;

  _E3D_WINDOW.planes.planes =
      (_E3D_PLANE *)malloc(_E3D_PLANES_DEFAULT_ALLOC * sizeof(_E3D_PLANE));

  if (_E3D_WINDOW.onready) {
    _E3D_WINDOW.onready();
  }

  _E3D_WINDOW.ready = 1;

  MSG msg;
  while (msg.message != WM_QUIT) {
    if (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

LRESULT CALLBACK _E3D_WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam)
{
  switch (msg) {
    case WM_PAINT:
      if (_E3D_WINDOW.onupdate) {
        _E3D_WINDOW.onupdate();
      }

      break;
    case WM_KEYDOWN:
      if (_E3D_WINDOW.onkeydown) {
        _E3D_WINDOW.onkeydown(wParam);
      }

      break;
    case WM_KEYUP:
      if (_E3D_WINDOW.onkeyup) {
        _E3D_WINDOW.onkeyup(wParam);
      }

      break;
    case WM_MOUSEMOVE:
      if (_E3D_WINDOW.onmousemove) {
        _E3D_WINDOW.onmousemove(LOWORD(lParam), HIWORD(lParam));
      }

      break;
    case WM_CLOSE:
      if (_E3D_WINDOW.onclose) {
        _E3D_WINDOW.onclose();
      }
    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  return 0;
}

void E3D_CloseWindow() {
  DestroyWindow(*_E3D_WINDOW.hWnd);
  PostQuitMessage(0);
}

void E3D_OnWindowReady(void (*callback)(int, int)) {
  _E3D_WINDOW.onready = callback;
}

void E3D_OnWindowClose(void (*callback)(int, int)) {
  _E3D_WINDOW.onclose = callback;
}

long E3D_GetWindowWidth() {
  GetWindowRect(*_E3D_WINDOW.hWnd, &_E3D_WINDOW.lpRect);

  return _E3D_WINDOW.lpRect.right - _E3D_WINDOW.lpRect.left;
}

long E3D_GetWindowHeight() {
  GetWindowRect(*_E3D_WINDOW.hWnd, &_E3D_WINDOW.lpRect);

  return _E3D_WINDOW.lpRect.bottom - _E3D_WINDOW.lpRect.top;
}

long E3D_GetWidth() {
  GetClientRect(*_E3D_WINDOW.hWnd, &_E3D_WINDOW.lpRect);

  return _E3D_WINDOW.lpRect.right;
}

long E3D_GetHeight() {
  GetClientRect(*_E3D_WINDOW.hWnd, &_E3D_WINDOW.lpRect);

  return _E3D_WINDOW.lpRect.bottom;
}

void E3D_OnKeyDown(void (*callback)(short)) {
  _E3D_WINDOW.onkeydown = callback;
}

void E3D_OnKeyUp(void (*callback)(short)) {
  _E3D_WINDOW.onkeydown = callback;
}

void E3D_OnMouseMove(void (*callback)(int, int)) {
  _E3D_WINDOW.onmousemove = callback;
}

void E3D_OnUpdate(void (*callback)()) {
  _E3D_WINDOW.onupdate = callback;
}
