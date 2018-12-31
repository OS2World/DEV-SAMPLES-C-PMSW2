/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ PMSW2 - REXX Function to switch to a task by name in the task ³
  ³ list.  For example:  to switch to TSO mainframe session that  ³
  ³ is named "A - A - 3270 EMULATOR", provide a mask string as    ³
  ³ a command line entry:  "*3270 EMULATOR*". PMSW2 will switch   ³
  ³ to the first task that matches the mask string.  The chars    ³
  ³ "*" and "?" can be used to indicate "zero or more" and "one"  ³
  ³ any character(s) respectively.                                ³
  ³                                                               ³
  ³ Example:  Task entry:  "A - A - 3270 Emulator"                ³
  ³ Result=PMSW2("*3270 Emu*");                                   ³
  ³ OR:  "A*3270*EM*" is a good mask for selection.               ³
  ³                                                               ³
  ³ Example:  Task entry:  "CCMAIL.BAT"                           ³
  ³ Result=PMSW2("*CCMAIL*");                                     ³
  ³                                                               ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ Copyright (C) 1993 Bruce E. H”gman.  All Rights Reserved.     ³
  ³ This program has been dedicated to the Public Domain.         ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³                                                               ³
  ³ External functions:  mskchk                                   ³
  ³   (*mask,mlen,*area,alen,*qmark,*ast)                         ³
  ³ Command line:  pmsw2 "task mask" [/r]                         ³
  ³   /r:  test only.  Returns:  READY: if task is in list.       ³
  ³   if no /r, then switch to named task.                        ³
  ³                                                               ³
  ³ Returns:  string                                              ³
  ³   ERROR:  error in processing or task not found.  If error    ³
  ³           caused by system service failure or bad syntax,     ³
  ³           a message is displayed on STDERR file handle.       ³
  ³   READY:  Requested task by name is active and jumpable.      ³
  ³   FOCUS:  Requested task by name was made focus.              ³
  ³                                                               ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
