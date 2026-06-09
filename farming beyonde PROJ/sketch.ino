#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <DHT.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Inicializa o módulo RTC
RTC_DS3231 rtc;

// Configuração do DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Definição dos pinos dos botões
const int pinoBotaoNavegar = 2;
const int pinoBotaoSelecionar = 3;

unsigned long ultimoSalvamento = 0;
const long intervaloSalvamento = 30000; // Tempo em milissegundos (30000 ms = 30 segundos)

// Variáveis de controle dos botões (Debounce simples)
bool ultimoEstadoNavegar = HIGH;
bool ultimoEstadoSelecionar = HIGH;

// Estados do sistema
enum Estados { MENU_IDIOMA, TELA_MENSAGEM };
Estados estadoAtual = MENU_IDIOMA;

// Idiomas disponíveis
enum Idiomas { PORTUGUES, INGLES, ESPANHOL };
Idiomas idiomaSelecionado = PORTUGUES;
int opcaoMenu = 0; // 0 = PT, 1 = EN, 2 = ES

// Textos das mensagens (Apenas o cabeçalho, a data vem do RTC)
const char* msgPT = "Data de hoje:";
const char* msgEN = "Today's date:";
const char* msgES = "Fecha de hoy:";

// CONFIGURAÇÃO DA EEPROM
struct Registro {
  uint32_t timestamp; // Guarda data e hora juntas
  int16_t temp;       // Temperatura multiplicada por 10
  int16_t umid;       // Umidade multiplicada por 10
};

const int enderecoContador = 0; // Endereço 0 guarda quantos registros já foram feitos
int enderecoAtual = sizeof(int); // Registros começam logo após o contador (Byte 2)

void setup() {
  // Configura os pinos dos botões com o Pull-up interno ativado
  pinMode(pinoBotaoNavegar, INPUT_PULLUP);
  pinMode(pinoBotaoSelecionar, INPUT_PULLUP);

  // Inicializa o LCD
  lcd.init();
  lcd.backlight();

  // Inicializa o DHT22
  dht.begin();

  // Inicializa o RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC nao achado!");
    while (1); 
  }

  // Se o RTC perdeu energia, ajusta com a hora da compilação
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Desenha o menu inicial
  atualizarMenu();
}

void loop() {
  // Leitura dos botões
  bool leituraNavegar = digitalRead(pinoBotaoNavegar);
  bool leituraSelecionar = digitalRead(pinoBotaoSelecionar);

  // LÓGICA DO BOTÃO NAVEGAR
  if (leituraNavegar == LOW && ultimoEstadoNavegar == HIGH) {
    delay(50); // Debounce rápido
    if (estadoAtual == MENU_IDIOMA) {
      opcaoMenu = (opcaoMenu + 1) % 3; // Alterna entre 0, 1 e 2
      atualizarMenu();
    } else if (estadoAtual == TELA_MENSAGEM) {
      estadoAtual = MENU_IDIOMA;
      atualizarMenu();
    }
  }
  ultimoEstadoNavegar = leituraNavegar;

  // LÓGICA DO BOTÃO SELECIONAR 
  if (leituraSelecionar == LOW && ultimoEstadoSelecionar == HIGH) {
    delay(50); // Debounce rápido
    if (estadoAtual == MENU_IDIOMA) {
      idiomaSelecionado = (Idiomas)opcaoMenu;
      estadoAtual = TELA_MENSAGEM;
      lcd.clear(); 
    }
  }
  ultimoEstadoSelecionar = leituraSelecionar;

  // Atualizar a tela dos dados e salvar por tempo
  if (estadoAtual == TELA_MENSAGEM) {
    exibirMensagemERelogio();
    
    // LÓGICA PARA SALVAR
    if (millis() - ultimoSalvamento >= intervaloSalvamento) {
      ultimoSalvamento = millis();
      salvarDadosEEPROM();
    }

    delay(200); // Pequena pausa para o display não piscar agressivamente
  }
}

