#include <cassert>
#include <cstdint>
#include <type_traits>

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

class RCC
{
public:
	static constexpr uintptr_t base = 0x40023800U;
	volatile uint32_t& AHB1ENR = memory(base + 0x30);

	void enable_all_gpio_clocks() { AHB1ENR |= 0x3FF; }
};

enum class Port { A, B, C, D, E, F, G, H, I, J, K };
enum class Speed { Low, Medium, High, VeryHigh };
enum class Pull { None, Up, Down };
enum class OutputType { PushPull, OpenDrain };
enum class Mode { Input, Output, AlternateFunction, Analog };

template<Port P>
class GPIO
{
public:
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
};

template<Port P, int PinNum>
class Pin
{
public:
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

	void set_mode(Mode mode)
	{
		write_bits(_port.MODER, 0x3, 2*PinNum, static_cast<int>(mode));
	}

	void set_speed(Speed speed)
	{
		write_bits(_port.OSPEEDR, 0x3, 2*PinNum, static_cast<int>(speed));
	}

	void set_pull(Pull pull)
	{
		write_bits(_port.PUPDR, 0x3, 2*PinNum, static_cast<int>(pull));
	}

	void set_output_type(OutputType otype)
	{
		write_bits(_port.OTYPER, 0x1, PinNum, static_cast<int>(otype));
	}

	template<int N = PinNum>
	typename std::enable_if<(N < 8)>::type set_alternate_function(int af)
	{
		write_bits(_port.AFRL, 0xF, 4*N, af);
	}

	template<int N = PinNum>
	typename std::enable_if<(N >= 8)>::type set_alternate_function(int af)
	{
		write_bits(_port.AFRH, 0xF, 4*(N - 8), af);
	}

	void set() { _port.BSRR = 1U << PinNum; }

	void clr() { _port.BSRR = (1U << 16) << PinNum; }

	void tgl() { _port.ODR ^= (1 << PinNum); }

private:
	GPIO<P> _port;
};

class WWDG
{
public:
	static constexpr uintptr_t base = 0x40002C00U;
	volatile uint32_t& CR = memory(base + 0x00);
};

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
