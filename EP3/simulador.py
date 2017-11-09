#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################

# Abre o arquivo com titulo 'nome' e retorna uma lista de listas com
# os elementos do arquivo de trace

def simula(arquivo, espaco, subst, intervalo):
	linhas = 0
	with open(arquivo) as f:
		linhas = f.readlines()
	linha = linhas[0].split()
	total = int(linha[0])
	virtual = int(linha[1])
	s = int(linha[2])
	p = int(linha[3])

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

	print ("Falta o resto do programa safadão")