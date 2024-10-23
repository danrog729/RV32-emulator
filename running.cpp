#include "running.h"
#include "cache.h"

uint32_t pc = 0;
int32_t registers[32];

/// <summary>
/// The execution is in this function for the majority of the time. It loops from the end of the boot to shutdown.
/// It acts as the CPU, which means it fetches instructions from memory, decodes them and performs them.
/// </summary>
void run_cpu()
{
	uint8_t shouldTerminate = 0;
	while (shouldTerminate == 0)
	{
		// running
		uint32_t instruction = read_program_memory(pc);
		pc += 4;

		// Decode the CPU instruction. Refer to https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf for more info.
		
		uint8_t opcode = instruction & 0x7f;

		if (opcode == 0b0110011)
		{
			// R-type instruction
			R_type(instruction);
		}
		else if (opcode == 0b0010011 || opcode == 0b00000011 || opcode == 0b1110011)
		{
			// I-type instruction
			I_type(instruction);
		}
		else if (opcode == 0b0100011)
		{
			// S-type instruction
			S_type(instruction);
		}
	}
	return;
}

inline void R_type(uint32_t instruction)
{
	uint8_t rd = instruction >> 7 & 0x01f;
	uint8_t funct3 = instruction >> 12 & 0x07;
	uint8_t rs1 = instruction >> 15 & 0x01f;
	uint8_t rs2 = instruction >> 20 & 0x01f;
	uint8_t funct7 = instruction >> 25 & 0x7f;

	if (funct3 == 0x0 && funct7 == 0x00)
	{
		// add (ADD)
		registers[rd] = registers[rs1] + registers[rs2];
	}
	else if (funct3 == 0x0 && funct7 == 0x20)
	{
		// sub (SUB)
		registers[rd] = registers[rs1] - registers[rs2];
	}
	else if (funct3 == 0x4 && funct7 == 0x00)
	{
		// xor (XOR)
		registers[rd] = registers[rs1] ^ registers[rs2];
	}
	else if (funct3 == 0x6 && funct7 == 0x00)
	{
		// or (OR)
		registers[rd] = registers[rs1] | registers[rs2];
	}
	else if (funct3 == 0x7 && funct7 == 0x00)
	{
		// and (AND)
		registers[rd] = registers[rs1] & registers[rs2];
	}
	else if (funct3 == 0x1 && funct7 == 0x00)
	{
		// sll (Shift Left Logical)
		registers[rd] = registers[rs1] << registers[rs2];
	}
	else if (funct3 == 0x5 && funct7 == 0x00)
	{
		// srl (Shift Right Logical)
		registers[rd] = (uint32_t)registers[rs1] >> registers[rs2];
	}
	else if (funct3 == 0x5 && funct7 == 0x20)
	{
		// sra (Shift Right Arithmetic)
		registers[rd] = (int32_t)registers[rs1] >> registers[rs2];
	}
	else if (funct3 == 0x2 && funct7 == 0x00)
	{
		// slt (Set Less Than)
		if ((int32_t)registers[rs1] < (int32_t)registers[rs2])
		{
			registers[rd] = 1;
		}
		else
		{
			registers[rd] = 0;
		}
	}
	else if (funct3 == 0x3 && funct7 == 0x00)
	{
		// sltu (Set Less Than (Unsigned))
		if ((uint32_t)registers[rs1] < (uint32_t)registers[rs2])
		{
			registers[rd] = 1;
		}
		else
		{
			registers[rd] = 0;
		}
	}
}

