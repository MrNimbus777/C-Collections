#ifndef ERROR_RESULT_H
#define ERROR_RESULT_H

#define ERRORS_LIST                                     \
    /* General */                                       \
    X(ERR_EMPTY, "NO ERROR")                            \
    X(ERR_UNKNOWN, "UNKNOWN ERROR")                     \
                                                        \
    /* Memory */                                        \
    X(ERR_BAD_ALLOC, "BAD MEMORY ALLOCATION")           \
    X(ERR_OUT_OF_MEMORY, "OUT OF MEMORY")               \
                                                        \
    /* Pointers */                                      \
    X(ERR_NULLPTR, "NULL POINTER")                      \
    X(ERR_INVALID_POINTER, "INVALID POINTER")           \
                                                        \
    /* Arithmetic */                                    \
    X(ERR_DIV_ZERO, "DIVISION BY ZERO")                 \
    X(ERR_OVERFLOW, "ARITHMETIC OVERFLOW")              \
    X(ERR_UNDERFLOW, "ARITHMETIC UNDERFLOW")            \
                                                        \
    /* I/O */                                           \
    X(ERR_IO, "INPUT/OUTPUT ERROR")                     \
    X(ERR_EOF, "END OF FILE")                           \
                                                        \
    /* File system */                                   \
    X(ERR_FILE_NOT_FOUND, "FILE NOT FOUND")             \
    X(ERR_FILE_EXISTS, "FILE ALREADY EXISTS")           \
    X(ERR_FILE_READ, "FILE READ ERROR")                 \
    X(ERR_FILE_WRITE, "FILE WRITE ERROR")               \
    X(ERR_FILE_PERM, "FILE PERMISSION DENIED")          \
                                                        \
    /* Parsing / Format */                              \
    X(ERR_PARSE, "PARSE ERROR")                         \
    X(ERR_FORMAT, "INVALID FORMAT")                     \
                                                        \
    /* Logic */                                         \
    X(ERR_CONFLICT, "CONFLICT DETECTED")                \
    X(ERR_ALREADY_DONE, "OPERATION ALREADY PERFORMED")  \
    X(ERR_NOT_READY, "RESOURCE NOT READY")              \


#define X(code, msg) code,
typedef enum ErrorCode {
    ERRORS_LIST
} ErrorCode;
#undef X

#define X(code, msg) msg,
static const char* const ERROR_MESSAGES[] = {
    ERRORS_LIST
};
#undef X

#define STRUCT_RESULT(T) struct Result_##T {T value; ErrorCode er_code; }
#define RESULT(T) struct Result_##T

#define OK(T, v) ((RESULT(T)){ .value = (v), .er_code = ERR_EMPTY })
#define ERR(T, er_code_) ((RESULT(T)){ .value = (T){0}, .er_code = er_code_})

#define IS_ERR(res) ((res).er_code != ERR_EMPTY)
#define ERR_MSG(res) (ERROR_MESSAGES[(res).er_code])

#define ERCODE_MSG(er_code) (ERROR_MESSAGES[er_code])

#undef ERRORS_LIST

#endif // !ERROR_RESULT_H