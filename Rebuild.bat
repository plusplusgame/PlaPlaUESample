@echo off
setlocal

:: Unreal Engineのインストールディレクトリ
set "UnrealDir=C:\Program Files\Epic Games\UE_5.4"

:: プロジェクトのディレクトリ
set "ProjectDir=%~DP0"

:: プロジェクトのファイル名
set "ProjectName=PlaPlaUESample.uproject"

:: ビルドツールのパス
set "UATScript=%UnrealDir%\Engine\Build\BatchFiles\RunUAT.bat"

:: ビルドコマンドの実行
call "%UATScript%" BuildCookRun -project="%ProjectDir%\%ProjectName%" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -build -editor -editorrecompile -compileeditor -installed

if %ERRORLEVEL% neq 0 (
    echo Error: Build failed
    pause
    exit /b %ERRORLEVEL%
)

echo Build completed successfully
endlocal

:: プロジェクト起動
PlaPlaUESample.uproject

::pause
