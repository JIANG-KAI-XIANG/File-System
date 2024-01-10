
# main.o Memory.o FileSystemTree.o FileSystemManager.o Dump.o UI.o

# Makefile示例

# 定義編譯器
CC = gcc

# 定義編譯參數
CFLAGS = Memory.h FileSystemTree.h FileSystemManager.h Dump.h UI.h

# 定義目標文件
TARGET = APP

# 定義源文件
SOURCE_FILES = main.c Memory.c FileSystemManager.c FileSystemTree.c Dump.c UI.c

# 生成目標文件
$(TARGET): $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE_FILES)

# 清理編譯生成的文件
clean:
	rm -f $(TARGET) *.o *.d core