EEPROM_SIZE = 32768
FILE_NAME = "eeprom.bin"

data = bytearray([0xEA] * EEPROM_SIZE)

data[0x7FFE] = 0x00
data[0x7FFF] = 0x80

file = open(FILE_NAME, "wb")
file.write(data)
file.close()

