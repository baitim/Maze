#include "Math.h"

double clamp_double(double x, double min, double max)
{
    return MIN(MAX(x, min), max);
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