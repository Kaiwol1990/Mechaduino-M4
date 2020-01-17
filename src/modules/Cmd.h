#ifndef CMD_H
#define CMD_H

#include <stdint.h>
#include "Configuration.h"

#define cmd_prompt "CMD >> "
#define cmd_unrecognized "CMD: Command not recognized."
#define help_cmd "help"
#define help_subcmd "-help"
#define MAX_MSG_SIZE 256

// command line structure
typedef struct _cmd_t
{
  char *cmd;
  char *description;
  //void (*func)(int argc, char **argv);
  void (*func)();
  struct _cmd_t *next;
} cmd_t;

class SerialCommander
{
public:
  SerialCommander(Stream *);

  void cmdPoll();

  void cmdAdd(const char *name, const char *description, void (*func)());

  void printlist();

  int32_t return_integer_argument(const char *identifier, int32_t default_value, int32_t min_value, int32_t max_value);

  float return_float_argument(const char *identifier, float default_value, float min_value, float max_value);

  bool return_bool_argument(const char *identifier, bool default_value);

  bool check_argument(const char *identifier);

private:
  char *args[30];
  uint8_t arg_cnt;

  // counter that contains the number of commands
  uint8_t cmdCount;

  // Buffer to hold up to 100 commands
  cmd_t cmdBuffer[100];

  void cmd_handler(char c);
  void cmd_parse(char *cmd);
  void cmd_display();
};

#endif //CMD_H
