import serial
import time
import random
import string

DAT_FILE_NAME = "eeprom.data"
BLOCK_SIZE = 1024
EEPROM_SIZE = 32768

datFile = open(DAT_FILE_NAME, "rb")
data = datFile.read()
datFile.close()

arduino = serial.Serial(port="/dev/ttyACM0", baudrate=57600, timeout=0.1)
print(arduino)

time.sleep(1)

def blockingRead():
    res = arduino.readline()
    while len(res) == 0:
        res = arduino.readline()
    return res

def handshake():
    while True:
        randomSignature = "".join(random.choices(string.ascii_uppercase, k=6))
        readyString = "READY? " + randomSignature + "\n"
        print(readyString)

        arduino.write(readyString.encode())
        time.sleep(2)
        response = arduino.readline()
        print(response)
        if response == ("OK: " + readyString).encode():
            print("handshake success")
            break
        else:
            print("retry handshake")

handshake()

i = 0
def sendBlock():
    global i
    x = 0
    while (i < EEPROM_SIZE and x < BLOCK_SIZE):
        arduino.write(data[i])
        x += 1
        i += 1

while i < EEPROM_SIZE:
    more = arduino.readline()
    print(more)
    print(i)
    if more == "More Please\r\n".encode():
        sendBlock()
        while True:
            none = arduino.readline()
            print(none)
            if len(none) == 0:
                break


arduino.close()
