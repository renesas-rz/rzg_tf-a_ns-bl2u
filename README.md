Trusted Firmware-A(TF-A) Firmware Updater Application(NS_BL2U)
==========================================

<Div Align="right">
Renesas Electronics Corporation

Jul-06-2021
</Div>


This git contains source code for Trusted Firmware-A Firmware Updater Application.\
Currently the following boards and MPUs are supported:

- Board: EK874 / MPU: R8A774C0 (RZ/G2E)
- Board: HIHOPE-RZG2M / MPU: R8A774A1 (RZG2M)
- Board: HIHOPE-RZG2N / MPU: R8A774B1 (RZG2N)
- Board: HIHOPE-RZG2H / MPU: R8A774E1 (RZG2H)

## 1. Overview
TF-A NS_BL2U  is used to firmware update process in TEE for RZ/G2 environment
 .TF-A NS_BL2U is responsible for loading the complete SoC update firmware and save it into Non-Volatile memories such as SPI flash.

### 1.1. License

  - BSD-3-Clause (please see file [LICENSE](LICENSE) for the details)

### 1.2. Notice

RZ/G2 TF-A NS_BL2U is distributed as a sample software from Renesas without any warranty or support.

## 1.3. Contributing

To contribute to this software, you should email patches to renesas-rz@renesas.com. Please send .patch files as email attachments, not embedded in the email body.

## 1.5. References

The following table shows the document related to this function.

| Number | Issuer  | Title                                                          | Edition           |
|--------|---------|----------------------------------------------------------------|-------------------|
| 1      | Renesas | RZ/G2 Trusted Execution Environment Start-Up Guide             | Rev.1.10 or later |
| 2      | Renesas | RZ/G2 Trusted Execution Environment Porting Guide              | Rev.1.10 or later |


## 2. Operating Environment

### 2.1. Hardware Environment

The following table lists the hardware needed to use this utility.

__Hardware environment__

| Name         | Note                                        |
|--------------|---------------------------------------------|
| Target board | Hoperun HiHope RZ/G2[M,N,H] platform        |
|              | Silicon Linux RZ/G2E evaluation kit (EK874) |

### 2.2. Software Environment

The RZ/G Firmware Update TA must be executed on the TEE for RZ/G2 Environment.

For building TEE for RZ/G2 enviroment , refer to References Document 1.


## 3. Software

### 3.1. Software Function
TF-A NS_BL2U will Load the firmware updates which it were re-encrypted by Device-Specitify key and replaced it to the old firmware. After that, it will proceed to delete update firmware data. Finally, TF-A NS_BL2U will request to BL2 for reset the system via SMC call.
### 3.2. Build instructions
The following is the method to build the TF-A NS_BL2U.

#### 3.2.1 Prepare the compiler
Gets cross compiler for linaro or setup the Yocto SDK.

Linaro toolchain:

```shell
cd ~/
wget https://releases.linaro.org/components/toolchain/binaries/7.3-2018.05/aarch64-elf/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-elf.tar.xz
tar xvf gcc-linaro-7.3.1-2018.05-x86_64_aarch64-elf.tar.xz
```

Yocto SDK:

```shell
source <SDK>/environment-setup-aarch64-poky-linux
```

#### 3.2.2. Clone TF-A NS_BL2U

```bash
$ git clone https://github.com/renesas-rz/rzg_tf-a_ns-bl2u.git
$ cd rzg_tf-a_ns-bl2u
$ git checkout -b v1.01
```
#### 3.2.3. Build the TF-A NS_BL2U

binary file will be built by the following command.

Linaro toolchain:

```shell
make clean
export CROSS_COMPILE=~/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-elf/bin/aarch64-elf- 
make PLAT=rzg
```

Yocto SDK:

```shell
make clean
make PLAT=rzg
```

Output image will be available in the following directory.

* ./out/ns_bl2u.bin

### 3.3. How to run

In the Firmware Update implementation on TEE for RZ/G2 environment, TF-A NS_BL2U is loading and executing by BL2 when the fimrware update boot flow is detected. For more information of Firmware Update implementation, refer to the References Document No.2.

## 4. Revision history

Describe the revision history of RZ/G TF-A NS_BL2U.

### 4.1. v1.00

- First release.

### 4.2. v1.01

- SPI driver: Fix unalignment access.

