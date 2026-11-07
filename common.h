#ifndef COMMON_H
#define COMMON_H

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

#endif
