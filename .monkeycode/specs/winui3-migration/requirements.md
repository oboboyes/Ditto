# Requirements Document - Ditto WinUI 3 Migration

## Introduction

本项目旨在将 Ditto 剪贴板管理器从 MFC (Microsoft Foundation Classes) 迁移到 WinUI 3 (Windows UI Library 3)，实现现代化的高保真 Windows 原生界面。

## Glossary

- **Ditto**: Windows 剪贴板增强工具，支持剪贴板历史、搜索、分组、快速粘贴
- **WinUI 3**: Microsoft Windows UI Library 3，基于 C++/WinRT 的现代化 UI 框架
- **MFC**: Microsoft Foundation Classes，传统的 Windows C++ UI 框架
- **C++/WinRT**: Windows Runtime API 的标准 C++ 投影
- **MVVM**: Model-View-ViewModel，UI 架构模式
- **xaml**: Extensible Application Markup Language，WinUI 3 的 UI 标记语言

## Requirements

### Requirement 1: Clipboard Monitoring

**User Story:** AS a user, I want Ditto to automatically capture clipboard contents, so that I can access my clipboard history later.

#### Acceptance Criteria

1. WHEN the system clipboard changes, the system SHALL capture the clipboard data within 100ms.
2. WHEN clipboard contains multiple data formats, the system SHALL preserve all formats.
3. WHEN a capture error occurs, the system SHALL log the error and continue monitoring without crashing.
4. WHILE Ditto is running, the system SHALL maintain continuous clipboard monitoring.

### Requirement 2: Clipboard History Storage

**User Story:** AS a user, I want my clipboard history to be persisted, so that I can access past items after restarting the application.

#### Acceptance Criteria

1. WHEN a new clipboard item is captured, the system SHALL store it in the local SQLite database.
2. WHEN the database size exceeds the configured limit, the system SHALL automatically purge oldest entries.
3. WHEN storing images, the system SHALL compress images to reduce storage space.
4. THE system SHALL support storing at least 10,000 clipboard entries without performance degradation.

### Requirement 3: Quick Paste Window Display

**User Story:** AS a user, I want to display a quick paste window with my clipboard history, so that I can search and select items to paste.

#### Acceptance Criteria

1. WHEN the user activates the quick paste hotkey, the system SHALL display the quick paste window within 50ms.
2. WHEN the user types in the search box, the system SHALL filter the clipboard list in real-time.
3. WHEN the user selects an item and presses Enter, the system SHALL paste the selected item to the active application.
4. WHEN the quick paste window loses focus, the system SHALL hide the window automatically.

### Requirement 4: System Tray Integration

**User Story:** AS a user, I want Ditto to minimize to the system tray, so that it runs quietly in the background.

#### Acceptance Criteria

1. WHEN the user clicks the close button, the system SHALL minimize Ditto to the system tray instead of exiting.
2. WHEN the user right-clicks the tray icon, the system SHALL display a context menu with options.
3. WHEN the user double-clicks the tray icon, the system SHALL show the main window.
4. THE system SHALL display a notification icon in the system tray while running.

### Requirement 5: Global Hotkey Support

**User Story:** AS a user, I want to configure global hotkeys, so that I can control Ditto from any application.

#### Acceptance Criteria

1. WHEN the user configures a hotkey combination, the system SHALL register it as a global hotkey.
2. WHEN the user presses the quick paste hotkey, the system SHALL activate the quick paste window.
3. WHEN a hotkey conflict is detected, the system SHALL notify the user and prompt for resolution.
4. THE system SHALL support hotkey combinations with modifiers (Ctrl, Alt, Shift, Win).

### Requirement 6: Group Management

**User Story:** AS a user, I want to organize clipboard items into groups, so that I can categorize and find items easily.

#### Acceptance Criteria

1. WHEN the user creates a new group, the system SHALL add it to the group tree.
2. WHEN the user drags an item to a group, the system SHALL associate the item with that group.
3. WHEN the user selects a group, the system SHALL display only items belonging to that group.
4. WHEN the user deletes a group, the system SHALL prompt whether to delete items or move them to the default group.

