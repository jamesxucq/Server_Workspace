#include "string_utility.h"
#include "anchor_xml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<anchor version="1.2.1">
<uid>12345321</uid>
<last_anchor>xxxxx</last_anchor>
</anchor>
 */


int anchor_xml_value(char *xml_buffer, struct xml_anchor *anchor) {
    if (!xml_buffer || !anchor) return -1; //modify by james 20110126
//
    char *line_txt = lscpy(xml_buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<anchor version=\"1.2.1\">");
    line_txt = lsprif(line_txt, "<uid>%u</uid>", anchor->uid);
    line_txt = lsprif(line_txt, "<last_anchor>%u</last_anchor>", anchor->last_anchor);
    strcpy(line_txt, "</anchor>");
//
    return 0;
}