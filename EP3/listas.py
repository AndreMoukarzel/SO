class listaLigada:
    livre = True
    pos = 0 # Posicao da memoria que se refere
    rep = 0 # Por quantas posicoes ela se repete (ocupado/livre)
    prox = None # Proximo elemento da lista ligada

    # Metodos de declaracao
    def setProx(self, prox):
        self.prox = prox

    def setPos(self, val):
        self.pos = val

    def setRep(self, val):
        self.rep = val

    def setLivre(self, val):
        self.livre = val

    # Metodos de manipulacao

    # Insere uma nova celula como a proxima de 'celula'
    def insere(self, celula, pos, repete, livre):
        new = listaLigada()
        new.setPos(pos)
        new.setRep(repete)
        new.setLivre(livre)

        new.setProx(celula.prox)
        celula.setProx(new)

    # Remove a celula que vem depois de 'celula'
    def removeProx(self):
        temp = listaLigada()
        temp = self.prox
        self.setProx(temp.prox)
        temp = None

def printLista(raiz):
    temp = listaLigada()
    temp = raiz
    while(temp != None):
        print(temp.livre, temp.pos, temp.rep)
        temp = temp.prox

# Testes

# raiz = listaLigada()
# raiz.setRep(3)
# raiz.setLivre(False)
# raiz.setPos(0)
#
# for i in range(4):
#     temp = listaLigada()
#     temp.insere(raiz, i*3, i + 1, True)
#
# printLista(raiz)
# print("----")
# temp = listaLigada()
# temp = raiz.prox.prox
# temp.removeProx()
# printLista(raiz)