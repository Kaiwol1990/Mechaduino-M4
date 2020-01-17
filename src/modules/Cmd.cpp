#include <Arduino.h>
#include "Cmd.h"
#include "Configuration.h"

// command line message buffer and pointer
static uint8_t msg[MAX_MSG_SIZE];
static uint8_t *msg_ptr;

static Stream *stream;

SerialCommander::SerialCommander(Stream *str)
{
  stream = str;
  msg_ptr = msg;
  cmdCount = 0;
}

//Generate the main command prompt
void SerialCommander::cmd_display()
{
  stream->println();
  stream->print(cmd_prompt);
}

/**************************************************************************/
/*!
    Parse the command line. This function tokenizes the command input, then
    searches for the command table entry associated with the commmand. Once found,
    it will jump to the corresponding function.
*/
/**************************************************************************/
void SerialCommander::cmd_parse(char *cmd)
{
  uint8_t i = 0;

  fflush(stdout);

  // parse the command line statement and break it up into space-delimited
  // strings. the array of strings will be saved in the argv array.
  args[i] = strtok(cmd, " ");

  while ((args[i] != NULL) && (i < 30))
  {
    args[++i] = strtok(NULL, " ");
  }

  // save off the number of arguments for the particular command.
  arg_cnt = i;

  // parse the command table for valid command. used argv[0] which is the
  // actual command name typed in at the prompt
  for (uint8_t i = 0; i < cmdCount; i++)
  {
    if (!strcmp(args[0], cmdBuffer[i].cmd))
    {
      //cmdBuffer[i].func(argc, argv);
      cmdBuffer[i].func();
      cmd_display();
      return;
    }
  }

  // command not recognized. print message and re-generate prompt.
  stream->println(cmd_unrecognized);
  stream->println("");

  SerialCommander::cmd_display();
}

/**************************************************************************/
/*!
    This function processes the individual characters typed into the command
    prompt. It saves them off into the message buffer unless its a "backspace"
    or "enter" key.
*/
/**************************************************************************/
void SerialCommander::cmd_handler(char c)
{
  switch (c)
  {
  case '\r':
    // terminate the msg and reset the msg ptr. then send
    // it to the handler for processing.
    *msg_ptr = '\0';
    stream->print("\r\n");
    cmd_parse((char *)msg);
    msg_ptr = msg;
    break;

  case '\n':
    // ignore newline characters. they usually come in pairs
    // with the \r characters we use for newline detection.
    break;

  case '\b':
    // backspace
    stream->print(c);
    if (msg_ptr > msg)
    {
      msg_ptr--;
    }
    break;

  default:
    // normal character entered. add it to the buffer
    stream->print(c);
    *msg_ptr++ = c;
    break;
  }
}

// if Serial available parse it to handler
void SerialCommander::cmdPoll()
{
  while (stream->available())
  {
    cmd_handler(stream->read());
  }
}

//Add a command to the command table.
void SerialCommander::cmdAdd(const char *name, const char *description, void (*func)())
{
  // alloc memory for command name
  char *cmd_name = (char *)malloc(strlen(name) + 1);

  // alloc memory for command description
  char *cmd_des = (char *)malloc(strlen(description) + 1);

  // copy command name and description
  strcpy(cmd_name, name);
  strcpy(cmd_des, description);

  // terminate the command name
  cmd_name[strlen(name)] = '\0';

  // add data to cmd Buffer
  cmdBuffer[cmdCount].cmd = cmd_name;
  cmdBuffer[cmdCount].description = cmd_des;
  cmdBuffer[cmdCount].func = func;

  // increment counter
  cmdCount++;
}

void SerialCommander::printlist()
{
  for (uint8_t i = 0; i < cmdCount; i++)
  {
    uint8_t length = strlen(cmdBuffer[i].cmd);

    Serial.print(cmdBuffer[i].cmd);

    for (uint8_t k = 0; k < (10 - length); k++)
    {
      Serial.print(" ");
    }
    Serial.print("\t - ");
    Serial.println(cmdBuffer[i].description);
  }
}

// Checks if a argument was called and returns the integer after it
int32_t SerialCommander::return_integer_argument(const char *identifier, int32_t default_value, int32_t min_value, int32_t max_value)
{

  int32_t val = default_value;
  for (uint8_t i = 1; i < arg_cnt; i++)
  {
    String argument = args[i];
    if (argument == identifier)
    {
      if (strcmp(args[i + 1], " ") != 220)
      {
        val = strtol(args[i + 1], NULL, 10);
      }
    }
  }

  val = constrain(val, min_value, max_value);
  return val;
}

//Checks if a argument was called and returns the float after it
float SerialCommander::return_float_argument(const char *identifier, float default_value, float min_value, float max_value)
{

  float val = default_value;
  for (byte i = 1; i < arg_cnt; i++)
  {
    String argument = args[i];
    if (argument == identifier)
    {

      val = strtof(args[i + 1], NULL);
    }
  }

  if (val > max_value)
  {
    val = max_value;
  }
  else if (val < min_value)
  {
    val = min_value;
  }

  return val;
}

//Checks if a argument was called and returns the boolean value after it
bool SerialCommander::return_bool_argument(const char *identifier, bool default_value)
{

  bool val = default_value;
  for (byte i = 1; i < arg_cnt; i++)
  {
    String argument = args[i];
    if (argument == identifier)
    {
      val = strtof(args[i + 1], NULL);
    }
  }
  //val = constrain(val, 0, 1);
  return val;
}

//Checks if a argument was called and returns true if the argument was found
bool SerialCommander::check_argument(const char *identifier)
{

  for (byte i = 1; i < arg_cnt; i++)
  {
    String argument = args[i];
    if (argument == identifier)
    {
      return true;
    }
  }
  return false;
}
