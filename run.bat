@ECHO OFF

TITLE Running the OpenCOR environment...

IF NOT EXIST build\OpenCOR.com GOTO Information
IF NOT EXIST build\OpenCOR.exe GOTO Information

build\OpenCOR %*

GOTO End

:Information

ECHO Information: OpenCOR must first be built before being run.

:End
