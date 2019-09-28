#include <math.h>
#include <iostream>

void GetMinMax(float* a, size_t size, float *out_min, float* out_max) {
  for (size_t i = 0; i < size; i++) {
    if (i == 0) {
      *out_min = a[i];
      *out_max = a[i];
    } else {
      if (a[i] < *out_min) {
        *out_min = a[i];
      }
      if (a[i] > *out_max) {
        *out_max = a[i];
      }
    }
  }
}

void SymmetricQuant(float* a, float min, float max, size_t size,
        float *scale, int8_t* out) {
  const int kScale = 127;
  const float range = std::max(std::abs(min), std::abs(max));
  *scale = range / kScale;
  for (int i = 0; i < size; i++) {
    if (range == 0) {
      out[0] = 0;
    } else {
      out[i] = round(a[i] / *scale);
    }
  }
}

void Dequant(const int8_t* v, float scale, const int size, float* out) {
  for (int i = 0; i < size; i++) {
    out[i] = v[i] * scale;
  }
}

void Sum(float* a, float* b, size_t size, float* out) {
  *out = 0;
  for (int i = 0; i < size; i++) {
    *out += a[i] + b[i];
  }
}

void Sum(int8_t* a, float a_scale, int8_t* b, float b_scale,
         float c_scale, size_t size, int32_t* out) {
  *out = 0;
  float s = (a_scale * b_scale) / c_scale;
  for (int i = 0; i < size; i++) {
    *out += ((a[i] * a_scale) + (b[i] * b_scale)) / c_scale;
  }
}

void DotProd(float* a, float* b, size_t size, float* out) {
  *out = 0;
  for (int i = 0; i < size; i++) {
    *out += a[i] * b[i];
  }
}

void DotProd(int8_t* a, float a_scale, int8_t* b, float b_scale,
             float c_scale, size_t size, int32_t* out) {
  *out = 0;
  for (int i = 0; i < size; i++) {
    *out += a[i] * b[i];
  }
  *out = round(*out * ((a_scale * b_scale) / c_scale));
}

int main() {
  size_t a_size = 4;
  size_t b_size = 4;
  float a[] = {1.0f, 2.0f, 3.0f, 4.0f};
  float b[] = {5.0f, 6.0f, 7.0f, 8.0f};
  float c;

  DotProd(a, b, a_size, &c);
  float c_scale = c / 127;

  std::cerr << "dot prod: " << c << std::endl;

  float a_min, a_max;
  GetMinMax(a, a_size, &a_min, &a_max);

  float b_min, b_max;
  GetMinMax(b, b_size, &b_min, &b_max);

  float a_scale;
  int8_t quant_a[a_size];
  SymmetricQuant(a, a_min, a_max, a_size, &a_scale, quant_a);

  float b_scale;
  int8_t quant_b[b_size];
  SymmetricQuant(b, b_min, b_max, b_size, &b_scale, quant_b);

  int32_t q_dot_prod;
  DotProd(quant_a, a_scale, quant_b, b_scale, c_scale, a_size, &q_dot_prod);
  std::cerr << "q_dot_prod         : " << q_dot_prod << std::endl;
  std::cerr << "q_dot_prod (scaled): " << q_dot_prod * c_scale << std::endl;

  std::cerr << std::endl;

  float d;
  Sum(a, b, a_size, &d);
  std::cerr << "sum: " << d << std::endl;
  float d_scale = d / 127;
  std::cerr << "sum scale: " << d_scale << std::endl;

  int32_t q_sum;
  Sum(quant_a, a_scale, quant_b, b_scale, d_scale, a_size, &q_sum);
  std::cerr << "quant sum         : " << q_sum << std::endl;
  std::cerr << "quant sum (scaled): " << q_sum * d_scale << std::endl;

  return 0;
}
