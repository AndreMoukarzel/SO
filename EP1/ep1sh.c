/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: ???????
////////////////////////// COMO RODAR ///////////////////////////////
//
// compila com -lreadline -termcap
//
///////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LENGTH 1024

int main(int argc, char **argv){
	/* pega o diretorio atual e gera o prompt */
	char prompt[MAX_LENGTH];
	char cwd[MAX_LENGTH];
	strcpy(prompt, "[");
	strcat(prompt, getcwd(cwd, sizeof(cwd)));
	strcat(prompt, "]$ ");

	while(1){
		/* exibe o prompt e aguardo por input do usuário */
		char *line = readline(prompt); /* essa func faz o malloc p/ line */

		if (strcmp(line, "")) {
			/* adiciona o comando ao hitorico se nao for uma string vazia */
			add_history(line);
		}

		if (!strcmp(line, "date")) {
			int date = system("date");
			printf("%d\n", date);
		}
		else if(!(strcmp(line, "exit"))) {
			/* fecha o programa */
			free(line);
			return EXIT_SUCCESS;
		}
		else
			printf("%s: comando não encontrado\n", line);
		free(line);
	}
}