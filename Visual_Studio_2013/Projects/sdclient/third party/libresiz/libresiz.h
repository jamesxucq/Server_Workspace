#ifndef _LIB_RESIZE_H_
#define _LIB_RESIZE_H_

#ifndef _ERRCODE_H_
#define	_ERRCODE_H_

//#define EPERM											1 /* Operation not permitted */
#define EPARAM                                          2
#define ESTATUS                                         3
#define EMOUNT                                     		4
//#define EIO											5 /* I/O error */
//#define EOPEN											6
#define ELARGE                          				7
#define EVERSION										8
#define EDEVICE                                         9
#define ECOMPARE                                        10
#define ECHKCLUS                                 		11
#define ESMALL                                       	12
#define ESECTORS                                 		13
#define EDECOMP                                         14
#define ELOOKUP                                    		15
#define EBITMAP                                        	16
#define ERECON                                  		17
#define ERECHK                                          18
#define EFULL                            				19
#define ENEWSIZ                            				20
#define ENOTHING										21
#define EFSYNC                          				23
#define ERELOCATE                                    	24
#define EPREPARE										25
#define ETCLUSE                                         26
#define ETBITMAP                                        27
#define EBOOT                                           28
#define EDIRTY                                  		29
#define EVINVAL                                         31
#define EVIO                                            32
//#define EBUSY  16 /* Device or resource busy */
//#define EINVAL  22 /* Invalid argument */
//#define EOPNOTSUPP 95 /* Operation not supported on transport endpoint */

#endif	/* _ERRCODE_H_ */


#ifdef __cplusplus
extern "C" {
#endif

#if BUILDING_DLL
#define DLLPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
#define DLLPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

	int DLLPORT ntfsinfo(int *pusiz, char *volume);
	int DLLPORT ntfsresiz(char *volume, int nsize);

#ifdef __cplusplus
}
#endif

#endif /* _LIB_RESIZE_H_ */
