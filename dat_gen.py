EEPROM_SIZE = 32768
FILE_NAME = "eeprom.bin"

data = bytearray([0xEA] * EEPROM_SIZE)

data[0x7FFC] = 0x00
data[0x7FFD] = 0x80

file = open(FILE_NAME, "wb")
file.write(data)
file.close()

