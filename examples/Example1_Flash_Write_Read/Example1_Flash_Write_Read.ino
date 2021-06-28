/*
 * Arduino-MX25xxxYY is an Arduino Library for the Macronix MX25R6435F flash chip.
 * Copyright (C) 2021 eResearch, James Cook University
 * Author: NigelB
 *
 * This program is free software: you can redistribute it and/or modify
 *         it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 *         but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Repository: https://github.com/jcu-eresearch/Arduino-MX25xxxYY
 *
 */

/*
 * This example demonstrates reading and writing to the Macronix MX25R6435F flash chip.
 * It starts by assuming that the first page is in the default state of all 0xFF
 * It reads out the first page and displays it, and then waits for user input to continue.
 * Then it creates a MX25R6435F page sized buffer of and sets the values of the buffer from 0 to PAGE_SIZE.
 * Then it writes this buffer to the first page, reads it back, and displays it.
 * We once again await use input before continuing.
 * Next we erasing the first 4KB block, and read back the first page.
 *
 * This example also assumes that the Macronix MX25R6435F flash chip is kept powered off and that its power
 * can be switched on and off with the FLASH_POWER_PIN
 */

#include "MX25xxxYY.h"
#include "ArduinoMX25xxxYY.h"

#define FLASH_RESET_PIN 27
#define FLASH_CS_PIN 28
#define FLASH_WP_PIN 4

#define FLASH_SPI_CLOCK_SPEED 4000000

ArduinoMX25xxxYY flash;

void hex_dump(size_t length, uint8_t *buf);
void clearBuf();


void setup()
{
//  Configure Hardware
    Serial.begin(115200);
    SPISettings _spi_settings(FLASH_SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0);
    SPI.beginTransaction(_spi_settings);
    flash.setDebugStream(&Serial); //Remove this line to disable ArduinoMX25Series DEBUG output.

//
    MX25xxxYY_Chip_Info_t *chip_def = &MX25R6435F_Chip_Def_Low_Power;
    bool found_flash = flash.begin(chip_def, FLASH_CS_PIN, FLASH_RESET_PIN, FLASH_WP_PIN, &SPI);
    if(found_flash)
    {

        Serial.println("Chip found!....");

        uint8_t buffer[chip_def->page_size];
        memset(buffer, 0, sizeof(buffer));

        Serial.println("Reading first Page...");
        ArduinoMX25xxxYY_status status = flash.readFlashMemory(0, sizeof(buffer), buffer);

        if(MX25xxxYY_HAS_ERROR(status))
        {
            Serial.println("Error reading data from Flash!");
            return;
        }

        //Then display the first page
        hex_dump(sizeof(buffer), buffer);

        //Then we set the values of the buffer to count from 0x00 to sizeof(buffer)
        for(size_t i = 0; i < sizeof(buffer); i++)
        {
            buffer[i] = i;
        }

        //Wait for user input to continue.
        clearBuf();
        Serial.println("Write Test");
        Serial.println("Press any key to continue...");
        while(!Serial.available()){}
        clearBuf();

        status = flash.writeFlashMemory(0, sizeof(buffer), buffer);

        if(MX25xxxYY_HAS_ERROR(status))
        {
            Serial.println("Error reading data from Flash!");
            return;
        }

        Serial.println("Successfully wrote first page.");

        Serial.println("Reading first Page...");

        //To ensure we are seing actual data from the flash chip clear the buffer.
        memset(buffer, 0, sizeof(buffer));
        status = flash.readFlashMemory(0, sizeof(buffer), buffer);

        if(MX25xxxYY_HAS_ERROR(status))
        {
            Serial.println("Error writing data to Flash!");
            return;
        }

        //If this displays all 0x00 then we failed to read data from the flash.
        hex_dump(sizeof(buffer), buffer);

        clearBuf();
        Serial.println("Erase Test - First 4KB Block");
        Serial.println("Press any key to continue...");
        while(!Serial.available()){}
        clearBuf();

        status = flash.eraseFlashMemory(0, MX25xxxYY_Erase_Block_4K);

        if(MX25xxxYY_HAS_ERROR(status))
        {
            Serial.println("Error erasing Flash!");
            return;
        }

        Serial.println("Successfully erased first 4KB block.");

        //To ensure we are seeing actual data from the flash chip clear the buffer.
        memset(buffer, 0, sizeof(buffer));

        Serial.println("Reading first Page...");
        status = flash.readFlashMemory(0, sizeof(buffer), buffer);

        if(MX25xxxYY_HAS_ERROR(status))
        {
            Serial.println("Error writing data to Flash!");
            return;
        }

        //If this displays all 0x00 then we failed to read data from the flash.
        //If this displays incrementing numbers (0x01 0x02 0x03 ...) then we failed to erase the flash.
        //If this displays all 0xFF then we succeeded in erasing the flash.
        hex_dump(sizeof(buffer), buffer);

    }else
    {
        Serial.println("Error! Could not find MX25R6435F chip.");
    }
    Serial.println("Powering chip off...");
}


