#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ProcessCmd.h"

const CmdLineOption_t OPTIONS[] = {
    {"--output_file", "include file with equation",   1, output_file_callback},
    {"--help",        "help",                         0, help_callback}
};
const int COUNT_OPTIONS = sizeof(OPTIONS) / sizeof(CmdLineOption_t);

ErrorCode input_cmd(int argc, const char* argv[], CmdInputData_t* cmd_data)
{
    assert(argv);
    assert(cmd_data);

    ErrorCode err = ERROR_NO;

    for (int i = 0; i < argc; i++) {
        for (int j = 0; j < COUNT_OPTIONS; j++) {
        	if (strcmp(argv[i], OPTIONS[j].name) == 0) {
                err = (*OPTIONS[j].callback)(&argv[i], cmd_data);
                if (err) return err;
                i += OPTIONS[j].n_args;
                break;
            }
        }
    }
    return err;
}

ErrorCode output_file_callback(const char* argv[], CmdInputData_t* data)
{
    data->is_output_file = 1;
    data->output_file = (char*)calloc(MAX_CMD_COMMAND_SIZE, sizeof(char));
    if (!data->output_file) return ERROR_ALLOC_FAIL;
	data->output_file = strcpy(data->output_file, argv[1]);
    if (!data->output_file) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

ErrorCode help_callback(const char* /*argv*/[], CmdInputData_t* data)
{
    data->is_help = 1;

    return ERROR_NO;
}

ErrorCode cmd_data_delete(CmdInputData_t* cmd_data)
{
    if (!cmd_data) return ERROR_INVALID_CMD_DATA;

    if (cmd_data->output_file)
        free(cmd_data->output_file);

    return ERROR_NO;
}