inline void I_type(uint32_t instruction)
{
	uint8_t opcode = instruction & 0x7f;
	uint8_t rd = instruction >> 7 & 0x1f;
	uint8_t funct3 = instruction >> 12 & 0x07;
	uint8_t rs1 = instruction >> 15 & 0x1f;
	uint16_t imm = instruction >> 20 & 0x07ff;

	if (opcode == 0b0010011)
	{
		if (funct3 == 0x0)
		{
			// addi (ADD Immediate)
			registers[rd] = registers[rs1] + imm;
		}
		else if (funct3 == 0x4)
		{
			// xori (XOR Immediate)
			registers[rd] = registers[rs1] ^ imm;
		}
		else if (funct3 == 0x6)
		{
			// ori (OR Immediate)
			registers[rd] = registers[rs1] | imm;
		}
		else if (funct3 == 0x7)
		{
			// andi (AND Immediate)
			registers[rd] = registers[rs1] & imm;
		}
		else if (funct3 == 0x1)
		{
			//slli (Shift Left Logical Imm)
			imm &= 0x001f;
			registers[rd] = registers[rs1] << imm;
		}
		else if (funct3 == 0x5 && imm == 0x0000)
		{
			// srli (Shift Right Logical Imm)
			imm &= 0x001f;
			registers[rd] = (uint32_t)registers[rd] >> imm;
		}
		else if (funct3 == 0x5 && imm == 0x0400)
		{
			// srai (Shift Right Arith Imm)
			imm &= 0x001f;
			registers[rd] = (int32_t)registers[rd] >> imm;
		}
		else if (funct3 == 0x2)
		{
			// slti (Set Less Than Imm)
			if ((int32_t)registers[rd] < (int32_t)imm)
			{
				rd = 1;
			}
			else
			{
				rd = 0;
			}
		}
		else if (funct3 == 0x3)
		{
			// sltiu (Set Less Than Imm (U))
			if ((uint32_t)registers[rd] < (uint32_t)imm)
			{
				rd = 1;
			}
			else
			{
				rd = 0;
			}
		}
	}
	else if (opcode == 0b0000011)
	{
		if (funct3 == 0x0)
		{
			// lb (Load Byte)
			registers[rd] = (int32_t)read_memory_b((uint32_t)registers[rs1] + imm);
		}
		else if (funct3 == 0x1)
		{
			// lh (Load Half)
			registers[rd] = (int32_t)read_memory_s((uint32_t)registers[rs1] + imm);
		}
		else if (funct3 == 0x2)
		{
			// lw (Load Word)
			registers[rd] = (int32_t)read_memory_i((uint32_t)registers[rs1] + imm);
		}
		else if (funct3 == 0x4)
		{
			// lbu (Load Byte (U))
			registers[rd] = (uint32_t)read_memory_b((uint32_t)registers[rs1] + imm);
		}
		else if (funct3 == 0x5)
		{
			// lhu (Load Half (U))
			registers[rd] = (uint32_t)read_memory_s((uint32_t)registers[rs1] + imm);
		}
	}
	else if (opcode == 0b1110011)
	{
		if (funct3 == 0x0 && imm == 0x0)
		{
			// ecall (Environment Call)
			// NOT YET IMPLEMENTED
		}
		else if (funct3 == 0x0 && imm == 0x1)
		{
			// ebreak (Environment Break)
			// NOT YET IMPLEMENTED
		}
	}
}

inline void S_type(uint32_t instruction)
{
	uint8_t immLower = instruction >> 7 & 0x01f;
	uint8_t funct3 = instruction >> 12 & 0x07;
	uint8_t rs1 = instruction >> 15 & 0x01f;
	uint8_t rs2 = instruction >> 20 & 0x01f;
	uint8_t immUpper = instruction >> 25 & 0x7f;
	uint16_t imm = immUpper << 5 + immLower;

	if (funct3 == 0x0)
	{
		// sb (Store Byte)
		write_memory_b((uint32_t)registers[rs1] + imm, (uint8_t)registers[rs2]);
	}
	else if (funct3 == 0x1)
	{
		// sh (Store Half)
		write_memory_s((uint32_t)registers[rs1] + imm, (uint16_t)registers[rs2]);
	}
	else if (funct3 == 0x2)
	{
		// sw (Store Word)
		write_memory_i((uint32_t)registers[rs1] + imm, (uint32_t)registers[rs2]);
	}
}