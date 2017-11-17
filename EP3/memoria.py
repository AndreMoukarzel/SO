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
	ua = 0 # unidade de alocacao
	pag = 0 # tamanho da pagina
	ll = None # Lista ligada dos espacos livres


	def __init__(self, nome, tamanho, ua, pag):
		self.tam = tamanho
		self.arquivo = nome
		self.ua = ua
		self.pag = pag

		self.criaArquivo()
		self.bitmap = ((tamanho/ua) + 1) * bitarray('0')
		self.atualizaLL()


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
			bit = str(int(self.bitmap[int(i/self.ua)]))
			if l[i] == 128:
				if not i%self.ua:
					print (str(i) + '\t|\t' + str(-1) + '\t|\t' + bit)
				else:
					print (str(i) + '\t|\t' + str(-1) + '\t|')
			else:
				if not i%self.ua:
					print (str(i) + '\t|\t' + str(l[i]) + '\t|\t' + bit)
				else:
					print (str(i) + '\t|\t' + str(l[i]) + '\t|')


	# Insere processo de PID comecando na posicao pos
	def insere(self, processo, pos):
		l = self.read()
		b = int(math.ceil(processo.b/float(self.ua)) * self.ua) # Unidades de alocacao sao preenchidas

		for i in range(pos, pos + b): # Atualiza bitmap
			l[i] = processo.pid
			self.bitmap[int(i/self.ua)] = True

		self.atualizaLL()
		self.write(l)


	# Remove processo com o PID dado
	def remove(self, pid):
		l = self.read()

		# Encontra a posicao em que o processo comeca
		ret = -1
		i = 0
		for i in range(0, self.tam, self.pag):
			if l[i] == pid:
				ret = i
				break
		while i < self.tam and l[i] == pid:
			l[i] = 128
			self.bitmap[int(i/self.ua)] = False
			i += 1

		self.atualizaLL()
		self.write(l)
		return ret # -1 eh erro


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
	def compacta(self):
		mem = self.read()
		compactado = False
		ocupado = self.tam

		while not compactado:
			# Procura a primeiro pagina livre
			cheio = True
			for i in range(0, self.tam, self.ua):
				if mem[i] == 128:
					cheio = False
					livre = i
					break
				else:
					i += self.pag
					continue
			# Ja esta compactado se esta cheio
			if cheio:
				break

			# Procura a proxima pagina ocupada
			for i in range(livre, self.tam, self.ua):
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
			for i in range(0, self.tam, self.pag):
				compactado = True
				if mem[i] == 128:
					fim = 1
				if fim == 1 and mem[i] != 128:
					compactado = False
					break
			# Essa checagem n deve mais funcionar 100%. Eh melhor usarmos outra condicao de parada

		for i in range(0, self.tam, self.ua): # Atualiza o bitmap
			if mem[i] == 128:
				self.bitmap[int(i/self.ua)] = False
			else:
				self.bitmap[int(i/self.ua)] = True

		self.write(mem)


	def bytes_to_int(self, bytes):
		return int(bytes.encode('hex'), 16)


	# "Atualiza" a lista ligada que representa o bitmap da memoria criando uma
	# nova lista do zero e retorna a sua raiz
	def atualizaLL(self):
		raiz = es.listaLigada()
		last = es.listaLigada() # Ultimo elemento da lista
		ini, fim = 0, 0
		dif = self.pag/float(self.ua)

		while fim < len(self.bitmap) and ini < len(self.bitmap):
			# Procura um bloco livre
			while ini < len(self.bitmap) and self.bitmap[int(ini)]: # bitmap[ini] = 1
				ini += dif
			fim = int(ini) + 1
			# E define seu tamanho
			while fim < len(self.bitmap) and not self.bitmap[fim]: # bitmap[fim] = 0
				fim += 1

			if ini < len(self.bitmap) and fim <= len(self.bitmap):
				ini = int(ini)
				if raiz.pos == -1:
					raiz.iniRaiz(ini * self.ua, (fim - ini) * self.ua)
					last = raiz
				else:
					last.insere(last, ini * self.ua, (fim - ini) * self.ua)
					last = last.prox
			ini = fim + 1

		self.ll = raiz



