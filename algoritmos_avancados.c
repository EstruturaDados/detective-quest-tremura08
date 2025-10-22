#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definindo a estrutura para um cômodo (nó da árvore binária)
typedef struct Sala {
    char nome[50];        // Nome do cômodo
    struct Sala *esquerda; // Ponteiro para o cômodo à esquerda
    struct Sala *direita;  // Ponteiro para o cômodo à direita
} Sala;

/**
 * @brief Cria dinamicamente uma nova sala com o nome especificado.
 * * Aloca memória para uma nova estrutura Sala, copia o nome fornecido 
 * e inicializa os ponteiros 'esquerda' e 'direita' como NULL.
 * * @param nome O nome do cômodo a ser criado.
 * @return Um ponteiro para a Sala recém-criada.
 */
Sala* criarSala(const char* nome) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        perror("Erro ao alocar memória para a sala");
        exit(EXIT_FAILURE);
    }
    // Copia o nome para a string de nome da struct, garantindo que não exceda o tamanho
    strncpy(novaSala->nome, nome, sizeof(novaSala->nome) - 1);
    novaSala->nome[sizeof(novaSala->nome) - 1] = '\0'; // Garantir terminação da string
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * @brief Permite ao jogador navegar pela mansão (árvore binária) interativamente.
 * * Começa no cômodo fornecido e guia o jogador a tomar decisões de 'esquerda' ou 'direita'.
 * A exploração continua até que o jogador chegue a um cômodo sem mais caminhos 
 * (nó-folha) ou escolha sair ('s').
 * * @param atual O ponteiro para o cômodo (Sala) onde a exploração deve começar.
 */
void explorarSalas(Sala* atual) {
    char escolha;
    
    // Continua a exploração enquanto houver um cômodo atual
    while (atual != NULL) {
        printf("\nVocê está no(a): **%s**\n", atual->nome);

        // Verifica se é um cômodo-folha (sem caminhos)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Fim do caminho! Este cômodo não possui mais saídas. A exploração terminou.\n");
            break; // Sai do loop, pois o jogador chegou a uma folha
        }

        // Exibe as opções de navegação
        printf("Para onde você deseja ir?\n");
        if (atual->esquerda != NULL) {
            printf("[e] Esquerda\n");
        }
        if (atual->direita != NULL) {
            printf("[d] Direita\n");
        }
        printf("[s] Sair da mansão: ");
        
        // Limpa o buffer de entrada antes de ler a nova escolha
        if (scanf(" %c", &escolha) != 1) { 
            printf("Entrada inválida. Tente novamente.\n");
            // Se o scanf falhar, tentamos limpar o lixo que pode ter ficado.
            while (getchar() != '\n'); 
            continue;
        }

        // LIMPA o restante do buffer (o '\n' gerado pelo Enter do usuário)
        // Isso impede que o '\n' seja lido na próxima iteração do loop.
        while (getchar() != '\n');

        // Processa a escolha do jogador
        if (escolha == 's' || escolha == 'S') {
            printf("\nExploração encerrada. Saindo da mansão...\n");
            break;
        } else if ((escolha == 'e' || escolha == 'E') && atual->esquerda != NULL) {
            atual = atual->esquerda; // Move para o cômodo da esquerda
        } else if ((escolha == 'd' || escolha == 'D') && atual->direita != NULL) {
            atual = atual->direita;  // Move para o cômodo da direita
        } else {
            // Caso o jogador escolha um caminho que não existe a partir da sala atual
            printf("Opção inválida ou caminho inexistente. Tente novamente.\n");
        }
    }
}

/**
 * @brief Libera a memória alocada para a árvore (mansão) recursivamente.
 * * É importante liberar a memória para evitar vazamento de memória.
 * * @param sala A raiz da subárvore a ser liberada.
 */
void liberarMansao(Sala* sala) {
    if (sala != NULL) {
        liberarMansao(sala->esquerda);
        liberarMansao(sala->direita);
        // printf("Liberando: %s\n", sala->nome); // Opcional: para rastrear a liberação
        free(sala);
    }
}

/**
 * @brief Função principal: monta o mapa da mansão e inicia a exploração.
 * * Monta a estrutura da árvore binária fixada e chama a função de exploração.
 * No final, libera toda a memória alocada.
 * * @return 0 se o programa for executado com sucesso.
 */
int main() {
    printf("======================================\n");
    printf("       BEM-VINDO(A) AO DETECTIVE QUEST\n");
    printf("      Explorando a Mansão Misteriosa\n");
    printf("======================================\n");

    // 1. Montagem do Mapa da Mansão (Árvore Binária Fixa)
    // Nível 0: Raiz (Hall de entrada)
    Sala* hallEntrada = criarSala("Hall de Entrada");

    // Nível 1
    Sala* salaEstar = criarSala("Sala de Estar");
    Sala* cozinha   = criarSala("Cozinha");
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita = cozinha;

    // Nível 2 - Ramo Esquerdo (Sala de Estar)
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* escritorio = criarSala("Escritório");
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = escritorio;

    // Nível 2 - Ramo Direito (Cozinha)
    Sala* despensa = criarSala("Despensa"); // Nó-folha
    Sala* jardim   = criarSala("Jardim");
    cozinha->esquerda = despensa;
    cozinha->direita = jardim;

    // Nível 3 - Ramo Esquerdo (Biblioteca, Escritório)
    biblioteca->esquerda = criarSala("Quarto Principal"); // Nó-folha
    // biblioteca->direita = NULL (Folha)
    escritorio->direita = criarSala("Quarto de Hóspedes"); // Nó-folha
    // escritorio->esquerda = NULL (Folha)

    // Nível 3 - Ramo Direito (Jardim)
    jardim->direita = criarSala("Cabana do Jardineiro"); // Nó-folha
    // jardim->esquerda = NULL (Folha)
    
    // Exemplo de Estrutura:
    /*
                  (Hall de Entrada)
                 /                 \
        (Sala de Estar)        (Cozinha)
          /       \            /      \
    (Biblioteca) (Escritório) (Despensa) (Jardim)
       /             \                    \
  (Quarto P)      (Quarto H)       (Cabana)
    */

    // 2. Início da Exploração
    explorarSalas(hallEntrada);

    // 3. Liberação da Memória
    liberarMansao(hallEntrada);
    
    return 0;
}