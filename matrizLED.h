#ifndef MATRIZLED_H
#define MATRIZLED_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Definição do número total de LEDs na matriz 5x5 (25 LEDs) e pino usado para controle.
#define LED_COUNT 25
#define LED_PIN 7

// Definição da estrutura para representar um pixel em formato GRB (Green, Red, Blue).
struct pixel_t {
    uint8_t G, R, B; // Cada valor de 8 bits representa a intensidade da cor (G, R, B) do pixel.
};

typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Tipo definido para representar cada LED como um pixel.

// Variáveis para controle do hardware PIO e da máquina de estados.
static PIO np_pio; // Ponteiro para o PIO.
static uint sm; // Máquina de estados para controle do PIO.

// Função para inicializar a matriz de LEDs, configurando o PIO e a máquina de estados.
void npMatrizInit(const PIO pio_info, const uint sm_info);

// Função para atribuir uma cor RGB a um LED específico na matriz, dado o índice.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);

// Função para limpar o buffer de LEDs, ou seja, desligar todos os LEDs da matriz.
void npClear();

// Função para escrever os dados do buffer (cor de cada LED) na matriz de LEDs.
void npWrite();

// Função para calcular o índice linear de um LED, dado as coordenadas (x, y) na matriz 5x5.
int getIndex(int x, int y);

// Função para configurar a cor de um LED específico usando suas coordenadas (x, y) na matriz.
void setLEDxy(const uint y, const uint x, const uint8_t r, const uint8_t g, const uint8_t b);

// Função para mostrar um número (de 0 a 9) na matriz de LEDs, com a cor especificada para os LEDs acesos.
void setLEDnumber(const int number, const uint8_t r, const uint8_t g, const uint8_t b);

// Função para atribuir uma cor RGB a uma linha na matriz de LED.
void setLEDline(const int line, const uint8_t r, const uint8_t g, const uint8_t b);

// Função para atribuir uma cor RGB a uma coluna na matriz de LED.
void setLEDrow(const int row, const uint8_t r, const uint8_t g, const uint8_t b);

#endif // MATRIZLED_H
