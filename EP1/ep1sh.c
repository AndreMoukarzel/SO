/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: ???????
////////////////////////// COMO RODAR ///////////////////////////////
//
// compila com -lreadline -termcap
//
/* ////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>


int main(int argc, char **argv){
	// pega o diretorio atual e gera o prompt
	char prompt[1024];
	char cwd[1024];
	strcpy(prompt, "[");
	strcat(prompt, getcwd(cwd, sizeof(cwd)));
	strcat(prompt, "]$ ");

	while(1){
		char *line = readline(prompt); // essa func faz o malloc p/ line
		if (strcmp(line, "")){
			// adiciona o comando ao hitorico se nao for uma string vazia
			add_history(line);
		}
		if (!strcmp(line, "date")){
			int date = system("date");
			printf("%d\n", date);
		}
		else if(!(strcmp(line, "exit"))){
			// rage quit
			free(line);
			return EXIT_SUCCESS;
		}
		else
			printf("%s: COMMAND NOT FOUND\n", line);
		free(line);
	}
}