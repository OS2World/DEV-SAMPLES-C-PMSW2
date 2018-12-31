/* REXX */
if 0 < RxFuncQuery('SysLoadFuncs') THEN DO;
   Call rxfuncadd 'SysLoadFuncs','REXXUTIL','SysLoadFuncs';
   Call SysLoadFuncs;
END;
parse arg PgmName
say 'Arguments:' PgmName
lq=0; BQ=0;
lq=pos('/r',PgmName); BQ=pos('/R',PgmName);
Count='';
if (lq>0|BQ>0) then do;
   PgmName=substr(PgmName,1,(lq+BQ)-1);
   Count='/r';
   say '/r: args='PgmName;
end;
else say 'args='PgmName;
RC=RXFUNCDROP('PMSW2');
say 'RxFuncDrop RC='RC;
RC=RXFUNCADD('PMSW2','PMSW2','PMSW2');
say 'RxFuncAdd  RC='RC;
if (lq>0|BQ>0) then  do;
   say 'Call with' PgmName ',/R'
   Result=PMSW2(PgmName,Count);
end;
else do;
   say 'Call with' PgmName;
   Result=PMSW2(PgmName);
end;
say 'PMSW2 returned' Result;
RC=RXFUNCDROP('PMSW2');
say 'RxFuncDrop RC='RC;
