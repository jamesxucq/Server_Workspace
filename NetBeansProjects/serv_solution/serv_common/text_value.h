
/* 
 * File:   text_value.h
 * Author: Administrator
 *
 * Created on 2017.1.18, AM9:07
 */

#ifndef TEXT_VALUE_H
#define TEXT_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

    struct value_node {
        const char *text;
        int value;
    };

#define TEXT_VALUE(VALUE, TABLE, TEXT) { \
    struct value_node *item; \
    for(item = (struct value_node *)TABLE; item->text; ++item) \
        if(!strcmp(item->text, TEXT)) \
            break; \
    VALUE = item->value; \
}

    //
    extern const struct value_node content_value_table[];


#ifdef __cplusplus
}
#endif

#endif /* TEXT_VALUE_H */

