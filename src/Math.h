#ifndef SRC_MATH_H
#define SRC_MATH_H

#include <math.h>

static const double EPSILON = 1e-9;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

char clamp_char     (char x, char min, char max);
unsigned char clamp_uchar(unsigned char x, unsigned char min, unsigned char max);
int clamp_int       (int x, int min, int max);
double clamp_double (double x, double min, double max);
double my_pow       (double x, double st);
double fast_pow     (double x, int st);
char max_chars      (char x, char y);
char min_chars      (char x, char y);
unsigned char max_uchars(unsigned char x, unsigned char y);
unsigned char min_uchars(unsigned char x, unsigned char y);
int max_ints        (int x, int y);
int min_ints        (int x, int y);
double max_doubles  (double x, double y);
double min_doubles  (double x, double y);
int is_double_equal (double x, double y);
int is_double_less  (double x, double y);
int is_double_above (double x, double y);
void swap_ints      (int* x, int* y);

#endif // SRC_MATH_H