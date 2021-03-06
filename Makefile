TARGET = main
MCU_SPEC = cortex-m3
ST_MCU_DEF = STM32F103xB

TOOLCHAIN = /usr/bin
CC  = $(TOOLCHAIN)/arm-none-eabi-gcc
CPP = $(TOOLCHAIN)/arm-none-eabi-g++
AS  = $(TOOLCHAIN)/arm-none-eabi-as
LD  = $(TOOLCHAIN)/arm-none-eabi-ld
OC  = $(TOOLCHAIN)/arm-none-eabi-objcopy
OD  = $(TOOLCHAIN)/arm-none-eabi-objdump
OS  = $(TOOLCHAIN)/arm-none-eabi-size


# Assembly directives.
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb

# C compilation directives.
CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -O0
CFLAGS += -fmessage-length=0 -fno-common
CFLAGS += -ffunction-sections -fdata-sections
#CFLAGS += --specs=nosys.specs
CFLAGS += -D$(ST_MCU_DEF)
CFLAGS += -DVVC_$(MCU_CLASS)
CFLAGS += -DVVC_$(MCU)
CFLAGS += -std=c99

# Linker directives.
LSCRIPT = STM32F103CBT6.ld
LFLAGS += -mcpu=$(MCU_SPEC)
LFLAGS += -mthumb
LFLAGS += -msoft-float
LFLAGS += -mfloat-abi=soft
LFLAGS += -Wall
#LFLAGS += --specs=nosys.specs
LFLAGS += --static
LFLAGS += -Wl,-Map=$(TARGET).map
LFLAGS += -Wl,--gc-sections
LFLAGS += -lgcc
LFLAGS += -lc
#LFLAGS += -lnosys
LFLAGS += -T$(LSCRIPT)

# Source files.
AS_SRC    = STM32F103CBT6_boot.S
AS_SRC   += STM32F103CBT6_vt.S
C_SRC	 += ./src/main.c

INCLUDE  += -I./
INCLUDE  += -I./src
INCLUDE  += -I./device_headers

OBJS  = $(C_SRC:.c=.o)
OBJS += $(CPP_SRC:.cpp=.o)
OBJS += $(AS_SRC:.S=.o)

.PHONY: all
all: $(TARGET).bin

%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) $< -o $@

$(TARGET).elf: $(OBJS)
	$(CPP) $^ $(LFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OC) -S -O binary $< $@
	$(OS) $<

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin
	rm -f $(TARGET).map