### Requirement 7: Settings/Options Management

**User Story:** AS a user, I want to configure Ditto's behavior through a settings dialog, so that I can customize it to my needs.

#### Acceptance Criteria

1. WHEN the user opens the settings dialog, the system SHALL display all configurable options in a navigation view.
2. WHEN the user changes a setting, the system SHALL apply it immediately or after confirmation.
3. WHEN the user resets settings to default, the system SHALL restore all values without requiring a restart.
4. THE system SHALL persist user settings across application restarts.

### Requirement 8: Edit Clipboard Items

**User Story:** AS a user, I want to edit clipboard items before pasting, so that I can modify text or adjust images.

#### Acceptance Criteria

1. WHEN the user selects an item and chooses edit, the system SHALL display the edit window.
2. WHEN the user modifies text content, the system SHALL update the item in the database.
3. WHEN the user saves changes, the system SHALL close the edit window and refresh the list.
4. THE system SHALL support editing text and image formats.

### Requirement 9: Multi-format Support

**User Story:** AS a user, I want Ditto to handle various data formats, so that I can work with text, images, files, and rich content.

#### Acceptance Criteria

1. THE system SHALL support plain text (CF_TEXT) format.
2. THE system SHALL support Unicode text (CF_UNICODETEXT) format.
3. THE system SHALL support images (CF_BITMAP, PNG, JPEG) format.
4. THE system SHALL support file drop lists (CF_HDROP) format.
5. THE system SHALL support HTML content with formatting.
6. THE system SHALL support Rich Text Format (RTF).

### Requirement 10: Window Appearance (WinUI 3 Style)

**User Story:** AS a user, I want the application to use WinUI 3 design language, so that it looks modern and consistent with Windows 11.

#### Acceptance Criteria

1. THE system SHALL use WinUI 3 Fluent Design System components.
2. THE system SHALL support Windows 11 Mica material and backdrop effects.
3. THE system SHALL use the system accent color for highlights.
4. THE system SHALL support both light and dark themes.
5. THE system SHALL render crisp text using DirectWrite typography.
6. WHEN the window is displayed, it SHALL use rounded corners and modern shadows.

### Requirement 11: Performance Requirements

**User Story:** AS a user, I want Ditto to be responsive and fast, so that it does not interrupt my workflow.

#### Acceptance Criteria

1. THE system SHALL start within 2 seconds on a typical system.
2. THE system SHALL consume less than 100MB of memory during normal operation.
3. THE system SHALL not cause noticeable CPU usage when idle.
4. THE quick paste window SHALL appear within 50ms of hotkey press.

### Requirement 12: Plugin/Addin Support

**User Story:** AS a user, I want to extend Ditto's functionality through plugins, so that I can add custom features.

#### Acceptance Criteria

1. THE system SHALL support loading plugins from the Addins directory.
2. THE system SHALL provide a plugin API for common operations.
3. WHEN a plugin causes an error, the system SHALL log it and disable the plugin without crashing.
4. THE system SHALL support ChaiScript for scripting automation.

### Requirement 13: Data Import/Export

**User Story:** AS a user, I want to backup and restore my clipboard history, so that I can transfer data between systems.

#### Acceptance Criteria

1. THE system SHALL export clipboard history to a file in the Ditto archive format.
2. THE system SHALL import clipboard history from a Ditto archive file.
3. WHEN importing, the system SHALL merge with existing data or replace based on user choice.
4. THE system SHALL support exporting individual items to standard formats (TXT, PNG, etc.).

### Requirement 14: Network Sync (Optional)

**User Story:** AS a user, I want to sync clipboard history across multiple computers, so that I can access my clips anywhere.

#### Acceptance Criteria

1. WHEN network sync is enabled, the system SHALL sync clips to the configured server.
2. WHEN a conflict occurs, the system SHALL use the most recent item or prompt for resolution.
3. THE system SHALL support encrypted transfer for security.
4. THE system SHALL allow selective sync of specific groups.
