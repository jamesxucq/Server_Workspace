#ifndef MATCH_FILES_H_
#define MATCH_FILES_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "session.h"

#define CONEN_MODI          0x01
#define CHECK_SUM           0x02
    
#pragma	pack(1)

    struct file_matcher {
        unsigned char match_type; //m: 0x01 data. CHECK_SUM: sum data
        uint32 offset; // new offset
        unsigned int inde_len;
    };
#pragma	pack()

    struct local_match {
        struct file_matcher *matcher;
        unsigned char *buffer;
    };

    struct match_list {
        struct match_list *next;
        struct local_match match;
    };
    typedef struct match_list **match_listh;

#define MLIST_APPEND(head, add) { \
  LDECLTYPE(head) _tmp; \
  (add)->next=NULL; \
  if (head) { \
    _tmp = head; \
    while (_tmp->next) { _tmp = _tmp->next; } \
    _tmp->next=(add); \
  } else { (head)=(add); } \
}

#define MLIST_FOREACH(head,el) \
    for(el=head;el;el=el->next)

    //
    int match_chks_by_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    //
    struct file_matcher *add_chks_match(match_listh mlisth);
    struct local_match *add_modify_data(match_listh mlisth, int length);
    void delete_mlist(struct match_list *mlist);
    //
    int flush_match_file(struct seion_ctrl *sctrl, struct inte_value *ival);

#ifdef	__cplusplus
}
#endif

#endif /* MATCH_FILES_H_ */
