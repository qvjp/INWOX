# INWOX

![Build INWOX](https://github.com/qvjp/INWOX/workflows/Build%20INWOX/badge.svg)
[![Build Status](https://travis-ci.org/qvjp/INWOX.svg?branch=master)](https://travis-ci.org/qvjp/INWOX)

又一次C语言OS尝试，看看这次能走多远。

## 构建开发环境

Linux环境（Ubuntu 20.04测试通过）

1. 安装依赖

    `sudo apt install git build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev grub-pc-bin xorriso qemu-system-x86`

2. 下载并构建交叉编译环境

    `make install-toolchain`

## 构建

`make`

## 运行

`make qemu-curses`

## 调试

1. 以debug模式运行

    `make qemu-curses-dbg`

2. 使用gdb连接qemu

    `gdb`（需要在INWOX根目录输入gdb）

3. 使用gdb正常的调试方式进行调试
