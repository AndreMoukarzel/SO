/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
////////////////////////// COMO RODAR ///////////////////////////////
//
// compila com -lreadline -ltermcap
//
///////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <time.h>
#include "stringReader.h"

#define MAX_LENGTH 1024


/* Cria uma única string de todas as strings que não das
// extremidades. Caso não haja nenhuma, devolve NULL
*/
char *catCommand(char **strings, int str_num) {
	int length = 0, i;
	char *buffer;

	if (str_num < 3)
		return NULL;
	if (str_num == 3)
		return strings[1];

	for (i = 1; i < str_num - 1; i++)
		length += strlen(strings[i]);

	/* Espaço para todas as strings, espaços e '\0' no final */
	buffer = malloc((length + (str_num - 2) + 1) * sizeof(char));

	length = 0;
	for (i = 1; i < str_num - 1; i++) {
		strcat(buffer, strings[i]);
		length += strlen(strings[i]);
		buffer[length] = ' ';
		length++; /* devido ao ' ' extra */
	}
	buffer[length - 1] = '\0';

	return buffer;
}


void createProcess(char *domain, char *command, char *argument) {
	int pid = fork();

	if (pid != 0) /* processo pai */
		wait(NULL); /* espera processo filho acabar (deveria?) */
	else { /* processo filho */
		execl(domain, domain, command, argument);
		/*execl(domain, process (ultima string do domain), comand, argument); */
		return;
	}
}


void runExecutable(char *name) {
	/* num sei qual exec seria melhor */
}


void date() {
	char buffer[32];
	struct tm *currentDate;
	size_t last;
	time_t timestamp = time(NULL);

	currentDate = localtime(&timestamp);
	last = strftime(buffer, 32, "%c", currentDate);
	buffer[last] = '\0';

	printf("%s\n", buffer);
}


void myChown() {

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
		char *bar_pos, *dot_pos, **strings;
		int str_num;

		if (strcmp(line, "")) { /* linha não é uma string vazia */
			add_history(line);

			if (!strcmp(line, "date")) {
				date();
			}
			else if(!(strcmp(line, "exit"))) {
				/* fecha o programa */
				free(line);
				return EXIT_SUCCESS;
			}
			else {
				strings = leLinha(line, &str_num);
				bar_pos = strchr(line, '/');
				dot_pos = strchr(line, '.');

				if ((bar_pos - line) == 0) { /* é chamada de processo (?), começa com '/' */
					createProcess(strings[0], catCommand(strings, str_num), strings[str_num - 1]);
					/* catCommand(strings, str_num); */
				}
				else if((bar_pos - line) == 1 && (dot_pos - line) == 0) {
					printf("É UM EXECUTÁVEL\n");
				}
				else 
					printf("%s: comando não encontrado\n", line);
			}
		}

		free(line);
	}
}