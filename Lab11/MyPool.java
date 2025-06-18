/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;

/*
  Classe que implementa um pool de threads
  Um número fixo de threads é criado e reutilizado para executar tarefas submetidas na forma de objetos que implementam Runnable.
 As tarefas são armazenadas em uma fila e distribuídas entre as threads.
 */
class FilaTarefas {
    private final int nThreads; // Número de threads no pool
    private final MyPoolThreads[] threads; // Vetor de threads do pool
    private final LinkedList<Runnable> queue; // Fila de tarefas a serem executadas
    private boolean shutdown; // Flag para encerrar o pool

    /*
     Construtor do pool de threads. 
     Inicializa a fila de tarefas, cria e inicia todas as threads do pool.
     O parâmetro nThreads é o número de threads que o pool terá
     */
    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        this.queue = new LinkedList<Runnable>();
        this.threads = new MyPoolThreads[nThreads];

        // Cria e inicia cada thread
        for (int i = 0; i < nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        }
    }

    /*
     Submete uma nova tarefa para execução.
     A tarefa é adicionada ao final da fila e notifica uma thread livre. 
     O parâmetro R é a tarefa a ser executada
     */
    public void execute(Runnable r) {
        // Synchonized utilizado para garantir exclusão mútua
        synchronized (queue) {
            if (this.shutdown) return; // Ignora se já estiver em shutdown
            queue.addLast(r); // Adiciona tarefa ao fim da fila
            queue.notify(); // Acorda uma thread que estiver esperando
        }
    }

    /*
     Encerra o pool de threads.
     Sinaliza que não serão aceitas novas tarefas, acorda todas as threads e espera que elas terminem a execução.
     */
    public void shutdown() {
        synchronized (queue) {
            this.shutdown = true;
            queue.notifyAll(); // Acorda todas as threads para que possam encerrar
        }

        // Aguarda cada thread terminar
        for (int i = 0; i < nThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }
    }

    /*
     Classe interna que representa uma thread do pool.
     Cada thread executa um loop contínuo, onde:
     - espera por tarefas na fila
     - executa a tarefa
     - termina quando a fila estiver vazia e o pool for encerrado
     */
    private class MyPoolThreads extends Thread {
        public void run() {
            Runnable r;

            while (true) {
                synchronized (queue) {
                    // Espera até que haja tarefa ou o pool seja encerrado
                    while (queue.isEmpty() && !shutdown) {
                        try {
                            queue.wait();
                        } catch (InterruptedException ignored) {}
                    }

                    // Se estiver vazio e em shutdown, a thread pode sair
                    if (queue.isEmpty()) return;

                    // Pega a próxima tarefa da fila
                    r = queue.removeFirst();
                }

                // Executa a tarefa fora do bloco synchronized
                try {
                    r.run();
                } catch (RuntimeException e) {}
            }
        }
    }
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}

class Primo implements Runnable {
    private final long numero;

    public Primo(long numero) {
        this.numero = numero;
    }

    public void run() {
        if (ehPrimo(numero)) {
            System.out.println("O número " + numero + " é primo.");
        } else {
            System.out.println("O número " + numero + " não é primo.");
        }
    }

    // Função auxiliar para verificar se é primo
    private boolean ehPrimo(long n) {
        if (n <= 1) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        for (int i = 3; i <= Math.sqrt(n); i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
}

//Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 10;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 

      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 0; i < 25; i++) {
        final String m = "Hello da tarefa " + i;
        Runnable hello = new Hello(m);
        pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");
   }
}
