# Wave - Hand-Tracked Harmonics Synthesizer

A hand-gesture controlled harmonic synthesizer for accessible music creation and education.

## Features

### Display Modes
- **Solo**: Shows only the mixed output waveform (all harmonics combined)
- **Separate**: Displays each harmonic in its own vertical lane for individual analysis
- **Combined**: Overlaps all harmonic channels in the same view for comparison. Note: Can become visually unclear with many harmonics - recommended maximum of 5 harmonics for best clarity.

### Waveform Presets
- **Sine**: Pure sine wave
- **Square**: Mixed harmonics create a square shaped waveform
- **Triangle**: Harmonics combine to form a triangular waveform
- **Sawtooth**: Creates a sawtooth pattern from harmonic synthesis

## Prerequisites

- **CMake** (version 3.20 or higher)
- **Qt** (Qt5 or Qt6)
- **Python 3** (for hand tracking)
- **C++ Compiler** (supporting C++20)

## Build Instructions

### macOS

**Requirement:** [Homebrew](https://brew.sh/) must be installed.

1. Install dependencies:
   ```bash
   brew install cmake qt
   ```

2. Set up Python environment (or run `./setup.sh`):
   ```bash
   python3 -m venv .venv
   source .venv/bin/activate
   pip install -r requirements.txt
   ```

3. Build the project:
   ```bash
   mkdir -p build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . --parallel
   ```

4. Run:
   ```bash
   ./bin/WaveControllerFinal
   ```

### VSCode (Recommended)

If using VSCode, simply press **F5** to build and run the project automatically.

### Linux

1. Install dependencies:
   ```bash
   sudo apt-get update
   sudo apt-get install cmake qt6-base-dev qt6-multimedia-dev python3-venv
   ```

2. Set up Python environment (or run `./setup.sh`):
   ```bash
   python3 -m venv .venv
   source .venv/bin/activate
   pip install -r requirements.txt
   ```

3. Build the project:
   ```bash
   mkdir -p build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . --parallel
   ```

4. Run:
   ```bash
   ./bin/WaveControllerFinal
   ```

### Windows

1. Install dependencies:
   - Download and install [CMake](https://cmake.org/download/)
   - Download and install [Qt](https://www.qt.io/download-qt-installer)
   - Install [Python 3](https://www.python.org/downloads/)

2. Set up Python environment (Command Prompt):
   ```cmd
   python -m venv .venv
   .venv\Scripts\activate
   pip install -r requirements.txt
   ```

3. Build the project (Command Prompt):
   ```cmd
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . --config Release --parallel
   ```

4. Run:
   ```cmd
   bin\Release\WaveControllerFinal.exe
   ```

## Project Structure

```
WaveController_Final/
├── setup.sh                   # Python virtual environment setup script
├── requirements.txt           # Python dependencies for hand tracking
├── external/                  # Third-party library
│   └── kissfft/               # FFT library for frequency analysis
├── CMakeLists.txt             # Root CMake build configuration
├── main.cpp                   # Main C++ application
├── wave/                      # Core library modules
│   ├── Audio/                 # Audio generation and synthesis
│   │   ├── inc/               # Audio headers (AudioGenerator, AudioRecorder)
│   │   ├── src/               # Audio implementation files
│   │   └── CMakeLists.txt     # Audio module build config
│   ├── GUI/                   # User interface components
│   │   ├── inc/               # GUI headers (HarmonicControlPanel)
│   │   ├── src/               # GUI implementation files
│   │   └── CMakeLists.txt     # GUI module build config
│   ├── HandTracker/           # Hand tracking integration
│   │   ├── inc/               # HandTracker headers (WaveController)
│   │   ├── src/               # HandTracker implementation files
│   │   └── CMakeLists.txt     # HandTracker module build config
│   ├── Visualization/         # Waveform and spectrum display
│   │   ├── inc/               # Visualization headers (Waveform, SpectrumAnalyzer)
│   │   ├── src/               # Visualization implementation files
│   │   └── CMakeLists.txt     # Visualization module build config
│   ├── Common/                # Shared utilities and constants
│   │   ├── inc/               # Common headers (Constants, Timer)
│   │   ├── src/               # Common implementation files
│   │   └── CMakeLists.txt     # Common module build config
│   └── CMakeLists.txt         # Wave library root build config
├── hand_landmarker.task       # MediaPipe hand tracking model file
├── main.py                    # Python hand tracking camera script
├── recordings/                # Output folder for recorded audio files
└── README.md                  # Project documentation
```

## Source Files

```
WaveController_Final/
├── main.cpp                                        # Application Main window setup
└── wave/
    ├── Audio/
    │   ├── inc/
    │   │   ├── AudioGenerator.h                    # Harmonic synthesis generator
    │   │   ├── AudioRecorder.h                     # WAV file recording and file creation
    │   │   └── Buffer.h                            # Bbuffer for audio data
    │   └── src/
    │       ├── AudioGenerator.cpp                  # Audio generation implementation
    │       ├── AudioRecorder.cpp                   # Recording implementation with WAV header
    │       └── Buffer.cpp                          # Ring buffer implementation
    ├── Common/
    │   ├── inc/
    │   │   ├── Constants.h                         # Common audio definition constants
    │   │   └── Timer.h                             # Centralized Timer Singleton
    │   └── src/
    │       └── Timer.cpp                           # Timer implementation
    ├── GUI/
    │   ├── inc/
    │   │   ├── HarmonicControlPanel.h              # Harmonic slider and recording button controls
    │   │   ├── SynthVisualizationWindow.h          # Main window combining all visualizations
    │   │   └── WaveForm.h                          # Waveform display with Solo, Separate or Combined modes
    │   └── src/
    │       ├── HarmonicControlPanel.cpp            # Control panel implementation
    │       ├── SynthVisualizationWindow.cpp        # Main window layout and coordination
    │       └── WaveForm.cpp                        # Waveform rendering with vertical stacking
    ├── HandTracker/
    │   ├── inc/
    │   │   └── WaveController.h                    # UDP receiver for hand tracking data from Python
    │   └── src/
    │       └── WaveController.cpp                  # Hand gesture processing and audio control
    └── Visualization/
        ├── inc/
        │   ├── KissFFTAnalyzer.h                   # FFT wrapper for frequency analysis
        │   └── SpectrumAnalyzer.h                  # Real time frequency spectrum visualization
        └── src/
            ├── KissFFTAnalyzer.cpp                 # KissFFT integration
            └── SpectrumAnalyzer.cpp                # Spectrum rendering with logarithmic scale
```

## Usage
Start the hand tracking camera (optional):
   ```bash
   source .venv/bin/activate  # or .venv\Scripts\activate on Windows
   python main.py
   ```
   
   **macOS Warning:** If you have an iPhone, it may override your MacBook's camera causing the script to fail. Turn off your iPhone to resolve this issue.

2. Launch the application:
   - Press **F5** in VSCode, or
   - Run `./build/bin/WaveControllerFinal` (macOS/Linux) `build\bin\Release\WaveControllerFinal.exe` (Windows)

3. Use the harmonic Control Panel to modify the number of Harmonics or their settings - either manually or with the preset profiles (Sine, Sawtooth, Square or Triangle)
4. Switch between the different display modes Solo, Separate, Combined
5. Click Record to record audio to the `recordings/` folder
6. Wave one hand in front of the camera to start synthesizing music. Pinch your index finger and thumb together to add an effect 
