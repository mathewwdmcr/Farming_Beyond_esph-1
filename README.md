# Farming_Beyond_esph-1
Farming Beyond data logger

Funcionalidades

- **Menu de Idiomas:** Seleção de interface entre Português, Inglês e Espanhol na inicialização ou através da navegação.
- **Relógio em Tempo Real (RTC):** Exibição contínua de data e hora atualizadas com precisão.
- **Monitoramento Ambiental:** Leitura em tempo real de temperatura (°C) e umidade (%) através do sensor DHT22.
- **Estrutura de Log (EEPROM):** Preparado para salvar registros compactados de data, hora e clima diretamente na memória interna do Arduino.
- **Interface Segura:** Sistema de filtros contra leituras incorretas dos sensores (valores `NaN`) e tratamento de debounce nos botões para evitar cliques fantasmas.

Componentes do Sistema

Para montar este projeto, você precisará dos seguintes componentes de hardware:
1. **Placa Microcontroladora:** Arduino Uno
2. **Display:** LCD 20x4 com Módulo Adaptador I2C.
3. **Sensor de Clima:** DHT22
4. **Módulo de Relógio:** RTC DS3231
5. **Botões (Push Buttons):** 2 unidades
6. **Protoboard**
7. **21x fios macho -> macho**

---

Estrutura de Conexões (Pinagem)

Os componentes devem ser conectados ao Arduino seguindo a tabela abaixo:
| Componente | Pino no Componente | Pino no Arduino | Observação |
| :--- | :--- | :--- | :--- |
| **Display LCD I2C** | VCC / GND | 5V / GND | Alimentação |
| | SDA | A4 (SDA) | Comunicação I2C |
| | SCL | A5 (SCL) | Comunicação I2C |
| **RTC DS3231** | VCC / GND | 5V / GND | Alimentação |
| | SDA | A4 (SDA) | Compartilha a linha I2C |
| | SCL | A5 (SCL) | Compartilha a linha I2C |
| **Sensor DHT22** | VCC / GND | 5V / GND | Alimentação |
| | DATA (Dados) | **Pino 4** | Entrada Digital |
| **Botão Navegar** | Terminal 1 / 2 | **Pino 2** / GND | Configurado como INPUT_PULLUP |
| **Botão Selecionar**| Terminal 1 / 2 | **Pino 3** / GND | Configurado como INPUT_PULLUP |

Instruções de Uso

1. Pré-requisitos (Instalação de Bibliotecas)
Antes de carregar o código, instale as seguintes bibliotecas diretamente no Gerenciador de Bibliotecas da Arduino IDE (`Ferramentas` > `Gerenciar Bibliotecas...`):
- `Wire` (Nativa)
- `EEPROM` (Nativa)
- `LiquidCrystal_I2C` por Frank de Brabander
- `RTClib` por Adafruit
- `DHT sensor library` por Adafruit

2. Operação do Sistema
1. Ligando o Dispositivo: Assim que o Arduino for energizado, a tela inicial exibirá o menu de configuração: `Config menu:`.
2. Navegando entre Idiomas: - Pressione o Botão Navegar Azul (Pino 2) para mover a seta (`>`) entre as opções: *Portugues*, *English* ou *Espanol*.
3. Confirmando o Idioma:
   - Posicione a seta no idioma desejado e pressione o Botão Selecionar Verde (Pino 3).
4. Tela de Monitoramento:
   - O display passará a exibir o cabeçalho no idioma escolhido, acompanhado da data/hora atual (atualizada segundo a segundo) e dos dados de temperatura e umidade do sensor DHT22.
5. Voltando ao Menu:
   - Caso queira alterar o idioma novamente, estando na tela de monitoramento, basta pressionar o Botão Navegar Azul para retornar instantaneamente ao menu inicial.
