/* Amiga-specific additions and replacements for PSPZX81
 * by Chris Young <chris@unsatisfactorysoftware.co.uk> 2008
 */

#ifndef Z81_AMIGA_H  
#define Z81_AMIGA_H  

#include <workbench/startup.h>

char amiga_data_dir[256];
char *amiga_file_request_retval;

int amiga_open_libs(void);
void amiga_close_libs(void);

void amiga_read_tooltypes(struct WBStartup *);
char *amiga_file_request(char *, BOOL);

#endif
