#ifndef CMD_H
#define CMD_H

#define MAX_MSG_SIZE    256
#include <stdint.h>
#include "Configuration.h"

// command line structure
typedef struct _cmd_t
{
  char *cmd;
  void (*func)(int argc, char **argv);
  struct _cmd_t *next;
} cmd_t;

void cmdInit(Stream *);
void cmdPoll();
void cmdAdd(const char *name, void (*func)(int argc, char **argv));



int32_t return_integer_argument(char **args, uint8_t arg_cnt, const char *identifier, int32_t default_value, int32_t min_value, int32_t max_value);

float return_float_argument(char **args, uint8_t arg_cnt, const char *identifier, float default_value, float min_value, float max_value);

bool return_bool_argument(char **args, uint8_t arg_cnt, const char *identifier, bool default_value);

String return_char_argument(char **args, uint8_t arg_cnt, const char *identifier, const String default_value);

bool check_argument(char **args, uint8_t arg_cnt, const char *identifier);

void insert_command(const char* command_string);

//bool userqst(int timeout, const String qst_string);

#endif //CMD_H
