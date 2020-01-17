#ifndef __SerialTools_H__
#define __SerialTools_H__

#include <arduino.h>

void SoftReset(int arg_cnt, char **args);

void set_TC(int arg_cnt, char **args);

void get_max_frequency(int arg_cnt, char **args);

#endif
