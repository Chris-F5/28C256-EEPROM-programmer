import serial
import time

TIMEOUT_SECONDS = 10
BAUDRATE = 115200
EEPROM_SIZE = 32768
BLOCK_SIZE = 512
BLOCK_COUNT = 64

arduino = serial.Serial(port="/dev/ttyACM0", baudrate=BAUDRATE, timeout=TIMEOUT_SECONDS)

# wait a bit for the connection to work
time.sleep(5)

def send_request(request, arg):
    arduino.write(bytes(request + ":" + arg + "\n", 'utf-8'))

def wait_and_print_response():
    response = b""
    while True:
        line = arduino.read_until(b"\n");
        if line == b"END_RESPONSE\n":
            break
        else:
            print(line.decode("iso-8859-1"), end="")
            response += line

    return response

def read_all():
    send_request("READ", "ALL")
    wait_and_print_response()

def write_block(block_index, block_bytes):
    if(len(block_bytes) != BLOCK_SIZE):
        print("invalid block size. exiting.")
        exit(1)

    send_request("WRITE", str(block_index))
    arduino.write(block_bytes)
    wait_and_print_response()

#while True:
#    request = input("Enter request: ").lower()
#    if request == "q" or request == "quit" or request == "exit":
#        arduino.close()
#        print("exiting...")
#        exit(0)
#    if(request == "read all"):
#        read_all()
#    elif(request == "write"):
#        block_bytes = bytearray([0xea] * BLOCK_SIZE)
#        write_block(0, block_bytes)
#    else:
#        print("invalid request")

