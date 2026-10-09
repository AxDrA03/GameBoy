#include <iostream>
#include <vector>
#include <iomanip>

#include "src/cpu.hpp"
#include "src/MemoryBus.hpp"



int main (){
    cpu GameBoy;
    MemoryBus memory; 

    std::vector <uint8_t> test_program = {
        0x00,              // NOP
        0x01, 0x34, 0x12,  // LD BC, 0x1234
        0x04,              // INC B
        0x05,              // DEC B
        0x06, 0xFF         // LD B, 0xFF
    };

    memory.load_rom(test_program.data(), test_program.size()); 

    std::cout<<"INICIANDO PRUEBA\n";
    
    // Ejecutamos 5 instrucciones
    for (int i = 0; i < 5; ++i) {
        uint16_t pc_before = GameBoy.PC;
        
        GameBoy.fetch(memory);
        GameBoy.execute(memory);

        std::cout << "Step " << i + 1 << ": "
                  << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << pc_before
                  << " | Opcode executed: 0x" << std::setw(2) << (int)GameBoy.current_op_code
                  << " | BC: 0x" << std::setw(4) << GameBoy.BC
                  << " | Cycles: " << std::dec << GameBoy.cycles << "\n";
    }




    return 0;       
}

