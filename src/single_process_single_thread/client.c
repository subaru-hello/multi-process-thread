#include "../../include/common.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // ソケットの作成
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        ERROR_EXIT("socket creation failed");
    }

    // サーバーアドレスの設定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        ERROR_EXIT("Invalid address");
    }

    // サーバーに接続
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ERROR_EXIT("connect failed");
    }

    LOG_INFO("Connected to server %s:%d", argv[1], PORT);

    // ユーザーからの入力を受け取り、サーバーに送信
    while (1) {
        printf("Enter message (or 'quit' to exit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // 改行文字を削除

        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        // パケットの送信ログ
        LOG_PACKET("SEND", buffer, strlen(buffer));

        // サーバーにメッセージを送信
        if (send(client_fd, buffer, strlen(buffer), 0) < 0) {
            LOG_ERROR("send failed");
            break;
        }

        // サーバーからの応答を受信
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            LOG_ERROR("recv failed");
            break;
        }

        buffer[bytes_received] = '\0';
        // パケットの受信ログ
        LOG_PACKET("RECV", buffer, bytes_received);
        printf("Server response: %s\n", buffer);
    }

    close(client_fd);
    return 0;
} 