#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <cstring>
#include <errno.h>
#include <sstream>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

enum class ReadStatus {OK,CLEAN_EOF,TORN_OR_ERR};

bool append_set_log(int mode,int fd, const std::string& key, const std::string& val);
ReadStatus read_exact(int fd, void* buf, size_t count);

class KVEngine
{
    private:
        std::unordered_map<std::string,std::string> m_storage;
        int m_fd = -1;
        bool load_from_disk()
        {
            return true;
        }
    public:
        KVEngine()
        {
            m_fd = open("newfile.txt", O_APPEND | O_CREAT | O_RDWR, 0644);
            if (m_fd == -1)
            {
                throw std::runtime_error(strerror(errno));
            }
        }
        void set(std::string key, std::string value)
        {
            
            if(append_set_log(1, m_fd, key, value))
            {
                m_storage[key] = value;
                std::cout<<"OK\n";
            }
            else
            {
                std::cout<<"Failed\n";
            }
        }
        void del(std::string key)
        {
            if(append_set_log(0,m_fd,key,"") && m_storage.erase(key) == 1)
            {
                std::cout<<"OK\n";
            }
            else
            {
                std::cout<<"FAILED\n";
                return;
            }
        }
        void get(std::string key)
        {
            auto it = m_storage.find(key);
            if(it!= m_storage.end())
            {
                std::cout<<it->second<<'\n';
            }
            else
            {
                std::cout<<"No value\n";
            }
        }
        ~KVEngine()
        {
            close(m_fd);
        }
};
int main()
{

    try
    {
        KVEngine db;
        std::string line;
        while(std::cout<<">" && std::getline(std::cin, line))
        {
            if(line.empty())
            {
                continue;
            }
            std::stringstream ss(line);
            std::string commmand,key,value,word;
            ss >> word;
            if(word == "SET")
            {
                ss>>key>>value;
                db.set(key,value);
            }
            else if(word == "GET")
            {
                ss>>key;
                db.get(key);
            }
            else if(word == "DEL")
            {
                ss>>key;
                db.del(key);
            }
            else if(word == "EXIT")
            {
                break;
            }
            else
            {
                std::cout<<"Invalid command\n";
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}


bool append_set_log(int mode, int fd, const std::string& key, const std::string& val)
{
    std::string frame;
    std::uint8_t cmd_type = mode;
    std::uint32_t klen = static_cast<std::uint32_t>(key.size());
    if(mode == 1)
    {
        std::uint32_t vlen = static_cast<std::uint32_t>(val.size());
        frame.reserve(1 + 4 + klen + 4 + vlen);
        frame.append(reinterpret_cast<const char*>(&cmd_type), sizeof(cmd_type));
        frame.append(reinterpret_cast<const char*>(&klen),sizeof(klen));
        frame.append(key.data(),klen);
        frame.append(reinterpret_cast<const char*>(&vlen),sizeof(vlen));
        frame.append(val.data(),vlen);      
    }
    else
    {
        frame.reserve(1 + 4 + klen);
        frame.append(reinterpret_cast<const char*>(&cmd_type), sizeof(cmd_type));
        frame.append(reinterpret_cast<const char*>(&klen),sizeof(klen));
        frame.append(key.data(),klen);
    }
    

    // keep writing until no bytes is left
    off_t safe_offset = lseek(fd,0,SEEK_END);
    const char* p = frame.data();
    size_t n = frame.size();
    while(n > 0)
    {
        ssize_t w = write(fd,p,n);
        if(w < 0)
        {
            // bi signal cat, thu lai
            if(errno == EINTR) continue;
            if(safe_offset != -1) ftruncate(fd,safe_offset);
            return false;
        }
        n -= static_cast<size_t>(w);
        p += static_cast<size_t>(w);
    }
    return fdatasync(fd) == 0;
}

ReadStatus read_exact(int fd, void* buf, size_t count)
{
    char* ptr = static_cast<char*>(buf);
    size_t total_read = 0;
    while(total_read < count)
    {
        ssize_t n = read(fd,ptr + total_read, count);
        if(n < 0)
        { 
            if (errno == EINTR)
            {
                continue;
            }
            return ReadStatus::TORN_OR_ERR;
        }
        if(n == 0)
        {
            if(total_read == 0) return ReadStatus::CLEAN_EOF;
            return ReadStatus::TORN_OR_ERR;
        }
        total_read+=static_cast<size_t>(n);
    }
    return ReadStatus::OK;
}
