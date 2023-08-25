# Base64-Encoding

Assignment 1: Base64 encoding system using Client-Server socket programming 

This system includes server and client programs, which can communicate using TCP sockets. 
Aim - To implement a base64 encoding system during communication

The server here is a concurrent server, i.e., multiple clients can connect to the server at the same time and communicate with it.
The port number needs to be entered from the command line  and is not hard coded.

Steps To Run :

Prototypes to run programs-
Server: <executable code> <Server Port number> 
Client: <executable code> <Server IP Address> <Server Port number> 

1. Run the server.cpp file while passing the port number as an argument.
2. Run the client.cpp files by passing the server's predefined/known IP address and port number.
3. Now, the command line will ask whether the connection is to be terminated or not. If not, then it will ask for the message to be sent, which will
be encoded using the base64 encoding system and will send to the server.
4. The server will receive the encoded message and print the original message by decoding it on the command prompt. It will also send an acknowledgment
message to the client.
5. Communication continues till the client initiates termination of the connection.
Multiple clients can connect to the server at the same time and communicate with it.

Base64 Encoding System Description: 
Base64 encoding is used for sending a binary message over the net. In this scheme, groups of 24bit are broken into four 6-bit groups 
and each group is encoded with an ASCII character. 
For binary values 0 to 25, ASCII characters ‘A’ to ‘Z’ are used, followed by lowercase letters and 
the digits for binary values 26 to 51 & 52 to 61, respectively. 
Character ‘+’ and ‘/’ are used for binary values 62 & 63, respectively. 
In case the last group contains only 8 & 16 bits, then the “==” & “=” sequence is appended to the end. 