// Função para desenhar a tela de configuração
void atualizarMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Config menu:");

  // Exibe as opções e coloca uma seta (>) na opção atual
  lcd.setCursor(2, 1);
  lcd.print((opcaoMenu == 0) ? "> Portugues" : "  Portugues");

  lcd.setCursor(2, 2);
  lcd.print((opcaoMenu == 1) ? "> English" : "  English");

  lcd.setCursor(2, 3);
  lcd.print((opcaoMenu == 2) ? "> Espanol" : "  Espanol");
}

void exibirMensagemERelogio() {
  // Escreve o cabeçalho na primeira linha (Linha 0)
  lcd.setCursor(0, 0);
  switch (idiomaSelecionado) {
    case PORTUGUES: lcd.print(msgPT); break;
    case INGLES:    lcd.print(msgEN); break;
    case ESPANHOL:  lcd.print(msgES); break;
  }

  // Pega os dados atuais do RTC
  DateTime agora = rtc.now();

  lcd.setCursor(0, 1);

  // escreve a data
  if (agora.day() < 10) lcd.print('0');
  lcd.print(agora.day(), DEC);
  lcd.print('/');
  if (agora.month() < 10) lcd.print('0');
  lcd.print(agora.month(), DEC);
  lcd.print('/');
  lcd.print(agora.year(), DEC);

  lcd.print(" "); // espaço em branco para separar a data da hora

  // escreve a hora
  if (agora.hour() < 10) lcd.print('0');
  lcd.print(agora.hour(), DEC);
  lcd.print(':');
  if (agora.minute() < 10) lcd.print('0');
  lcd.print(agora.minute(), DEC);
  lcd.print(':');
  if (agora.second() < 10) lcd.print('0');
  lcd.print(agora.second(), DEC);

  // Leitura do DHT22
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Linha 2 (Terceira Linha): Dados do Sensor DHT22
  lcd.setCursor(0, 2);
  
  // Verifica se o sensor está lendo corretamente para não exibir erros na tela
  if (isnan(umidade) || isnan(temperatura)) {
    lcd.print("Erro no DHT22!    ");
  } else {
    // Exibe a Temperatura
    lcd.print("T:");
    lcd.print(temperatura, 1);
    lcd.print((char)223);      // Desenha o símbolo de graus (°) no LCD
    lcd.print("C ");

    // Exibe a Umidade
    lcd.print("Umidt:");
    lcd.print(umidade, 1);
    lcd.print("%");
  }
}

// FUNÇÃO PARA SALVAR OS DADOS
void salvarDadosEEPROM() {
  int qtdRegistros;
  EEPROM.get(enderecoContador, qtdRegistros);
  
  // Se a EEPROM estiver limpa, o valor lido será -1 ou 65535. Resetamos para 0.
  if (qtdRegistros < 0 || qtdRegistros > 125) {
    qtdRegistros = 0;
  }

  // Calcula a posição do próximo slot livre na memória
  int posicaoEscrita = enderecoAtual + (qtdRegistros * sizeof(Registro));

  // Verifica se ainda há espaço na memória do Arduino Uno (1024 bytes)
  if (posicaoEscrita + sizeof(Registro) > 1024) {
    lcd.setCursor(0, 3);
    lcd.print("EEPROM Cheia!       ");
    delay(1500);
    return;
  }

  // Coleta os dados
  DateTime agora = rtc.now();
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) return; // Evita salvar leituras erradas

  Registro novoRegistro;
  novoRegistro.timestamp = agora.unixtime(); // Salva a data de forma compacta
  novoRegistro.temp = (int16_t)(temperatura * 10); // Ex: 25.4°C vira 254
  novoRegistro.umid = (int16_t)(umidade * 10);     // Ex: 62.1% vira 621

  // Grava na EEPROM
  EEPROM.put(posicaoEscrita, novoRegistro);

  // Atualiza o contador de registros salvos
  qtdRegistros++;
  EEPROM.put(enderecoContador, qtdRegistros);

  // Feedback visual rápido no LCD (Linha 4)
  lcd.setCursor(0, 3);
  lcd.print("Salvo no Log #");
  lcd.print(qtdRegistros);
  lcd.setCursor(0, 3);
  delay(1500);
}
