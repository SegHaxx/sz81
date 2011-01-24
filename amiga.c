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
struct IconIFace *IIcon;

struct Library *AslBase;
struct AslIFace *IAsl;

static char USED ver[] = "\0$VER:sz81 " VERSION " (" DATE ")\0";

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

	strcpy(amiga_data_dir, "PROGDIR:save");

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

			if((s = IIcon->FindToolType(toolarray,"FULLSCREEN")))
			{
				sdl_com_line.fullscreen=1;
			}

			if((s = IIcon->FindToolType(toolarray,"XRES")))
			{
				sdl_com_line.xres=atoi(s);
			}

			if((s = IIcon->FindToolType(toolarray,"YRES")))
			{
				sdl_com_line.yres=atoi(s);
			}

			if((s = IIcon->FindToolType(toolarray,"LOCALDATA")))
			{
			 	strcpy(amiga_data_dir, s);
			}

			IIcon->FreeDiskObject(dobj);
		}

		if(olddir !=-1) IDOS->CurrentDir(olddir);
	}

	if(WBenchMsg->sm_NumArgs > 1)
	{
		wbarg--;
		strcpy(sdl_com_line.filename, wbarg->wa_Name);
	}
}

char *amiga_file_request(char *title, BOOL save)
{
	int sel=0;
	struct FileRequester *freq;
	char pattern[20] = "#?.(p|81)";
	static char fname[1024];

	if(zx80) strcpy(pattern,"#?.(o|80)");

	if((freq = (struct FileRequester *)IAsl->AllocAslRequest(ASL_FileRequest,NULL)))
	{
		if(IAsl->AslRequestTags(freq,
								ASLFR_TitleText, title,
                             	ASLFR_InitialPattern,&pattern,
                          //   	ASLFR_InitialDrawer,path,
                             	ASLFR_DoPatterns,TRUE,
								ASLFR_DoSaveMode, save,
                    			TAG_DONE))
		{
			strlcpy(fname,freq->fr_Drawer,1024);	
			IDOS->AddPart(fname,freq->fr_File,1024);
			sel=1;
		}
		IAsl->FreeAslRequest(freq);
	}

	if(sel) return fname;
		else return NULL;
}
