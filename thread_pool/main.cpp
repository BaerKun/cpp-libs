#include "thread_pool.hpp"
#include <iostream>

int main() {
    ThreadPool pool(4, 10);
    unsigned result[100] = {0};
    for (int i = 0; i < 100; ++i){
        pool.pushTask([i, output = result + i](){
            unsigned sum = 0;
            for(unsigned j = i * 100; j < i * 500; ++j){
                sum += j;
            }
            *output = sum;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    pool.join();

    for (int i = 0; i < 100; ++i){
        std::cout << i << ": " << result[i] << std::endl;
    }
    return 0;
}
