//Madison Sawvel

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

int dirWork(char *path);
void dirIdent(dirent *dirNode, char *path);
void lnkIdent(dirent *dirNode, char *path);
int fileIdent(dirent *dirNode, char *path);
char *getFullPath(char *PathTo, dirent *dirNode);

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);
    int totalBytes = dirWork(argv[1]);
    cout << "Total file size: " << totalBytes << endl; 
    return 0;
}

int dirWork(char *path)
{
    int totalBytes = 0;
    auto *dp = opendir(path);
    cout << path << endl;
    assert(dp != nullptr);
    dirent *dirNode;
    while ((dirNode = readdir(dp)) != nullptr)
    {
        cout << path << '/' << dirNode->d_name;
        switch (dirNode->d_type)
        {
        case DT_DIR:
            dirIdent(dirNode, path);
            break;
        case DT_LNK:
            lnkIdent(dirNode, path);
            break;
        case DT_REG:
            totalBytes += fileIdent(dirNode, path);
            break;
        default:
            exit(-1);
        }
    }
    return totalBytes;
}

void dirIdent(dirent *dirNode, char *path)
{
    cout << " (d)" << endl;
    if (!((string)(dirNode->d_name) == "." || (string)(dirNode->d_name) == ".."))
    {
        dirWork(getFullPath(path, dirNode));
    }
}

void lnkIdent(dirent *dirNode, char *path)
{
    char buff[PATH_MAX];
    char *fullPath = getFullPath(path, dirNode);
    ssize_t len = readlink(fullPath, buff, sizeof(buff) - 1);
    if (len != -1)
    {
        buff[len] = '\0';
        cout << "(l, -> " << buff << ")" << endl;
    }
}

int fileIdent(dirent *dirNode, char *path)
{
    //from stack overflow:( https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c)
    struct stat fileStat;
    int rc = stat(getFullPath(path, dirNode), &fileStat);
    cout << " (f, " << fileStat.st_size << ") -> "; //prints size 
    //stackoverflow
    printf("File Permissions: \t");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    cout << endl;
    return fileStat.st_size;
}

char *getFullPath(char *PathTo, dirent *dirNode)
{
    char *nDir = new char[strlen(PathTo) + 3];
    strcpy(nDir, PathTo);
    strcat(nDir, "/");
    char *buffer = new char[strlen(nDir) + strlen(dirNode->d_name) + 1];
    strcpy(buffer, nDir);
    strcat(buffer, dirNode->d_name);
    delete nDir;
    return buffer;
}

//have now: Total file size(totalBytes) is returning 0 when running against one test, stills shows size when printing i.e test4 (f,4399)
//total file size(totalBytes) returns larger number when running all tests at once

/*
Run:
  ./a.out /home/rappleto/pub/Classes/CS426/Assignments/dircount-assignment - all together
  ./a.out /home/rappleto/pub/Classes/CS426/Assignments/dircount-assignment/test1 - individually
*/
