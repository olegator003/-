#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>

const std::string FILENAME = "chat.txt";
std::mutex fileMutex;

void writer() {
    std::ofstream file(FILENAME, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    while (true) {
        std::string message;
        std::cout << "Enter your message: ";
        std::getline(std::cin, message);

        {
            std::lock_guard<std::mutex> lock(fileMutex);
            file << message << std::endl;
            file.flush();
        }
    }
}

void reader() {
    std::ifstream file(FILENAME, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    std::string lastLine;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(fileMutex);
            std::string line;
            while (std::getline(file, line)) {
                if (line != lastLine) {
                    lastLine = line;
                    std::cout << "Received message: " << line << std::endl;
                }
            }
        }

        file.clear();
        file.seekg(0, std::ios::end);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::thread writerThread(writer);
    std::thread readerThread(reader);

    writerThread.join();
    readerThread.join();

    return 0;
}
