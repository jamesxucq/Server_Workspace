#include "query_xml.h"    

/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>2</uid>
<validation>VB48uIcNsS9JvOaY0btu6Kaqmo1wLGpY</validation>
<worker address="192.168.1.103" port="8090" />
<cached_anchor>0</cached_anchor>
<session_id>9209-1348697447-2</session_id>
</query>
 */

int query_xml_value(char *xml_txt, struct xml_query *query_status) {
    if (!xml_txt || !query_status) return -1; //modify by james 20110126
//
    char *line_txt = lscpy(xml_txt, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<query version=\"1.2.1\">");
    line_txt = lsprif(line_txt, "<uid>%u</uid>", query_status->uid);
    line_txt = lsprif(line_txt, "<validation>%s</validation>", query_status->req_valid);
    line_txt = lsprif(line_txt, "<worker address=\"%s\" port=\"%d\" />",
            query_status->saddr.sin_addr, query_status->saddr.sin_port);
    line_txt = lsprif(line_txt, "<cached_anchor>%u</cached_anchor>", query_status->cached_anchor);
    line_txt = lsprif(line_txt, "<session_id>%s</session_id>", query_status->seion_id);
    strcpy(line_txt, "</query>");
//
    return 0;
}

