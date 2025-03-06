TCP Server for Connection Testing
C++ version

Author: Vadim Sachkov
Date: 2025-02-06

1. Overview

This is a simple TCP server for connection testing. Clients can connect, send messages, and receive responses. The server also sends periodic time updates.

2. System Requirements

- Windows with Winsock (WS2_32.dll)
- MinGW (g++) or MSVC (Visual Studio) compiler
- Firewall must allow incoming connections on the selected port (default: 1088)
- For external access, configure port forwarding (dst-nat) on your router


3. Running the Server

Start the server:

tcpserver4test.exe [port]

By default, the server listens on port 1088. To use a custom port, specify it when starting:

tcpserver4test.exe 5555

4. Connecting to the Server

Clients can connect using Telnet or Netcat.

Telnet:

telnet SERVER_IP 1088

Replace SERVER_IP with the actual IP address.

Netcat (for local testing):

nc 127.0.0.1 1088

5. Commands and Server Interaction

q + Enter       Disconnect from the server
ESC             Immediate exit
Text + Enter    Server replies with "Received: text"
Long text (>100 chars)   Extra characters are ignored

The server automatically sends a timestamp every 5 seconds.

6. Stopping the Server

To stop the server, press Ctrl + C in the console or close the terminal.




7. Compilation Instructions

	 System Requirements

	- MinGW (g++) or MSVC (Visual Studio) compiler


MinGW (g++):

g++ -o tcpserver.exe tcpserver4test.cpp -lws2_32 -std=c++11

MSVC (Visual Studio):

cl /EHsc tcpserver4test.cpp ws2_32.lib
