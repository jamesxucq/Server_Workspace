#include "regis_xml.h"    

/*
<?xml version="1.0" encoding="UTF-8"?>
<register version="1.2.1">
<uid>12345321</uid>
<cache address="123.354.345.345" port="325" />
<access_key>2343ewr544r454rewfa45qwee</access_key>
</register>
 */

int regis_xml_value(char *xml_txt, struct xml_register *regis) {
    if (!xml_txt || !regis) return -1; //modify by james 20110126
//
    char *line_txt = lscpy(xml_txt, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<register version=\"1.2.1\">");
    line_txt = lsprif(line_txt, "<uid>%u</uid>", regis->uid);
    line_txt = lsprif(line_txt, "<cache address=\"%s\" port=\"%d\" />", regis->cache_addr.sin_addr, regis->cache_addr.sin_port);
    line_txt = lsprif(line_txt, "<access_key>%s</access_key>", regis->access_key);
    strcpy(line_txt, "</register>");
//
    return 0;
}