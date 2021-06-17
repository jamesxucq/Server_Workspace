#ifndef _BUILDFILES_H_
#define _BUILDFILES_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "match_utility.h"

struct file_builder {
    char file_name[MAX_PATH * 3];
    uint32 length;
    uint64 offset;
};

// 
int build_file_content_memory(struct file_builder *builder, BYTE *ptrm);
int build_chunk_content_memory(struct file_builder *builder, BYTE *ptrm);
extern int (*build_content_by_memory[])(struct file_builder *, BYTE *);

//
int build_chunk_content_file(struct file_builder *builder, FILE *build_stre);
int build_file_content_file(struct file_builder *builder, FILE *build_stre);
extern int (*build_content_by_file[])(struct file_builder *, FILE *);

#define MATCH_INFORMATION_END   1 // sizeof(struct file_matcher)

#ifdef	__cplusplus
}
#endif

#endif /* _BUILDFILES_H_ */
