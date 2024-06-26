//Atualizações nos atalhos do teclado … Em quinta-feira, 1 de agosto de 2024, os atalhos de teclado do Drive serão atualizados para oferecer a navegação por letras iniciais.Saiba mais
// Trabalho de Sistemas Operacionais TT304 | Prof. André Leon S. Gradvohl
// Autores:
// Danilo Barcellos Corrêa | RA: 193740
// Samuel Rodrigues da Silva | RA: 249564

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

typedef struct Matriz
{
    int dimensao;
    FILE *arquivo;
    int *matriz;
    char *nomeArquivo;
    int soma;
} Matriz;

typedef struct MatrizDupla
{
    // Para Reduzir Matriz
    int vetorInicial;
    int vetorFinal;
    int somaThread; // Soma de cada Thread independente para redução da matriz E

    // Para Multiplicar Matrizes
    int linhaInicial;
    int linhaFinal;
    int colunaInicial;
    int colunaFinal;

    Matriz *matrizPrimeira;
    Matriz *matrizSegunda;
    Matriz *matrizResultado;
} MatrizDupla;

Matriz *AlocarNovaMatriz(int dimensao, char *nomeArquivo);

MatrizDupla *AlocarMatrizDupla(Matriz *matriz1, Matriz *matriz2, char *nomeArquivo);

void *lerMatrizDeArquivo(void *args);

void *lerMatrizThread(void *args);

void *somarMatrizes(void *args);

void *multiplicarMatrizes(void *args);

void *calcularReducaoDeMatriz(void *args);

void zerarMatriz(Matriz *matriz);

void verificarParametrosLinhaDeComando(int Num_de_parametros, char *argv[]);

void *gravarMatrizEmArquivo(void *args);

void *chamarMultiplicacaoDoisThread(void *args);

void *chamarMultiplicacaoQuatroThread(void *args);

void *chamarReducaoQuatroThreads(void *args);

void *chamarReducaoDoisThreads(void *args);

void exibirResultados(int reducao, double tempoSoma, double tempoMultiplicacao, double tempoReducao, double tempoTotal);

void *somarMatrizesDoisThread(void *args);

void *somarMatrizesThread(void *args);

void *somarMatrizesQuatroThread(void *args);


