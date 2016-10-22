float E3D_DegToRad(float d) {
  return d * E3D_PI / 180;
}

float E3D_RadToDeg(float r) {
  return r * 180 / E3D_PI;
}

float E3D_Min(float a, float b) {
  return a < b ? a : b;
}

float E3D_Max(float a, float b) {
  return a > b ? a : b;
}
