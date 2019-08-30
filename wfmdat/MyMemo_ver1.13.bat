@REM Created: 2011/08/01	Modified: 2011/08/05
@REM 
@REM ####　MyMemo.txtの作成&更新バッチ　####
@REM 
@REM MyMemo.txtがないときは新たにファイルを作成、あるときは更新日時を更新する。
@REM ●実行について：
@REM 同ディレクトリ内に、FILENAME変数に指定したファイル(デフォルトはMyMemo.txt)が存在しなければ、このバッチのテストができる。
@REM ●注意：
@REM 作成したMyMemo.txtは"--------"以下にメモ内容を記述すること。

@ECHO OFF
SETLOCAL EnableDelayedExpansion

REM #####　設定
SET FILENAME=MyMemo.txt
SET CREATOR=Koizumi Hayato

SET TMPFILE=MyMemoCreatetmp.dat
REM #####　

set time2=%time: =0%
SET TODAY=%date:~-10,4%/%date:~-5,2%/%date:~-2,2% %time2:~0,2%:%time2:~3,2%:%time2:~6,2%
SET THIS_VERSION_STR=[by %~n0%~x0]
SET THIS_VERSION=%~n0%~x0
SET VERSION_CNT=0

REM #####　バッチ実行するかの確認促し
:CONFIRMATION
SET /p INPUT=0: 実行する	1: 実行せずに終了する --- 
IF /i "%INPUT%" == "0" (goto EXECUTE)
IF /i "%INPUT%" == "1" (goto BAT_END)
IF /i "%INPUT%" == "9" (goto MAKE_PROGRAM_MEMO)
goto CONFIRMATION


:EXECUTE
IF EXIST %FILENAME% (GOTO FILE_TRUE) ELSE GOTO FILE_FALSE


:FILE_TRUE
REM #####　FILENAMEファイルがあるときは、更新日時を更新する

FOR /f "tokens=1* delims=:" %%a in ('FINDSTR /n "^" %FILENAME%') DO (
 REM #####　%%aに行番号、%%bに行の内容が入る。理由は知らない。
 SET STR_CHECK=%%b
 SET STR_CREATE=!STR_CHECK:~0,8!
 SET STR_MODIFY=!STR_CHECK:~30,9!
 SET STR_VERSION=!STR_CHECK:~4,-1!
 SET OUTPUT_LINE=!STR_CHECK!


 REM #####　更新日時の更新
 IF "!STR_CREATE!-!STR_MODIFY!" == "Created:-Modified:" (
  SET OUTPUT_LINE=!STR_CHECK:~0,39! %TODAY%
 ) ELSE (
  REM #####　バージョン情報の更新
  IF "!STR_VERSION!" == "%THIS_VERSION%" (
   SET OUTPUT_LINE=%THIS_VERSION_STR%
   SET /a VERSION_CNT=%VERSION_CNT%+1
  )
 )

 IF "!STR_CHECK!" == "" (
  REM #####　空行出力
  echo.>> %TMPFILE%
 ) ELSE (
  REM #####　1行出力
  echo !OUTPUT_LINE!>> %TMPFILE%
 )
)

REM #####　元ファイルの削除
IF "%VERSION_CNT%" == "1" (
 DEL %FILENAME%
 RENAME %TMPFILE% %FILENAME%
) ELSE (
 ECHO.
 ECHO 恐らくMyMemo.batのバージョンが違います。
 ECHO 同バージョンのバッチを使って。
 ECHO.
 PAUSE
 DEL %TMPFILE%
)
GOTO BAT_END


:FILE_FALSE
REM #####　FILENAMEファイルがない時は、作成する

ECHO Created: %TODAY%		Modified: %TODAY%> %FILENAME%
ECHO Creator: %CREATOR%?>> %FILENAME%
ECHO %THIS_VERSION_STR%>> %FILENAME%
ECHO ----------------------------->> %FILENAME%
ECHO.>> %FILENAME%
GOTO BAT_END


:MAKE_PROGRAM_MEMO
REM #####　プログラム用のメモ作成

IF EXIST %FILENAME% (GOTO FILE_TRUE)

ECHO Created: %TODAY%		Modified: %TODAY%> %FILENAME%
ECHO Creator: %CREATOR%?>> %FILENAME%
ECHO %THIS_VERSION_STR%>> %FILENAME%
ECHO ----------------------------->> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####　使い方　####>> %FILENAME%
ECHO ・>> %FILENAME%
ECHO ー>> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####　????について　####>> %FILENAME%
ECHO.>> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####　注意書き　####>> %FILENAME%
ECHO ●>> %FILENAME%
ECHO.>> %FILENAME%
ECHO ####　デバッグ情報　####>> %FILENAME%
ECHO 20??/??/??>> %FILENAME%
ECHO ・>> %FILENAME%
ECHO.>> %FILENAME%
GOTO BAT_END



:BAT_END
exit


