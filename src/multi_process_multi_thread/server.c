#include "../../include/common.h"

// クライアントハンドラーのスレッド関数
void *client_handler(void *arg) {
    client_info_t *client = (client_info_t *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    LOG_INFO("Thread %ld in process %d handling client %s:%d", 
             pthread_self(), getpid(),
             inet_ntoa(client->address.sin_addr), 
             ntohs(client->address.sin_port));

    while ((bytes_received = recv(client->socket_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        LOG_INFO("Thread %ld in process %d received: %s", 
                 pthread_self(), getpid(), buffer);

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
    LOG_INFO("Thread %ld in process %d exiting", pthread_self(), getpid());
    return NULL;
}

// シグナルハンドラー
void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;
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

    // ワーカープロセスの作成
    for (int i = 0; i < 4; i++) { // 4つのワーカープロセスを作成
        pid = fork();
        if (pid < 0) {
            LOG_ERROR("fork failed");
            continue;
        }

        if (pid == 0) { // ワーカープロセス
            LOG_INFO("Worker process %d started", getpid());

            while (1) {
                client_info_t *client = (client_info_t *)malloc(sizeof(client_info_t));
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

                LOG_INFO("Worker process %d accepted connection from %s:%d", 
                         getpid(),
                         inet_ntoa(client->address.sin_addr), 
                         ntohs(client->address.sin_port));

                // 新しいスレッドを作成してクライアントを処理
                pthread_t thread_id;
                if (pthread_create(&thread_id, NULL, client_handler, (void *)client) != 0) {
                    LOG_ERROR("pthread_create failed");
                    close(client->socket_fd);
                    free(client);
                    continue;
                }

                // スレッドをデタッチ
                pthread_detach(thread_id);
            }
        }
    }

    // 親プロセスは終了を待つ
    while (1) {
        sleep(1);
    }

    close(server_fd);
    return 0;
} 