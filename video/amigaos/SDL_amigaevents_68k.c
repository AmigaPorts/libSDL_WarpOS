/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#include "SDL_amigaevents_c.h"
#include <intuition/intuition.h>
#include <exec/avl.h>
#include <inline/exec_protos.h>
#include <inline/lowlevel_protos.h>

#define JP_ANALOGUE_PORT_MAGIC (1<<16) /* port offset to force analogue readout */
#define MAX_JOYSTICKS 4 /* lowlevel.library is limited to 4 ports */
#define NO_LOWLEVEL_EXT

struct JoyData {
	int data_norm;
#ifndef NO_LOWLEVEL_EXT
	int data_ext;
#endif
};

extern struct Library *LowLevelBase;

/* lowlevel portNumber 0 <-> 1 */
static int PortIndex68k(int index)
{
	switch(index)
	{
		case 0:
			return 1;
			break;

		case 1:
			return 0;
			break;

		default:
			break;
	}

	return index;
}

int GetMessages68k(__reg("a1") struct MsgPort *port, __reg("a0") struct MsgStruct *msg,
			 __reg("a2") struct JoyData *myjoydata, __reg("d0") int maxmsg)
{
 int z, i = 0;
 struct IntuiMessage *imsg;

 if (LowLevelBase)
 { 
   for(z = 0; z < MAX_JOYSTICKS; z++)
   {
   myjoydata[z].data_norm = ReadJoyPort(PortIndex68k(z));
#ifndef NO_LOWLEVEL_EXT
   myjoydata[z].data_ext = ReadJoyPort(PortIndex68k(z) + JP_ANALOGUE_PORT_MAGIC);
#endif
   }
 }

 while ((imsg = (struct IntuiMessage *)GetMsg(port))) {
   if (i < maxmsg) {
     msg[i].Code = imsg->Code;
     msg[i].Class = imsg->Class;
     msg[i].Qualifier = imsg->Qualifier;
     msg[i].MouseX = imsg->MouseX;
     msg[i].MouseY = imsg->MouseY;
     i++;
   }
   ReplyMsg((struct Message *)imsg);
 }
 return i;
}

