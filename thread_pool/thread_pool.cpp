#include "thread_pool.hpp"

ThreadPool::ThreadPool(int threadsNumber, int taskQueueSize) :
    threads(threadsNumber), queueSize(taskQueueSize), busyThread(0), shouldQuit(false) {
    for(auto &t : threads){
        t = std::thread([this]() {
            while (true) {
                if (shouldQuit)
                    return;

                queueMutex.lock();
                if (queue.empty()){
                    queueMutex.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    continue;
                }

                const Task task = queue.front();
                queue.pop();
                queueMutex.unlock();
                task();
            }
        });
    }
}

void ThreadPool::pushTask(const Task &task){
    queueMutex.lock();
    if(queue.size() == queueSize){
        queue.pop();
    }
    queue.push(task);
    queueMutex.unlock();
}

void ThreadPool::join() {
    while(!queue.empty() && busyThread == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

ThreadPool::~ThreadPool() {
    shouldQuit = true;
    for(auto& t : threads)
        t.join();
}