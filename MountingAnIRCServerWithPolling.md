**IRC communication Diagram**
![alt text](image.png)

1. **Create the Server Socket (Listening):** ⚙️
   - Create a socket with the `socket()` system call. It will be used for the client to connect to the server, which will use `accept()`. 🖧
   - Add options (non-blocking, reuse address, etc.) to the socket using `setsockopt()` and `fcntl()`. 🔧
   - Use the `bind()` function to assign a specific address and port to the socket. 🌍
   - Use the `listen()` function to make the socket a “listening” socket. 📡

2. **Use `poll_fd` to Track Client Events:** 📝
   - Register the socket file descriptor in a new `pollfd` structure. This structure stores the file descriptors you want to monitor for events. 📑
   - Mark the events as `POLLIN` to listen for incoming data. 📥
   - The `revents` field will be used to store the events that are triggered. 🔄
   - Create a loop to monitor the events. There will be two event categories: accepting connections and handling clients. 🔄💻

3. **Create the Client Socket (Accept Connection):** 🤝
   - When a client tries to connect, the server stores its data using the `accept()` function. It takes the following parameters:
     - The file descriptor from the server socket (previously configured in listen mode). 💼
     - The address of the client (IP, port) stored in the `sockaddr_in` structure. 🗺️
   - `accept()` returns a new client socket file descriptor. This file descriptor is stored in the `pollfd` structure to be monitored by `poll()`. It will be used to send and receive data from the client. 📬

4. **Received New Data from a Registered Client:** 📥
   - Prepare a `char` array `buff` to store the received data (initialize the buffer with 0). 💾
   - Call `recv()` to receive the data from the client socket specified by the file descriptor. 📲
   - Use `send()` to send a numeric reply and make the client perform the corresponding action (e.g., as seen in HexChat). 📤
