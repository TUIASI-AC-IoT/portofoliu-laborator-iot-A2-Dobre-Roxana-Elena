import socket
import time

# Completati cu adresa IP a platformei ESP32
PEER_IP = "192.168.89.50"
PEER_PORT = 10001

MESSAGE = b"Salut!"
LED_OFF = b"GPIO4=0"
LED_ON  = b"GPIO4=1"

i = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while 1:
    # Modify the existing application to enable LED control through UDP messages. The
    # LED is connected to GPIO4, and its state will be toggled based on received datagrams
    try:
        if i % 2:
            TO_SEND = LED_ON
            sock.sendto(TO_SEND, (PEER_IP, PEER_PORT))
        else:
            TO_SEND = LED_OFF
            sock.sendto(TO_SEND, (PEER_IP, PEER_PORT))

        print("Am trimis mesajul: ", TO_SEND)
        i = i + 1
        time.sleep(1)
    except KeyboardInterrupt:
        break
        
