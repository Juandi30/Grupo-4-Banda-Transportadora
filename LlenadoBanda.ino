#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
//#include <Stepper.h>

int incremento=0;
LiquidCrystal_I2C lcd(0x27, 20, 4);
const int stepsPerRevolution = 200;
//Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

//SENSOR PROXIMIDAD
#define SENSOR_INFRA_1 13
#define SENSOR_INFRA_2 2
int estadoinfra1 =0;
int estadoinfra2 =0;

//ENCODER
#define outputA 6
#define outputB 7
#define sw 5

///ultrasonico

#define TRIGER 4
#define ECHO 3
int cm = 0;
int Duracion1, Nivel_1;
int   Nivel1 = 0; // variable para lecturas de nivel
int   Nivel2 = 0; // variable para lecturas de nivel
int   NivelR = 0; // variable para lecturas de nivel
void Nivel()
{
  // SENSOR_TEST();//Testear Nivel
  digitalWrite(TRIGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGER, HIGH);
  Duracion1 = pulseIn(ECHO, HIGH);
  Nivel_1 = (Duracion1 / 2) / 19;
  Nivel1 = Nivel_1 - 23.5; // resta la altura maxima de la real
  Nivel2 = pow(Nivel1, 2); // elimina valor negativo
  NivelR = sqrt(Nivel2); //recupera valor medida
  delay(100);
}

void readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  Duracion1 =  pulseIn(echoPin, HIGH);
  Nivel_1 = (Duracion1 / 2) / 29.1;
  Nivel1 = Nivel_1 - 19; // resta la altura maxima de la real
  Nivel2 = pow(Nivel1, 2); // elimina valor negativo
  NivelR = sqrt(Nivel2); //recupera valor medida
}

// Botones
#define Subir 11
#define Bajar 10
int estado_0;
int estado_1;
int estado_0a;
int estado_1a;

//////BOMBA
#define bomba 12

///// Motor
int velocidad =0;
#define mt 8


int sensorValue = 0;

int aState;
int aLastState;  

int aState_2;
int aLastState_2;  

int pos = 0;

int nivel_agua = 0;
int option = 0;
int ejecutar =0;


LiquidLine linea1(0, 0, "------- MENU -------");
LiquidLine linea2(1, 1, "Automatico");
LiquidLine linea3(1, 2, "Manual");
LiquidLine linea4(1, 3, "Prueba Banda");
LiquidScreen pantalla1(linea1,linea2,linea3,linea4);

LiquidLine linea1_2(0, 0, "---- AUTOMATICO ----");
LiquidLine linea2_2(1, 1, "Aceptar");
LiquidLine linea3_2(1, 2, "Atras");
LiquidScreen pantalla2(linea1_2,linea2_2,linea3_2);

LiquidLine linea1_3(0, 0, "------ MANUAL ------");
LiquidLine linea2_3(0, 1, "Niv. Liq cm: ",nivel_agua );
LiquidLine linea3_3(1, 2, "Aceptar");
LiquidLine linea4_3(1, 3, "Atras");
LiquidScreen pantalla3(linea1_3,linea2_3,linea3_3,linea4_3);


LiquidMenu menu(lcd,pantalla1,pantalla2,pantalla3);



