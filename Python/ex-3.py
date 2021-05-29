from multiprocessing import Process, Queue


def origem(fila1):
  for i in range(10):
    msg = 'Teste ' + str(i)
    # Envia mensagem para a ponte pela fila 1.
    fila1.put(msg)
    print('Mensagem enviada:', msg)

def ponte(fila1, fila2):
  for i in range(10):
    # Recebe mensagem na fila 1.
    msg = fila1.get()
    # Reenvia mensagem na fila 2.
    fila2.put(msg)
    print('Mensagem encaminhada:', msg)

def destino(fila2):
  for i in range(10):
    # Recebe mensagem na fila 2.]
    msg = fila2.get()
    print('Mensagem recebida:', msg)

if __name__ == '__main__':
  fila1 = Queue()
  fila2 = Queue()
  o = Process(target=origem, args=(fila1,))
  p = Process(target=ponte, args=(fila1, fila2,))
  d = Process(target=destino, args=(fila2,))
  
  # Executa os processos.
  o.start()
  p.start()
  d.start()
  
  # Aguarda conclusão.
  o.join()
  p.join()
  d.join()
  
  # Libera as filas.
  fila1.close()
  fila2.close()
  