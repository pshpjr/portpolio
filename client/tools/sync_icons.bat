@echo off
REM Manual icon import trigger. Forces full manifest reimport and ignores hash cache.
REM Edit UE_EDITOR_CMD below if your install path differs, or set the env var before invoking.

setlocal

if "%UE_EDITOR_CMD%"=="" (
    set "UE_EDITOR_CMD=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
)

set "PROJECT_DIR=%~dp0.."
set "UPROJECT=%PROJECT_DIR%\client.uproject"
set "SCRIPT=%PROJECT_DIR%\Content\Python\import_icons.py"

if not exist "%UE_EDITOR_CMD%" (
    echo [IconPipeline] UnrealEditor-Cmd not found at "%UE_EDITOR_CMD%".
    echo [IconPipeline] Set UE_EDITOR_CMD env var to your UnrealEditor-Cmd.exe path and retry.
    exit /b 1
)

"%UE_EDITOR_CMD%" "%UPROJECT%" -run=pythonscript -script="%SCRIPT% --force" -unattended -nop4 -nosplash

endlocal
