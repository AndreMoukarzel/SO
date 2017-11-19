#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################
import memoria as mem
import estruturas as es
import time # RESULTADOS


def simula(arquivo, espaco, subst, intervalo):
	linhas = 0
	with open(arquivo) as f:
		linhas = f.readlines()
	linha = linhas[0].split()
	num = len(linhas) # Numero de linhas no arquivo de trace
	s = int(linha[2]) # Tamanho da unidade de alocacao
	p = int(linha[3]) # Tamanho da pagina
	fila = es.FilaDeAcessos()
	proc_dict = {}

	# Instancia as memorias
	vir = mem.Virtual(int(linha[1]), s, p, espaco, linhas)
	fis = mem.Fisica(int(linha[0]), s, p, subst, linhas)

	# Descobre quando que a simulacao vai acabar
	tempos_finais = []
	for i in range(1, num):
		linha = linhas[i].split()
		if linha[1] != 'COMPACTAR':
			tempos_finais.append(int(linha[1]))
	last_tf = max(tempos_finais)

	# RESULTADOS #
	results_tempos = []
	page_faults = 0
	##############
	t = 0
	i = 1 # Linha 0 ja foi interpretada
	linha = linhas[i].split()
	while  True:
		while i < num and int(linha[0]) == t:

			if len(linha) == 2 and linha[1] == 'COMPACTAR': # Excecao para COMPACTAR
				fis.compacta()
				vir.compacta()
			else: # adiciona processo na fila e memoria virtual
				proc = es.Processo(linha)
				fila.push(proc)
				proc_dict[proc.nome] = proc
				fis.proc_dict[proc.nome] = proc
				tempo_res = time.time()
				vir.insere(proc)
				results_tempos.append(time.time() - tempo_res)


			if i < num:
				i += 1
				if i < num:
					linha = linhas[i].split()


		while fila.size > 0 and fila.peak().prox_acesso()[1] == t: # Processo acessando memoria fisica
			proc = fila.peak()
			temp_pag = proc.prox_acesso()[0] # Pagina local que sera acessada
			if not temp_pag in fis.proc_dict[proc.nome].presente: # Checa se a pagina ja esta na memoria
				page_faults += 1 # RESULTADOS
				fis.insere(proc, temp_pag)

			elif subst == 3 or subst == 4: # Se ja estiver na memoria, grava seu acesso
				proc = fis.proc_dict[proc.nome]
				index = proc.presente.index(temp_pag)
				pos = proc.presente_pos[index]

				if subst == 3:
					fis.atualizaLRU2(pos)
				else: # subst = 4
					# Foi acessado, soma no bit mais significativo do contador
					fis.somaLRU4(pos, 0)

			fila.pop()


		for j in range(1, num): # Remove os processos que acabaram
			temp = linhas[j].split()
			if len(temp) > 2 and int(temp[1]) == t: # tf = t
				pid = proc_dict.get(temp[3]).pid
				while fis.memoria.remove(pid) != -1:
					fis.memoria.remove(pid)
				vir.memoria.remove(pid)


		if t % intervalo == 0: # Imprime informacoess
			print (30 * '-' + '\nTempo = ' + str(t) + '\n')
			print ('Memoria Virtual')
			vir.memoria.imprime()
			print ('\nMemoria Fisica')
			fis.memoria.imprime()
			print ('\n' + 30 * '-' + '\n')


		if i == num and t >= last_tf: # Todos os processos acabaram sua execucao
			break

		if subst == 4:# Atualiza os contadores
			fis.divideLRU4()

		t += 1

	print (results_tempos)
	print (page_faults)