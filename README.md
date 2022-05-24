RZ/G2L Firmware Updater Application (RZ/G2L NS_BL2U)
==========================================

<Div Align="right">
Renesas Electronics Corporation

May-24-2022
</Div>


This git contains source code for RZ/G2L Firmware Updater Application.

## 1. Overview
The RZ/G2L NS_BL2U is used to firmware update process in RZ environment.
The RZ/G2L NS_BL2U is responsible for loading the firmware and save it into Non-Volatile memories such as SPI flash.

### 1.1. License

  - BSD-3-Clause (please see file [LICENSE](LICENSE) for the details)

### 1.2. Notice

The RZ/G2L NS_BL2U is distributed as a sample software from Renesas without any warranty or support.

## 1.3. Contributing

To contribute to this software, you should email patches to renesas-rz@renesas.com. 
Please send .patch files as email attachments, not embedded in the email body.

## 1.5. References
The following table shows the document related to this function.

### Related Document
| Number | Issuer  | Title                                                       | Edition           |
|--------|---------|-------------------------------------------------------------|-------------------|
| 1      | Renesas | Security Package for RZ MPU, Security User's Manual         | Rev.2.10 or later |
| 2      | Renesas | Release Note for Verified Linux Package for 64bit kernel    | Rev.1.01 or later |
| 3      | Renesas | RZ/G2L Yocto recipe Start-Up Guide                          | Rev.1.01 or later |


## 2. Operating Environment

### 2.1. Hardware Environment

The following table lists the hardware needed to use this function.
#### Hardware environment

| Name         | Note                                              |
| ------------ | ------------------------------------------------- |
| Target board | RZ/G2L SMARC Concrete Evaluation Kit(RZ/G2L EVK)  |
|              | RZ/G2L SMARC PMIC Evaluation Kit(RZ/G2L PMIC EVK) |
|              | RZ/V2L SMARC Concrete Evaluation Kit(RZ/V2L EVK)  |
|              | RZ/V2L SMARC PMIC Evaluation Kit(RZ/V2L PMIC EVK) |
|              | RZ/G2LC SMARC Evaluation Kit(RZ/G2LC EVK)         |
|              | RZ/G2UL SMARC Evaluation Kit(RZ/G2UL EVK)         |
| Host PC      | Ubuntu Desktop 20.04(64bit) or later              |


The following table shows Serial Flash and eMMC support for each MPU.
##### Serial Flash / eMMC support status of each MPU

| MPU             | Serial Flash  | eMMC          |
| --------------- | ------------- | ------------- |
| RZ/G2L EVK      | Support       | Unsupported   |
| RZ/G2L PMIC EVK | Support       | Unsupported   |
| RZ/V2L EVK      | Support       | Unsupported   |
| RZ/V2L PMIC EVK | Support       | Unsupported   |
| RZ/G2LC EVK     | Support       | Unsupported   |
| RZ/G2UL EVK     | Support       | Unsupported   |

### 2.2. Software Environment

The following table lists the software required to use this sample software.

### Software environment

| Name                             | Note                                                    |
| -------------------------------- | ------------------------------------------------------- |
| ARM64 Cross-compile Toolchain *1 | ARM64 Cross-compile Toolchain Release GCC v10.2         |
| Yocto SDK *1 *2                  | Yocto SDK built from Yocto environment for RZ/G2L Group |

\*1: One of the above environments is required.

\*2: Regarding how to get the Yocto SDK, refer to [Related Document](#related-document) No.2 or No.3.

## 3. Software

### 3.1. Software Function
The RZ/G2L NS_BL2U will Load the firmware update image and replaced it to the old firmware. 
After that, the RZ/G2L NS_BL2U will request to BL2 for reset the system via SMC call.

### 3.2. Build instructions
The following is the method to build the RZ/G2L NS_BL2U.

#### 3.2.1 Prepare the compiler
Gets cross compiler for linaro or setup the Yocto SDK.

ARM64 Cross-compile toolchain:

```shell
cd ~/
wget https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-aarch64-none-elf.tar.xz
tar xvf gcc-arm-10.2-2020.11-x86_64-aarch64-none-elf.tar.xz
```

Yocto SDK:

```shell
source /opt/poky/3.1.5/environment-setup-aarch64-poky-linux
```

#### 3.2.2.  Prepare the source code
Get the source code of the RZ/G2L NS_BL2U.

```shell
cd ~/
git clone https://github.com/renesas-rz/rzg_tf-a_ns-bl2u.git
cd rzg_tf-a_ns-bl2u
git checkout rz_g2l
```

#### 3.2.3. Build the RZ/G2L NS_BL2U
Binary file will be built by the following command.

ARM64 Cross-compile toolchain:

```shell
make clean
CROSS_COMPILE=~/gcc-arm-10.2-2020.11-x86_64-aarch64-none-elf/bin/aarch64-none-elf- make PLAT=rz
```

Yocto SDK:

```shell
make clean
make PLAT=rz
```

Output image will be available in the following directory.

* ./out/ns_bl2u.bin

### 3.3. How to run

In the firmware update implementation in the Security Package for the RZ MPU, 
BL2 loads BL1 and NS_BL2U into RAM when it detects the firmware update boot flow.
BL1 is run by BL2 and NS_BL2U is run by BL1.
For more information of Firmware Update implementation, refer to the References Document No.1.

## 4. Revision history

Describe the revision history of the RZ/G2L NS_BL2U.

### 4.1. v1.00

- First release.
