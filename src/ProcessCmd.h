#ifndef SRC_PROCESS_CMD_H
#define SRC_PROCESS_CMD_H

#include "Errors.h"

const int MAX_CMD_COMMAND_SIZE = 100;

typedef struct CmdInputData_t_ {
    int is_output_file;
    int is_help;
    char* output_file;
} CmdInputData_t;

typedef struct CmdLineOption_t_ {
    const char* name;
    const char* description;
    int n_args;
    ErrorCode (*callback)(const char* argv[], CmdInputData_t* data);
} CmdLineOption_t;

ErrorCode input_cmd         (int argc, const char* argv[], CmdInputData_t* cmd_data);
ErrorCode help_callback     (const char* /*argv*/[], CmdInputData_t* data);
ErrorCode cmd_data_delete   (CmdInputData_t* cmd_data);
ErrorCode output_file_callback(const char* argv[], CmdInputData_t* data);

extern const CmdLineOption_t OPTIONS[];
extern const int COUNT_OPTIONS;

#endif // SRC_PROCESS_CMD_H