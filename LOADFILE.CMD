/* REXX */
DrvLtr="";
parse arg DrvLtr;
if DrvLtr="" then do;
say 'Mount PM2DOS diskette in a floppy drive.';
'@pause';
SDrv='A:';
DInfo=SysDriveInfo('A:');
parse value DInfo with DrvLtr DFree DTotal DLabel;
if DInfo='' | DLabel\='PM2DOS' then do; SDrv='B:'; DInfo=SysDriveInfo('B:'); end;
if DInfo='' then do; say 'Floppy not mounted'; exit; end;
parse value DInfo with DrvLtr DFree DTotal DLabel;
if DLabel\='PM2DOS' then do; say 'Floppy not labelled PM2DOS'; exit; end;
say 'Floppy:' DrvLtr DFree DTotal DLabel;
end;
'Replace ' DrvLtr'\PMSW2\*.* E:\CSOURCE\PMSW2 /U'
'Replace ' DrvLtr'\PMSW2\*.* E:\CSOURCE\PMSW2 /A'
