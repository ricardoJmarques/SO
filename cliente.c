#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>

#define tamCMD 32

/*
char *CriaComando(int cmd){
  switch cmd {

    case 0: 
    break;
    case 1: 
    break;
    default:
    break;
}
*/

void ok(int s){
  printf("Sinal OK Recebido.\n");
}

void ko(int s){
  printf("Sinal KO Recebido.\n");
}

int enviaDados(char* FIFO, char* CMD, int tam){
  int pid;
  int fd;
  int i;
  char comando[32];

  signal(SIGUSR1,ok);
  signal(SIGUSR2,ko);
  fd = open(FIFO, O_WRONLY);
  pid = getpid();
  i = sprintf(comando, "%d", pid);

  if(fd > 0){
    printf("pid - %s, tamanho %d\n", comando, i);
    write(fd, comando, i); /*comando[tamCMD]*/
    close(fd);
    pause();
    return 0;
  }
  else{
    printf("erro open FIFO");
    return -2; /*erro open FIFO*/
  }
}

int main(int argc, char *argv[]){
  char *sobusrv = "/tmp/sobusrv"; /*Named Pipe*/

  /*char *comando = "1 COMANDO-BACKUP-DATAABBCCDDEEF\0";*/
  if (argc < 2)
    return -1; /*erro NO args*/
  else {
    enviaDados(sobusrv, "A", 32);
  }
  return 0;
}


