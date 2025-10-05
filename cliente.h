#include <stdio.h>
#include <string.h> // Para usar a função strcmp e strcspn
#include <stdlib.h> // Para usar exit() se necessário

// --- Constantes e Tipos Globais ---

// [MELHORIA] Usar uma constante para o nome do arquivo facilita a manutenção.
// Se o nome do arquivo precisar mudar, só alteramos em um lugar.
#define NOME_ARQUIVO_DADOS "clientes.dat"
#define MAX_CLIENTES 100 // Define um tamanho máximo para o nosso array de clientes

// [MELHORIA] Usar uma enumeração (enum) torna o código mais legível.
// Em vez de usar if (opcao == 1), usamos if (opcao == PESQUISA_POR_CODIGO).
typedef enum {
    PESQUISA_POR_CODIGO = 1,
    PESQUISA_POR_NOME = 2
} TipoPesquisa;

// Estrutura de dados para representar um cliente.
typedef struct {
    int codigo;
    char nome[100];
    char endereco[200];
    char telefone[20];
} Cliente;


// --- Funções Auxiliares ---

/**
 * @brief Lê uma string do teclado de forma segura, prevenindo buffer overflow.
 * @param buffer Onde a string será armazenada.
 * @param tamanho O tamanho máximo do buffer.
 */
void ler_string_segura(char *buffer, int tamanho) {
    // [SEGURANÇA] fgets é usado em vez de scanf. Ele lê no máximo 'tamanho - 1' caracteres,
    // garantindo que não haverá estouro de buffer.
    fgets(buffer, tamanho, stdin);

    // O fgets armazena o caractere de nova linha ('\n') quando o usuário aperta Enter.
    // A linha abaixo encontra o '\n' e o substitui por '\0' (fim de string).
    buffer[strcspn(buffer, "\n")] = 0;
}

/**
 * @brief Imprime os dados de um cliente de forma formatada.
 * @param cliente O cliente cujos dados serão impressos.
 */
void imprimir_cliente(Cliente cliente) {
    // [DRY] Centralizamos a lógica de impressão aqui. Se o formato de exibição
    // precisar mudar, alteramos apenas nesta função.
    printf("\n--- Cliente Encontrado ---\n");
    printf("Codigo:   %d\n", cliente.codigo);
    printf("Nome:     %s\n", cliente.nome);
    printf("Endereco: %s\n", cliente.endereco);
    printf("Telefone: %s\n", cliente.telefone);
    printf("-------------------------\n");
}


// --- Lógica de Negócio (Core) ---

/**
 * @brief Busca um cliente pelo código no array.
 * @param clientes O array de clientes.
 * @param num_clientes O número atual de clientes.
 * @param codigo O código a ser pesquisado.
 * @return O índice do cliente no array se encontrado, ou -1 caso contrário.
 */
int buscar_cliente_por_codigo(Cliente clientes[], int num_clientes, int codigo) {
    // [LEITURA] Usar um loop 'for' é mais convencional para percorrer arrays.
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].codigo == codigo) {
            return i; // Retorna o índice onde o cliente foi encontrado
        }
    }
    return -1; // Sinaliza que nenhum cliente com esse código foi encontrado
}


// --- Funções de Interface com o Usuário (UI) ---

/**
 * @brief Gerencia o fluxo de cadastro de um novo cliente.
 * @param clientes O array de clientes.
 * @param num_clientes Ponteiro para o número atual de clientes.
 */
void cadastrar_cliente(Cliente clientes[], int *num_clientes) {
    if (*num_clientes >= MAX_CLIENTES) {
        printf("Erro: Limite maximo de clientes atingido.\n");
        return;
    }
    
    Cliente novo_cliente;
    int codigo_digitado;

    printf("\n--- Cadastro de Novo Cliente ---\n");
    printf("Digite o codigo do cliente: ");
    scanf("%d", &codigo_digitado);
    // Limpa o buffer do teclado para a próxima leitura de string
    getchar(); 

    // [ARQUITETURA] A UI chama a lógica de negócio (busca) para validar os dados.
    if (buscar_cliente_por_codigo(clientes, *num_clientes, codigo_digitado) != -1) {
        printf("Erro: Ja existe um cliente com esse codigo.\n");
        return;
    }
    novo_cliente.codigo = codigo_digitado;

    printf("Digite o nome do cliente: ");
    ler_string_segura(novo_cliente.nome, sizeof(novo_cliente.nome));

    printf("Digite o endereco do cliente: ");
    ler_string_segura(novo_cliente.endereco, sizeof(novo_cliente.endereco));

    printf("Digite o telefone do cliente: ");
    ler_string_segura(novo_cliente.telefone, sizeof(novo_cliente.telefone));

    // Adiciona o novo cliente ao final do array
    clientes[*num_clientes] = novo_cliente;
    (*num_clientes)++; // Incrementa o contador de clientes
    printf("\nCliente cadastrado com sucesso!\n");
}

