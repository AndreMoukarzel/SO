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


	# Insere processo de PID com by bytes comecando na posicao pos
	def insere(self, pid, pos, by):
		l = self.read()
		b = int(math.ceil(by/float(self.ua)) * self.ua) # Unidades de alocacao sao preenchidas

		for i in range(pos, pos + b): # Atualiza bitmap
			l[i] = pid
			self.bitmap[int(i/self.ua)] = True

		self.atualizaLL()
		self.write(l)


	# Remove processo com o PID dado
	# Caso receba o argumento pos, remove apenas a pagina iniciada em pos
	# Caso receba um dicionario, atualiza o processo que foi removido
	def remove(self, pid, pos = -1, proc_dict = None):
		l = self.read()
		ini = 0
		fim = self.tam
		if pos != -1:
			ini = pos
			fim = pos + self.pag

			if proc_dict != None:
				for nome in proc_dict:
					if proc_dict[nome].pid == pid:
						proc = proc_dict[nome]
						break
				# Remove a pagina da lista de paginas presentes do processo
				i = proc.presente_pos.index(pos)
				del(proc.presente[i])
				del(proc.presente_pos[i])
				# Atualiza o dicionario
				proc_dict[proc.nome] = proc

		# Encontra a posicao em que o processo comeca
		ret = -1
		i = 0
		for i in range(ini, fim, self.pag):
			if l[i] == pid:
				ret = i
				break
		while i < fim and l[i] == pid:
			l[i] = 128
			self.bitmap[int(i/self.ua)] = False
			i += 1

		self.atualizaLL()
		self.write(l)
		if pos != -1:
			return proc_dict
		else:
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
			# Procura a primeira pagina livre
			cheio = True
			for i in range(0, self.tam, self.pag):
				if mem[i] == 128:
					cheio = False
					livre = i
					break
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

	proc_dict = {}
	filaFIFO = []
	matrizLRU2 = []
	k = 0
	vetorLRU4 = []
	optimal = []


	class Pagina:
	
		def __init__(self, processo, p, ins, pag):
			self.pid = processo.pid
			self.p = p # posicao relativa do processo a qual corresponde
			self.tam = pag
			dif = processo.b - int(p/pag)
			if dif > 0 and dif < pag: # p esta na ultima pagina, e a pagina n esta cheia
				self.tam = pag - dif
			self.ins = ins # posicao em que foi inserida na memoria fisica



	def __init__(self, tamanho, ua, pag, alg, linhas = None):
		self.memoria = Memoria('/tmp/ep3.mem', tamanho, ua, pag)
		self.alg = alg

		if alg == 3:
			self.iniciaLRU2()
		elif alg == 4:
			self.iniciaLRU4()
		elif alg == 1:
			self.iniciaOptimal(linhas)


	# Tenta inserir o processo na memoria fisica, substitui uma pagina caso nao consiga
	def insere(self, processo, pagina):
		ll = self.memoria.ll
		pag_tam = self.memoria.pag

		while ll != None and ll.pos != -1:
			if ll.tam >= pag_tam: # Cabe uma nova pagina
				pag = self.Pagina(processo, pagina, ll.pos, pag_tam)
				self.memoria.insere(pag.pid, ll.pos, pag.tam)
				# Atualiza a lista de paginas no processo
				# Grava a posicao da memoria que a pagina foi inserida
				processo.presente.append(pagina)
				processo.presente_pos.append(ll.pos)
				# Atualiza o dicionario da memoria
				self.proc_dict[processo.nome] = processo

				if self.alg == 2:
					self.filaFIFO.append([processo, pag])
				elif self.alg == 3:
					self.atualizaLRU2(pag.ins)
				elif self.alg == 4:
					self.somaLRU4(pag.ins, 1) # Reseta o contador dessa pagina, ja que foi removida
				elif self.alg == 1:
					self.atualizaOptimal(processo, pagina)


				return 1
			ll = ll.prox

		self.substitui(processo, pagina)


	def compacta(self):
		self.memoria.compacta()
		# atualizar vetores de presentes nos processos


	def substitui(self, processo, pagina):
		if self.alg == 2:
			self.FIFO(processo, pagina)
		elif self.alg == 3:
			self.LRU2(processo, pagina)
		elif self.alg == 4:
			self.LRU4(processo, pagina)
		elif self.alg == 1:
			self.Optimal(processo, pagina)


	# Mantem uma fila dos processos em ordem de chegada e tira eles nessa ordem
	# Para inserir um processo na fila so usar append()
	def FIFO(self, processo, pagina):
		if len(self.filaFIFO) > 0:
			proc, pag = self.filaFIFO.pop(0) # [Processo, Pagina] mais antiga
			# Remove pagina na posicao pag.ins da memoria
			self.proc_dict = self.memoria.remove(proc.pid, pag.ins, self.proc_dict)
			self.insere(processo, pagina)
		else:
			print("FIFO vazia")


	def LRU2(self, processo, pagina):
		mem = self.memoria.read()
		somas = []
		for i in range(len(self.matrizLRU2)):
			somas.append(sum(self.matrizLRU2[i]))

		# Posicao da menor soma, ou seja, pos da pagina que sera removida
		subst = somas.index(min(somas)) * self.memoria.pag
		pid = mem[subst]
		self.proc_dict = self.memoria.remove(pid, subst, self.proc_dict)
		self.insere(processo, pagina)


	def iniciaLRU2(self): # LRU 2: Inicia a matriz nxn com 0
		n = (self.memoria.tam/self.memoria.pag)
		for i in range(n):
			temp = []
			for j in range(n):
				temp.append(0)
			self.matrizLRU2.append(temp)


	def atualizaLRU2(self, pos):
		for i in range(self.memoria.pag - 1): # Todos da linha pos vao pra 1
			self.matrizLRU2[pos / self.memoria.pag][i] = 1
		for i in range(self.memoria.pag - 1): # Todos da coluna k vao pra 0
			self.matrizLRU2[i][self.k] = 0

		self.k += 1
		self.k %= len(self.matrizLRU2)


	def LRU4(self, processo, pagina):
		mem = self.memoria.read()

		# Pos da pagina que sera removida
		pos = self.vetorLRU4.index(min(self.vetorLRU4)) * self.memoria.pag
		pid = mem[pos]
		self.proc_dict = self.memoria.remove(pid, pos, self.proc_dict)
		self.insere(processo, pagina)


	def iniciaLRU4(self): # LRU 4: Inicia o vetor de contadores com 0
		for i in range(self.memoria.tam/self.memoria.pag):
			self.vetorLRU4.append(0)


	# Soma no bit mais significativo do contador, e reseta ele se zera for 1
	def somaLRU4(self, pos, zera):
		if zera:
			self.vetorLRU4[pos / self.memoria.pag] = 10000000
		else:
			self.vetorLRU4[pos / self.memoria.pag] += 10000000


	# Atualiza os contadores
	def divideLRU4(self):
		for i in range(len(self.vetorLRU4)):
			self.vetorLRU4[i] /= 10 # Desloca o contador 1 bit pra direita


	def Optimal(self, processo, pagina):
		# Ve todas as paginas na memoria, e remove a que sera acessada em um futuro mais distante
		vals = self.proc_dict.values()
		pags = [] # (pagina, pid, pos) de todas as paginas presentes
		tempos = [] # tempos dos proximos acessos das paginas atualmente na memoria
		for proc in vals:
			for pag in proc.presente:
				pags.append([pag, proc.pid, proc.presente_pos[proc.presente.index(pag)]])
				tempos.append(-1) # Se pagina nunca mais for ser acessada, mantem esse valor absurdo de tempo
				for n, p, t in self.optimal: # n = nome, p = pagina, t = tempo
					if n == proc.nome and p == pag: # Proximo acesso da pagina encontrado
						tempos.pop() # remove valor absurdo
						tempos.append(t)
						break

		i = tempos.index(max(tempos))
		print ('Pagina a ser removida')
		print (pags[i][1], pags[i][2]) # pid, pos
		print ('Pagina a entrar no seu lugar')
		print (processo.pid)
		self.proc_dict = self.memoria.remove(pags[i][1], pags[i][2], self.proc_dict)
		self.insere(processo, pagina)


	def iniciaOptimal(self, linhas):
		for i in range(1, len(linhas)):
			linha = linhas[i].split()
			if len(linha) > 2: # ignora compactar
				for j in range(4, len(linha), 2):
					novo = [linha[3], int(linha[j]), int(linha[j + 1])] # [nome, pagina, tempo do acesso]
					self.optimal.append(novo)
		self.optimal.sort(key=lambda tup: tup[2]) # ordena pelos tempos


	# Remove [processo, pagina, tempo] da lista de acessos futuros
	def atualizaOptimal(self, processo, pagina):
		# teoricamente um pop(0) funcionaria, mas vamos fazer funcionar antes, neh?
		alvo = [processo.nome, pagina]
		for i in range(len(self.optimal)):
			atual = self.optimal[i]
			if atual[0] == alvo[0] and atual[1] == alvo[1]:
				del(self.optimal[i])
				break




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
			self.ql = self.QuickList(trace)
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
		best_index = 0
		best_tam = self.memoria.tam
		p_tam = processo.b # tamanho do processo
		ll = self.memoria.ll

		while ll != None and ll.pos != -1:
			if ll.tam < best_tam and ll.tam > p_tam:
					best_tam = ll.tam
					best_index = ll.pos
			ll = ll.prox

		self.memoria.insere(processo.pid, best_index, p_tam)


	def worstFit(self, processo):
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

		self.memoria.insere(processo.pid, worst_index, p_tam)


	def quickFit(self, processo):
		ql = self.ql
		i = 0
		for i in range(len(ql.tams)):
			if processo.b == ql.tams[i]:
				break

		self.memoria.insere(processo.pid, ql.espacos[i].pop(), p_tam)
