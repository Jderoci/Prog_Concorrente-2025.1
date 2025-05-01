## 💻 Código
O programa calcula a quantidade de números primos entre $$1$$ e $$N$$ de forma concorrente utilizando múltiplas threads, distribuindo dinamicamente os números a serem testados entre as threads por meio de uma variável global protegida por mutex. Ao final, imprime o total de primos encontrados e o tempo de execução da versão concorrente.

Para verificação de corretude, foi implementada também uma versão sequencial simples para comparação dos resultados das duas versões: se os totais de primos forem iguais, isso indica que o paralelismo foi feito corretamente.
<br>

## 🔍 Análise dos Resultados
Após executar 5 vezes cada configuração, foram obtidos os seguintes tempos médios de execução para os diferentes números de threads e tamanhos de entrada:

![](TempoConcorrente.png)

Com base nos tempos médios obtidos, foram calculadas a aceleração e a eficiência, utilizando as seguintes métricas:

$$Aceleração (n, t) = \frac{T_p(n, 1)}{T_p(n, t)}$$

$$Eficiência (n, t) = \frac{Aceleração (n, t)}{t}$$

*Onde:*
* $$T_p(n, 1)$$ : Tempo concorrente de $$n$$ com **1 thread**;
* $$n$$ : Dimensão das matrizes;
* $$t$$ : Quantidade de threads usadas na execução.

Assim, foram obtidos os seguintes resultados: 
<br>

![](GráficoAceleração.png)


![](GráficoEficiência.png)


## 💡 Conclusão
A partir dos testes realizados, observei que o uso de múltiplas threads só apresenta vantagem quando $$N$$ é suficientemente grande. Para $$N = 1.000$$, a execução com **1 thread** foi mais eficiente. Isso porque o custo de criação e sincronização das threads, junto com o uso de mutex, acaba superando os ganhos de paralelismo em tarefas pequenas. Isso é visível no gráfico de eficiência: com **2 e 4 threads**, a eficiência cai bastante nesse caso.

Já para $$N = 1.000.000$$, a concorrência mostra vantagem: com **2 threads**, o tempo caiu quase pela metade, mas com **4 threads** o ganho não foi tão alto. Isso pode ter ocorrido devido à limitação do número de núcleos da máquina utilizada ou pelo aumento da disputa entre as threads pelo acesso à variável compartilhada, o que pode gerar atrasos por conta do uso do mutex.


## ⚙️ Configurações da Máquina
A máquina utilizada na realização dos testes possui as seguintes configurações:
- **Sistema Operacional**: Windows 11 Pro – Versão 24H2;
- **Compilação do Sistema**: 26100.3775;
- **Processador**: AMD Ryzen 5 2600, com 6 núcleos e 12 threads, frequência base de 3.40 GHz;
- **Memória RAM**: 16,0 GB;
- **Arquitetura**: Sistema operacional de 64 bits, processador baseado em x64.
