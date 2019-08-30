@ECHO OFF
@REM #####　パラメータ設定　####
@REM ---- 何点ごとに点を出力するか
SET MANUKI=0
rem 100
@REM ---- 出力ファイルへの出力行数
SET OUTPUT_LINE=10000000
rem 20000
@REM ---- wfm読込モード　[0: 通常読込、1: 8bit読込]
SET READ_MODE=0

@REM ---- 出力モード　[0: 通常波形、1: 量子化波形]
SET OUTPUT_MODE=0

@REM ---- wfm取得時のサンプリング周波数[GHz]
SET SAMPLING_FREQ=100


REM #####　バッチ実行するかの確認促し
:CONFIRMATION
ECHO.
ECHO ●doAll.bat内の各パラメータ設定を行ないましたか？
ECHO MANUKI --- %MANUKI%
ECHO OUTPUT_LINE --- %OUTPUT_LINE%
ECHO READ_MODE --- %READ_MODE%
ECHO OUTPUT_MODE --- %OUTPUT_MODE%
ECHO SAMPLING_FREQ --- %SAMPLING_FREQ%
ECHO.
SET /p INPUT=0: 実行する	1: 実行せずに終了する --- 
IF /i "%INPUT%" == "0" (goto EXECUTE)
IF /i "%INPUT%" == "1" (goto BAT_END)
REM goto CONFIRMATION
IF /i "%INPUT%" == "2" (
 gcc -o rw_wfm.exe main.c -O2
 del rw_wfm.exe
)
goto CONFIRMATION


:EXECUTE
cd  %~dp0
gcc -o rw_wfm.exe main.c -O2
gcc -o histgram.exe histgram.c -O2
for %%i in (.\wfm\*.wfm) do (
 rw_wfm.exe "%%~fi" %MANUKI% %OUTPUT_LINE% %READ_MODE% %OUTPUT_MODE% %SAMPLING_FREQ%
 echo wfmtoint "%%i" finished...
)
for %%i in (.\wfm\*.txt) do (
 histgram.exe %%i
 echo histgram "%%i" finished...
)
pause
del rw_wfm.exe



:BAT_END
exit

@REM ---- ごみ。
for %%i in (.\wfm\*.wfm) do (
 @REM rw_wfm.exe "%%~fi" 20 30000
 @REM rw_wfm.exe "%%~fi" 20 15000
 @REM rw_wfm.exe "%%~fi" 25 15000
 @rem rw_wfm.exe "%%~fi" 25 15000
 @rem rw_wfm.exe "%%~fi" 200 15000
 rw_wfm.exe "%%~fi" 0 10 1 1 25
)
