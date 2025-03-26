#include "callbacks_timer.h"

#include "pico/bootrom.h"  // Configuração para teste do bootsel
#include "pico/stdlib.h"
#include "init_GPIO.h"
#include <stdio.h>

// Variáveis para debounce
static volatile uint32_t ultima_interrupcao_b = 0;
static volatile uint32_t ultima_interrupcao_a = 0;

// Variáveis globais para armazenar os valores de sensibilidade e threshold
extern uint8_t sensitivity_level;
extern float threshold;

// Variáveis globais para armazenar os valores de x e y do joystick
extern int x;  // Declarado em outro arquivo (por exemplo, main.c)
extern int y;  // Declarado em outro arquivo (por exemplo, main.c)

// Função de callback para tratar interrupções dos botões
void botao_callback(uint gpio, uint32_t eventos) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    
    switch (gpio) {
        case BOTAO_A:
            if (tempo_atual - ultima_interrupcao_a > DEBOUNCE_TIME) {
                ultima_interrupcao_a = tempo_atual;
                if (eventos & GPIO_IRQ_EDGE_FALL) {
                    printf("Botão A pressionado\n");
                    sensitivity_level = (sensitivity_level % 5) + 1; // Cicla entre 1 e 5.
                    threshold = sensitivity_level * 0.1f;            // Ajusta o limiar com base no nível.
                    printf("Sensibilidade ajustada: %d, Limiar: %.2f\n", sensitivity_level, threshold);
                }
            }
            break;

        case BOTAO_B:
            if (tempo_atual - ultima_interrupcao_b > DEBOUNCE_TIME) {
                ultima_interrupcao_b = tempo_atual;
                if (eventos & GPIO_IRQ_EDGE_FALL) {
                    printf("Botão B pressionado\n");
                    reset_usb_boot(0, 0); // Entra no modo bootsel
                }
            }
            break;
    
        default:
            break;
    }
}

// Função para inicializar um botão
void botao_init(uint8_t pino) {
    gpio_init(pino);
    gpio_set_dir(pino, GPIO_IN);
    gpio_pull_up(pino);
    gpio_set_irq_enabled_with_callback(pino, GPIO_IRQ_EDGE_FALL, true, &botao_callback);
}