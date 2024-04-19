#include <stdlib.h>
#include <string.h>

#include "Output.h"
#include "ProcessCmd.h"

const CmdLineOption_t OPTIONS[] = {
    {"--map_txt_file",    "include file to save map in txt",    1, map_txt_file_callback},
    {"--screenshot_file", "include file to save sreenshot",     1, screenshot_file_callback},
    {"--font_file",       "include file to set font",           1, font_file_callback},
    {"--help",            "help",                               0, help_callback}
};
const int COUNT_OPTIONS = sizeof(OPTIONS) / sizeof(CmdLineOption_t);

ErrorCode cmd_data_init(int argc, const char* argv[], CmdInputData_t* cmd_data)
{
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

ErrorCode map_txt_file_callback(const char* argv[], CmdInputData_t* data)
{
    data->is_map_txt_file = 1;
    data->map_txt_file = strdup(argv[1]);
    if (!data->map_txt_file) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

ErrorCode screenshot_file_callback(const char* argv[], CmdInputData_t* data)
{
    data->is_screenshot_file = 1;
    data->screenshot_file = strdup(argv[1]);
    if (!data->screenshot_file) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

ErrorCode font_file_callback(const char* argv[], CmdInputData_t* data)
{
    data->is_font_file = 1;
    data->font_file = strdup(argv[1]);
    if (!data->font_file) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

ErrorCode help_callback(const char* /*argv*/[], CmdInputData_t* data)
{
    data->is_help = 1;

    return ERROR_NO;
}

ErrorCode cmd_data_verify(CmdInputData_t* cmd_data)
{
    if (!cmd_data->is_map_txt_file)
        return ERROR_INVALID_FILE_MAP;

    if (!cmd_data->is_screenshot_file)
        return ERROR_INVALID_FILE_SCR;

    if (!cmd_data->font_file)
        return ERROR_INVALID_FILE_FONT;

    return ERROR_NO;
}

ErrorCode cmd_data_callback(CmdInputData_t* cmd_data)
{
    if (cmd_data->is_help)
        print_help();

    return ERROR_NO;
}

ErrorCode cmd_data_delete(CmdInputData_t* cmd_data)
{
    if (!cmd_data) return ERROR_INVALID_CMD_DATA;

    if (cmd_data->map_txt_file)
        free(cmd_data->map_txt_file);

    if (cmd_data->screenshot_file)
        free(cmd_data->screenshot_file);

    if (cmd_data->font_file)
        free(cmd_data->font_file);

    return ERROR_NO;
}