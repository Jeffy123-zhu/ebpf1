# 故障排除指南

遇到问题？这里有常见问题的解决方案。

## 🔧 构建问题

### 问题：找不到 libbpf

```
fatal error: bpf/libbpf.h: No such file or directory
```

**解决方案**：

```bash
# Ubuntu/Debian
sudo apt-get install libbpf-dev

# Fedora/RHEL
sudo dnf install libbpf-devel

# 从源码安装
git clone https://github.com/libbpf/libbpf.git
cd libbpf/src
make
sudo make install
```

### 问题：找不到 bpftool

```
bpftool: command not found
```

**解决方案**：

```bash
# Ubuntu/Debian
sudo apt-get install linux-tools-common linux-tools-generic

# Fedora/RHEL
sudo dnf install bpftool

# 手动安装
sudo apt-get install linux-tools-$(uname -r)
```

### 问题：clang 版本太旧

```
error: unknown argument: '-target bpf'
```

**解决方案**：

需要 clang 10 或更高版本：

```bash
# Ubuntu/Debian
sudo apt-get install clang-12 llvm-12

# 更新 Makefile 中的 CLANG 变量
CLANG ?= clang-12
```

### 问题：BTF 不可用

```
libbpf: failed to find valid kernel BTF
```

**解决方案**：

1. 检查内核是否支持 BTF：
```bash
ls /sys/kernel/btf/vmlinux
```

2. 如果不存在，升级内核到 5.8+：
```bash
# Ubuntu
sudo apt-get install linux-generic-hwe-20.04
```

3. 或者禁用 BTF（性能会下降）：
```bash
# 在 tracer.bpf.c 中添加
#define BPF_NO_PRESERVE_ACCESS_INDEX
```

## 🚫 权限问题

### 问题：需要 root 权限

```
Error: Failed to load BPF
Permission denied
```

**解决方案**：

方式 1：使用 sudo
```bash
sudo ./build/timemachine record <pid>
```

方式 2：设置 capabilities
```bash
sudo make install
# 或
sudo setcap cap_sys_admin,cap_bpf,cap_perfmon=eip build/timemachine
```

方式 3：添加到 bpf 组（某些发行版）
```bash
sudo usermod -aG bpf $USER
# 重新登录
```

### 问题：无法追踪进程

```
Error: Failed to attach
```

**解决方案**：

1. 确认进程存在：
```bash
ps -p <pid>
```

2. 确认有权限追踪：
```bash
# 检查 ptrace_scope
cat /proc/sys/kernel/yama/ptrace_scope
# 如果是 1 或更高，临时设置为 0
sudo sysctl kernel.yama.ptrace_scope=0
```

3. 确认不是内核进程：
```bash
# Time Machine 只能追踪用户态进程
```

## 💾 内存问题

### 问题：内存不足

```
Error: Failed to create ring buffer
Cannot allocate memory
```

**解决方案**：

1. 增加 memlock 限制：
```bash
# 临时
sudo ulimit -l unlimited

# 永久（编辑 /etc/security/limits.conf）
* soft memlock unlimited
* hard memlock unlimited
```

2. 减小 ring buffer 大小（编辑 `src/bpf/events.h`）：
```c
struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 128 * 1024 * 1024);  // 从 256MB 减到 128MB
} events SEC(".maps");
```

3. 减少事件存储（编辑 `src/main.c`）：
```c
#define MAX_EVENTS 50000  // 从 100000 减到 50000
```

### 问题：事件丢失

```
[WARNING] Ring buffer full, events may be lost
```

**解决方案**：

1. 增加 ring buffer 大小（见上）

2. 增加轮询频率（编辑 `src/main.c`）：
```c
// 在 ring_buffer__poll 调用中
ring_buffer__poll(rb, 10);  // 从 100ms 改为 10ms
```

3. 减少追踪的事件类型

## 🐛 运行时问题

### 问题：没有捕获到事件

```
[0 events] 0.0 MB
```

**解决方案**：

1. 确认 PID 正确：
```bash
ps aux | grep your_program
```

2. 确认程序在运行：
```bash
kill -0 <pid>  # 如果返回错误，进程不存在
```

3. 检查 BPF 程序是否加载：
```bash
sudo bpftool prog list
```

4. 检查 BPF 日志：
```bash
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

### 问题：崩溃未被检测

```
Program crashed but Time Machine didn't detect it
```

**解决方案**：

1. 确认信号类型被追踪（编辑 `src/bpf/tracer.bpf.c`）：
```c
// 添加更多信号
if (sig != 11 && sig != 6 && sig != 4 && sig != 8 && sig != 7)
    return 0;
```

2. 检查信号是否被程序捕获：
```bash
# 如果程序有自己的信号处理器，可能会拦截信号
```

3. 使用 strace 确认信号：
```bash
strace -p <pid>
```

### 问题：时间倒带没有显示事件

```
No events found in time window
```

**解决方案**：

1. 增加时间窗口（编辑 `src/main.c`）：
```c
#define WINDOW_SEC 10  // 从 5 秒增加到 10 秒
```

2. 确认时间戳正确：
```bash
# 时间戳应该是纳秒级的
# 正确：1234567890123456
# 错误：1234567890
```

3. 检查事件是否被记录：
```bash
# 在 record 模式下应该看到事件计数增加
```

## 🔍 调试技巧

### 启用详细日志

编辑 `src/main.c`，添加调试输出：

```c
#define DEBUG 1

#ifdef DEBUG
#define debug_print(fmt, ...) \
    fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif
```

### 检查 BPF 验证器错误

```bash
# 查看详细的 BPF 加载日志
sudo bpftool prog load build/tracer.bpf.o /sys/fs/bpf/tracer
```

### 使用 bpftrace 测试

```bash
# 快速测试 BPF 功能
sudo bpftrace -e 'uprobe:/lib/x86_64-linux-gnu/libc.so.6:malloc { printf("malloc called\n"); }'
```

## 📊 性能问题

### 问题：CPU 使用率过高

**解决方案**：

1. 减少追踪的事件类型
2. 增加采样间隔
3. 使用过滤器只追踪特定函数

### 问题：程序运行变慢

**解决方案**：

1. 这是正常的，eBPF 会有一定开销
2. 在生产环境使用时，只追踪必要的事件
3. 考虑使用采样而不是追踪所有事件

## 🆘 获取帮助

如果以上方法都不能解决问题：

1. **查看日志**：
```bash
dmesg | grep -i bpf
journalctl -xe
```

2. **收集信息**：
```bash
uname -a
clang --version
bpftool version
cat /proc/version
```

3. **提交 Issue**：
   - 包含完整的错误信息
   - 包含系统信息
   - 包含重现步骤

4. **社区支持**：
   - GitHub Discussions
   - eBPF Slack
   - Stack Overflow (tag: ebpf)

## 📚 相关资源

- [eBPF 官方文档](https://ebpf.io/)
- [libbpf 文档](https://libbpf.readthedocs.io/)
- [BPF 性能工具](http://www.brendangregg.com/ebpf.html)
- [Linux 内核 BPF 文档](https://www.kernel.org/doc/html/latest/bpf/)

---

还有问题？欢迎提交 Issue！⏰✨
