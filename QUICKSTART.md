# 快速入门指南

5 分钟上手 eBPF Time Machine！

## 📦 安装

### Ubuntu/Debian

```bash
# 安装依赖
sudo apt-get update
sudo apt-get install -y clang llvm libbpf-dev linux-tools-common linux-tools-generic gcc make

# 克隆项目
git clone https://github.com/your-username/ebpf-time-machine.git
cd ebpf-time-machine

# 构建
make
```

### Fedora/RHEL

```bash
# 安装依赖
sudo dnf install -y clang llvm libbpf-devel bpftool gcc make

# 克隆项目
git clone https://github.com/your-username/ebpf-time-machine.git
cd ebpf-time-machine

# 构建
make
```

## 🚀 第一次运行

### 方式 1：使用演示程序

```bash
# 运行自动演示
sudo ./demo.sh
```

### 方式 2：手动运行

```bash
# 1. 启动演示程序
./build/crash_demo &

# 2. 获取 PID
DEMO_PID=$(pgrep crash_demo)

# 3. 开始记录
sudo ./build/timemachine record $DEMO_PID

# 4. 等待崩溃发生，Time Machine 会自动检测并显示时间戳

# 5. 时间倒带（使用上一步显示的时间戳）
sudo ./build/timemachine rewind <timestamp>
```

## 📖 基本用法

### 追踪任意程序

```bash
# 启动你的程序
./your_program &

# 获取 PID
PID=$(pgrep your_program)

# 开始追踪
sudo ./build/timemachine record $PID
```

### 查看事件

当程序崩溃时，Time Machine 会显示：

```
═══════════════════════════════════════════════════
🚨 CRASH DETECTED!
═══════════════════════════════════════════════════
Timestamp: 1234567890123456
Signal: SIGSEGV
PID: 12345

Use rewind:
  ./timemachine rewind 1234567890123456
═══════════════════════════════════════════════════
```

### 时间倒带

```bash
sudo ./build/timemachine rewind 1234567890123456
```

这会显示崩溃前后 5 秒的所有事件。

## 🔍 常见场景

### 场景 1：调试 Segmentation Fault

```bash
# 你的程序崩溃了
./buggy_program
# Segmentation fault (core dumped)

# 使用 Time Machine
./buggy_program &
sudo ./build/timemachine record $(pgrep buggy_program)

# 查看崩溃前发生了什么
sudo ./build/timemachine rewind <timestamp>
```

### 场景 2：检测内存泄漏

```bash
# 追踪程序
sudo ./build/timemachine record <pid>

# 按 Ctrl+C 停止
# Time Machine 会自动分析内存泄漏

# 输出示例：
# Total allocations: 1000
# Total frees: 950
# Potentially leaked: 50
# ⚠️  Possible memory leak detected!
```

### 场景 3：分析 Use-After-Free

```bash
# Time Machine 会显示：
    1234567885000000  ALLOC   0x7f1234567890  size=1024
    1234567887000000  FREE    0x7f1234567890  (freed)
>>> 1234567890123456  CRASH   SIGSEGV         sig=11

# 清楚地看到内存被释放后又被访问
```

## ⚙️ 配置选项

### 调整事件缓冲区大小

编辑 `src/main.c`：

```c
#define MAX_EVENTS 100000  // 增加到 200000
```

### 调整时间窗口

编辑 `src/main.c`：

```c
#define WINDOW_SEC 5  // 改为 10 秒
```

### 非 root 运行

```bash
# 设置 capabilities
sudo make install

# 现在可以不用 sudo
./build/timemachine record <pid>
```

## 📊 理解输出

### 事件类型

- `FUNC_ENTRY`：函数调用
- `FUNC_EXIT`：函数返回
- `ALLOC`：内存分配（malloc/calloc）
- `FREE`：内存释放
- `SYSCALL`：系统调用
- `FILE`：文件操作
- `💥CRASH`：程序崩溃

### 时间戳

时间戳是纳秒级的系统启动时间：

```
1234567890123456 = 1234567890.123456 秒
```

### 地址

显示的地址是虚拟内存地址：

```
0x7f1234567890  # 典型的堆地址
0x555555554abc  # 典型的代码地址
```

## 🎯 下一步

- 阅读 [README.md](README.md) 了解更多功能
- 查看 [HACKATHON.md](HACKATHON.md) 了解技术细节
- 阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 参与贡献

## ❓ 遇到问题？

查看 [故障排除指南](TROUBLESHOOTING.md) 或提交 Issue。

---

祝你调试愉快！⏰✨
