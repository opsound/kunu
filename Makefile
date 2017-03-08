TARGET = app

BUILDDIR = build

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
CXXFLAGS += -MMD

LFLAGS += -L.
LFLAGS += -T STM32F767ZITx_FLASH.ld
# LFLAGS += --specs=rdimon.specs
LFLAGS += --specs=nosys.specs
LFLAGS += -fno-exceptions
LFLAGS += -Wl,--gc-sections
LFLAGS += -Wl,-Map=$(BUILDDIR)/$(TARGET).map

CXX = arm-none-eabi-g++

SRCS += main.cpp
SRCS += deps/system_stm32f7xx.cpp
SRCS += startup_stm32f767xx.s

OBJS = $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SRCS)))
DEPS = $(OBJS:.o=.d)

$(BUILDDIR)/$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LFLAGS) -o $@

$(BUILDDIR)/%.cpp.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.s.o : %.s
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS): | $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $@
	mkdir -p $(sort $(dir $(OBJS)))

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

-include $(DEPS)
