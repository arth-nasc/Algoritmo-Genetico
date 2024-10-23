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
    return sqrt(dx * dx + dy * dy);
}

// Gera a matriz de distâncias entre os pontos
void gerarMatrizDist(struct ponto *pontos, int tamanhoMax, double Dist[tamanhoMax][tamanhoMax]) {
    for (int eixoY = 0; eixoY < tamanhoMax; eixoY++) {
        for (int eixoX = 0; eixoX < tamanhoMax; eixoX++) {
            Dist[eixoY][eixoX] = distEuclideana(pontos[eixoY], pontos[eixoX]);
        }
    }

    // Imprime a matriz de distâncias
    printf("Matriz de Distancias Euclidianas:\n");
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
        soma += Dist[vetor[i]][vetor[i + 1]];
    }
    soma += Dist[vetor[tamanho - 1]][vetor[0]];
    
    return soma;
}

int roleta(double custoTotal, struct pop *populacao){
	double randomico = ((double)rand() / RAND_MAX) * custoTotal;
	printf("Valor randomico escolhido: %.2f\n", randomico);
	
	double somaCusto = 0.00;
	int posicao;
	
	for(int p = 99; p >= 0; p--){
		if(somaCusto < randomico){
			somaCusto += populacao[p].valor;
			posicao = p;
		}
	}
	printf("A posicao escolhida foi: %d\n", posicao);
	
	return 99 - posicao;
}

// Imprime a população ordenada
void imprimirPopulacao(struct pop *populacao, int tamanho) {
	//double custoTotal = 0;
    printf("Vetor Populacao Ordenado:\n");
    for (int i = 0; i < tamanho; i++) {
        printf("Indiv %d: ", i);
        for (int j = 0; j < 10; j++) {
            printf("%d ", populacao[i].indiv[j]);
        }
        printf("Valor acumulado: %.2f\n", populacao[i].valor);
        //custoTotal += populacao[i].valor;
    }
    //printf("Custo total: %.2f\n", custoTotal);
    //roleta(custoTotal, populacao);
    //return custoTotal;
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
    
    //qsort(populacao, tamanhoVetorPop, sizeof(struct pop), compararPopulacao);
    
    //double custoTotal = imprimirPopulacao(populacao, tamanhoVetorPop);
    
    //int posInvertida = roleta(custoTotal, populacao);
    
    //printf("A posicao invertida e: %d", posInvertida);
    
    return populacao;
}

void Cruzamento(int *A, int *B, int *novoIndiv, double Dist[10][10]) {
    int tamanhoIndiv = 10;  // Tamanho do indivíduo
    int pontoCruzamento = rand() % (tamanhoIndiv - 1);  // Escolhe um ponto de cruzamento aleatório

    // Realiza o cruzamento
    for (int i = 0; i <= pontoCruzamento; i++) {
        novoIndiv[i] = indivA[i];  // Copia genes de A até o ponto de cruzamento
    }
    for (int i = pontoCruzamento + 1; i < tamanhoIndiv; i++) {
        novoIndiv[i] = indivB[i];  // Copia genes de B após o ponto de cruzamento
    }

} //cruzamento em x nao funciona no caso, usar outro cruzamento
// usar pathrelinking. 
// fixa um pai e vai alterando um-a-um o pai de baixo até ficar no indice igual ao pai de cima, atraves de swaps.
// toda vez coloca no temp se for maior salve abaixo.
// vai-se comparando o res. do vetor pai e com vetor filho.
// fazer 40 vezes, dentro de um laço de 100 vezes.
// deve-se reordenar a pop.

void Mutacao(struct pop *populacao, int tamanhoVetorPop, double Dist[10][10]) {
    // Define a taxa de mutação (ex: 10%)
    double taxaMutacao = 0.1;

    for (int i = 0; i < tamanhoVetorPop; i++) {
        for (int j = 0; j < 10; j++) {
            // Decide se deve mutar esse bit
            if ((rand() / (double)RAND_MAX) < taxaMutacao) {
                // Mutação bitwise: inverte o bit
                populacao[i].indiv[j] ^= 1; // Usando XOR para inverter o bit
            }
        }
    }

    // Atualiza o valor da nova solução se necessário
    for (int i = 0; i < tamanhoVetorPop; i++) {
        populacao[i].valor = gerarSomaValor(populacao[i].indiv, 10, Dist);
    }

    printf("Mutação bitwise aplicada na população.\n");
}

//Receber como parâmetro o vetor população ordenado
//Percorrer o vetor população somando o custo
double custoTotal(struct pop *populacao, int tamanho){
	double custoTotal = 0;
    for (int i = 0; i < tamanho; i++) {
        custoTotal += populacao[i].valor;
    }
    printf("Custo total: %.2f\n", custoTotal);
    return custoTotal;
}

void novasGeracoes(struct pop *populacao, double Dist[10][10], int tamanhoVetorPop){
	struct pop populacao2[40];  // Nova população para armazenar os 40 últimos elementos
	int A;
	int B;
	
	for(int i = 1; i < 100; i++){
		//Calcula o custo total
		double custoTotalValor=custoTotal(populacao, tamanhoVetorPop);
		
		for(int j = 1; j < 40; j++){
			A = roleta(custoTotalValor, populacao);  // Seleciona A por roleta
            B = roleta(custoTotalValor, populacao);  // Seleciona B por roleta

            // Realiza o cruzamento entre A e B e armazena o resultado em populacao2[j]
            Cruzamento(populacao[A].indiv, populacao[B].indiv, populacao2[j].indiv, Dist);
            populacao2[j].valor = gerarSomaValor(populacao2[j].indiv, 10, Dist); // Atualiza o valor
        }
        for (int k = 0; k < 40; k++) {
            populacao[tamanhoVetorPop - 40 + k] = populacao2[k];
        }
        qsort(populacao, tamanhoVetorPop, sizeof(struct pop), compararPopulacao);

		}
	
	qsort(populacao, 100, sizeof(struct pop), compararPopulacao);
    Mutacao(populacao, tamanhoVetorPop, Dist);
	}


int main() {
	srand(time(NULL));
    int tamanhoMax = 10;
    int tamanhoVetorPop = 100;
    double Dist[tamanhoMax][tamanhoMax];
    
    struct ponto *pontos = gerarVetorPontos(tamanhoMax);
    
    gerarMatrizDist(pontos, tamanhoMax, Dist);
    
    //População de 100 indiv criada
    struct pop *populacao = gerarVetorPop(Dist, tamanhoVetorPop);
        //População de 100 indiv ordenada do menor para o maior custo
    qsort(populacao, 100, sizeof(struct pop), compararPopulacao);
    
    //Imprime a população ordenada
    imprimirPopulacao(populacao, tamanhoVetorPop);
    
    novasGeracoes(populacao,Dist,tamanhoMax);

    free(pontos);
}
