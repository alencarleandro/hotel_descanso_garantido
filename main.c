#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Necessário para a função system("clear") ou "cls"
#include <time.h>   // Usado indiretamente pela struct Estadia

// --- Simulação dos Arquivos .h ---

// [BOA PRÁTICA] Em um projeto real, as linhas abaixo estariam em arquivos separados
// (ex: cliente.h, quarto.h, etc.). As incluímos aqui para que o código seja
// completo e compilável em um único arquivo.

// Constantes para os tamanhos dos arrays
#define MAX_ENTIDADES 100

// Definições de Estruturas
typedef struct { int codigo; char nome[100]; char endereco[200]; char telefone[20]; } Cliente;
typedef struct { int codigo; char nome[100]; char telefone[20]; char cargo[50]; float salario; } Funcionario;
typedef struct { int numero_quarto; int quantidade_hospedes; float valor_diaria; char status[20]; } Quarto;
typedef struct { int codigo_estadia; struct tm data_entrada; struct tm data_saida; int quantidade_diarias; int codigo_cliente; int numero_quarto; } Estadia;

// Protótipos das funções que serão chamadas (declaradas em outros arquivos)
void carregar_clientes(Cliente[], int*);
void salvar_clientes(Cliente[], int);
void cadastrar_cliente(Cliente[], int*);
void pesquisar_cliente(Cliente[], int);

void carregar_funcionarios(Funcionario[], int*);
void salvar_funcionarios(Funcionario[], int);
void cadastrar_funcionario(Funcionario[], int*);
void pesquisar_funcionario(Funcionario[], int);

void carregar_quartos(Quarto[], int*);
void salvar_quartos(Quarto[], int);

void carregar_estadias(Estadia[], int*);
void salvar_estadias(Estadia[], int);
void cadastrar_estadia(Estadia[], int*, Cliente[], int, Quarto[], int);
void dar_baixa_estadia(Estadia[], int*, Quarto[], int);
void listar_estadias_cliente(Estadia[], int, int);


// --- Funções de Interface do Menu ---

/**
 * @brief Limpa a tela do console (funciona em Windows e Linux/Mac).
 */
void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief Exibe o menu principal de opções para o usuário.
 */
void exibir_menu() {
    // [ESTRUTURA] Mover a exibição do menu para uma função separada
    // deixa a função main() mais limpa e focada em sua lógica principal.
    printf("\n========= HOTELARIA TURING =========\n");
    printf("1.  Cadastrar Cliente\n");
    printf("2.  Cadastrar Funcionario\n");
    printf("3.  Cadastrar Estadia (Check-in)\n");
    printf("4.  Dar Baixa em Estadia (Check-out)\n");
    printf("5.  Pesquisar Cliente\n");
    printf("6.  Pesquisar Funcionario\n");
    printf("7.  Listar Estadias de um Cliente\n");
    printf("8.  Salvar e Sair\n");
    printf("==================================\n");
    printf("Escolha uma opcao: ");
}

/**
 * @brief Salva todos os dados da aplicação nos respectivos arquivos.
 */
void salvar_todos_os_dados(
    Cliente clientes[], int num_clientes,
    Funcionario funcionarios[], int num_funcionarios,
    Estadia estadias[], int num_estadias,
    Quarto quartos[], int num_quartos)
{
    // [EFICIÊNCIA] Esta função centraliza a lógica de salvar. Em vez de salvar
    // a cada pequena alteração, salvamos tudo de uma vez no final,
    // o que é muito mais rápido e eficiente.
    printf("\nSalvando todos os dados...\n");
    salvar_clientes(clientes, num_clientes);
    salvar_funcionarios(funcionarios, num_funcionarios);
    salvar_estadias(estadias, num_estadias);
    salvar_quartos(quartos, num_quartos);
    printf("Dados salvos com sucesso!\n");
}


// --- Função Principal ---

