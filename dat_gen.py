EEPROM_SIZE = 32768
FILE_NAME = "eeprom.data"

data = bytearray()
for i in range(EEPROM_SIZE):
    data.append(0x99);

file = open(FILE_NAME, "wb")
file.write(data)
file.close()

