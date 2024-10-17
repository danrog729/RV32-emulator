#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <io.h>
#define access _access
#define F_OK 0
#else
#include <unistd.h>
#endif

#include "cache.h"

uint32_t pc;
uint32_t registers[32];

FILE* biosChip;
FILE* secondaryStorage;

int main(int argc, char* argv[])
{
	// startup

	// check if bios chip file exists
	if (access("bios.sto", F_OK) != 0)
	{
		printf("FATAL: BIOS chip not found.\n");
		return -1;
	}
	else
	{
		fopen_s(&biosChip, "bios.sto", "rb");
		if (biosChip == NULL)
		{
			printf("FATAL: BIOS chip not found.\n");
			return -1;
		}
	}

	// load bios chip contents into ram
	for (uint16_t index = 0; index < 65535; index++)
	{
		uint32_t byte = fgetc(biosChip);
		if (byte == EOF)
		{
			break;
		}
		ram[index] = (uint8_t)byte;
	}
	fclose(biosChip);

	pc = 0;

	uint8_t shouldTerminate = 0;
	while (shouldTerminate == 0)
	{
		// running
		uint32_t opcode = read_program_memory(pc);

		pc += 4;
		if (pc == 128)
		{
			shouldTerminate = 1;
		}
	}

	// shutdown

	return 0;
}