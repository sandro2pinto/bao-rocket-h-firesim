#include <config.h>

VM_IMAGE(linux_image, /home/josecm/code/linux/lloader/linux-rv64-rocket-firesim.bin);
VM_IMAGE(interf_image, /home/josecm/code/bao-baremetal-guest/build/rocket-firesim/bao_bare-metal_guest.bin);

struct vm_config vm_config = {
    
    VM_CONFIG_HEADER
    
    .vmlist_size = 2,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x80200000,
                        .size = 0x10000000
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {
                    {
                        .pa = 0x54000000,
                        .va = 0x54000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {2}
                    }
                },

                .arch = {
                    .plic_base = 0xc000000,
                }
            },
        },
        { 
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(interf_image),
                .size = VM_IMAGE_SIZE(interf_image)
            },

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 3,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x80200000,
                        .size = 0x08000000
                    }
                },

                .arch = {
                    .plic_base = 0xc000000,
                }
            },
        },
     }
};
