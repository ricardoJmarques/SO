#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/wait.h>

#define tamCMD 32

void sigHandler(int s){
  switch (s){
    case SIGUSR1:
      printf("Sinal OK Recebido.\n");
    break;
    case SIGUSR2:
      printf("Sinal KO Recebido.\n");
    break;
    default:
    break;  
  }
}

int enviaComando(char* FIFO, char TIPO, char* ficheiro){
  int pid, fd, i, status;
  char comando[32];

  signal(SIGUSR1,sigHandler);
  signal(SIGUSR2,sigHandler);
  pid = getpid();
  i = sprintf(comando, "%d %c %s", pid, TIPO, ficheiro);

  if(fork()==0){
    fd = open(FIFO, O_WRONLY);
    if(fd > 0){
      write(fd, comando, i); /*comando[tamCMD]*/
      close(fd);
      _exit(0);
      printf("Comando \"%s\" com tamanho %d foi enviado com sucesso!\n", comando, i);
    }
    else{
      printf("erro open FIFO.\n");
    }
  }
  else {
    pause();
    wait(&status);
    printf("Comando \"%s\" com tamanho %d foi enviado com sucesso!\n", comando, i);
  }
  return 0;
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

char *calculaSha1Sum(char* nomeFicheiro){
	int fd[2], nBytesLidos;
	char buff[128];
	char *str;
  pipe(fd);
	if(fork()==0){
	  dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
    execlp("sha1sum", "sha1sum", nomeFicheiro, NULL);
  }
  else{
    close(fd[1]);
    nBytesLidos = read(fd[0], buff, 128);
    str = malloc(sizeof(char) * (nBytesLidos + 1));
    if(str){
      strcpy(str, buff);
    }
    close(fd[0]);
  }
	return str;
}

int backupCmd(char* fifo, int argc, char* argv[]){
  int i, fd;
  char *sha1, *path;
  char str1[100];
  int pid;
  pid=getpid();
  for (i=2; i<argc; i++){
    fd = open(argv[i], O_RDONLY);
    if (fd == -1){
      printf("Erro ao abrir Ficheiro\n");
    }
    else{
      close(fd);
      sha1 = calculaSha1Sum(argv[i]);
      sha1 = strtok(sha1, " ");
      path = strtok(NULL, "\n");
      printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
      printf("inicia copia do ficheiro %s para o servidor.\n", argv[i]);
      sprintf(str1, "teste/fifos/%d", pid);
      enviaComando(fifo, 'B', argv[i]);
      iniciaTx(str1, argv[i]);
    }
  }
  return 0;
}

int restoreCmd(int argc, char *argv[]){
  int i, fd;
  char *sha1, *path;
  char str1[100];
  int pid;
  pid=getpid();
  for (i=2; i<argc; i++){
    fd = open(argv[i], O_RDONLY);
    if (fd == -1){
      printf("Erro ao abrir Ficheiro\n");
    }
    else{
      close(fd);
      sha1 = calculaSha1Sum(argv[i]);
      sha1 = strtok(sha1, " ");
      path = strtok(NULL, "\n");
      printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
      printf("inicia reposicao do ficheiro %s do servidor.\n", argv[i]);
      sprintf(str1, "teste/fifos/%d", pid);
      iniciaRx(str1, path);
    }
  }
  return 0;
}
    
int main(int argc, char *argv[]){
  char *sobusrv = "teste/fifos/srv"; /*Named Pipe do servidor*/

  if (argc<=2){
      printf("Modo de uso:\n");
      printf(" sobucli backup *.txt\n");
      printf(" sobucli restore a.txt\n");
  }
  else{
    /*comando backup */
  	if(strcmp(argv[1], "backup")==0){
      backupCmd(sobusrv, argc, argv);
  	}
  	
    /*comando restore*/
  	else if (strcmp(argv[1], "restore")==0){
  	  restoreCmd(argc, argv);
    }
    
    /* outros comandos */
/*    
    else if (strcmp(argv[1], "outros comandos")==0){
    }
*/
  }
  return 0;
}

