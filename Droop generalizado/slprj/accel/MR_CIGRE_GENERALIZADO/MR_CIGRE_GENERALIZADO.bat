@echo off
set MATLAB=C:\Program Files\MATLAB\R2017b
"%MATLAB%\bin\win64\gmake" -f MR_CIGRE_GENERALIZADO.mk  ISPROTECTINGMODEL=NOTPROTECTING
