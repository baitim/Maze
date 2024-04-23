#ifndef SERVER_SRC_PROCESS_CMD_H
#define SERVER_SRC_PROCESS_CMD_H

#include "Errors.h"

#define MAX_CMD_COMMAND_SIZE (int)30

typedef struct CmdInputData_t_ {
    int is_map_txt_file;
    int is_help;
    char* map_txt_file;
} CmdInputData_t;

typedef struct CmdLineOption_t_ {
    const char* name;
    const char* description;
    int n_args;
    ErrorCode (*callback)(const char* argv[], CmdInputData_t* data);
} CmdLineOption_t;

ErrorCode cmd_data_init     (int argc, const char* argv[], CmdInputData_t* cmd_data);
ErrorCode cmd_data_verify   (CmdInputData_t* cmd_data);
ErrorCode cmd_data_callback (CmdInputData_t* cmd_data);
ErrorCode cmd_data_delete   (CmdInputData_t* cmd_data);
ErrorCode help_callback     (const char* /*argv*/[], CmdInputData_t* data);
ErrorCode map_txt_file_callback     (const char* argv[], CmdInputData_t* data);

extern const CmdLineOption_t OPTIONS[];
extern const int COUNT_OPTIONS;

#endif // SERVER_SRC_PROCESS_CMD_H