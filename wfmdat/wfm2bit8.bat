@ECHO OFF
@REM #####�@�p�����[�^�ݒ�@####
@REM ---- ���_���Ƃɓ_���o�͂��邩
SET MANUKI=0
rem 100
@REM ---- �o�̓t�@�C���ւ̏o�͍s��
SET OUTPUT_LINE=500
rem 20000
@REM ---- wfm�Ǎ����[�h�@[0: �ʏ�Ǎ��A1: 8bit�Ǎ�]
SET READ_MODE=0

@REM ---- �o�̓��[�h�@[0: �ʏ�g�`�A1: �ʎq���g�`]
SET OUTPUT_MODE=0

@REM ---- wfm�擾���̃T���v�����O���g��[GHz]
SET SAMPLING_FREQ=100

cd  %~dp0
gcc -o rw_wfm.exe main.c -O2
for %%i in (.\wfm\*.wfm) do (
 rw_wfm.exe "%%~fi" %MANUKI% %OUTPUT_LINE% %READ_MODE% %OUTPUT_MODE% %SAMPLING_FREQ%
 echo "%%i" finished...
)
pause
del rw_wfm.exe
