#include "common_macro.h"
#include "string_utility.h"

#include "settings_xml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<settings version="1.2.1">
<uid>12345321</uid>
<client_version>1.12</client_version>
<pool_size>1</pool_size>
<data_bomb>false</data_bomb>
</settings>
 */

static const char *bool_text_tab[] = {
    "false",
    "true"
};

int settings_xml_value(char *xml_txt, struct xml_settings *settings) {
    if (!xml_txt || !settings) return -1; //modify by james 20110126
//
    char *line_txt = lscpy(xml_txt, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<settings version=\"1.2.1\">");
    line_txt = lsprif(line_txt, "<uid>%u</uid>", settings->uid);
    line_txt = lsprif(line_txt, "<client_version>%s</client_version>", settings->client_version);
    line_txt = lsprif(line_txt, "<pool_size>%d</pool_size>", settings->pool_size);
    line_txt = lsprif(line_txt, "<data_bomb>%s</data_bomb>", bool_text_tab[settings->data_bomb]);
    strcpy(line_txt, "</settings>");
//
    return 0;
}


