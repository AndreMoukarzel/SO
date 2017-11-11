#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################
import math

class Memoria:
	tam = 0
	arquivo = 0

	def __init__(self, nome, tamanho):
		self.tam = tamanho
		self.arquivo = nome

		self.criaArquivo()


	def criaArquivo(self):
		f = open(self.arquivo, 'w+b')
		f.close()
		l = []
		for i in range (self.tam):
			l.append(128)
		self.write(l)


	def read(self):
		lista = []

		with open(self.arquivo, "rb") as f:
			byte = f.read(1)
			while byte != "":
				lista.append(self.bytes_to_int(byte))
				byte = f.read(1)
		f.close()

		return lista


	def write(self, lista):
		binarios = bytearray(lista)
		f = open(self.arquivo, 'r+')
		f.write(binarios)
		f.close()


	def compactar(self):
		print ("Haha eu n fiz o compactar ainda lololkillmelololol")


	def bytes_to_int(self, bytes):
		return int(bytes.encode('hex'), 16)



def simula(arquivo, espaco, subst, intervalo):
	linhas = 0
	with open(arquivo) as f:
		linhas = f.readlines()
	linha = linhas[0].split()
	fis_total = int(linha[0]) # Memoria fisica total
	vir_total = int(linha[1]) # Memoria virtual total
	s = int(linha[2]) # Tamanho da pagina
	p = int(linha[3]) # Tamanho da unidade de alocacao

	fis = Memoria('/tmp/ep3.mem', fis_total)
	vir = Memoria('/tmp/ep3.vir', vir_total)

	bestFit(vir, "ronaldo", int(math.ceil(8/s)))
	print (vir.read())


# Insere p com p_paginas na memoria especificada
def bestFit(memoria, p_nome, p_paginas):
	mem = memoria.read()
	best_index = 0
	best_tam = memoria.tam
	ini = 0

	for i in range(memoria.tam):
		# Deve considerar o fim da memoria para n perder intervalos ao final
		if mem[i] != 128 or i == memoria.tam - 1: # 128 = -1 em binario
			if i - ini < best_tam and i - ini > p_paginas:
				best_tam = i - ini
				best_index = ini
			ini = i + 1

	for i in range(p_paginas):
		mem[best_index + i] = 3 # PID do processo no lugar de 3 aqui

	memoria.write(mem)