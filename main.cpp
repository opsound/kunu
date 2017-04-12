#include "GPIO.hpp"
#include "RCC.hpp"

int main()
{
	rcc::enable_gpio_clock({gpio::Port::A, gpio::Port::B});

	gpio::Pin<gpio::Port::B, 14> led_red(gpio::Mode::Output, gpio::Speed::VeryHigh, gpio::Pull::None, gpio::OutputType::PushPull, 0);
	gpio::Pin<gpio::Port::B, 7> led_blue(gpio::Mode::Output, gpio::Speed::VeryHigh, gpio::Pull::None, gpio::OutputType::PushPull, 0);

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
