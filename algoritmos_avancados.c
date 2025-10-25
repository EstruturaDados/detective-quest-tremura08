#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição da estrutura para uma Pista (nó da Árvore BST)
typedef struct PistaNode {
    char conteudo[100];         // Conteúdo textual da pista
    struct PistaNode *esquerda; // Ponteiro para a subárvore esquerda (menores)
    struct PistaNode *direita;  // Ponteiro para a subárvore direita (maiores)
} PistaNode;

// Definição da estrutura para um Cômodo (nó da Árvore Binária da Mansão)
typedef struct Sala {
    char nome[50];              // Nome do cômodo
    char pista[100];            // Pista associada a este cômodo (pode ser vazio)
    struct Sala *esquerda;      // Ponteiro para o cômodo à esquerda
    struct Sala *direita;       // Ponteiro para o cômodo à direita
} Sala;

// Variável global para armazenar a raiz da Árvore BST de Pistas Coletadas
PistaNode* raizPistas = NULL;


/**
 * @brief Cria dinamicamente uma nova sala com o nome e a pista especificados.
 *
 * Aloca memória para uma nova estrutura Sala, copia o nome e a pista fornecida
 * e inicializa os ponteiros 'esquerda' e 'direita' como NULL.
 * Se a pista for uma string vazia (""), a sala é criada sem uma pista para coletar.
 *
 * @param nome O nome do cômodo a ser criado.
 * @param pista O conteúdo da pista a ser associada ao cômodo (ou "" se não houver).
 * @return Um ponteiro para a Sala recém-criada.
 */
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        perror("Erro ao alocar memória para a sala");
        exit(EXIT_FAILURE);
    }
    // Copia o nome
    strncpy(novaSala->nome, nome, sizeof(novaSala->nome) - 1);
    novaSala->nome[sizeof(novaSala->nome) - 1] = '\0';
    
    // Copia a pista
    strncpy(novaSala->pista, pista, sizeof(novaSala->pista) - 1);
    novaSala->pista[sizeof(novaSala->pista) - 1] = '\0';
    
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * @brief Cria e insere dinamicamente uma nova pista na Árvore Binária de Busca (BST) de pistas.
 *
 * A inserção é feita de forma recursiva, mantendo a propriedade de ordenação alfabética:
 * Pistas menores (alfabeticamente) vão para a esquerda, maiores vão para a direita.
 *
 * @param raiz O ponteiro para a raiz da subárvore atual (pode ser NULL se vazia).
 * @param conteudo O conteúdo textual da pista a ser inserida.
 * @return O ponteiro para a nova raiz da subárvore após a inserção.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* conteudo) {
    // 1. Caso base: Se a raiz for NULL, cria e retorna o novo nó.
    if (raiz == NULL) {
        PistaNode* novaPista = (PistaNode*)malloc(sizeof(PistaNode));
        if (novaPista == NULL) {
            perror("Erro ao alocar memória para a pista");
            exit(EXIT_FAILURE);
        }
        strncpy(novaPista->conteudo, conteudo, sizeof(novaPista->conteudo) - 1);
        novaPista->conteudo[sizeof(novaPista->conteudo) - 1] = '\0';
        novaPista->esquerda = NULL;
        novaPista->direita = NULL;
        return novaPista;
    }

    // 2. Caso recursivo: Compara o novo conteúdo com o conteúdo da raiz atual
    int comparacao = strcasecmp(conteudo, raiz->conteudo); // Compara ignorando maiúsculas/minúsculas

    if (comparacao < 0) {
        // Pista menor (alfabeticamente): vai para a subárvore esquerda
        raiz->esquerda = inserirPista(raiz->esquerda, conteudo);
    } else if (comparacao > 0) {
        // Pista maior (alfabeticamente): vai para a subárvore direita
        raiz->direita = inserirPista(raiz->direita, conteudo);
    } else {
        // Caso de pista duplicada: Ignora a inserção e informa o jogador.
        printf(" -> Pista já coletada: \"%s\".\n", conteudo);
    }

    // Retorna o ponteiro para a raiz (que pode ter sido alterado na criação do nó)
    return raiz;
}

/**
 * @brief Exibe todas as pistas coletadas na árvore BST em ordem alfabética (percurso In-order).
 *
 * A travessia In-order (Esquerda -> Raiz -> Direita) garante que os elementos
 * de uma BST sejam impressos em ordem crescente (ou alfabética, neste caso).
 *
 * @param raiz A raiz da subárvore de pistas a ser exibida.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        // 1. Visita a subárvore esquerda
        exibirPistas(raiz->esquerda);
        
        // 2. Visita o nó atual (imprime a pista)
        printf("- %s\n", raiz->conteudo);
        
        // 3. Visita a subárvore direita
        exibirPistas(raiz->direita);
    }
}

/**
 * @brief Permite ao jogador navegar pela mansão e coletar pistas interativamente.
 *
 * Guia o jogador a tomar decisões de 'esquerda', 'direita' ou 'sair'.
 * Se a sala atual tiver uma pista não vazia, ela é coletada (inserida na BST).
 *
 * @param atual O ponteiro para o cômodo (Sala) onde a exploração deve começar.
 * @return O ponteiro para a raiz da BST de pistas após a exploração.
 */
