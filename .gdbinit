# GDB初始化代码

python
class AddSymbolFileAuto (gdb.Command):
    def __init__(self):
        super(AddSymbolFileAuto, self).__init__("add-symbol-file-auto", gdb.COMMAND_USER)

    def invoke(self, solibpath, from_tty):
        if os.path.isfile(solibpath):
            self.addSymbolFile(solibpath, from_tty)
        elif os.path.isdir(solibpath):
            for path in self.findAllFile(solibpath):
                self.addSymbolFile(path, from_tty)

    def addSymbolFile(self, path, from_tty):
        offset = self.getTextOffset(path)
        gdb_cmd = "add-symbol-file %s %s" % (path, offset)
        gdb.execute(gdb_cmd, from_tty)

    def getTextOffset(self, solibpath):
        import subprocess
        elfres = subprocess.check_output(["readelf", "-WS", solibpath])
        for line in elfres.splitlines():
            if "] .text " in str(line):
                return "0x" + str(line).split()[5]
        return ""

    def findAllFile(self, base):
        for root, _, fs in os.walk(base):
            for f in fs:
                fullname = os.path.join(root, f)
                if f.endswith('.o') or os.access(fullname, os.X_OK):
                    yield fullname

AddSymbolFileAuto()
end

# 连接qemu
target remote localhost:1234

# 添加kernel调试信息
symbol-file build/kernel.sym

# 添加libc调试信息
add-symbol-file-auto build/libc/

# 根据需要添加用户态程序调试信息
#add-symbol-file-auto build/programs/echo/echo
