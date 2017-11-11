#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################
import math
from bitarray import bitarray

class Memoria:
	tam = 0
	arquivo = 0
	bitmap = 0

	def __init__(self, nome, tamanho):
		self.tam = tamanho
		self.arquivo = nome

		self.criaArquivo()
		self.bitmap = tamanho * bitarray('0')


	def criaArquivo(self):
		f = open(self.arquivo, 'w+b')
		f.close()
		l = [128, 128, 3, 4, 5, 128, 128, 128, 128, 128, 2, 128, 128, 128, 10, 128] # vetor pra teste
		# for i in range (self.tam):
		# 	l.append(128) # Preenche com -1
		self.write(l)


	def imprime(self):
		l = self.read()
		for i in range(self.tam):
			print (str(i) + ' | ' + str(l[i]) + ' | ' + str(int(self.bitmap[i])))


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


	def compactar(self, tam_bloco):
		print ("vrrrrrr *compactando memoria*")
		mem = self.read()
		compactado = False
		print("antes de compactar:")
		print(mem)

		while not compactado:
			# Procura o primeiro bloco livre
			cheio = True
			for i in range(0, len(mem), tam_bloco):
				if mem[i] == 128:
					cheio = False
					livre = i
					break
			# Ja esta compactado se esta cheio
			if cheio:
				break

			# Procura o proximo bloco ocupado
			for i in range(livre + tam_bloco, len(mem), tam_bloco):
				if mem[i] != 128:
					ocupado = i
					break


			# Move os blocos ocupados para a esquerda
			while ocupado <= (len(mem) - tam_bloco) and mem[ocupado] != 128:
				# Move o bloco inteiro
				for i in range(tam_bloco):
					mem[livre + i], mem[ocupado + i] = mem[ocupado + i], mem[livre + i]

				livre += tam_bloco
				ocupado += tam_bloco

			# Checa se compactou
			fim = 0
			for i in range(0, len(mem), tam_bloco):
				compactado = True
				if mem[i] == 128:
					fim = 1
				if fim == 1 and mem[i] != 128:
					compactado = False
					break

		print("depois de compactar:")
		print(mem)


	def bytes_to_int(self, bytes):
		return int(bytes.encode('hex'), 16)



def simula(arquivo, espaco, subst, intervalo):
	linhas = 0
	with open(arquivo) as f:
		linhas = f.readlines()
	linha = linhas[0].split()
	num = len(linhas) # Numero de linhas no arquivo de trace
	fis_total = int(linha[0]) # Memoria fisica total
	vir_total = int(linha[1]) # Memoria virtual total
	s = int(linha[2]) # Tamanho da unidade de alocacao
	p = int(linha[3]) # Tamanho da pagina

	fis = Memoria('/tmp/ep3.mem', fis_total)
	vir = Memoria('/tmp/ep3.vir', vir_total)
	fis.compactar(2)

	bestFit(vir, "ronaldo", int(math.ceil(8/s))) # TESTE

	t = 0
	i = 1 # Linha 0 ja foi interpretada
	while  True:

		linha = linhas[i].split()
		while i < num and int(linha[0]) == t:

			if len(linha) == 2 and linha[1] == 'COMPACTAR': # Excecao para COMPACTAR
				fis.compactar()
				vir.compactar()

			# tratar processo adequadamente aqui
			i += 1
			if i < num:
				linha = linhas[i].split()

		if t % intervalo == 0: # Imprime informacoess
			print (30 * '-' + '\nTempo = ' + str(t) + '\n')
			print ('Memoria Virtual')
			vir.imprime()
			print ('\nMemoria Fisica')
			fis.imprime()
			print ('\n' + 30 * '-' + '\n')

		if i == num:
			# isso aqui esta incorreto, na vdd.
			# o break deve vir n quando terminamos de ler todos os processos,
			# e sim quando terminamos de ler E n vai ter mais acessos por nenhum processo.
			# vamos ter q guardar essa informacao sobre acessos em algum lugar
			break

		t += 1



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