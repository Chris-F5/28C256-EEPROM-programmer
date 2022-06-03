from eeprom_lib import *

DAT_FILE_NAME = "eeprom.bin"

print("reading file...")
datFile = open(DAT_FILE_NAME, "rb")
data = datFile.read()
datFile.close()

if len(data) != EEPROM_SIZE:
    print("invalid binary file size. exiting...")
    exit(1)

print("writing eeprom...")
for block in range(BLOCK_COUNT):
    block_bytes = data[block * BLOCK_SIZE : block * BLOCK_SIZE + BLOCK_SIZE]
    write_block(block, block_bytes)

print("closing serial connection...")
arduino.close()
