CC = gcc
CFLAGS = -Wall -Wextra -pthread
INCLUDES = -I./include

# ソースファイルのディレクトリ
SRC_DIR = src
BIN_DIR = bin

# 各パターンのソースファイル
SINGLE_PROCESS_SINGLE_THREAD_SRC = $(SRC_DIR)/single_process_single_thread/server.c $(SRC_DIR)/single_process_single_thread/client.c
SINGLE_PROCESS_MULTI_THREAD_SRC = $(SRC_DIR)/single_process_multi_thread/server.c $(SRC_DIR)/single_process_multi_thread/client.c
MULTI_PROCESS_SINGLE_THREAD_SRC = $(SRC_DIR)/multi_process_single_thread/server.c $(SRC_DIR)/multi_process_single_thread/client.c
MULTI_PROCESS_MULTI_THREAD_SRC = $(SRC_DIR)/multi_process_multi_thread/server.c $(SRC_DIR)/multi_process_multi_thread/client.c

# 実行ファイルのターゲット
TARGETS = \
    $(BIN_DIR)/single_process_single_thread/server \
    $(BIN_DIR)/single_process_single_thread/client \
    $(BIN_DIR)/single_process_multi_thread/server \
    $(BIN_DIR)/single_process_multi_thread/client \
    $(BIN_DIR)/multi_process_single_thread/server \
    $(BIN_DIR)/multi_process_single_thread/client \
    $(BIN_DIR)/multi_process_multi_thread/server \
    $(BIN_DIR)/multi_process_multi_thread/client

.PHONY: all clean

all: $(TARGETS)

# シングルプロセス・シングルスレッド
$(BIN_DIR)/single_process_single_thread/server: $(SRC_DIR)/single_process_single_thread/server.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(BIN_DIR)/single_process_single_thread/client: $(SRC_DIR)/single_process_single_thread/client.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

# シングルプロセス・マルチスレッド
$(BIN_DIR)/single_process_multi_thread/server: $(SRC_DIR)/single_process_multi_thread/server.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(BIN_DIR)/single_process_multi_thread/client: $(SRC_DIR)/single_process_multi_thread/client.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

# マルチプロセス・シングルスレッド
$(BIN_DIR)/multi_process_single_thread/server: $(SRC_DIR)/multi_process_single_thread/server.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(BIN_DIR)/multi_process_single_thread/client: $(SRC_DIR)/multi_process_single_thread/client.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

# マルチプロセス・マルチスレッド
$(BIN_DIR)/multi_process_multi_thread/server: $(SRC_DIR)/multi_process_multi_thread/server.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(BIN_DIR)/multi_process_multi_thread/client: $(SRC_DIR)/multi_process_multi_thread/client.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -rf $(BIN_DIR) 