# GDB初始化代码

# 连接qemu
target remote localhost:1234

# 添加调试信息
symbol-file build/kernel.sym
