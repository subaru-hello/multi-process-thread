#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define PORT 8081
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

// デバッグモードの有効化
#define DEBUG_MODE 1

// エラーハンドリング用のマクロ
#define ERROR_EXIT(msg) do { \
    perror(msg); \
    exit(EXIT_FAILURE); \
} while(0)

// ログ出力用のマクロ
#define LOG_INFO(fmt, ...) \
    printf("[INFO] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    printf("[ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// パケットデバッグ用のマクロ
#ifdef DEBUG_MODE
#define LOG_PACKET(direction, data, len) do { \
    printf("[PACKET] %s: ", direction); \
    for (size_t i = 0; i < len; i++) { \
        printf("%02x ", (unsigned char)data[i]); \
    } \
    printf("\n"); \
    printf("[PACKET] %s (ASCII): %.*s\n", direction, (int)len, data); \
} while(0)
#else
#define LOG_PACKET(direction, data, len)
#endif

// クライアント情報を保持する構造体
typedef struct {
    int socket_fd;
    struct sockaddr_in address;
    socklen_t addr_len;
} client_info_t;

#endif // COMMON_H 