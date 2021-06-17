// #include "file_utility.h"
// #include "fatt_type.h"
// #include "fatt_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "com.h"
#include "fatt_xml.h"
// #include "logger.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<files_attrib version="1.2.1">
<file action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</file>
<file action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</file>
</files_attrib>
 */

#define ADD_XML_HEAD(line_buff) strcpy(line_buff, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")

struct text_entry {
    int value;
    char *text;
};

#define VALUE_TEXT(TEXT, TABLE, VALUE) { \
    struct text_entry *entry; \
    for(entry = (struct text_entry *)TABLE; entry->value; ++entry) { \
	if(entry->value == VALUE) \
            break; \
    } \
    TEXT = entry->text; \
}
static struct text_entry modify_text_table[] = {
    {RECU, "recu"},
    {LIST, "list"},
    {NONE, "none"}
};

#define ADD_ATTRIB_LINE(line_buff, text_ptr, time_str, latt) \
        VALUE_TEXT(text_ptr, modify_text_table, latt.action_type) \
        UTC_TIME_TEXT(time_str, latt.last_write) \
        sprintf(line_buff, "<file action_type=\"%s\" file_size=\"%llu\" last_write=\"%s\" reserved=\"%u\">%s</file>", \
          text_ptr, latt.file_size, time_str, latt.reserved, latt.file_name)

int flush_elist_xmlfile(FILE *alist_xml, char *location, char *list_file) {
    char line_txt[LINE_LENGTH];
    rewind(alist_xml);
    //
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, list_file);
    FILE *alist = fopen64(file_name, "rb+");
    if (!alist) return -1;
    printf("flush elist open: %s\n", file_name);
    //
    ADD_XML_HEAD(line_txt);
    fputs(line_txt, alist_xml);
    fputs("<files_attrib>", alist_xml);
    //
    struct list_attrib latt;
    char *text_ptr, last_write[TIME_LENGTH];
    int rlen;
    for (;;) {
        rlen = fread(&latt, sizeof (struct list_attrib), 0x01, alist);
        if (0x01 != rlen) break;
        printf("--- xml name: %s\n", latt.file_name);
        ADD_ATTRIB_LINE(line_txt, text_ptr, last_write, latt);
        fputs(line_txt, alist_xml);
    }
    fputs("</files_attrib>", alist_xml);
    //
    return 0;
}
