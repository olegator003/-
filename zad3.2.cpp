#include <iostream>
#include <fcntl.h>
#include <cstring>

const int MAX_MESSAGE_SIZE = 100;
const int MAX_MESSAGES = 10;
const char* SHARED_MEMORY_NAME = "/chat_memory";
const char* MUTEX_NAME = "/chat_mutex";

struct ChatMessage {
    char text[MAX_MESSAGE_SIZE];
};

struct Mutex {
    int flag;
};

int main() {
    int shm_id = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_id, MAX_MESSAGES * sizeof(ChatMessage));
    ChatMessage* messages = (ChatMessage*)mmap(NULL, MAX_MESSAGES * sizeof(ChatMessage), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

    int mutex_id = shm_open(MUTEX_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(mutex_id, sizeof(Mutex));
    Mutex* mutex = (Mutex*)mmap(NULL, sizeof(Mutex), PROT_READ | PROT_WRITE, MAP_SHARED, mutex_id, 0);

    while (true) {
        // Writer
        {
            while (__sync_lock_test_and_set(&mutex->flag, 1) == 1);

            std::string message;
            std::cout << "Enter your message: ";
            std::getline(std::cin, message);

            for (int i = 0; i < MAX_MESSAGES - 1; ++i) {
                std::strcpy(messages[i].text, messages[i + 1].text);
            }

            std::strcpy(messages[MAX_MESSAGES - 1].text, message.c_str());

            __sync_lock_release(&mutex->flag);
        }

        // Reader
        {
            while (__sync_lock_test_and_set(&mutex->flag, 1) == 1);

            for (int i = 0; i < MAX_MESSAGES; ++i) {
                std::cout << "Received message: " << messages[i].text << std::endl;
            }

            __sync_lock_release(&mutex->flag);
        }
    }

    munmap(messages, MAX_MESSAGES * sizeof(ChatMessage));
    close(shm_id);
    shm_unlink(SHARED_MEMORY_NAME);

    munmap(mutex, sizeof(Mutex));
    close(mutex_id);
    shm_unlink(MUTEX_NAME);

    return 0;
}
