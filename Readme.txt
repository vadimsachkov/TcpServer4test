TCP Server for Connection and Port Accessibility Testing  
C++ Version

Author: Vadim Sachkov  
Date: 2025-02-06

---

1. Overview

This is a simple multithreaded TCP server written in C++ for testing port accessibility and TCP connectivity.  
Its main purpose is to verify that a server is reachable via a specific TCP port — for example, to confirm proper router port forwarding, firewall configuration, or remote network access.

Clients can connect, send messages, and receive responses. The server also sends periodic time-based heartbeat messages, helping to monitor stable bidirectional communication.

Unlike ICMP ping, this server uses actual TCP connections, which makes it useful in environments where ICMP is blocked or unreliable.

---

2. Features

- Accepts up to 10 simultaneous TCP connections.
- Sends automatic timestamp messages every 5 seconds, if the client is idle.
- Echoes back client messages, limited to 100 characters.
- Supports clean client disconnect via "q" or the ESC key.
- Logs connection activity with client IP and port in the server console.

---

3. System Requirements

- Windows OS with Winsock (WS2_32.dll)
- MinGW (g++) or MSVC (Visual Studio) compiler
- Firewall must allow incoming connections on the selected TCP port (default: 1088)
- For external connections: ensure proper port forwarding (dst-nat) is configured on your router

---

4. Running the Server

Command:
    tcpserver4test.exe [port]

If no port is specified, the server defaults to port 1088.

Example:
    tcpserver4test.exe 5555

After launching, the server listens for incoming TCP connections and logs client events.

---

5. Connecting to the Server

Clients can use Telnet, Netcat, or custom TCP clients.

Telnet:
    telnet SERVER_IP 1088

Replace SERVER_IP with the actual IP address of the server.

Netcat (for local testing):
    nc 127.0.0.1 1088

---

6. Commands and Server Interaction

Client Input              | Server Response
--------------------------|-------------------------------
q + Enter                 | Closes the connection
ESC (ASCII 27)            | Immediately disconnects
Text + Enter              | Server replies with "Received: <your text>"
> 100 chars               | Input truncated to 100 characters
(no input, idle)          | Every 5 seconds, server sends a line:
                          | `<n> #<conn_id> <IP>:<port> - <timestamp>`

Example:
    3 #2 192.168.1.10:52918 - 2025-03-28 15:40:10

---

7. Stopping the Server

To stop the server, press Ctrl + C in the console or close the terminal window.

---

8. Compilation Instructions

MinGW (g++):

    g++ -o tcpserver.exe tcpserver4test.cpp -lws2_32 -std=c++11

MSVC (Visual Studio):

    cl /EHsc tcpserver4test.cpp ws2_32.lib

The compiled binary `tcpserver.exe` can then be launched from the command line.

---

9. Use Cases

- Testing if a port is open and reachable from a remote network
- Verifying NAT and port forwarding rules
- Diagnosing firewalls or ISP-level restrictions
- Monitoring TCP availability when ICMP ping is blocked
- Lightweight heartbeat mechanism for TCP health checking

---