int main(int argc, char *argv[])
{
    // Declaração de variáveis
    clock_t inicio, inicioTotal, fim;
    double tempoSoma, tempoMultiplicacao, tempoReducao, tempoTotal;
    int numeroThreads, dimensaoMatriz;

    verificarParametrosLinhaDeComando(argc, argv);

    numeroThreads = atoi(argv[1]);  // numero de threads de processamento
    dimensaoMatriz = atoi(argv[2]); // tamanho das matrizes quadradas

    // Inicio da contagem de Tempo Total
    inicioTotal = clock();

    // Passo 1: Ler matrizes A e B com 2 Threads
    Matriz *mA = AlocarNovaMatriz(dimensaoMatriz, argv[3]);
    Matriz *mB = AlocarNovaMatriz(dimensaoMatriz, argv[4]);
    MatrizDupla *mAmB = AlocarMatrizDupla(mA, mB, argv[6]);
    lerMatrizThread(mAmB);

    // Passo 2: Somar matrizes A e B com T threads (1, 2 e 4)
    if (numeroThreads == 1)
    {
        inicio = clock();
        somarMatrizes(mAmB);
        fim = clock() - inicio;
        tempoSoma = ((double)fim) / CLOCKS_PER_SEC;
    }
    else
    {
        if (numeroThreads == 2)
        {
            inicio = clock();
            somarMatrizesDoisThread(mAmB);
            fim = clock() - inicio;
            tempoSoma = ((double)fim) / CLOCKS_PER_SEC;
        }
        else //automaticamente 4 threads
        {
            inicio = clock();
            somarMatrizesQuatroThread(mAmB);
            fim = clock() - inicio;
            tempoSoma = ((double)fim) / CLOCKS_PER_SEC;
        }
    }

    // Passo 3: Gravar matriz D com 1 thread
    gravarMatrizEmArquivo(mAmB->matrizResultado);

    // Passo 4: ler matriz C com 1 thread
    Matriz *mC = AlocarNovaMatriz(dimensaoMatriz, argv[5]);
    lerMatrizDeArquivo(mC); // <----------------------------

    // Passo 5: Multiplicar matrizes D e C (DxC = E) com T threads (1, 2 e 4)
    MatrizDupla *mDmC = AlocarMatrizDupla(mAmB->matrizResultado, mC, argv[7]);

    int dimensao = mDmC->matrizPrimeira->dimensao;

    // Limpa a matriz resultado
    for (int i = 0; i < dimensao * dimensao; i++)
    {
        mDmC->matrizResultado->matriz[i] = 0;
    }

    if (numeroThreads == 1)
    {
        mDmC->linhaInicial = 0;
        mDmC->linhaFinal = mDmC->matrizPrimeira->dimensao;
        mDmC->colunaInicial = 0;
        mDmC->colunaFinal = mDmC->matrizPrimeira->dimensao;

        inicio = clock();
        multiplicarMatrizes(mDmC);
        fim = clock() - inicio;
        tempoMultiplicacao = ((double)fim) / CLOCKS_PER_SEC;
    }
    else if (numeroThreads == 2)
    {
        inicio = clock();
        chamarMultiplicacaoDoisThread(mDmC);
        fim = clock() - inicio;
        tempoMultiplicacao = ((double)fim) / CLOCKS_PER_SEC;
    }
    else //automaticamente 4 threads
    {
        inicio = clock();
        chamarMultiplicacaoQuatroThread(mDmC);
        fim = clock() - inicio;
        tempoMultiplicacao = ((double)fim) / CLOCKS_PER_SEC;
    }

    // Passo 6: Gravar matriz E com 1 thread
    gravarMatrizEmArquivo(mDmC->matrizResultado);

    // Passo 7: Reduzir a matriz E com T threads (1, 2 e 4)
    if (numeroThreads == 1)
    {   mDmC->vetorInicial = 0;
        mDmC->vetorFinal = mDmC->matrizPrimeira->dimensao * mDmC->matrizPrimeira->dimensao;

        inicio = clock();
        calcularReducaoDeMatriz(mDmC);
        mDmC->matrizResultado->soma = mDmC->somaThread;
        fim = clock() - inicio;
        tempoReducao = ((double)fim) / CLOCKS_PER_SEC;
    }
    else if (numeroThreads == 2)
    {   
        mDmC->vetorInicial = 0;
        mDmC->vetorFinal = mDmC->matrizPrimeira->dimensao * mDmC->matrizPrimeira->dimensao;

        inicio = clock();
        chamarReducaoDoisThreads(mDmC);
        fim = clock() - inicio;
        tempoReducao = ((double)fim) / CLOCKS_PER_SEC;
    }
    else //automaticamente 4 threads
    {
        inicio = clock();
        chamarReducaoQuatroThreads(mDmC);
        fim = clock() - inicio;
        tempoReducao = ((double)fim) / CLOCKS_PER_SEC;
    }

    // Medição do tempo total
    fim = clock() - inicioTotal;
    tempoTotal = ((double)fim) / CLOCKS_PER_SEC;

    exibirResultados(mDmC->matrizResultado->soma, tempoSoma, tempoMultiplicacao, tempoReducao, tempoTotal);

    free(mA);
    free(mB);
    free(mAmB);
    free(mC);
    free(mDmC);

    return 0;
}

