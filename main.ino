#define BLYNK_TEMPLATE_ID "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Defina os pinos para o sensor ultrassônico
#define echoPin 13  // Pino de eco
#define trigPin 14  // Pino de trigger

// Defina o pino do termistor (KY-013)
const int ThermistorPinKY013 = 32; // Pino conectado ao A0 do KY-013
float R1_KY013 = 10000.0; // Valor do resistor R1 no módulo KY-013
float logR2_KY013, R2_KY013, T_KY013;
float c1_KY013 = 1.009249522e-03, c2_KY013 = 2.378405444e-04, c3_KY013 = 2.019202697e-07; // Coeficientes Steinhart-Hart KY-013

// Calibração da temperatura (se necessário)
float tempCalibrationOffset = 0.0; // Ajuste este valor para calibrar a leitura da temperatura

// Defina o pino do servo
const int servoPin = 33;
// Crie um objeto servo
Servo myServo;

// Credenciais Wi-Fi
char ssid[] = "WIFI NAME";   // Substitua pelo SSID da sua rede Wi-Fi
char pass[] = "PASSWORD";  // Substitua pela senha da sua rede Wi-Fi

// Token de autenticação do Blynk
char auth[] = BLYNK_AUTH_TOKEN;

long duration, distance;
float percentage;

// Variáveis para controle do temporizador
unsigned long lastServoMoveTime = 0;
unsigned long timerInterval = 720 * 60 * 1000; // 12 horas em milissegundos (inicializado com 720 minutos)
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 2000; // 2 segundos em milissegundos

// Variável global para armazenar o valor do timer do Blynk
int blynkTimerValue = 720; // Inicializa com o valor máximo (720 minutos)

// Endereço I2C do LCD encontrado
LiquidCrystal_I2C lcd(0x27, 16, 2); // endereço, colunas, linhas

void setup() {
  // Inicialize a comunicação serial
  Serial.begin(115200);

  // Inicialize os pinos do sensor ultrassônico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Conecte-se ao Wi-Fi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Inicialize o Blynk
  Blynk.begin(auth, ssid, pass);

  // Inicialize o servo
  myServo.attach(servoPin);
  myServo.write(0); // Coloque o servo na posição inicial (0 graus)
  Serial.println("Servo inicializado na posição 0 graus");

  // Mensagem de debug
  Serial.println("Conectado ao Blynk");

  // Inicializa o timer no Blynk com o valor inicial
  Blynk.virtualWrite(V2, blynkTimerValue);

  // Inicialize o LCD
  Wire.begin(); // Inicia a biblioteca Wire
  lcd.init();   // Inicializa o LCD
  lcd.backlight(); // Liga a luz de fundo do LCD
  lcd.setCursor(0, 0); // Define o cursor para a primeira coluna da primeira linha
  lcd.print("Sistema Iniciado"); // Mensagem inicial no LCD
}

