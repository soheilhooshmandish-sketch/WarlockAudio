# Warlock 1.1.0 — pack all data and install on Windows

One 64-bit VST3. Skins, knobs, stomp, logo, and three cabinet IRs are **compiled into the DLL**. The DAW never has to find loose image files.

This kit is source. You build the `.vst3` on a Windows PC (Visual Studio 2022). This file is also `README-EXPORT.txt` in the zip.

---

## 1. What gets packed

### A. Inside the DLL (`juce_add_binary_data`)

These become `BinaryData::*` in C++. `PluginEditor` / `PluginProcessor` load them from **memory**. If you delete `Contents/Resources`, the UI and cabs still work.

| File | Role | Format |
|---|---|---|
| `Resources/chassis.jpg` | Pedal metal face | JPEG ~384 px |
| `Resources/knob.jpg` | Aluminum 1510 knob | JPEG ~192 px |
| `Resources/stomp.jpg` | 3PDT mushroom | JPEG ~192 px |
| `Resources/logo.png` | Warlock mark | PNG 192 px |
| `Resources/Cab_Center.wav` | 4×12 on-axis IR | 16-bit mono 44.1 kHz |
| `Resources/Cab_Edge.wav` | 4×12 off-axis IR | 16-bit mono 44.1 kHz |
| `Resources/CabinetIR.wav` | Bedroom IR | 16-bit mono 48 kHz |
| `Resources/legal.txt` | License text | UTF-8 |

CMake list (must match filenames exactly):

```
juce_add_binary_data(WarlockData ... SOURCES
    Resources/CabinetIR.wav
    Resources/Cab_Center.wav
    Resources/Cab_Edge.wav
    Resources/legal.txt
    Resources/chassis.jpg
    Resources/knob.jpg
    Resources/stomp.jpg
    Resources/logo.png
)
```

### B. Beside the DLL (host can read without loading code)

| Path in the bundle | Why |
|---|---|
| `Contents/Resources/moduleinfo.json` | Cubase / SDK 3.7.8+ catalog |
| `Contents/Resources/legal.txt` | Shown by some hosts |
| `Contents/Resources/Snapshots/*_snapshot.png` | Cubase Media Rack 1× |
| `Contents/Resources/Snapshots/*_snapshot_2.0x.png` | HiDPI thumbnail |
| `desktop.ini` + `Plugin.ico` | Windows Explorer icon |

Do **not** also copy chassis/knobs/IRs into `Contents/Resources`. That doubles size. They already live in the DLL.

### C. Final folder you install

```
C:\Program Files\Common Files\VST3\Warlock.vst3\     ← this folder IS the plugin
├── desktop.ini
├── Plugin.ico
└── Contents\
    ├── x86_64-win\
    │   └── Warlock.vst3          ← PE DLL (skins + IRs inside)
    └── Resources\
        ├── legal.txt
        ├── moduleinfo.json
        └── Snapshots\
            ├── Warlock_snapshot.png
            └── Warlock_snapshot_2.0x.png
```