// [BOA PRÁTICA] A assinatura padrão da função main em C é 'int main(void)'.
int main(void) {
    // Declaração dos arrays que armazenarão os dados em memória
    Cliente clientes[MAX_ENTIDADES];
    Funcionario funcionarios[MAX_ENTIDADES];
    Estadia estadias[MAX_ENTIDADES];
    Quarto quartos[MAX_ENTIDADES];

    // Contadores para cada tipo de entidade
    int num_clientes = 0, num_funcionarios = 0, num_estadias = 0, num_quartos = 0;

    // Carrega os dados dos arquivos para a memória no início da execução
    carregar_clientes(clientes, &num_clientes);
    carregar_funcionarios(funcionarios, &num_funcionarios);
    carregar_estadias(estadias, &num_estadias);
    carregar_quartos(quartos, &num_quartos);

    // Lógica para popular os quartos na primeira execução do programa
    if (num_quartos == 0) {
        printf("Nenhum quarto encontrado. Criando quartos padrao...\n");
        quartos[0] = (Quarto){101, 2, 150.0, "desocupado"};
        quartos[1] = (Quarto){102, 3, 200.0, "desocupado"};
        quartos[2] = (Quarto){201, 4, 250.0, "desocupado"};
        quartos[3] = (Quarto){202, 1, 120.0, "desocupado"};
        num_quartos = 4;
        salvar_quartos(quartos, num_quartos); // Salva os quartos iniciais
    }

    int opcao;
    do {
        exibir_menu(); // Chama a função que mostra as opções

        // [ROBUSTEZ] Verifica se o scanf conseguiu ler um número. Se o usuário
        // digitar uma letra, o programa não entrará em loop infinito.
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida! Por favor, digite um numero.\n");
            // Limpa o buffer de entrada para evitar loop infinito
            while (getchar() != '\n');
            opcao = 0; // Atribui um valor inválido para continuar o loop
            continue;  // Pula para a próxima iteração do loop
        }
        
        limpar_tela(); // Limpa a tela após ler a opção para uma interface mais limpa

        switch (opcao) {
            case 1:
                cadastrar_cliente(clientes, &num_clientes);
                break;
            case 2:
                cadastrar_funcionario(funcionarios, &num_funcionarios);
                break;
            case 3:
                cadastrar_estadia(estadias, &num_estadias, clientes, num_clientes, quartos, num_quartos);
                break;
            case 4:
                dar_baixa_estadia(estadias, &num_estadias, quartos, num_quartos);
                break;
            case 5:
                pesquisar_cliente(clientes, num_clientes);
                break;
            case 6:
                pesquisar_funcionario(funcionarios, num_funcionarios);
                break;
            case 7: { // Usamos chaves para poder declarar uma variável dentro do case
                int codigo_cliente;
                printf("Digite o codigo do cliente para listar as estadias: ");
                scanf("%d", &codigo_cliente);
                listar_estadias_cliente(estadias, num_estadias, codigo_cliente);
                break;
            }
            case 8:
                // A opção 8 agora apenas sinaliza a saída do loop.
                // Os dados serão salvos após o término do loop.
                break;
            default:
                printf("Opcao invalida! Pressione Enter para tentar novamente.\n");
                // Pausa para o usuário ler a mensagem de erro.
                while (getchar() != '\n'); // Limpa buffer
                getchar(); // Espera o Enter
                break;
        }

        // Pausa no final de cada operação, exceto ao sair
        if (opcao != 8) {
            printf("\nPressione Enter para voltar ao menu...");
            while (getchar() != '\n'); // Limpa qualquer resquício no buffer
            getchar(); // Espera o usuário pressionar Enter
            limpar_tela();
        }

    } while (opcao != 8);

    // [EFICIÊNCIA] Salva todos os dados de uma só vez antes de encerrar.
    salvar_todos_os_dados(clientes, num_clientes, funcionarios, num_funcionarios, estadias, num_estadias, quartos, num_quartos);

    printf("\nPrograma finalizado. Ate logo!\n");

    return 0; // Retorna 0 para indicar que o programa terminou com sucesso.
}