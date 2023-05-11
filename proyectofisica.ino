#include "HX711.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "Adafruit_VL53L0X.h"
  
//Define las variables globales
byte DT=5;
byte CLK=4;
byte modo = 7;
byte tara = 6;
float radio = 4.4;
float peso;
float peso2;
float volumen;
float volumen2;
float volumenT;
float densidadIrr;


int peso_conocido[4] = {500,1000,3000,5000};
long escala;
Adafruit_VL53L0X lox = Adafruit_VL53L0X(); 
//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  
//Crear el objeto balanza
HX711 balanza;
//Función de Anti-debounce (Evitar el rebote del pulsador)
void anti_debounce(byte boton){
      delay(100);
      while(digitalRead(boton)); //Anti-Rebote
      delay(100);
}
//Función de calibración y ajuste
void calibration(){
  
  int i = 0,cal=1;
  long adc_lecture;
  
  // Escribimos el Mensaje en el LCD
  lcd.setCursor(2, 0);
  lcd.print("Calibracion de");
  lcd.setCursor(4, 1);
  lcd.print("Balanza");
  delay(1500);
  balanza.read();
  balanza.set_scale(); //La escala por defecto es 1
  balanza.tare(0);  //El peso actual es considerado Tara.
  
  lcd.clear();
  //Inicia el proceso de ajuste y calibración
  while(cal == 1){
    
    lcd.setCursor(1, 0);
    lcd.print("Peso Conocido:");
    lcd.setCursor(1, 1);
    lcd.print(peso_conocido[i]);
    lcd.print(" g        ");
    //Busca el peso conocido con el botón tara
    if(digitalRead(tara)){  
      anti_debounce(tara);
      i =(i>2) ? 0:i+1; //if-else en una linea
    }
    //Selecciona el peso conocido con el boton modo
    if(digitalRead(modo)){
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Ponga el Peso");
      lcd.setCursor(1, 1);
      lcd.print("y espere ...");
      delay(2000);
      //Lee el valor del HX711
      adc_lecture = balanza.get_value(100);
      //Calcula la escala con el valor leido dividido el peso conocido
      escala = adc_lecture / peso_conocido[i];
      //Guarda la escala en la EEPROM
      EEPROM.put( 0, escala );
      delay(100);
      cal = 0; //Cambia la bandera para salir del while
      lcd.clear();
    }
    
  }
}
//Función de peso, distancia volumen y densidad
void clasBalanza(){
  //Mide el peso de la balanza
    peso = balanza.get_units(10);
    //Muestra el resultado
    lcd.setCursor(1, 0);
    lcd.print("Peso: ");
    lcd.print(peso,0);
    lcd.println(" g        ");
    delay(10000);
    //Botón de Tara
      if(digitalRead(tara)){  
        anti_debounce(tara);
        balanza.tare(10);  //El peso actual es considerado Tara.
      }
}
void clasDistancia(){
  lcd.clear();
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

    if (measure.RangeStatus != 4) {
    lcd.setCursor(2,0);
    lcd.print(measure.RangeMilliMeter);
    lcd.setCursor(5,0);
    lcd.print("mm");
    lcd.setCursor(2,1);
    lcd.print(measure.RangeMilliMeter/10);
    lcd.setCursor(4,1);
    lcd.print("cm");
  } else {
    lcd.print("fuera alcanze");
  }
  
  lcd.setCursor(5,0);
  lcd.print("mm");
  delay(10000);
}

void clasVolumen(){
  lcd.clear();
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  lcd.setCursor(4,0);
  lcd.print("Volumen:");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0, 1);  // Se posiciona en la segunda línea de la pantalla LCD
  volumen = PI * radio * radio * measure.RangeMilliMeter/10;  // Calcula el volumen del cilindro
  lcd.print(volumen);  // Muestra el volumen en la pantalla LCD
  lcd.print(" cm^3");  // Muestra la unidad en la pantalla LCD
  delay(10000);
}

void clasDensidad(){
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("densidad:");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0,1 );  // Se posiciona en la segunda línea de la pantalla LCD
  float densidad = peso / volumen;  // Calcula el la densidad
  lcd.print(densidad);
  lcd.print(" g/cm^3");  // Muestra el volumen en la pantalla LCD
  delay(10000);
}

