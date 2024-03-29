#####################################################################
# Nome: Andre Ferrari Moukarzel                     NUSP: 9298166   #
# Nome: Henrique Cerquinho                          NUSP: 9793700   #
#####################################################################
import heapq

class listaLigada:
    pos = -1 # Posicao da memoria que se refere
    tam = -1 # Por quantas posicoes o espaco se repete
    prox = None # Proximo elemento da lista ligada
    ant = None # Elemento anterior da lista ligada

    # Metodos de declaracao
    def setPos(self, val):
        self.pos = val

    def setTam(self, val):
        self.tam = val

    def setProx(self, prox):
        self.prox = prox

    def setAnt(self, ant):
        self.ant = ant

    # Inicializa uma raiz
    def iniRaiz(self, pos, tam):
        self.setPos(pos)
        self.setTam(tam)

    # Metodos de manipulacao

    # Insere uma nova celula como a proxima de 'celula'
    def insere(self, celula, pos, tam):
        new = listaLigada()
        new.setPos(pos)
        new.setTam(tam)
        new.setProx(celula.prox)
        new.setAnt(celula)

        celula.setProx(new)

    # Remove a celula que vem depois de 'celula'
    def removeProx(self):
        temp = listaLigada()
        temp = self.prox
        if temp != None and temp.prox != None:
            temp.prox.setAnt(self)
            self.setProx(temp.prox)
        else:
            self.setProx(None)
        temp = None

def printLista(raiz):
    temp = listaLigada()
    temp = raiz
    while(temp != None):
        print(temp.pos, temp.tam)
        temp = temp.prox



class Processo:
    g_pid = 0

    def __init__(self, linha):
        self.pid = Processo.g_pid
        self.t0 = int(linha[0])
        self.tf = int(linha[1]) # t0 tf b nome p1 t1 p2 t2... pn tn
        self.b = int(linha[2])
        self.nome = linha[3]
        self.p_acessos = [] # pi
        self.t_acessos = [] # ti
        for j in range(4, len(linha), 2):
            self.p_acessos.append(int(linha[j]))
            self.t_acessos.append(int(linha[j + 1]))
        self.presente = [] # Paginas locais do processo que estao presentes na memoria fisica
        self.presente_pos = [] # Posicao dessas paginas na memoria
        self.i = 0 # indice do prox acesso

        Processo.g_pid = (Processo.g_pid + 1) % 128

    def prox_acesso(self):
        return [self.p_acessos[self.i], self.t_acessos[self.i]]



# Ordena os processos em relacao ao proximo acesso na memoria
class FilaDeAcessos:
    heap = []
    size = 0

    class Item:
        proc = None # Processo

        def __init__ (self, proc):
            self.proc = proc

        def __lt__(self, other):
            s = self.proc.prox_acesso()
            o = other.proc.prox_acesso()

            return s[1] < o[1]

        def __eq__(self, other):
            s = self.proc.prox_acesso()
            o = other.proc.prox_acesso()

            return s[1] == o[1]


    def push(self, proc):
        heapq.heappush (self.heap, self.Item(proc))
        self.size += 1

    # Remove elemento do topo e o devolve
    def pop(self):
        if self.size < 1:
            return None

        proc = heapq.heappop(self.heap) # Devolve o item no topo
        proc = proc.proc
        proc.i += 1
        self.size -= 1
        temp = proc
        if proc.i < len(proc.p_acessos):
            self.push(proc)

        return temp

    # Devolve o elemento do topo sem remover
    def peak(self):
        if self.size < 1:
            return None
        return self.heap[0].proc
