# 编译器设置
CXX := E:\CodeBlocks\MinGW\bin\g++.exe
CXXFLAGS := -g -m64

# 源文件目录
SRCDIR := src

# 包含文件目录
INCDIR := include

# 目标文件目录
BUILDDIR := build

# 源文件列表
SRCS := $(wildcard $(SRCDIR)/*.cpp)

# 中间目标文件列表（将每个源文件的路径替换为目标文件路径）
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# 目标可执行文件
TARGET := program.exe


# 默认目标
all: create_build_dir $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 生成目标文件
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# 创建目标文件目录
create_build_dir:
	if not exist $(BUILDDIR) mkdir $(BUILDDIR)

# 清理生成的文件
clean:
	del /s /q $(BUILDDIR)\* $(TARGET)
	if exist $(BUILDDIR) rmdir /s /q $(BUILDDIR)

run:
	.\program.exe

.PHONY: all clean create_build_dir
