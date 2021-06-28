/*
 * Arduino-MX25xxxYY is an Arduino Library for the Macronix MX25R6435F flash chip.
 * Copyright (C) 2021  eResearch, James Cook University
 * Author: NigelB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Repository: https://github.com/jcu-eresearch/Arduino-MX25xxxYY
 *
 */


#ifndef ARGOSTAG_ARDUINOMX25XXXYY_H
#define ARGOSTAG_ARDUINOMX25XXXYY_H

#include "SPI.h"
#include "MX25xxxYY.h"

#ifndef ArduinoMX25xxxYY_DEBUG_UART_TYPE
#define ArduinoMX25xxxYY_DEBUG_UART_TYPE UART
#endif

typedef uint64_t ArduinoMX25xxxYY_status;


class ArduinoMX25xxxYY {
private:
    MX25xxxYY_t dev = {};
    int manufacturer_id = 0;
    int memory_type = 0;
    int memory_density = 0;
    ArduinoMX25xxxYY_DEBUG_UART_TYPE *debug_stream = nullptr;

    ArduinoMX25xxxYY_status waitForWriteEnableBit(bool to_set, uint32_t max_expected_time_us);
    ArduinoMX25xxxYY_status waitForWriteInProgressBit(bool to_set, uint32_t max_expected_time_us);

public:


    /**
     * begin initializes the ArduinoMX25xxxYY object. It configures the specified pins as OUTPUT and sets their values
     * to the required defaults. It also reads the IDs from the flash chip and ensures that they match those in the
     * specified chip_def. begin also ensure that the MX25xxxYY___test_linker function returns true; ensuring that the
     * ArduinoMX25xxxYY library is properly linked with the c-MX25xxxYY library.
     * @param chip_def the populated MX25xxxYY_Chip_Info_t that matches the attached Flash Chip.
     * @param cs_pin the pin to use for the CS signal
     * @param reset_pin the pin to use for the RESET signal
     * @param wp_pin the pin to use for the WP signal
     * @param spi the SPI (object/bus) to use to communicate with the chip
     * @return True if the chip IDs match those supplied by the chip_def
     */
    bool begin(MX25xxxYY_Chip_Info_t *chip_def, uint8_t cs_pin, uint8_t reset_pin, uint8_t wp_pin, SPIClass *spi);

    /**
     * setDebugStream sets the Stream like object that is used to print out debug messages.
     * Example: flash.setDebugStream(&Serial);
     * @param stream the stream to use to output the debug messages.
     */
    void setDebugStream(ArduinoMX25xxxYY_DEBUG_UART_TYPE *stream);

    /**
     * readIdentities reads the IDs from the flash chip and stores the values for retrieval by getManufacturerId(),
     * getMemoryType(), and getMemoryDensity()
     * @return Flash Status codes
     */
    ArduinoMX25xxxYY_status readIdentities();

    /**
     * getManufacturerId returns the Manufacture ID component of the IDs retrieved by readIdentities();
     * @return The Manufacturer ID for the chip
     */
    int getManufacturerId() const;

    /**
     * getMemoryType returns the Memory Type component of the IDs retrieved by readIdentities();
     * @return The Memory Type for the chip
     */
    int getMemoryType() const;

    /**
     * getMemoryDensity returns the Memory Density component of the IDs retrieved by readIdentities();
     * @return The Memory Density for the chip
     */
    int getMemoryDensity() const;

    /**
     * readFlashMemory reads data stored in the flash chip.
     * @param memory_address The memory Address to start reading from.
     * @param length The size of the buffer we are reading data into.
     * @param buffer The buffer to read the data into.
     * @param fast_mode Use the fast mode for reading?
     * @return Flash Status codes
     */
    ArduinoMX25xxxYY_status readFlashMemory(uint32_t memory_address, size_t length, uint8_t *buffer, bool fast_mode=false);

    /**
     * writeFlashMemory writes the provided data to the specified memory location in the flash.
     * @param memory_address The memory_address to write the data to.
     * @param length The length of the data buffer we are writing.
     * @param buffer The data buffer we will write to the flash chip.
     * @param block If true, this method will wait until the Write In Progress (WIP) bit of the status register is zero.
     * @return Flash Status codes
     */
    ArduinoMX25xxxYY_status writeFlashMemory(uint32_t memory_address, size_t length, uint8_t *buffer, bool block = true);

    /**
     * eraseFlashMemory erases the specified memory block.
     * @param block_memory_address the memory address to erase. Any address falling into a block will cause the entire block to be erased.
     * @param block_size
     * @param block If true, this method will wait until the Write In Progress (WIP) bit of the status register is zero.
     * @return Flash Status codes
     */
    ArduinoMX25xxxYY_status eraseFlashMemory(uint32_t block_memory_address, MX25xxxYY_Erase_enum_t block_size, bool block = true);

    /**
     * resetChip asserts the reset pin for 100 ms
     */
    void resetChip();

    /**
     * getChipName gets the name from the configured MX25xxxYY_Chip_Info_t.
     * @return
     */
    const char* getChipName();

    /**
     * getChipDefinition return the pointer to the MX25xxxYY_Chip_Info_t supplied to the begin method.
     * @return the pointer to the MX25xxxYY_Chip_Info_t supplied to the begin method.
     */
    const MX25xxxYY_Chip_Info_t* getChipDefinition();

    /**
     * printf passes through to the Stream set by the setDebugStream method
     */
    template<typename... Args>
    int printf(const char * fmt, Args... args)
    {
        if(this->debug_stream != nullptr)
        {
            this->debug_stream->printf(fmt, args...);
        }
        return 0;
    }

    /**
     * println passes through to the Stream set by the setDebugStream method
     */
    template<typename Value>
    int println(const Value value)
    {
        if(this->debug_stream != nullptr)
        {
            this->debug_stream->println(value);
        }
        return 0;
    }

    /**
     * print passes through to the Stream set by the setDebugStream method
     */
    template<typename Value>
    int print(const Value value)
    {
        if(this->debug_stream != nullptr)
        {
            this->debug_stream->print(value);
        }
        return 0;
    }

    /**
     * print passes through to the Stream set by the setDebugStream method
     */
    template<typename Value, typename V>
    int print(const Value value, V type)
    {
        if(this->debug_stream != nullptr)
        {
            this->debug_stream->print(value, type);
        }
        return 0;
    }

    /**
     * println passes through to the Stream set by the setDebugStream method
     */
    int println();

};


#endif //ARGOSTAG_ARDUINOMX25XXXYY_H
