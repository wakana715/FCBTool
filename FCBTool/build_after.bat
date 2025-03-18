ECHO - build_after.bat start -
ECHO ProjectDir  : %1
ECHO ProjectName : %2
ECHO OutDir      : %3
ECHO COPY "%~1%~2.ps1" %3
     COPY "%~1%~2.ps1" %3
ECHO - build_after.bat end   -
