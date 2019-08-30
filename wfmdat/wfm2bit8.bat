@ECHO OFF
@REM #####　パラメータ設定　####
@REM ---- 何点ごとに点を出力するか
SET MANUKI=0
rem 100
@REM ---- 出力ファイルへの出力行数
SET OUTPUT_LINE=500
rem 20000
@REM ---- wfm読込モード　[0: 通常読込、1: 8bit読込]
SET READ_MODE=0

@REM ---- 出力モード　[0: 通常波形、1: 量子化波形]
SET OUTPUT_MODE=0

@REM ---- wfm取得時のサンプリング周波数[GHz]
SET SAMPLING_FREQ=100

cd  %~dp0
gcc -o rw_wfm.exe main.c -O2
for %%i in (.\wfm\*.wfm) do (
 rw_wfm.exe "%%~fi" %MANUKI% %OUTPUT_LINE% %READ_MODE% %OUTPUT_MODE% %SAMPLING_FREQ%
 echo "%%i" finished...
)
pause
del rw_wfm.exe
