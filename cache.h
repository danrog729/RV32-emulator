#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

struct l1CacheEntry
{
	uint32_t tag;
	uint8_t valid;
	uint8_t dirty;
};

struct l1CacheSet
{
	uint8_t LRU;
	l1CacheEntry line0;
	l1CacheEntry line1;
};

struct l1CacheFullLine
{
	l1CacheEntry* metadata;
	uint32_t cacheIndex;
};

extern uint8_t ram[1024 * 1024 * 1024]; // 1GiB RAM
extern uint8_t l1CacheData[8192]; // 8KiB L1 Cache for data
extern uint8_t l1CacheProgram[8192]; // 8KiB L1 Cache for programs
extern l1CacheSet l1CacheDataMetadata[64];
extern l1CacheSet l1CacheProgramMetadata[64];
/*
64 sets in the cache
each set is 128 bytes

address format is:
most significant 20 bits are for the tag
6 bits after that are for the index
6 bits after that is for the offset into the entry
*/

uint8_t read_memory_b(uint32_t address);
uint16_t read_memory_s(uint32_t address);
uint32_t read_memory_i(uint32_t address);
uint32_t read_program_memory(uint32_t address);

void write_memory_b(uint32_t address, uint8_t data);
void write_memory_s(uint32_t address, uint16_t data);
void write_memory_i(uint32_t address, uint32_t data);

l1CacheFullLine get_cache_line(uint8_t* cache, l1CacheSet* metadata, uint32_t address);

void read_ram(uint8_t* cache, uint32_t cacheIndex, uint32_t lineAddress);
void write_ram(uint8_t* cache, uint32_t cacheIndex, uint32_t lineAddress);

#endif