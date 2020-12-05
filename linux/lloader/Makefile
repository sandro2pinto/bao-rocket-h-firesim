
ifeq ($(and $(IMAGE), $(DTB), $(TARGET), $(ARCH)),)
ifneq ($(MAKECMDGOALS), clean)
 $(error Linux image (IMAGE) and/or device tree (DTB) and/or target name \
 	(TARGET) and/or architecture (ARCH) not specified)
endif
endif

ARCH=rv64
ifeq ($(ARCH), rv64)
CROSS_COMPILE ?=riscv64-unknown-elf-
OPTIONS=-mcmodel=medany
else
$(error unkown architecture $(ARCH))
endif

all: $(TARGET).bin

clean:
	-rm *.elf *.bin

.PHONY: all clean
	
$(TARGET).bin: $(TARGET).elf
	$(CROSS_COMPILE)objcopy -S -O binary $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(ARCH).S $(IMAGE) $(DTB) loader_$(ARCH).ld
	$(CROSS_COMPILE)gcc -Wl,-build-id=none -nostdlib -T loader_$(ARCH).ld\
		-o $(TARGET).elf $(OPTIONS) $(ARCH).S -I. -D IMAGE=$(IMAGE) -D DTB=$(DTB)
