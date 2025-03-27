#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "pico/bootrom.h"
#include "ws2818b.pio.h"
#include "hardware/clocks.h"
#include "matrizLED.h"
#include "hardware/uart.h"
#include "callbacks_timer.h"
#include "mic.h"
#include "math.h"
#include "init_GPIO.h"

ssd1306_t display;

// Protótipos de funções
void i2c_setup(void);
void npInit(uint pin);
void update_full_display(ssd1306_t *display, float db_value, uint8_t sensitivity);
void draw_progress_bar(ssd1306_t *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress);
void update_led_matrix(float db);

// Variáveis globais
uint8_t sensitivity_level = 1; // Nível de sensibilidade (1 a 5)

// Faixas de dB e cores para cada nível
const struct {
    float min_db;
    float max_db;
    uint8_t color[3]; // R, G, B
} SENSITIVITY_RANGES[5] = {
    {60.0f, 90.0f, {0, 30, 70}},   // Nível 1 (azul)
    {50.0f, 80.0f, {0, 50, 50}},   // Nível 2 (ciano)
    {40.0f, 70.0f, {50, 50, 0}},   // Nível 3 (amarelo)
    {30.0f, 60.0f, {80, 30, 0}},   // Nível 4 (laranja)
    {20.0f, 50.0f, {80, 0, 0}}     // Nível 5 (vermelho)
};

int main() {
    stdio_init_all();

    // Inicializações
    botao_init(BOTAO_A);
    botao_init(BOTAO_B);
    uint dma_channel = mic_init();
    
    // Configura LEDs
    npInit(LED_PIN);
    npClear();
    npWrite();
    
    // Configura display
    i2c_setup();
    ssd1306_init(&display, I2C_PORT, 64, 128, 0x3C, false);
    
    // Mensagem inicial
    ssd1306_draw_string(&display, "INICIANDO...", 20, 30);
    ssd1306_update(&display);
    sleep_ms(1000);
    ssd1306_clear_display(&display);

    // Buffer de amostras
    uint16_t adc_buffer[SAMPLES];
    mic_sample(adc_buffer, dma_channel);

    while (true) {
        mic_sample(adc_buffer, dma_channel);
        float rms = mic_power(adc_buffer);
        float rms_voltage = fabs(rms);
        float db = mic_rms_to_db(rms_voltage);
        
        printf("dB: %.1f, Sens: %d\n", db, sensitivity_level);
        
        update_full_display(&display, db, sensitivity_level);
        
        npClear();
        update_led_matrix(db);
        npWrite();
        
        sleep_ms(200);
    }
}

void update_led_matrix(float db) {
    if (db < 0) return;

    // O nível de sensibilidade não deve alterar o valor do dB, apenas a exibição dos LEDs.
    // Ajuste especial para o nível 5 (sensibilidade máxima)
    float min_db = SENSITIVITY_RANGES[sensitivity_level].min_db;
    float max_db = SENSITIVITY_RANGES[sensitivity_level].max_db;
    
    // Se for nível 5 e estiver abaixo do mínimo, mostra pelo menos 1 LED
    if (sensitivity_level == 5 && db < min_db) {
        db = min_db;
    }
    
    // Atualiza indicador de sensibilidade (colunas 3 e 4)
    for (int y = 0; y < sensitivity_level; y++) {
        setLEDxy(3, y, 0, 30, 70); // Azul claro
        setLEDxy(4, y, 0, 30, 70);
    }

    // Calcula posição na faixa (0.0 a 1.0)
    float position = (db - min_db) / (max_db - min_db);
    position = position < 0 ? 0 : (position > 1 ? 1 : position);

    // Mapeia para número de linhas (0-5)
    uint8_t rows_to_light = (uint8_t)(position * 5.0f);

    // Atualiza medição com cores progressivas (colunas 0 e 1)
    for (int y = 0; y < rows_to_light; y++) {
        uint8_t red, green, blue;
        
        // Cores baseadas na altura:
        if (y == 0) {           // Primeira linha - verde
            red = 0; green = 30; blue = 0;
        } 
        else if (y <= 2) {      // Linhas 2 e 3 - amarelo
            red = 50; green = 50; blue = 0;
        } 
        else {                  // Linhas 4 e 5 - vermelho
            red = 80; green = 0; blue = 0;
        }
        
        // Efeito de alerta se exceder o máximo
        if (db > max_db) {
            red = 100; green = 0; blue = 0;
            // Pisca se estiver muito acima
            if ((db > max_db + 10) && (to_ms_since_boot(get_absolute_time()) % 200 < 100)) {
                red = green = blue = 0;
            }
        }

        setLEDxy(0, y, red, green, blue);
        setLEDxy(1, y, red, green, blue);
    }
}


void update_full_display(ssd1306_t *display, float db_value, uint8_t sensitivity) {
    float display_max_db = SENSITIVITY_RANGES[sensitivity].max_db * 1.2f;
    
    ssd1306_clear_display(display);
    
    // Cabeçalho
    ssd1306_draw_string(display, "NIVEL DE RUIDO", 15, 2);
    ssd1306_draw_line(display, 0, 12, 127, 12);
    
    // Valor atual
    char db_str[16];
    sprintf(db_str, "%.1f dB", db_value);
    uint8_t x_pos = (display->width - strlen(db_str) * 6) / 2;
    ssd1306_draw_string(display, db_str, x_pos, 20);
    
    // Barra de progresso
    uint8_t progress = (uint8_t)((db_value / display_max_db) * 100);
    progress = progress > 100 ? 100 : progress;
    draw_progress_bar(display, 10, 35, 108, 8, progress);
    
    // Indicador de nível
    const char* level_str;
    if (db_value < 30.0f)      level_str = "Silencioso";
    else if (db_value < 60.0f) level_str = "Moderado";
    else if (db_value < 90.0f) level_str = "Ruidoso";
    else                       level_str = "PERIGOSO!";
    x_pos = (display->width - strlen(level_str) * 6) / 2;
    ssd1306_draw_string(display, level_str, x_pos, 46);
    
    // Sensibilidade
    ssd1306_draw_string(display, "Sens:", 10, 56);
    for(uint8_t i = 0; i < 5; i++) {
        ssd1306_draw_char(display, i < sensitivity ? 0xFF : '-', 50 + i*10, 56);
    }
    
    ssd1306_update(display);
}

void draw_progress_bar(ssd1306_t *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress) {
    // Borda
    ssd1306_draw_empty_rectangle(display, x, y, x+width, y+height);
    
    // Preenchimento
    uint8_t fill_width = (progress * (width-4)) / 100;
    ssd1306_draw_filled_rectangle(display, x+2, y+2, x+2+fill_width, y+height-2);
}

void i2c_setup(void) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, false);
    
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }
    
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
    npMatrizInit(np_pio, sm);
}