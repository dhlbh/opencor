@ECHO OFF

CALL cleanprojall
CALL makeproj %1

TITLE Packaging OpenCOR...

CD build

cpack

CD ..
