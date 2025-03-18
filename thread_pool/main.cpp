#include <iostream>
#include <thread>
#include <vector>
#include <queue>

using namespace std;

class ThreadPool{
    using Task = function<void()>;
public:
    ThreadPool(int threadsNumber, int taskQueueSize) {
        threads = vector<thread>(threadsNumber);
        queueSize = taskQueueSize;

        for(thread &t : threads){
            t = thread([this]() {
                while (true) {
                    if (over)
                        return;

                    queueMutex.lock();
                    if (queue.empty()){
                        queueMutex.unlock();
                        this_thread::sleep_for(chrono::milliseconds(5));
                        continue;
                    }

                    const Task &task = queue.front();
                    queue.pop();
                    queueMutex.unlock();
                    task();
                }
            });
        }
    }

    void pushTask(const Task &task){
        queueMutex.lock();
        if(queue.size() == queueSize){
            queue.pop();
        }
        queue.push(task);
        queueMutex.unlock();
    }

    void join() {
        while(!queue.empty())
            this_thread::sleep_for(chrono::milliseconds(10));
    }

    ~ThreadPool() {
        over = true;
        for(auto& t : threads)
            t.join();
    }
private:
    vector<thread> threads;
    queue<Task> queue;
    mutex queueMutex;
    bool over = false;
    int queueSize;
};

int main() {
    ThreadPool pool(4, 10);
    unsigned result[100] = {0};
    for (int i = 0; i < 100; ++i){
        pool.pushTask([i, output = result + i](){
            unsigned sum = 0;
            for(unsigned j = i * 100; j < i * 1000; ++j){
                sum += j;
            }
            *output = sum;
        });
        this_thread::sleep_for(chrono::milliseconds(5));
    }
    pool.join();
    for (unsigned i : result){
        cout << i << endl;
    }
    return 0;
}
