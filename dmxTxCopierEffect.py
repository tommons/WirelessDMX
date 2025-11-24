import socket
import time

# Define the broadcast address and port
BROADCAST_ADDRESS = '255.255.255.255'  # Or a specific subnet broadcast address like '192.168.1.255'
BROADCAST_PORT = 4444

nDmxBytes = 513
my_byte_array = bytearray(nDmxBytes)

def send_broadcast_message(message):
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Enable broadcasting on the socket
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    try:
        # Send the broadcast message
        sock.sendto(message, (BROADCAST_ADDRESS, BROADCAST_PORT))
        print(f"Sent broadcast to {BROADCAST_ADDRESS}:{BROADCAST_PORT}")
    except Exception as e:
        print(f"Error sending broadcast: {e}")
    finally:
        sock.close()

def zeroArray(data):
    for i in range(nDmxBytes):
        data[i] = 0
        
    return data

if __name__ == '__main__':
    
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
            
    # You can also send messages repeatedly in a loop
    # while True:
    #     send_broadcast_message("Another broadcast message!")
    #     time.sleep(5)