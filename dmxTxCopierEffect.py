import socket
import time
from dmxSend import DmxSend

if __name__ == '__main__':

    dmxSend = DmxSend()

    startAddr = 1
    nStrip = 11
    speedFwd = 0.125/2
    speedRev = 0.05
    brightness = 255
        
    while(True):
        for i in range(nStrip):
            dmxSend.zero(startAddr, nStrip)
            dmxSend.set(startAddr+i, brightness)
            time.sleep(speedFwd)
            
        for i in range(nStrip-2,1,-1):
            dmxSend.zero(startAddr, nStrip)
            dmxSend.set(startAddr+i, brightness)
            time.sleep(speedRev)
            
    ...
    nStrip = 12
    speed = 0.125/2
    
    mode = 4

    weightArray = bytearray(nStrip)
    for i in range(nStrip):
        pass
    
    send_broadcast_message(my_byte_array)

    while(mode == 1 and True):
        for i in range(nStrip):
            for j in range(nStrip):
                my_byte_array[j+1] = 0
            my_byte_array[i+1] = 255
            send_broadcast_message(my_byte_array)
            time.sleep(speed)
        for i in range(nStrip-2,1,-1):
            for j in range(nStrip):
                my_byte_array[j+1] = 0
            my_byte_array[i+1] = 255
            send_broadcast_message(my_byte_array)
            time.sleep(0.05)
            
    nOff = 8
    while(mode == 2  and True):
        for i in range(nStrip):
            for j in range(nStrip):
                my_byte_array[j+1] = 255
            for j in range(nOff):
                my_byte_array[( (i+j) % nStrip ) + 1] = 0
                
            send_broadcast_message(my_byte_array)
            time.sleep(speed)
            

    if mode == 3:
        for i in range(3,8):
            my_byte_array[i+1] = 255
            send_broadcast_message(my_byte_array)
            time.sleep(speed)
            
    while(mode == 4 and True):
        for i in range(nStrip):
            
            
            # zero everything out
            for j in range(nStrip):
                my_byte_array[j+1] = 0
            
            my_byte_array[i+1] = 255
            send_broadcast_message(my_byte_array)
            time.sleep(speed)
            
            # zero everything out
            for j in range(nStrip):
                my_byte_array[j+1] = 0
                
            my_byte_array[i+1] = 128
            if i+2 < nStrip+1:
                my_byte_array[i+2] = 128
            
            send_broadcast_message(my_byte_array)
            time.sleep(speed)
            
            
        for i in range(nStrip-2,1,-1):
            for j in range(nStrip):
                my_byte_array[j+1] = 0
            my_byte_array[i+1] = 255
            send_broadcast_message(my_byte_array)
            time.sleep(0.05)
            
    ...