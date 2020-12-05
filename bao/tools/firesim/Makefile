BAO_PATH:=$(realpath ../..)
OPENSBI_PATH:=$(realpath ../../../opensbi)
PLATFORM:=rocket-firesim
TARGET:=$(PLATFORM)_$(CONFIG)
IMAGE:=images/bao_$(CONFIG).elf
BAO_CONFIG=$(BAO_PATH)/configs/$(CONFIG)/$(CONFIG).bin
BAO_BIN=$(BAO_PATH)/bin/$(PLATFORM)/bao.bin
BAO_IMAGE:=$(BAO_PATH)/tools/baoloader/images/$(TARGET).bin 
FIRESIM_IMAGES:=images

FW_DIR:=$(OPENSBI_PATH)
FW:=$(OPENSBI_PATH)/build/platform/$(PLATFORM)/firmware/fw_payload.elf 

$(IMAGE): $(BAO_IMAGE) $(FW) | images 
	cp $(FW) $@

$(FW):
	make -C $(OPENSBI_PATH) PLATFORM=$(PLATFORM) \
    FW_PAYLOAD=y FW_PAYLOAD_PATH=$(BAO_IMAGE)

$(BAO_IMAGE): $(BAO_BIN) $(BAO_CONFIG)
	make -C $(BAO_PATH)/tools/baoloader ARCH=rv64 PLAT=$(PLATFORM) CONFIG=$(CONFIG) TARGET=$(TARGET)

$(BAO_BIN) $(BAO_CONFIG):
	make -C $(BAO_PATH) PLATFORM=$(PLATFORM) CONFIG=$(CONFIG) 

images:
	mkdir -p $@

clean:
	make -C $(BAO_PATH) clean CONFIG=$(CONFIG) 
	make -C $(FW_DIR) distclean
	rm -rf images

.PHONY: $(BAO_BIN) $(BAO_CONFIG) $(FW) clean
