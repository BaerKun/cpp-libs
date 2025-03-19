#include <functional>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool{
    using Task = std::function<void()>;

public:
    ThreadPool(int threadsNumber, int taskQueueSize=0);

    void pushTask(const Task &task);

    void waitTaskOver();

    ~ThreadPool();
    
private:
    std::vector<std::thread> threads;
    std::queue<Task> queue;
    std::mutex queueMutex;
    std::mutex taskMutex;
    std::condition_variable queueCond;
    std::condition_variable taskCond;
    std::atomic<int> busyThread;
    int queueSize;
    bool shouldQuit;
};