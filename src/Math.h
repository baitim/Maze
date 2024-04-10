#ifndef SRC_MATH_H
#define SRC_MATH_H

#include <math.h>

static const double EPSILON = 1e-9;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

double clamp_double (double x, double min, double max);
double my_pow       (double x, double st);
double fast_pow     (double x, int st);
int is_double_equal (double x, double y);
int is_double_less  (double x, double y);
int is_double_above (double x, double y);
void swap_ints      (int* a, int* b);

#endif // SRC_MATH_H