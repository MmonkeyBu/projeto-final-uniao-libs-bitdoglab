# Sistema de Monitoramento de Nível de Ruído
## 📌 Visão Geral
Este projeto implementa um sistema de monitoramento de nível de ruído utilizando o Raspberry Pi Pico, com:

-Display OLED para visualização

-Matriz de LEDs WS2812B como medidor

-Microfone para captura de áudio

-5 níveis de sensibilidade configuráveis

##🛠️ Hardware Necessário
-Raspberry Pi Pico

-Display OLED SSD1306 (128x64, I2C)

-Matriz de LEDs WS2812B (5x5)

-Microfone analógico com ADC

-Botões para controle

##🔧 Configuração
-Conecte os componentes conforme definido em init_GPIO.h

-Alimente o sistema via USB ou fonte externa

-Os pinos padrão são:

-I2C: SDA=GP4, SCL=GP5

-LEDs: GP16

-Botões: GP14 e GP15

#🎛️ Funcionalidades Principais
##📊 Visualização em OLED
-Exibe nível de dB em tempo real

-Barra de progresso gráfica

-Classificação do ambiente:

-Silencioso (<30dB)

-Moderado (30-60dB)

-Ruidoso (60-90dB)

-Perigoso (>90dB)

-🌈 Matriz de LEDs
Colunas 0-1: Medição principal com cores progressivas:

-Verde (níveis baixos)🟩

-Amarelo (níveis médios)🟨

-Vermelho (níveis altos)🟥

Colunas 3-4: Indicador de sensibilidade📊

#⚙️ Níveis de Sensibilidade
Nível	Faixa dB	Cor	Aplicação típica
1	60-90	Azul	Ambientes ruidosos
2	50-80	Ciano	Escritórios
3	40-70	Amarelo	Residências
4	30-60	Laranja	Estúdios
5	20-50	Vermelho	Gravação profissional
⚠️ Alerta Visual
Pisca quando o nível excede em 10dB o máximo configurado

Vermelho sólido para níveis acima do máximo

🖥️ Estrutura do Código
Principais Funções
```c
Copy
void update_led_matrix(float db);          // Atualiza a matriz de LEDs
void update_full_display(ssd1306_t *display, float db, uint8_t sens); // Atualiza OLED
void mic_sample(uint16_t *buffer, uint channel); // Captura áudio
float mic_rms_to_db(float rms_voltage);   // Converte RMS para dB
Variáveis Globais

Copy
uint8_t sensitivity_level;                 // Nível atual (1-5)
ssd1306_t display;                         // Estrutura do display
const struct {...} SENSITIVITY_RANGES[5];  // Configurações de sensibilidade
```
##📈 Fluxo de Operação
-Captura amostra de áudio via ADC

-Calcula RMS e converte para dB

-Atualiza:

-Display OLED com valores e barra de progresso

-Matriz de LEDs com representação visual

*Repete a cada 200ms*

##🛠️ Dependências
-Biblioteca SSD1306 para OLED

-Biblioteca para WS2812B

-HardwareAPI do Pico SDK

##🔄 Melhorias Futuras
-Adicionar calibração automática

-Implementar logging de dados

-Adicionar modo noturno (inversão de cores)

-Conexão Bluetooth para monitoramento remoto

##⚠️ Limitações Conhecidas
-Precisão limitada pelo microfone analógico

-Atualização dos LEDs bloqueante (pode afetar responsividade)

#📄 Licença
MIT License
