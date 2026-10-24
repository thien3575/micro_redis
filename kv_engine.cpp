#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
class KVEngine {
private:
    // Dữ liệu nội bộ của hệ thống lưu trữ
    std::unordered_map<std::string, std::string> m_storage;
    void loadFromDisk()
    {
        std::fstream file("data.log",std::ios::in);
        if(file) 
        {
            std::string line,command;
            while(std::getline(file,line))
            {
                if(line.empty()) continue;
                std::stringstream ss(line);
                ss >> command;
                if(command == "SET")
                {
                    std::string key, value;
                    ss>>key>>value;
                    key = key.substr(1,key.size()-2);
                    value = value.substr(1,value.size()-2);
                    m_storage[key] = value;
                }
                else
                {
                    std::string key;
                    ss>>key;
                    key = key.substr(1,key.size()-2);
                    m_storage.erase(key);
                }
            }
        }
        file.close();
    }
public:
    // Constructor khởi tạo
    KVEngine() 
    {
        loadFromDisk();
    }

    // Ghi dữ liệu
    void set(const std::string& key, const std::string& value) 
    {
        // TỰ ĐIỀN CODE CỦA BẠN VÀO ĐÂY
        m_storage[key] = value;
    }

    // Đọc dữ liệu
    std::string get(const std::string& key) 
    {
        // TỰ ĐIỀN CODE CỦA BẠN VÀO ĐÂY: Trả về value hoặc "(nil)"
        auto it = m_storage.find(key);
        if(it == m_storage.end()) {
            return "(nil)";
        }
        return it->second;
    }

    // Xóa dữ liệu
    bool del(const std::string& key) 
    {
        // TỰ ĐIỀN CODE CỦA BẠN VÀO ĐÂY: Trả về true nếu xóa được, false nếu không có
        return m_storage.erase(key) > 0;
    };
};
int main() {
    KVEngine db; // Khởi tạo instance của storage engine
    std::string line;
    std::cout << "KV-Storage Engine v0.1 Ready. Type commands:\n";
    std::fstream file("data.log",std::ios::app);
    // Vòng lặp nhận lệnh từ người dùng (CLI REPL)
    while (std::cout << "> " && std::getline(std::cin, line)) 
    {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string command, key, value;
        ss >> command;

        if (command == "EXIT") break;

        if (command == "SET") 
        {
            ss >> key >> value;
            db.set(key, value);
            file<<"SET <"<<key<<"> <"<<value<<">\n";
            std::cout << "OK\n";
        } 
        else if (command == "GET") 
        {
            ss >> key;
            std::cout << db.get(key) << "\n";
        } 
        else if (command == "DEL") 
        {
            ss >> key;
            file<<"DEL <"<<key<<">\n";
            std::cout << (db.del(key) ? "1" : "0") << "\n";
        } 
        else 
        {
            std::cout << "ERR Unknown command\n";
        }
    }
    return 0;
}