/**
 * @brief Gerencia o fluxo de pesquisa de um cliente.
 * @param clientes O array de clientes.
 * @param num_clientes O número atual de clientes.
 */
void pesquisar_cliente(Cliente clientes[], int num_clientes) {
    int opcao;
    printf("\n--- Pesquisa de Cliente ---\n");
    printf("Pesquisar por: %d. Codigo ou %d. Nome: ", PESQUISA_POR_CODIGO, PESQUISA_POR_NOME);
    scanf("%d", &opcao);
    getchar(); // Limpa o buffer do teclado

    if (opcao == PESQUISA_POR_CODIGO) {
        int codigo_busca;
        printf("Digite o codigo do cliente: ");
        scanf("%d", &codigo_busca);
        getchar();

        int indice = buscar_cliente_por_codigo(clientes, num_clientes, codigo_busca);
        if (indice != -1) {
            imprimir_cliente(clientes[indice]);
        } else {
            printf("Cliente com codigo %d nao encontrado.\n", codigo_busca);
        }
    } else if (opcao == PESQUISA_POR_NOME) {
        char nome_busca[100];
        printf("Digite o nome do cliente: ");
        ler_string_segura(nome_busca, sizeof(nome_busca));

        int encontrado = 0;
        for (int i = 0; i < num_clientes; i++) {
            if (strcmp(clientes[i].nome, nome_busca) == 0) {
                imprimir_cliente(clientes[i]);
                encontrado = 1;
                // Não usamos 'return' para o caso de haver múltiplos clientes com o mesmo nome
            }
        }
        if (!encontrado) {
            printf("Nenhum cliente com o nome '%s' foi encontrado.\n", nome_busca);
        }
    } else {
        printf("Opcao invalida.\n");
    }
}


// --- Funções de Persistência (Leitura/Gravação em Arquivo) ---

/**
 * @brief Salva o array de clientes em um arquivo binário.
 * @param clientes O array de clientes.
 * @param num_clientes O número atual de clientes.
 */
void salvar_clientes(Cliente clientes[], int num_clientes) {
    // Abre o arquivo em modo de escrita binária ("wb")
    FILE *file = fopen(NOME_ARQUIVO_DADOS, "wb");

    // [ROBUSTEZ] Sempre verifique se o arquivo foi aberto com sucesso.
    if (file == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    // Grava primeiro o número de clientes, para sabermos quantos ler depois
    fwrite(&num_clientes, sizeof(int), 1, file);
    // Grava o array inteiro de clientes
    fwrite(clientes, sizeof(Cliente), num_clientes, file);

    fclose(file); // Fecha o arquivo para garantir que os dados sejam salvos
    printf("Dados dos clientes salvos com sucesso.\n");
}

/**
 * @brief Carrega os clientes de um arquivo binário para o array.
 * @param clientes O array onde os clientes serão carregados.
 * @param num_clientes Ponteiro para o número de clientes, que será atualizado.
 */
void carregar_clientes(Cliente clientes[], int *num_clientes) {
    // Abre o arquivo em modo de leitura binária ("rb")
    FILE *file = fopen(NOME_ARQUIVO_DADOS, "rb");

    // Se o arquivo não existir (pode ser a primeira vez que o programa roda),
    // a função fopen retornará NULL.
    if (file) {
        // Lê o número de clientes que foi salvo no início do arquivo
        fread(num_clientes, sizeof(int), 1, file);
        // Lê os dados dos clientes para o array
        fread(clientes, sizeof(Cliente), *num_clientes, file);
        fclose(file);
        printf("%d cliente(s) carregado(s) do arquivo.\n", *num_clientes);
    } else {
        // Se o arquivo não existe, simplesmente iniciamos com 0 clientes.
        *num_clientes = 0;
        printf("Arquivo de dados nao encontrado. Iniciando com 0 clientes.\n");
    }
}