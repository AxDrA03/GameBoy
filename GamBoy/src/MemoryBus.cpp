#include "MemoryBus.hpp" // O "MemoryBus.h" según el nombre de tu archivo

// 1. Constructor: Inicializa la memoria (RAM) a cero
MemoryBus::MemoryBus() {
    ram.fill(0x00);
}

// 2. Lectura de memoria
uint8_t MemoryBus::read_byte(uint16_t address) const {
    return ram[address];
}

// 3. Escritura en memoria
void MemoryBus::write_byte(uint16_t address, uint8_t value) {
    ram[address] = value;
}

// 4. Carga de la ROM
void MemoryBus::load_rom(const uint8_t* rom_data, std::size_t size) {
    // Evita desbordamiento si la ROM supera los 64 KB
    std::size_t bytes_to_copy = (size > ram.size()) ? ram.size() : size;

    for (std::size_t i = 0; i < bytes_to_copy; ++i) {
        ram[i] = rom_data[i];
    }
}