/* Amiga-specific additions and replacements for PSPZX81/sz81
 * by Chris Young <chris@unsatisfactorysoftware.co.uk> 2008, 2010
 */

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "amiga.h"
#include "common.h"
#include "sdl.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/asl.h>

struct Library *IconBase;
struct Library *AslBase;
struct AslIFace *IAsl;

extern int unexpanded, autolist, load_hook, save_hook;
extern char *zxpfilename;
static char USED ver[] = "\0$VER:sz81 " VERSION " (" DATE ")\0";

extern void autoload_setup(char *filename);

int amiga_open_libs(void)
{
	int ret=1;

	if((IconBase = IExec->OpenLibrary("icon.library",50)))
	{
		IIcon=(struct IconIFace *)IExec->GetInterface(IconBase,"main",1,NULL);
	}
	else
	{
		ret=0;
	}

	if((AslBase = IExec->OpenLibrary("asl.library",50)))
	{
		IAsl=(struct AslIFace *)IExec->GetInterface(AslBase,"main",1,NULL);
	}
	else
	{
		ret=0;
	}

	strcpy(amiga_resource_file, "PROGDIR:.sz81rc");

	return(ret);
}

void amiga_close_libs(void)
{
	if(IconBase) 
	{
		IExec->DropInterface((struct Interface *)IIcon);
		IExec->CloseLibrary(IconBase);
	}

	if(AslBase) 
	{
		IExec->DropInterface((struct Interface *)IAsl);
		IExec->CloseLibrary(AslBase);
	}
}

void amiga_read_tooltypes(struct WBStartup *WBenchMsg)
{
	struct WBArg *wbarg;
	char i;
	int olddir;
	struct DiskObject *dobj;
	STRPTR *toolarray;
	char *s;

	for(i=0,wbarg=WBenchMsg->sm_ArgList;i<WBenchMsg->sm_NumArgs;i++,wbarg++)
	{
		olddir =-1;
		if((wbarg->wa_Lock)&&(*wbarg->wa_Name))
			olddir = IDOS->CurrentDir(wbarg->wa_Lock);

		if((*wbarg->wa_Name) && (dobj=IIcon->GetIconTags(wbarg->wa_Name,NULL)))
		{
			toolarray = dobj->do_ToolTypes;

			if((s = IIcon->FindToolType(toolarray,"ZX80")))
			{
			 	zx80=1;
			}

			if((s = IIcon->FindToolType(toolarray,"ZXPRINTER")))
			{
			 	zxpfilename = strdup(s);
			}

			if((s = IIcon->FindToolType(toolarray,"INVERT")))
			{
				invert_screen=1;
			}

			if((s = IIcon->FindToolType(toolarray,"AUTOLIST")))
			{
			 	autolist=1;
			}

			if((s = IIcon->FindToolType(toolarray,"NOLOADHOOK")))
			{
			 	load_hook=0;
			}

			if((s = IIcon->FindToolType(toolarray,"NOSAVEHOOK")))
			{
			 	save_hook=0;
			}

			if((s = IIcon->FindToolType(toolarray,"REFRESH")))
			{
			 	scrn_freq = atoi(s);
				if(scrn_freq<1) scrn_freq=1;
				if(scrn_freq>50) scrn_freq=50;
			}

			if((s = IIcon->FindToolType(toolarray,"TAGULA")))
			{
			 	taguladisp=1;
			}

			if((s = IIcon->FindToolType(toolarray,"UNEXPANDED")))
			{
			 	unexpanded=1;
			}

			if((s = IIcon->FindToolType(toolarray,"VSYNC")))
			{
			 	vsync_visuals=1;
			}

			if((s = IIcon->FindToolType(toolarray,"SHOWDEVIDS")))
			{
				sdl_cl_show_input_id=1;
			}

			if((s = IIcon->FindToolType(toolarray,"RESOURCEFILE")))
			{
			 	strcpy(amiga_resource_file, s);
			}

			IIcon->FreeDiskObject(dobj);
		}

		if(olddir !=-1) IDOS->CurrentDir(olddir);
	}

	if(WBenchMsg->sm_NumArgs > 1)
	{
		wbarg--;
		autoload_setup(wbarg->wa_Name);
	}
	else
	{
		if(autolist) autoload_setup(NULL);
	}
}

char *amiga_file_request(char *path)
{
	int sel=0;
	struct FileRequester *freq;
	char pattern[20] = "#?.(p|81)";
	static char fname[1024];

	if(zx80) strcpy(pattern,"#?.(o|80)");

	if((freq = (struct FileRequester *)IAsl->AllocAslRequest(ASL_FileRequest,NULL)))
	{
		if(IAsl->AslRequestTags(freq,
								ASLFR_TitleText,"SZ81",
                             	ASLFR_InitialPattern,&pattern,
                             	ASLFR_InitialDrawer,path,
                             	ASLFR_DoPatterns,TRUE,
                    			TAG_DONE))
		{
			strlcpy(fname,freq->fr_Drawer,1024);	
			IDOS->AddPart(fname,freq->fr_File,1024);
			sel=1;
		}
		IAsl->FreeAslRequest(freq);
	}
	return fname;
}
