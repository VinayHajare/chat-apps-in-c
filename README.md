# 🗨️ C Chat Applications Collection

This repository contains three different implementations of chat applications in C, ranging from simple to more complex. Each application demonstrates different networking concepts and chat functionalities.

## 📚 Table of Contents

1. [📁 Repository Structure](#repository-structure)
2. [💬 Applications](#applications)
   - [Simple UDP Peer-to-Peer Chat](#simple-udp-chat)
   - [Group UDP Chat](#group-udp-chat)
   - [Peer-to-Peer Whatsapp style Chat](#p2p-style-chat)
3. [🛠️ Building and Running](#building-and-running)
4. [📚 Dependencies](#dependencies)
5. [🤝 Contributing](#contributing)
6. [📄 License](#license)

## 📁 Repository Structure

```
ChatApp/
├── Peer-to-Peer Chat APP/
│   ├── chat_server.c
│   └── chat_client.c
├── Group Chat APP/
│   ├── group_chat_server.c
│   └── group_chat_client.c
├── Peer-to-Peer WhatsAPP/
│   ├── chat_shared.h
│   ├── chat_server.c
│   └── chat_client.c
└── README.md
```

## 💬 Applications

### Simple UDP Peer-to-Peer Chat

A basic client-server chat application using UDP sockets. This application demonstrates one-to-one communication between a server and a client.

**Features:**
- 🔄 Turn-based communication
- 🔌 Simple connection handling
- ✉️ Basic message exchange

### Group UDP Chat

An extension of the simple UDP chat, allowing multiple clients to communicate in a group setting through a central server.

**Features:**
- 👥 Multiple client support
- 🔊 Message broadcasting
- 🆔 Client ID assignment
- 🚪 Join and leave notifications

### Peer-to-Peer Whatsapp style Chat

A more advanced chat application mimicking some features of messaging apps like WhatsApp. It uses a central server for user management and message routing but allows direct communication between users.

**Features:**
- 📝 User registration and login
- 👤 Online user list
- 💌 Direct messaging between users
- 🔒 Basic error handling

## 🛠️ Building and Running

### Prerequisites

- GCC compiler
- POSIX-compliant operating system (Linux, macOS, or Windows with WSL)
- `pthread` library

### Compilation

Navigate to the directory of the chat application you want to build and use the following commands:

For Simple UDP Chat and Group UDP Chat:
```bash
gcc group_chat_server.c -o server
gcc group_chat_client.c -o client
```

For P2P-style Chat:
```bash
gcc chat_server.c -o server -pthread
gcc chat_client.c -o client -pthread
```

### Running the Applications

1. Start the server:
   ```bash
   ./server
   ```

2. In separate terminal windows, start one or more clients:
   ```bash
   ./client
   ```

3. Follow the on-screen prompts to use the chat application.

## 📚 Dependencies

- Standard C libraries
- POSIX sockets
- pthread library (for P2P-style chat)

## 🤝 Contributing

Contributions to improve the chat applications or add new features are welcome! Please follow these steps:

1. Fork the repository
2. Create a new branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Happy chatting! 💻💬🎉
