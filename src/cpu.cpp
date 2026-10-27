#include "cpu.hpp"

cpu::cpu(){
    reset();
}


void cpu::reset(){
    AF = 0X01B0; 
    BC = 0X0013; 
    DE = 0X00D8; 
    HL = 0X014D; 
    SP = 0XFFFE; 
    PC = 0X0100; 
}

void cpu::fetch( MemoryBus &memory){
    current_op_code = fetch_byte(memory); 
}

uint8_t& cpu::reg8(int idx) {
    return regs[reg_offset[idx]];
}

void cpu::execute(MemoryBus &memory) {
    instrucction_counter++; 

    const int y = (current_op_code >> 3 & 7); 
    const int z = current_op_code & 7;

    const int pair = (current_op_code >> 4) & 3; 

    //LD 
    if ( current_op_code >= 0x40 && current_op_code <= 0x7F && current_op_code != 0x76){
        if (y == 6) memory.write_byte(HL, reg8(z)); 
        else if ( z == 6) reg8(y) = memory.read_byte(HL); 
        else reg8(y) = reg8(z);
        cycles += (y == 6 || z == 6) ? 8 : 4; 
        return;
    }

    if (current_op_code < 0x40 && z == 6){
        if( y == 6 ){ memory.write_byte(HL, fetch_byte(memory)); cycles += 12; }
        else {ld_8_bits( reg8(y), memory, 8);};
        return;
    }
    
    if ((current_op_code & 0xCF) == 0x01 ){
        ld_16_bits(regs_16[pair], memory, 12); 
        return;
    }
    //INC && DEC 
    if ( current_op_code < 0x40 && (z == 4 || z == 5) ){
        if ( y == 6 ) {
            uint8_t v = memory.read_byte(HL);
            if( z == 4 ) inc_8_bits(v, 12); 
            else dec_8_bits (v, 12); 
            memory.write_byte(HL, v); 
        }
        else {
            if (z == 4) inc_8_bits (reg8(y), 4); 
            else dec_8_bits (reg8(y), 4);
        }
        return;
    }

    //inc 16 bits
    if( (current_op_code & 0xCF) == 0x03 ){
        inc_16_bits(regs_16[pair], 8); 
    }
    //dec 16 bits
    if( (current_op_code & 0xCF) == 0x0B ){
        inc_16_bits(regs_16[pair], 8); 
    }


    switch (current_op_code) {
        case 0x00: {cycles += 4; break;}// NOP (1 M-cycle)
        case 0x02: {  memory.write_byte(BC, A); cycles += 8; break; } // LD (BC), A (2 M-cycles) - Escribe A en la memoria RAM en la dirección BC
        case 0x03: { inc_16_bits(BC, 8);break; } // INC BC (2 M-cycles) - Incremento de 16 bits (sin banderas)        case 0x04: { B = inc_8_bits(B, 4); break; } // INC B (1 M-cycle) - Incremento de 8 bits (actualiza Z, N, H)        case 0x06: { ld_8_bits(B, memory, 8); break; }// LD B, d8 (2 M-cycles) - Carga un byte inmediato de ROM a B
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
    bool half_carry = (reg & 0x0F) == 0x0F; 
    reg++;
    cycles+=cycles_cost; 
    set_flag( FLAG_Z, reg == 0); 
    set_flag( FLAG_N, false); 
    set_flag(FLAG_H, half_carry);

    return reg;
}

void cpu::inc_16_bits(uint16_t& reg, uint8_t cycles_cost){
    reg++; 
    cycles += cycles_cost;
}

void cpu::dec_16bits(uint16_t& reg, uint8_t cycles_cost){
    reg--; 
    cycles += cycles_cost;
}

uint8_t cpu::dec_8_bits(uint8_t& reg, uint8_t cycles_cost){
    bool half_carry = (reg & 0x0F) == 0; 
    reg--; 
    cycles += cycles_cost;
    set_flag(FLAG_Z, reg == 0); 
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