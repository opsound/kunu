#pragma once

#include "Misc.hpp"

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
		write_bits(_port.MODER, 2*PinNum, 0x3, static_cast<int>(mode));
	}

	void set_speed(Speed speed)
	{
		write_bits(_port.OSPEEDR, 2*PinNum, 0x3, static_cast<int>(speed));
	}

	void set_pull(Pull pull)
	{
		write_bits(_port.PUPDR, 2*PinNum, 0x3, static_cast<int>(pull));
	}

	void set_output_type(OutputType otype)
	{
		write_bits(_port.OTYPER, PinNum, 0x1, static_cast<int>(otype));
	}

	template<int N = PinNum>
	typename std::enable_if<(N < 8)>::type set_alternate_function(int af)
	{
		write_bits(_port.AFRL, 4*N, 0xF, af);
	}

	template<int N = PinNum>
	typename std::enable_if<(N >= 8)>::type set_alternate_function(int af)
	{
		write_bits(_port.AFRH, 4*(N - 8), 0xF, af);
	}

	void set() { _port.BSRR = 1U << PinNum; }

	void clr() { _port.BSRR = (1U << 16) << PinNum; }

	void tgl() { _port.ODR ^= (1 << PinNum); }

private:
	GPIO<P> _port;
};
