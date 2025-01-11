// error_handler.h
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

typedef enum
{
    ERR_MEMORY_ALLOCATION_FAILED,
    ERR_FILE_OPEN_FAILED,
    ERR_UNKNOWN_TOKEN,
    ERR_NULL_POINTER,
    // 添加其他错误代码
} ErrorCode;

// 错误处理函数，打印错误信息并退出程序
void handleError(ErrorCode code, const char *details);

#endif // ERROR_HANDLER_H
