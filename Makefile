TARGET = app

CXXFLAGS += -mthumb
CXXFLAGS += -mcpu=cortex-m7
CXXFLAGS += -mfloat-abi=hard
CXXFLAGS += -mfpu=fpv5-d16
CXXFLAGS += -O2
CXXFLAGS += -g
CXXFLAGS += -Wall
CXXFLAGS += -flto
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections
CXXFLAGS += -fno-exceptions
CXXFLAGS += -fno-rtti
CXXFLAGS += -Ideps

LFLAGS += -L.
LFLAGS += -T STM32F767ZITx_FLASH.ld
# LFLAGS += --specs=rdimon.specs
LFLAGS += --specs=nosys.specs
LFLAGS += -fno-exceptions
LFLAGS += -Wl,--gc-sections
LFLAGS += -Wl,-Map=$(BUILDDIR)/$(TARGET).map

CXX = arm-none-eabi-g++

SRCS += main.cpp
SRCS += deps/system_stm32f7xx.c
SRCS += startup_stm32f767xx.s

BUILDDIR = build

$(BUILDDIR)/$(TARGET).elf: $(SRCS) Makefile | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(filter-out $(lastword $^),$^) $(LFLAGS) -o $@

$(BUILDDIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
