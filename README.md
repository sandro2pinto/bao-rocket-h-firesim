# Bao Hypervisor - Rocket chip with H-extension on FireSim

For the sake of simplicity, in this tutorial, we refer to the Rocket chip extended with the hypervisor extensions as Rocket-H.



# 0 - Setting up the Toolchain

For all software (except Linux) you should use the *riscv64-unkowen-elf-* gcc 8.3.0 2020.04.0.

You can donwload from: 

https://static.dev.sifive.com/dev-tools/riscv64-unknown-elf-gcc-8.3.0-2020.04.0-x86_64-linux-ubuntu14.tar.gz.


Specifically for building linux, we recommend to use a different toolchain: *riscv64-linux-* gcc version 9.3.0 2020.02-2

You can downloaded from:

https://toolchains.bootlin.com/downloads/releases/toolchains/riscv64/tarballs/riscv64--glibc--bleeding-edge-2020.02-2.tar.bz2.

Setup ARCH and your RISC-V toolchain prefix:

`export ARCH=riscv`\
`export CROSS_COMPILE=toolchain-prefix-`

Repeat these steps every time you start a new terminal.

For the remaining of this tutorial, for every path starting with */path/to/*, please replace by the corresponding absolute path on your machine.



# 1 - Compiling the Software (Guests / Linux, Bao, and openSBI)

## 1.1 - Guest Bare-Metal Application

Download and build the bare-metal guest:

`git clone https://github.com/bao-project/bao-baremetal-guest`\
`cd bao-baremetal-guest`\
`git checkout rocket`\
`make PLATFORM=rocket-firesim`

## 1.2 - Linux

**Note**: The following steps shall be done using the *riscv64-linux-* toolchain.

Clone Linux, defconfig and apply sifive uart patch:

`git clone https://github.com/torvalds/linux.git --depth 1 --branch=v5.9 linux-5.9`\
`cd linux-5.9`\
`make defconfig`\
`git apply /path/to/patches/0001-force-sifive-uart-config.patch`

Enable the following configs in `make menuconfig`:


- **SERIAL_SIFIVE**
(Device Drivers -> Character devices -> Serial drivers -> SiFive UART support)
- **SERIAL_SIFIVE_CONSOLE**
(Device Drivers -> Character devices -> Serial drivers -> SiFive UART support -> Console on SiFive UART)
- **HVC_RISCV_SBI**
(Device Drivers -> Character devices -> RISC-V SBI console support)
- set **INITRAMFS_SOURCE** to */path/to/linux/initramfs.cpio*
(General Setup -> Initial RAM filesystem and RAM disk (initramfs/initrd) support)

**Note**: You can search for symbols using the character '/' and jump directly into the option by using the numbers (e.g., 1, 2, ...)

Build Linux:

`make -j$(nproc)`

Build the device tree:

`cd /path/to/this/repo/linux`\
`dtc rocket-firesim-minimal.dts > rocket-firesim-minimal.dtb`

**Note**: The following steps shall be done using the *riscv64-unkowen-elf-* toolchain.

And build the final guest images by concatening the minimal bootloader, linux and device tree binaries:

`cd /path/to/this/repo/linux/lloader`\
`make ARCH=rv64 IMAGE=/path/to/linux-5.9/arch/riscv/boot/Image DTB=/path/to/this/repo/linux/rocket-firesim-minimal.dtb TARGET=linux-rv64-rocket-firesim`

## 1.3 - OpenSBI

Download opensbi and checkout the firesim branch:

`git clone https://github.com/josecm/opensbi.git`\
`cd opensbi`\
`git checkout josecm/rocket`

**Note**: OpenSBI will be compiled at later stage.

## 1.4 -  Bao

Clone Bao and checkout the RISC-V branch with support for rocket-chip:

`git clone https://github.com/bao-project/bao-hypervisor`\
`cd bao-hypervisor`\
`git checkout wip/riscv-rocket`

Copy the provided external tools to the bao-hypervisor dir:

