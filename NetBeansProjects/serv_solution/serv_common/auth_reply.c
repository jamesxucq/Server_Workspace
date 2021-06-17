#include "auth_reply.h"

//
const char *bool_text_table[] = {
    "false",
    "true"
};

//
const struct value_node execute_value_table[] = {
    {"successful", 0},
    {"exception", 1},
    {NULL, 0}
};
