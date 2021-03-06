/*
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � PMSW2 - REXX Function to switch to a task by name in the task �
  � list.  For example:  to switch to TSO mainframe session that  �
  � is named "A - A - 3270 EMULATOR", provide a mask string as    �
  � a command line entry:  "*3270 EMULATOR*". PMSW2 will switch   �
  � to the first task that matches the mask string.  The chars    �
  � "*" and "?" can be used to indicate "zero or more" and "one"  �
  � any character(s) respectively.                                �
  �                                                               �
  � Example:  Task entry:  "A - A - 3270 Emulator"                �
  � Result=PMSW2("*3270 Emu*");                                   �
  � OR:  "A*3270*EM*" is a good mask for selection.               �
  �                                                               �
  � Example:  Task entry:  "CCMAIL.BAT"                           �
  � Result=PMSW2("*CCMAIL*");                                     �
  �                                                               �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � Copyright (C) 1993 Bruce E. H봥man.  All Rights Reserved.     �
  � This program has been dedicated to the Public Domain.         �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  �                                                               �
  � External functions:  mskchk                                   �
  �   (*mask,mlen,*area,alen,*qmark,*ast)                         �
  � Command line:  pmsw2 "task mask" [/r]                         �
  �   /r:  test only.  Returns:  READY: if task is in list.       �
  �   if no /r, then switch to named task.                        �
  �                                                               �
  � Returns:  string                                              �
  �   ERROR:  error in processing or task not found.  If error    �
  �           caused by system service failure or bad syntax,     �
  �           a message is displayed on STDERR file handle.       �
  �   READY:  Requested task by name is active and jumpable.      �
  �   FOCUS:  Requested task by name was made focus.              �
  �                                                               �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � The OS2H include file code is reproduced here to assist in    �
  � code development.  Be sure to check the latest .H files.      �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
 #ifdef USERDUMMY
/*
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � SWBLOCK switch-list block structure.                          �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
 typedef struct _SWBLOCK {
 ULONG      cswentry;     /* Count of switch list entries  */
 SWENTRY    aswentry[1];  /* Switch list entries  */
  } SWBLOCK;
/*
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � SWENTRY switch-list entry structure.                          �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
 typedef struct _SWENTRY {
 HSWITCH    hswitch;  /* Switch-list entry handle used for focus */
 SWCNTRL    swctl;    /* Switch-list control block structure  */
  } SWENTRY;
/*
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � SWCNTRL switch-list entry structure.                          �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � PMSW2 program entry as RexxFunctionHandler                    �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � On bad command line syntax (no argument(s)), display msg.     �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
 if (Argc<1)
 { ErrorReturn:
   fprintf(stderr,"旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커\n");
   fprintf(stderr,"� PMSW2 REXX Function call syntax:               �\n");
   fprintf(stderr,"�                                                �\n");
   fprintf(stderr,"� Retstr=pmsw2(task_name [,\"/r\"]);               �\n");
   fprintf(stderr,"�                                                �\n");
   fprintf(stderr,"� Returns: FOCUS:/READY:/ERROR:                  �\n");
   fprintf(stderr,"�                                                �\n");
   fprintf(stderr,"� FOCUS:  task named was made focus.             �\n");
   fprintf(stderr,"� READY:  /r suppressed FOCUS:                   �\n");
   fprintf(stderr,"� ERROR:  Error occurred during processing.      �\n");
   fprintf(stderr,"�                                                �\n");
   fprintf(stderr,"� Control returns immediately to caller in all   �\n");
   fprintf(stderr,"� cases.  When FOCUS:, then desktop focus has    �\n");
   fprintf(stderr,"� changed as requested.                          �\n");
   fprintf(stderr,"읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸\n");
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
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � Assemble the contents of the switch list as an array in       �
  � our area.  We'll walk thru list item by item, checking names  �
  � using the name mask.                                          �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
  � If the requested task is found, either set the return code    �
  � and return, or request change of focus to the target window   �
  � and then return with the FOCUS: return string.                �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
