#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <cstring>

int main()
{
    int fd = open("newfile.txt",O_APPEND | O_CREAT | O_RDWR,0644);
    if(fd == -1)
    {
        std::cout<<strerror(errno)<<"\n";
        return 1;   
    }
    std::string s = "write this to file\n"; 
    write(fd,s.data(),s.size());
    if(fsync(fd) == -1)
    {
        std::cout<<strerror(errno)<<"\n";
        return 1;   
    }
    close(fd);
    return 0;
}
