#include <cassert>
#include <cstdint>
#include <type_traits>
#include "GPIO.hpp"
#include "RCC.hpp"

int main()
{
	RCC rcc;
	rcc.enable_all_gpio_clocks();

	Pin<Port::B, 7> led_blue(Mode::Output, Speed::VeryHigh, Pull::None, OutputType::PushPull, 0);
	Pin<Port::B, 14> led_red(Mode::Output, Speed::VeryHigh, Pull::None, OutputType::PushPull, 0);

	led_red.clr();
	led_blue.set();

	while (1)
	{
		led_red.tgl();
		led_blue.tgl();

		for (int i = 0; i < 4 * 1000 * 1000; ++i)
		{
			asm volatile("nop" : : : );
		}
	}
	return 0;
}