void classObjetoIrregular(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese objeto");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0, 1);  // Se posiciona en la segunda línea de la pantalla LCD
  lcd.print("IRREGULAR");  // Muestra el mensaje en la pantalla LCD
}

//funcion de objeto regular
void classBalanzaObjetoIrregular(){
  lcd.clear();
  //Mide el peso de la balanza
    peso2 = balanza.get_units(10);
    //Muestra el resultado
    lcd.setCursor(1, 0);
    lcd.print("Peso: ");
    lcd.print(peso2,0);
    lcd.println(" g        ");
    delay(10000);
    //Botón de Tara
      if(digitalRead(tara)){  
        anti_debounce(tara);
        balanza.tare(10);  //El peso actual es considerado Tara.
      }
}
void classVolumenObjetoIrregular(){
  lcd.clear();
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  lcd.setCursor(4,0);
  lcd.print("Volumen R:");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0, 1);  // Se posiciona en la segunda línea de la pantalla LCD
  volumen2 = PI * radio * radio * measure.RangeMilliMeter/10;  // Calcula el volumen del objeto irregular del cilindro
  lcd.print(volumen);  // Muestra el volumen en la pantalla LCD
  lcd.print(" cm^3");  // Muestra la unidad en la pantalla LCD
  delay(10000);
}

void volumenIrregularTotal(){
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Volumen T:");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0, 1);  // Se posiciona en la segunda línea de la pantalla LCD
  volumenT = volumen2 - volumen;  // Calcula el volumen total del primero menos el segundo irregular 
  lcd.print(volumen);  // Muestra el volumen en la pantalla LCD
  lcd.print(" cm^3");  // Muestra la unidad en la pantalla LCD
  delay(10000);
}

void densidadObjetoIrregular(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("densidad OB IRR:");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0,1 );  // Se posiciona en la segunda línea de la pantalla LCD
  densidadIrr = peso2 / volumenT;  // Calcula la densidad del cilindro
  lcd.print(densidadIrr);
  lcd.print(" g/cm^3");  // Muestra el volumen en la pantalla LCD
  delay(10000);
}

void masaObjectoIrregular(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("masa OB IRR:");  // Muestra el mensaje en la pantalla LCD
  lcd.setCursor(0,1 );  // Se posiciona en la segunda línea de la pantalla LCD
  float masa = volumenT * densidadIrr;  // Calcula la masa del cilindro
  lcd.print(masa);
  lcd.print(" g");  // Muestra la unidad de medida de la masa en la pantalla LCD
  delay(10000);
}



void setup() {

  if (!lox.begin()) {
    while(1);
  }
  //Configura la balanza
  balanza.begin(DT, CLK);
  
  //Configura los botones
  
  pinMode(modo, INPUT);
  pinMode(tara, INPUT);
  //Pregunta si se desea entrar en calibración
  if(digitalRead(modo) && digitalRead(tara)){
    calibration();
  }
  lcd.init();

  //Encender la luz de fondo.
  lcd.backlight();
    //Lee el valor de la escala en la EEPROM
    EEPROM.get( 0, escala );
    //Mensaje inicial en el LCD
      lcd.setCursor(1, 0);
      lcd.print("Retire el Peso");
      lcd.setCursor(1, 1);
      lcd.print("y Espere");
      delay(2000);
      balanza.set_scale(escala); // Establecemos la escala
      balanza.tare(20);  //El peso actual es considerado Tara.
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Listo....");  
      delay(100);
      lcd.clear(); 
      delay(100);
  
}

void loop() {
    
    clasBalanza();
      delay(10000);
    clasDistancia();
      delay(10000);

    clasVolumen();
      delay(10000);

    clasDensidad();
      delay(10000);

    classObjetoIrregular();
      delay(10000);
    //Llamamos a la clases para el objeto Irregular

    classBalanzaObjetoIrregular();
      delay(10000);

    classVolumenObjetoIrregular();
      delay(10000);

    volumenIrregularTotal();
      delay(10000);

    densidadObjetoIrregular();
      delay(10000);

    masaObjectoIrregular();
      delay(10000);

    lcd.clear();
}