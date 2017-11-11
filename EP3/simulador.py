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
		f = open(self.arquivo, 'w+b') # Constroi o arquivo
		f.close()
		l = []
		for i in range (self.tam):
			l.append(128) # Preenche com -1
		self.write(l)


	def imprime(self):
		l = self.read()
		for i in range(self.tam):
			print (str(i) + ' | ' + str(l[i]) + ' | ' + str(int(self.bitmap[i])))


	# Insere processo de PID que ocupa b blocos de memoria, comecando na posicao pos
	def insere(self, pid, pos, b):
		l = self.read()

		for i in range(b):
			l[pos + i] = pid
			self.bitmap[pos + 1] = True

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
		mem = self.read()
		compactado = False
		ocupado = self.tam

		while not compactado:
			# Procura o primeiro bloco livre
			cheio = True
			for i in range(self.tam):
				if mem[i] == 128:
					cheio = False
					livre = i
					break
			# Ja esta compactado se esta cheio
			if cheio:
				break

			# Procura o proximo bloco ocupado
			for i in range(livre + 1, self.tam):
				if mem[i] != 128:
					ocupado = i
					break


			# Move os blocos ocupados para a esquerda
			while ocupado < self.tam:
				# Move o bloco inteiro
				while mem[ocupado] != 128:
					mem[livre] = mem[ocupado]
					mem[ocupado] = 128

				livre += 1
				ocupado += 1

			# Checa se compactou
			fim = 0
			for i in range(self.tam):
				compactado = True
				if mem[i] == 128:
					fim = 1
				if fim == 1 and mem[i] != 128:
					compactado = False
					break
		
		for i in range(self.tam): # Atualiza o bitmap
			if mem[i] == 128:
				self.bitmap[i] = False
			else:
				self.bitmap[i] = True

		self.write(mem)


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

	memoria.insere(3, best_index, p_paginas) # substituir 3 por PID