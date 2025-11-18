# ⏰ eBPF Time Machine

**用 eBPF 追踪程序崩溃的时光机器**

这是一个为 eBPF/Cilium 黑客松创建的项目，展示了如何使用 eBPF 技术来调试和分析程序崩溃。

## 🎯 项目简介

Time Machine 是一个基于 eBPF 的调试工具，可以：
- 📹 实时记录程序的所有关键事件（函数调用、内存分配、系统调用等）
- 💥 自动检测程序崩溃（SIGSEGV, SIGABRT 等）
- ⏪ "倒带"到崩溃前的时间点，查看崩溃前后的所有事件
- 🔍 分析内存泄漏和 use-after-free 等问题

## ✨ 特性

- **零开销记录**：使用 eBPF 在内核态高效捕获事件
- **时间旅行**：可以查看崩溃前后任意时间窗口的事件
- **内存追踪**：自动追踪 malloc/free，检测内存泄漏
- **崩溃检测**：自动捕获致命信号并提供详细上下文
- **栈追踪**：记录每个事件的调用栈

## 🚀 快速开始

### 前置要求

- Linux 内核 5.8+ (支持 BTF)
- clang/LLVM
- libbpf
- bpftool

在 Ubuntu/Debian 上安装：
```bash
sudo apt install clang llvm libbpf-dev linux-tools-common linux-tools-generic
```

### 构建

```bash
chmod +x build.sh
./build.sh
```

### 使用示例

1. **运行演示程序**（包含一个 use-after-free bug）：
```bash
./build/crash_demo &
```

2. **启动 Time Machine 记录**：
```bash
sudo ./build/timemachine record $(pgrep crash_demo)
```

3. **等待崩溃发生**，Time Machine 会自动检测并显示：
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

4. **时间倒带**，查看崩溃前后的事件：
```bash
sudo ./build/timemachine rewind 1234567890123456
```

## 📊 输出示例

```
⏰ REWINDING to 1234567890123456
   Showing [-5s to +5s]

Timestamp            PID      Type         Address            Details
────────────────────────────────────────────────────────────
    1234567885000000 12345    ALLOC        0x7f1234567890     size=1024
    1234567886000000 12345    FUNC_ENTRY   0x555555554abc     args=1,0
    1234567887000000 12345    FREE         0x7f1234567890     (freed)
>>> 1234567890123456 12345    💥CRASH      SIGSEGV            sig=11
```

## 🎓 技术细节

### eBPF 程序

- **uprobe/uretprobe**：追踪用户态函数调用
- **tracepoint**：追踪系统调用和信号
- **Ring Buffer**：高效的事件传输机制
- **Stack Traces**：记录调用栈信息

### 事件类型

- `FUNC_ENTRY/EXIT`：函数进入/退出
- `ALLOC/FREE`：内存分配/释放
- `SYSCALL`：系统调用
- `FILE_OP`：文件操作
- `SIGNAL`：信号（崩溃）

## 🏆 黑客松亮点

1. **实用性**：解决真实的调试难题
2. **创新性**：将 eBPF 应用于时间旅行调试
3. **教育性**：展示 eBPF 的强大能力
4. **完整性**：包含演示程序和文档

## 📝 项目结构

```
.
├── src/
│   ├── main.c              # 用户态主程序
│   └── bpf/
│       ├── tracer.bpf.c    # eBPF 追踪程序
│       └── events.h        # 事件结构定义
├── examples/
│   └── crash_demo.c        # 演示程序
├── build.sh                # 构建脚本
└── README.md
```

## 🔧 扩展想法

- 支持更多事件类型（网络、线程等）
- 添加 Web UI 可视化
- 集成 Cilium 网络追踪
- 支持分布式追踪

## 📄 许可证

GPL-2.0 (eBPF 程序要求)

## 👨‍💻 作者

为 eBPF/Cilium 黑客松创建

---

**让调试像时间旅行一样简单！** ⏰✨
