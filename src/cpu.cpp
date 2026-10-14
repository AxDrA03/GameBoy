#include "cpu.hpp"

void cpu::reset(){
    AF = 0X01B0; 
    BC = 0X0013; 
    DE = 0X0013; 
    HL = 0X00D8; 
    SP = 0XFFFE; 
    PC = 0X0100; 
}

void cpu::fetch( MemoryBus &memory){
    current_op_code = fetch_byte(memory); 
}


void cpu::execute(MemoryBus &memory) {
    instrucction_counter++; 

    switch (current_op_code) {
        case 0x00: {cycles += 4; break;}// NOP (1 M-cycle)
        case 0x01: { ld_16_bits(BC, memory, 12); break;} // LD BC, d16 (3 M-cycles)
        case 0x02: {  memory.write_byte(BC, A); cycles += 8; break; } // LD (BC), A (2 M-cycles) - Escribe A en la memoria RAM en la dirección BC
        case 0x03: { inc_16_bits(BC, 8);break; } // INC BC (2 M-cycles) - Incremento de 16 bits (sin banderas)
        case 0x04: { B = inc_8_bits(B, 4); break; } // INC B (1 M-cycle) - Incremento de 8 bits (actualiza Z, N, H)
        case 0x05: { B = dec_8_bits(B, 4); break; }// DEC B (1 M-cycle) - Decremento de 8 bits (actualiza Z, N, H)
        case 0x06: { ld_8_bits(B, memory, 8); break; }// LD B, d8 (2 M-cycles) - Carga un byte inmediato de ROM a B
        case 0x07: {RLCA(); break;};
        case 0x08: {ld_a16_(SP, memory, 20);  break;} // 5 M-cycles; break;

       default:
            std::cerr << "Opcode NO implementado: 0x" 
                    << std::hex << (int)current_op_code 
                    << " en PC: 0x" << (PC - 1) << "\n";
            std::exit(1);
    }
}
void cpu::set_flag(uint8_t flag_mask, bool condition){
    if(condition)
        F |= flag_mask;
    else  
        F &= ~flag_mask;

    F &= 0xF0;
}

uint8_t cpu::fetch_byte(MemoryBus& bus) {
    uint8_t byte = bus.read_byte(PC); // Ejemplo usando el Program Counter
    PC++;
    return byte;
}

// 2. Lectura de 16 bits (little-endian)
uint16_t cpu::fetch_word(MemoryBus& bus) {
    uint16_t low = fetch_byte(bus);
    uint16_t high = fetch_byte(bus);
    return (high << 8) | low;
}

void cpu::ld_16_bits(uint16_t& reg, MemoryBus& memory, uint8_t cycles_cost){
    reg = fetch_word(memory); 
    cycles += cycles_cost;  
}



void cpu::ld_8_bits(uint8_t& reg, MemoryBus& memory, uint8_t cycles_cost){
    reg = fetch_byte(memory); 
   cycles += cycles_cost;    
}

uint8_t cpu::inc_8_bits(uint8_t& reg, uint8_t cycles_cost){
    bool half_carry = (B & 0x0F) == 0x0F; 
    reg++;
    cycles+=cycles_cost; 
    set_flag( FLAG_Z, reg == 0); 
    set_flag( FLAG_N, false); 
    set_flag(FLAG_H, half_carry);

    return reg;
}

void cpu::inc_16_bits(uint16_t& reg, uint8_t cycles){
    reg++; 
}

uint8_t cpu::dec_8_bits(uint8_t& reg, uint8_t cycles_cost){
    bool half_carry = (B & 0x0F) == 0; 
    reg--; 
    cycles += cycles_cost;
    set_flag(FLAG_Z, B==0); 
    set_flag(FLAG_N, true); 
    set_flag(FLAG_H, half_carry);

    return reg; 
}

bool cpu::get_flag(        
uint8_t flag_mask)const{
    return (F & flag_mask) != 0;
}

void cpu::RLCA(){
    uint8_t bit7 = (A >> 7) & 1; 

    A = static_cast<uint8_t> ((A<<1) | bit7); 

    // Banderas en LR35902 para RLCA:
    set_flag(FLAG_Z, false); // Z SIEMPRE es 0 en RLCA
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, false);
    set_flag(FLAG_C, bit7 == 1);

    cycles += 4; 
}


void cpu::ld_a16_(uint16_t reg, MemoryBus& memory, uint8_t cycles_cost){
    uint16_t address = fetch_word(memory);
    memory.write_word(address, reg);
    cycles += cycles_cost; // 5 M-cycles
}

void cpu::ld_a16_8bits(uint8_t reg, MemoryBus& memory, uint8_t cycles_cost){
    uint16_t address = fetch_word(memory);
    memory.write_byte(address, reg);
    cycles += cycles_cost; // 5 M-cycles
}