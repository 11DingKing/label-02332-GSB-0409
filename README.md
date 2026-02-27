## 1. How to Run

### 一键启动（推荐）

**Linux/macOS:**
```bash
chmod +x start.sh
./start.sh
```

**Windows:**
```bash
start.bat
```

（在项目根目录执行，脚本会自动进入 `frontend-app` 完成构建与启动）

启动脚本会自动检测并安装 CMake、Qt 6 依赖，然后编译并启动应用。

### 手动构建

**前置条件：**
- CMake >= 3.16
- Qt 6（Widgets + Sql 模块）
- C++17 编译器（GCC 9+ / Clang 10+ / MSVC 2019+）

**macOS (Homebrew):**
```bash
brew install qt@6 cmake

cd frontend-app
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6"
cmake --build . --parallel

# 启动
open GameConsole.app
# 或
./GameConsole
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install qt6-base-dev qt6-svg-dev libgl1-mesa-dev cmake g++

cd frontend-app
mkdir build && cd build
cmake ..
cmake --build . --parallel
./GameConsole
```

**Windows (MSVC):**
```bash
cd frontend-app
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
cmake --build . --config Release
Release\GameConsole.exe
```

## 2. Services

| 服务 | 说明 |
|------|------|
| GameConsole | Qt 桌面应用，独立运行，无需网络连接 |
| SQLite | 嵌入式数据库，存储于系统应用数据目录，自动创建 |

## 3. 测试账号

本项目为单机桌面游戏，无登录系统，无需测试账号。

## 4. 题目内容

purefrontend_db_desktop 我需要使用QT开发一个游戏掌机。 
要求如下： 
1. 启动后会给出一个UI，水平布局 
左侧为控制面板：扫雷，贪吃蛇，五子棋，黑白棋，俄罗斯方块，历史记录，右侧为图片按钮，采用网格布局。点击左侧游戏或者右侧的图片都能进入游戏。 
2. 点击具体的游戏后，会有几个按钮，简单 中级 高级 自定义 历史记录 
3. 点击UI界面的历史纪录后，会有几个按钮就是各个游戏的按钮，继续点击则是查看具体游戏的历史记录

## 5. 项目结构

```
label-02332/
├── frontend-app/                 # Qt 桌面应用
│   ├── CMakeLists.txt            # CMake 构建配置
│   ├── src/
│   │   ├── main.cpp              # 应用入口
│   │   ├── mainwindow.h/cpp      # 主窗口（QStackedWidget 管理页面切换）
│   │   ├── gamebase.h/cpp        # 游戏抽象基类
│   │   ├── gameselectwidget.h/cpp # 游戏选择主界面
│   │   ├── difficultyselectwidget.h/cpp # 难度选择界面
│   │   ├── historymanager.h/cpp  # SQLite 历史记录管理器（单例）
│   │   ├── historywidget.h/cpp   # 历史记录查看界面
│   │   ├── minesweepergame.h/cpp # 扫雷游戏
│   │   ├── snakegame.h/cpp       # 贪吃蛇游戏
│   │   ├── gomokugame.h/cpp      # 五子棋游戏
│   │   ├── reversigame.h/cpp     # 黑白棋游戏
│   │   └── tetrisgame.h/cpp      # 俄罗斯方块游戏
│   ├── resources/
│   │   ├── resources.qrc         # Qt 资源文件
│   │   ├── styles/theme.qss      # 赛博朋克主题样式
│   │   └── icons/                # SVG 矢量图标
├── start.sh                      # Linux/macOS 一键启动脚本
├── start.bat                     # Windows 一键启动脚本
├── database-sqlite/
│   └── init.sql                  # SQLite 初始化 + 示例数据脚本
├── docs/
│   └── project_design.md         # 项目设计文档
├── README.md                     # 项目说明（本文件）
└── .gitignore                    # Git 忽略规则
```

## 6. 功能清单

- [x] **主界面** — 水平布局：左侧控制面板（游戏列表 + 历史记录按钮）+ 右侧游戏图标网格
- [x] **难度选择** — 简单 / 中级 / 高级 / 自定义 / 历史记录 五个选项
- [x] **自定义难度** — 弹窗式参数调整（各游戏独立参数）
- [x] **扫雷** — 左键揭开、右键标旗、自动展开空白区域、计时、雷数显示
- [x] **贪吃蛇** — 方向键/WASD 控制、吃食物增长计分、碰壁/碰身结束
- [x] **五子棋** — 双人对战、可变棋盘(13/15/19)、五子连珠判胜、平局检测
- [x] **黑白棋** — 双人对战、8×8 棋盘、8 方向合法判断、批量翻转、跳过无子可下
- [x] **俄罗斯方块** — 7 种标准方块、旋转、消行计分、等级递进加速、幽灵预览、下一个预览
- [x] **历史记录** — SQLite 持久化存储、按游戏分类查看、日期/难度/得分/结果
- [x] **赛博朋克主题** — 深色 UI、霓虹色强调、QSS 全局样式
- [x] **一键启动脚本** — 自动检测/安装 CMake 和 Qt 6、自动编译和启动

## 7. 开发与数据说明

本项目使用 SQLite 嵌入式数据库存储游戏历史记录：
- **数据来源**：首次启动时自动创建数据库并加载 25 条示例数据（每个游戏 5 条）
- **数据库位置**：系统应用数据目录（macOS: `~/Library/Application Support/GameConsole/`，Linux: `~/.local/share/GameConsole/`）
- **数据用途**：开发演示，用户正常游戏后会自动追加真实记录

## 编码说明

本项目所有文件使用 UTF-8 编码，确保中文正常显示：
- 源代码：UTF-8 without BOM
- 数据库：SQLite 默认 UTF-8
- QSS 样式：UTF-8
- 启动脚本：`start.sh` 设置 LANG、`start.bat` 设置 chcp 65001

## 键盘操作

| 游戏 | 操作 |
|------|------|
| 扫雷 | 鼠标左键揭开、右键标旗 |
| 贪吃蛇 | ↑↓←→ 或 WASD 移动、Space 暂停 |
| 五子棋 | 鼠标点击落子 |
| 黑白棋 | 鼠标点击落子 |
| 俄罗斯方块 | ←→ 或 AD 移动、↑ 或 W 旋转、↓ 或 S 加速、Space 硬降、P 暂停 |
