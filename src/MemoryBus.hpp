#pragma once 
#include <array> 
#include <cstdint>
#include <cstddef>
#include <iostream>

class MemoryBus
{
private:
    std::array<uint8_t, 0x10000> ram;
public:
    MemoryBus(); 
    uint8_t read_byte(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value); 

    void write_word(uint16_t address, uint16_t value);
    uint16_t read_word(uint16_t addess) const;

    bool load_rom(const std::string& filepath);
};

