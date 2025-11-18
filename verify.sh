#!/bin/bash
# verify.sh - 验证项目完整性

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}"
cat << "EOF"
╔═══════════════════════════════════════════════════╗
║     eBPF Time Machine - 项目验证工具              ║
╚═══════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

ERRORS=0
WARNINGS=0

# 检查函数
check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
    else
        echo -e "${RED}✗${NC} $1 ${RED}(缺失)${NC}"
        ((ERRORS++))
    fi
}

check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
    else
        echo -e "${RED}✗${NC} $1/ ${RED}(缺失)${NC}"
        ((ERRORS++))
    fi
}

check_executable() {
    if [ -x "$1" ]; then
        echo -e "${GREEN}✓${NC} $1 ${GREEN}(可执行)${NC}"
    else
        echo -e "${YELLOW}⚠${NC} $1 ${YELLOW}(不可执行)${NC}"
        ((WARNINGS++))
    fi
}

# 检查目录结构
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查目录结构${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_dir "src"
check_dir "src/bpf"
check_dir "examples"
check_dir "build"
check_dir ".github"
check_dir ".github/workflows"

# 检查源代码
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查源代码${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file "src/main.c"
check_file "src/bpf/tracer.bpf.c"
check_file "src/bpf/events.h"
check_file "examples/crash_demo.c"

# 检查构建系统
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查构建系统${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file "Makefile"
check_file "build.sh"
check_file ".gitignore"
check_executable "build.sh"

# 检查核心文档
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查核心文档${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file "README.md"
check_file "README_CN.md"
check_file "HACKATHON.md"
check_file "LICENSE"
check_file "CHANGELOG.md"

# 检查用户指南
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查用户指南${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file "QUICKSTART.md"
check_file "TROUBLESHOOTING.md"
check_file "demo.sh"
check_executable "demo.sh"

# 检查开发文档
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查开发文档${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file "CONTRIBUTING.md"
check_file "TODO.md"
check_file "PROJECT_SUMMARY.md"
check_file "DEMO_SCRIPT.md"

# 检查提交材料
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查提交材料${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file "SUBMISSION_GUIDE.md"
check_file "CHECKLIST.md"
check_file "PROJECT_OVERVIEW.md"

# 检查 CI/CD
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查 CI/CD${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

check_file ".github/workflows/build.yml"

# 检查代码质量
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查代码质量${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# 检查源代码行数
if [ -f "src/main.c" ]; then
    LINES=$(wc -l < src/main.c)
    echo -e "${GREEN}✓${NC} src/main.c: $LINES 行"
fi

if [ -f "src/bpf/tracer.bpf.c" ]; then
    LINES=$(wc -l < src/bpf/tracer.bpf.c)
    echo -e "${GREEN}✓${NC} src/bpf/tracer.bpf.c: $LINES 行"
fi

if [ -f "src/bpf/events.h" ]; then
    LINES=$(wc -l < src/bpf/events.h)
    echo -e "${GREEN}✓${NC} src/bpf/events.h: $LINES 行"
fi

# 检查文档完整性
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}检查文档完整性${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# 检查 README 是否包含关键部分
if [ -f "README.md" ]; then
    if grep -q "Quick Start" README.md || grep -q "快速开始" README.md; then
        echo -e "${GREEN}✓${NC} README.md 包含快速开始"
    else
        echo -e "${YELLOW}⚠${NC} README.md 缺少快速开始"
        ((WARNINGS++))
    fi
    
    if grep -q "Installation" README.md || grep -q "安装" README.md; then
        echo -e "${GREEN}✓${NC} README.md 包含安装说明"
    else
        echo -e "${YELLOW}⚠${NC} README.md 缺少安装说明"
        ((WARNINGS++))
    fi
fi

# 总结
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}验证总结${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

echo ""
if [ $ERRORS -eq 0 ] && [ $WARNINGS -eq 0 ]; then
    echo -e "${GREEN}✅ 完美！项目完整无误！${NC}"
    echo ""
    echo -e "${GREEN}项目已准备好提交！${NC}"
    echo ""
    echo "下一步："
    echo "  1. 录制演示视频（参考 DEMO_SCRIPT.md）"
    echo "  2. 创建 GitHub 仓库"
    echo "  3. 推送代码"
    echo "  4. 填写提交表单"
    echo ""
    echo "详细步骤请查看: SUBMISSION_GUIDE.md"
    exit 0
elif [ $ERRORS -eq 0 ]; then
    echo -e "${YELLOW}⚠ 有 $WARNINGS 个警告${NC}"
    echo ""
    echo "项目基本完整，但有一些小问题需要注意。"
    echo "建议修复警告后再提交。"
    exit 0
else
    echo -e "${RED}✗ 发现 $ERRORS 个错误和 $WARNINGS 个警告${NC}"
    echo ""
    echo "请修复上述问题后再提交。"
    exit 1
fi