Never copy only the inner `Warlock.vst3` file up to `Common Files\VST3\`. Cubase will miss snapshots; some scanners skip a bare DLL.

---

## 2. Tools on the PC

| Tool | Why |
|---|---|
| Windows 10/11 64-bit | Host OS |
| [Visual Studio 2022](https://visualstudio.microsoft.com/) — workload **Desktop development with C++** | Compiler |
| [CMake 3.22+](https://cmake.org/download/) — tick “Add CMake to PATH” | Build |
| Git | CMake fetches JUCE 7.0.12 |
| Optional: [Inno Setup 6.3 or 7](https://jrsoftware.org/isdl.php) | `Warlock_Distortion_Setup_x64.exe` |

Close Cubase, Reaper, Live, FL, Studio One, Bitwig before replacing a DLL.

---

## 3. Pack (build the VST3)

1. Unzip the kit. You want a folder that contains `CMakeLists.txt`, `build.bat`, `Source\`, `Resources\`.
2. Double-click **`build.bat`**  
   or in **x64 Native Tools Command Prompt for VS 2022**:

```
cd /d C:\src\Warlock
cmake --preset vs2022-x64
cmake --build --preset warlock-release
```

3. Wait for JUCE to download (first time only).
4. Output:

```
build\Warlock_artefacts\Release\VST3\Warlock.vst3\
```

That folder is already a Steinberg bundle. POST_BUILD copies snapshots, legal, `moduleinfo.json`, `desktop.ini`, `Plugin.ico`.

### Add or replace a skin / IR

1. Overwrite the file under `Resources\` (keep the **same filename**).
2. Run `build.bat` again. BinaryData is regenerated. Miss a filename in `CMakeLists.txt` and the editor will have no knob/logo.

| You changed | Rebuild needed |
|---|---|
| jpg / png / wav / legal.txt | Yes (`build.bat`) |
| `PluginEditor.cpp` layout | Yes |
| Snapshots PNG only | Yes (POST_BUILD copies them) |
| `installer.iss` text | No — only recompile Inno |

---

## 4. Pack the installer (optional)

1. Install Inno Setup 6.3+.
2. Confirm this exists:

```
build\Warlock_artefacts\Release\VST3\Warlock.vst3\Contents\x86_64-win\Warlock.vst3
```

3. Open `installer.iss` → **Build → Compile**.
4. Output:

```
BuildInstaller\Warlock_Distortion_Setup_x64.exe
```

`{app}` **is** `Warlock.vst3`. The script unpacks **into** that folder, then adds snapshots / legal / icon.

Silent:

```
Warlock_Distortion_Setup_x64.exe /VERYSILENT /NORESTART /ALLUSERS
```

---

## 5. Install on the PC

### Path A — installer

1. Right-click `Warlock_Distortion_Setup_x64.exe` → **Run as administrator**.
2. Leave the folder as:

```
C:\Program Files\Common Files\VST3\Warlock.vst3
```

   It **must** end in `.vst3`.
3. Finish. Allow Windows to close a locked DAW if asked.

### Path B — manual (no Inno)

Copy the **whole folder**:

From: `build\Warlock_artefacts\Release\VST3\Warlock.vst3`  
To:   `C:\Program Files\Common Files\VST3\Warlock.vst3`

Admin Explorer or:

```
xcopy /E /I /Y build\Warlock_artefacts\Release\VST3\Warlock.vst3 "C:\Program Files\Common Files\VST3\Warlock.vst3"
```

### Wrong places (will not scan)

- `C:\Program Files\Nihil Audio\`
- `C:\Program Files (x86)\Common Files\VST3\` (32-bit)
- Flattened single file `C:\Program Files\Common Files\VST3\Warlock.vst3` that is a DLL, not a folder

---

## 6. Rescan in the DAW

Vendor **Nihil Audio**. Category **Fx / Distortion**. Name **Warlock**.

| Host | Rescan |
|---|---|
| Cubase / Nuendo | Studio → VST Plug-in Manager → Rescan |
| Reaper | Preferences → Plug-ins → VST → Clear cache / re-scan |
| Ableton Live | Preferences → Plug-Ins → Use VST3 system folders → Rescan |
| FL Studio | Options → Manage plugins → Find plugins |
| Studio One | Options → Locations → VST Plug-ins → Rescan |
| Bitwig | Settings → Locations → VST3 → Rescan |

Uninstall: Windows **Settings → Apps → Warlock Chainsaw Distortion**, or delete the `Warlock.vst3` folder.

---

## 7. Checklist — “did all data pack?”

After `build.bat`, open `build\Warlock_artefacts\Release\VST3\Warlock.vst3` and confirm:

- [ ] `Contents\x86_64-win\Warlock.vst3` exists and is several hundred KB (DLL + BinaryData)
- [ ] `Contents\Resources\Snapshots\Warlock_snapshot.png`
- [ ] `Contents\Resources\Snapshots\Warlock_snapshot_2.0x.png`
- [ ] `Contents\Resources\legal.txt`
- [ ] `Contents\Resources\moduleinfo.json`
- [ ] `Plugin.ico` at the bundle root

In a DAW: open the editor. You must see the **logo**, **rotating knobs**, **stomp**, **chassis tint**. If you see grey circles, BinaryData did not compile — check `CMakeLists.txt` SOURCES and rebuild.

Cab IR: enable **CAB IR**, pick Crypt / Catacomb / Bedroom. Tone should darken. That means the three WAVs are inside the DLL.

---

## 8. If the UI is missing

1. You copied only the inner DLL — copy the **folder** named `Warlock.vst3`.
2. Old plugin cached — rescan / restart the DAW.
3. Rebuilt without `juce_add_binary_data` — open `CMakeLists.txt`, confirm the eight SOURCES, delete `build\` and run `build.bat` again.
4. Filename change (`logo.PNG` vs `logo.png`) — Windows is case-insensitive; JUCE BinaryData names are `logo_png`. Keep lowercase.

The browser pedal on this page is a preview. It is **not** what the DAW loads.
