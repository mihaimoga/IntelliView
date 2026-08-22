# AGENTS.md — AI & Contributor Guide for IntelliView

This document provides architectural context, development guidelines, build instructions, coding standards, and operational rules for AI agents and human contributors working on the **IntelliView** solution.

---

## 1. Solution Overview

**IntelliView** is an open-source, high-performance Microsoft Windows desktop image viewer written in C++ with MFC (Microsoft Foundation Classes) and modern Win32 APIs.

- **Primary Language**: C++ (targeting `stdcpplatest` / `stdclatest`)
- **UI Framework**: MFC with Ribbon interface (`res\ribbon.mfcribbon-ms`) & Edge WebView2 integration
- **Platform**: Microsoft Windows (Win32 / x64, Unicode, statically linked MFC)
- **License**: GNU General Public License v3.0 (GPL-3.0) for IntelliView; MIT for `genUp4win`
- **Solution File**: `IntelliView.sln`

---

## 2. Projects & Workspace Structure

The solution contains three main projects:

| Project | Type | Description |
| :--- | :--- | :--- |
| **`IntelliView`** | Application (`.exe`) | Main MFC Document/View image viewing application with Ribbon UI, WebView2 browser integration, slideshow, multi-frame TIFF/QOI/GIF viewing, and updater dialogs. |
| **`genUp4win`** | Dynamic Library (`.dll`) | Generic update checking and download engine utilizing WinINet/URLDownloadToFile, XML configuration parsing, and SHA256 checksum verification. |
| **`Setup`** | Installer (`.vdproj`) | Visual Studio Setup project producing the `IntelliViewSetup.msi` installation package. |

### Directory Layout

```
IntelliView/
├── .github/                     # GitHub workflows and metadata (e.g. FUNDING.yml)
├── genUp4win/                   # genUp4win DLL project source and CMake configs
│   ├── AppSettings.h            # XML application settings helper
│   ├── genUp4win.cpp / .h       # Main DLL export implementations
│   ├── SHA256.cpp / .h          # SHA-256 hash calculation routines
│   ├── VersionInfo.cpp / .h     # Win32 file version extraction
│   ├── CMakeLists.txt           # CMake build definition
│   └── genUp4win.vcxproj        # Visual Studio project file
├── IntelliView/                 # IntelliView MFC desktop app source
│   ├── res/                     # Bitmaps, icons, ribbon markup (.mfcribbon-ms)
│   ├── CheckForUpdatesDlg.*     # Auto-update dialog leveraging genUp4win
│   ├── ChildFrame.*             # MDI child frame handling
│   ├── EdgeWebBrowser.*         # WebView2 browser wrapper
│   ├── GotoPageDlg.*            # Multi-page navigation dialog
│   ├── HLinkCtrl.*              # Hyperlink control (PJ Naughter)
│   ├── IntelliView.*            # CWinAppEx application class & entry point
│   ├── IntelliViewDoc.*         # CDocument image data management
│   ├── IntelliViewView.*        # CView rendering, zoom, panning, GDI+/WIC operations
│   ├── MainFrame.*              # CMDIFrameWndEx ribbon frame window
│   ├── QOIPP.h                  # Quite OK Image (QOI) format reader/writer
│   ├── RenameDlg.*              # File renaming dialog
│   ├── sinstance.*              # Single-instance enforcement (PJ Naughter)
│   ├── VersionInfo.*            # File version helper
│   ├── WebBrowserDlg.*          # WebView2 embedded browser dialog
│   ├── packages.config          # NuGet package dependencies
│   └── IntelliView.vcxproj      # Visual Studio project file
├── packages/                    # Restored NuGet packages (WebView2, WIL)
├── Setup/                       # Installer setup project
├── IntelliView.sln              # Visual Studio solution file
├── CONTRIBUTING.md               # Contribution rules and coding style
├── SECURITY.md                   # Security reporting policy
├── SoftwareContentRegister.html # 3rd party attribution register
└── README.md                    # Project README
```

---

## 3. Dependencies & Prerequisites

- **IDE / Toolset**: Visual Studio 2022 (MSVC toolset `v143` or `v145`)
- **C++ Standard**: `/std:c++latest`, `/std:clatest`
- **Windows SDK**: Windows 10 SDK (`10.0` or latest)
- **MFC/ATL**: C++ MFC for latest v143/v145 build tools (x86 & x64)
- **NuGet Packages**:
  - `Microsoft.Web.WebView2` (`1.0.4129.50`+)
  - `Microsoft.Windows.ImplementationLibrary` (`1.0.260126.7`+)
- **Included Third-Party Libraries**:
  - *genUp4win* (Stefan-Mihai MOGA, MIT)
  - *EZView*, *CHLinkCtrl*, *CInstanceChecker*, *CVersionInfo* (PJ Naughter)
  - *QOIPP* (Quite OK Image format MSVC encapsulation)

---

## 4. Build & Verification Commands

