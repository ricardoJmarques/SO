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

void sigHandler(int s){
  switch (s){
    case SIGUSR1:
      printf("Sinal OK Recebido.\n");
    break;
    case SIGUSR2:
      printf("Sinal KO Recebido.\n");
    break;
    case SIGINT:
      unlink("teste/fifos/srv");
    break;
    default:
    break;  
  }
}

int iniciaTx(char* FIFO, char* ficheiro){
  int fdFicheiro, fdFIFO, tamMax;
  char buffer[4096];
  fdFicheiro = open(ficheiro, O_RDONLY);
  fdFIFO = open(FIFO, O_WRONLY);
  printf("TX FIFO: %s\n", FIFO);
  printf("TX File: %s\n", ficheiro);
  while ((tamMax=read(fdFicheiro, buffer, 4096)) > 0){
    write(fdFIFO, buffer, tamMax);
  }
  close(fdFIFO);
  close(fdFicheiro);
  return 0;
}

int iniciaRx(char* FIFO, char* ficheiro){
  int fdFicheiro, fdFIFO, tamMax;
  char buffer[4096];
  printf("RX FIFO: %s\n", FIFO);
  printf("RX File: %s\n", ficheiro);
  mkfifo(FIFO, 0666);
  fdFicheiro = open(ficheiro, O_WRONLY | O_CREAT, 0644); /*stdout*/
  fdFIFO = open(FIFO, O_RDONLY); /*stdin*/
  while ((tamMax=read(fdFIFO, buffer, 4096)) > 0){
    write(fdFicheiro, buffer, tamMax);
  }
  close(fdFicheiro);
  close(fdFIFO);
  unlink(FIFO);
  return 0;
}

void comandoB(char* FIFO, char* ficheiro){
  iniciaRx(FIFO, ficheiro);
}

void comandoR(char* FIFO, char* ficheiro){
  iniciaTx(FIFO, ficheiro);
}

int executaComando(char *cmd, int pid){
  char fifo[64];
  char *path = "teste/fifos/";
  char *ficheiro = "teste/serverfiles/a.txt";
  sprintf(fifo, "%s%d", path, pid);
  if (strcmp(cmd,"B")==0){
    comandoB(fifo, ficheiro);
  }
  else if (strcmp(cmd,"R")==0)
    comandoR(fifo, ficheiro);
  else {
    printf("Comando Desconhecido\n");
  }
  return 0;
}
  
int leComandoPid(char *comando){ /*retorna o pid do processo filho*/
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

int main(){
  int fd, loopT, pid;
  char *servidorFIFOPATH = "teste/fifos/srv";
/*  char servidorFIFO[250]; */
  char buffer[32];
  
  /*    sprintf(buffer, "%s%d", servidorFIFO, getpid()); cria string para abrir buffer /tmp/sobuserv/7850 com PID do servidor*/
  loopT = 1;
  signal(SIGINT,sigHandler);
  
  if ((mkfifo(servidorFIFOPATH, 0666)) == 0) {
     printf("arrancou servidor com id: %d\n", getpid());
  }
  else {
    printf("Erro de FIFO");
    loopT = 0;
  }
  
  while(loopT){
    fd = open(servidorFIFOPATH, O_RDONLY);
    while (read(fd, buffer, 32) > 0) {
      pid = leComandoPid(buffer);
    }
    close(fd);
    kill(pid,SIGUSR1);
    executaComando("B", pid);
    printf("Enviou Sinal %d para o pid %d\n", SIGUSR1, pid);
  }
  unlink(servidorFIFOPATH);
  return 0;
}

