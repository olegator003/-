#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

const std::string FILENAME = "chat.txt";
std::mutex mtx;
std::condition_variable cv;
bool newMessage = false;

void writer() {
    std::ofstream file(FILENAME, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return newMessage; });

        std::string message;
        std::cout << "Enter your message: ";
        std::getline(std::cin, message);

        file << message << std::endl;
        newMessage = false;
        lock.unlock();
        cv.notify_all();
    }
}

void reader() {
    std::ifstream file(FILENAME, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_all();
        cv.wait(lock, [] { return !newMessage; });

        file.clear();
        file.seekg(0, std::ios::beg);
        std::string line;
        while (std::getline(file, line)) {
            std::cout << "Received message: " << line << std::endl;
        }
        newMessage = true;
    }
}

int main() {
    std::thread writerThread(writer);
    std::thread readerThread(reader);

    writerThread.join();
    readerThread.join();

    return 0;
}
