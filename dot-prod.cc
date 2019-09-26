#include <math.h>
#include <iostream>

void GetMinMax(float* a, size_t size, float *out_min, float* out_max) {
  *out_min = 0.f;
  *out_max = 0.f;
  for (size_t i = 0; i < size; i++) {
    if (a[i] < *out_min) {
      *out_min = a[i];
    }
    if (a[i] > *out_max) {
      *out_max = a[i];
    }
  }
}

void SymmetricQuant(float* a, float min, float max, size_t size, int8_t* out) {
  const int kScale = 127;
  const float range = std::max(std::abs(min), std::abs(max));
  if (range == 0) {
    // scaling factor === 1...
    // all values are 0
    // ... return.
  }

  float scale = range / kScale;
  const float scaling_factor_inv = kScale / range;
  std::cerr << "scale: " << scale << std::endl;
  for (int i = 0; i < size; i++) {
    out[i] = round(a[i] / scale);
  }
}

void DotProd(float* a, float* b, size_t size, float* out) {
  for (int i = 0; i < size; i++) {
    *out += a[i] * b[i];
  }
}

int main() {
  size_t a_size = 4;
  float a[] = {1.0f, 2.0f, 3.0f, 4.0f};
  float b[] = {5.0f, 6.0f, 7.0f, 8.0f};
  float c;

  DotProd(a, b, a_size, &c);

  std::cerr << "dot prod: " << c << std::endl;

  float a_min, a_max;
  GetMinMax(a, a_size, &a_min, &a_max);

  int8_t quant_a[a_size];
  SymmetricQuant(a, a_min, a_max, a_size, quant_a);
  for (int i = 0; i < a_size; i++) {
    fprintf(stderr, "[%d] = %d\n", i, quant_a[i]);
   }
  return 0;
}
