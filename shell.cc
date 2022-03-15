//Madison Sawvel

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

void DoCommand(char *cmd);
char **FormatCommand(char *cmd);
void DoChildStuff(char **cmd);
void DoFork(char **args);

//	1 Can run an executable.  I type in some full path, and you exec it.
//	3 You search the path for the executable
//	1 Knows how to change directory
//	2 Tab Completion and Arrow History (see the readline library)
//	1 Control-L clears the screen
//	3 Can do file input redirection "<"
//	3 Can do file output redirection ">"
//	3 Can do file output redirection append mode ">>"


//https://eli.thegreenplace.net/2016/basics-of-using-the-readline-library/
int main(int argc, char **argv)
{
    printf("Welcome! You can exit by pressing Ctrl+C at any time...\n");

    if (argc > 1 && std::string(argv[1]) == "-d")
    {
        rl_bind_key('\t', rl_insert);
    }

    char *buf;
    while (true)
    {
        buf = readline(">> ");
        if (strlen(buf) > 0)
        {
            add_history(buf);
        }
        DoCommand(buf);
        free(buf);
    }

    return 0;
}

void DoCommand(char *cmd)
{ // Taken from http://euclid.nmu.edu/~rappleto/Classes/CS426/Assignments/TheBigAssignment/notes-2020.txt

    char *formattedCommand[5];
    int count = sscanf(cmd, "%ms %ms %ms %ms %ms", &formattedCommand[0], &formattedCommand[1], &formattedCommand[2], &formattedCommand[3], &formattedCommand[4]);
    char *cmds = formattedCommand[0];

    if ((string)cmds == "cd")
    {
        if (chdir(formattedCommand[1]) == -1)
            perror("Error changing directory");
    }
    else
    {
        DoFork(formattedCommand);
    }
}

void DoChildStuff(char **cmd)
{
    int stdout = dup(1);
    int stdin = dup(0);

    // Based off https://stackoverflow.com/questions/19215027/check-if-element-found-in-array-c
    int fileOutput = -1;
    int fileInput = -1;
    int fileOutputAppend = -1;
    for (int i = 0; i < 5; i++)
    {
        if(cmd[i] == nullptr){
            break;
        }
        if ((string)cmd[i] == "<")
        {
            cmd[i] = nullptr;
            fileInput = i;
            break;
        }
        if ((string)cmd[i] == ">")
        {
            cmd[i] = nullptr;
            fileOutput = i;
            break;
        }
        if ((string)cmd[i] == ">>")
        {
            cmd[i] = nullptr;
            fileOutputAppend = i;
            break;
        }
    }

    if (fileInput != -1)
    {
        char *path = cmd[fileInput + 1];
        int fd = open(path, O_RDONLY);
        if (fd == -1)
            perror("File input could not be opened");
        dup2(fd, 0);
    }
    else if (fileOutput != -1)
    {
        char *path = cmd[fileOutput + 1];
        int fd = open(path, O_WRONLY | O_CREAT);
        if (fd == -1)
            perror("File output could not be opened");
        dup2(fd, 1);
    }
    else if (fileOutputAppend != -1)
    {
        char *path = cmd[fileOutputAppend + 1];
        int fd = open(path, O_WRONLY| O_CREAT | O_APPEND);
        if (fd == -1)
            perror("File output append could not be opened");
        dup2(fd, 1);
    }
    if(execvp(cmd[0], cmd) == -1)
        perror("Error execvp");

    close(stdout);
    close(stdin);
}

void DoFork(char **args)
{
    int pid = fork();
    if (pid == -1)
    {
        perror("Unable to fork");
        exit(1);
    }
    else if (pid > 0)
    { //parent
        int w;
        do
        {
            w = waitpid(pid, NULL, WNOHANG);
        } while (w > 0);
    }
    else
    { //child
        DoChildStuff(args);
        exit(1);
    }
}
