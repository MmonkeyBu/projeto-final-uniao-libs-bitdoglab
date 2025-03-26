#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "pico/bootrom.h" // Configuração para teste do bootsel

#include "ws2818b.pio.h"
#include "hardware/clocks.h"
#include "matrizLED.h"
#include "hardware/uart.h"

#include "callbacks_timer.h"
#include "mic.h"
#include "math.h"
#include "init_GPIO.h"

ssd1306_t display;

void i2c_setup();
void npInit(uint pin);

// Variáveis globais para sensibilidade
uint8_t sensitivity_level = 1; // Nível de sensibilidade (1 a 5)
float threshold = 0.5f;        // Limiar inicial (ajustável pelo botão)

int main()
{
    stdio_init_all();

    botao_init(BOTAO_A);
    botao_init(BOTAO_B);

    uint dma_channel = mic_init();

    npInit(LED_PIN);
    npClear(); // limpa a matriz
    npWrite(); // Escreve os dados nos LEDs (reset neste caso)

    i2c_setup();
    ssd1306_init(&display, I2C_PORT, 64, 128, 0x3C, false);

    // Buffer de amostras do microfone.
    uint16_t adc_buffer[SAMPLES];

    // Exibe uma mensagem inicial no OLED.
    ssd1306_init(&display, I2C_PORT, 64, 128, 0x3C, false);
    ssd1306_draw_string(&display, "OLA MAGNATA!", 10, 20);
    ssd1306_update(&display);
    sleep_ms(500);
    ssd1306_clear_display(&display);

    // Amostragem de teste.
    printf("Amostragem de teste...\n");
    mic_sample(adc_buffer, dma_channel);
    printf("Configuracoes completas!\n");

    float filtered_db = 0.0f;
    char db_str[20];

    while (true)
    {
        // Realiza uma amostragem do microfone.
        mic_sample(adc_buffer, dma_channel);

        // Print out raw ADC values
        printf("Raw ADC Values:\n");
        for (int i = 0; i < 10; i++)
        {
            printf("%d ", adc_buffer[i]);
        }
        printf("\n");

        float rms = mic_power(adc_buffer);
        float rms_voltage = fabs(rms); // Use absolute value
        float db = mic_rms_to_db(rms_voltage, sensitivity_level);

        printf("Detailed Debug:\n");
        printf("Raw ADC: First 10 values are printed above\n");
        printf("RMS Voltage: %.6f V\n", rms_voltage);
        printf("Calculated dB: %.2f\n", db);

        // Calcula a potência média do som.
        float avg = mic_power(adc_buffer);
        avg = 2.f * abs(ADC_ADJUST(avg));      // Ajusta para intervalo de 0 a 3.3V.
        printf("Volume captado: %.2f\n", avg); // Log para depuração.

        sprintf(db_str, "%.1f dB", db);
        ssd1306_draw_string(&display, db_str, 5, 5);
        ssd1306_update(&display);

        // Limpa a matriz antes de atualizar.
        npClear();

        // Exibe o nível de sensibilidade nas duas primeiras colunas.
        for (int y = 0; y < sensitivity_level; y++)
        {
            setLEDxy(3, 0 + y, 0, 30, 70); // Primeira coluna.
            setLEDxy(4, 0 + y, 0, 30, 70); // Segunda coluna.
        }

        // Exibe a onda de LEDs nas duas últimas colunas baseado nos decibéis
        if (db > 0) // Apenas acende LEDs se houver som detectado
        {
            // Mapeia os decibéis para o número de linhas acesas (0 a 5)
            // Considerando uma faixa de 0 a 80 dB como intervalo útil
            float normalized_db = db / 20.0f;  // Dividir por um valor que faça sentido para seu contexto
            uint8_t rows_to_light = (uint8_t)(normalized_db);

            // Limita o número de linhas para no máximo 5
            if (rows_to_light > 5)
            rows_to_light = 5;

            printf("dB Normalizado: %.2f, Linhas acesas: %d\n", normalized_db, rows_to_light);

            // Escolhe cores dinâmicas baseadas na intensidade do som
            uint8_t red, green, blue;

            // Gradiente de cores: verde (baixo volume) -> amarelo -> laranja -> vermelho (alto volume)
            if (rows_to_light <= 1)
            {
                // Verde para baixos volumes
                red = 0;
                green = 30;
                blue = 0;
            }
            else if (rows_to_light <= 3)
            {
                // Amarelo para volumes médios
                red = 50;
                green = 50;
                blue = 0;
            }
            else if (rows_to_light >= 4)
            {
                // Vermelho para altos volumes
                red = 80;
                green = 0;
                blue = 0;
            }

            // Acende os LEDs nas duas últimas colunas com cores dinâmicas
            for (int y = 0; y < rows_to_light; y++)
            {
                setLEDxy(0, 0 + y, red, green, blue); // Quarta coluna
                setLEDxy(1, 0 + y, red, green, blue); // Quinta coluna
            }
        }

        // Atualiza a matriz de LEDs.
        npWrite();

        sleep_ms(300); // Aguarda um pouco antes da próxima iteração.
        ssd1306_clear_display(&display);
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
