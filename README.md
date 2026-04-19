# Ft_IRC-42

A lightweight, efficient IRC (Internet Relay Chat) server implementation in C++, built to RFC 1459 specifications with extended functionality including channel management, bot support, and multiplexed I/O operations.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Requirements](#system-requirements)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
- [API Reference](#api-reference)
- [Configuration](#configuration)
- [Development](#development)

## Overview

Ft_IRC-42 is a production-ready IRC server implementation designed to handle multiple concurrent client connections with efficient event-driven architecture. Built with strict C++98 standards compliance and comprehensive error handling, it provides a robust platform for real-time communication.

**Key Characteristics:**
- **Language:** C++98 (ISO/IEC 14882:1998)
- **Architecture:** Event-driven with poll-based multiplexing
- **Maximum Clients:** 100 concurrent connections
- **Memory Model:** Dynamic allocation with proper resource cleanup
- **Build Standards:** Strict compilation flags (-Wall -Wextra -Werror)

## Features

### Core IRC Functionality
- **User Authentication:** Secure password-based server access
- **Channel Management:** Create, join, leave, and manage IRC channels
- **Message Broadcasting:** Efficient message distribution to channel members
- **User Modes:** Support for channel-specific user privileges
- **Command Processing:** Full IRC command parsing and execution

### Server Capabilities
- **Multiplexed I/O:** Poll-based event handling for optimal resource utilization
- **Signal Handling:** Graceful shutdown on SIGINT, SIGTERM, SIGTSTP, SIGQUIT
- **Bot Integration:** Built-in IRC bot for channel management and automation
- **Buffer Management:** Separate send/receive buffers for each client connection
- **Connection Stability:** Automatic client timeout and cleanup

### Advanced Features
- **Dynamic Channel Creation:** Channels instantiated on-demand
- **Bot Auto-Join:** Automatic bot participation in new channels
- **Connection Persistence:** Robust handling of partial messages and network interruptions

## System Requirements

### Minimum Requirements
- **OS:** Linux/Unix-based systems
- **Compiler:** GCC/Clang supporting C++98 standard
- **C++ Standard Library:** Standard C++ library with networking support
- **Architecture:** x86_64 or compatible

### Network Requirements
- Available TCP port (no port < 1024 without elevated privileges)
- Network interface for listening

### Build Dependencies
- `make` (build automation)
- C++ compiler with C++98 support
- Standard POSIX development headers (`unistd.h`, `arpa/inet.h`, `poll.h`)

## Build Instructions

### Prerequisites
Ensure you have the required build tools installed:
```bash
sudo apt-get install build-essential g++ make  # Debian/Ubuntu
sudo yum install gcc-c++ make                  # CentOS/RHEL
```

### Compilation

**Standard Build:**
```bash
make
```

**Rebuild (clean and rebuild):**
```bash
make re
```

**Clean Object Files:**
```bash
make clean
```

**Remove All Artifacts:**
```bash
make fclean
```

**Output:** Executable binary named `server`

## Usage

### Starting the Server

```bash
./server <password> <port>
```

**Parameters:**
- `<password>` (string): Server authentication password for client connections
- `<port>` (integer): TCP port number to listen on (e.g., 6667 for standard IRC)

**Examples:**
```bash
# Standard IRC port with secure password
./server MySecurePassword 6667

# Custom port with simple password
./server irc_pass 8888

# Development/testing on high port
./server dev_password 9999
```

### Client Connection

Connect to the server using any IRC client:

```bash
irc://localhost:6667
# Authenticate with the password when prompted
```

**Popular IRC Clients:**
- irssi
- weechat
- hexchat
- nc (telnet-style)

### Graceful Shutdown

Send termination signal to the server:
```bash
# Interrupt signal
Ctrl+C

# Or use kill command
kill -SIGTERM <pid>
kill -SIGINT <pid>
```

The server will:
1. Receive the signal
2. Notify all connected clients
3. Properly close all connections
4. Free all allocated resources
5. Exit cleanly

## Architecture

### Design Pattern: Event-Driven I/O Multiplexing

```
┌─────────────────────────────────────┐
│   main() - Process Arguments        │
├─────────────────────────────────────┤
│   IRCServer::setupServer()          │
│   - Create socket                   │
│   - Bind to port                    │
│   - Listen for connections          │
├─────────────────────────────────────┤
│   IRCServer::run() - Main Loop      │
│   ├─ poll() - Wait for events       │
│   ├─ acceptClient() - New incoming  │
│   ├─ handleClient() - Process data  │
│   └─ executeCommand() - Act on cmd  │
├─────────────────────────────────────┤
│   Signal Handler - Graceful exit    │
└─────────────────────────────────────┘
```

### Component Interaction

```
Client (IRC Protocol)
    ↓
Network Socket → poll()
    ↓
Server::handleClient()
    ↓
Commands::CheckCommand() → Commands::ExecuteCommand()
    ↓
Channel/Client Objects
    ↓
Send buffers → Network Socket
    ↓
Response to Client
```

### Data Flow

1. **Receive:** Socket → `clientBuffers` map
2. **Parse:** Extract complete IRC commands from buffers
3. **Execute:** Match command to handler, execute logic
4. **Broadcast:** Write responses to `sendBuffers` map
5. **Transmit:** `sendBuffers` → Client sockets

## Project Structure

```
Ft_IRC-42/
├── README.md                 # This file
├── Makefile                  # Build configuration
├── main.cpp                  # Server entry point
├── include/                  # Header files
│   ├── Server.hpp           # Main server class
│   ├── Client.hpp           # Client representation
│   ├── Channel.hpp          # Channel management
│   ├── Commands.hpp         # Command processing
│   ├── Utils.hpp            # Utility functions
│   └── Bot.hpp              # Bot implementation (if present)
└── sources/                 # Implementation files
    ├── Server.cpp           # Server core logic
    ├── Client.cpp           # Client management
    ├── Channel.cpp          # Channel operations
    ├── Commands.cpp         # Command handlers
    ├── Utils.cpp            # Helper utilities
    └── bot.cpp              # Bot functionality
```

## Technical Details

### Socket Implementation
- **Protocol:** TCP/IP (IPv4)
- **Address Family:** AF_INET
- **Socket Type:** SOCK_STREAM
- **Multiplexing:** poll() system call
- **Max Poll Array Size:** 100 file descriptors

### Memory Management
- **Client Objects:** Dynamically allocated on connection, freed on disconnection
- **Channel Objects:** Created on-demand, persisted while members exist
- **Buffers:** Per-client send/receive buffers for incomplete message handling
- **Cleanup:** All resources released in destructor; signal handler ensures cleanup

### Protocol Compliance
- Supports IRC protocol commands
- Handles multi-line messages with proper buffering
- Implements channel modes and user privileges
- Maintains RFC 1459 core compatibility

### Performance Characteristics
- **Connection Handling:** O(1) socket acceptance
- **Client Lookup:** O(n) linear scan (acceptable for max 100 clients)
- **Message Dispatch:** O(n) for channel broadcast
- **Command Parsing:** O(1) hash-map lookup

### Buffer Management Strategy
```cpp
std::map<int, std::string> clientBuffers;   // Incomplete incoming data
std::map<int, std::string> sendBuffers;     // Outgoing data queue
```

Handles:
- Partial message assembly
- Out-of-order packet delivery
- Network congestion buffering

## API Reference

### Server Class

**Constructor:**
```cpp
IRCServer(const std::string &password, int port);
```

**Public Methods:**
```cpp
bool setupServer();              // Initialize server socket
void run();                       // Main event loop
void createBot();               // Instantiate bot user
void botJoinNewChannel(Channel *ch);  // Add bot to channel
std::map<std::string, Channel*>& getChannels();  // Access channels
void stop();                    // Graceful shutdown
```

### Client Class
```cpp
void setNickname(const std::string &nick);
void setUsername(const std::string &user);
int getFd() const;
std::string getNickname() const;
```

### Channel Class
```cpp
void addClient(Client *client);
void removeClient(Client *client);
void broadcast(const std::string &message);
void setTopic(const std::string &topic);
```

### Commands Class
```cpp
static std::string CheckCommand(int fd);
static void ExecuteCommand(int fd);
```

## Configuration

### Compile-Time Configuration

**Maximum Clients:** Modify in `Server.hpp`
```cpp
#define MAX_CLIENTS 100
```

**Compilation Flags:** Modify in `Makefile`
```makefile
CFLAGS = -Wall -Wextra -Werror -std=c++98
```

### Runtime Configuration

Only two parameters required at startup:
- Server password (string)
- Listening port (integer)

No configuration files needed; simple command-line interface.

## Development

### Building with Debugging
```bash
# Modify Makefile to add debug flags
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -O0
make re
```

### Testing Approach
```bash
# Terminal 1: Start server
./server testpass 6667

# Terminal 2: Connect with netcat
nc localhost 6667

# Terminal 3: Connect another client
nc localhost 6667
```