Matriz *AlocarNovaMatriz(int dimensao, char *nomeArquivo)
{
    Matriz *matriz = (Matriz *)malloc(sizeof(Matriz));
    matriz->dimensao = dimensao;
    matriz->arquivo = NULL;
    matriz->nomeArquivo = nomeArquivo;
    matriz->matriz = (int *)malloc(matriz->dimensao * matriz->dimensao * sizeof(int));
    matriz->soma = 0;

    if (matriz->matriz == NULL)
    {
        printf("Erro ao alocar memória para Matriz!!!\n");
        exit(EXIT_FAILURE);
    }

    zerarMatriz(matriz);

    return matriz;
}

void zerarMatriz(Matriz *matriz)
{
    for (int i = 0; i < matriz->dimensao; ++i)
    {
        matriz->matriz[i] = 0;
    }
}

MatrizDupla *AlocarMatrizDupla(Matriz *matriz1, Matriz *matriz2, char *nomeArquivo)
{
    MatrizDupla *mDupla = (MatrizDupla *)malloc(sizeof(MatrizDupla));
    mDupla->matrizPrimeira = matriz1;
    mDupla->matrizSegunda = matriz2;
    mDupla->matrizResultado = AlocarNovaMatriz(mDupla->matrizPrimeira->dimensao, nomeArquivo);
    return mDupla;
}

void *lerMatrizDeArquivo(void *args)
{
    Matriz *matriz = (Matriz *)args;

    matriz->arquivo = fopen(matriz->nomeArquivo, "r+");

    if (matriz->arquivo == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", matriz->nomeArquivo);
        exit(EXIT_FAILURE);
    }

    int i = 0;
    int numeroMatriz;
    while (fscanf(matriz->arquivo, "%d ", &numeroMatriz) != EOF)
    {
        matriz->matriz[i] = numeroMatriz;
        ++i;
        if (i > matriz->dimensao * matriz->dimensao)
        {
            printf("Erro: O tamanho da matriz do arquivo \"%s\" não corresponde ao parâmetro passado!!!\n", matriz->nomeArquivo);
            exit(EXIT_FAILURE);
        }
    }

    if (i != matriz->dimensao * matriz->dimensao)
    {
        printf("Erro: O tamanho da matriz do arquivo \"%s\" não corresponde ao parâmetro passado!!!\n", matriz->nomeArquivo);
        exit(EXIT_FAILURE);
    }

    fclose(matriz->arquivo);

    return NULL;
}

void *somarMatrizes(void *args)
{
    MatrizDupla *matrizDupla = (MatrizDupla *)args;

    int dimensaoPrimeira = matrizDupla->matrizPrimeira->dimensao;
    for (int i = 0; i < dimensaoPrimeira * dimensaoPrimeira; i++)
    {
        matrizDupla->matrizResultado->matriz[i] = matrizDupla->matrizPrimeira->matriz[i] + matrizDupla->matrizSegunda->matriz[i];
    }

    return NULL;
}

void *somarMatrizesThread(void *args)
{
    MatrizDupla *matrizDupla = (MatrizDupla *)args;

    int dimensaoPrimeira = matrizDupla->matrizPrimeira->dimensao;
    for (int i = matrizDupla->linhaInicial; i < matrizDupla->linhaFinal; i++)
    {
        matrizDupla->matrizResultado->matriz[i] = matrizDupla->matrizPrimeira->matriz[i] + matrizDupla->matrizSegunda->matriz[i];
    }

    return NULL;
}

