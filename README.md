A Simple DLL that detects beep sounds in Caravan SandWitch using audio analysis and provides visual feedback based on vehicle state.

<img width="1917" height="1024" alt="image" src="https://github.com/user-attachments/assets/32b7917d-d7dc-408b-9050-073dffb107d7" />


## Features

- Realtime audio capture and analysis using WASAPI
- FFT based beep sound detection
- Vehicle state detection via memory reading
- Color coded console output logs (green for in vehicle, red for outside/in menu)
- Spacebar triggered beep playback

## Building the DLL

### Prerequisites

- Visual Studio 2019 or later
- Windows SDK 10.0 or later
- C++17 support

### Build Steps

1. **Clone/Extract the project**
   ```
   Extract all source files maintaining the folder structure
   ```

2. **Open in Visual Studio**
   - Create a new DLL project
   - Add all `.cpp` and `.h` files to the project
   - Add `beep.rc` resource file

3. **Configure Project Settings**
   - Set Configuration Type: `Dynamic Library (.dll)`
   - Set C++ Language Standard: `ISO C++17`
   - Add Precompiled Header: `pch.h`
   - Set pch.cpp to "Create" precompiled header

4. **Link Required Libraries**
   Add to Linker > Input > Additional Dependencies:
   ```
   ole32.lib
   winmm.lib
   psapi.lib
   ```

5. **Build**
   - Select Release configuration (x64)
   - Build
   - Output: `beep.dll`

## Injection

### Using Extreme Injector

1. Download and run Extreme Injector as Administrator
2. Launch Caravan SandWitch
3. In Extreme Injector:
   - Select process: `CaravanSandWitch-Win64-Shipping.exe`
   - Add `beep.dll` to the DLL list
   - Click "Inject"
4. A console window titled "B3n00n - Beep Detector" should appear

### Alternative Injection Methods

Any standard DLL injector that supports 64bit processes will work:
- Process Hacker
- Cheat Engine
- Manual mapping tools

## How Beep Detection Works

### Audio Capture Pipeline

1. **WASAPI Loopback Capture**
   - Captures system audio output
   - Samples at 48kHz in float format
   - Processes audio in realtime buffers

2. **Sound Event Detection**
   - Monitors audio energy levels
   - Triggers when energy exceeds threshold (0.0005)
   - Buffers audio until silence detected (< 0.0002)

3. **Frequency Analysis**
   - Applies Hamming window to reduce spectral leakage
   - Performs 2048 point FFT using Cooley Tukey algorithm
   - Extracts frequency spectrum and peaks

4. **Beep Signature Matching**
   
   The algorithm checks for:
   - **Duration**: 90-140 milliseconds
   - **Fundamental Frequency**: 750-900 Hz
   - **Secondary Frequency**: 850-980 Hz
   - **Spectral Centroid**: 3500-5000 Hz
   - **Spectral Spread**: 5500-6800 Hz

5. **Vehicle State Detection**
   - Reads memory at `base + 0x7DCF8B8` (static)
   - Value 145 = In Vehicle (green message)
   - Value ≠ 145 = Outside Vehicle (red message)

## Console Output

When a beep is detected:
- **[+] BEEP BOOP DETECTED IN VEHICLE** (green) - When in the van
- **[-] BEEP BOOP DETECTED OUTSIDE OF VEHICLE** (red) - When outside/menu

## Controls

- **Spacebar**: Plays a beep sound

## Assumptions

1. **Audio Configuration**
   - Game uses default Windows audio output device
   - Audio sample rate is 48kHz
   - System volume is audible

2. **Game Version**
   - Built for Caravan SandWitch Demo (Steam version)
   - Memory offset tested on current version as of development
   - Process name: `CaravanSandWitch-Win64-Shipping.exe`

3. **System Requirements**
   - Windows 10/11 x64
   - Administrative privileges for injection
   - .NET Framework for some injectors

## Limitations

1. **Audio Capture**
   - Only captures system audio
   - Won't detect beeps if game uses exclusive audio mode
   - Requires game/any third party audio to be muted
<img width="980" height="585" alt="image" src="https://github.com/user-attachments/assets/3a5f4257-62bc-4c7b-80e9-221c3813c5f2" />

2. **Memory Reading**
   - Memory offset (0x7DCF8B8) may change with game updates

3. **Detection Accuracy**
   - Optimized for specific van entry beep
   - May have false positives with similar frequency patterns
   - Background sfx/ambience does interfere with detection

## Troubleshooting

**No console window appears:**
- Ensure you're injecting into the correct process
- Run injector as Administrator
- Check if antivirus is blocking the injection

**No beep detection:**
- Verify game audio is playing through default device
- Mute all sounds that may be interfering ingame/youtube/spotify etc
- Check Windows volume mixer
- Try pressing spacebar to test audio playback

**Incorrect vehicle state:**
- Memory offset may have changed with game update
- Ensure you're using the Steam demo version

**Build errors:**
- Verify all Windows SDK components are installed
- Check that linked libraries are available
- Ensure beep.wav resource is properly included

## Technical Architecture

Generally aimed to follow open closed, horizontally scalable code.

```
DLL Entry Point (DllMain)
    ↓
AudioAnalyzer (Main Controller)
    ├── AudioCapture (WASAPI Loopback)
    ├── FrequencyAnalyzer (FFT Processing)
    ├── BeepDetector (Pattern Matching)
    ├── KeyboardMonitor (Global Hook)
    └── VehicleDetector (Memory Reading)
```

## Notes

- The DLL creates a separate thread for audio analysis to avoid blocking the game
- Keyboard hook runs in its own thread with message pump
- Vehicle state is polled every 100ms
- All components are initialized on DLL injection and cleaned up on ejection via destructors
- There has been an attempt to ignore low frequency audio. it works, however it needs to be configurated properly, which I did not do
- This repo will be removed very soon