void loop()
{

}

/**
 * clearBuf removes everything the the Serial input buffer
 */
void clearBuf()
{
    while(Serial.available())
    {
        Serial.read();
    }
}

/**
 * padded_hex creates a string of arbitrarily padded hex values.
 * Some systems don't have a proper printf implementation that honours formats such as: %08X.
 * This function implements such functionality.
 * @param val The value to represent in HEX
 * @param pad_char The character to use for padding
 * @param pad_width The width with which to pad too
 * @param length The length of the output buffer
 * @param buffer The output buffer
 */
void padded_hex(unsigned int val, char pad_char, size_t pad_width, size_t length, char* buffer)
{
    char tmp_buf[64] = {0};
    memset(tmp_buf, pad_char, sizeof (tmp_buf) - 1);
    memset(buffer, 0, length);
    int len = snprintf(tmp_buf, sizeof(tmp_buf), "%X", val);
    int diff = (int)pad_width - len;
    if(diff > 0)
    {
        memset(buffer, '0', diff);
        if((len + diff) < length ) {
            strncpy(buffer + diff, tmp_buf, len);
        }
    }else
    {
        strncpy(buffer, tmp_buf, len);
    }
}

/**
 * hex_dump displays a buffer as
 * @param length
 * @param buf
 *
 * Example: a 256 byte array with values from 0 to 256:
 *
 * Base Addr. | 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10
 * -----------+------------------------------------------------
 * 0x00000000 | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
 * 0x00000010 | 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
 * 0x00000020 | 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F
 * 0x00000030 | 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F
 * 0x00000040 | 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F
 * 0x00000050 | 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F
 * 0x00000060 | 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F
 * 0x00000070 | 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F
 * 0x00000080 | 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F
 * 0x00000090 | 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F
 * 0x000000A0 | A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF
 * 0x000000B0 | B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF
 * 0x000000C0 | C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF
 * 0x000000D0 | D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF
 * 0x000000E0 | E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF
 * 0x000000F0 | F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF
 *
 */
void hex_dump(size_t length, uint8_t *buf)
{
    Serial.println();
    Serial.println("Base Addr. | 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10");
    Serial.println("-----------+------------------------------------------------");
    for(size_t i = 0; i < length; i++)
    {
        if(i % 16 == 0) {
            char padding_buf[9] = {0};
            padded_hex(i, '0', 8, sizeof(padding_buf), padding_buf);
            Serial.printf("0x%s | ", padding_buf);
        }

        char padding_buf[9] = {0};
        padded_hex(buf[i], '0', 2, sizeof(padding_buf), padding_buf);
        Serial.printf("%s ", padding_buf);
        if(i % 16 == 15){Serial.println();}
    }
    Serial.println();
}
