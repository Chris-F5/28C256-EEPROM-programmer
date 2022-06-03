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

byte readEEPROM(int address)
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

void writeEEPROM(unsigned int address, byte data)
{
    if(readEEPROM(address) == data) {
        return;
    }

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
    delay(10);
}

void printEEPROM()
{
    unsigned int i = 0;
    while(i < EEPROM_SIZE) {
        byte data = readEEPROM(i);
        char hex[2];
        sprintf(hex, "%02X", data);
        Serial.print(hex);
        Serial.print(" ");
        if (i % 16 == 15) {
            Serial.print("| ");
            Serial.print(i);
            Serial.print(" / 32768\n");
        }
        i += 1;
    }
}

void fillEEPROM(byte data[])
{
    Serial.print("\n");
    for(unsigned int i = 0; i < EEPROM_SIZE; i++) {
        writeEEPROM(i, data[i]);
    }
    Serial.print("\n");
    delay(500);
}

void setup()
{
    Serial.begin(57600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(GPIO_SHIFT_DAT, OUTPUT);
    pinMode(GPIO_SHIFT_CLK, OUTPUT);
    pinMode(GPIO_SHIFT_LATCH, OUTPUT);

    digitalWrite(GPIO_WRITE_ENABLE, HIGH);
    pinMode(GPIO_WRITE_ENABLE, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);

    // handshake
    while (true){
        String in = Serial.readStringUntil("\n");
        Serial.println("OK: " + in);
        delay(10);
        if(in.length() > 3) {
            break;
        }
    }


    //printEEPROM();
    /*byte data[EEPROM_SIZE];
    for(unsigned int i = 0; i < EEPROM_SIZE; i++) {
        data[i] = 0xEA;
    }

    fillEEPROM(data);

    printEEPROM();
    */
}

unsigned int i = 0;
void loop()
{

    if(Serial.available() > 0) {
        byte data = Serial.read();
        writeEEPROM(i, data);
        i++;
    } else {
        Serial.println("More Please");
        delay(1000);
    }
    /*
    if(Serial.available() && i < EEPROM_SIZE) {
        if(i == 0) {
            Serial.println("Writing EEPROM");
            for (unsigned int i = 0; i < EEPROM_SIZE / 512; i ++) {
                Serial.print("_");
            }
            Serial.print("\n");
        }
        char data[512];
        Serial.readBytes(data, 512);
        for(unsigned int x = 0; x < 512; x++) {
            writeEEPROM(i, data[x]);
            i++;
        }
        Serial.print(".");
        
        if(i >= EEPROM_SIZE) {
            Serial.print("\n");
            Serial.println("Write done.");
            digitalWrite(LED_BUILTIN, LOW);
            delayMicroseconds(100);
            printEEPROM();
        }
    }
    */
    /*
    delay(10000);
    digitalWrite(LED_BUILTIN, HIGH);
    setShiftRegisters(4, true);
    digitalWrite(LED_BUILTIN, LOW);
    delay(10000);
    setShiftRegisters(2, false);
    digitalWrite(LED_BUILTIN, HIGH);
    */
}
