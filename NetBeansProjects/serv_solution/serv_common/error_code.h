#ifndef ERROR_CODE_H_
#define ERROR_CODE_H_

#ifdef	__cplusplus
extern "C" {
#endif

    typedef unsigned int ErrCode;
    typedef unsigned int ec;

    //
#define ERR_SUCCESS     0x00
#define	ERR_FAULT       -1
#define	ERR_EXCEP       0x01

    /* View Error Code Define */
#define  VIEW_BASE      0x01000000
#define ERR_SOCK_BIND   VIEW_BASE | 0x000001
    /* Ctrl Error Code Define */
#define  CTRL_BASE      0x03000000
    /* Mode Error Code Define */
#define  MODE_BASE      0x05000000
// #define  ERR_CONFIG     MODE_BASE | 0x000001
// #define  ERR_LOAD       MODE_BASE | 0x000003

#ifdef	__cplusplus
}
#endif

#endif /* ERROR_CODE_H_ */
