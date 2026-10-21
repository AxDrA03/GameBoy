#pragma once 
#include <cstdint>
#include <iostream> 
#include "MemoryBus.hpp"

class cpu {

public:
    // Registers
    union {
        uint8_t regs[8];
        uint16_t regs_16[4];
        struct {
            union { struct { uint8_t C; uint8_t B; }; uint16_t BC; };
            union { struct { uint8_t E; uint8_t D; }; uint16_t DE; };
            union { struct { uint8_t L; uint8_t H; }; uint16_t HL; };
            union { struct { uint8_t F; uint8_t A; }; uint16_t AF; };
        };
    };
    
    static constexpr uint8_t reg_offset[8] = {1, 0, 3, 2, 5, 4, 6, 7}; 

    uint8_t read_r(int idx, MemoryBus& memory) const;
    void write_r(int idx, uint8_t v,MemoryBus& memory);


    //uint16_t AF; //Acumulstor&Flags 

    uint16_t SP{0x0000};//Stack Pointer
    uint16_t PC{0x0000};//Program Counter/Pointer

    //The Flags Register, lower 8 bit of AF Register
    static constexpr uint8_t FLAG_Z = 1 << 7; //Zero Flags
    static constexpr uint8_t FLAG_N = 1 << 6;//Subtraction Flag (BCD)
    static constexpr uint8_t FLAG_H = 1 << 5;//Half Carry flag (BCD)
    static constexpr uint8_t FLAG_C = 1 << 4;//Carry flag 

    uint8_t current_op_code = 0; 
    uint32_t instrucction_counter = 0; 
    uint64_t cycles = 0; // 4 T-cyles = M-cycles que son los valores 
   
    cpu();
    void reset(); 
    void fetch(MemoryBus &memory); //Read from memory bus 
    void execute(MemoryBus &memory);    
    
    uint8_t fetch_byte(MemoryBus& bus); 
    uint16_t fetch_word(MemoryBus& memory);
    
    void set_flag(uint8_t flag_mask, bool condition); 
    bool get_flag(uint8_t flag_mask) const;
    
private:
    void execute_cb (MemoryBus& memory);
    
    void ld_8_bits(uint8_t& reg, MemoryBus& memory, const uint8_t cycles_cost); 
    void ld_16_bits(uint16_t& reg, MemoryBus& memory, const uint8_t cycles_cost); 
    
    uint8_t inc_8_bits(uint8_t& reg, uint8_t cycles_cost);
    uint8_t dec_8_bits(uint8_t& reg, uint8_t cycles_cost);
    
    void inc_16_bits(uint16_t& reg, uint8_t cycles_cost);
    void dec_16bits(uint16_t& reg, uint8_t cycles_cost);

    void ld_a16_(uint16_t reg, MemoryBus& memory, uint8_t cycles_cos);
    void ld_a16_8bits(uint8_t reg, MemoryBus& memory, uint8_t cycles_cos);


    void RLCA(); 

};
