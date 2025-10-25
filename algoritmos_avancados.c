#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Para tolower/toupper, se necessário para padronização

// =====================================================================
// DEFINIÇÕES DA TABELA HASH (Pista -> Suspeito)
// =====================================================================

#define HASH_SIZE 10 // Tamanho da Tabela Hash (simplificado)

// Estrutura para um nó da lista encadeada (para colisões na Hash)
typedef struct HashNode {
    char pista[100];        // Chave: O conteúdo da pista (único)
    char suspeito[50];      // Valor: O suspeito associado a esta pista
    struct HashNode *proximo;
} HashNode;

// A Tabela Hash é um array de ponteiros para HashNode
HashNode* tabelaHash[HASH_SIZE];

/**
 * @brief Função de hash simples que converte uma string em um índice.
 *
 * Soma os valores ASCII dos caracteres e aplica o módulo HASH_SIZE.
 *
 * @param chave A string (pista) a ser hasheada.
 * @return O índice da tabela hash.
 */
int calcularHash(const char* chave) {
    unsigned long int valorHash = 0;
    int i = 0;
    while (chave[i] != '\0') {
        valorHash = valorHash * 31 + chave[i]; // Multiplicação por primo para dispersão
        i++;
    }
    return valorHash % HASH_SIZE;
}

/**
 * @brief Insere uma associação Pista -> Suspeito na Tabela Hash.
 *
 * Trata colisões usando encadeamento (separate chaining). Se a pista já existir,
 * a função pode (nesta versão simplificada) ignorar ou sobrescrever.
 *
 * @param pista A chave (conteúdo da pista).
 * @param suspeito O valor (nome do suspeito) a ser associado à pista.
 */
void inserirNaHash(const char* pista, const char* suspeito) {
    if (strlen(pista) == 0) return; // Não insere pistas vazias
    
    int indice = calcularHash(pista);

    // Cria o novo nó
    HashNode* novoNo = (HashNode*)malloc(sizeof(HashNode));
    if (novoNo == NULL) {
        perror("Erro ao alocar HashNode");
        exit(EXIT_FAILURE);
    }
    strncpy(novoNo->pista, pista, sizeof(novoNo->pista) - 1);
    novoNo->pista[sizeof(novoNo->pista) - 1] = '\0';
    strncpy(novoNo->suspeito, suspeito, sizeof(novoNo->suspeito) - 1);
    novoNo->suspeito[sizeof(novoNo->suspeito) - 1] = '\0';
    novoNo->proximo = NULL;

    // Inserção no início da lista encadeada (para simplificar)
    novoNo->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoNo;
}

/**
 * @brief Busca o suspeito associado a uma pista na Tabela Hash.
 *
 * @param pista A chave (conteúdo da pista) a ser buscada.
 * @return O nome do suspeito, ou uma string vazia ("") se não for encontrado.
 */
const char* encontrarSuspeito(const char* pista) {
    if (strlen(pista) == 0) return "";
    
    int indice = calcularHash(pista);
    HashNode* atual = tabelaHash[indice];

    while (atual != NULL) {
        // strcasecmp é usado para comparação de strings sem case sensitivity
        if (strcasecmp(atual->pista, pista) == 0) {
            return atual->suspeito; // Encontrado!
        }
        atual = atual->proximo;
    }
    
    return ""; // Não encontrado
}

/**
 * @brief Libera a memória alocada para a Tabela Hash.
 */
void liberarHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* atual = tabelaHash[i];
        while (atual != NULL) {
            HashNode* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        tabelaHash[i] = NULL;
    }
}

// =====================================================================
// DEFINIÇÕES DA ÁRVORE DE PISTAS (BST)
// =====================================================================

