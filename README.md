📢 Sistema de Monitoramento de Nível de Ruído Inteligente
Badge Status
Badge Versão

🌟 Visão Geral
Um sistema avançado de monitoramento acústico desenvolvido para Raspberry Pi Pico que combina:

🖥️ Display OLED SSD1306 para visualização detalhada

🌈 Matriz de LEDs WS2812B 5x5 como medidor visual intuitivo

🎤 Captura de áudio preciso com microfone analógico

⚙️ 5 níveis de sensibilidade ajustáveis para diferentes ambientes

🧩 Componentes Necessários
Componente	Especificações
Raspberry Pi Pico	Microcontrolador RP2040
Display OLED	SSD1306 128x64 (I2C)
Matriz de LEDs	WS2812B 5x5
Microfone	Analógico com saída ADC
Botões	2x para controle
🛠️ Configuração de Hardware
plaintext
Copy
Pinos Padrão:
- I2C: SDA=GP4, SCL=GP5
- Matriz LED: GP16
- Botão A: GP14
- Botão B: GP15
- Microfone: ADC0 (GP26)
✨ Recursos Principais
🖥️ Visualização OLED
Gráfico de barras em tempo real

Medição numérica precisa (dB)

Classificação automática do ambiente:

🟢 Silencioso (<30dB)

🟡 Moderado (30-60dB)

🟠 Ruidoso (60-90dB)

🔴 Perigoso (>90dB)

🌈 Matriz LED Inteligente
Colunas	Função	Cores
0-1	Medição principal	Verde → Amarelo → Vermelho
3-4	Indicador de sensibilidade	Azul degradê
⚙️ Níveis de Sensibilidade
Nível	Faixa (dB)	Cor	Aplicação
1	60-90	🔵	Ambientes industriais
2	50-80	💧	Escritórios abertos
3	40-70	🟡	Residências
4	30-60	🟠	Estúdios caseiros
5	20-50	🔴	Estúdios profissionais
⚠️ Sistema de Alerta
🔥 Pisca rapidamente quando excede +10dB do limite

🔴 Vermelho contínuo para níveis perigosos

📈 Efeito visual de "transbordamento" para níveis extremos

🏗️ Arquitetura do Sistema
mermaid
Copy
graph TD
    A[Microfone] -->|Sinal Analógico| B(ADC)
    B --> C[Processamento DSP]
    C --> D[Conversão dB]
    D --> E[Matriz LED]
    D --> F[Display OLED]
    G[Botões] --> H[Ajuste Sensibilidade]
📚 Documentação Técnica
🔧 Funções Principais
```c
Copy
// Atualiza matriz de LEDs com efeitos visuais
void update_led_matrix(float db) {
  // Implementação com suavização e transições de cor
}

// Exibe dados no OLED com formatação profissional
void update_full_display(ssd1306_t *display, float db, uint8_t sens) {
  // Layout otimizado com informações hierárquicas
}

// Captura e processa amostras de áudio
void mic_sample(uint16_t *buffer, uint channel) {
  // Algoritmo com filtro anti-ruído
}
⚙️ Variáveis Globais
c
Copy
typedef struct {
  float min_db;
  float max_db;
  uint8_t color[3];  // RGB
} SensitivityRange;

SensitivityRange SENSITIVITY_RANGES[5] = {
  {60.0f, 90.0f, {0, 100, 200}},   // Azul
  {50.0f, 80.0f, {0, 180, 180}},   // Ciano
  {40.0f, 70.0f, {180, 180, 0}},   // Amarelo
  {30.0f, 60.0f, {255, 120, 0}},   // Laranja
  {20.0f, 50.0f, {255, 0, 0}}      // Vermelho
};
```
🚀 Guia Rápido
Conecte todos os componentes

Carregue o firmware no Pico

Ajuste a sensibilidade com os botões:

Botão A: Aumenta sensibilidade

Botão B: Diminui sensibilidade

Observe a visualização em tempo real

🔮 Roadmap
Calibração automática de ganho

Modo noturno com inversão de cores

Exportação de dados via serial

Integração Bluetooth (BLE)

Aplicativo móvel para monitoramento

⚠️ Considerações Importantes
A precisão varia conforme a qualidade do microfone

Em ambientes muito ruidosos, pode ocorrer saturação

Recomendado operar entre 0-40°C

📜 Licença
Distribuído sob licença MIT. Consulte o arquivo LICENSE para mais informações.
