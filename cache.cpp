#include "cache.h"

uint8_t ram[1024 * 1024 * 1024]; // 1GiB RAM
uint8_t l1CacheData[8192]; // 8KiB L1 Cache for data
uint8_t l1CacheProgram[8192]; // 8KiB L1 Cache for programs
l1CacheSet l1CacheDataMetadata[64];
l1CacheSet l1CacheProgramMetadata[64];





/// <summary>
/// Reads 1 byte from memory at the specified address. Checks the data cache.
/// </summary>
/// <param name="address"> The address of the byte to read. </param>
/// <returns> The byte found in memory at the address given. </returns>
uint8_t read_memory_b(uint32_t address)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address);

	return l1CacheData[cacheLine.cacheIndex + addressOffset];
}

/// <summary>
/// Reads 2 bytes from memory at the specified address. Checks the data cache.
/// </summary>
/// <param name="address"> The address of the left-most byte to read. </param>
/// <returns> The 2 bytes found in memory at the address given. </returns>
uint16_t read_memory_s(uint32_t address)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address);
	uint16_t output = l1CacheData[cacheLine.cacheIndex + addressOffset] << 8;
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 1);
	output |= l1CacheData[cacheLine.cacheIndex + addressOffset + 1];

	return output;
}

/// <summary>
/// Reads 4 bytes from memory at the specified address. Checks the data cache.
/// </summary>
/// <param name="address"> The address of the left-most byte to read. </param>
/// <returns> The 4 bytes found in memory at the address given. </returns>
uint32_t read_memory_i(uint32_t address)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address);
	uint32_t output = l1CacheData[cacheLine.cacheIndex + addressOffset] << 24;
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 1);
	output |= l1CacheData[cacheLine.cacheIndex + addressOffset + 1] << 16;
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 2);
	output |= l1CacheData[cacheLine.cacheIndex + addressOffset + 2] << 8;
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 3);
	output |= l1CacheData[cacheLine.cacheIndex + addressOffset + 3];

	return output;
}

/// <summary>
/// Reads an opcode (4 bytes) from memory. Checks the program cache.
/// </summary>
/// <param name="address"> The address of the left-most byte of the opcode to read. </param>
/// <returns> The opcode found in memory at the address given. </returns>
uint32_t read_program_memory(uint32_t address)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheProgram, l1CacheProgramMetadata, address);
	uint32_t output = l1CacheProgram[cacheLine.cacheIndex + addressOffset] << 24;
	cacheLine = get_cache_line(l1CacheProgram, l1CacheProgramMetadata, address + 1);
	output |= l1CacheProgram[cacheLine.cacheIndex + addressOffset + 1] << 16;
	cacheLine = get_cache_line(l1CacheProgram, l1CacheProgramMetadata, address + 2);
	output |= l1CacheProgram[cacheLine.cacheIndex + addressOffset + 2] << 8;
	cacheLine = get_cache_line(l1CacheProgram, l1CacheProgramMetadata, address + 3);
	output |= l1CacheProgram[cacheLine.cacheIndex + addressOffset + 3];

	return output;
}





/// <summary>
/// Writes 1 byte to memory, at the given address.
/// </summary>
/// <param name="address"> The destination address in memory. </param>
/// <param name="data"> The data that is to be written to memory. </param>
void write_memory_b(uint32_t address, uint8_t data)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address);

	l1CacheData[cacheLine.cacheIndex + addressOffset] = data;
}

/// <summary>
/// Writes 2 bytes to memory, starting at the given address.
/// </summary>
/// <param name="address"> The destination address in memory of the first byte to be written. </param>
/// <param name="data"> The data that is to be written to memory. </param>
void write_memory_s(uint32_t address, uint16_t data)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address);
	l1CacheData[cacheLine.cacheIndex + addressOffset] = (uint8_t)(data >> 8 & 0x00ff);
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 1);
	l1CacheData[cacheLine.cacheIndex + addressOffset + 1] = (uint8_t)(data & 0x00ff);
}

/// <summary>
/// Writes 4 bytes to memory, starting at the given address.
/// </summary>
/// <param name="address"> The destination address in memory of the first byte to be written. </param>
/// <param name="data"> The data that is to be written to memory. </param>
void write_memory_i(uint32_t address, uint32_t data)
{
	uint8_t addressOffset = address & 0x0000003f; // last 6 bits of address

	l1CacheFullLine cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address);
	l1CacheData[cacheLine.cacheIndex + addressOffset] = (uint8_t)(data >> 24 & 0x00ff);
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 1);
	l1CacheData[cacheLine.cacheIndex + addressOffset + 1] = (uint8_t)(data >> 16 & 0x00ff);
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 1);
	l1CacheData[cacheLine.cacheIndex + addressOffset + 2] = (uint8_t)(data >> 8 & 0x00ff);
	cacheLine = get_cache_line(l1CacheData, l1CacheDataMetadata, address + 1);
	l1CacheData[cacheLine.cacheIndex + addressOffset + 3] = (uint8_t)(data & 0x00ff);
}





