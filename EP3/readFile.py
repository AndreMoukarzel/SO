#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################

# Abre o arquivo com titulo 'nome' e retorna uma lista de listas com
# os elementos do arquivo de trace

def read_file(nome):
	dados = []
	arquivo = open(nome, "r")

	for linha in arquivo:
		dados.append(linha.split())

	# Converte todas as string de numero para ints
	for i in range(len(dados)):
		for j in range(len(dados[i])):
			try:
				dados[i][j] = int(dados[i][j])
			except:
				continue
	return dados