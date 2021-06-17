#include <stdio.h>
#include <stdlib.h>

#include "server_macro.h"
#include "text_value.h"

//
const struct value_node content_value_table[] = {
        {"text/xml", CONTENT_TYPE_XML},
        {"application/octet-stream", CONTENT_TYPE_OCTET},
        {"text/plain", CONTENT_TYPE_PLAIN},
        {NULL, 0}
    };