void *somarMatrizesDoisThread(void *args)
{
    MatrizDupla *matrizDupla = (MatrizDupla *)args;
    int dimensao = matrizDupla->matrizPrimeira->dimensao;
    
    pthread_t idThreadA;
    pthread_t idThreadB;
    
    idThreadA = pthread_self();
    idThreadB = pthread_self();
    
    //Matrizes temporárias
    MatrizDupla matrizA, matrizB;
    matrizA.matrizPrimeira = matrizDupla->matrizPrimeira;
    matrizA.matrizSegunda = matrizDupla->matrizSegunda;
    matrizA.matrizResultado = matrizDupla->matrizResultado;
    
    matrizB.matrizPrimeira = matrizDupla->matrizPrimeira;
    matrizB.matrizSegunda = matrizDupla->matrizSegunda;
    matrizB.matrizResultado = matrizDupla->matrizResultado;
    
    //Configuração da matriz temporária A
    matrizA.linhaInicial = 0;
    matrizA.linhaFinal = (dimensao * dimensao) / 2;
    
    //Configuração da matriz temporária B
    matrizB.linhaInicial = (dimensao * dimensao) / 2;
    matrizB.linhaFinal = dimensao * dimensao;
    
    //Criação das Threads
    pthread_create(&idThreadA, NULL, somarMatrizes, (void *)&matrizA);
    pthread_create(&idThreadB, NULL, somarMatrizes, (void *)&matrizB);
    
    pthread_join(idThreadA, NULL);
    pthread_join(idThreadB, NULL);
    
    //linha inicial e a final com significado de posições iniciais e finais
    
    return NULL;
}

void *somarMatrizesQuatroThread(void *args)
{
    MatrizDupla *matrizDupla = (MatrizDupla *)args;
    int dimensao = matrizDupla->matrizPrimeira->dimensao;
    
    pthread_t idThreadA;
    pthread_t idThreadB;
    pthread_t idThreadC;
    pthread_t idThreadD;
    
    idThreadA = pthread_self();
    idThreadB = pthread_self();
    idThreadC = pthread_self();
    idThreadD = pthread_self();
    
    //Matrizes temporárias
    MatrizDupla matrizA, matrizB, matrizC, matrizD;
    matrizA.matrizPrimeira = matrizDupla->matrizPrimeira;
    matrizA.matrizSegunda = matrizDupla->matrizSegunda;
    matrizA.matrizResultado = matrizDupla->matrizResultado;
    
    matrizB.matrizPrimeira = matrizDupla->matrizPrimeira;
    matrizB.matrizSegunda = matrizDupla->matrizSegunda;
    matrizB.matrizResultado = matrizDupla->matrizResultado;
    
    matrizC.matrizPrimeira = matrizDupla->matrizPrimeira;
    matrizC.matrizSegunda = matrizDupla->matrizSegunda;
    matrizC.matrizResultado = matrizDupla->matrizResultado;
    
    matrizD.matrizPrimeira = matrizDupla->matrizPrimeira;
    matrizD.matrizSegunda = matrizDupla->matrizSegunda;
    matrizD.matrizResultado = matrizDupla->matrizResultado;
    
    //Configuração da matriz temporária A
    matrizA.linhaInicial = 0;
    matrizA.linhaFinal = (dimensao * dimensao) / 4;
    
    //Configuração da matriz temporária B
    matrizB.linhaInicial = (dimensao * dimensao) / 4;
    matrizB.linhaFinal = (dimensao * dimensao) / 2;
    
    //Configuração da matriz temporária C
    matrizC.linhaInicial = (dimensao * dimensao) / 2;
    matrizC.linhaFinal = (3 * (dimensao * dimensao)) / 4;
    
    //Configuração da matriz temporária D
    matrizD.linhaInicial = (3 * (dimensao * dimensao)) / 4;
    matrizD.linhaFinal = dimensao * dimensao;
    
    //Criação das Threads
    pthread_create(&idThreadA, NULL, somarMatrizes, (void *)&matrizA);
    pthread_create(&idThreadB, NULL, somarMatrizes, (void *)&matrizB);
    pthread_create(&idThreadC, NULL, somarMatrizes, (void *)&matrizC);
    pthread_create(&idThreadD, NULL, somarMatrizes, (void *)&matrizD);
    
    pthread_join(idThreadA, NULL);
    pthread_join(idThreadB, NULL);
    pthread_join(idThreadC, NULL);
    pthread_join(idThreadD, NULL);
    
    //linha inicial e a final com significado de posições iniciais e finais
    
    return NULL;
}

