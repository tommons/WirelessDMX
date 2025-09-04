#!/usr/bin/env python3

import socket

IP = "0.0.0.0"
PORT = 4444

addrs = list(range(1,16)) + list(range(353,356))

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IP,PORT))

while True:
    data,addr = sock.recvfrom(2048)
    outstr = ""
    count = len(data)
    outstr = str(count) + " " + "{:03d}".format(data[0]) + " "
    for i in addrs:
        outstr = outstr + f"{data[i]:#0{4}x}" + " "
    print(outstr)
