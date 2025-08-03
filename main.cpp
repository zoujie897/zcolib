#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>


// 生产者消费者模型示例
std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;
const int max_size = 5;
bool done = false;

void producer() {
    for (int i = 0; i < 10; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return q.size() < max_size; });
        q.push(i);
        std::cout << "生产者生产: " << i << std::endl;
        lock.unlock();
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::unique_lock<std::mutex> lock(mtx);
    done = true;
    lock.unlock();
    cv.notify_all();
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !q.empty() || done; });
        if (!q.empty()) {
            int value = q.front();
            q.pop();
            std::cout << "消费者消费: " << value << std::endl;
            lock.unlock();
            cv.notify_all();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        } else if (done) {
            break;
        }
    }
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    std::cout << "生产和消费过程结束。" << std::endl;
    return 0;
}

