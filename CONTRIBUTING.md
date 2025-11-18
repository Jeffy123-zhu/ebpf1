# 贡献指南

感谢你对 eBPF Time Machine 项目的兴趣！我们欢迎各种形式的贡献。

## 🚀 快速开始

1. **Fork 项目**
2. **克隆到本地**：
   ```bash
   git clone https://github.com/your-username/ebpf-time-machine.git
   cd ebpf-time-machine
   ```
3. **创建分支**：
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. **进行修改**
5. **提交更改**：
   ```bash
   git commit -m "Add: your feature description"
   ```
6. **推送分支**：
   ```bash
   git push origin feature/your-feature-name
   ```
7. **创建 Pull Request**

## 📝 贡献类型

### 代码贡献

- 新功能实现
- Bug 修复
- 性能优化
- 代码重构

### 文档贡献

- 改进 README
- 添加使用示例
- 翻译文档
- API 文档

### 测试贡献

- 添加测试用例
- 改进测试覆盖率
- 性能测试

### 其他贡献

- 报告 Bug
- 提出新功能建议
- 改进构建系统
- 优化 CI/CD

## 🔧 开发环境设置

### 前置要求

- Linux 内核 5.8+
- clang/LLVM 10+
- libbpf 0.3+
- bpftool

### 构建项目

```bash
make
```

### 运行测试

```bash
make demo
```

## 📋 代码规范

### C 代码风格

- 使用 4 空格缩进
- 遵循 Linux 内核代码风格
- 使用 clang-format 格式化代码

```bash
clang-format -i src/**/*.c src/**/*.h
```

### eBPF 代码规范

- 保持 eBPF 程序简洁
- 避免复杂的循环
- 注意栈空间限制（512 字节）
- 使用 BPF helper 函数

### 提交信息规范

使用语义化的提交信息：

- `Add: 新功能`
- `Fix: Bug 修复`
- `Docs: 文档更新`
- `Refactor: 代码重构`
- `Test: 测试相关`
- `Perf: 性能优化`

示例：
```
Add: support for network event tracing

- Implement TCP connection tracking
- Add UDP packet monitoring
- Update event structure
```

## 🧪 测试指南

### 单元测试

```bash
# TODO: 添加单元测试框架
```

### 集成测试

```bash
./demo.sh
```

### 性能测试

```bash
# TODO: 添加性能测试脚本
```

## 📚 项目结构

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
├── Makefile                # Make 构建文件
├── README.md               # 项目说明
├── HACKATHON.md            # 黑客松文档
└── CONTRIBUTING.md         # 本文件
```

## 🐛 报告 Bug

使用 GitHub Issues 报告 Bug，请包含：

1. **环境信息**：
   - 操作系统和版本
   - 内核版本
   - 工具链版本

2. **重现步骤**：
   - 详细的操作步骤
   - 预期行为
   - 实际行为

3. **日志和错误信息**：
   - 完整的错误输出
   - 相关日志

4. **最小可重现示例**（如果可能）

## 💡 提出新功能

使用 GitHub Issues 提出新功能建议，请包含：

1. **功能描述**：清晰描述想要的功能
2. **使用场景**：为什么需要这个功能
3. **实现建议**：如果有想法，可以提供实现思路
4. **替代方案**：是否考虑过其他方案

## 🎯 开发路线图

### 短期目标（v1.x）

- [ ] 完善文档
- [ ] 添加更多测试
- [ ] 性能优化
- [ ] Bug 修复

### 中期目标（v2.x）

- [ ] Web UI 可视化
- [ ] 更多事件类型
- [ ] 分布式追踪
- [ ] 配置文件支持

### 长期目标（v3.x）

- [ ] 集成 Cilium
- [ ] AI 辅助分析
- [ ] 云原生支持
- [ ] 商业化功能

## 📞 联系方式

- GitHub Issues: 技术问题和 Bug 报告
- GitHub Discussions: 一般讨论和问答
- Email: [你的邮箱]

## 📄 许可证

通过贡献代码，你同意你的贡献将在 GPL-2.0 许可证下发布。

---

再次感谢你的贡献！让我们一起让调试变得更简单！⏰✨