class Fisica:
	memoria = None
	alg = 0
	paginas = [] # Paginas locais dos processos presentes na memoria
	proc_dic = {}
	filaFIFO = []
	matrizLRU2 = []
	k = 0
	vetorLRU4 = []
	vetorR = []

	def __init__(self, tamanho, ua, pag, alg):
		self.memoria = Memoria('/tmp/ep3.mem', tamanho, ua, pag)
		self.alg = alg

		for i in range(tamanho/pag):
			self.paginas.append(-1)

		if alg == 3:
			iniciaLRU2()
		elif alg == 4:
			iniciaLRU4()


	# Tenta inserir o processo na memoria fisica, retorna 1 se conseguir, 0 cc
	# Tambem insere o endereco local (pagina) do processo no vetor de paginas
	# da memoria
	def insere(self, processo, pagina):
		l = self.memoria.read()

		# Procura espaco livre
		for i in range(0, self.memoria.tam, self.memoria.pag):
			if l[i] == 128 and i <= self.memoria.tam - self.memoria.pag:
				# Coloca o processo na memoria
				for j in range(i, i + self.memoria.pag):
					l[j] = processo.pid
					self.memoria.bitmap[j / self.memoria.pag] = True

				# Atualiza a lista do processo de paginas presente na memoria
				processo.presente.append(pagina)
				# Atualiza o vetor de paginas da memoria
				self.paginas[i / self.memoria.pag] = pagina

				# Atualiza a lista do FIFO, caso esse seja o algoritmo usado
				if self.alg == 2:
					self.filaFIFO.append(processo)
				# atualiza estruturas dos outros algoritmos

				self.memoria.write(l)
				return 1
		return 0


	def compacta(self):
		self.memoria.compacta()
		# atualiza paginas

	def atualizaDict(self, proc_dict):
		self.proc_dict = proc_dict


	def substitui(self, processo, pagina):
		if self.alg == 2:
			self.FIFO(processo, pagina)

	# Mantem uma fila dos processos em ordem de chegada e tira eles nessa ordem
	# Para inserir um processo na fila so usar append()
	def FIFO(self, processo, pagina):
		mem = self.memoria.read()
		if len(self.filaFIFO) > 0:
			rem = self.filaFIFO[0] # Processo mais antigo
			del(self.filaFIFO[0])
			pos = self.memoria.remove(rem.pid) # Posicao do processo removido
			pag_removida = self.paginas[pos] # Pagina local do processo removida

			# Remove a pagina do vetor de paginas presentes do processo
			del(rem.presente[rem.presente.index(pag_removida)])

			if pos == -1:
				print("oh no")

			self.insere(processo, pagina)
		else:
			print("Mas a fila esta vazia D:")


	def LRU2(self, processo, pagina):
		mem = self.memoria.read()
		somas = []
		for i in range(len(self.matrizLRU2)):
			somas.append(sum(self.matrizLRU2[i]))

		# Posicao da menor soma, ou seja, pos do processo que sera removido
		subst = somas.index(min(somas))
		self.remove(mem[rem * self.bloco])


	def iniciaLRU2(): # LRU 2: Inicia a matriz nxn com 0
		n = self.memoria.tam/self.memoria.pag
		for i in range(n):
			temp = []
			for j in range(n):
				temp.append(0)
			self.matrizLRU2.append(temp)


	def atualizaLRU2():
		for i in range(self.memoria.pag): # Todos da linha = 1
			self.matrizLRU2[pos][i] = 1
		for i in range(self.memoria.pag): # Todos da coluna = 0
			self.matrizLRU2[i][self.k] = 0

		self.k = (self.k + 1) % self.memoria.tam/self.memoria.pag


	def LRU4(self, processo):
		# Se o processo foi acessado na ultima iteracao, soma no seu contador
		# no bit mais significativo
		for i in range(len(self.vetorR)):
			# Esse vetor de R tem que ser pra cada pagina, e nao pra cada processo, aparentemente
			if self.vetorR[i]:
				# vetorLRU4 tem o mesmo tamanho do de bit R
				self.vetorLRU4[i] += 100000000
			self.vetorLRU4[i] /= 10 # Desloca 1 bit pra direita

		# Remove o menos acessado
		rem = self.vetorLRU4.index(min(self.vetorLRU4))
		mem = self.memoria.read()
		self.memoria.remove(mem[rem * self.bloco])
		self.memoria.insere


	def iniciaLRU4(): # LRU 4: Inicia o vetor de contadores com 0
		for i in range(self.memoria.tam/self.memoria.pag):
			vetorLRU4.append(0)
			vetorR.append(False)


	# Recebe uma lista das paginas que foram acessadas nessa iteracao e atualiza
	# o vetor de bit R
	def atualizaVetorR(self, acessos):
		for i in range(len(self.vetorR)):
			self.vetorR[i] = False
		for i in acessos:
			self.vetorR[i] = True




