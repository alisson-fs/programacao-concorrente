from time import sleep
from random import randint
from threading import Thread, Lock, Condition


def produtor():
  global buffer
  for i in range(10):
    sleep(randint(0,2))           # Fica um tempo produzindo...
    item = 'item ' + str(i)
    with lock:
      if len(buffer) == tam_buffer:
        print('>>> Buffer cheio. Produtor ira aguardar.')
        lugar_no_buffer.wait()    # Aguarda que haja lugar no buffer.
      buffer.append(item)
      print('Produzido %s (ha %i itens no buffer)' % (item,len(buffer)))
      item_no_buffer.notify()

def consumidor():
  global buffer
  for i in range(10):
    with lock:
      if len(buffer) == 0:
        print('>>> Buffer vazio. Consumidor ira aguardar.')
        item_no_buffer.wait()   # Aguarda que haja um item para consumir.
      item = buffer.pop(0)
      print('Consumido %s (ha %i itens no buffer)' % (item,len(buffer)))
      lugar_no_buffer.notify()
    sleep(randint(0,2))         # Fica um tempo consumindo...

buffer = []
tam_buffer = 5
lock = Lock()
lugar_no_buffer = Condition(lock)
item_no_buffer = Condition(lock)

# Define o numero de threads produtoras e consumidoras.
n_produtoras = 2
n_consumidoras = 2

# Lista para guardar as threads produtoras e consumidoras.
produtoras = []
consumidoras = []

# Cria e inicia as threads produtoras.
for _ in range(n_produtoras):
  produtora = Thread(target=produtor) 
  produtoras.append(produtora)
  produtora.start()

# Cria e inicia as threads consumidoras.
for _ in range(n_consumidoras):
  consumidora = Thread(target=consumidor)
  consumidoras.append(consumidora)
  consumidora.start()

# Aguarda as threads produtoras finalizarem.
for p in range(n_produtoras):
  produtoras[p].join()

# Aguarda as threads consumidoras finalizarem.
for c in range(n_consumidoras):
  consumidoras[c].join()
