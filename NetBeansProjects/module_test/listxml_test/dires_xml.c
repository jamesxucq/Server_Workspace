// #include "file_utility.h"
//#include "utility/utlist.h"
// #include "fatt_xml.h"
#include "dires_xml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<directories version="1.2.1">
<dir_name>xxxxxxxxxx</dir_name>
<dir_name>xxxxxxxxxx</dir_name>
</directories>
 */

#define ADD_XML_HEAD(line_buff) strcpy(line_buff, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
#define ADD_DIR_LINE(line_buff, dname) \
    sprintf(line_buff, "<dir_name>%s</dir_name>", dname)

int flush_dlist_xmlfile(FILE *dlist_xml, char *location, char *list_file) {
    char line_txt[LINE_LENGTH];
    rewind(dlist_xml);
    //
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, list_file);
    FILE *dlist = fopen64(file_name, "rb+");
    if (!dlist) return -1;
    //
    ADD_XML_HEAD(line_txt);
    fputs(line_txt, dlist_xml);
    fputs("<directories>", dlist_xml);
    //
    char dir_name[MAX_PATH * 3];
    int rlen;
    for (;;) {
        rlen = fread(dir_name, MAX_PATH * 3, 0x01, dlist);
        if (0x01 != rlen) break;
        ADD_DIR_LINE(line_txt, dir_name);
        fputs(line_txt, dlist_xml);
    }
    fputs("</directories>", dlist_xml);
    //
    return 0;
}
