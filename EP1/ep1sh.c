/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: ???????
////////////////////////// COMO RODAR ///////////////////////////////
//
// compila com -lreadline -ltermcap
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


char *getDomain(char *line) {
	char *domain, *pch = strchr(line, ' ');
	int size = pch - line;

	domain = malloc(size * sizeof(char));
	strncpy(domain, line, size);
	strcat(domain, "\0");

	return domain;
}


void createProcess(char *domain, char *comand, char *argument) {
	int pid = fork();

	if (pid != 0) /* processo pai */
		wait(NULL); /* espera processo filho acabar (deveria?) */
	else { /* processo filho */
		execl(domain, domain, "-c 2 ", "www.google.com.br");
		/*execl(domain, process (ultima string do domain), comand, argument); */
		return;
	}
}


void runExecutable(char *name) {
	/* num sei qual exec seria melhor */
}


int main(int argc, char **argv) {
	/* pega o diretorio atual e gera o prompt */
	char prompt[MAX_LENGTH];
	char cwd[MAX_LENGTH];
	strcpy(prompt, "[");
	strcat(prompt, getcwd(cwd, sizeof(cwd)));
	strcat(prompt, "]$ ");

	while(1){
		/* exibe o prompt e aguardo por input do usuário */
		char *line = readline(prompt); /* essa func faz o malloc p/ line */
		char *bar_pos;

		if (strcmp(line, "")) {
			/* adiciona o comando ao hitorico se nao for uma string vazia */
			add_history(line);
		}

		if (!strcmp(line, "date")) {
			/* pode fazer isso? system() usa execl */
			int date = system("date");
			printf("%d\n", date);
		}
		else if(!(strcmp(line, "exit"))) {
			/* fecha o programa */
			free(line);
			return EXIT_SUCCESS;
		}
		else {
			bar_pos = strchr(line, '/');

			if (bar_pos - line == 0) { /* é chamada de processo (?), começa com '/' */
				createProcess(getDomain(line), "", "");
			}
			else 
				printf("%s: comando não encontrado\n", line);
			/* createProcess("", "", ""); */
		}
		free(line);
	}
}