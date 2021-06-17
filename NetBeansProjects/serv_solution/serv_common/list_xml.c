#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list_xml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<list version="1.2.1">
<admin>xxxxxx</admin>
<admin>xxxxxxxxx</admin>
</list>
 */
int admin_xml_value(char *xml_txt, struct addr_list *alist) {
    char *line_txt = lscpy(xml_txt, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<list version=\"1.2.1\">");
    struct addr_list *alistmp = alist;
    while (alistmp) {
        line_txt = lsprif(line_txt, "<admin>%s</admin>", alistmp->saddr.sin_addr);
        alistmp = alistmp->next;
    }
    strcpy(line_txt, "</list>");
    //
    return 0;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<list version="1.2.1">
<server>xxxxxx</server>
<server>xxxxxxxxx</server>
</list>
 */
int serv_xml_value(char *xml_txt, struct server_list *slist) {
    char *line_txt = lscpy(xml_txt, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<list version=\"1.2.1\">");
    struct server_list *servlist = slist;
    while (servlist) {
        line_txt = lsprif(line_txt, "<server>%s:%d</server>", servlist->saddr.sin_addr, servlist->saddr.sin_port);
        servlist = servlist->next;
    }
    strcpy(line_txt, "</list>");
    //
    return 0;
}