void setup() {
  Serial.begin(9600);
  ///Botones
  pinMode(Subir,INPUT);
  pinMode(Bajar,INPUT);
  estado_0a=digitalRead(Subir);
  estado_1a=digitalRead(Bajar);

  //SENSOR PROXIMIDAD
  pinMode(SENSOR_INFRA_1, INPUT_PULLUP);// define pin as Input  sensor
  pinMode(SENSOR_INFRA_2, INPUT_PULLUP);// define pin as Input  sensor

  //ULtrasosido
  //pinMode(4, OUTPUT); //salida del pulso generado por el sensor ultrasónico
  //pinMode(3, INPUT);//entrada del pulso generado por el sensor ultrasónico

  //BOMBA
  pinMode(bomba, OUTPUT);
  digitalWrite(bomba, HIGH);
  //////
  //mt
  pinMode(mt, OUTPUT);
  digitalWrite(mt, HIGH);
  //////
  
  pinMode(sw,INPUT_PULLUP);

  
//  myStepper.setSpeed(100);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5,1);
  lcd.print("BIENVENIDO");
  lcd.setCursor(2,2);
  lcd.print("LLENADO - ENVASE");
  lcd.setCursor(6,3);
  lcd.print("GRUPO #4");
  delay(2000);
  
  menu.init();

  //Pantalla Inicial 1
  linea2.set_focusPosition(Position::LEFT); 
  linea3.set_focusPosition(Position::LEFT); 
  linea4.set_focusPosition(Position::LEFT); 

  linea2.attach_function(1, screen_2); 
  linea3.attach_function(1, screen_3);
  linea4.attach_function(1, chequeo);

  menu.add_screen(pantalla1);

  //Pantalla Secundaria Primera Opcion Automatico
  linea2_2.set_focusPosition(Position::LEFT); 
  linea3_2.set_focusPosition(Position::LEFT); 
  
  linea2_2.attach_function(1, Aceptar_1); 
  linea3_2.attach_function(1, Atras);

  menu.add_screen(pantalla2);

  //Pantalla Secundaria Segunda Opcion Manual
  linea2_3.set_focusPosition(Position::LEFT); 
  linea3_3.set_focusPosition(Position::LEFT);
  linea4_3.set_focusPosition(Position::LEFT);

  //linea2_3.attach_function(1, Aceptar_3);
  linea3_3.attach_function(1, Aceptar_2); 
  linea4_3.attach_function(1, Atras);

  menu.add_screen(pantalla3);



  pantalla1.set_displayLineCount(4);
  pantalla2.set_displayLineCount(4);
  pantalla3.set_displayLineCount(4);

  menu.set_focusedLine(1);
  menu.update();
}

void loop() {
  //Nivel();
  int sensor =digitalRead(SENSOR_INFRA_1);
  int sensor2 =digitalRead(SENSOR_INFRA_2);
  //cm = 0.01723 * readUltrasonicDistance(TRIGER, ECHO);
  readUltrasonicDistance(TRIGER, ECHO);
  Serial.println(NivelR);
  if(option==2)
  {
    Mod_CantAdgua();
    if(option ==2 && ejecutar ==2)
    {
    
      //Serial.println("Dentroooo");
      //nivel_agua = 10;
    
      if(sensor == 0)
      {
        estadoinfra1=1; 
        //Serial.println("Lect Infra 1");     
      }
      if(estadoinfra1==1)
      {
        //Serial.println("Enciendo MOtor");
    //      myStepper.setSpeed(120);
    //      myStepper.step(stepsPerRevolution / 100);
        digitalWrite(mt, LOW);
        
      }
      if((sensor2==0)&& (estadoinfra1==1) )
      {
        //Serial.println("Lect Infra 2");
        //estadoinfra1=0;
    //      myStepper.setSpeed(0);
    //      myStepper.step(stepsPerRevolution / 100);
        //delay(100);
        //Serial.println("Stop");
        
        digitalWrite(mt, HIGH);
        //delay(5000);
        digitalWrite(bomba, LOW);
        
        //Serial.println(dis);
        
    //      digitalWrite(4,LOW);//recibimiento del pulso.
    //      delayMicroseconds(5);
    //      digitalWrite(4, HIGH);//envió del pulso.
    //      delayMicroseconds(10);
    //      tiem=pulseIn(3, HIGH);//fórmula para medir el pulso entrante.
    //      dis= long(0.017*tiem);//fórmula para calcular la distancia del sensor ultrasónico.
        
        //Serial.println(cm);
    //      if((cm<=nivel_agua)&&(sensor2==0))
    //      {
          delay(nivel_agua*1000);
          //Serial.print("Nivel Agua");
          //Serial.println(dis);
          digitalWrite(bomba, HIGH);
          delay(2000);
          digitalWrite(mt, LOW);
          
          estadoinfra1=0;
          delay(15000);
          digitalWrite(mt, HIGH);
    //      } 
        //delay(20000);
       // digitalWrite(mt, HIGH);
    //      myStepper.setSpeed(120);
    //      myStepper.step(stepsPerRevolution / 100);  
      }
     
    }
  }

  if(option ==1 && ejecutar ==1)
  {

    //Serial.println("Dentroooo");
    nivel_agua = 10;

    if(sensor == 0)
    {
      estadoinfra1=1; 
      //Serial.println("Lect Infra 1");     
    }
    if(estadoinfra1==1)
    {
      //Serial.println("Enciendo MOtor");
//      myStepper.setSpeed(120);
//      myStepper.step(stepsPerRevolution / 100);
      digitalWrite(mt, LOW);
      
    }
    if((sensor2==0)&& (estadoinfra1==1) )
    {
      //Serial.println("Lect Infra 2");
      //estadoinfra1=0;
//      myStepper.setSpeed(0);
//      myStepper.step(stepsPerRevolution / 100);
      //delay(100);
      //Serial.println("Stop");
      
      digitalWrite(mt, HIGH);
      //delay(5000);
      digitalWrite(bomba, LOW);
      
      //Serial.println(dis);
      
//      digitalWrite(4,LOW);//recibimiento del pulso.
//      delayMicroseconds(5);
//      digitalWrite(4, HIGH);//envió del pulso.
//      delayMicroseconds(10);
//      tiem=pulseIn(3, HIGH);//fórmula para medir el pulso entrante.
//      dis= long(0.017*tiem);//fórmula para calcular la distancia del sensor ultrasónico.
      
      //Serial.println(cm);
//      if((cm<=nivel_agua)&&(sensor2==0))
//      {
        delay(15000);
        //Serial.print("Nivel Agua");
        //Serial.println(dis);
        digitalWrite(bomba, HIGH);
        delay(2000);
        digitalWrite(mt, LOW);
        
        estadoinfra1=0;
        delay(15000);
        digitalWrite(mt, HIGH);
//      } 
      //delay(20000);
     // digitalWrite(mt, HIGH);
//      myStepper.setSpeed(120);
//      myStepper.step(stepsPerRevolution / 100);  
    }
   
  }
  
  
  selectOption();
  
  
  aState = digitalRead(outputA); 
  if (aState != aLastState){     
    if (digitalRead(outputB) != aState) { 
      incremento++;
      if(incremento>=1){
        incremento = 0;
        menu.switch_focus(false);
      }
    } else {
      incremento++;
      if(incremento>=1){
        incremento = 0;
        menu.switch_focus(true);
      }
      
    }
    menu.update();
    aLastState = aState;
  }
}