`cp -r /path/to/this/repo/bao/tools .`
 
Copy the provided configs to bao's directory:

`cp -r /path/to/this/repo/linux/bao/configs .`

For the target configuration file (*configs/xxxconfig/config.c*), setup the absolute path for the VM image. 

For example, for running Linux as VM:

VM_IMAGE(linux_image, /path/to/linux/lloader/linux-rv64-rocket-firesim.bin);


## 1.5 - Build final system image (openSBI + Bao + Guests)

Build the system image:

`cd /path/to/bao-hypervisor`\
`make -C tools/firesim CONFIG=rocket-firesim-xxx`

At this stage, the final system image is ready to be sent to the AWS machine (FireSim).


# 2 - Building your Rocket-H design

**Note**: There is a public afi for "our" Rocket-H design. In case the public afi is used 
(see details below), you may skip Step 2.2 "Configure useHype parameter on Boom and Ariane" 

**GLOBAL AFI**
`agfi-09ca752d0efd62a14`\
`(aws ec2 describe-fpga-images --filters Name=fpga-image-global-id,Values=agfi-09ca752d0efd62a14)`

**EU-WEST-1 AFI**
`afi-0c7c2676e01a2edf2`\
`(aws ec2 describe-fpga-images --region eu-west-1 --fpga-image-ids afi-0c7c2676e01a2edf2)`

**US-WEST-2 AFI**
`afi-08045dd2d8a3c7571`\
`(aws ec2 describe-fpga-images --region us-west-2 --fpga-image-ids afi-08045dd2d8a3c7571)`

**Other Regions**
Please get in touch and we will make the AFI publicly available to your region.


For FireSim Initial Setup/Installation please refer to the FireSim documentation:

https://docs.fires.im/en/latest/Initial-Setup/index.html

**Note**: While selecting the AMI, you can use the FPGA Developer AMI - 1.6.1 

**This tutorial expects the following Firesim and Chipyard versions (commit):**
firesim - eb7d1af4964814270b176d4e682fc999e8db561a
chipyard - 0e0c89cb1fa64eda74fa8e7dac438eae54a9df56


## 2.1 - Add Rocket-H to Chipyard

On your AWS machine, add our rocket-chip design:

`cd firesim/target-design/chipyard/generators/rocket-chip/`\
`git remote add josecm https://github.com/josecm/rocket-chip.git`\
`git fetch josecm`\
`git checkout hyp`


## 2.2 - Configure useHype parameter on Boom and Ariane

**Note**: You should install nano (or other editor) on your AWS machine to perform the following steps

`sudo yum install nano`

### 2.2.1 - Boom

Edit the 'parameters.scala' file to include the useHype parameter:

firesim/target-design/chipyard/generators/boom/src/main/scala/common/parameters.scala

`useSupervisor: Boolean = false,`\
**`useHype: Boolean = false,`**\
`useVM: Boolean = true,`

### 2.2.2 - Ariane

Edit the 'ArianeTile.scala' file to include the useHype parameter:

firesim/target-design/chipyard/generators/ariane/src/main/scala/ArianeTile.scala

`'val useSupervisor: Boolean = false'`\
**`'val useHype: Boolean = false'`**\
`'val useDebug: Boolean = true'`


## 2.3 - Extend Chipyard rocket configurations 

Edit the 'RocketConfigs.scala' file to define the QuadRocketHypConfig class:

firesim/target-design/chipyard/generators/chipyard/src/main/scala/config/RocketConfigs.scala

**`class QuadRocketHypConfig extends Config(`**\
**`  new freechips.rocketchip.subsystem.WithHyp ++`**\
**`  new QuadRocketConfig)`**

**Note**: As a sugestion, add the class at the top of the file

Edit the 'TargetConfigs.scala' file to include the QuadRocketHypConfig class:

firesim/target-design/chipyard/generators/firechip/src/main/scala/TargetConfigs.scala

