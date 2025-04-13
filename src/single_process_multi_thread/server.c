#include "../../include/common.h"

// クライアントハンドラーのスレッド関数
void *client_handler(void *arg) {
    client_info_t *client = (client_info_t *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    LOG_INFO("Thread started for client %s:%d", 
             inet_ntoa(client->address.sin_addr), 
             ntohs(client->address.sin_port));

    while ((bytes_received = recv(client->socket_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        LOG_INFO("Received from %s:%d: %s", 
                 inet_ntoa(client->address.sin_addr), 
                 ntohs(client->address.sin_port), 
                 buffer);

        if (send(client->socket_fd, buffer, bytes_received, 0) < 0) {
            LOG_ERROR("send failed");
            break;
        }
    }

    if (bytes_received < 0) {
        LOG_ERROR("recv failed");
    }

    close(client->socket_fd);
    free(client);
    LOG_INFO("Client connection closed");
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;
    pthread_t thread_id;
    client_info_t *client;

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
        // クライアント情報のメモリ確保
        client = (client_info_t *)malloc(sizeof(client_info_t));
        if (!client) {
            LOG_ERROR("malloc failed");
            continue;
        }

        client->addr_len = sizeof(client->address);

        // クライアントからの接続を受け付ける
        client->socket_fd = accept(server_fd, (struct sockaddr *)&client->address, &client->addr_len);
        if (client->socket_fd < 0) {
            LOG_ERROR("accept failed");
            free(client);
            continue;
        }

        LOG_INFO("New connection from %s:%d", 
                 inet_ntoa(client->address.sin_addr), 
                 ntohs(client->address.sin_port));

        // 新しいスレッドを作成してクライアントを処理
        if (pthread_create(&thread_id, NULL, client_handler, (void *)client) != 0) {
            LOG_ERROR("pthread_create failed");
            close(client->socket_fd);
            free(client);
            continue;
        }

        // スレッドをデタッチ（終了時に自動的にリソースを解放）
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
} 