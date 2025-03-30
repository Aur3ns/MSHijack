# Project MSHijack  
  - UAC Bypass via Registry Hijack (ComputerDefaults.exe)

This project is a **proof-of-concept (PoC)** demonstrating how to bypass Windows **User Account Control (UAC)** by hijacking a registry key used by auto-elevated system binaries. Specifically, it abuses `ComputerDefaults.exe` (a trusted Windows binary) to launch arbitrary commands with elevated privileges — **without triggering a UAC prompt**.

> ⚠️ This project is for **educational and research purposes only**.  
> **Unauthorized use** on systems you do not own or have permission to test is **illegal**.

---

## 📖 Table of Contents

- [Overview](#overview)
- [Technical Background](#technical-background)
- [How It Works](#how-it-works)
- [Requirements](#requirements)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Testing Example](#testing-example)
- [Manual Cleanup](#manual-cleanup)
- [Security Considerations](#security-considerations)
- [References](#references)
- [Disclaimer](#disclaimer)

---

## 📌 Overview

- **Target OS**: Windows 10 / Windows 11 (x64)
- **Technique**: UAC bypass via `HKCU` registry hijack
- **Method**: Auto-elevated binary (`ComputerDefaults.exe`) executes user-defined command from registry
- **Persistence**: None (one-shot execution, key is deleted post-use)
- **Privileges Gained**: Elevated (High Integrity Level)

---

## 🧠 Technical Background

Windows UAC is designed to prevent unauthorized elevation of privilege. However, several system processes are **whitelisted** to auto-elevate without prompting the user — as long as they are called by a local administrator.

`ComputerDefaults.exe` is one such binary. When it runs, it attempts to open the following registry key to check if a shell command is configured:

```
HKCU\Software\Classes\ms-settings\shell\open\command
```

If this key exists and contains a valid `(Default)` string, and a `DelegateExecute` value (even empty), the binary will **execute the content of `(Default)` with elevated rights** without any UAC prompt — **as long as the current user is in the Administrators group**.

---

## ⚙️ How It Works

### Registry Keys Created:
- `HKCU\Software\Classes\ms-settings\shell\open\command`
  - `(Default)` = Command to run (e.g. `cmd.exe`)
  - `DelegateExecute` = "" (empty string)

### Binary Triggered:
- `C:\Windows\System32\ComputerDefaults.exe`

---

## ✅ Requirements

| Requirement        | Details                                |
|--------------------|----------------------------------------|
| OS                 | Windows 10 or 11 (64-bit)              |
| Privileges         | Must be a member of `Administrators`   |
| UAC                | Must be enabled (but not max level)    |
| Antivirus/EDR      | Must not block `ComputerDefaults.exe`  |
| Registry Access    | Must be allowed for `HKCU`             |

---

## 🛠️ Build Instructions

### Prerequisites

- `gcc` from **MinGW-w64** or
- Microsoft **Visual Studio** (cl.exe)

### With GCC (MinGW-w64):

```bash
gcc -m64 -o ms.exe ms.c -ladvapi32 -lshell32
```

✅ Use `-m64` to ensure the binary is 64-bit, matching `ComputerDefaults.exe`.

---

## ▶️ Usage

```bash
ms.exe "your_command_here"
```

### Example:

```bash
ms.exe "cmd.exe /c whoami > %TEMP%\elevated.txt"
```

This will create a file with the current elevated user name, **without showing any UAC prompt**.

---

## 🧪 Testing Example

To validate that this works:

1. Open a regular command prompt **(not Admin)**.
2. Run:

   ```bash
   ms.exe "powershell -Command \"Start-Process notepad.exe -Verb runAs\""
   ```

3. Notepad should open with elevated privileges, **without showing the UAC consent prompt**.
4. You can verify elevation by checking the process integrity level in Task Manager (via Process Explorer or similar).

---

## 🧹 Manual Cleanup

The tool automatically deletes the registry key after execution.  
If something fails, you can clean up manually:

```cmd
reg delete "HKCU\Software\Classes\ms-settings\shell\open\command" /f
```

---

## 🔒 Security Considerations

- This technique is **well known** and documented in MITRE ATT&CK (T1546.015).
- It works because of Microsoft’s **trust** in certain auto-elevated binaries.
- It relies on the fact that the `HKCU\Software\Classes` path is **user-writable**.
- **Modern EDRs and AVs may detect or block** the modification of this key or the execution of `ComputerDefaults.exe` with a hijacked handler.
- **Windows Defender may log this behavior** under event logs or controlled folder access.

> 🛑 It does **not work** when UAC is set to "Always Notify".

---

## 📚 References

- [MITRE ATT&CK T1546.015 – Registry Run Keys / Startup Folder](https://attack.mitre.org/techniques/T1546/015/)
- [UACMe – UAC bypass techniques](https://github.com/hfiref0x/UACME)
- [UAC Bypass Using Fodhelper](https://pentestlab.blog/2017/04/18/uac-bypass-computersettings/)
- [Windows Auto-Elevation – Matt Nelson](https://posts.specterops.io/uac-bypass-using-eventvwr-exe-and-registry-hijacking-9ebe83f4f52)

---

## 🗂 Project Structure

```text
ms.c           - Main C source file (exploit implementation)
README.md      - Project documentation
LICENSE        - MIT license file (optional)
```

---

## 📝 License

This project is licensed under the MIT License. See `LICENSE` for details.

---

## ⚠️ Disclaimer

This code is provided **for educational purposes only**.  
The author assumes **no responsibility** for misuse.  
**Do not use this code in production or on systems you do not own or have explicit authorization to test.**
