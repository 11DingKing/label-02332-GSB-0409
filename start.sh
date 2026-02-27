#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/frontend-app"

REQUIRED_CMAKE_VERSION="3.16"
APP_NAME="游戏掌机 (Game Console)"

echo "============================================"
echo "  $APP_NAME - 一键启动脚本"
echo "  版本管理 + 自动构建"
echo "============================================"
echo ""

command_exists() {
    command -v "$1" >/dev/null 2>&1
}

version_gte() {
    local v1=$(echo "$1" | sed 's/^v//' | sed 's/[^0-9.]//g')
    local v2=$(echo "$2" | sed 's/^v//' | sed 's/[^0-9.]//g')
    if [[ "$(printf '%s\n%s' "$v2" "$v1" | sort -V | head -n1)" == "$v2" ]]; then
        return 0
    else
        return 1
    fi
}

print_ok() { echo "  ✓ $1"; }
print_fail() { echo "  ✗ $1"; }
print_info() { echo "  → $1"; }

# Step 1: 检查 CMake
echo "[1/4] 检测 CMake..."
if command_exists cmake; then
    CMAKE_VER=$(cmake --version | head -n1 | sed 's/cmake version //')
    if version_gte "$CMAKE_VER" "$REQUIRED_CMAKE_VERSION"; then
        print_ok "CMake $CMAKE_VER 已安装"
    else
        print_fail "CMake $CMAKE_VER 版本过低 (需要 >= $REQUIRED_CMAKE_VERSION)"
        if [[ "$OSTYPE" == "darwin"* ]]; then
            brew install cmake
        elif command_exists apt-get; then
            sudo apt-get update && sudo apt-get install -y cmake
        fi
    fi
else
    print_fail "CMake 未安装"
    print_info "正在安装 CMake..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            brew install cmake
        else
            echo "  请先安装 Homebrew: https://brew.sh"
            exit 1
        fi
    elif command_exists apt-get; then
        sudo apt-get update && sudo apt-get install -y cmake
    elif command_exists yum; then
        sudo yum install -y cmake
    else
        echo "  请手动安装 CMake: https://cmake.org/download/"
        exit 1
    fi
    print_ok "CMake 安装完成"
fi

# Step 2: 检查 Qt 6
echo ""
echo "[2/4] 检测 Qt 6..."

QT_FOUND=false

if command_exists qmake6; then
    QT_VER=$(qmake6 --version | tail -n1 | sed 's/.*Qt version //' | awk '{print $1}')
    print_ok "Qt $QT_VER 已安装 (via qmake6)"
    QT_FOUND=true
elif command_exists qmake; then
    QT_VER=$(qmake --version | tail -n1 | sed 's/.*Qt version //' | awk '{print $1}')
    if [[ "$QT_VER" == 6.* ]]; then
        print_ok "Qt $QT_VER 已安装 (via qmake)"
        QT_FOUND=true
    fi
fi

if [ "$QT_FOUND" = false ]; then
    print_fail "Qt 6 未检测到"
    print_info "正在安装 Qt 6..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            brew install qt@6
            echo 'export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"' >> ~/.zshrc
            export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
            export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6"
        fi
    elif command_exists apt-get; then
        sudo apt-get update
        sudo apt-get install -y qt6-base-dev qt6-svg-dev libgl1-mesa-dev
    elif command_exists pacman; then
        sudo pacman -S --noconfirm qt6-base qt6-svg
    else
        echo "  请手动安装 Qt 6: https://www.qt.io/download"
        exit 1
    fi
    print_ok "Qt 6 安装完成"
fi

# Step 3: 构建
echo ""
echo "[3/4] 构建应用..."

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

if [ ! -f "Makefile" ] && [ ! -f "build.ninja" ]; then
    print_info "运行 CMake 配置..."
    if [[ "$OSTYPE" == "darwin"* ]] && [ -d "/opt/homebrew/opt/qt@6" ]; then
        cmake .. -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6"
    else
        cmake ..
    fi
fi

NEEDS_BUILD=true
if [ -f "GameConsole" ] || [ -d "GameConsole.app" ]; then
    SRC_NEWER=$(find ../src -type f -newer GameConsole 2>/dev/null -o -type f -newer GameConsole.app 2>/dev/null | head -1)
    if [ -z "$SRC_NEWER" ]; then
        print_ok "应用已构建且为最新"
        NEEDS_BUILD=false
    fi
fi

if [ "$NEEDS_BUILD" = true ]; then
    print_info "编译中..."
    cmake --build . --parallel
    print_ok "构建完成"
fi

# Step 4: 启动
echo ""
echo "[4/4] 启动应用..."
echo ""

if [[ "$OSTYPE" == "darwin"* ]] && [ -d "GameConsole.app" ]; then
    open GameConsole.app
elif [ -f "GameConsole" ]; then
    ./GameConsole
else
    print_fail "未找到可执行文件"
    exit 1
fi

echo ""
echo "启动成功！"
