//Madison Sawvel

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>

using namespace std;

int main(int argc, char *argv[])
{
  int numOfMaps = 0;
  int totalSize = 0;
  int inputs, out;
  struct stat st;
  void *ip, *op;
  out = open(argv[argc - 1], O_RDWR | O_CREAT | O_EXCL, 0422);
  if (out == -1)
  {
    //fprintf(stderr, "File %s cannot be opened:\n", argv[argc-1]);
    perror("open destintation");
    exit(1);
  }
  char *maps[argc - 2];
  int *sizeArray = new int[argc - 2];
  for (int i = 1; i < argc - 1; i++)
  {
    inputs = open(argv[i], O_RDONLY);
    if (inputs == -1)
    {
      perror("open source");
      exit(1);
    }
    if (fstat(inputs, &st))
    {
      perror("stat source");
      exit(1);
    }
    totalSize += (int)st.st_size;
    sizeArray[i - 1] = (int)st.st_size;
    ip = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, inputs, 0);
    madvise(ip, st.st_size, MADV_SEQUENTIAL); //first argument is pointer, second is length in bytes; don't do size of pointer
    maps[i - 1] = (char *)ip;
    if (ip == MAP_FAILED)
    {
      perror("map source");
      exit(1);
    }
    numOfMaps++;
    close(inputs);
  }
  if (ftruncate(out, totalSize))
  {
    perror("truncate destination");
    exit(1);
  }
  op = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, out, 0);
  if (op == MAP_FAILED)
  {
    perror("map destintation");
    exit(1);
  }
  madvise(op, st.st_size, MADV_SEQUENTIAL); //first argument is pointer, second is length in bytes - switch out sizeOf 
  int currentByte = 0;
  for (int i = 0; i < numOfMaps; i++)
  {
    memcpy((char *)op + currentByte, (void *)maps[i], sizeArray[i]);
    currentByte += sizeArray[i];
  }
  close(out);
}

//have now: no errors shown in terminal, out file has the 4 test files in it
