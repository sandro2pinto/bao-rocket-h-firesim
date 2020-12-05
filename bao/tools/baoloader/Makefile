ifeq ($(and $(ARCH), $(PLAT), $(CONFIG)),)
ifneq ($(MAKECMDGOALS), clean)
 $(error Target arch (ARCH) and/or configuration (CONFIG) and/or platform \
 	(PLAT) not specified)
endif
endif

ARCH=rv64
ifeq ($(ARCH), rv64)
CROSS_COMPILE?=riscv64-unknown-elf-
OPTIONS=-mcmodel=medany
else
$(error unkown architecture $(ARCH))
endif


TARGET:=$(PLAT)_$(CONFIG)
override TARGET:=images/$(TARGET)
override CONFIG:=../../configs/$(CONFIG)/$(CONFIG).bin
IMAGE:=../../bin/$(PLAT)/bao.bin

all: $(TARGET).bin

clean:
	-rm -rf ./images

.PHONY: all clean
	
$(TARGET).bin: $(TARGET).elf 
	$(CROSS_COMPILE)objcopy -S -O binary $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(ARCH).S baoloader.ld $(IMAGE) $(CONFIG) $(dir $(TARGET))
	$(CROSS_COMPILE)gcc -Wl,-build-id=none -g -nostdlib -T baoloader.ld\
		-o $(TARGET).elf $(OPTIONS) $(ARCH).S -I. -D IMAGE=$(IMAGE) -D CONFIG=$(CONFIG)

$(dir $(TARGET)):
	mkdir -p $@
