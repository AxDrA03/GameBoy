#include <iostream>
#include <vector>
#include <iomanip>
#include "src/cpu.hpp"
#include "src/MemoryBus.hpp"


int main() {
    cpu gameboy_cpu;
    MemoryBus memory;

    if (!memory.load_rom("../test/cpu_instrs.gb")) {
        return 1;
    }

    std::cout << "--- EJECUTANDO PRUEBA BLARGG ---\n";

    while (true) {
        gameboy_cpu.fetch(memory);
        gameboy_cpu.execute(memory);
    }

    return 0;
}