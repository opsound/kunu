#pragma once

#include "Misc.hpp"
#include "stm32f767xx.h"


enum class Port { A, B, C, D, E, F, G, H, I, J, K };
enum class Speed { Low, Medium, High, VeryHigh };
enum class Pull { None, Up, Down };
enum class OutputType { PushPull, OpenDrain };
enum class Mode { Input, Output, AlternateFunction, Analog };

constexpr GPIO_TypeDef* to_gpio(Port port)
{
	switch (port)
	{
	case Port::A: return GPIOA;
	case Port::B: return GPIOB;
	case Port::C: return GPIOC;
	case Port::D: return GPIOD;
	case Port::E: return GPIOE;
	case Port::F: return GPIOF;
	case Port::G: return GPIOG;
	case Port::H: return GPIOH;
	case Port::I: return GPIOI;
	case Port::J: return GPIOJ;
	case Port::K: return GPIOK;
	}
	return nullptr;
}

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
		write_bits(_port->MODER, 2 * PinNum, 0x3, static_cast<int>(mode));
	}

	void set_speed(Speed speed)
	{
		write_bits(_port->OSPEEDR, 2 * PinNum, 0x3, static_cast<int>(speed));
	}

	void set_pull(Pull pull)
	{
		write_bits(_port->PUPDR, 2 * PinNum, 0x3, static_cast<int>(pull));
	}

	void set_output_type(OutputType otype)
	{
		write_bits(_port->OTYPER, PinNum, 0x1, static_cast<int>(otype));
	}

	template<int N = PinNum>
	typename std::enable_if<(N < 8)>::type set_alternate_function(int af)
	{
		write_bits(_port->AFR[0], 4 * N, 0xF, af);
	}

	template<int N = PinNum>
	typename std::enable_if<(N >= 8)>::type set_alternate_function(int af)
	{
		write_bits(_port->AFR[1], 4 * (N - 8), 0xF, af);
	}

	void set() { _port->BSRR = 1U << PinNum; }

	void clr() { _port->BSRR = (1U << 16) << PinNum; }

	void tgl() { _port->ODR ^= (1 << PinNum); }

private:
	static constexpr GPIO_TypeDef* _port = to_gpio(P);
};
