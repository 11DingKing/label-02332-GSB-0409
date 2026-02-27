@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion
cd /d "%~dp0"
cd frontend-app

set APP_NAME=游戏掌机 (Game Console)

echo ============================================
echo   %APP_NAME% - 一键启动脚本
echo   版本管理 + 自动构建
echo ============================================
echo.

:: Step 1: 检查 CMake
echo [1/4] 检测 CMake...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo   ✗ CMake 未安装
    echo   → 正在打开 CMake 下载页面...
    start https://cmake.org/download/
    echo   请安装 CMake 后重新运行此脚本
    pause
    exit /b 1
)
for /f "tokens=3" %%v in ('cmake --version 2^>^&1 ^| findstr /i "cmake version"') do (
    echo   ✓ CMake %%v 已安装
)

:: Step 2: 检查 Qt 6
echo.
echo [2/4] 检测 Qt 6...
where qmake6 >nul 2>&1
if %errorlevel% equ 0 (
    echo   ✓ Qt 6 已安装
    goto :build
)
where qmake >nul 2>&1
if %errorlevel% equ 0 (
    echo   ✓ Qt 已安装
    goto :build
)

echo   ✗ Qt 6 未检测到
echo   请从 https://www.qt.io/download 下载并安装 Qt 6
echo   安装时确保勾选 "Qt 6.x" 和 "CMake" 组件
echo   安装完成后将 Qt 的 bin 目录添加到 PATH 环境变量
start https://www.qt.io/download
pause
exit /b 1

:: Step 3: 构建
:build
echo.
echo [3/4] 构建应用...

if not exist "build" mkdir build
cd build

if not exist "Makefile" if not exist "build.ninja" (
    echo   → 运行 CMake 配置...
    cmake .. -G "MinGW Makefiles"
    if %errorlevel% neq 0 (
        cmake ..
    )
)

if not exist "GameConsole.exe" (
    echo   → 编译中...
    cmake --build . --parallel
    if %errorlevel% neq 0 (
        echo   ❌ 构建失败
        pause
        exit /b 1
    )
    echo   ✓ 构建完成
) else (
    echo   ✓ 应用已构建
)

:: Step 4: 启动
echo.
echo [4/4] 启动应用...
echo.

if exist "GameConsole.exe" (
    start "" "GameConsole.exe"
    echo   启动成功！
) else (
    echo   ❌ 未找到可执行文件
    pause
    exit /b 1
)

timeout /t 3 >nul
exit /b 0
