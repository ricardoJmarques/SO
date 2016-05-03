#define _POSIX_SOURCE /*comando Kill - compiler warning: implicit declaration of function ‘kill’ */
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>


int main(){
  int fd, tamMax;
  char *servidorFIFO = "/tmp/sobusrv";
  char buffer[32];
  int flag;
  int pid;
  
  flag = 0;
  mkfifo(servidorFIFO, 0666);
  
  while(1){
    fd = open(servidorFIFO, O_RDONLY);
    while ((tamMax = read(fd, buffer, 32)) > 0) {
      printf("Recebeu a mensagem: %s, com tamanho: %d\n", buffer, tamMax);
      flag++;
    }
    close(fd);
    if (flag != 0){
      pid = atoi(buffer);
      kill(pid,SIGUSR1);
      flag = 0;
      printf("Enviou Sinal\n");
    }
  }
  return 0;
}

