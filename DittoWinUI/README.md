# Ditto WinUI 3

A modern clipboard manager for Windows 10/11, built with WinUI 3 and C++/WinRT.

---

## 简体中文说明

### 项目概述

Ditto WinUI 3 是经典剪贴板管理器 Ditto 的完全重写版本，采用现代化的 Windows UI 技术构建。它提供了 Fluent Design System 界面设计，包括 Mica 背景材质、圆角效果和原生 Windows 11 集成。

### 主要特性

- **剪贴板历史**: 自动捕获并保存剪贴板内容
- **快速粘贴窗口**: 通过全局热键 (Ctrl+`) 快速访问
- **分组管理**: 将剪贴内容整理到自定义分组
- **多种格式支持**: 文本、图片、文件、RTF、HTML
- **全局热键**: 可从任何应用程序控制
- **系统托盘**: 后台静默运行
- **WinUI 3 样式**: 现代 Windows 11 Mica 背景和 Fluent Design
- **主题支持**: 浅色/深色模式，与系统同步
- **性能优化**: 虚拟化列表、压缩存储、异步操作

### 系统要求

- Windows 10 版本 1809 (2018年10月更新) 或更高版本
- Windows 11
- Visual Studio 2022 或 2026
- Windows SDK 10.0.22621.0 或更高版本

### 从源码编译

#### 前置条件

1. **Visual Studio 2022/2026** 需安装以下工作负载：
   - "使用 C++ 的桌面开发"
   - "通用 Windows 平台开发"
   - Windows 11 SDK

2. **NuGet 包**（会自动还原）：
   - Microsoft.WindowsAppSDK (1.5+)
   - Microsoft.Windows.CppWinRT
   - SQLite

#### 编译步骤

1. 克隆仓库：
   ```bash
   git clone https://github.com/oboboyes/Ditto.git
   cd Ditto
   ```

2. 在 Visual Studio 中打开解决方案：
   ```
   DittoWinUI/DittoWinUI.sln
   ```

3. 选择配置：
   - 配置: `Debug` 或 `Release`
   - 平台: `x64` 或 `ARM64`

4. 生成解决方案：
   - 菜单: `生成` → `重新生成解决方案` (Ctrl+Shift+B)

5. 运行应用程序：
   - 按 `F5` 调试，或
   - 在 `DittoWinUI/x64/Debug/` 或 `Release/` 目录中找到可执行文件

### 项目结构

```
DittoWinUI/
├── DittoWinUI/
│   ├── App.xaml/h/cpp           # 应用程序入口
│   ├── MainWindow.xaml/h/cpp   # 主应用程序窗口
│   │
│   ├── Models/                 # 数据模型
│   │   ├── ClipItem.h         # 剪贴板项目模型
│   │   └── QueryFilter.h       # 数据库查询过滤器
│   │
│   ├── ViewModels/             # MVVM ViewModel
│   │   ├── ViewModelBase.h    # 基类，包含 INotifyPropertyChanged
│   │   ├── MainViewModel.h    # 主窗口状态
│   │   ├── QuickPasteViewModel.h
│   │   └── SettingsViewModel.h
│   │
│   ├── Views/                 # XAML 视图
│   │   ├── QuickPasteWindow.xaml
│   │   └── SettingsDialog.xaml
│   │
│   ├── Services/              # 业务逻辑
│   │   ├── DatabaseService.h  # SQLite 操作
│   │   ├── ClipboardService.h # 剪贴板监控
│   │   ├── HotKeyService.h   # 全局热键
│   │   ├── SettingsService.h  # 配置管理
│   │   ├── ThemeService.h    # 主题管理
│   │   └── TrayIconService.h # 系统托盘
│   │
│   ├── Interop/              # Win32 互操作
│   │   ├── ClipboardInterop.h
│   │   ├── HotKeyInterop.h
│   │   └── ThemeInterop.h
│   │
│   ├── Tests/                # 单元测试
│   │   └── UnitTests.h
│   │
│   ├── Converters/           # XAML 值转换器
│   │
│   ├── Assets/               # 图片资源
│   └── Package.appxmanifest   # MSIX 清单
│
└── DittoWinUI.sln            # 解决方案文件
```

### 架构设计

#### MVVM 模式

项目遵循 Model-View-ViewModel (MVVM) 架构：

- **Models**: `ClipItem`, `ClipFormat`, `ClipGroup`, `AppSettings`
- **ViewModels**: `MainViewModel`, `QuickPasteViewModel`, `SettingsViewModel`
- **Views**: XAML 文件与数据绑定

#### 服务层

| 服务 | 描述 |
|------|------|
| `DatabaseService` | SQLite CRUD 操作，带优化 |
| `ClipboardService` | 剪贴板监控和操作 |
| `HotKeyService` | 全局热键注册 |
| `SettingsService` | JSON 配置管理 |
| `ThemeService` | 浅色/深色主题管理 |
| `TrayIconService` | 系统托盘集成 |
| `PerformanceService` | 性能监控 |
| `AccessibilityService` | 键盘导航 |

#### 互操作层

Win32 API 调用：
- 剪贴板访问 (`OpenClipboard`, `GetClipboardData`)
- 热键注册 (`RegisterHotKey`)
- 主题效果 (`DwmSetWindowAttribute`)

### 配置说明

配置存储在 `%LOCALAPPDATA%/Ditto/settings.json`：

```json
{
  "QuickPasteHotkey": "Ctrl+`",
  "StartWithWindows": true,
  "MinimizeToTray": true,
  "Theme": 0,
  "MaxClipItems": 1000,
  "MaxStorageMB": 500
}
```

### 键盘快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl+`` ` | 打开快速粘贴窗口 |
| `Enter` | 粘贴选定项目 |
| `Delete` | 删除选定项目 |
| `Escape` | 关闭窗口 |
| `上/下` | 导航项目 |

### 性能特性

- **虚拟化 ListView**: 大列表滚动流畅
- **延迟加载**: 按需加载格式数据
- **WAL 模式**: 更好的并发数据库访问
- **预编译语句**: 查询缓存
- **数据压缩**: 大数据使用 zlib 压缩
- **异步操作**: 所有 I/O 操作异步执行

### 无障碍支持

- 完整的键盘导航
- 屏幕阅读器支持
- 高对比度主题支持
- 焦点管理

### 已知问题

- 需要 Windows 10 1809 或更高版本
- 部分功能需要 Windows 11 (Mica 背景效果)

### 参与贡献

1. Fork 本仓库
2. 创建功能分支
3. 进行修改
4. 提交 Pull Request

### 许可证

本项目是经典 [Ditto](https://github.com/sabrogdon/Ditto) 的 WinUI 3 现代化重写版本。

### 鸣谢

- 原版 Ditto: https://github.com/sabrogdon/Ditto
- WinUI 3: https://learn.microsoft.com/windows/apps/winui/
- C++/WinRT: https://learn.microsoft.com/windows/uwp/cpp-and-winrt-apis/

---

## English Description

### Overview

Ditto WinUI 3 is a complete rewrite of the classic Ditto clipboard manager using modern Windows UI technologies. It provides a Fluent Design System interface with Mica backdrop, rounded corners, and native Windows 11 integration.

### Features

- **Clipboard History**: Automatically capture and store clipboard contents
- **Quick Paste Window**: Fast access via global hotkey (Ctrl+`)
- **Group Management**: Organize clips into custom groups
- **Multiple Formats**: Support for text, images, files, RTF, HTML
- **Global Hotkeys**: Control from any application
- **System Tray**: Runs quietly in the background
- **WinUI 3 Styling**: Modern Windows 11 Mica backdrop and Fluent Design
- **Theme Support**: Light/Dark mode with system synchronization
- **Performance Optimized**: Virtualized lists, compressed storage, async operations

### System Requirements

- Windows 10 version 1809 (October 2018 Update) or later
- Windows 11
- Visual Studio 2022 or 2026
- Windows SDK 10.0.22621.0 or later

### Building from Source

#### Prerequisites

1. **Visual Studio 2022/2026** with:
   - "Desktop development with C++"
   - "Universal Windows Platform development"
   - Windows 11 SDK

2. **NuGet Packages** (will restore automatically):
   - Microsoft.WindowsAppSDK (1.5+)
   - Microsoft.Windows.CppWinRT
   - SQLite

#### Build Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/oboboyes/Ditto.git
   cd Ditto
   ```

2. Open the solution in Visual Studio:
   ```
   DittoWinUI/DittoWinUI.sln
   ```

3. Select configuration:
   - Configuration: `Debug` or `Release`
   - Platform: `x64` or `ARM64`

4. Build the solution:
   - Menu: `Build` → `Build Solution` (Ctrl+Shift+B)

5. Run the application:
   - Press `F5` to debug, or
   - Find the executable in `DittoWinUI/x64/Debug/` or `Release/`

### Project Structure

```
DittoWinUI/
├── DittoWinUI/
│   ├── App.xaml/h/cpp           # Application entry point
│   ├── MainWindow.xaml/h/cpp   # Main application window
│   ├── Models/                 # Data models
│   ├── ViewModels/             # MVVM ViewModels
│   ├── Views/                  # XAML Views
│   ├── Services/               # Business logic
│   ├── Interop/                # Win32 Interop
│   ├── Tests/                  # Unit tests
│   ├── Converters/              # XAML Converters
│   ├── Assets/                 # Images and icons
│   └── Package.appxmanifest     # MSIX manifest
└── DittoWinUI.sln              # Solution file
```

### Architecture

#### MVVM Pattern

The project follows the Model-View-ViewModel (MVVM) architecture:

- **Models**: `ClipItem`, `ClipFormat`, `ClipGroup`, `AppSettings`
- **ViewModels**: `MainViewModel`, `QuickPasteViewModel`, `SettingsViewModel`
- **Views**: XAML files with data bindings

#### Services Layer

| Service | Description |
|---------|-------------|
| `DatabaseService` | SQLite CRUD operations with optimizations |
| `ClipboardService` | Clipboard monitoring and manipulation |
| `HotKeyService` | Global hotkey registration |
| `SettingsService` | JSON-based configuration |
| `ThemeService` | Light/Dark theme management |
| `TrayIconService` | System tray integration |
| `PerformanceService` | Performance monitoring |
| `AccessibilityService` | Keyboard navigation |

#### Interop Layer

Win32 API calls for:
- Clipboard access (`OpenClipboard`, `GetClipboardData`)
- Hotkey registration (`RegisterHotKey`)
- Theme effects (`DwmSetWindowAttribute`)

### Configuration

Settings are stored in `%LOCALAPPDATA%/Ditto/settings.json`:

```json
{
  "QuickPasteHotkey": "Ctrl+`",
  "StartWithWindows": true,
  "MinimizeToTray": true,
  "Theme": 0,
  "MaxClipItems": 1000,
  "MaxStorageMB": 500
}
```

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+`` | Open Quick Paste Window |
| `Enter` | Paste selected item |
| `Delete` | Delete selected item |
| `Escape` | Close window |
| `Up/Down` | Navigate items |

### Performance Features

- **Virtualized ListView**: Smooth scrolling with large clip lists
- **Lazy Loading**: Formats loaded on-demand
- **WAL Mode**: Better concurrent database access
- **Prepared Statements**: Query caching
- **Data Compression**: Large clipboard data compressed with zlib
- **Async Operations**: All I/O operations are asynchronous

### Accessibility

- Full keyboard navigation
- Screen reader support
- High contrast theme support
- Focus management

### Known Issues

- Requires Windows 10 1809 or later
- Some features require Windows 11 (Mica backdrop)

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

### License

This project is a rewrite of [Ditto](https://github.com/sabrogdon/Ditto) with a modern WinUI 3 interface.

### Credits

- Original Ditto: https://github.com/sabrogdon/Ditto
- WinUI 3: https://learn.microsoft.com/windows/apps/winui/
- C++/WinRT: https://learn.microsoft.com/windows/uwp/cpp-and-winrt-apis/
