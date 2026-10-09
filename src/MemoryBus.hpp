#pragma once 
#include <array> 
#include <cstdint>
#include <cstddef>

class MemoryBus
{
private:
    std::array<uint8_t, 0x10000> ram;
public:
    MemoryBus(); 
    uint8_t read_byte(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value); 

    void load_rom(const uint8_t* rom_data, std::size_t size);
};

