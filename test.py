import socket

# This is just a socket for testing purposes, binds to the loopback device
sock =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_QUICKACK, 1)

sock.close()