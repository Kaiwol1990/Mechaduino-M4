#ifndef __Serial_H__
#define __Serial_H__

#include <arduino.h>

void init_menu();

void Serial_menu(int arg_cnt, char **args);

void cmd_calibration(int arg_cnt, char **args);

void reportPostition(int arg_cnt, char **args);

void state(int arg_cnt, char **args);

#endif
