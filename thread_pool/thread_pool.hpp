#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <queue>

class ThreadPool{
    using Task = std::function<void()>;

public:
    ThreadPool(int threadsNumber, int taskQueueSize);

    void pushTask(const Task &task);

    void join();

    ~ThreadPool();
    
private:
    std::vector<std::thread> threads;
    std::queue<Task> queue;
    std::mutex queueMutex;
    int queueSize;
    int busyThread;
    bool shouldQuit;
};