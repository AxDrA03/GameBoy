#include "MemoryBus.hpp" // O "MemoryBus.h" según el nombre de tu archivo
#include <iostream>
#include <fstream>
#include <vector>

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
    if (address == 0xFF02 && value == 0x81) {
        char c = static_cast<char>(read_byte(0xFF01));
        std::cout << c << std::flush;
        ram[0xFF02] = 0;
        return;
    }

    ram[address] = value;
}

void MemoryBus::write_word(uint16_t address, uint16_t value){

    write_byte(address, static_cast<uint8_t>(value & 0xFF)); 
    write_byte(address+1, static_cast<uint8_t>(value >> 8)); 

}

uint16_t MemoryBus::read_word(uint16_t address) const{
    uint8_t low = read_byte(address); 
    uint8_t high = read_byte(address + 1);
    
    return (static_cast<uint16_t> (high)<<8) | low; 
}

// 4. Carga de la ROM
bool MemoryBus::load_rom(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir la ROM: " << filepath << "\n";
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        // Copiar el contenido cargado al arreglo de memoria (ROM bank 0/1)
        for (size_t i = 0; i < buffer.size() && i < ram.size(); ++i) {
            ram[i] = buffer[i];
        }
        std::cout << "ROM cargada con éxito (" << size << " bytes).\n";
        return true;
    }

    return false;
}