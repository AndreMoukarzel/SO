#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166	#
# Nome: Henrique Cerquinho                          NUSP: 9793700	#
#####################################################################
import struct

class Memoria:
	tam = 0
	arquivo = 0

	def __init__(self, nome, tamanho):
		self.tam = tamanho
		self.arquivo = nome

		### Preenche arquivo binario com -1 ###
		binarios = bytearray()
		for i in range(tamanho):
			binarios.append(128) # 128 = -1 em binario
		f = open(nome, 'w+b')
		f.write(binarios)
		f.close()
		#######################################

		print (self.read())


	def read(self):
		lista = []

		with open(self.arquivo, "rb") as f:
			byte = f.read(1)
			while byte != "":
				lista.append(self.bytes_to_int(byte))
				byte = f.read(1)

		return lista

	def bytes_to_int(self, bytes):
		return int(bytes.encode('hex'), 16)


def simula(arquivo, espaco, subst, intervalo):
	linhas = 0
	with open(arquivo) as f:
		linhas = f.readlines()
	linha = linhas[0].split()
	fis_total = int(linha[0]) # Memoria fisica total
	vir_total = int(linha[1]) # Memoria virtual total
	s = int(linha[2]) # Tamanho da pagina
	p = int(linha[3]) # Tamanho da unidade de alocacao

	Memoria('/tmp/ep3.mem', fis_total)
	Memoria('/tmp/ep3.vir', fis_total)
