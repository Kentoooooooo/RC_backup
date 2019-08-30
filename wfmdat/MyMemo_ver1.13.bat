@REM Created: 2011/08/01	Modified: 2011/08/05
@REM 
@REM ####�@MyMemo.txt�̍쐬&�X�V�o�b�`�@####
@REM 
@REM MyMemo.txt���Ȃ��Ƃ��͐V���Ƀt�@�C�����쐬�A����Ƃ��͍X�V�������X�V����B
@REM �����s�ɂ��āF
@REM ���f�B���N�g�����ɁAFILENAME�ϐ��Ɏw�肵���t�@�C��(�f�t�H���g��MyMemo.txt)�����݂��Ȃ���΁A���̃o�b�`�̃e�X�g���ł���B
@REM �����ӁF
@REM �쐬����MyMemo.txt��"--------"�ȉ��Ƀ������e���L�q���邱�ƁB

@ECHO OFF
SETLOCAL EnableDelayedExpansion

REM #####�@�ݒ�
SET FILENAME=MyMemo.txt
SET CREATOR=Koizumi Hayato

SET TMPFILE=MyMemoCreatetmp.dat
REM #####�@

set time2=%time: =0%
SET TODAY=%date:~-10,4%/%date:~-5,2%/%date:~-2,2% %time2:~0,2%:%time2:~3,2%:%time2:~6,2%
SET THIS_VERSION_STR=[by %~n0%~x0]
SET THIS_VERSION=%~n0%~x0
SET VERSION_CNT=0

REM #####�@�o�b�`���s���邩�̊m�F����
:CONFIRMATION
SET /p INPUT=0: ���s����	1: ���s�����ɏI������ --- 
IF /i "%INPUT%" == "0" (goto EXECUTE)
IF /i "%INPUT%" == "1" (goto BAT_END)
IF /i "%INPUT%" == "9" (goto MAKE_PROGRAM_MEMO)
goto CONFIRMATION


:EXECUTE
IF EXIST %FILENAME% (GOTO FILE_TRUE) ELSE GOTO FILE_FALSE


:FILE_TRUE
REM #####�@FILENAME�t�@�C��������Ƃ��́A�X�V�������X�V����

FOR /f "tokens=1* delims=:" %%a in ('FINDSTR /n "^" %FILENAME%') DO (
 REM #####�@%%a�ɍs�ԍ��A%%b�ɍs�̓��e������B���R�͒m��Ȃ��B
 SET STR_CHECK=%%b
 SET STR_CREATE=!STR_CHECK:~0,8!
 SET STR_MODIFY=!STR_CHECK:~30,9!
 SET STR_VERSION=!STR_CHECK:~4,-1!
 SET OUTPUT_LINE=!STR_CHECK!


 REM #####�@�X�V�����̍X�V
 IF "!STR_CREATE!-!STR_MODIFY!" == "Created:-Modified:" (
  SET OUTPUT_LINE=!STR_CHECK:~0,39! %TODAY%
 ) ELSE (
  REM #####�@�o�[�W�������̍X�V
  IF "!STR_VERSION!" == "%THIS_VERSION%" (
   SET OUTPUT_LINE=%THIS_VERSION_STR%
   SET /a VERSION_CNT=%VERSION_CNT%+1
  )
 )

 IF "!STR_CHECK!" == "" (
  REM #####�@��s�o��
  echo.>> %TMPFILE%
 ) ELSE (
  REM #####�@1�s�o��
  echo !OUTPUT_LINE!>> %TMPFILE%
 )
)

REM #####�@���t�@�C���̍폜
IF "%VERSION_CNT%" == "1" (
 DEL %FILENAME%
 RENAME %TMPFILE% %FILENAME%
) ELSE (
 ECHO.
 ECHO ���炭MyMemo.bat�̃o�[�W�������Ⴂ�܂��B
 ECHO ���o�[�W�����̃o�b�`���g���āB
 ECHO.
 PAUSE
 DEL %TMPFILE%
)
GOTO BAT_END


:FILE_FALSE
REM #####�@FILENAME�t�@�C�����Ȃ����́A�쐬����

ECHO Created: %TODAY%		Modified: %TODAY%> %FILENAME%
ECHO Creator: %CREATOR%?>> %FILENAME%
ECHO %THIS_VERSION_STR%>> %FILENAME%
ECHO ----------------------------->> %FILENAME%
ECHO.>> %FILENAME%
GOTO BAT_END


:MAKE_PROGRAM_MEMO
REM #####�@�v���O�����p�̃����쐬

IF EXIST %FILENAME% (GOTO FILE_TRUE)

ECHO Created: %TODAY%		Modified: %TODAY%> %FILENAME%
ECHO Creator: %CREATOR%?>> %FILENAME%
ECHO %THIS_VERSION_STR%>> %FILENAME%
ECHO ----------------------------->> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####�@�g�����@####>> %FILENAME%
ECHO �E>> %FILENAME%
ECHO �[>> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####�@????�ɂ��ā@####>> %FILENAME%
ECHO.>> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####�@���ӏ����@####>> %FILENAME%
ECHO ��>> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####�@�f�o�b�O���@####>> %FILENAME%
ECHO 20??/??/??>> %FILENAME%
ECHO �E>> %FILENAME%
ECHO.>> %FILENAME%
GOTO BAT_END



:BAT_END
exit


