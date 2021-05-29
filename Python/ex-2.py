from time import sleep
from random import randint
from threading import Thread, Semaphore


def produtor():
  global buffer
  for i in range(10):
    # Decrementa o semáforo que controla o número de espaços.
    sem_espaco.acquire()
    sleep(randint(0,2))           # fica um tempo produzindo...
    item = 'item ' + str(i)
    # Verifica se há lugar no buffer.
    buffer.append(item)
    print('Produzido %s (ha %i itens no buffer)' % (item,len(buffer)))
    # Incrementa semáforo que controla o número de produtos.
    sem_produtos.release()

def consumidor():
  global buffer
  for i in range(10):
    # Decrementa o semáforo que controla o número de produtos.
    sem_produtos.acquire()
    # Aguarda que haja um item para consumir.
    item = buffer.pop(0)
    print('Consumido %s (ha %i itens no buffer)' % (item,len(buffer)))
    sleep(randint(0,2))         # Fica um tempo consumindo...
    # Incrementa o semáforo que controla o número de espaços.
    sem_espaco.release()

buffer = []
tam_buffer = 3

# Cria semáforos.
sem_espaco = Semaphore(tam_buffer)
sem_produtos = Semaphore(0)

# Cria a thread produtora e a consumidora.
produtor = Thread(target=produtor) 
consumidor = Thread(target=consumidor)

# Inicia a thread produtora e a consumidora.
produtor.start()
consumidor.start()

# Aguarda a thread produtora e a consumidora finalizarem.
produtor.join()
consumidor.join()
