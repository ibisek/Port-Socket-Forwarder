# Port Forwarder

Simple threaded socket-based traffic forwarder for usecases one needs to forward traffic from one machine:port to another one:port in specific environments.

Cross compilation based on [https://electrosome.com/cross-compile-openwrt-c-program/](https://electrosome.com/cross-compile-openwrt-c-program/)

## NEXX WT3020

[https://openwrt.org/toh/nexx/wt3020](https://openwrt.org/toh/nexx/wt3020)

### Setup OpenWRT SDK:

(1) `git clone https://github.com/openwrt/openwrt.git`

(2) `cd openwrt`

(3) `./scripts/feeds update -a`

(4) `./scripts/feeds install -a`

(5) `make menuconfig`
```
  Target system -> MediaTek Ralink MIPS
  Subtarget -> MT7620 based boards
  Target profile -> Nexx WT3020 (8MB)
```

(6) `make`


### Prepare environment

(7) `sudo vim /bin/openwrt.config`
```
# Set up paths and environment for cross compiling for openwrt
export STAGING_DIR=/home/ibisek/temp/openwrt/staging_dir
export TOOLCHAIN_DIR=$STAGING_DIR/toolchain-mipsel_24kc_gcc-7.3.0_musl/
export LDCFLAGS=$TOOLCHAIN_DIR/usr/lib
export LD_LIBRARY_PATH=$TOOLCHAIN_DIR/usr/lib
export PATH=$TOOLCHAIN_DIR/bin:$PATH
```

(8) `mipsel-openwrt-linux-gcc -o socketServerForwarder.bin ./socketServerForwarder.c` 

or just run the 
`crossCompile.sh`

Configure, run & enjoy.

:)
