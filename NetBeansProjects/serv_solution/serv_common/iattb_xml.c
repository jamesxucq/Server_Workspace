#include "file_utility.h"
#include "iattb_type.h"
#include "fasd_list.h"
#include "iattb_xml.h"
#include "logger.h"

//
#define ADDI_XML_HEAD(line_buff) strcpy(line_buff, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")

struct text_entry {
    int value;
    char *text;
};

#define VALUE_TEXT(TEXT, TABLE, VALUE) { \
    struct text_entry *item; \
    for(item = (struct text_entry *)TABLE; item->value; ++item) { \
	if(item->value == VALUE) \
            break; \
    } \
    TEXT = item->text; \
}

//
/*
<?xml version="1.0" encoding="UTF-8"?>
<inodes_attrib version="1.2.1">
<inode action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</inode>
<inode action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</inode>
</inodes_attrib>
 */

static struct text_entry list_text_table[] = {
    {RECURSIV, "recu"},
    {LIST, "list"},
    {INVA_ATTR, "inva"}
};

#define ADDI_ATTRIB_LINE(line_buff, text_ptr, time_str, latt) \
        VALUE_TEXT(text_ptr, list_text_table, REALITY_STAT(latt.action_type)) \
        UTC_TIME_TEXT(time_str, latt.last_write) \
        sprintf(line_buff, "<inode action_type=\"%s\" file_size=\"%lu\" last_write=\"%s\" reserved=\"%u\">%s</inode>", \
            text_ptr, latt.file_size, time_str, latt.reserved, latt.file_name)

int flush_elist_xmlfile(FILE *alist_xml, char *location, char *list_file) {
    char line_txt[LINE_LENGTH];
    rewind(alist_xml);
    //
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, list_file);
    FILE *alist = fopen64(file_name, "rb+");
    if (!alist) {
        _LOG_ERROR("not open file:%s", file_name);
        return -1;
    }
// _LOG_DEBUG("flush elist open: %s\n", file_name);
    //
    ADDI_XML_HEAD(line_txt);
    fputs(line_txt, alist_xml);
    fputs("<inodes_attrib>", alist_xml);
    //
    struct list_attrib latt;
    char *text_ptr, last_write[TIME_LENGTH];
    int rlen;
    for (;;) {
        rlen = fread(&latt, sizeof (struct list_attrib), 1, alist);
        if (0x01 != rlen) break;
// fprintf(stderr, "--- xml name: %s\n", latt.file_name);
        ADDI_ATTRIB_LINE(line_txt, text_ptr, last_write, latt);
        fputs(line_txt, alist_xml);
    }
    fputs("</inodes_attrib>", alist_xml);
    //
    return 0;
}

//
/*
<?xml version="1.0" encoding="UTF-8"?>
<inodes_action version="1.2.1">
<inode action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</inode>
<inode action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</inode>
</inodes_action>
 */

static struct text_entry real_text_table[] = {
    {ADDI, "add"},
    {MODIFY, "mod"},
    {DELE, "del"},
    {EXIST, "exis"},
    {COPY, "copy"},
    {MOVE, "move"},
    {INVA_ATTR, "inva"}
};

#define ADDI_ACTION_LINE(line_buff, text_ptr, time_str, fsdo) \
        VALUE_TEXT(text_ptr, real_text_table, REALITY_STAT(fsdo->action_type)) \
        UTC_TIME_TEXT(time_str, fsdo->last_write) \
        sprintf(line_buff, "<inode action_type=\"%s\" file_size=\"%lu\" last_write=\"%s\" reserved=\"%u\">%s</inode>", \
            text_ptr, fsdo->file_size, time_str, fsdo->reserved, fsdo->file_name)

int flush_alist_xmlfile(FILE *alist_xml, struct fsdo_list *alist) {
    char line_buff[LINE_LENGTH];
    struct fsdo_list *alistmp;
    rewind(alist_xml);
    //
    ADDI_XML_HEAD(line_buff);
    fputs(line_buff, alist_xml);
    fputs("<inodes_action>", alist_xml);
    if (alist) {
        char *text_ptr, last_write[TIME_LENGTH];
        struct fatsdo *fsdo;
        // for(el=head;el;el=el->next)
        //

        ALIST_FOREACH(alist, alistmp) {
            fsdo = &alistmp->fsdo;
            ADDI_ACTION_LINE(line_buff, text_ptr, last_write, fsdo);
// _LOG_DEBUG("action_type:%x file_name:%s", fsdo->action_type, fsdo->file_name);
// _LOG_DEBUG("line_buff:%s", line_buff);
            fputs(line_buff, alist_xml);
        }
    }
    fputs("</inodes_action>", alist_xml);
    //
    return 0;
}