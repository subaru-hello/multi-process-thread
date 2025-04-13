#include "../../include/common.h"

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

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

        // クライアントからのデータを受信
        ssize_t bytes_received;
        while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            // パケットの受信ログ
            LOG_PACKET("RECV", buffer, bytes_received);
            LOG_INFO("Received: %s", buffer);

            // パケットの送信ログ
            LOG_PACKET("SEND", buffer, bytes_received);

            // クライアントに応答を送信
            if (send(client_fd, buffer, bytes_received, 0) < 0) {
                LOG_ERROR("send failed");
                break;
            }
        }

        if (bytes_received < 0) {
            LOG_ERROR("recv failed");
        }

        close(client_fd);
        LOG_INFO("Connection closed");
    }

    close(server_fd);
    return 0;
} 