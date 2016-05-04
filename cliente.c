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

int enviaComando(char* FIFO, char TIPO){
  int pid, fd, i, status;
  char comando[32];

  signal(SIGUSR1,sigHandler);
  pid = getpid();
  i = sprintf(comando, "%d %c", pid, TIPO);

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

int backupCmd(int argc, char* argv[]){
  int i, fd;
  char *sha1, *path;
  if (argc == 3){
    fd = open(argv[2], O_RDONLY);
      if (fd == -1){
        printf("Erro ao abrir Ficheiro\n");
      }
      else{
        sha1 = calculaSha1Sum(argv[2]);
        sha1 = strtok(sha1, " ");
        path = strtok(NULL, "\n");
        printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
        printf("inicia copia do ficheiro %s para o servidor.\n", argv[2]);
        close(fd);
      }
  }
  
  else{
    for (i=2; i<argc; i++){
      fd = open(argv[i], O_RDONLY);
      if (fd == -1){
        printf("Erro ao abrir Ficheiro\n");
      }
      else{
        sha1 = calculaSha1Sum(argv[i]);
        sha1 = strtok(sha1, " ");
        path = strtok(NULL, "\n");
        printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
        printf("inicia copia do ficheiro %s para o servidor.\n", argv[i]);
        close(fd);
      }
    }
  }
  
  return 0;
}

int restoreCmd(int argc, char *argv[]){
  int i, fd;
  char *sha1, *path;
  if (argc == 3){
    fd = open(argv[2], O_RDONLY);
    if (fd == -1){
      printf("Erro ao abrir Ficheiro\n");
    }
    else{
      sha1 = calculaSha1Sum(argv[2]);
      sha1 = strtok(sha1, " ");
      path = strtok(NULL, "\n");
      printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
      printf("inicia reposicao do ficheiro %s do servidor.\n", argv[2]);
      close(fd);
    }
  }
  
  else{
    for (i=2; i<argc; i++){
      fd = open(argv[i], O_RDONLY);
      if (fd == -1){
        printf("Erro ao abrir Ficheiro\n");
      }
      else{
        sha1 = calculaSha1Sum(argv[i]);
        sha1 = strtok(sha1, " ");
        path = strtok(NULL, "\n");
        printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
        printf("inicia reposicao do ficheiro %s do servidor.\n", argv[i]);
        close(fd);
      }
    }
  }
  
  return 0;
}
    
int main(int argc, char *argv[]){
  char *sobusrv = "/tmp/sobusrv"; /*Named Pipe do servidor*/

  if (argc<=2){
      printf("Modo de uso:\n");
      printf(" sobucli backup *.txt\n");
      printf(" sobucli restore a.txt\n");
  }
  else{
    /*comando backup */
  	if(strcmp(argv[1], "backup")==0){
      enviaComando(sobusrv, 'B');
      backupCmd(argc, argv);
  	}
  	
    /*comando restore*/
  	else if (strcmp(argv[1], "restore")==0){
      enviaComando(sobusrv, 'R');
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