void loop() {
  Blynk.run(); // Mantenha a conexão Blynk viva

  // Atualize a cada 2 segundos
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();
    
    // Leia a temperatura do termistor KY-013
    int Vo_KY013 = analogRead(ThermistorPinKY013); // Leitura analógica do sensor KY-013
    R2_KY013 = R1_KY013 * (4095.0 / (float)Vo_KY013 - 1.0); // Calcula a resistência do termistor KY-013
    if (R2_KY013 > 0) {
      logR2_KY013 = log(R2_KY013);
      T_KY013 = 1.0 / (c1_KY013 + c2_KY013 * logR2_KY013 + c3_KY013 * logR2_KY013 * logR2_KY013 * logR2_KY013); // Temperatura em Kelvin
      T_KY013 = T_KY013 - 273.15; // Converte Kelvin para Celsius
      T_KY013 += tempCalibrationOffset; // Aplica a calibração de temperatura

      // Exibe a temperatura do KY-013 no monitor serial
      Serial.print("Temperature: ");
      Serial.print(T_KY013);
      Serial.println(" C");
    } else {
      // Caso a resistência calculada não seja válida, exiba um aviso
      Serial.println("Invalid resistance reading KY-013!");
    }

    // Leia a distância do sensor ultrassônico
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration / 58.2;

    // Calcular a porcentagem de ração
    if (distance < 5) distance = 5;   // Limita a leitura mínima a 5 cm
    if (distance > 12) distance = 12; // Limita a leitura máxima a 12 cm
    percentage = ((12 - distance) / 7.0) * 100; // Calcula a porcentagem com base nos limites 5 cm (100%) e 12 cm (0%)

    // Exibe a distância e temperatura no monitor serial
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.print("Percentage: ");
    Serial.print(percentage);
    Serial.println(" %");

    // Atualiza o display LCD
    lcd.clear(); // Limpa o LCD
    lcd.setCursor(0, 0); // Define o cursor para a primeira coluna da primeira linha
    lcd.print("Temp: "); // Escreve "Temp: " no LCD
    lcd.print(T_KY013); // Escreve a temperatura no LCD
    lcd.print(" C"); // Escreve " C" no LCD
    lcd.setCursor(0, 1); // Define o cursor para a primeira coluna da segunda linha
    lcd.print("Comida: "); // Escreve "Comida: " no LCD
    lcd.print(percentage); // Escreve a porcentagem de comida no LCD
    lcd.print(" %"); // Escreve " %" no LCD

    // Envie os valores ao Blynk
    Blynk.virtualWrite(V3, T_KY013);     // Envia temperatura em C do KY-013 para o Virtual Pin V3
    Blynk.virtualWrite(V5, percentage);  // Envia a porcentagem de ração para o Virtual Pin V5

    // Atualize o display do Blynk com o tempo restante
    unsigned long timeRemaining = (timerInterval - (millis() - lastServoMoveTime)) / 60000; // Converter para minutos
    Blynk.virtualWrite(V4, timeRemaining);
  }

  // Controle do servo para feeder automático
  if (millis() - lastServoMoveTime >= timerInterval) {
    moveServo();
    lastServoMoveTime = millis();
  }

  // Verifica se o timer chegou a zero para reiniciar
  if (millis() - lastServoMoveTime >= timerInterval && timerInterval == 0) {
    blynkTimerValue = 720; // Reinicia o timer para 720 minutos
    timerInterval = blynkTimerValue * 60 * 1000; // Converte para milissegundos
    Blynk.virtualWrite(V2, blynkTimerValue); // Atualiza o valor no Blynk
  }
}

// Função chamada quando o valor do widget no app Blynk é alterado
BLYNK_WRITE(V2) {
  int newTimerValue = param.asInt(); // Pega o valor do controle de tempo no app Blynk (em minutos)
  if (newTimerValue > 0 && newTimerValue <= 720) {
    blynkTimerValue = newTimerValue; // Armazena o novo valor do timer
    timerInterval = blynkTimerValue * 60 * 1000; // Atualiza o timerInterval com o novo valor
    lastServoMoveTime = millis(); // Reinicia o tempo do último movimento do servo
  }
}

// Função chamada quando o botão do servo é pressionado
BLYNK_WRITE(V6) {
  moveServo();
  lastServoMoveTime = millis(); // Reseta o timer
}

void moveServo() {
  // Gira o servo para 0 graus
  myServo.write(0);
  Serial.println("Servo movido para 0 graus");

  // Espera um curto período de tempo para o servo se mover
  delay(500);

  // Gira o servo para 120 graus
  myServo.write(120);
  Serial.println("Servo movido para 120 graus");

  // Espera 1.2 segundos
  delay(1200);

  // Volta o servo para a posição inicial (0 graus)
  myServo.write(0);
  Serial.println("Servo voltado para 0 graus");

  // Define o novo intervalo do timer com base no valor do Blynk
  timerInterval = blynkTimerValue * 60 * 1000; // Converte para milissegundos
}
