#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################
import memoria as mem
import estruturas as es


def simula(arquivo, espaco, subst, intervalo):
	linhas = 0
	with open(arquivo) as f:
		linhas = f.readlines()
	linha = linhas[0].split()
	num = len(linhas) # Numero de linhas no arquivo de trace
	s = int(linha[2]) # Tamanho da unidade de alocacao
	p = int(linha[3]) # Tamanho da pagina
	fila = es.FilaDeAcessos()
	pid_dict = {}

	# Instancia as memorias
	vir = mem.Virtual(int(linha[1]), s, p, espaco, linhas)
	fis = mem.Fisica(int(linha[0]), s, p, subst)

	# Descobre quando que a simulacao vai acabar
	tempos_finais = []
	for i in range(1, num):
		linha = linhas[i].split()
		if linha[1] != 'COMPACTAR':
			tempos_finais.append(int(linha[1]))
	last_tf = max(tempos_finais)


	t = 0
	i = 1 # Linha 0 ja foi interpretada
	linha = linhas[i].split()
	while  True:
		while i < num and int(linha[0]) == t:
			if len(linha) == 2 and linha[1] == 'COMPACTAR': # Excecao para COMPACTAR
				fis.memoria.compactar()
				vir.memoria.compactar()
			else: # adiciona processo na fila e memoria virtual
				proc = es.Processo(linha)
				fila.push(proc)
				pid_dict[proc.nome] = proc.pid
				vir.insere(proc)


			if i < num:
				i += 1
				if i < num:
					linha = linhas[i].split()


		while fila.size > 0 and fila.peak().prox_acesso()[1] == t: # Processo acessando memoria fisica
			proc = fila.peak()
			# enfiar proc na memoria fisica aqui
			fila.pop()


		for j in range(1, num):
			temp = linhas[j].split()
			if len(temp) > 2 and int(temp[1]) == t: # tf = t
				pid = pid_dict.get(temp[3])
				#fis.remove(pid)
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

		t += 1