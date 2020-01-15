#ifndef __Serial_H__
#define __Serial_H__


#include <arduino.h>



void init_menu();

void reportPostition(int arg_cnt, char **args);

void set_gains(int arg_cnt, char **args);

void set_motion(int arg_cnt, char **args);

void splineMovement(int arg_cnt, char **args);

void set_TC(int arg_cnt, char **args);

void state(int arg_cnt, char **args);

void SoftReset(int arg_cnt, char **args);

void Serial_menu(int arg_cnt, char **args);

void step_response(int arg_cnt, char **args);

void get_max_frequency(int arg_cnt, char **args);

void paramter_streamer(int arg_cnt, char **args);

#endif
