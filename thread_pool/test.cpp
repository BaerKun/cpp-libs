#include "thread_pool.hpp"
#include <iostream>

int main() {
    ThreadPool pool(4, 20);
    unsigned result[200] = {0};
    for (int i = 0; i < 200; ++i){
        pool.pushTask([i, output = result + i](){
            unsigned sum = 0;
            for(unsigned j = i; j < i * 400; ++j){
                sum += j;
            }
            *output = sum;
        });
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    pool.waitTaskOver();

    for (int i = 0; i < 200; ++i){
        std::cout << i << ": " << result[i] << std::endl;
    }
    return 0;
}
