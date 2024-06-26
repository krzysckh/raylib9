#include <stdio.h>
#include <math.h>

typedef long long vlong;

extern void abort(void);

double
round(double f) 
{
  if ((double)(f-(int)f) > 0.5)
    return ((int)f)+1;
  return (double)(int)f;
}

float
floorf(float f)
{
  return floor(f);
}

float
fabsf(float f)
{
  if (f < 0)
    return -f;
  return f;
}

float
fmaxf(float a, float b)
{
  if (a > b)
    return a;
  return b;
}


double
fmax(double a, double b)
{
  if (a > b)
    return a;
  return b;
}


float
fminf(float a, float b)
{
  if (a > b)
    return b;
  return a;
}

float
sqrtf(float f)
{
  return sqrt(f);
}

double
exp2(double f)
{
  return pow(2,f);
}

float
atan2f(float a, float b)
{
  return atan2(a, b);
}

float
cosf(float f)
{
  return cos(f);
}

float
sinf(float f)
{
  return sin(f);
}

float
tanf(float f)
{
  return tan(f);
}

float
powf(float a, float b)
{
  return pow(a, b);
}

float
fmodf(float a, float b)
{
  return fmod(a, b);
}

float
roundf(float f)
{
  return round(f);
}

float
hypotf(float a, float b)
{
  return hypot(a, b);
}

float
logf(float f)
{
  return log(f);
}

float
acosf(float f)
{
  return acos(f);
}

float
asinf(float f)
{
  return asin(f);
}

float
ceilf(float f)
{
  return ceil(f);
}

void
sysfatal(char *s)
{
  fprintf(stderr, "fuck: %s", s);
  abort();
}
