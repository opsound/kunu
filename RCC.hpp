#pragma once

class RCC
{
public:
	static constexpr uintptr_t base = 0x40023800U;
	volatile uint32_t& AHB1ENR = memory(base + 0x30);

	void enable_all_gpio_clocks() { AHB1ENR |= 0x3FF; }
};
