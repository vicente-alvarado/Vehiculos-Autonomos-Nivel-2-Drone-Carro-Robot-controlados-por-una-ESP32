/*
Por Vicente Alvarado
*/

#include <AFMotor.h>
#include <Servo.h>
#include <SoftwareSerial.h>  // Librería para la comunicación serial por software

// Pines de SoftwareSerial con la esp32
SoftwareSerial esp32Serial(A0, A1);  // RX, TX

// Pines iniciales de los motores
AF_DCMotor motor1(1, MOTOR12_1KHZ); //Frontal derecha
AF_DCMotor motor2(2, MOTOR12_1KHZ); //Frontal izquierda
AF_DCMotor motor3(3, MOTOR34_1KHZ); //Trasera izquierda
AF_DCMotor motor4(4, MOTOR34_1KHZ); //Trasera derecha

char command;
unsigned long lastCommandTime = 0;  // Tiempo en que se recibió el último comando
const unsigned long timeOut = 200;  // Tiempo máximo (0.2 segundos) sin recibir comandos

Servo myServo9;   // Servo para el griper de liberación del drone
Servo myServo10;  // Servo para controlar el Sensor ultrasonico

void setup() {
  // Configurar comunicación serial por software
  esp32Serial.begin(9600);  // Para recibir comandos desde la ESP32
  Serial.begin(9600);       // Para depuración en el monitor serial

  myServo9.attach(9);
  myServo10.attach(10);

  myServo9.write(90);   // Posición inicial del servo
  myServo10.write(90);  // Posición inicial del servo
}

void loop() {
  // Verificar si hay datos en la comunicación serial por software con la esp32
  if (esp32Serial.available() > 0) {
    command = esp32Serial.read();  // Leer comando recibido

    // Actualizar el tiempo del último comando recibido
    lastCommandTime = millis();

    Stop();  // Inicializar motores detenidos

    switch (command) {
      case 'F':  // Adelante
        forward();
        break;
      case 'B':  // Atrás
        back();
        break;
      case 'L':  // Izquierda
        left();
        break;
      case 'R':  // Derecha
        right();
        break;
      case 'x':  // Mover servo hacia abajo grados:15
        moveServo9(15);
        break;
      case 'X':  // Centrar el servo grados:120
        moveServo9(140);
        break;
      case 'y':
        moveServo10(0);
        break;
      case 'Y':
        moveServo10(180);
        break;
    }

    // Mostrar el comando recibido en el monitor serial
    Serial.print("Comando recibido: ");
    Serial.println(command);
  }

  // Verificar si el tiempo ha excedido el timeout sin recibir comandos
  if (millis() - lastCommandTime > timeOut) {
    Stop();  // Detener los motores si no se recibe comando en 2 segundos
  }
}

void forward() {
  motor1.setSpeed(255);
  motor1.run(FORWARD);
  motor2.setSpeed(255);
  motor2.run(FORWARD);
  motor3.setSpeed(255);
  motor3.run(FORWARD);
  motor4.setSpeed(255);
  motor4.run(FORWARD);
}

void back() {
  motor1.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.setSpeed(255);
  motor2.run(BACKWARD);
  motor3.setSpeed(255);
  motor3.run(BACKWARD);
  motor4.setSpeed(255);
  motor4.run(BACKWARD);
}

void left() {
  motor1.setSpeed(255);
  motor1.run(FORWARD);
  motor2.setSpeed(255);
  motor2.run(BACKWARD);
  motor3.setSpeed(255);
  motor3.run(BACKWARD);
  motor4.setSpeed(255);
  motor4.run(FORWARD);
}

void right() {
  motor1.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.setSpeed(255);
  motor2.run(FORWARD);
  motor3.setSpeed(255);
  motor3.run(FORWARD);
  motor4.setSpeed(255);
  motor4.run(BACKWARD);
}

void Stop() {
  motor1.setSpeed(0);
  motor1.run(RELEASE);
  motor2.setSpeed(0);
  motor2.run(RELEASE);
  motor3.setSpeed(0);
  motor3.run(RELEASE);
  motor4.setSpeed(0);
  motor4.run(RELEASE);
}

void moveServo9(int angle) {
  myServo9.write(angle);  // Mover el servo al ángulo especificado
  delay(200);
}

void moveServo10(int angle) {
  myServo10.write(angle);  // Mover el servo al ángulo especificado
  delay(200);
}

// Función para controlar servos
void mapJoystick2Angle(String command) {
  int separatorIndex = command.indexOf(':');  // Encontrar la posición de los dos puntos ':'

  if (separatorIndex > 1) {                                         // Asegurarse de que haya algo antes y después del ':'
    int servoIndex = command.substring(1, separatorIndex).toInt();  // Extraer el número del servo
    int angle = command.substring(separatorIndex + 1).toInt();      // Extraer el ángulo

    // Validar el número de servo (9 o 10)
    if (servoIndex == 9 || servoIndex == 10) {
      // Validar el rango del ángulo (0-180)
      if (angle >= 0 && angle <= 180) {
        if (servoIndex == 9) {
          myServo9.write(angle);                                 // Mover servo 9
          Serial.println("Servo 9 moved to: " + String(angle));  // Confirmación
        } else if (servoIndex == 10) {
          myServo10.write(angle);                                 // Mover servo 10
          Serial.println("Servo 10 moved to: " + String(angle));  // Confirmación
        }
      } else {
        Serial.println("Invalid angle!");  // Error si el ángulo es inválido
      }
    } else {
      Serial.println("Invalid servo number!");  // Error si el número de servo es inválido
    }
  } else {
    Serial.println("Invalid command format!");  // Error si el formato es incorrecto
  }
}
