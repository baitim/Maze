#include "Math.h"

char clamp_char(char x, char min, char max)
{
    return min_chars(max_chars(x, min), max);
}

unsigned char clamp_uchar(unsigned char x, unsigned char min, unsigned char max)
{
    return min_uchars(max_uchars(x, min), max);
}

int clamp_int(int x, int min, int max)
{
    return min_ints(max_ints(x, min), max);
}

double clamp_double(double x, double min, double max)
{
    return min_doubles(max_doubles(x, min), max);
}

double my_pow(double x, double st)
{
    if (is_double_equal(st, (int)st) && (int)st >= 0)
        return fast_pow(x, (int)st);
    else
        return pow(x, st);
}

double fast_pow(double x, int st)
{
    if (st == 0) return 1;
    if (st % 2 == 1) return x * fast_pow(x, st - 1);
    double z = fast_pow(x, st / 2);
    return z * z;
}

char max_chars(char x, char y)
{
    return x > y ? x : y;
}

char min_chars(char x, char y)
{
    return x < y ? x : y;
}

unsigned char max_uchars(unsigned char x, unsigned char y)
{
    return x > y ? x : y;
}

unsigned char min_uchars(unsigned char x, unsigned char y)
{
    return x < y ? x : y;
}

int max_ints(int x, int y)
{
    return x > y ? x : y;
}

int min_ints(int x, int y)
{
    return x < y ? x : y;
}

double max_doubles(double x, double y)
{
    return x > y ? x : y;
}

double min_doubles(double x, double y)
{
    return x < y ? x : y;
}

int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}

int is_double_less(double x, double y)
{
    return (x - y <= EPSILON);
}

int is_double_above(double x, double y)
{
    return (y - x <= EPSILON);
}

void swap_ints(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}