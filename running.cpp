#include "running.h"
#include "cache.h"

uint32_t pc = 0;
uint32_t registers[32];

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
				if (registers[rs1] < registers[rs2])
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
	}
	return;
}