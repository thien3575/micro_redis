#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <cstdint>

int main()
{
    int fd = open("newfile.txt", O_APPEND | O_CREAT | O_RDWR, 0644);
    std::cout<<lseek(fd,0,SEEK_END);
}
