#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################

import readline
import readFile
import time

ger = {1: "Best Fit", 2: "Worst Fit", 3: "Quick Fit"}
sub = {1: "Optimal", 2: "FIFO", 3: "LRU.2", 4: "LRU.4"}

######################### VARIAVEIS GLOBAIS #########################
total = 0
virtual = 0
s = 0
p = 0
# Retorna o tempo de execucao atual
start_time = time.time()
def get_time():
	print(s)
	return (time.time() - start_time)


def main():

	while True:
		linha = raw_input("[ep3]: ")
		linha = linha.split()

		if linha[0] == "carrega":
			print ("Carregando arquivo " + linha[1])
			dados = readFile.read_file(linha[1])

			global total, virtual, s, p
			total = dados[0][0]
			virtual = dados[0][1]
			s = dados[0][2]
			p = dados[0][3]

		elif linha[0] == "espaco":
			print ("Gerenciamento de espaco definido para " + ger[int(linha[1])])
			# Settar alguma variavel como linha[1] aqui

		elif linha[0] == "substitui":
			print ("Algoritmo de substituicao definido para " + sub[int(linha[1])])
			# Settar alguma variavel como linha[1] aqui

		elif linha[0] == "executa":
			# Gera o arquivo de memoria fisica no diretorio /tmp
			# e preenche  com -1
			with open('/tmp/ep3.mem', 'wb') as f:
					for i in range(total):
		    				f.write('-1 ')
						# 1 linha pra cada byte?
						if (not i%8 and i != 0):
								f.write('\n')

			# E o de memoria virtual
			with open('/tmp/ep3.vir', 'wb') as f:
					for i in range(virtual):
		    				f.write('-1 ')
						# 1 linha pra cada byte?
						if (not i%8 and i != 0):
								f.write('\n')

			print ("BLIP BLOP BLUP")

		elif linha[0] == "sai":
			break

		else:
			print ("Comando invalido")
	return
main()