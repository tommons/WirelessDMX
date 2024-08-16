#!/usr/bin/env python3

import socket

IP = "0.0.0.0"
PORT = 4444

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IP,PORT))

while True:
    data,addr = sock.recvfrom(2048)
    print("bytes " + str(len(data)) + " count: " + str(data[0]))
    count = len(data)
    outstr = ""
    for i in range(count):
        outstr = outstr + f"{data[i]:#0{4}x}" + " "
        if (i+1) % 16 == 0:
            print(outstr)
            outstr = ""
            