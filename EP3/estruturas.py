class listaLigada:
    pos = 0 # Posicao da memoria que se refere
    tam = 0 # Por quantas posicoes o espaco se repete
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

# Testes

#raiz = listaLigada()
#raiz.setTam(3)
#raiz.setPos(0)

#for i in range(4):
#    temp = listaLigada()
#    temp.insere(raiz, i*3, i + 1)

#printLista(raiz)
#print("----")
#temp = listaLigada()
#temp = raiz.prox.prox.prox
#temp.removeProx()
#printLista(raiz)