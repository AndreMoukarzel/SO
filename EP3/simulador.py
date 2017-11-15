#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################
import math
import estruturas as es
from bitarray import bitarray

class Memoria:
	tam = 0
	arquivo = 0
	bitmap = 0
	bloco = 0 # unidade de alocacao na memoria virtual, tamanho da pagina na fisica

	def __init__(self, nome, tamanho, bloco):
		self.tam = tamanho
		self.arquivo = nome
		self.bloco = bloco

		self.criaArquivo()
		self.bitmap = ((tamanho/bloco) + 1) * bitarray('0')
		# Devemos completar a memoria ate que ela seja um multiplo de 'bloco'?


	def criaArquivo(self):
		f = open(self.arquivo, 'w+b') # Constroi o arquivo
		f.close()
		l = []
		for i in range (self.tam):
			l.append(128) # Preenche com -1
		self.write(l)


	def imprime(self):
		l = self.read()
		print ('Indice\t|\tPID\t|\tBitmap')
		for i in range(self.tam):
			if l[i] == 128:
				print (str(i) + '\t|\t' + str(-1) + '\t|\t' + str(int(self.bitmap[int(i/self.bloco)])))
			else:
				print (str(i) + '\t|\t' + str(l[i]) + '\t|\t' + str(int(self.bitmap[int(i/self.bloco)])))


	# Insere processo de PID que ocupa by bytes, comecando na posicao pos
	def insere(self, pid, pos, by):
		l = self.read()

		for i in range(pos, by):
			l[i] = pid
			self.bitmap[int(i/self.bloco)] = True

		self.write(l)


	# Remove processo que comeca na posicao pos
	def remove(self, pos):
		l = self.read()
		i = pos
		pid = l[i]

		while l[i] == pid:
			l[i] = 128
			self.bitmap[int(i/self.bloco)] = False
			i += 1

		self.write()


	# Devolve a memoria como uma lista de PIDs (int)
	def read(self):
		lista = []

		with open(self.arquivo, "rb") as f:
			byte = f.read(1)
			while byte != "":
				lista.append(self.bytes_to_int(byte))
				byte = f.read(1)
		f.close()

		return lista


	# Define a memoria como a lista de PIDs passada como argumento
	def write(self, lista):
		binarios = bytearray(lista)
		f = open(self.arquivo, 'r+')
		f.write(binarios)
		f.close()


	# Compacta memoria
	def compactar(self):
		mem = self.read()
		compactado = False
		ocupado = self.tam

		while not compactado:
			# Procura o primeiro bloco livre
			cheio = True
			for i in range(0, self.tam, self.bloco):
				if mem[i] == 128:
					cheio = False
					livre = i
					break
			# Ja esta compactado se esta cheio
			if cheio:
				break

			# Procura o proximo bloco ocupado
			for i in range(livre + 1, self.tam, self.bloco):
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
			for i in range(0, self.tam, self.bloco):
				compactado = True
				if mem[i] == 128:
					fim = 1
				if fim == 1 and mem[i] != 128:
					compactado = False
					break

		for i in range(0, self.tam, self.bloco): # Atualiza o bitmap
			if mem[i] == 128:
				self.bitmap[int(i/self.bloco)] = False
			else:
				self.bitmap[int(i/self.bloco)] = True

		self.write(mem)


	def bytes_to_int(self, bytes):
		return int(bytes.encode('hex'), 16)


# "Atualiza" a lista ligada que representa o bitmap da memoria criando uma
# nova lista do zero e retorna a sua raiz
def atualizaLL(memoria):
	raiz = es.listaLigada()
	last = es.listaLigada() # Ultimo elemento da lista
	ini, fim = 0, 0

	while fim < len(memoria.bitmap):
		# Procura um bloco livre
		while ini < len(memoria.bitmap) and memoria.bitmap[ini]:
			ini += 1
		fim = ini + 1
		# E define seu tamanho
		while fim < len(memoria.bitmap) and not memoria.bitmap[fim]:
			fim += 1

		if raiz.pos == -1:
			raiz.iniRaiz(ini, fim - ini)
			last = raiz
		else:
			last.insere(last, ini, fim - ini)
			last = last.prox

	return raiz



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
	fila = es.FilaDeAcessos()

	# Descobre quando que a simulacao acabara
	tempos_finais = []
	for i in range(1, num):
		linha = linhas[i].split()
		if linha[1] != 'COMPACTAR':
			tempos_finais.append(int(linha[1]))
	last_tf = max(tempos_finais)

	vir = Memoria('/tmp/ep3.vir', vir_total, s)
	fis = Memoria('/tmp/ep3.mem', fis_total, p)


	# TESTEs
	bestFit(vir, "ronaldo", int(math.ceil(8/s)), 2)
	raiz = atualizaLL(vir)
	es.printLista(raiz)
	print("RONALDO")


	t = 0
	i = 1 # Linha 0 ja foi interpretada
	while  True:
		linha = linhas[i].split()
		while i < num - 1 and int(linha[0]) == t:
			if len(linha) == 2 and linha[1] == 'COMPACTAR': # Excecao para COMPACTAR
				fis.compactar()
				vir.compactar()
			else: # adiciona processo na fila
				fila.push(es.Processo(linha))

			if i < num -1:
				i += 1
				linha = linhas[i].split()


		while fila.size > 0 and fila.peak().prox_acesso()[1] == t: # Processo acessando memoria fisica
			proc = fila.peak()
			# enfiar proc na memoria fisica aqui
			fila.pop()


		for j in range(num):
			temp = linhas[i].split()
			if len(temp) > 2 and temp[1] == t: # tf = t
				# remover o processo temp de ambas as memorias
				pass


		if t % intervalo == 0: # Imprime informacoess
			print (30 * '-' + '\nTempo = ' + str(t) + '\n')
			print ('Memoria Virtual')
			vir.imprime()
			print ('\nMemoria Fisica')
			fis.imprime()
			print ('\n' + 30 * '-' + '\n')

		if i >= num - 1 and t >= last_tf: # Todos os processos acabaram sua execucao
			break

		t += 1



# Insere p com p_paginas na memoria especificada
def bestFit(memoria, p_nome, p_paginas, pid):
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

	memoria.insere(pid, best_index, p_paginas) # substituir 3 por PID