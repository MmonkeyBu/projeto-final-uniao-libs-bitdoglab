#include "MatrizLED.h"

/**
 * Para uma matriz 5x5 de LEDs, os índices são mapeados da seguinte forma:
 * 4 24 23 22 21 20
 * 3 15 16 17 18 19
 * 2 14 13 12 11 10
 * 1 05 06 07 08 09
 * 0 04 03 02 01 00
 *    0  1  2  3  4
 */

// Declaração do buffer de LEDs que formam a matriz.
npLED_t leds[LED_COUNT];

/**
 * Atribui as informações relevantes da máquina PIO em uso.
 * 
 * @param pio_info O PIO (processador de entrada/saída) em uso.
 * @param sm_info O número da máquina de estado (sm) em uso.
 */
void npMatrizInit(const PIO pio_info, const uint sm_info)
{
  np_pio = pio_info;  // Armazena a referência do PIO em uso.
  sm = sm_info;       // Armazena o número da máquina de estado.
}

/**
 * Atribui uma cor RGB a um LED específico no buffer.
 * 
 * @param index O índice do LED no buffer (0 a 24 para uma matriz 5x5).
 * @param r O valor da componente vermelha da cor (0-255).
 * @param g O valor da componente verde da cor (0-255).
 * @param b O valor da componente azul da cor (0-255).
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

/**
 * Limpa o buffer de LEDs, atribuindo a cor preta (0, 0, 0) a todos os LEDs.
 */
void npClear()
{
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);  // Define cada LED como apagado (RGB = 0, 0, 0).
}

/**
 * Escreve os dados do buffer de LEDs nos LEDs físicos, enviando os valores das cores.
 * Realiza a operação de forma sequencial para cada LED da matriz.
 */
void npWrite()
{
    // Para cada LED no buffer, envia as componentes de cor para a máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);  // Envia o valor do verde.
        pio_sm_put_blocking(np_pio, sm, leds[i].R);  // Envia o valor do vermelho.
        pio_sm_put_blocking(np_pio, sm, leds[i].B);  // Envia o valor do azul.
    }
    sleep_us(100); // Aguarda 100us para permitir o reset do sinal de controle (conforme datasheet).
}

/**
 * Converte as coordenadas (x, y) de uma matriz 5x5 para o índice correspondente no buffer linear de LEDs.
 * 
 * @param x A coluna (0 a 4).
 * @param y A linha (0 a 4).
 * @return O índice correspondente na fila linear de LEDs (0 a 24).
 */
int getIndex(int x, int y)
{
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return y * 5 + x;  // Linha par: percorre da esquerda para a direita.
    } else {
        return y * 5 + (4 - x);  // Linha ímpar: percorre da direita para a esquerda.
    }
}

/**
 * Define a cor de um LED específico na matriz, com base nas coordenadas (x, y).
 * 
 * @param y A coluna (0 a 4).
 * @param x A linha (0 a 4).
 * @param r O valor da componente vermelha da cor (0-255).
 * @param g O valor da componente verde da cor (0-255).
 * @param b O valor da componente azul da cor (0-255).
 */
void setLEDxy(const uint y, const uint x, const uint8_t r, const uint8_t g, const uint8_t b)
{
    int index = getIndex(y, x);  // Converte as coordenadas para o índice linear.
    npSetLED(index, r, g, b);    // Define a cor para o LED no índice calculado.
}

/**
 * Exibe um número de 0 a 9 na matriz 5x5, utilizando um template de números.
 * Cada número é representado por um padrão de LEDs acesos (1) ou apagados (0).
 * 
 * @param number O número a ser exibido (0-9).
 * @param r O valor da componente vermelha da cor (0-255).
 * @param g O valor da componente verde da cor (0-255).
 * @param b O valor da componente azul da cor (0-255).
 */
void setLEDnumber(const int number, const uint8_t r, const uint8_t g, const uint8_t b)
{
    npClear();  // Limpa a matriz antes de exibir o novo número.

    // Template para os números de 0 a 9 na matriz 5x5.
    int templates[10][5][5] = {
        // 0
        {{1, 1, 1, 1, 1},
         {1, 0, 0, 0, 1},
         {1, 0, 0, 0, 1},
         {1, 0, 0, 0, 1},
         {1, 1, 1, 1, 1}},

        // 1
        {{0, 1, 1, 1, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 1, 1, 0},
         {0, 0, 1, 0, 0}},

        // 2
        {{1, 1, 1, 1, 1},
         {0, 0, 0, 0, 1},
         {1, 1, 1, 1, 1},
         {1, 0, 0, 0, 0},
         {1, 1, 1, 1, 1}},

        // 3
        {{1, 1, 1, 1, 1},
         {1, 0, 0, 0, 0},
         {1, 1, 1, 1, 0},
         {1, 0, 0, 0, 0},
         {1, 1, 1, 1, 1}},

        // 4
        {{0, 1, 0, 0, 0},
         {0, 1, 0, 0, 0},
         {1, 1, 1, 1, 1},
         {0, 1, 0, 0, 1},
         {0, 1, 0, 0, 1}},

        // 5
        {{1, 1, 1, 1, 1},
         {1, 0, 0, 0, 0},
         {1, 1, 1, 1, 1},
         {0, 0, 0, 0, 1},
         {1, 1, 1, 1, 1}},

        // 6
        {{1, 1, 1, 1, 1},
         {1, 0, 0, 0, 1},
         {1, 1, 1, 1, 1},
         {0, 0, 0, 0, 1},
         {1, 1, 1, 1, 1}},

        // 7
        {{0, 0, 0, 1, 0},
         {0, 0, 1, 0, 0},
         {0, 1, 0, 0, 0},
         {1, 0, 0, 0, 0},
         {1, 1, 1, 1, 1}},

        // 8
        {{1, 1, 1, 1, 1},
         {1, 0, 0, 0, 1},
         {1, 1, 1, 1, 1},
         {1, 0, 0, 0, 1},
         {1, 1, 1, 1, 1}},

        // 9
        {{1, 1, 1, 1, 1},
         {1, 0, 0, 0, 0},
         {1, 1, 1, 1, 1},
         {1, 0, 0, 0, 1},
         {1, 1, 1, 1, 1}}
    };

    // Preenche a matriz com os LEDs acesos de acordo com o template do número.
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (templates[number][i][j] == 1)
                setLEDxy(j, i, r, g, b);  // Define a cor para os LEDs correspondentes ao número.
        }
    }

    npWrite();  // Atualiza a matriz de LEDs com os novos valores.
}

/**
 * Exibe uma linha na matriz 5x5.
 * 
 * @param line A linha selecionada (0-4).
 * @param r O valor da componente vermelha da cor (0-255).
 * @param g O valor da componente verde da cor (0-255).
 * @param b O valor da componente azul da cor (0-255).
 */
void setLEDline(const int line, const uint8_t r, const uint8_t g, const uint8_t b)
{
    for (int i = 0; i < 5; i++)
    {
        setLEDxy(i, line, r, g, b);
    }
}

/**
 * Exibe uma coluna na matriz 5x5.
 * 
 * @param row A coluna selecionada (0-4).
 * @param r O valor da componente vermelha da cor (0-255).
 * @param g O valor da componente verde da cor (0-255).
 * @param b O valor da componente azul da cor (0-255).
 */
void setLEDrow(const int row, const uint8_t r, const uint8_t g, const uint8_t b)
{
    for (int i = 0; i < 5; i++)
    {
        setLEDxy(row, i, r, g, b);
    }
}
