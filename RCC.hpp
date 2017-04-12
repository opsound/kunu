#pragma once

#include "GPIO.hpp"
#include "stm32f767xx.h"
#include <utility>

namespace rcc
{

void enable_gpio_clock(std::initializer_list<gpio::Port> ports)
{
	uint32_t mask = 0;
	for (auto p : ports)
	{
		mask |= (static_cast<int>(p) << 1);
	}
	RCC->AHB1ENR |= mask;
}

}
