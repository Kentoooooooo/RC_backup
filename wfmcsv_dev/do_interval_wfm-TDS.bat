rem @echo off

gcc -O2 -o interval_wfm-TDS.exe interval_wfm-TDS.c -lm
move interval_wfm-TDS.exe data
cd data

del filelist

FOR %%i in (*.wfm) do ( echo %%i >> filelist)
rem pause
interval_wfm-TDS.exe < filelist
rem pause
del interval_wfm-TDS.exe
cd ..\

pause
