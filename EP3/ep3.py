#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################

import readline
import time
import simulador as sim

ger = {1: "Best Fit", 2: "Worst Fit", 3: "Quick Fit"}
subst = {1: "Optimal", 2: "FIFO", 3: "LRU.2", 4: "LRU.4"}

######################### VARIAVEIS GLOBAIS #########################

arquivo = ""
esp = 0
sub = 0
intervalo = 0
start_time = time.time()


# Retorna o tempo de execucao atual
def get_time():
	print(s)
	return (time.time() - start_time)


def main():
	while True:
		linha = raw_input("[ep3]: ")
		linha = linha.split()

		if linha[0] == "carrega":
			arquivo = linha[1]
			print ("Carregando arquivo " + arquivo)

		elif linha[0] == "espaco":
			esp = int(linha[1])
			print ("Gerenciamento de espaco definido para " + ger[esp])

		elif linha[0] == "substitui":
			sub = int(linha[1])
			print ("Algoritmo de substituicao definido para " + subst[sub])

		elif linha[0] == "executa":
			intervalo = int(linha[1])
			print ("Intervalo definido como " + str(intervalo))
			# print ("Executando " + arquivo + " " + ger[esp] + " " + subst[sub])

			sim.simula(arquivo, esp, sub, intervalo)
			break

		elif linha[0] == "sai":
			break

		else:
			print ("Comando invalido")
	return
main()
