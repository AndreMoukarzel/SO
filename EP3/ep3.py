#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################

import readline
import time

ger = {1: "Best Fit", 2: "Worst Fit", 3: "Quick Fit"}
sub = {1: "Optimal", 2: "FIFO", 3: "LRU.2", 4: "LRU.4"}


def readFile(nome):
	dados = []
	arquivo = open(nome, "r")

	for linha in arquivo:
		dados.append(linha.split())

	# Converte todas as string de numero para ints
	for i in range(len(dados)):
		for j in range(len(dados[i])):
			try:
				dados[i][j] = int(dados[i][j])
			except:
				continue
	return dados


def main():
	
	while True:
		linha = raw_input("[ep3]: ")
		linha = linha.split()

		if linha[0] == "carrega":
			print ("Carregando arquivo " + linha[1])
			dados = readFile(linha[1])
			print(dados)

		elif linha[0] == "espaco":
			print ("Gerenciamento de espaco definido para " + ger[int(linha[1])])
			# Settar alguma variavel como linha[1] aqui

		elif linha[0] == "substitui":
			print ("Algoritmo de substituicao definido para " + sub[int(linha[1])])
			# Settar alguma variavel como linha[1] aqui

		elif linha[0] == "executa":
			print ("BLIP BLOP BLUP")

		elif linha[0] == "sai":
			break

		else:
			print ("Comando invalido")
	return
main()