//Funciones:::::
void selectOption(){
  if(digitalRead(sw) == LOW){
      menu.call_function(1);
      delay(500);
  }
}


void screen_2()
{
  menu.change_screen(2);
  menu.set_focusedLine(1);
  option = 1;
}

void screen_3()
{
  menu.change_screen(3);
  menu.set_focusedLine(2);
  option = 2;
}



void chequeo ()
{
  menu.change_screen(3);
  menu.set_focusedLine(1);
  option = 3;
}

void Atras(){
  menu.change_screen(1);
  menu.set_focusedLine(1);
  option = 0;
  ejecutar =0;
}

void Aceptar_1()
{
  ejecutar = 1;
}
void Aceptar_2()
{
  ejecutar = 2;
}

void Mod_CantAdgua()
{
  estado_0 = digitalRead(Subir);
  estado_1 = digitalRead(Bajar);
  int Boton_Subir = digitalRead(Subir);
  if((estado_0== 0)&&(estado_0a== 1))
  {
    if((nivel_agua>=0) && (nivel_agua<15))
    {
      nivel_agua++;
      menu.update();
    }
    
  }
  int Boton_Bjar = digitalRead(Bajar);
  if((estado_1== 0)&&(estado_1a== 1))
  {
    if((nivel_agua>0) && (nivel_agua<=15))
    {
      nivel_agua--;
      menu.update();
    }

  }
  estado_0a=estado_0;
  estado_1a=estado_1;
}



void Ejec_action1()
{
//  myStepper.step(-950);
//  delay(500);
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//                  // tell servo to go to position in variable 'pos'
//    delay(15);                        // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//                 // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  delay(500);
//  myStepper.step(400);
//  delay(500);
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//                 // tell servo to go to position in variable 'pos'
//    delay(15);                        // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//                  // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  delay(500);
//  myStepper.step(550);
//  delay(500);
}

void Ejec_action2()
{
//  myStepper.step(-950);
//  delay(500);
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//                  // tell servo to go to position in variable 'pos'
//    delay(15);                        // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//                 // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  delay(500);
//  myStepper.step(770);
//  delay(500);
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//                 // tell servo to go to position in variable 'pos'
//    delay(15);                        // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//                  // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  delay(500);
//  myStepper.step(180);
//  delay(500);
}
