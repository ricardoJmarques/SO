#define _POSIX_SOURCE /*comando Kill - compiler warning: implicit declaration of function ‘kill’ */
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <signal.h>

#include <sys/wait.h>

int leComando(char *comando){ /*retorna o pid do processo filho*/
  int pid;
  char *cmd;
  char *str;
  str = (char*)malloc(sizeof(char) * (strlen(comando)+1));
  cmd = (char*)malloc(sizeof(char) * (strlen(comando)+1));
  str = strtok(comando, "\r\n");
  pid = atoi(strtok(str, " "));
  cmd = strtok(NULL, "\0");  
  printf("Recebeu do pid %d o comando %s\n", pid, cmd);
  return pid;
}

int iniciaTx(char* FIFO, char* ficheiro){
  int fdFicheiro, fdFIFO;
  char *buffer[4096];
  fdFIFO = open(FIFO, O_WRONLY);
  fdFicheiro = open(FIFO, O_RDONLY);
  while (read(fdFicheiro, buffer, 4096) > 0){
    write(fdFIFO, buffer, 4096);
  }
  close(fdFIFO);
  close(fdFicheiro);
  return 0;
}

int iniciaRx(char* FIFO, char* ficheiro){
  int fdFicheiro, fdFIFO;
  char *buffer[4096];
  fdFIFO = open(FIFO, O_RDONLY);
  fdFicheiro = open(FIFO, O_WRONLY);
  while (read(fdFIFO, buffer, 4096) > 0){
    write(fdFicheiro, buffer, 4096);
  }
  close(fdFIFO);
  close(fdFicheiro);
  return 0;
}

int main(){
  int fd, loopT, pid;
/*  int pid; */
  char *servidorFIFOPATH = "/tmp/sobusrv";
/*  char servidorFIFO[250]; */
  char buffer[32];
  
  /*    sprintf(buffer, "%s%d", servidorFIFO, getpid()); cria string para abrir buffer /tmp/sobuserv/7850 com PID do servidor*/
  loopT = 1;
  mkfifo(servidorFIFOPATH, 0666);
  
   if (mkfifo(servidorFIFOPATH, 0666)) {
     printf("arrancou servidor com id: %d\n", getpid());
   }
  else {
    printf("Erro de FIFO");
    loopT = 0;
  }
  
  while(loopT){
    fd = open(servidorFIFOPATH, O_RDONLY);
    while (read(fd, buffer, 32) > 0) {
      pid = leComando(buffer);
    }
    close(fd);
    kill(pid,SIGUSR1);
    printf("Enviou Sinal %d para o pid %d\n", SIGUSR1, pid);
  }
  unlink(servidorFIFOPATH);
  return 0;
}

