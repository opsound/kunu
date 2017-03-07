#include <cassert>
#include <cstdint>

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

void write_bits(volatile uint32_t& dst, uint32_t mask, int pos, uint32_t bits)
{
	dst = (dst & ~(mask << pos)) | ((mask & bits) << pos);
}

struct RCC
{
	static constexpr uintptr_t base = 0x40023800U;
	volatile uint32_t& AHB1ENR = memory(base + 0x30);

	void enable_all_gpio_clocks() { AHB1ENR |= 0x3FF; }
};

enum class Port { A, B, C, D, E, F, G, H, I, J, K };
enum class Speed { LOW, MEDIUM, HIGH, VERY_HIGH };
enum class Pull { NONE, UP, DOWN };
enum class OutputType { PUSH_PULL, OPEN_DRAIN };
enum class Mode { INPUT, OUTPUT, ALTERNATE_FUNCTION, ANALOG };

template<Port P>
struct GPIO
{
	static constexpr uintptr_t base = 0x40020000U + (static_cast<int>(P) * 0x400U);
	volatile uint32_t& MODER = memory(base + 0x00);
	volatile uint32_t& OTYPER = memory(base + 0x04);
	volatile uint32_t& OSPEEDR = memory(base + 0x08);
	volatile uint32_t& PUPDR = memory(base + 0x0C);
	volatile uint32_t& IDR = memory(base + 0x10);
	volatile uint32_t& ODR = memory(base + 0x14);
	volatile uint32_t& BSRR = memory(base + 0x18);
	volatile uint32_t& LCKR = memory(base + 0x1C);
	volatile uint32_t& AFRL = memory(base + 0x20);
	volatile uint32_t& AFRH = memory(base + 0x24);

	void set_pin_mode(int pin, Mode mode)
	{
		write_bits(MODER, 0x3, 2*pin, static_cast<int>(mode));
	}

	void set_pin_speed(int pin, Speed speed)
	{
		write_bits(OSPEEDR, 0x3, 2*pin, static_cast<int>(speed));
	}

	void set_pin_pull(int pin, Pull pull)
	{
		write_bits(PUPDR, 0x3, 2*pin, static_cast<int>(pull));
	}

	void set_pin_output_type(int pin, OutputType otype)
	{
		write_bits(OTYPER, 0x1, pin, static_cast<int>(otype));
	}

	void set_pin_alternate_function(int pin, int af)
	{
		if (pin < 8)
		{
			write_bits(AFRL, 0xF, 4*pin, af);
		}
		else
		{
			pin -= 8;
			write_bits(AFRH, 0xF, 4*pin, af);
		}
	}

	void set_pin(int pin) { BSRR = 1U << pin; }

	void clr_pin(int pin) { BSRR = (1U << 16) << pin; }

	void tgl_pin(int pin) { ODR ^= (1 << pin); }

};

template<Port P, int PinNum>
struct Pin
{
	static_assert(PinNum < 16, "");

	Pin () {}

	Pin (Mode mode, Speed speed, Pull pull, OutputType otype, int af)
	{
		set_mode(mode);
		set_speed(speed);
		set_pull(pull);
		set_output_type(otype);
		set_alternate_function(af);
	}

	void set_mode(Mode mode) { _port.set_pin_mode(PinNum, mode); }

	void set_speed(Speed speed) { _port.set_pin_speed(PinNum, speed); }

	void set_pull(Pull pull) { _port.set_pin_pull(PinNum, pull); }

	void set_output_type(OutputType otype) { _port.set_pin_output_type(PinNum, otype); }

	void set_alternate_function(int af) { _port.set_pin_alternate_function(PinNum, af); }

	void set() { _port.set_pin(PinNum); }

	void clr() { _port.clr_pin(PinNum); }

	void tgl() { _port.tgl_pin(PinNum); }

private:
	GPIO<P> _port;
};

struct WWDG
{
	static constexpr uintptr_t base = 0x40002C00U;
	volatile uint32_t& CR = memory(base + 0x00);
};

int main()
{
	RCC rcc;
	rcc.enable_all_gpio_clocks();

	Pin<Port::B, 7> led_blue(Mode::OUTPUT, Speed::VERY_HIGH, Pull::NONE, OutputType::PUSH_PULL, 0);
	Pin<Port::B, 14> led_red(Mode::OUTPUT, Speed::VERY_HIGH, Pull::NONE, OutputType::PUSH_PULL, 0);

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
