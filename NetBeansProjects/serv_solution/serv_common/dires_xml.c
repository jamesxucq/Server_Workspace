#include "file_utility.h"
// #include "utility/utlist.h"
// #include "fatt_xml.h"
#include "dires_xml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<inodes_dire version="1.2.1">
<inode>xxxxxxxxxx</inode>
<inode>xxxxxxxxxx</inode>
</inodes_dire>
 */

#define ADDI_XML_HEAD(line_buff) strcpy(line_buff, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
#define ADDI_IDIRE_LINE(line_buff, dname) \
    sprintf(line_buff, "<inode>%s</inode>", dname)

int flush_dlist_xmlfile(FILE *dlist_xml, char *location, char *list_file) {
    char line_txt[LINE_LENGTH];
    rewind(dlist_xml);
    //
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, list_file);
    FILE *dlist = fopen64(file_name, "rb+");
    if (!dlist) {
        _LOG_ERROR("not open file:%s", file_name);
        return -1;
    }
    //
    ADDI_XML_HEAD(line_txt);
    fputs(line_txt, dlist_xml);
    fputs("<inodes_dire>", dlist_xml);
    //
    char dire_name[MAX_PATH * 3];
    int rlen;
    for (;;) {
        rlen = fread(dire_name, MAX_PATH * 3, 1, dlist);
        if (0x01 != rlen) break;
        ADDI_IDIRE_LINE(line_txt, dire_name);
        fputs(line_txt, dlist_xml);
    }
    fputs("</inodes_dire>", dlist_xml);
    //
    return 0;
}
