// commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>

void cmd_pwd(void);
void cmd_echo(char *args);
void cmd_cp(char *args);
void cmd_mv(char *args);
void cmd_help(void);
bool cmd_exit(void);

#endif