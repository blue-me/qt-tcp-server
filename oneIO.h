#ifndef ONEIO_H
#define ONEIO_H
#include <string.h>
#include <unistd.h>

using namespace std;
typedef struct{
    MYSQL mysql;
    SOCKET i_connfd;
}OneIO;

#endif // ONEIO_H