void *multiplicarMatrizes(void *args)
{
    MatrizDupla *mE = (MatrizDupla *)args;
    int dimensao = mE->matrizPrimeira->dimensao;

    for (int i = mE->linhaInicial; i < mE->linhaFinal; i++)
    {
        for (int j = mE->colunaInicial; j < mE->colunaFinal; j++)
        {
            for (int k = 0; k < dimensao; k++)
            {
                mE->matrizResultado->matriz[i * dimensao + j] += (mE->matrizPrimeira->matriz[i * dimensao + k] * mE->matrizSegunda->matriz[k * dimensao + j]);
            }
        }
    }

    return NULL;
}

void *calcularReducaoDeMatriz(void *args) // Modifiquei
{
    MatrizDupla *mE = (MatrizDupla *)args;
    mE->somaThread = 0;
    for (int i = mE->vetorInicial; i < mE->vetorFinal; ++i)
    {
        mE->somaThread += mE->matrizResultado->matriz[i];
    }

    return NULL;
}

void *gravarMatrizEmArquivo(void *args)
{
    Matriz *matriz = (Matriz *)args;
    matriz->arquivo = fopen(matriz->nomeArquivo, "w");

    if (matriz->arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s para escrita!!!", matriz->nomeArquivo);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < matriz->dimensao; i++)
    {
        for (int j = 0; j < matriz->dimensao; j++)
        {
            fprintf(matriz->arquivo, "%d ", matriz->matriz[(i * matriz->dimensao) + j]);
        }
        fprintf(matriz->arquivo, "\n");
    }

    fclose(matriz->arquivo);

    return NULL;
}

void verificarParametrosLinhaDeComando(int Num_de_parametros, char *argv[])
{
    if (Num_de_parametros != 8)
    {
        int Posicao_ultimo_caractere_argumeto_1 = strlen(argv[0]);
        argv[0][1] = '/';
        argv[0][Posicao_ultimo_caractere_argumeto_1 - 4] = '\0'; // Exclusao da
        argv[0][Posicao_ultimo_caractere_argumeto_1 - 3] = '\0'; // string ".exe"
        argv[0][Posicao_ultimo_caractere_argumeto_1 - 2] = '\0'; // do primeiro
        argv[0][Posicao_ultimo_caractere_argumeto_1 - 1] = '\0'; // parametro
        printf("Sintaxe Correta: %s T n arqA.dat arqB.dat arqC.dat arqD.dat arqE.dat\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

void *lerMatrizThread(void *args)
{
    MatrizDupla *matrizDupla = (MatrizDupla *)args;

    pthread_t idThreadMatrizA;
    pthread_t idThreadMatrizB;

    idThreadMatrizA = pthread_self();
    idThreadMatrizB = pthread_self();
    
    pthread_create(&idThreadMatrizA, NULL, lerMatrizDeArquivo, (void *)matrizDupla->matrizPrimeira);
    pthread_create(&idThreadMatrizB, NULL, lerMatrizDeArquivo, (void *)matrizDupla->matrizSegunda);

    pthread_join(idThreadMatrizA, NULL);
    pthread_join(idThreadMatrizB, NULL);

    return NULL;
}

void *chamarMultiplicacaoDoisThread(void *args)
{
    MatrizDupla *mE = (MatrizDupla *)args;
    int dimensao = mE->matrizPrimeira->dimensao;

    pthread_t idThreadA;
    pthread_t idThreadB;

    idThreadA = pthread_self();
    idThreadB = pthread_self();

    // Criação de estruturas temporárias para cada thread
    MatrizDupla mE_A, mE_B;
    mE_A.matrizPrimeira = mE->matrizPrimeira;
    mE_A.matrizSegunda = mE->matrizSegunda;
    mE_A.matrizResultado = mE->matrizResultado;
    mE_B.matrizPrimeira = mE->matrizPrimeira;
    mE_B.matrizSegunda = mE->matrizSegunda;
    mE_B.matrizResultado = mE->matrizResultado;
        
    // Configuração para thread A
    mE_A.linhaInicial = 0;
    mE_A.linhaFinal = dimensao / 2;
    mE_A.colunaInicial = 0;
    mE_A.colunaFinal = dimensao;

    // Configuração para thread B
    mE_B.linhaInicial = dimensao / 2;
    mE_B.linhaFinal = dimensao;
    mE_B.colunaInicial = 0;
    mE_B.colunaFinal = dimensao;

    pthread_create(&idThreadA, NULL, multiplicarMatrizes, (void *)&mE_A);
    pthread_create(&idThreadB, NULL, multiplicarMatrizes, (void *)&mE_B);

    pthread_join(idThreadA, NULL);
    pthread_join(idThreadB, NULL);

    return NULL;
}

void *chamarMultiplicacaoQuatroThread(void *args)
{
    MatrizDupla *mE = (MatrizDupla *)args;
    int dimensao = mE->matrizPrimeira->dimensao;

    pthread_t idThreadA;
    pthread_t idThreadB;
    pthread_t idThreadC;
    pthread_t idThreadD;

    idThreadA = pthread_self();
    idThreadB = pthread_self();
    idThreadC = pthread_self();
    idThreadD = pthread_self();

    // Criação de estruturas temporárias para cada thread
    MatrizDupla mE_A, mE_B, mE_C, mE_D;
    mE_A.matrizPrimeira = mE->matrizPrimeira;
    mE_A.matrizSegunda = mE->matrizSegunda;
    mE_A.matrizResultado = mE->matrizResultado;
    mE_B.matrizPrimeira = mE->matrizPrimeira;
    mE_B.matrizSegunda = mE->matrizSegunda;
    mE_B.matrizResultado = mE->matrizResultado;
    mE_C.matrizPrimeira = mE->matrizPrimeira;
    mE_C.matrizSegunda = mE->matrizSegunda;
    mE_C.matrizResultado = mE->matrizResultado;
    mE_D.matrizPrimeira = mE->matrizPrimeira;
    mE_D.matrizSegunda = mE->matrizSegunda;
    mE_D.matrizResultado = mE->matrizResultado;

    // Configuração para thread A
    mE_A.linhaInicial = 0;
    mE_A.linhaFinal = dimensao / 2;
    mE_A.colunaInicial = 0;
    mE_A.colunaFinal = dimensao / 2;

    // Configuração para thread B
    mE_B.linhaInicial = dimensao / 2;
    mE_B.linhaFinal = dimensao;
    mE_B.colunaInicial = 0;
    mE_B.colunaFinal = dimensao / 2;

    // Configuração para thread C
    mE_C.linhaInicial = 0;
    mE_C.linhaFinal = dimensao / 2;
    mE_C.colunaInicial = dimensao / 2;
    mE_C.colunaFinal = dimensao;

    // Configuração para thread D
    mE_D.linhaInicial = dimensao / 2;
    mE_D.linhaFinal = dimensao;
    mE_D.colunaInicial = dimensao / 2;
    mE_D.colunaFinal = dimensao;

    pthread_create(&idThreadA, NULL, multiplicarMatrizes, (void *)&mE_A);
    pthread_create(&idThreadB, NULL, multiplicarMatrizes, (void *)&mE_B);
    pthread_create(&idThreadC, NULL, multiplicarMatrizes, (void *)&mE_C);
    pthread_create(&idThreadD, NULL, multiplicarMatrizes, (void *)&mE_D);

    pthread_join(idThreadA, NULL);
    pthread_join(idThreadB, NULL);
    pthread_join(idThreadC, NULL);
    pthread_join(idThreadD, NULL);

    return NULL;
}

void *chamarReducaoDoisThreads(void *args)
{
    MatrizDupla *mE = (MatrizDupla *)args;
    int tamTotalMatriz = mE->matrizPrimeira->dimensao * mE->matrizPrimeira->dimensao;

    pthread_t idThreadA;
    pthread_t idThreadB;

    idThreadA = pthread_self();
    idThreadB = pthread_self();

    // Criação de estruturas temporárias para cada thread
    MatrizDupla mE_A, mE_B, mE_C, mE_D;
    mE_A.matrizResultado = mE->matrizResultado;
    mE_B.matrizResultado = mE->matrizResultado;

    // Configuração para thread A
    mE_A.vetorInicial = 0;
    mE_A.vetorFinal = tamTotalMatriz / 2;

    // Configuração para thread B
    mE_B.vetorInicial = tamTotalMatriz / 2;
    mE_B.vetorFinal = tamTotalMatriz;

    pthread_create(&idThreadA, NULL, calcularReducaoDeMatriz, (void *)&mE_A);
    pthread_create(&idThreadB, NULL, calcularReducaoDeMatriz, (void *)&mE_B);

    pthread_join(idThreadA, NULL);
    pthread_join(idThreadB, NULL);

    mE->matrizResultado->soma = mE_A.somaThread + mE_B.somaThread; 

    return NULL;

}

void *chamarReducaoQuatroThreads(void *args)
{
    MatrizDupla *mE = (MatrizDupla *)args;
    int tamTotalMatriz = mE->matrizPrimeira->dimensao * mE->matrizPrimeira->dimensao;

    pthread_t idThreadA;
    pthread_t idThreadB;
    pthread_t idThreadC;
    pthread_t idThreadD;

    idThreadA = pthread_self();
    idThreadB = pthread_self();
    idThreadC = pthread_self();
    idThreadD = pthread_self();

    // Criação de estruturas temporárias para cada thread
    MatrizDupla mE_A, mE_B, mE_C, mE_D;
    mE_A.matrizResultado = mE->matrizResultado;
    mE_B.matrizResultado = mE->matrizResultado;
    mE_C.matrizResultado = mE->matrizResultado;
    mE_D.matrizResultado = mE->matrizResultado;

    // Configuração para thread A
    mE_A.vetorInicial = 0;
    mE_A.vetorFinal = tamTotalMatriz / 4;

    // Configuração para thread B
    mE_B.vetorInicial = tamTotalMatriz / 4;
    mE_B.vetorFinal = tamTotalMatriz / 2;

    // Configuração para thread C
    mE_C.vetorInicial = tamTotalMatriz / 2;
    mE_C.vetorFinal = 3 * tamTotalMatriz / 4;

    // Configuração para thread D
    mE_D.vetorInicial = 3 * tamTotalMatriz / 4;
    mE_D.vetorFinal = tamTotalMatriz;

    pthread_create(&idThreadA, NULL, calcularReducaoDeMatriz, (void *)&mE_A);
    pthread_create(&idThreadB, NULL, calcularReducaoDeMatriz, (void *)&mE_B);
    pthread_create(&idThreadC, NULL, calcularReducaoDeMatriz, (void *)&mE_C);
    pthread_create(&idThreadD, NULL, calcularReducaoDeMatriz, (void *)&mE_D);

    pthread_join(idThreadA, NULL);
    pthread_join(idThreadB, NULL);
    pthread_join(idThreadC, NULL);
    pthread_join(idThreadD, NULL);

    mE->matrizResultado->soma = mE_A.somaThread + mE_B.somaThread + mE_C.somaThread + mE_D.somaThread; 

    return NULL;

}

void exibirResultados(int reducao, double tempoSoma, double tempoMultiplicacao, double tempoReducao, double tempoTotal)
{
    printf("Redução: %d\n", reducao);
    printf("Tempo soma: %.10lf segundos.\n", tempoSoma);
    printf("Tempo multiplicação: %.10lf segundos.\n", tempoMultiplicacao);
    printf("Tempo redução: %.10lf segundos.\n", tempoReducao);
    printf("Tempo total: %.10lf segundos.\n", tempoTotal);
}
