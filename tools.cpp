#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>

#include "tools.hpp"

// Coleta informações de data e transforma em um int
unsigned long long datainfo()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  return ((tm.tm_year + 1900)*pow(10,10) +
          (tm.tm_mon + 1)*pow(10,8) +
          (tm.tm_mday)*pow(10,6) +
          (tm.tm_hour)*pow(10,4) +
          (tm.tm_min)*pow(10,2) +
          (tm.tm_sec));
}
