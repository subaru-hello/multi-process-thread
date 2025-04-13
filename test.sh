#!/bin/bash

# エラーが発生したら即座に終了
set -e

# ビルド
make clean
make

# 各パターンのテスト関数
test_single_process_single_thread() {
    echo "Testing Single Process Single Thread..."
    ./bin/single_process_single_thread/server &
    SERVER_PID=$!
    sleep 1
    ./bin/single_process_single_thread/client 127.0.0.1
    kill $SERVER_PID
    echo "----------------------------------------"
}

test_single_process_multi_thread() {
    echo "Testing Single Process Multi Thread..."
    ./bin/single_process_multi_thread/server &
    SERVER_PID=$!
    sleep 1
    ./bin/single_process_multi_thread/client 127.0.0.1
    kill $SERVER_PID
    echo "----------------------------------------"
}

test_multi_process_single_thread() {
    echo "Testing Multi Process Single Thread..."
    ./bin/multi_process_single_thread/server &
    SERVER_PID=$!
    sleep 1
    ./bin/multi_process_single_thread/client 127.0.0.1
    kill $SERVER_PID
    echo "----------------------------------------"
}

test_multi_process_multi_thread() {
    echo "Testing Multi Process Multi Thread..."
    ./bin/multi_process_multi_thread/server &
    SERVER_PID=$!
    sleep 1
    ./bin/multi_process_multi_thread/client 127.0.0.1
    kill $SERVER_PID
    echo "----------------------------------------"
}

# メインのテスト実行
echo "Starting tests..."
echo "----------------------------------------"

test_single_process_single_thread
test_single_process_multi_thread
test_multi_process_single_thread
test_multi_process_multi_thread

echo "All tests completed!" 