#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "pico/bootrom.h" // Configuração para teste do bootsel

#include "ws2818b.pio.h"
#include "hardware/clocks.h"
#include "matrizLED.h"
#include "hardware/uart.h"

#include "mic.h"
#include "math.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define BOTAO_B 6

ssd1306_t display;

void i2c_setup();
void npInit(uint pin);

int main()
{
    stdio_init_all();


    uint dma_channel = mic_init();

    // Buffer de amostras do ADC.
    uint16_t adc_buffer[SAMPLES];


    npInit(LED_PIN);
    npClear(); // limpa a matriz
    npWrite(); // Escreve os dados nos LEDs (reset neste caso)


    i2c_setup();
    ssd1306_init(&display, I2C_PORT, 64, 128, 0x3C, false);

    ssd1306_draw_string(&display, "Hello world! :)", 10, 20);
    ssd1306_update(&display);

    // Amostragem de teste.
    printf("Amostragem de teste...\n");
    mic_sample(adc_buffer, dma_channel);

    printf("Configuracoes completas!\n");

    while (true)
    {
        // Realiza uma amostragem do microfone.
        mic_sample(adc_buffer, dma_channel);

        // Pega a potência média da amostragem do microfone.
        float avg = mic_power(adc_buffer);
        avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)

        // Calcula intensidade a ser mostrada na matriz de LEDs.
        uint intensity = mic_get_intensity(avg) <= 5 ? mic_get_intensity(avg) : 5;
        printf("intensity: %d \n", intensity);

        npClear();
        switch (intensity) {
            case 0: 
                setLEDline(0,0,0,0);
                setLEDline(1,0,0,0);
                setLEDline(2,0,0,0);
                setLEDline(3,0,0,0);
                setLEDline(4,0,0,0);
                break;
            case 1:
                setLEDline(0,0,15,35);
                break;
            case 2:
                setLEDline(0,0,24,56);
                setLEDline(1,0,24,56);
                break;
            case 3:
                setLEDline(0,0,27,63);
                setLEDline(1,0,27,63);
                setLEDline(2,0,27,63);
                break;
            case 4:
                setLEDline(0,0,30,70);
                setLEDline(1,0,30,70);
                setLEDline(2,0,30,70);
                setLEDline(3,0,30,70);
                break;
            case 5: 
                setLEDline(0,0,30,70);
                setLEDline(1,0,30,70);
                setLEDline(2,0,30,70);
                setLEDline(3,0,30,70);
                setLEDline(4,0,30,70);
                break;
        }
        npWrite();

        /*Exemplo 1: semáforo*/
        // sleep_ms(1000);
        // npClear();
        // for (int i = 0; i < 5; i++)
        // {
        //     for (int j = 0; j < 5; j++)
        //     {
        //         setLEDxy(j, i, 25, 0, 0); // setLEDxy(x, y, r, g, b);
        //     }
        // }

        // npWrite(); // Escreve os dados nos LEDs.
        // sleep_ms(1000);
        // npClear();
        // for (int i = 0; i < 5; i++)
        // {
        //     for (int j = 0; j < 5; j++)
        //     {
        //         setLEDxy(j, i, 12.5, 12.5, 0);
        //     }
        // }
        // npWrite(); // Escreve os dados nos LEDs.
        // sleep_ms(1000);
        // npClear();
        // for (int i = 0; i < 5; i++)
        // {
        //     for (int j = 0; j < 5; j++)
        //     {
        //         setLEDxy(j, i, 0, 25, 0);
        //     }
        // }
        // npWrite(); // Escreve os dados nos LEDs.
        // sleep_ms(1000);

        /*Exemplo 2: Contador de 9s com reset automático*/
        // static int i = 9;
        // setLEDnumber(i,80,0,0);
        // if(i>0)
        //   i--;
        // else
        //   i=9;
        // sleep_ms(1000);

        /*Exemplo 3: Varredor de linhas*/
        // static int i = 4;
        // npClear();
        // setLEDline(i,80,0,0);
        // npWrite(); // Escreve os dados nos LEDs.
        // if(i>0)
        //   i--;
        // else
        //   i=4;
        // sleep_ms(1000);

        sleep_ms(200);
    }
}

void i2c_setup(void)
{

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs WS2818b.
 *
 * @param pin Pino GPIO usado para controlar os LEDs WS2818b.
 *
 * Este processo configura a máquina PIO e carrega o programa necessário
 * para controlar a matriz de LEDs WS2818b. O PIO (Programmable I/O) é utilizado
 * para gerar sinais de controle com precisão para os LEDs.
 */
void npInit(uint pin)
{

    // Adiciona o programa PIO específico para o controle dos LEDs WS2818b no PIO0.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0; // Definindo que o PIO utilizado será o PIO0.

    // Reivindica uma máquina de estados (SM) do PIO0 para usar no controle dos LEDs.
    sm = pio_claim_unused_sm(np_pio, false); // Tenta obter uma máquina de estados não utilizada.
    if (sm < 0)
    {
        // Se não houver máquinas de estados livres no PIO0, tenta usar o PIO1.
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }

    // Inicializa o programa WS2818b na máquina de estados (SM) obtida no PIO selecionado.
    // O parâmetro '800000.f' refere-se à taxa de atualização (em Hz) dos LEDs WS2818b.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    // Inicializa a matriz de LEDs, passando o PIO e a máquina de estados como parâmetros.
    npMatrizInit(np_pio, sm);
}