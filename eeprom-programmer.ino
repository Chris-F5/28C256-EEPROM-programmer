#define GPIO_SHIFT_DAT 2
#define GPIO_SHIFT_CLK 3
#define GPIO_SHIFT_LATCH 4
#define GPIO_WRITE_ENABLE 5
#define GPIO_D0 6
#define GPIO_D1 7
#define GPIO_D2 8
#define GPIO_D3 9
#define GPIO_D4 10
#define GPIO_D5 11
#define GPIO_D6 12
#define GPIO_D7 13

#define EEPROM_SIZE 32768
#define BLOCK_SIZE 512

void setShiftRegisters(unsigned int address, bool outputEnable)
{
    byte flags = outputEnable ? 0 : 1;
    byte addLow = address;
    byte addHigh = (address >> 8) & 0b01111111;
    addHigh |= outputEnable ? 0 : 0b10000000;
    shiftOut(GPIO_SHIFT_DAT, GPIO_SHIFT_CLK, MSBFIRST, addHigh);
    shiftOut(GPIO_SHIFT_DAT, GPIO_SHIFT_CLK, MSBFIRST, addLow);
    digitalWrite(GPIO_SHIFT_LATCH, LOW);
    digitalWrite(GPIO_SHIFT_LATCH, HIGH);
    digitalWrite(GPIO_SHIFT_LATCH, LOW);
}

byte readByteEEPROM(int address)
{
    pinMode(GPIO_D0, INPUT);
    pinMode(GPIO_D1, INPUT);
    pinMode(GPIO_D2, INPUT);
    pinMode(GPIO_D3, INPUT);
    pinMode(GPIO_D4, INPUT);
    pinMode(GPIO_D5, INPUT);
    pinMode(GPIO_D6, INPUT);
    pinMode(GPIO_D7, INPUT);

    setShiftRegisters(address, true);
    byte data = 0;
    data = (data << 1) + digitalRead(GPIO_D7);
    data = (data << 1) + digitalRead(GPIO_D6);
    data = (data << 1) + digitalRead(GPIO_D5);
    data = (data << 1) + digitalRead(GPIO_D4);
    data = (data << 1) + digitalRead(GPIO_D3);
    data = (data << 1) + digitalRead(GPIO_D2);
    data = (data << 1) + digitalRead(GPIO_D1);
    data = (data << 1) + digitalRead(GPIO_D0);
    return data;
}

void writeByteEEPROM(unsigned int address, byte data)
{
    while(readByteEEPROM(address) != data) {
        setShiftRegisters(address, false);
        pinMode(GPIO_D0, OUTPUT);
        pinMode(GPIO_D1, OUTPUT);
        pinMode(GPIO_D2, OUTPUT);
        pinMode(GPIO_D3, OUTPUT);
        pinMode(GPIO_D4, OUTPUT);
        pinMode(GPIO_D5, OUTPUT);
        pinMode(GPIO_D6, OUTPUT);
        pinMode(GPIO_D7, OUTPUT);

        digitalWrite(GPIO_D0, data & 1);
        digitalWrite(GPIO_D1, data & 2);
        digitalWrite(GPIO_D2, data & 4);
        digitalWrite(GPIO_D3, data & 8);
        digitalWrite(GPIO_D4, data & 16);
        digitalWrite(GPIO_D5, data & 32);
        digitalWrite(GPIO_D6, data & 64);
        digitalWrite(GPIO_D7, data & 128);
        digitalWrite(GPIO_WRITE_ENABLE, LOW);
        delayMicroseconds(1);
        digitalWrite(GPIO_WRITE_ENABLE, HIGH);
        delay(12);
    }
}

void printEEPROM()
{
    unsigned int i = 0;
    while(i < EEPROM_SIZE) {
        byte data = readByteEEPROM(i);
        char hex[2];
        sprintf(hex, "%02X", data);
        Serial.print(hex);
        Serial.print(' ');
        if (i % 16 == 15) {
            Serial.print("| ");
            Serial.print(i - 15);
            Serial.print(" to ");
            Serial.print(i);
            Serial.print(" / 32768\n");
        }
        i += 1;
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(GPIO_SHIFT_DAT, OUTPUT);
    pinMode(GPIO_SHIFT_CLK, OUTPUT);
    pinMode(GPIO_SHIFT_LATCH, OUTPUT);

    digitalWrite(GPIO_WRITE_ENABLE, HIGH);
    pinMode(GPIO_WRITE_ENABLE, OUTPUT);

    Serial.begin(115200);
    Serial.setTimeout(3000);
}

void loop() {
    while(!Serial.available());
    String request = Serial.readStringUntil(':');
    String argument = Serial.readStringUntil('\n');

    if(request == "READ" and argument == "ALL") {
        printEEPROM();
        Serial.print("END_RESPONSE\n");
        return;
    } else if(request == "WRITE") {
        unsigned int block = argument.toInt();
        Serial.print("Block ");
        Serial.print(block);
        Serial.print(" selected. Awaiting bytes...\n");

        byte buffer[BLOCK_SIZE];
        int w = Serial.readBytes(buffer, BLOCK_SIZE);
        if (w != BLOCK_SIZE) {
            Serial.print("Failed to read bytes. Timed out. Aborting write.\n");
            Serial.print("END_RESPONSE\n");
            return;
        }
        Serial.print("Received bytes. Writing to EEPROM...\n");

        for(unsigned int i = 0; i < BLOCK_SIZE; i++) {
            writeByteEEPROM(i + BLOCK_SIZE * block, buffer[i]);
        }
        Serial.print("Finish writing to block ");
        Serial.print(block);
        Serial.print(".\n");
        Serial.print("END_RESPONSE\n");
        return;
    }
    Serial.print("Invalid request: ");
    Serial.print(request + ".\n" + argument + ".\n");
    Serial.print("END_RESPONSE\n");
}
