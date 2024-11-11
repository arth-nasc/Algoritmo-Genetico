#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

struct ponto {
    int id;
    double px;
    double py;
};

struct pop {
    double valor;
    int indiv[10];
};

// Gera um número aleatório entre 0 e 360
double gerarDouble() {
    return ((double)rand() / RAND_MAX) * 360.0;
}

// Gera um vetor de pontos com valores aleatórios
struct ponto *gerarVetorPontos(int tamanhoMax) {
    struct ponto *pontos = malloc(tamanhoMax * sizeof(struct ponto));
    
    for (int i = 0; i < tamanhoMax; i++) {
        pontos[i].id = i;
        pontos[i].px = gerarDouble();
        pontos[i].py = gerarDouble();
    }
    
    return pontos;
}

// Calcula a distância euclidiana entre dois pontos
double distEuclideana(struct ponto p1, struct ponto p2) {
    double dx = p1.px - p2.px;
    double dy = p1.py - p2.py;
    double distancia = sqrt(dx * dx + dy * dy);
    
    // Verifique se a distância é válida
    if (isnan(distancia) || isinf(distancia)) {
        printf("Erro: distância inválida entre os pontos %d e %d\n", p1.id, p2.id);
    }
    
    return distancia;
}

// Gera a matriz de distâncias entre os pontos
void gerarMatrizDist(struct ponto *pontos, int tamanhoMax, double Dist[tamanhoMax][tamanhoMax]) {
    for (int eixoY = 0; eixoY < tamanhoMax; eixoY++) {
        for (int eixoX = 0; eixoX < tamanhoMax; eixoX++) {
            Dist[eixoY][eixoX] = distEuclideana(pontos[eixoY], pontos[eixoX]);
        }
    }
    //DEBUG: VER SE TEM VALORES INVALIDOS
	printf("DEBUG: Matriz de Distâncias Euclidianas:\n");
    for (int i = 0; i < tamanhoMax; i++) {
        for (int j = 0; j < tamanhoMax; j++) {
            printf("%.2f\t", Dist[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Gera números únicos para o vetor de indivíduos
void gerarNumerosUnicos(int vetor[], int tamanho) {
    bool numeros_usados[10] = { false };

    for (int i = 0; i < tamanho; i++) {
        int index;
        do {
            index = rand() % 10;
        } while (numeros_usados[index]);

        vetor[i] = index;
        numeros_usados[index] = true;
    }
}

// Calcula o valor acumulado de uma rota
double gerarSomaValor(int vetor[], int tamanho, double Dist[][10]) {
    double soma = 0;

    for (int i = 0; i < tamanho - 1; i++) {
        // Verifica se os índices estão dentro do intervalo válido
        if (vetor[i] < 0 || vetor[i] >= tamanho || vetor[i + 1] < 0 || vetor[i + 1] >= tamanho) {
            printf("Erro: índice fora dos limites (%d, %d)\n", vetor[i], vetor[i + 1]);
            return -1;  // Retorna um valor inválido para indicar o erro
        }
        soma += Dist[vetor[i]][vetor[i + 1]];
    }
    soma += Dist[vetor[tamanho - 1]][vetor[0]];  // Distância de volta ao primeiro ponto

    // Verifica se a soma resultante é válida
    if (isnan(soma) || isinf(soma)) {
        printf("Erro: soma de distâncias inválida\n");
    }

    return soma;
}

// Função de comparação para qsort
int compararPopulacao(const void *a, const void *b) {
    const struct pop *pop1 = (const struct pop *)a;
    const struct pop *pop2 = (const struct pop *)b;
    
    if (pop1->valor < pop2->valor) return -1;
    else if (pop1->valor > pop2->valor) return 1;
    else return 0;
}

// Gera o vetor população
struct pop *gerarVetorPop(double Dist[][10], int tamanhoVetorPop) {
    struct pop *populacao = malloc(tamanhoVetorPop * sizeof(struct pop));
    
    for (int i = 0; i < tamanhoVetorPop; i++) {
        gerarNumerosUnicos(populacao[i].indiv, 10);
        populacao[i].valor = gerarSomaValor(populacao[i].indiv, 10, Dist);
    }
    
    return populacao;
}

// Função de roleta para seleção de indivíduos
int roleta(double custoTotal, struct pop *populacao, int tamanhoPop) {
    double randomico = ((double)rand() / RAND_MAX) * custoTotal;
    double somaCusto = 0.0;
    for (int p = 0; p < tamanhoPop; p++) {
        somaCusto += populacao[p].valor;
        if (somaCusto >= randomico) {
            return p;
        }
    }
    return tamanhoPop - 1; // Caso a roleta não funcione como esperado
}

// Função de cruzamento usando Path Relinking
void Cruzamento(int *A, int *B, int *novoIndiv, double Dist[10][10]) {
    // Path Relinking: começa com um indivíduo e tenta "recriar" o outro
    int tamanhoIndiv = 10;
    
    // Inicializa o novo indivíduo com valores inválidos
    for (int i = 0; i < tamanhoIndiv; i++) {
        novoIndiv[i] = -1;
    }
    
    // Fixa um pai (A) e tenta alterar o filho (B) até corresponder a A
    int swaps = 0;
    for (int i = 0; i < tamanhoIndiv; i++) {
        if (novoIndiv[i] == -1) {
            novoIndiv[i] = A[i];
            for (int j = 0; j < tamanhoIndiv; j++) {
                if (novoIndiv[i] == B[j]) {
                    // Troca
                    int temp = novoIndiv[i];
                    novoIndiv[i] = novoIndiv[j];
                    novoIndiv[j] = temp;
                    swaps++;
                    break;
                }
            }
        }
    }
    
    if (swaps == 0) {
        for (int i = 0; i < tamanhoIndiv; i++) {
            novoIndiv[i] = B[i];
        }
    }
}

// Função de mutação do tipo SCRAMBLE
void Mutacao(struct pop *populacao, int tamanhoVetorPop, double Dist[10][10]) {
    int tamanhoIndividuo = 10; // Tamanho fixo do vetor de indivíduos
    for (int i = 0; i < tamanhoVetorPop; i++) {
        // Decide se a mutação será aplicada ao indivíduo com base em uma probabilidade
        if (rand() % 2 == 0) { // Aqui você pode adicionar uma probabilidade, caso queira
            // Escolhe dois índices aleatórios para definir o intervalo da mutação
            int inicio = rand() % tamanhoIndividuo;
            int fim = rand() % tamanhoIndividuo;

            // Garante que o início seja menor que o fim
            if (inicio > fim) {
                int temp = inicio;
                inicio = fim;
                fim = temp;
            }

            // Embaralha a subsequência entre 'inicio' e 'fim'
            for (int j = inicio; j < fim; j++) {
                int k = rand() % (fim - inicio) + inicio;
                // Troca as cidades
                int temp = populacao[i].indiv[j];
                populacao[i].indiv[j] = populacao[i].indiv[k];
                populacao[i].indiv[k] = temp;
            }

            // Recalcula o valor (custo) do indivíduo após a mutação
            populacao[i].valor = gerarSomaValor(populacao[i].indiv, tamanhoIndividuo, Dist);
        }
    }
}

// Função de cálculo do custo total da população
double custoTotal(struct pop *populacao, int tamanho) {
    double custoTotal = 0;
    for (int i = 0; i < tamanho; i++) {
		if (isnan(populacao[i].valor) || isinf(populacao[i].valor)) {
            printf("Erro: custo inválido para o indivíduo %d\n", i);
        }
        custoTotal += populacao[i].valor;
    }
    printf("Custo total: %.2f\n", custoTotal);
    return custoTotal;
}

void novasGeracoes(struct pop *populacao, double Dist[10][10], int tamanhoVetorPop) {
    struct pop populacao2[40]; // Nova população para armazenar os 40 últimos elementos
    int A, B;
    
    for (int i = 0; i < 100; i++) {
        // Calcula o custo total
        double custoTotalValor = custoTotal(populacao, tamanhoVetorPop);
        
        for (int j = 0; j < 40; j++) {
            // Seleciona A e B por roleta
            A = roleta(custoTotalValor, populacao, tamanhoVetorPop);
            B = roleta(custoTotalValor, populacao, tamanhoVetorPop);
            
            // Realiza o cruzamento entre A e B e armazena o resultado em populacao2[j]
            Cruzamento(populacao[A].indiv, populacao[B].indiv, populacao2[j].indiv, Dist);
            populacao2[j].valor = gerarSomaValor(populacao2[j].indiv, 10, Dist); // Atualiza o valor
        }

        // Atualiza a população com os melhores indivíduos
        for (int k = 0; k < 40; k++) {
            populacao[tamanhoVetorPop - 40 + k] = populacao2[k];
        }
        qsort(populacao, tamanhoVetorPop, sizeof(struct pop), compararPopulacao);
        
        // Aplica a mutação
        Mutacao(populacao, tamanhoVetorPop, Dist);
    }
}

int main() {
    srand(time(NULL));
    int tamanhoMax = 10;
    int tamanhoVetorPop = 100;
    double Dist[tamanhoMax][tamanhoMax];
    
    struct ponto *pontos = gerarVetorPontos(tamanhoMax);
    
    gerarMatrizDist(pontos, tamanhoMax, Dist);
    
    struct pop *populacao = gerarVetorPop(Dist, tamanhoVetorPop);
    
    qsort(populacao, tamanhoVetorPop, sizeof(struct pop), compararPopulacao);
    
    // Imprime a população ordenada
    for (int i = 0; i < tamanhoVetorPop; i++) {
        printf("Indiv %d: ", i);
        for (int j = 0; j < 10; j++) {
            printf("%d ", populacao[i].indiv[j]);
        }
        printf("Valor acumulado: %.2f\n", populacao[i].valor);
    }
    
    novasGeracoes(populacao, Dist, tamanhoVetorPop);

    free(pontos);
    free(populacao);
    return 0;
}