### Visual Studio / MSBuild
To restore NuGet packages and build all configurations:

```powershell
# Restore NuGet packages
nuget restore IntelliView.sln

# Build x64 Release
msbuild IntelliView.sln /p:Configuration=Release /p:Platform=x64 /m

# Build x64 Debug
msbuild IntelliView.sln /p:Configuration=Debug /p:Platform=x64 /m

# Build Win32 (x86) Release
msbuild IntelliView.sln /p:Configuration=Release /p:Platform=Win32 /m
```

### genUp4win Standalone (CMake)
```powershell
cd genUp4win
cmake -B build -S .
cmake --build build --config Release
```

---

## 5. Coding Standards & Conventions

All modifications must adhere to the style guide specified in `CONTRIBUTING.md`:

### Formatting & Braces
- **No Java-like braces**: Place opening braces `{` on a new line for functions, control flow statements, and classes.
  - *Exception*: Single-line inline method definitions in header files may use same-line braces (`int getVal() { return _val; }`).
- **Indentation**: Use **Tabs** instead of spaces for indentation (tab width 4).
- **Whitespace**:
  - Leave one space before and after binary/ternary operators (`a == 10 && b == 42`).
  - Leave one space after semicolons in `for` loops (`for (int i = 0; i != 10; ++i)`).
  - No space between function names and opening parentheses (`foo()`, `obj.foo(24)`).
  - One space between control keywords and opening parentheses (`if (condition)`, `while (condition)`).
- **Switch Statements**:
  ```cpp
  switch (expression)
  {
	  case 1:
	  {
		  // Action
		  break;
	  }
	  default:
		  break;
  }
  ```

### Naming Conventions
- **Classes**: PascalCase (`CIntelliViewDoc`, `CMainFrame`).
- **Methods & Parameters**: camelCase or standard MFC conventions (`myMethod(int myParam)` / `OnAppAbout()`).
- **Member Variables**: Preceded by an underscore (`_memberVar`) or standard MFC Hungarian prefixes (`m_nIndex`, `m_strPath`).
- **Descriptive Names**: Avoid short or cryptic variable names; choose descriptive identifiers.

### C++ Idioms & Best Practices
- **Modern C++**: Use C++17/C++20/C++latest features where appropriate (e.g., brace initialization `MyClass instance{10.4};`).
- **Strings**: Use `.empty()` to check for empty strings instead of `str == ""`.
- **Casts**: Always use C++ explicit casts (`static_cast`, `reinterpret_cast`) rather than C-style casts `(int)x`.
- **Operators**: Use `!` instead of `not`, `&&` instead of `and`, `||` instead of `or`.
- **Pointers & References**:
  - Avoid raw pointers where references or smart pointers can be used.
  - Prefer `std::unique_ptr` over `std::shared_ptr`. Avoid `new` when automatic/stack variables suffice.
- **Incrementing**: Prefer pre-increment (`++i`) over post-increment (`i++`).
- **Headers**: Never put `using namespace ...` in header files.
- **Comments**: Use C++ single-line comments (`// ...`) instead of C block comments (`/* ... */`).

---

## 6. Architecture & Implementation Guidelines

1. **Document / View Pattern**:
   - `CIntelliViewDoc` owns image loading, image frames/pages, and state persistence.
   - `CIntelliViewView` handles GDI+/Direct2D/WIC drawing, user interaction, zooming, and full-screen display.
2. **Ribbon & Commands**:
   - Ribbon elements are declared in `res/ribbon.mfcribbon-ms` and mapped to command handlers in `MainFrame` or `IntelliViewView`.
3. **WebView2 Integration**:
   - Use `CEdgeWebBrowser` and `CWebBrowserDlg` for rendering HTML pages (Release Notes, About info) using modern Edge WebView2 runtime.
4. **Auto-Updater**:
   - `CheckForUpdatesDlg` calls `genUp4win` functions (`CheckForUpdates`, `WriteConfigFile`). Ensure any changes maintain DLL ABI compatibility.
5. **Static Analysis & Safety**:
   - Code analysis is enabled (`RunCodeAnalysis = true`). Address any compiler or analyzer warnings (`Level4` in Debug).

---

## 7. Guidelines for AI Agents

When working on tasks in this repository:

1. **Plan & Explore First**: Check related header files, message maps, and resource files before modifying MFC components.
2. **Preserve Surrounding Style**: Match existing indentation (tabs), brace formatting, and MFC macro conventions (`BEGIN_MESSAGE_MAP`, `END_MESSAGE_MAP`, `DDX_Control`).
3. **Verify Builds**: Run MSBuild or the workspace build tool (`run_build`) after making changes to verify compilation on both Debug and Release configurations.
4. **Keep Edits Atomic**: Make minimal, focused changes without unnecessary reformatting.
5. **No Regressions in Resource IDs**: Ensure `Resource.h` and `.rc` file modifications stay synchronized when adding or updating dialog controls, ribbon commands, or string resources.
