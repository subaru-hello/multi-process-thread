#include "../../include/common.h"

// シグナルハンドラー
void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pid_t pid;

    // シグナルハンドラーの設定
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        ERROR_EXIT("sigaction failed");
    }

    // ソケットの作成
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        ERROR_EXIT("socket creation failed");
    }

    // サーバーアドレスの設定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // ソケットのバインド
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ERROR_EXIT("bind failed");
    }

    // 接続待ち
    if (listen(server_fd, 5) < 0) {
        ERROR_EXIT("listen failed");
    }

    LOG_INFO("Server started on port %d", PORT);

    while (1) {
        // クライアントからの接続を受け付ける
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            LOG_ERROR("accept failed");
            continue;
        }

        LOG_INFO("New connection from %s:%d", 
                 inet_ntoa(client_addr.sin_addr), 
                 ntohs(client_addr.sin_port));

        // 新しいプロセスを作成
        pid = fork();
        if (pid < 0) {
            LOG_ERROR("fork failed");
            close(client_fd);
            continue;
        }

        if (pid == 0) { // 子プロセス
            close(server_fd); // 子プロセスではサーバーソケットは不要

            char buffer[BUFFER_SIZE];
            ssize_t bytes_received;

            while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
                buffer[bytes_received] = '\0';
                LOG_INFO("Child process %d received: %s", getpid(), buffer);

                if (send(client_fd, buffer, bytes_received, 0) < 0) {
                    LOG_ERROR("send failed");
                    break;
                }
            }

            if (bytes_received < 0) {
                LOG_ERROR("recv failed");
            }

            close(client_fd);
            LOG_INFO("Child process %d exiting", getpid());
            exit(0);
        } else { // 親プロセス
            close(client_fd); // 親プロセスではクライアントソケットは不要
        }
    }

    close(server_fd);
    return 0;
} 