PistaNode* explorarSalasComPistas(Sala* atual) {
    char escolha;
    
    // Inicia com a BST de pistas coletadas (global)
    PistaNode* pistasColetadas = raizPistas; 

    // Continua a exploração enquanto houver um cômodo atual
    while (atual != NULL) {
        printf("\n==========================================\n");
        printf("Você está no(a): **%s**\n", atual->nome);

        // ** Lógica de Coleta de Pista **
        if (strlen(atual->pista) > 0) {
            printf("\n- Indício encontrado! Pista: \"%s\"\n", atual->pista);
            // Insere a pista na Árvore BST
            pistasColetadas = inserirPista(pistasColetadas, atual->pista); 
            // Marca a pista como coletada para que não seja coletada novamente
            atual->pista[0] = '\0'; 
            printf("Pista adicionada ao seu diário de bordo.\n");
        } else {
            printf("Este cômodo já foi vasculhado ou não possui indícios.\n");
        }
        
        // Define se o caminho é final
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\nFim do caminho! Este cômodo não possui mais saídas.\n");
            break; 
        }

        // Exibe as opções de navegação
        printf("\nPara onde você deseja ir?\n");
        if (atual->esquerda != NULL) {
            printf("[e] Esquerda -> %s\n", atual->esquerda->nome);
        }
        if (atual->direita != NULL) {
            printf("[d] Direita -> %s\n", atual->direita->nome);
        }
        printf("[s] Sair da mansão: ");
        
        // Leitura da escolha
        if (scanf(" %c", &escolha) != 1) { 
            printf("Entrada inválida. Tente novamente.\n");
            // Limpa o buffer de entrada em caso de erro
            while (getchar() != '\n'); 
            continue;
        }

        // Limpa o restante do buffer de entrada ('\n')
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
            printf("Opção inválida ou caminho inexistente. Tente novamente.\n");
        }
    }
    
    return pistasColetadas; // Retorna a raiz atualizada da BST
}

/**
 * @brief Libera a memória alocada para a árvore da mansão recursivamente.
 *
 * @param sala A raiz da subárvore a ser liberada.
 */
void liberarMansao(Sala* sala) {
    if (sala != NULL) {
        liberarMansao(sala->esquerda);
        liberarMansao(sala->direita);
        free(sala);
    }
}

/**
 * @brief Libera a memória alocada para a Árvore BST de pistas recursivamente.
 *
 * @param pista A raiz da subárvore de pistas a ser liberada.
 */
void liberarPistas(PistaNode* pista) {
    if (pista != NULL) {
        liberarPistas(pista->esquerda);
        liberarPistas(pista->direita);
        free(pista);
    }
}


/**
 * @brief Função principal: monta o mapa, inicia a exploração e exibe o relatório final.
 *
 * @return 0 se o programa for executado com sucesso.
 */
int main() {
    printf("======================================\n");
    printf("  BEM-VINDO(A) AO DETECTIVE QUEST\n");
    printf("  Explorando a Mansão Misteriosa\n");
    printf("======================================\n");

    // 1. Montagem do Mapa da Mansão (Árvore Binária Fixa com Pistas)
    // Usamos o criarSala(nome, pista)
    
    // Nível 0: Raiz (Hall de entrada)
    Sala* hallEntrada = criarSala("Hall de Entrada", "A chave está com quem mente.");

    // Nível 1
    Sala* salaEstar = criarSala("Sala de Estar", "O papel rasgado aponta para o Leste.");
    Sala* cozinha = criarSala("Cozinha", ""); // Sem pista
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita = cozinha;

    // Nível 2 - Ramo Esquerdo (Sala de Estar)
    Sala* biblioteca = criarSala("Biblioteca", "A luz do abajur tem uma mancha.");
    Sala* escritorio = criarSala("Escritório", "A pena e tinta estão novas.");
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = escritorio;

    // Nível 2 - Ramo Direito (Cozinha)
    Sala* despensa = criarSala("Despensa", "Uma garrafa de vinho tinto está faltando."); 
    Sala* jardim = criarSala("Jardim", "As flores raras estão murchas.");
    cozinha->esquerda = despensa;
    cozinha->direita = jardim;

    // Nível 3 - Ramo Esquerdo (Biblioteca, Escritório)
    biblioteca->esquerda = criarSala("Quarto Principal", ""); // Sem pista
    escritorio->direita = criarSala("Quarto de Hóspedes", "Um relógio parou às 3:00.");
    
    // Nível 3 - Ramo Direito (Jardim)
    jardim->direita = criarSala("Cabana do Jardineiro", "Um rastelo de metal está quebrado."); 

    printf("\nA exploração começa no Hall de Entrada. Boa sorte, Detetive!\n");

    // 2. Início da Exploração e Coleta de Pistas
    // O retorno da função atualiza a raiz global de pistas
    raizPistas = explorarSalasComPistas(hallEntrada);

    // 3. Relatório Final de Pistas Coletadas
    printf("\n\n******************************************\n");
    printf("       RELATÓRIO FINAL DE INDÍCIOS\n");
    printf("******************************************\n");
    
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada durante a exploração.\n");
    } else {
        printf("Pistas coletadas (em ordem alfabética):\n");
        exibirPistas(raizPistas);
    }
    printf("******************************************\n");

    // 4. Liberação da Memória
    liberarMansao(hallEntrada);
    liberarPistas(raizPistas); // Libera a BST de pistas
    
    return 0;
}