**`class FireSimQuadRocketHypConfig extends Config(`**\
**`  new WithDefaultFireSimBridges ++`**\
**`  new WithDefaultMemModel ++`**\
**`  new WithFireSimConfigTweaks ++`**\
**` new chipyard.QuadRocketHypConfig)`**

**Note**: As a sugestion, add the class after the definition of the FireSimQuadRocketConfig class


## 2.4 - Build the Rocket-H image 

### 2.4.1 - Build configuration

Edit the build configuration, per instructions described in FireSim documentation:

firesim/deploy/config_build.ini

https://docs.fires.im/en/latest/Building-a-FireSim-AFI.html

### 2.4.2 - Build recipe

Edit the build recipe file to include the Rocket-H configuration: 

firesim/deploy/config_build_recipes.ini

`# Quad-core, Rocket-based recipes`\
`[firesim-rocket-quadcore-nic-l2-llc4mb-ddr3]`\
`DESIGN=FireSim`\
`TARGET_CONFIG=WithNIC_DDR3FRFCFSLLC4MB_FireSimQuadRocketConfig`\
`PLATFORM_CONFIG=F90MHz_BaseF1Config`\
`instancetype=z1d.2xlarge`\
`deploytriplet=None`\
\
`[firesim-rocket-quadcore-no-nic-l2-llc4mb-ddr3]`\
`DESIGN=FireSim`\
`TARGET_CONFIG=DDR3FRFCFSLLC4MB_FireSimQuadRocketConfig`\
`PLATFORM_CONFIG=F90MHz_BaseF1Config`\
`instancetype=z1d.2xlarge`\
`deploytriplet=None`\
\
**`[firesim-rocket-h-quadcore-no-nic-l2-llc4mb-ddr3]`**\
**`DESIGN=FireSim`**\
**`TARGET_CONFIG=DDR3FRFCFSLLC4MB_FireSimQuadRocketHypConfig`**\
**`PLATFORM_CONFIG=F90MHz_BaseF1Config`**\
**`instancetype=z1d.2xlarge`**\
**`deploytriplet=None`**

Edit the build file to include the Rocket-H configuration: 

firesim/deploy/config_build.ini

`[builds]`\
`# this section references builds defined in config_build_recipes.ini`\
`# if you add a build here, it will be built when you run buildafi`\
`#firesim-rocket-quadcore-nic-l2-llc4mb-ddr3`\
**`#firesim-rocket-quadcore-no-nic-l2-llc4mb-ddr3`**\
`#firesim-boom-singlecore-no-nic-l2-llc4mb-ddr3`\
`#firesim-boom-singlecore-nic-l2-llc4mb-ddr3`\
**`firesim-rocket-h-quadcore-no-nic-l2-llc4mb-ddr3`**


### 2.4.4 - Run the build

`firesim buildafi`

**Note**: Depending on your AWS instance resources, the build can take around 5-6 hours (i.e., c4.4xlarge)


# 4 - Running your Rocket-H simulation

### 4.1 Setting Up your workload

In ~/firesim/deploy/workloads create a bao directory and a bao.json file with the following content:

```
{
  "benchmark_name"            : "bao",
  "common_bootbinary"         : "bao_rocket-firesim-xxx.elf",
  "common_rootfs"             : "dummy.rootfs",
  "common_simulation_outputs" : ["uartlog"]
}
```

Upload the dummy.rootfs and the final bao binary built in step 1.5 (named **/path/to/bao-hypervisor/tools/firesim/images/bao-rocket-firesim_rocket-firesim-xxx.elf**) to ~/firesim/deploy/workloads/bao in the aws machine.

Finally, please refer to the FireSim documentation "Running FireSim Single Node Simulations" (https://docs.fires.im/en/latest/Running-Simulations-Tutorial/Running-a-Single-Node-Simulation.html) to run the simulation. You'll just need to adapt the defaulthwconfig and workload fields in config_runtime.ini:

```
defaulthwconfig=firesim-rocket-h-quadcore-no-nic-l2-llc4mb-ddr3
...
workloadname=bao.json
```