class Virtual:
	memoria = None
	alg = 0
	ql = None

	class QuickList:
		tams = [] # Tamanhos monitorados
		espacos = [] # Vetor de vetores. espacos[i] contem as posicoes em q cabe um processo de tamanho tams[i]

		def __init__(self, trace):
			tam = len(trace)

			for i in range(1, tam):
				linha = trace[i].split()
				if len(linha) > 2: # Ignora compactar
					if not int(linha[2]) in self.tams:
						self.tams.append(int(linha[2]))
						self.espacos.append([])

		def atualiza(listaligada):
			l = es.listaLigada()
			l = listaligada
			tamanho = len(self.tams)

			for i in range(tamanho): # Reseta espacos conhecidos
				self.espacos[i] = []
			while l != None and l.pos != -1:
				for i in range(tamanho):
					if l.tam >= self.tams[i]:
						self.espacos[i].append(l.pos)
				l = l.prox


	def __init__(self, tamanho, ua, pag, alg, trace):
		self.memoria = Memoria('/tmp/ep3.vir', tamanho, ua, pag)
		self.alg = alg

		if alg == 3: # QuickFit
			self.ql = QuickList(trace)
			self.ql.atualiza(self.memoria.ll)


	def insere(self, processo):
		if self.alg == 1:
			self.bestFit(processo)
		elif self.alg == 2:
			self.worstFit(processo)
		elif self.alg == 3:
			self.quickFit(processo)
			self.ql.atualiza(self.memoria.ll)


	def compacta(self):
		self.memoria.compacta()


	def bestFit(self, processo):
		mem = self.memoria.read()
		best_index = 0
		best_tam = self.memoria.tam
		p_tam = processo.b # tamanho do processo
		ll = self.memoria.ll

		while ll != None and ll.pos != -1:
			if ll.tam < best_tam and ll.tam > p_tam:
					best_tam = ll.tam
					best_index = ll.pos
			ll = ll.prox

		self.memoria.insere(processo, best_index)


	def worstFit(self, processo):
		mem = self.memoria.read()
		worst_index = 0
		worst_tam = 0
		p_tam = processo.b # tamanho do processo
		ll = self.memoria.ll

		while ll != None and ll.pos != -1:
			if ll.tam > worst_tam:
					worst_tam = ll.tam
					worst_index = ll.pos
			ll = ll.prox

		if worst_tam < p_tam:
			print ("Nao ha espaco na memoria")

		self.memoria.insere(processo, worst_index)


	def quickFit(self, processo):
		ql = self.ql
		i = 0
		for i in range(len(ql.tams)):
			if processo.b == ql.tams[i]:
				break

		self.memoria.insere(processo, ql.espacos[i].pop())
