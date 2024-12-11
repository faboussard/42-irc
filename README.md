# ft_irc

## 📌 Project Overview

This project consists of an IRC server and a bot that interacts with the server.   
Server is responsible for managing incoming messages, parsing commands, and handling communication between Clients and Channels. Clients in this context refer to external IRC clients (e.g., HexChat), while the server tracks the Client information.   
The Bot operates independently of the server and behaves like a client, responding to specific commands.

---
## 📌 Usage

### IRC Server

```
make
./ircserv 6667 pass
```

### IRC Bot

```
make bot
./ircbot 6667 pass
```
From client's interface (like HexChat)
```
/privmsg KawaiiBot :!command
```

To use `!weather` command, an API key for [Weather API](https://www.weatherapi.com/) is required.
The key should be set in `.env` file as `WEATHER_API_KEY=key` at the root of the project.

---
## 📌 IRC Server

### Communication flow

The server uses sockets for communication with clients, providing a reliable method for sending and receiving messages over a network.   
When the server starts, it opens a socket to listen for incoming client connections on a specified port. Once a client connects, the server creates a communication channel using the socket, enabling the exchange of data (e.g., IRC commands).
The server then processes these messages via a polling mechanism that ensures active management of incoming requests without blocking.

For a detailed explanation of the server's socket usage and polling mechanism, refer to [Server mounting with polling](./assets/MountingAnIRCServerWithPolling.md).

### Supported commands

The IRC Bot supports the following commands, which are implemented in separate source files:
- `/pass`: Handle server password.
- `/nick`: Register or change the nickname.
- `/user`: Set user information.
- `/join`: Join a specified channel.
- `/privmsg`: Send a private message to a user or channel.
- `/mode`: Set channel modes (i, t, k, l, o).
- `/invite`: Invite a user to a channel.
- `/topic`: Change or retrieves the current channel topic.
- `/kick`: Kicks a user from a channel.
- `/list`: List available channels.
- `/who`: List the users in a specific channel.
- `/ping`: Send a ping to the server.
- `/part`: Leave a channel.
- `/quit`: Disconnect from the server.    

For detailed information on each command, refer to their respective source files.


## 📌 IRC Bot

The IRC Bot operates independently of the server and interacts with the server as a client.   
It listens for specific commands and responds accordingly integrating external APIs to provide services.

For detailed implementation and features of the bot, refer [here](./assets/bot.md)

## 📌 Log

The Log class provides a static logging mechanism to record messages with various severity levels (DEBUG, INFO, NOTIFY, WARNING, ERROR) and contexts (e.g., SYSTEM, COMMAND, CLIENT). It formats logs with a timestamp and color-coded output for better clarity, supporting both standard output and error output for different log levels.

For more details, refer [here](./assets/log.md)

---
## 📌 License

- This project is created as part of a 42 School curriculum.
- You are allowed to use, test, and review the project.
- You may **NOT** copy, distribute, or make any public relations efforts regarding this project.
- Any attempt to reuse the code may be considered as academic misconduct.
- This project is intended solely for academic use and not for external deployment or further modification.
