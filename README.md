# OSC Demo

A simple C++ application demonstrating Open Sound Control (OSC) host functionality, compatible with the `oscsend` command-line utility.

## Overview

This project implements a basic OSC server that listens for incoming OSC messages and responds accordingly. The initial implementation supports a ping/pong functionality to demonstrate basic OSC communication.

## Features

- **Ping/Pong**: Send a `/ping` message and receive a `/pong` response
- **Generic Message Handler**: Logs any unmatched OSC messages
- **Graceful Shutdown**: Handles SIGINT and SIGTERM signals
- **Thread-based Server**: Uses liblo's server thread for concurrent message handling

## Requirements

- CMake 3.10 or higher
- C++11 compatible compiler (g++, clang++)
- liblo (Lightweight OSC library)
- oscsend utility (from liblo-tools package)

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install cmake g++ liblo-dev liblo-tools
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

The executable `osc_host` will be created in the `build` directory.

## Running

Start the OSC host:
```bash
./build/osc_host
```

The server will listen on port 7770 by default.

## Testing

In another terminal, send a ping message using `oscsend`:
```bash
oscsend localhost 7770 /ping
```

You should see output in the server terminal indicating:
- Message received
- Pong response sent

### Additional Test Commands

Send a custom message:
```bash
oscsend localhost 7770 /test s "hello"
```

## Usage Examples

The server currently supports the following OSC addresses:

- `/ping` - Responds with a `/pong` message back to the sender
- Any other address - Logged as an unhandled message

## Future Enhancements

- Add 5-6 simple commands to further demonstrate OSC's potential
- Implement a JUCE application with sliders and toggles controllable via OSC
- Add more sophisticated message handling and routing

## License

This is a demonstration project for educational purposes.
