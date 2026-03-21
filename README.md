# PC效率工具箱 (PC Productivity Toolbox)

[![Version](https://img.shields.io/badge/version-1.3.0-blue.svg)](https://github.com/your-repo/kitools)
[![Platform](https://img.shields.io/badge/platform-Windows%2010%2F11-lightgrey.svg)](https://www.microsoft.com/windows)
[![Qt](https://img.shields.io/badge/Qt-6.9.0-green.svg)](https://www.qt.io)
[![C++](https://img.shields.io/badge/C%2B%2B-17-orange.svg)](https://isocpp.org/)

一款专为Windows桌面用户设计的模块化效率工具集合。

---

## 功能模块

### 桌面置顶计时器
- 倒计时/番茄钟/正向计时模式
- 外观自定义（窗口大小、颜色、透明度）
- 声音提示、窗口闪烁提醒

### 桌面待办事项
- 任务管理（添加、编辑、删除、完成标记）
- 完成度评分（0-10分自评）
- 日志记录与数据导出（JSON/CSV）
- 深色模式切换

### 桌面便签
- 独立窗口，可拖动到桌面任意位置
- Markdown渲染支持
- 五种预设颜色分类
- 定时提醒（系统通知+窗口闪烁）

### 剪贴板历史管理
- 自动监控剪贴板变化
- 多类型支持（文本、图片、文件路径、网页内容）
- 搜索、收藏、分类筛选
- 数据持久化存储

### 快捷启动器
- 全盘应用索引（开始菜单、桌面、Program Files等）
- 分类筛选（全部/常用/最近/固定/系统/开发/办公/网络/媒体）
- 快速搜索启动
- 使用频率统计与智能排序

### 系统托盘集成
- 最小化到托盘后台运行
- 托盘菜单快速操作

---

## 技术栈

| 层级 | 技术选型 |
|------|---------|
| 开发语言 | C++ 17 |
| GUI框架 | Qt 6.9.0 |
| 构建系统 | CMake 3.16+ |
| 目标平台 | Windows 10/11 |

---

## 项目架构

```
kitools/
├── src/
│   ├── main.cpp                 # 程序入口
│   ├── core/                    # 核心框架
│   │   ├── mainwindow.h/cpp     # 主窗口
│   │   └── globalshortcutmanager.h/cpp  # 全局快捷键
│   └── modules/                 # 功能模块
│       ├── timer/               # 计时器模块
│       ├── todo/                # 待办事项模块
│       ├── notes/               # 便签模块
│       ├── clipboard/           # 剪贴板模块
│       └── launcher/            # 快捷启动器模块
├── docs/                        # 文档目录
└── CMakeLists.txt               # 构建配置
```

### 分层架构

```
┌─────────────────────────────────────────┐
│            表现层 (Presentation)          │
│   SettingsPanel / Widget / ItemWidget    │
├─────────────────────────────────────────┤
│            控制层 (Controller)            │
│   TimerController / TodoController / ... │
├─────────────────────────────────────────┤
│            业务层 (Business)              │
│   Manager / Reminder / Settings          │
├─────────────────────────────────────────┤
│            核心框架 (Core)                │
│        MainWindow / GlobalShortcut       │
└─────────────────────────────────────────┘
```

---

## 版本迭代

| 版本 | 发布日期 | 主要功能 |
|------|---------|---------|
| v1.0.0 | 2025-03-10 | 计时器模块、待办事项模块、系统托盘 |
| v1.1.0 | 2025-03-15 | 桌面便签模块 |
| v1.2.0 | 2025-03-16 | 剪贴板历史管理模块 |
| v1.3.0 | 2025-03-19 | 快捷启动器模块、全局快捷键管理器 |

---

## 未来规划

### 短期规划（v1.4.0）
- 全局快捷键 Alt+Space 快速呼出启动器
- 剪贴板快捷键快速粘贴

### 中期规划（v1.5.0 - v1.6.0）
- 时间追踪器：自动追踪应用使用时间，生成效率报告
- 习惯养成打卡：习惯打卡与追踪
- 专注模式增强：屏蔽干扰网站和应用

### 长期规划（v2.0.0+）
- 项目管理看板：简易看板视图，任务拖拽管理
- 效率数据分析：整合各模块数据，提供效率仪表盘
- 跨平台支持：扩展至 macOS、Linux 平台

---

## 快速开始

### 环境要求
- Windows 10/11 (64位)
- Qt 6.9.0 或更高版本
- CMake 3.16+
- C++17 兼容编译器

### 构建步骤

```bash
git clone https://github.com/your-repo/kitools.git
cd kitools
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

---

## 许可证

本项目采用 MIT 许可证。
