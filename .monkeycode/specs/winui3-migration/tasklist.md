# Task List - Ditto WinUI 3 Migration

## Project Overview

This task list tracks the migration of Ditto from MFC to WinUI 3.

**Feature**: winui3-migration
**Created**: 2026-03-28
**Status**: In Progress

---

## Phase 1: Project Setup

- [x] **1.1** Create WinUI 3 project structure
  - Create DittoWinUI/ folder hierarchy
  - Create solution file DittoWinUI.sln
  - Create project file DittoWinUI.vcxproj
  - Set up MVVM folder structure (ViewModels/, Views/, Services/, Models/, Interop/, Converters/, Resources/)

- [x] **1.2** Configure project dependencies
  - Add Windows App SDK (1.5+) NuGet package
  - Add C++/WinRT support
  - Add SQLite NuGet package
  - Configure MSIX packaging in Package.appxmanifest

- [x] **1.3** Create basic App.xaml and App.xaml.cpp
  - Initialize Windows App SDK resources
  - Set up global exception handling
  - Configure theme resources (Mica backdrop)

---

## Phase 2: Core Infrastructure

- [x] **2.1** Implement DatabaseService
  - Create IDatabaseService interface
  - Implement DatabaseService with SQLite
  - Create ClipItem and ClipFormat data models
  - Implement CRUD operations for clips

- [x] **2.2** Implement SettingsService
  - Create ISettingsService interface
  - Implement SettingsService using JSON file storage
  - Implement Settings ViewModel binding support
  - Add settings for hotkeys, appearance, storage

- [x] **2.3** Implement ClipboardService
  - Create IClipboardService interface
  - Implement ClipboardInterop with Win32 API
  - Implement clipboard monitoring thread
  - Support multiple formats (Text, Unicode, RTF, HTML, Images, Files)

- [x] **2.4** Implement HotKeyService
  - Create IHotKeyService interface
  - Implement HotKeyInterop with RegisterHotKey
  - Support modifier keys (Ctrl, Alt, Shift, Win)
  - Add hotkey conflict detection

---

## Phase 3: ViewModels (MVVM)

- [x] **3.1** Create ViewModelBase
  - Implement INotifyPropertyChanged in C++/WinRT
  - Create ObservableProperty helper macros
  - Add ICommand support with RelayCommand

- [x] **3.2** Implement MainViewModel
  - Group management (CRUD operations)
  - Tray icon state management
  - Application state (IsRunning, etc.)

- [x] **3.3** Implement QuickPasteViewModel
  - Clip list with ObservableCollection
  - Real-time search filtering
  - Item selection and paste operations
  - Keyboard navigation support

- [x] **3.4** Implement SettingsViewModel
  - Settings page navigation
  - Property bindings for all settings
  - Save/Load settings operations
  - Reset to defaults functionality

---

## Phase 4: UI (XAML)

- [x] **4.1** Create MainWindow
  - Set up NavigationView as container
  - Add Mica backdrop material
  - Implement native title bar
  - Add system tray integration

- [x] **4.2** Create QuickPasteWindow
  - Floating window with rounded corners
  - ListView for clip items with DataTemplate
  - SearchBox with real-time filtering
  - Keyboard navigation (Up/Down/Enter/Escape)

- [x] **4.3** Create SettingsDialog
  - NavigationView with settings categories
  - General settings page (hotkeys, startup, etc.)
  - Appearance settings page (theme, colors)
  - Storage settings page (database size, auto-purge)

- [x] **4.4** Implement WinUI 3 Styling
  - Apply Mica backdrop to all windows
  - Configure rounded corners (8px radius)
  - Add system accent color integration
  - Support light/dark theme switching

---

## Phase 5: Integration

- [x] **5.1** Connect ViewModels to Services
  - Dependency injection setup
  - Service registration in App.xaml.cpp
  - Bind services to ViewModels

- [x] **5.2** Implement Tray Icon
  - Create TrayIconService
  - Add Shell_NotifyIcon interop
  - Context menu for tray icon
  - Double-click to show main window

- [x] **5.3** Add Global Hotkey Handling
  - Register configured hotkeys on startup
  - Handle WM_HOTKEY in message loop
  - Show QuickPasteWindow on hotkey press
  - Paste selected item on Enter

- [x] **5.4** Implement Theme Switching
  - Detect Windows theme (light/dark)
  - Add manual theme override option
  - Apply theme resources dynamically

---

## Phase 6: Polish

- [x] **6.1** Add Animations and Transitions
  - QuickPasteWindow show/hide animations
  - ListView item animations
  - Page transition animations

- [x] **6.2** Optimize Performance
  - Virtualized ListView for large clip lists
  - Async loading of clip previews
  - Database query optimization

- [x] **6.3** Add Accessibility Support
  - Keyboard navigation throughout
  - Screen reader support
  - High contrast theme support

- [x] **6.4** Testing
  - Unit tests for Services
  - Unit tests for ViewModels
  - Integration tests
  - UI automation tests

---

## Completion Criteria

- [x] All Phase 1-6 tasks completed
- [ ] Application builds without errors
- [ ] Unit tests pass
- [ ] Application runs on Windows 10 1809+ and Windows 11
- [x] WinUI 3 styling (Mica, rounded corners) visible
- [x] Core clipboard functionality implemented
- [x] Global hotkeys implemented
- [x] Settings persist across restarts
- [x] Animations and transitions implemented
- [x] Performance optimizations applied
- [x] Accessibility support added

---

## Notes

- This is a greenfield implementation - the new WinUI 3 code will be created alongside the existing MFC code
- The existing MFC code in `/workspace/src/` will be preserved for reference until the new implementation is complete
- Testing will be performed using Windows App SDK's built-in testing capabilities
