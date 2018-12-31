:userdoc.
:docprof.
:title.PMSW2 - Switch Focus to Application by Name
:h1 group=1.Function and Command Line Syntax
:link reftype=hd refid=EIAOS2 auto titlebar=none scroll=none rules=none vpx=0% vpy=0% vpcx=100% vpcy=100%.
PMSW2 is used in REXX programs to switch desktop focus to an
application whose name appears in the DESKTOP switch list.
:p.PMSW2
is called as a REXX function with two arguments:
:ol.
:li.Name mask of task in switch list
:li.Optional "/r" switch to suppress actual focus change
:eol.
:p.If the string passed as the first argument matches a name in
the WorkPlace Shell "Switch List" that is made the focus
with the Ctrl+Esc keys, then that task is given the focus with
a call to WinSwitchToProgram.  The name mask may contain -
usually does contain - wild card characters.  These characters are
the asterisk and question mark, and they have their usual
DOS command line meanings.
:ul.
:li.* wild card matches zero or more characters
:li.? wild card matches exactly one character
:eul.
:xmp.Mask=*ABC*:exmp.
This mask matches a name containing the letters ABC.
:xmp.Mask=*A*B*C*:exmp.
This mask matches a name containing the letters A, B, C, but not
necessarily next to each other.
:h1.Sample Application
:p.PMSW2 can be used to launch a DOS
program from an OS/2 REXX and switch control to it.
The REXX can contain logic such that, if the target DOS
application is already active when the REXX begins,
the DOS application receives the focus instead of the
REXX starting another DOS application window.
This is important to PC LAN administrators who want to
place central applications on the LAN but who need to
provide OS/2 users with the tools to automatically
acquire LAN connection to the application, tailoring the
DOS startup parameters for the target application so that
its environment, PATH, etc. are properly initialized.
:p.At the author's customer's site, the CCMAIL application resides on
the LAN.  A MAIL program folder can be defined on the desktop that
invokes the MAIL.CMD REXX program.  This program tests to see if the
target CCMAIL application is now active, and switches focus to it if so.
If the CCMAIL application is not active, then MAIL.CMD REXX checks to
see if the user is now logged onto the LAN (a LOGON command has been
done), and if the CCMAIL LAN drive has been connected before issuing a
START command for CCMAIL and switching focus to it.
:p.The logic for starting a DOS application:
:cgraphic.
RESULT=PMSW2('*APPNAMESTRING*','/R');
IF RESULT='READY&colon.' THEN DO;
   RESULT=PMSW2('*APPNAMESTRING*');
   IF RESULT='FOCUS&colon.' THEN EXIT;
   SAY 'COULD NOT SWITCH FOCUS'
   EXIT;
END;
'START "APPNAMESTRING" /C /MIN DOSAPP.EXE'
/* OR START A REXX TO DEFINE A DOS OBJECT WITH OPEN=PROGRAM */
BSUCCESS=0;
DO WHILE BSUCCESS<1;
   RESULT=PMSW2('*APPNAMESTRING*','/R');
   IF RESULT='READY&colon.' THEN BSUCCESS=1;
   ELSE CALL SYSSLEEP(2); /* WAIT FOR 2 SECONDS OR SO */
   /* PUT A LIMIT ON CALLS HERE */
END;
:ecgraphic.
:h1 id=TechData.Technical Details
:p.Processing outline:
:ol.
:li.PMSW2 is defined as a Rexx function handler
:li.It examines the input parameters to see if there is a task name
and optional /r flag.
:li.It requests a count of items in the switch list
using WinQuerySwitchList.
:li.It allocates memory to hold the full switch list array.
:li.It requests return of items in the switch list using
WinQuerySwitchList.
:li.It scans the entries in the switch list array and matches the
name mask given against each item.
:li.The MSKCHK function supports use of multiple "*" and "?" wild cards
with the usual:link reftype=fn refid=wild. meanings :elink.
:fn id=wild.The "*" means zero or more characters in this position,
and "?" means exactly one character in this position.:efn.
:li.On finding a name match, if the /r switch was used,
the REXX return string value is set to READY&colon., else
the REXX return string value is set to FOCUS&colon..
:li.If FOCUS&colon., then call WinSwitchToProgram using the
switch list entry handle in the SWCNTRL structure
of the matching switch list entry.
:li.RETURN from PMSW2 to the REXX program that invoked it.
:eol.
:p.Note that return is immediate, regardless of change in focus.
The calling REXX program continues execution while the
user sees the focus change to the target task.
To the person at the keyboard, it appears as though the
REXX program window, if visible, has been replaced by the
target application window, which now has the keyboard focus.
:h1 group=11.Author &amp. Legal Stuff
:h2 group=12 titlebar=yes scroll=vertical rules=none id=Cpyr.Copyright
:hp9.Copyright (C) 1993 by Bruce E. H”gman.:ehp9.:hp8. All Rights Reserved.:ehp8.
PMSW2 may be used by any person, corporation, or government agency
without cost.  PMSW2 may be distributed by any computer bulletin board
service (BBS), including but not limited to Compuserve, BIX, and
Prodigy, provided that no fees are charged for this distribution other
than usual and customary line or session connect charges, or charges
usual and customary for shipping and handling of magnetic media and
documentation.
:h2 group=13 titlebar=yes scroll=vertical rules=none id=Auth.Author
PMSW2 was developed for the US Dept of Energy, Energy Information
Administration (EIA), Forrestal Bldg. MailStop EI-10, 1000 Independence
Ave.  S.W., Washington DC 20585, (202) 586-1965
by Bruce E. H”gman of Electronic Data Systems Corporation (EDS).
The author can be reached via Compuserve 72050,1327 or at the
above mailing address.
:p.The author was an instructor in computer sciences
while on active duty with the Air Force during the
Vietnam years, and has been programming since 1965 on every
type of weird iron, stranger operating systems, and most
popular and arcane programming languages.
He's proudest of having taught US Marines at Quantico Virginia.
:h2 group=14 titlebar=yes scroll=vertical rules=none id=Warn.Warranty
This software is provided without warranty as a public service by the
author.  Neither the author, nor the federal government or EDS
Corporation makes any guarantee as to the usefulness of this software or
its correct operation.
:h1 hide id=EIAOS2.EIA OS/2 Application
:color fc=neutral bc=neutral.
:artwork name='d:\os2\bitmap\eiaos2.bmp' align=center fit.
:euserdoc.
