/* cache_serv.h
 * Created on: 2010-3-10
 * Author: David
 */

#ifndef SERVMAIN_H_
#define SERVMAIN_H_

#ifdef	__cplusplus
extern "C" {
#endif

int initialize_serv(char *xml_config);
int finalize_serv();

#ifdef	__cplusplus
}
#endif

#endif /* SERVMAIN_H_ */