/// <summary>
/// Looks up the given address in the cache metadata. If the containing cache line is in the cache, then it retrieves it. If not, it copies the cache line from RAM into the cache and the retrieves it.
/// </summary>
/// <param name="cache"> The cache to copy data from RAM into if required. </param>
/// <param name="metadata"> The metadata to look up the address in. </param>
/// <param name="address"> The memory address to search for in the cache. </param>
/// <returns> A struct containing a pointer to the metadata entry, and an index into the cache where the line starts. </returns>
l1CacheFullLine get_cache_line(uint8_t* cache, l1CacheSet* metadata, uint32_t address)
{
	// extract the tag and index from the address, as these are used to locate the correct cache line
	uint32_t tag = (uint32_t)(address >> 12); // last 20 bits
	uint8_t index = (uint8_t)((address >> 6) & 0x003f); // middle 6 bits

	// grab the part of the cache metadata that relates to the index
	l1CacheSet* set = &(metadata[index]);

	l1CacheFullLine output = { NULL, 0 };

	// the beginning of the cache line is the index of the address * 128
	output.cacheIndex = (index << 7);

	if (set->line0.valid && set->line0.tag == tag)
	{
		// read/write from line0
		output.metadata = &set->line0;
	}
	else if (set->line1.valid && set->line1.tag == tag)
	{
		// read/write from line1
		output.metadata = &set->line1;
		output.cacheIndex += 64;
	}

	// cache miss
	else if (!set->line0.valid)
	{
		// copy from ram into line0
		uint32_t ramAddress = (tag << 12) + (index << 6); // get the start of the line in ram
		read_ram(cache, output.cacheIndex, ramAddress);
		set->line0.tag = tag;
		// read/write from line0
		output.metadata = &set->line0;
		set->line0.valid = 1;
	}
	else if (!set->line1.valid)
	{
		// copy from ram into line1
		output.cacheIndex += 64;
		uint32_t ramAddress = (tag << 12) + (index << 6);
		read_ram(cache, output.cacheIndex, ramAddress); // get the start of the line in ram
		set->line1.tag = tag;
		// read/write from line1
		output.metadata = &set->line1;
		set->line1.valid = 1;
	}

	// cache eviction required
	else if (set->LRU = 0)
	{
		// write to ram if required
		uint32_t ramAddress = (set->line0.tag << 12) + (index << 6);
		if (set->line0.dirty)
		{
			write_ram(cache, ramAddress, output.cacheIndex);
		}
		// copy from ram into line0
		ramAddress = (tag << 12) + (index << 6);
		read_ram(cache, output.cacheIndex, ramAddress);
		set->line0.tag = tag;
		set->line0.dirty = 0;
		// read/write from line0
		output.metadata = &set->line0;
	}
	else if (set->LRU = 1)
	{
		// write to ram if required
		output.cacheIndex += 64;
		uint32_t ramAddress = (set->line1.tag << 12) + (index << 6);
		if (set->line1.dirty)
		{
			write_ram(cache, ramAddress, output.cacheIndex);
		}
		// copy from ram into line1
		ramAddress = (tag << 12) + (index << 6);
		read_ram(cache, output.cacheIndex, ramAddress);
		set->line1.tag = tag;
		set->line1.dirty = 0;
		// read/write from line1
		output.metadata = &set->line1;
	}

	return output;
}

/// <summary>
/// Copies 64 bytes from the RAM at the given line address into the cache at the given cache index
/// </summary>
/// <param name="cache"> The cache for the RAM to copy into. </param>
/// <param name="cacheIndex"> The index into the cache where the first byte should be copied into. </param>
/// <param name="lineAddress"> The index into the RAM where the first byte should be taken from. </param>
void read_ram(uint8_t* cache, uint32_t cacheIndex, uint32_t lineAddress)
{
	for (int byte = 0; byte < 64; byte++)
	{
		cache[cacheIndex + byte] = ram[lineAddress +byte];
	}
}

/// <summary>
/// Copies 64 bytes from the cache at the given cache index into the RAM at the given line address
/// </summary>
/// <param name="cache"> The cache for the RAM to be copied from. </param>
/// <param name="cacheIndex"> The index into the cache where the first byte should be taken from. </param>
/// <param name="lineAddress"> The index into the RAM where the first byte should be copied into. </param>
void write_ram(uint8_t* cache, uint32_t cacheIndex, uint32_t lineAddress)
{
	for (int byte = 0; byte < 64; byte++)
	{
		ram[lineAddress + byte] = cache[cacheIndex + byte];
	}
}