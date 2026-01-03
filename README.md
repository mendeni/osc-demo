# OSC Demo

A simple C++ application demonstrating Open Sound Control (OSC) host functionality, compatible with the `oscsend` command-line utility.

## Overview

This project includes two applications:
1. **osc_host** - A basic OSC server with ping/pong functionality
2. **OSCControlApp** - A JUCE GUI application with sliders and toggles controllable via OSC

## Features

### OSC Host (osc_host)
- **Ping/Pong**: Send a `/ping` message and receive a `/pong` response
- **Generic Message Handler**: Logs any unmatched OSC messages
- **Graceful Shutdown**: Handles SIGINT and SIGTERM signals
- **Thread-based Server**: Uses liblo's server thread for concurrent message handling

### JUCE OSC Control App (OSCControlApp)
- **Toggle Button**: OSC-controllable toggle switch
- **Horizontal Slider**: Linear slider with 0.0-1.0 range
- **Vertical Slider**: Linear slider with 0.0-1.0 range
- **Rotary Knob**: Rotary control with 0.0-1.0 range
- **Real-time OSC Control**: All UI elements respond to OSC messages
- **Visual Feedback**: Value labels display current control states

## Requirements

- CMake 3.15 or higher
- C++17 compatible compiler (g++, clang++)
- liblo (Lightweight OSC library)
- oscsend utility (from liblo-tools package)
- JUCE framework (included as a submodule)
- X11 development libraries (for Linux GUI support)

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install cmake g++ liblo-dev liblo-tools \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
    libasound2-dev libfreetype6-dev libgl1-mesa-dev
```

On macOS:
```bash
brew install liblo cmake
```

## Building

First, initialize the JUCE submodule:
```bash
git submodule update --init --recursive
```

Then build the project:
```bash
mkdir build
cd build
cmake ..
make
```

This will create two executables:
- `build/osc_host` - The basic OSC server
- `build/juce_osc_app/OSCControlApp_artefacts/OSCControlApp` - The JUCE GUI application

## Running

### OSC Host
Start the OSC host:
```bash
./build/osc_host
```

The server will listen on port 7770 by default.

### JUCE OSC Control App
Start the JUCE application:
```bash
./build/juce_osc_app/OSCControlApp_artefacts/OSCControlApp
```

The application will start a GUI with four controllable elements and listen for OSC messages on port 7771.

## Testing

### OSC Host (port 7770)
In another terminal, send a ping message using `oscsend`:
```bash
oscsend localhost 7770 /ping
```

You should see output in the server terminal indicating:
- Message received
- Pong response sent

Send a custom message:
```bash
oscsend localhost 7770 /test s "hello"
```

### JUCE OSC Control App (port 7771)

Control the toggle button (0 = OFF, 1 = ON):
```bash
oscsend localhost 7771 /toggle i 1
oscsend localhost 7771 /toggle i 0
```

Control the horizontal slider (float value 0.0 to 1.0):
```bash
oscsend localhost 7771 /hslider f 0.75
oscsend localhost 7771 /hslider f 0.25
```

Control the vertical slider (float value 0.0 to 1.0):
```bash
oscsend localhost 7771 /vslider f 0.9
oscsend localhost 7771 /vslider f 0.1
```

Control the rotary knob (float value 0.0 to 1.0):
```bash
oscsend localhost 7771 /knob f 0.5
oscsend localhost 7771 /knob f 1.0
```

## Usage Examples

### OSC Host Server (port 7770)
- `/ping` - Responds with a `/pong` message back to the sender
- Any other address - Logged as an unhandled message

### JUCE OSC Control App (port 7771)
- `/toggle` (int) - Controls the toggle button (0 = OFF, 1 = ON)
- `/hslider` (float) - Controls the horizontal slider (0.0 to 1.0)
- `/vslider` (float) - Controls the vertical slider (0.0 to 1.0)
- `/knob` (float) - Controls the rotary knob (0.0 to 1.0)

All float values are clamped to the 0.0-1.0 range automatically.

## Project Structure

```
osc-demo/
├── src/                    # OSC host source code
│   └── main.cpp
├── juce_osc_app/          # JUCE GUI application
│   ├── Source/
│   │   ├── Main.cpp
│   │   ├── MainComponent.h
│   │   └── MainComponent.cpp
│   └── CMakeLists.txt
├── JUCE/                   # JUCE framework (submodule)
├── CMakeLists.txt          # Root CMake configuration
└── README.md
```

## Future Enhancements

- Add more sophisticated message handling and routing
- Implement bidirectional OSC communication (UI changes send OSC messages)
- Add preset save/load functionality
- Support for additional control types

## License

This is a demonstration project for educational purposes.