// Definição da estrutura para uma Pista (nó da Árvore BST)
typedef struct PistaNode {
    char conteudo[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Raiz da BST de Pistas Coletadas (Variável Global)
PistaNode* raizPistas = NULL;


/**
 * @brief Cria e insere dinamicamente uma nova pista na Árvore Binária de Busca (BST).
 *
 * @param raiz O ponteiro para a raiz da subárvore atual.
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
    int comparacao = strcasecmp(conteudo, raiz->conteudo); // Compara ignorando case

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, conteudo);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, conteudo);
    } 
    // Se for igual, é duplicata, não faz nada (mantém apenas uma ocorrência)

    return raiz;
}

/**
 * @brief Exibe todas as pistas coletadas na árvore BST em ordem alfabética (In-order).
 *
 * @param raiz A raiz da subárvore de pistas a ser exibida.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->conteudo);
        exibirPistas(raiz->direita);
    }
}

/**
 * @brief Conta recursivamente quantas pistas na BST apontam para um suspeito.
 * * Utiliza a Tabela Hash global para verificar o suspeito de cada pista.
 *
 * @param raiz A raiz da subárvore de pistas.
 * @param suspeitoAcusado O nome do suspeito a ser contado.
 * @return O número de pistas encontradas.
 */
int contarPistasPorSuspeito(PistaNode* raiz, const char* suspeitoAcusado) {
    if (raiz == NULL) {
        return 0;
    }

    int contagem = 0;
    
    // 1. Visita a subárvore esquerda
    contagem += contarPistasPorSuspeito(raiz->esquerda, suspeitoAcusado);

    // 2. Visita o nó atual (verifica a pista)
    const char* suspeitoDaPista = encontrarSuspeito(raiz->conteudo);
    if (strcasecmp(suspeitoDaPista, suspeitoAcusado) == 0) {
        contagem++;
    }

    // 3. Visita a subárvore direita
    contagem += contarPistasPorSuspeito(raiz->direita, suspeitoAcusado);

    return contagem;
}


/**
 * @brief Libera a memória alocada para a Árvore BST de pistas recursivamente.
 */
void liberarPistas(PistaNode* pista) {
    if (pista != NULL) {
        liberarPistas(pista->esquerda);
        liberarPistas(pista->direita);
        free(pista);
    }
}

// =====================================================================
// DEFINIÇÕES DA ÁRVORE DA MANSÃO (Árvore Binária)
// =====================================================================

// Definição da estrutura para um Cômodo
typedef struct Sala {
    char nome[50];
    char pista[100]; // Pista que pode ser coletada nesta sala
    int coletada;    // Flag para evitar coleta duplicada (0=Não, 1=Sim)
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;


/**
 * @brief Cria dinamicamente uma nova sala com o nome e a pista especificados.
 *
 * @param nome O nome do cômodo.
 * @param pista O conteúdo da pista a ser associada ao cômodo (ou "" se não houver).
 * @return Um ponteiro para a Sala recém-criada.
 */
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        perror("Erro ao alocar memória para a sala");
        exit(EXIT_FAILURE);
    }
    
    strncpy(novaSala->nome, nome, sizeof(novaSala->nome) - 1);
    novaSala->nome[sizeof(novaSala->nome) - 1] = '\0';
    
    strncpy(novaSala->pista, pista, sizeof(novaSala->pista) - 1);
    novaSala->pista[sizeof(novaSala->pista) - 1] = '\0';
    
    novaSala->coletada = (strlen(pista) == 0) ? 1 : 0; // Se não tem pista, marca como "coletada"
    
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * @brief Libera a memória alocada para a árvore da mansão recursivamente.
 */
void liberarMansao(Sala* sala) {
    if (sala != NULL) {
        liberarMansao(sala->esquerda);
        liberarMansao(sala->direita);
        free(sala);
    }
}


// =====================================================================
// LÓGICA DO JOGO E JULGAMENTO
// =====================================================================

/**
 * @brief Permite ao jogador navegar pela mansão e coletar pistas interativamente.
 *
 * @param atual O ponteiro para o cômodo (Sala) onde a exploração deve começar.
 */
void explorarSalas(Sala* atual) {
    char escolha;
    
    while (atual != NULL) {
        printf("\n==========================================\n");
        printf("Você está no(a): **%s**\n", atual->nome);

        // ** Lógica de Coleta de Pista **
        if (atual->coletada == 0) {
            printf("\n- Indício encontrado! Pista: \"%s\"\n", atual->pista);
            // Insere a pista na Árvore BST
            raizPistas = inserirPista(raizPistas, atual->pista); 
            // Marca a pista como coletada para que não seja coletada novamente
            atual->coletada = 1; 
            printf("Pista adicionada ao seu diário de bordo e associada ao suspeito.\n");
        } else {
            printf("Este cômodo já foi vasculhado ou não possui indícios a coletar.\n");
        }
        
        // Define se o caminho é final
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\nFim do caminho! Este cômodo não possui mais saídas. Hora de decidir.\n");
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
        printf("[s] Sair da mansão e iniciar o julgamento: ");
        
        // Leitura da escolha
        if (scanf(" %c", &escolha) != 1) { 
            printf("Entrada inválida. Tente novamente.\n");
            while (getchar() != '\n'); 
            continue;
        }

        // Limpa o restante do buffer de entrada ('\n')
        while (getchar() != '\n');

        // Processa a escolha do jogador
        if (escolha == 's' || escolha == 'S') {
            printf("\nExploração encerrada. Início do Julgamento...\n");
            break;
        } else if ((escolha == 'e' || escolha == 'E') && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if ((escolha == 'd' || escolha == 'D') && atual->direita != NULL) {
            atual = atual->direita;
        } else {
            printf("Opção inválida ou caminho inexistente. Tente novamente.\n");
        }
    }
}

/**
 * @brief Conduz à fase de julgamento final: solicita a acusação e verifica a evidência.
 */
void verificarSuspeitoFinal() {
    char acusacao[50];
    
    printf("\n\n******************************************\n");
    printf("              JULGAMENTO FINAL\n");
    printf("******************************************\n");

    if (raizPistas == NULL) {
        printf("Você não coletou nenhuma pista. Acusação impossível!\n");
        return;
    }

    printf("Pistas coletadas (em ordem alfabética):\n");
    exibirPistas(raizPistas);
    printf("\n");

    printf("Suspeitos conhecidos: Senhorita Scarlet, Coronel Mustard, Professor Plum\n");
    printf("Quem você acusa como culpado(a)? (Digite o nome): ");
    
    // Leitura da acusação (garante que a string não passe do limite)
    if (fgets(acusacao, sizeof(acusacao), stdin) == NULL) {
        printf("Erro de leitura.\n");
        return;
    }
    // Remove o newline (Enter) do final da string lida por fgets
    acusacao[strcspn(acusacao, "\n")] = '\0';

    if (strlen(acusacao) < 2) {
        printf("\nAcusação inválida. O julgamento terminou por falta de seriedade.\n");
        return;
    }

    // 1. Contar as pistas que apontam para o suspeito
    int pistasDeSuporte = contarPistasPorSuspeito(raizPistas, acusacao);

    // 2. Apresentar o veredito
    printf("\nVEREDITO:\n");
    printf("O detetive acusa: **%s**\n", acusacao);
    printf("Pistas de suporte encontradas: %d\n", pistasDeSuporte);

    if (pistasDeSuporte >= 2) {
        printf("\n!!! PARABÉNS !!!\n");
        printf("As evidências (%d pistas) são suficientes para sustentar a acusação de **%s**.\n", pistasDeSuporte, acusacao);
        printf("A justiça foi feita.\n");
    } else {
        printf("\n!!! ACUSAÇÃO FALHA !!!\n");
        printf("Apenas %d pista(s) encontrada(s) apóia(m) sua acusação.\n", pistasDeSuporte);
        printf("São necessárias pelo menos duas pistas. O culpado escapou!\n");
    }
    printf("******************************************\n");
}


// =====================================================================
// FUNÇÃO PRINCIPAL
// =====================================================================

int main() {
    // Inicializa a Tabela Hash
    memset(tabelaHash, 0, sizeof(tabelaHash));

    printf("======================================\n");
    printf("      BEM-VINDO(A) AO DETECTIVE QUEST\n");
    printf("     Explorando a Mansão e Julgamento\n");
    printf("======================================\n");
    
    // ----------------------------------------------------
    // 1. Definição das Associações Pista -> Suspeito (Tabela Hash)
    // ----------------------------------------------------
    inserirNaHash("A chave está com quem mente.", "Coronel Mustard");
    inserirNaHash("O papel rasgado aponta para o Leste.", "Professor Plum");
    inserirNaHash("A luz do abajur tem uma mancha.", "Coronel Mustard");
    inserirNaHash("A pena e tinta estão novas.", "Professor Plum");
    inserirNaHash("Uma garrafa de vinho tinto está faltando.", "Senhorita Scarlet");
    inserirNaHash("As flores raras estão murchas.", "Professor Plum");
    inserirNaHash("Um relógio parou às 3:00.", "Senhorita Scarlet");
    inserirNaHash("Um rastelo de metal está quebrado.", "Coronel Mustard");

    // ----------------------------------------------------
    // 2. Montagem do Mapa da Mansão (Árvore Binária)
    // ----------------------------------------------------
    // O código da pista deve ser exatamente igual ao da Hash!

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

    // ----------------------------------------------------
    // 3. Início da Exploração e Coleta de Pistas
    // ----------------------------------------------------
    explorarSalas(hallEntrada);

    // ----------------------------------------------------
    // 4. Julgamento Final
    // ----------------------------------------------------
    verificarSuspeitoFinal();

    // ----------------------------------------------------
    // 5. Liberação da Memória
    // ----------------------------------------------------
    liberarMansao(hallEntrada);
    liberarPistas(raizPistas);
    liberarHash();
    
    return 0;
}