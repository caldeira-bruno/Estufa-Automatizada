#include <LiquidCrystal.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

// Definições do display LCD
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#define DHTPIN A1 // Porta do Sensor de Temperatura no A1
#define DHTTYPE DHT11 // Tipo do Sensor de Temperatura
DHT_Unified dht(DHTPIN, DHTTYPE); 
float temperatura = 0;
float umidadeAr = 0;


#define SERVOPIN 10 // Porta do Servo Motor
Servo myservo; // Servo Motor


int pinoLedLuminosidade = 6; // Porta do LED para Sensor de Luminosidade
int valorLuz = A0; // Porta para o Valor do Sensor de Luminosidade
#define pino_sinal_analogico A2 // Sensor de Umidade do Solo


int valor_analogico; // Leitura do Sensor de Umidade do Solo


//int RELEMOTOR = 7; // Relé do Motor
int RELEFAN = 8; // Relé do Ventilador


bool estadoVentoinha = false;
bool estadoServo = false;

void setup() {
    Serial.begin(9600);

    pinMode(pinoLedLuminosidade, OUTPUT);
    //pinMode(RELEMOTOR, OUTPUT);
    pinMode(RELEFAN, OUTPUT);
    pinMode(DHTPIN, INPUT);

    myservo.attach(SERVOPIN);
    myservo.write(0); // Inicia o motor na posição 0

    // Inicializa o LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BEM-");
    delay(1000);
    lcd.setCursor(8, 1);
    lcd.print("VINDO...");
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("PROJETO");
    delay(1000);
    Serial.println("PROJETO ESTUFA");
    lcd.setCursor(10, 1);
    lcd.print("ESTUFA");
    delay(2000);
    lcd.clear();

    for (int i = 0; i <= 15; i++) {
    lcd.setCursor(0, 0);
    lcd.print("Carregando...");
    lcd.setCursor(i, 1);
    lcd.print("*");
    delay (200);
    lcd.clear();
    }

    dht.begin(); // Inicializa o sensor DHT
}


void montaTelaSensores() {

    // Exibição dos dados no display LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Umid: ");
    lcd.print(umidadeAr);
    lcd.print(" %");
    
    delay(3000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Luz: ");
    if (valorLuz < 800) {
       lcd.print("ON");
    } else {
       lcd.print("OFF");
    }
    //lcd.print(valorLuz);
    
    // Exibe o estado do solo no LCD
    lcd.setCursor(0, 1);
    lcd.print("Solo: ");
    if (valor_analogico < 300) {
        lcd.print("Aguado");
    } else if (valor_analogico < 700) {
        lcd.print("Umido");     
    } else {
        lcd.print("Seco");
    }
    
    delay(3000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ventoinha: ");
    lcd.print(estadoVentoinha ? "ON" : "OFF");


    lcd.setCursor(0, 1);
    lcd.print("Servo: ");
    lcd.print(estadoServo ? "75 graus" : "0 graus");

    delay(3000);
}


void loop() {

    // Leitura do sensor de luminosidade
    valorLuz = analogRead(A0);

    // Exibição dos dados no Serial Monitor
    Serial.print("Luminosidade: ");
    Serial.println(valorLuz);

    if (valorLuz < 800) {
        digitalWrite(pinoLedLuminosidade, HIGH);
    } else {
        digitalWrite(pinoLedLuminosidade, LOW);
    }

    // Leitura do sensor de umidade do solo
    valor_analogico = analogRead(pino_sinal_analogico);
    
    Serial.print("Umidade do Solo: ");
    Serial.println(valor_analogico);

    // Leitura do sensor de temperatura e umidade
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature)) {
        temperatura = event.temperature;
        Serial.print("Temperatura: ");
        Serial.print(temperatura);
        Serial.println(" *C");
    } else {
        Serial.println("Erro ao ler a temperatura.");
    }

    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity)) {
        umidadeAr = event.relative_humidity;
        Serial.print("Umidade: ");
        Serial.print(umidadeAr);
        Serial.println("%");
    } else {
        Serial.println("Erro ao ler a umidade.");
    }

    // Controle do relé e do servo baseado na temperatura
    if (temperatura >= 23) {
        digitalWrite(RELEFAN, LOW);  // Ligar ventilador
        //digitalWrite(RELEMOTOR, LOW); // Ligar motor
        myservo.write(0); // Define a posição atual do servo
        estadoVentoinha = true;
        estadoServo = true;
    } else {
        digitalWrite(RELEFAN, HIGH);  // Desligar ventilador
        //digitalWrite(RELEMOTOR, HIGH); // Desligar motor
        myservo.write(75); // Define a posição atual do servo
        estadoVentoinha = false;
        estadoServo = false;
    }

    // Exibe os dados no LCD
    montaTelaSensores();
}