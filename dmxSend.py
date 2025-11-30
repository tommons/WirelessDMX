import socket
import time
import threading
import signal, sys

# Define the broadcast address and port
BROADCAST_ADDRESS = '255.255.255.255'  # Or a specific subnet broadcast address like '192.168.1.255'
BROADCAST_PORT = 4444
NUMDMXBYTES = 513
RUNNING = True

def signal_handler(sig, frame):
    global RUNNING
    print("\nCtrl+C detected via signal handler! Performing cleanup...")
    RUNNING = False
    # Add your cleanup code here
    sys.exit(0) # Exit the program after cleanup

signal.signal(signal.SIGINT, signal_handler)
    
class DmxSend:
    def __init__(self):
        self.byte_array = bytearray(NUMDMXBYTES)
        self.zero()
        
        # Create a UDP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Enable broadcasting on the socket
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.running = True
        self.sendRate = 1/44.0  # Send rate in seconds
        
        self.thread = threading.Thread(target=self.run)
        self.thread.start()
        
    def run(self):
        while self.running and RUNNING:
            self.send_broadcast_message()
            time.sleep(self.sendRate)
            
        print("Send thread exiting")
        
    def send_broadcast_message(self):
        try:
            # Send the broadcast message
            self.sock.sendto(self.byte_array, (BROADCAST_ADDRESS, BROADCAST_PORT))
            #print(f"Sent broadcast to {BROADCAST_ADDRESS}:{BROADCAST_PORT}")
        except Exception as e:
            print(f"Error sending broadcast: {e}")

    def zero(self, startIndex=0, count=NUMDMXBYTES):
        for i in range(startIndex, startIndex+count):
            self.byte_array[i] = 0
            
    def set(self, index, value):
        # 1-based indexing
        self.byte_array[index+1] = value
        
    def get(self, index):
        return self.byte_array[index+1]
    
    def stop(self):
        self.running = False
        self.thread.join()
        self.sock.close()
        
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