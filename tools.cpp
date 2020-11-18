#ifndef _TOOLS_CPP
#define _TOOLS_CPP

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>

#include "tools.hpp"

// Coleta informações de data e transforma em um int
int datainfo()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  return ((tm.tm_year + 1900)*pow(10,4) +
          (tm.tm_mon + 1)*pow(10,2) +
          (tm.tm_mday));
}

int clockinfo()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  return ((tm.tm_hour)*pow(10,4) +
          (tm.tm_min)*pow(10,2) +
          (tm.tm_sec));
}


#endif