#define INCL_RXFUNC
#define INCL_DOSMEMMGR
#define INCL_WINSWITCHLIST
#define INCL_PM
#include <stdio.h>
#include <OS2.H>
#include <rexxsaa.h>
#include "pmsw2.h"
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ The OS2H include file code is reproduced here to assist in    ³
  ³ code development.  Be sure to check the latest .H files.      ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 #ifdef USERDUMMY
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ SWBLOCK switch-list block structure.                          ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 typedef struct _SWBLOCK {
 ULONG      cswentry;     /* Count of switch list entries  */
 SWENTRY    aswentry[1];  /* Switch list entries  */
  } SWBLOCK;
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ SWENTRY switch-list entry structure.                          ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 typedef struct _SWENTRY {
 HSWITCH    hswitch;  /* Switch-list entry handle used for focus */
 SWCNTRL    swctl;    /* Switch-list control block structure  */
  } SWENTRY;
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ SWCNTRL switch-list entry structure.                          ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 typedef struct _SWCNTRL {
 HWND        hwnd;                   /* Window handle  */
 HWND        hwndIcon;               /* Window-handle icon  */
 HPROGRAM    hprog;                  /* Program handle  */
 PID         idProcess;              /* Process identity  */
 ULONG       idSession;              /* Session identity  */
 UCHAR       uchVisibility;          /* Visibility  */
 UCHAR       fbJump;                 /* Jump indicator  */
 CHAR        szSwtitle[MAXNAMEL+1];  /* Switch-list control block title (null-terminated)  */
 BYTE        bProgType;              /* Program type  */
  } SWCNTRL;
 #endif
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ PMSW2 program entry as RexxFunctionHandler                    ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 RexxFunctionHandler PMSW2;
 ULONG PMSW2(
     PUCHAR    Name,                   /* name of the function       */
     ULONG     Argc,                   /* number of arguments        */
     RXSTRING  Argv[],                 /* list of argument strings   */
     PSZ       Queuename,              /* current queue name         */
     PRXSTRING Retstr)                 /* returned result string     */
 {
 BOOL      bFound=FALSE;    /* TRUE if requested task name found */
 BOOL      bQuery=FALSE;    /* if /r flag is used to return info */
 HAB       hAnchorBlock;    /* Anchor-block handle */
 HSWITCH   hswitchSwHandle; /* Window List entry handle of program to be activated */
 int       i;
 int       iMaskLen=0;
 int       iRCmskchk=0;
 UCHAR     pAster[]="*";
 ULONG     pBase;
 UCHAR     pQmark[]="?";
 SWCNTRL  *pSWCItem;
 SWENTRY  *pSWEItem;
 PSWBLOCK  pswblkBlock;     /* Switch entries block */
 PSWBLOCK  pswblkItem;      /* ptr work to an individual SWCNTRL */
 UCHAR     scInputMask[64]="";
 char      scTestString[80]="";
 CHAR      szUCtitle[MAXNAMEL+1]="";
 ULONG     ulNrItems;
 ULONG     ulRetCode=0;     /* Return code *from Win* functions */
 ULONG     ulcbBufLength;

/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ On bad command line syntax (no argument(s)), display msg.     ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 if (Argc<1)
 { ErrorReturn:
   fprintf(stderr,"ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
   fprintf(stderr,"³ PMSW2 REXX Function call syntax:               ³\n");
   fprintf(stderr,"³                                                ³\n");
   fprintf(stderr,"³ Retstr=pmsw2(task_name [,\"/r\"]);               ³\n");
   fprintf(stderr,"³                                                ³\n");
   fprintf(stderr,"³ Returns: FOCUS:/READY:/ERROR:                  ³\n");
   fprintf(stderr,"³                                                ³\n");
   fprintf(stderr,"³ FOCUS:  task named was made focus.             ³\n");
   fprintf(stderr,"³ READY:  /r suppressed FOCUS:                   ³\n");
   fprintf(stderr,"³ ERROR:  Error occurred during processing.      ³\n");
   fprintf(stderr,"³                                                ³\n");
   fprintf(stderr,"³ Control returns immediately to caller in all   ³\n");
   fprintf(stderr,"³ cases.  When FOCUS:, then desktop focus has    ³\n");
   fprintf(stderr,"³ changed as requested.                          ³\n");
   fprintf(stderr,"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   Retstr->strlength=6;
   Retstr->strptr="ERROR:";  return -1; /* cause REXX message too */
 }
 strcpy(scInputMask,Argv[0].strptr); strupr(scInputMask);
 iMaskLen=strlen(scInputMask);
 if (Argc>1)
 { if ((Argv[1].strptr[0]='/')
   && ((Argv[1].strptr[1]='r')| (Argv[1].strptr[1]='R')))
     bQuery=TRUE;
 }
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ Assemble the contents of the switch list as an array in       ³
  ³ our area.  We'll walk thru list item by item, checking names  ³
  ³ using the name mask.                                          ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 ulNrItems = WinQuerySwitchList(hAnchorBlock, NULL, 0);
 ulcbBufLength = (ulNrItems * sizeof(SWENTRY)) + sizeof(ULONG);
 pswblkBlock=(SWBLOCK *)malloc(ulcbBufLength);
 /* gets struct. array */
 WinQuerySwitchList( hAnchorBlock, pswblkBlock, ulcbBufLength);

 pSWEItem=(SWENTRY *)&(pswblkBlock->aswentry[0]);
 for (i=0;i<ulNrItems;i++)
 { pSWCItem=(SWCNTRL *)&((pSWEItem+i)->swctl);
   strcpy(scTestString,pSWCItem->szSwtitle);
   if (pSWCItem->fbJump&SWL_JUMPABLE)
   { strcpy(szUCtitle,pSWCItem->szSwtitle); strupr(szUCtitle);
     iRCmskchk=mskchk( scInputMask,iMaskLen, szUCtitle,
       strlen(pSWCItem->szSwtitle), pQmark, pAster);
     if (!iRCmskchk)
     { bFound=TRUE; hswitchSwHandle=(pSWEItem+i)->hswitch; }
   }
   if (bFound) break;
 }
/*
  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³ If the requested task is found, either set the return code    ³
  ³ and return, or request change of focus to the target window   ³
  ³ and then return with the FOCUS: return string.                ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
 Retstr->strlength=6; /* all return string values are 6 chars */
 if (!bQuery)         /* request is to change focus */
 { ulRetCode = WinSwitchToProgram( hswitchSwHandle);
   if (0<ulRetCode)   /* return ERROR: on failure to switch */
   { Retstr->strptr="ERROR:";
   /*fprintf(stderr,"PMSW2 WinSwitchToProgram RC=%lu\n",ulRetCode);*/
   }
   else Retstr->strptr="FOCUS:"; /* FOCUS: focus has changed */
 }
 else                 /* request is to check on readiness for focus */
 { if (bFound) Retstr->strptr="READY:";
   else Retstr->strptr="ERROR:";
 }
 free(pswblkBlock);
 return 0;
 }
