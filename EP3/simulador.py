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
	ll = None # Lista ligada dos espacos livres
	tipo = None # Tipo de memoria (fis/vir)
	alg = None # Algoritmo usado na memoria

	filaFIFO = []

	matrizLRU2 = []
	k = 0

	def __init__(self, nome, tamanho, bloco, tipo, alg):
		self.tam = tamanho
		self.arquivo = nome
		self.bloco = bloco
		self.tipo = tipo
		self.alg = alg

		self.criaArquivo()
		self.bitmap = ((tamanho/bloco) + 1) * bitarray('0')
		self.ll = es.listaLigada()
		self.ll.iniRaiz(0, tamanho)
		# Inicializa as estruturas caso seja a memoria fisica
		if tipo == 'fisica':
			if alg == "LRU.2":
				# LRU 2: Inicia a matriz nxn com 0
				n = self.tam/self.bloco
				for i in range(n):
					temp = []
					for j in range(n):
						temp.append(0)
					self.matrizLRU2.append(temp)
			# elif alg == outro algoritmo
				# bip bop bup

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

		for i in range(pos, pos + by): # Atualiza bitmap
			l[i] = pid
			self.bitmap[int(i/self.bloco)] = True

		self.atualizaLL()
		self.write(l)

		if self.tipo == 'fisica':
			# Atualiza a matriz
			if self.alg == "LRU.2":
				for i in range(bloco): # Todos da linha = 1
					self.matrizLRU2[pos][i] = 1
				for i in range(bloco): # Todos da coluna = 0
					self.matrizLRU2[i][self.k] = 0

				self.k = (self.k + 1) % self.tam/self.bloco

			# Coloca o processo no fim da fila
			elif self.alg == "FIFO":
				self.filaFIFO.append(pid)

	# Remove processo com o PID dado
	def remove(self, pid):
		l = self.read()

		# Encontra a posicao em que o processo comeca
		for i in range(0, len(l), self.bloco):
			if l[i] == pid:
				break
		while l[i] == pid:
			l[i] = 128
			self.bitmap[int(i/self.bloco)] = False
			i += 1

		self.atualizaLL()
		self.write(l)


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
	def atualizaLL(self):
		raiz = es.listaLigada()
		last = es.listaLigada() # Ultimo elemento da lista
		ini, fim = 0, 0

		while fim < len(self.bitmap):
			# Procura um bloco livre
			while ini < len(self.bitmap) and self.bitmap[ini]: # bitmap[ini] = 1
				ini += 1
			fim = ini + 1
			# E define seu tamanho
			while fim < len(self.bitmap) and not self.bitmap[fim]: # bitmap[fim] = 0
				fim += 1

			if raiz.pos == -1:
				raiz.iniRaiz(ini * self.bloco, (fim - ini) * self.bloco)
				last = raiz
			else:
				last.insere(last, ini * self.bloco, (fim - ini) * self.bloco)
				last = last.prox

		self.ll = raiz


	# SUBSTITUICAO DE PAGINAS #

	# Mantem uma fila dos processos em ordem de chegada e tira eles nessa ordem
	# Para inserir um processo na fila so usar append()
	def FIFO(self, pid):
		mem = self.read()
		if len(self.filaFIFO) > 0:
			rem = pop(self.filaFIFO).pid # PID do processo mais antigo
			self.remove(rem)
		else:
			print("Mas a fila esta vazia D:")
		return fila


	def LRU2(self, pos):
		mem = self.read()
		somas = []
		for i in range(len(self.matrizLRU2)):
			somas.append(sum(self.matrizLRU2[i]))

		# Posicao da menor soma, ou seja, pos do processo que sera removido
		subst = somas.index(min(somas))




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

	vir = Memoria('/tmp/ep3.vir', vir_total, s, 'virtual', espaco)
	fis = Memoria('/tmp/ep3.mem', fis_total, p, 'fisica', subst)


	# TESTEs
	bestFit(vir, es.Processo(linhas[1].split()))
	bestFit(vir, es.Processo(linhas[2].split()))
	es.printLista(vir.ll)
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
				fis.remove(temp)
				vir.remove(temp)


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


# GERENCIAMENTO DE MEMORIA #

def bestFit(memoria, processo):
	mem = memoria.read()
	best_index = 0
	best_tam = memoria.tam
	p_tam = processo.b # tamanho do processo
	ll = memoria.ll

	while ll != None:
		if ll.tam < best_tam and ll.tam > p_tam:
				best_tam = ll.tam
				best_index = ll.pos
		ll = ll.prox

	memoria.insere(processo.pid, best_index, p_tam)


def worstFit(memoria, processo):
	mem = memoria.read()
	worst_index = 0
	worst_tam = 0
	p_tam = processo.b # tamanho do processo
	ll = memoria.ll

	while ll != None:
		if ll.tam > worst_tam:
				worst_tam = ll.tam
				worst_index = ll.pos
		ll = ll.prox

	if worst_tam < p_tam:
		print ("NAO CABE NA MEMORIA AAAAAAAAAAA")

	memoria.insere(processo.pid, worst_index, p_tam)




