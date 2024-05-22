#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>

struct File {
    std::string name;
    std::string content;
    size_t size;
    bool is_open;
};

std::vector<File> files;
std::mutex file_system_mutex;

// Функция создания нового файла
void create_file(const std::string& name) {
    std::lock_guard<std::mutex> lock(file_system_mutex);

    if (files.size() >= 100) {
        std::cout << "Maximum file limit reached.\n";
        return;
    }
    for (const auto& file : files) {
        if (file.name == name) {
            std::cout << "File with this name already exists.\n";
            return;
        }
    }
    files.push_back({ name, "", 0, false });
    std::cout << "File created successfully.\n";
}


//Функция для чтения содержимого файла
void read_file(const std::string& name) {
    std::lock_guard<std::mutex> lock(file_system_mutex);

    for (const auto& file : files) {
        if (file.name == name) {
            std::cout << "File content:\n" << file.content << "\n";
            return;
        }
    }
    std::cout << "File not found.\n";
}


// Функция для измнения содержимого файла
void write_file(const std::string& name, const std::string& new_content) {
    std::lock_guard<std::mutex> lock(file_system_mutex);

    for (auto& file : files) {
        if (file.name == name) {
            file.content = new_content;
            file.size = new_content.size();
            std::cout << "File content updated successfully.\n";
            return;
        }
    }
    std::cout << "File not found.\n";
}



//Функция для удаления файла
void delete_file(const std::string& name) {
    std::lock_guard<std::mutex> lock(file_system_mutex);

    auto it = std::remove_if(files.begin(), files.end(), [&name](const File& file) {
        return file.name == name;
        });
    if (it != files.end()) {
        files.erase(it, files.end());
        std::cout << "File deleted successfully.\n";
    }
    else {
        std::cout << "File not found.\n";
    }
}

//Функция для поиска файлов по имени или содержимому
void search_files(const std::string& query) {
    std::lock_guard<std::mutex> lock(file_system_mutex);

    bool found = false;
    for (const auto& file : files) {
        if (file.name.find(query) != std::string::npos || file.content.find(query) != std::string::npos) {
            std::cout << "File found: " << file.name << "\n";
            found = true;
        }
    }
    if (!found) {
        std::cout << "No files matching the query were found.\n";
    }
}

//Функция для отображения информации о файловой системе
void display_file_system_info() {
    std::lock_guard<std::mutex> lock(file_system_mutex);

    std::cout << "File System Info:\n";
    std::cout << "Number of files: " << files.size() << "\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "File " << i + 1 << ": " << files[i].name << ", Size: " << files[i].size << " bytes\n";
    }
}

int main() {
    create_file("test.txt");
    write_file("test.txt", "Hello, world!");
    read_file("test.txt");
    search_files("world");
    display_file_system_info();
    delete_file("test.txt");
    display_file_system_info();

    return 0;
}
