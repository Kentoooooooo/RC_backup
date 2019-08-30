@ECHO OFF
@REM #####�@�p�����[�^�ݒ�@####
@REM ---- ���_���Ƃɓ_���o�͂��邩
SET MANUKI=0
rem 100
@REM ---- �o�̓t�@�C���ւ̏o�͍s��
SET OUTPUT_LINE=10000000
rem 20000
@REM ---- wfm�Ǎ����[�h�@[0: �ʏ�Ǎ��A1: 8bit�Ǎ�]
SET READ_MODE=0

@REM ---- �o�̓��[�h�@[0: �ʏ�g�`�A1: �ʎq���g�`]
SET OUTPUT_MODE=0

@REM ---- wfm�擾���̃T���v�����O���g��[GHz]
SET SAMPLING_FREQ=100


REM #####�@�o�b�`���s���邩�̊m�F����
:CONFIRMATION
ECHO.
ECHO ��doAll.bat���̊e�p�����[�^�ݒ���s�Ȃ��܂������H
ECHO MANUKI --- %MANUKI%
ECHO OUTPUT_LINE --- %OUTPUT_LINE%
ECHO READ_MODE --- %READ_MODE%
ECHO OUTPUT_MODE --- %OUTPUT_MODE%
ECHO SAMPLING_FREQ --- %SAMPLING_FREQ%
ECHO.
SET /p INPUT=0: ���s����	1: ���s�����ɏI������ --- 
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

@REM ---- ���݁B
for %%i in (.\wfm\*.wfm) do (
 @REM rw_wfm.exe "%%~fi" 20 30000
 @REM rw_wfm.exe "%%~fi" 20 15000
 @REM rw_wfm.exe "%%~fi" 25 15000
 @rem rw_wfm.exe "%%~fi" 25 15000
 @rem rw_wfm.exe "%%~fi" 200 15000
 rw_wfm.exe "%%~fi" 0 10 1 1 25
)
