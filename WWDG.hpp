#pragma once

class WWDG
{
public:
	static constexpr uintptr_t base = 0x40002C00U;
	volatile uint32_t& CR = memory(base + 0x00);
};
