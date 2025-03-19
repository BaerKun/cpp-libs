#include "thread_pool.hpp"


/*
 *  互斥锁mutex
    所有线程中只有一个能持有，持有时间从lock->unlock
    其余线程将被阻塞于lock

 *  lock_guard/unique_lock会随初始化和折构而 lock和unlock

 *  wait会释放锁的控制权，直到条件变量condition_variable将其唤醒notify
    此时线程会尝试与其他线程竞争锁的所有权
    获取锁后会检查回调条件，只有条件返回true(期间线程持有锁)，线程继续持有锁，并向下执行
    否则释放锁，继续等待下一次notify，本次notify结束

 *  atomic用于原子操作，相当于封装了lock和unlock
 */

ThreadPool::ThreadPool(int threadsNumber, int taskQueueSize) :
    threads(threadsNumber), busyThread(0), queueSize(taskQueueSize), shouldQuit(false) {
    for(auto &t : threads){
        t = std::thread([this]() {
            while (true) {
                std::unique_lock lock(queueMutex);
                if (queue.empty()){
                    queueCond.wait(lock, [this](){return !queue.empty() || shouldQuit;});
                }

                if(shouldQuit)
                    return;
                
                const Task task = queue.front();
                queue.pop();
                lock.unlock();

                busyThread++;
                task();
                busyThread--;
                taskCond.notify_all();
            }
        });
    }
}

void ThreadPool::pushTask(const Task &task){
    queueMutex.lock();
    if(queueSize && queue.size() == queueSize){
        queue.pop();
    }
    queue.push(task);
    queueMutex.unlock();
    queueCond.notify_one();
}

void ThreadPool::waitTaskOver() {
    std::unique_lock lock(taskMutex);
    taskCond.wait(lock, [this](){return queue.empty() && busyThread == 0;});
}

ThreadPool::~ThreadPool() {
    shouldQuit = true;
    queueCond.notify_all();
    for(auto& t : threads)
        t.join();
}