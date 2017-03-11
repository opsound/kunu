#pragma once

void escape(void* p)
{
	asm volatile("" : : "g"(p) : "memory");
}

void clobber()
{
	asm volatile("" : : : "memory");
}

volatile uint32_t& memory(uintptr_t addr)
{
	return *reinterpret_cast<volatile uint32_t *>(addr);
}

void write_bits(volatile uint32_t& dst, int pos, uint32_t mask, uint32_t bits)
{
	dst = (dst & ~(mask << pos)) | ((mask & bits) << pos);
}
