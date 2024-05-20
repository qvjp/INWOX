# INWOX

![Build INWOX](https://github.com/qvjp/INWOX/workflows/Build%20INWOX/badge.svg)

又一次C语言OS尝试，看看这次能走多远。

## 构建开发环境

Linux环境（Ubuntu 24.04测试通过）

1. 安装依赖

    ```bash
    sudo apt install automake build-essential bison flex git grub-pc-bin libgmp3-dev libisl-dev libmpc-dev libmpfr-dev mtools qemu-system-x86 texinfo xorriso
    ```

2. 下载并构建交叉编译环境

    ```bash
    make install-toolchain
    ```

3. 或使用预构建Docker镜像进行开发

    ```bash
    docker run --name inwox-devbox -it qvjp/inwox-devbox
    ```

## 构建

```
make
```

## 运行

```bash
make qemu-curses
```

## 调试

1. 以debug模式运行

    ```bash
    make qemu-curses-dbg
    ```

2. 使用gdb连接qemu

    ```bash
    gdb #需要在INWOX根目录输入gdb
    ```

3. 使用gdb正常的调试方式进行调试
