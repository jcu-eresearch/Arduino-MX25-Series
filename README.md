# Arduino-MX25-Series
Arduino-MX25-Series is an Arduino wrapper for the [c-MX25-Series](https://github.com/jcu-eresearch/c-MX25-Series) library.

A very brief example:
```cpp

#include "MX25Series.h"
#include "ArduinoMX25Series.h"

#define FLASH_RESET_PIN 27
#define FLASH_CS_PIN 28
#define FLASH_WP_PIN 4

ArduinoMX25Series flash;

void setup()
{
    Serial.begin(115200);
    bool found_flash = flash.begin(&MX25R6435F_Chip_Def_Low_Power, FLASH_CS_PIN, FLASH_RESET_PIN, FLASH_WP_PIN, &SPI);
    if(found_flash)
    {
        Serial.println("Chip found!....");
    }
}

void loop()
{
}
```

A more extensive [example](https://github.com/jcu-eresearch/Arduino-MX25-Series/blob/master/examples/Example1_Flash_Write_Read/Example1_Flash_Write_Read.ino).
