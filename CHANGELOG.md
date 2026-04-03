# 更新日志

## [v1.4.0] - 2026-04-03

### 新增功能
- **时间追踪器模块**
  - Windows API窗口活动监控（GetForegroundWindow等）
  - 应用使用时间自动记录和统计
  - 智能应用分类系统（开发/浏览器/办公/通讯/媒体/游戏/系统/其他）
  - 浏览器标签页智能分类（工作/学习/社交/视频/购物/新闻）
  - 自定义分类规则管理（支持编辑默认规则）
  - 今日/历史数据查看和统计分析
  - 每日目标设定与进度跟踪

- **开机自启动功能**
  - 全局应用自启动开关（主界面首页）
  - Windows注册表自启动管理（HKEY_CURRENT_USER）
  - 剪贴板模块独立自启动选项
  - 时间追踪器模块独立自启动选项
  - 配置持久化保存（QSettings）
  - 用户选择状态跨会话保持

- **AutoStartManager核心组件**
  - 单例模式注册表管理器
  - 跨平台预留接口（Q_OS_WIN宏控制）
  - 完善的错误处理和日志输出
  - 状态变更信号通知机制

### 改进
- **游戏识别优化**：扩展游戏分类规则库
  - 新增：三角洲(Delta Force)、原神、永劫无间、PUBG、Apex等热门游戏
  - 新增：Steam、Epic、WeGame等游戏平台支持
- **浏览器分类增强**：
  - 窗口标题自动解析（域名+页面标题提取）
  - URL匹配规则和关键词匹配双重机制
  - 6大浏览器子分类覆盖主流网站
- **规则编辑功能完善**：
  - 支持编辑预设/默认分类规则
  - 多行文本输入批量添加进程名
  - 规则名称重复检测与确认提示

### 技术改进
- SettingsManager扩展：新增6个自启动配置接口
- MainWindow增强：initializeAutoStartModules()自动初始化
- ClipboardController新增startMonitoring()方法
- CMakeLists.txt更新：添加autostartmanager源文件
- Windows API集成：psapi库链接配置

### 打包与发布
- **自动化打包脚本**
  - PowerShell打包脚本（package_simple.ps1）
  - 自动查找Release构建产物
  - windeployqt自动部署Qt依赖库
  - 手动补充关键DLL（Qt6Charts等）
  - MinGW运行时库完整部署
  - 依赖完整性验证机制

- **NSIS安装程序**
  - Modern UI 2界面（简体中文）
  - 完整安装向导（欢迎/许可/目录/进度/完成）
  - 桌面和开始菜单快捷方式创建
  - 控制面板注册（程序和功能）
  - 标准卸载流程支持
  - 文件版本信息嵌入

- **发布包内容**
  - ZIP便携版（16.07 MB）：解压即用
  - NSIS安装版（16.13 MB）：标准安装体验
  - 完整Qt运行时：Qt6Core/Gui/Widgets/Network/Svg/Charts等
  - Qt插件：platforms/imageformats/styles/tls等
  - MinGW运行时：libgcc_s_seh-1/libstdc++-6/libwinpthread-1
  - 项目文档：README/CHANGELOG/LICENSE/docs目录

### 文件变更清单
**新建文件 (2个)**：
- `src/core/autostartmanager.h` - 注册表管理器头文件
- `src/core/autostartmanager.cpp` - Windows API实现

**修改文件 (9个)**：
- `src/modules/timer/settingsmanager.h/cpp` - +6个方法
- `src/core/mainwindow.h/cpp` - +80行UI和逻辑
- `src/modules/clipboard/clipboardcontroller.h/cpp` - +8行
- `src/modules/clipboard/clipboardsettingspanel.cpp` - +7行
- `src/modules/timetracker/timetrackersettingspanel.cpp` - +10行
- `CMakeLists.txt` - +3行源文件

---

## [v1.3.0] - 2025-03-19

### 新增功能
- **快捷启动器模块**
  - 全盘应用索引：自动扫描开始菜单、桌面、Program Files等目录
  - 分类筛选：按类型查看（全部/常用/最近/固定/系统/开发/办公/网络/媒体/其他）
  - 快速搜索：按名称和关键词搜索应用
  - 使用统计：记录启动次数，智能排序
  - 固定功能：固定常用应用到顶部
  - 多类型支持：支持exe、lnk、url、msi、bat等文件类型
  - 数据持久化：自动保存索引到本地JSON文件

- **全局快捷键管理器**
  - Windows原生API实现全局热键注册
  - 支持注册、注销、更新快捷键

### 改进
- 扩展索引路径：新增D/E盘Program Files、WindowsApps、用户本地应用目录
- 优化扫描性能：添加深度限制和目录过滤
- 智能分类：根据应用名称和路径自动匹配分类

### 界面优化
- 快捷启动器界面全部中文化
- 列表项显示启动次数和固定状态
- 工具提示显示完整路径和统计信息

---

## [v1.2.0] - 2025-03-16

### 新增功能
- **剪贴板历史管理模块**
  - 自动监控剪贴板变化，实时记录复制内容
  - 支持多种内容类型：文本、图片、文件路径、网页内容
  - 搜索功能：快速查找历史记录
  - 收藏功能：标记常用内容
  - 分类筛选：按类型查看（全部/文本/图片/文件/收藏）
  - 查看详情：查看完整内容，支持超长文本
  - 数据持久化：自动保存到本地JSON文件
  - 右键菜单：复制、收藏、删除、查看详情

### 改进
- 网页复制内容自动提取纯文本，避免HTML源码显示
- 预览长度增加到200字符
- 清空历史后允许重新添加之前复制过的内容

### 界面优化
- 剪贴板模块界面全部中文化
- 类型标签颜色区分（文本-蓝色、图片-紫色、文件-绿色、HTML-橙色）

---

## [v1.1.0] - 2025-03-15

### 新增功能
- **桌面便签模块**
  - 创建、编辑、删除便签
  - 便签置顶功能
  - 便签提醒功能
  - 便签分类管理

### 改进
- 优化主界面布局
- 添加模块图标

---

## [v1.0.0] - 2025-03-10

### 新增功能
- **计时器模块**
  - 正计时/倒计时
  - 桌面悬浮窗
  - 计时结束提醒

- **待办事项模块**
  - 任务管理（添加、编辑、删除）
  - 任务状态切换
  - 任务日志记录
  - 桌面悬浮显示

- **基础架构**
  - Qt 6.x 框架
  - 模块化设计
  - 系统托盘支持
