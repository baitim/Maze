#ifndef SERVER_SRC_ERRORS_H
#define SERVER_SRC_ERRORS_H

typedef struct Error_t_ {
    int error;
    const char* description;
} Error_t;

typedef enum ErrorCode_ {
    ERROR_NO =                  0,
    ERROR_ALLOC_FAIL =          1 << 0,
    ERROR_OPEN_FILE =           1 << 1,
    ERROR_INVALID_CMD_DATA =    1 << 2,
    ERROR_INVALID_FILE_MAP =    1 << 3,
} ErrorCode;

extern const Error_t ERRORS[];
extern const int COUNT_ERRORS;

#define err_dump(err) err_dump_(err, __FILE__, __PRETTY_FUNCTION__, __LINE__) 
void err_dump_(int err, const char* file, const char* func, int line);

#endif // SERVER_SRC_ERRORS_H