# AetherLock Backend

A C++ backend daemon that locks and unlocks a workspace with password protection, and controls which applications can be launched through a whitelist system.

## Features

- **Workspace Locking** — Lock and unlock your workspace with a password
- **Application Control** — Launch only applications that are on an allowed list
- **IPC Interface** — Communicate with the daemon using JSON messages over Unix sockets
- **Background Monitoring** — Runs as a background service with automatic cleanup on shutdown

## Project Structure

```
aetherlock-backend/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/
│   └── nlohmann/
│       └── json.hpp        # JSON library
├── src/
│   ├── main.cpp            # Entry point
│   ├── core/
│   │   ├── Server.cpp      # Socket server
│   │   ├── Server.h
│   │   ├── RequestRouter.cpp
│   │   ├── RequestRouter.h
│   │   ├── Locker.cpp
│   │   └── Locker.h        # Lock/unlock state
│   ├── security/
│   │   ├── Whitelist.cpp
│   │   ├── Whitelist.h     # Allowed apps list
│   │   ├── Monitor.cpp
│   │   └── Monitor.h       # Background monitor
│   ├── ipc/
│   │   └── Protocol.h      # Request/response formats
│   └── util/
│       ├── Logger.cpp
│       └── Logger.h        # Logging utility
└── tests/
    └── CMakeLists.txt
```

## Requirements

- C++17 compatible compiler
- CMake 3.10 or higher
- Linux with Unix domain socket support

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

Run the daemon with root privileges (required for log and config paths):

```bash
sudo ./backend
```

The daemon starts listening on `/tmp/backend.sock` for incoming requests.

## API Reference

All requests and responses use JSON format over Unix domain sockets.

### Send a Request

```bash
echo '{"request_id": "1", "action": "lock", "params": {}}' | nc -U /tmp/backend.sock
```

### Lock Workspace

```json
// Request
{
  "request_id": "unique-id",
  "action": "lock",
  "params": {}
}

// Response
{
  "status": "success",
  "message": "Workspace locked",
  "data": {}
}
```

### Unlock Workspace

```json
// Request
{
  "request_id": "unique-id",
  "action": "unlock",
  "params": { "password": "secret" }
}

// Response
{
  "status": "success",
  "message": "Workspace unlocked",
  "data": {}
}
```

### Launch Application

```json
// Request
{
  "request_id": "unique-id",
  "action": "launchApp",
  "params": { "name": "firefox" }
}

// Response
{
  "status": "success",
  "message": "Application launched",
  "data": {}
}
```

## Configuration

### Allowed Applications

Edit `/etc/backend/allowed_apps.json` to configure which applications can be launched:

```json
{
  "allowed_apps": ["firefox", "code"]
}
```

If the file does not exist, the daemon defaults to allowing `firefox` and `code`.

### Default Password

The default password is `secret`. Change this in `src/core/Locker.cpp` before deployment.

# 
