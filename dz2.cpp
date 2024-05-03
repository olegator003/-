#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

std::mutex mtx;
std::atomic<int> counter_atomic(0);
int counter;
int counter_no_sync = 0;


// выполнил студент 31 группы Челюканов ОС

void incrementCounterWithMutex() {
    for (int i = 0; i < 1000000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        counter++;
    }
}

void incrementCounterAtomic() {
    for (int i = 0; i < 1000000; ++i) {
        counter_atomic++;
    }
}

void incrementCounterNoSync() {
    for (int i = 0; i < 1000000; ++i) {
        counter_no_sync++;
    }
}

int main() {

    setlocale(LC_ALL, "RU");

    std::thread t1(incrementCounterWithMutex);
    std::thread t2(incrementCounterWithMutex);
    t1.join();
    t2.join();
    std::cout << "С Мютексом: " << counter << std::endl;

    std::thread t3(incrementCounterAtomic);
    std::thread t4(incrementCounterAtomic);
    t3.join();
    t4.join();
    std::cout << "С Атомиком: " << counter_atomic << std::endl;

    std::thread t5(incrementCounterNoSync);
    std::thread t6(incrementCounterNoSync);
    t5.join();
    t6.join();
    std::cout << "Обычная многопоточность: " << counter_no_sync << std::endl;

    return 0;
}
