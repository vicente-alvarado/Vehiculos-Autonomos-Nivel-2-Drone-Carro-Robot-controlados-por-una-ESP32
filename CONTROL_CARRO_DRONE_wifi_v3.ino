#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <WiFiUdp.h>

// Configuración de Wi-Fi del drone TELLO
//const char* ssid = "TELLO-993282";
//const char* password = "";

const char* ssid = "TELLO-993282";
const char* password = "";

// Configuración de Wi-Fi para modo Access Point
const char* ssidAP = "VehiculosESP32";
const char* passwordAP = "Vehiculos123";

IPAddress ip(192, 168, 100, 208);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

// Comunicación UDP para el dron
WiFiUDP udp;
const char* droneIP = "192.168.10.1";
const int dronePort = 8889;

// Comunicación Serial para el robot
#define RX_PIN 16
#define TX_PIN 17
HardwareSerial SerialArduino(1);

// Servidor web
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  SerialArduino.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Conectar al Wi-Fi del Tello
  WiFi.begin(ssid, password, 8);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado al Tello.");

  // Crear Access Point
  WiFi.softAP(ssidAP, passwordAP);
  WiFi.softAPConfig(ip, gateway, subnet);
  Serial.println("Access Point creado.");
  Serial.println("Servidor: http://192.168.100.208");

  // Página web principal
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String htmlContent = R"rawliteral(<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>PS5 Gamepad Interface</title>
  <style>
    body {
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      background-color: #121212;
      color: white;
      font-family: Arial, sans-serif;
      margin: 0;
    }

    #automaticMode {
      display: block;
      font-weight: bold;
      color: red;
    }

    #gamepadContainer {
      display: none;
    }

    .gamepad {
      width: 900px;
      height: 500px;
      background: #36363675;
      border-radius: 20px;
      position: relative;
      display: flex;
      justify-content: center;
      align-items: center;
      flex-direction: column;
      padding: 20px;
    }

    .button,
    .stick {
      display: flex;
      justify-content: center;
      align-items: center;
      width: 60px;
      height: 60px;
      border-radius: 50%;
      background: #555;
      cursor: pointer;
      transition: transform 0.1s, background 0.1s;
    }

    .button:active, .stick:active,
    .button:hover, .stick:hover {
      transform: scale(0.9);
      background: #777;
    }

    .joystick:hover {
        background-color: rgba(255, 255, 255, 0.4); /* Más visible */
        transition: background-color 0.3s ease; /* Suaviza la transición */
    }
    
    .joystick-inner:hover {
        background-color: rgba(0, 255, 0, 0.7); /* Color visible al hacer hover en el centro */
        transform: scale(1.2); /* Incrementa el tamaño del círculo interno */
        transition: all 0.3s ease;
    }
    
    /* Hover en Switch */
    .switch:hover .slider {
        background-color: #66BB6A; /* Cambia el color del slider al pasar el mouse */
        box-shadow: 0 0 10px rgba(102, 187, 106, 0.8); /* Añade un efecto de brillo */
        transition: all 0.3s ease;
    }
    
    .switch:hover .slider:before {
        background-color: #ffffff; /* Cambia el color del círculo dentro del slider */
        transform: scale(1.1); /* Incrementa el tamaño del círculo */
    }

    .dpad {
      position: absolute;
      top: 120px;
      left: 50px;
    }

    .dpad div {
      display: flex;
      justify-content: center;
      align-items: center;
      background: #555;
      border-radius: 5px;
      width: 50px;
      height: 50px;
      margin: 5px;
      cursor: pointer;
      transition: transform 0.1s, background 0.1s;
    }

    .dpad div:hover {
      background: #777;
    }

    .dpad div:active {
      transform: scale(0.9);
      background: #777;
    }

    .symbol-buttons {
      position: absolute;
      top: 120px;
      right: 50px;
    }

    .symbol-buttons .button {
      margin: 5px;
    }

    .ps-btn {
      width: 80px;
      height: 80px;
      border-radius: 50%;
      background: #777;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
    }

    .ps-btn:active {
      background: #888;
      transform: translate(-50%, -50%) scale(0.9);
    }

    .trigger {
      position: absolute;
      top: 30px;
    }

    #L2 {
      left: 50px;
    }

    #L1 {
      left: 120px;
    }

    #R1 {
      right: 120px;
    }

    #R2 {
      right: 50px;
    }

    .cross-layout {
      display: grid;
      grid-template-columns: 50px 50px 50px;
      grid-template-rows: 50px 50px 50px;
      gap: 5px;
    }

    .cross-layout div:nth-child(1) {
      grid-column: 2;
    }

    .cross-layout div:nth-child(2) {
      grid-row: 2;
    }

    .cross-layout div:nth-child(3) {
      grid-row: 2;
      grid-column: 3;
    }

    .cross-layout div:nth-child(4) {
      grid-column: 2;
      grid-row: 3;
    }

    .joystick {
      position: absolute;
      width: 120px;
      height: 120px;
      border-radius: 50%;
      background-color: rgba(255, 255, 255, 0.2);
      display: flex;
      justify-content: center;
      align-items: center;
    }

    .joystick-inner {
      width: 50px;
      height: 50px;
      border-radius: 50%;
      background-color: white;
      transition: transform 0.1s ease;
    }

    .joystick-left {
      left: 50px;
      bottom: 80px;
    }

    .joystick-right {
      right: 50px;
      bottom: 80px;
    }

    .switch-container {
      display: flex;
      justify-content: center;
      width: 600px;
      position: absolute;
      top: 10px;
      left: 50%;
      transform: translateX(-50%);
      padding: 10px;
    }

    .switch {
      display: flex;
      align-items: center;
      margin: 0 15px;
    }

    .switch-label {
      margin-left: 10px;
      color: white;
    }

    .switch input {
      display: none;
    }

    .switch .slider {
      width: 34px;
      height: 20px;
      background-color: #3399FF;
      border-radius: 50px;
      position: relative;
      transition: 0.4s;
    }

    .switch .slider:before {
      content: "";
      position: absolute;
      height: 16px;
      width: 16px;
      border-radius: 50px;
      left: 4px;
      bottom: 2px;
      background-color: white;
      transition: 0.4s;
    }

    .switch input:checked+.slider {
      background-color: #4CAF50;
    }

    .switch input:checked+.slider:before {
      transform: translateX(14px);
    }

    /* Ajustes para móviles */
    @media screen and (max-width: 768px) {
        .gamepad {
            flex-direction: column;
            width: 100%;
            padding: 10px;
        }

        .button, .stick {
            width: 50px;
            height: 50px;
        }

        .joystick {
            width: 100px;
            height: 100px;
        }

        .joystick-inner {
            width: 40px;
            height: 40px;
        }

        .switch-container {
            flex-direction: column;
            width: 100%;
        }

        .cross-layout {
            grid-template-columns: repeat(3, 50px);
            grid-template-rows: repeat(3, 50px);
            gap: 10px;
        }

        .dpad, .symbol-buttons {
            grid-template-columns: repeat(2, 50px);
            gap: 10px;
        }
    }

    /* Ajustes para orientación horizontal en móviles */
    @media screen and (orientation: landscape) and (max-width: 768px) {
        .gamepad {
            flex-direction: row;
            justify-content: space-around;
            width: 100%;
            padding: 20px;
        }

        .button, .stick {
            width: 60px;
            height: 60px;
        }

        .joystick {
            width: 120px;
            height: 120px;
        }

        .switch-container {
            flex-direction: row;
            width: 80%;
        }

        .cross-layout {
            grid-template-columns: repeat(4, 60px);
            gap: 15px;
        }
    }

  </style>
</head>

<body>

  <div class="switch-container">
    <div class="switch">
      <input type="checkbox" id="modeSwitch" onchange="toggleMode()">
      <label class="slider" for="modeSwitch"></label>
      <span class="switch-label" id="modeLabel">Modo Automático</span>
    </div>
    <div class="switch">
      <input type="checkbox" id="typeSwitch" onchange="toggleType()">
      <label class="slider" for="typeSwitch"></label>
      <span class="switch-label" id="typeLabel">Tipo Drone</span>
    </div>
  </div>

  <div id="automaticMode">
    Operando en modo automático.
  </div>

  <div class="gamepad" id="gamepadContainer">
    <div class="button trigger" id="L2">L2</div>
    <div class="button trigger" id="L1">L1</div>
    <div class="button trigger" id="R1">R1</div>
    <div class="button trigger" id="R2">R2</div>

    <div class="dpad cross-layout">
      <div class="direction" id="up">▲</div>
      <div class="direction" id="left">◄</div>
      <div class="direction" id="right">►</div>
      <div class="direction" id="down">▼</div>
    </div>

    <div class="symbol-buttons cross-layout">
      <div class="button" id="Triangle">△</div>
      <div class="button" id="Square">□</div>
      <div class="button" id="Circle">O</div>
      <div class="button" id="Cross">X</div>
    </div>

    <!-- Botón PS -->
    <div class="ps-btn" id="psButton">PS</div>

    <div class="joystick joystick-left" id="joystickLeft">
      <div class="joystick-inner" id="joystickLeftInner"></div>
    </div>
    <div class="joystick joystick-right" id="joystickRight">
      <div class="joystick-inner" id="joystickRightInner"></div>
    </div>
  </div>

  <script>
    let mode = "automatico";
    let controlType = "drone";
    // Función para cambiar de modo
    function toggleMode() {
      let modeSwitch = document.getElementById("modeSwitch");
      let modeLabel = document.getElementById("modeLabel");
      let automaticMode = document.getElementById("automaticMode");
      let gamepad = document.getElementById("gamepadContainer");

      if (modeSwitch.checked) {
        modeLabel.textContent = "Modo Manual";
        mode = "manual";
        automaticMode.style.display = 'none';
        gamepad.style.display = 'flex';
      } else {
        modeLabel.textContent = "Modo Automático";
        mode = "automatico";
        automaticMode.style.display = 'block';
        gamepad.style.display = 'none';
      }
    }

    // Función para cambiar el tipo de control (drone o robot)
    function toggleType() {
      let typeSwitch = document.getElementById("typeSwitch");
      let typeLabel = document.getElementById("typeLabel");

      if (typeSwitch.checked) {
        typeLabel.textContent = "Tipo Robot";
        controlType = "robot";
      } else {
        typeLabel.textContent = "Tipo Drone";
        controlType = "drone";
      }
    }

    //Eventos para direccionales
    const directions = ["up", "down", "left", "right"];
    const intervals = {};

    directions.forEach((dir) => {
      const button = document.getElementById(dir);

      // Función para manejar el inicio del evento
      const startEvent = () => {
        if (intervals[dir]) return; // Si ya está en ejecución, no iniciar otro
        sendCommand(dir);
        intervals[dir] = setInterval(() => sendCommand(dir), 100); // Enviar cada 100 ms
      };

      // Función para manejar el fin del evento
      const stopEvent = () => {
        clearInterval(intervals[dir]);
        delete intervals[dir];
      };

      // Eventos para computadoras
      button.addEventListener("mousedown", startEvent);
      button.addEventListener("mouseup", stopEvent);
      button.addEventListener("mouseleave", stopEvent); // Detener si el mouse sale del botón

      // Eventos para dispositivos táctiles
      button.addEventListener("touchstart", (e) => {
        e.preventDefault(); // Prevenir scrolling
        startEvent();
      });
      button.addEventListener("touchend", stopEvent);
    });

    // Eventos para botones
    const buttonIds = ["Triangle", "Square", "Circle", "Cross", "L1", "L2", "R1", "R2", "psButton"];
    buttonIds.forEach(id => {
      const button = document.getElementById(id);
      button.addEventListener("click", () => sendCommand(id.toLowerCase()));
      button.addEventListener("touchstart", (e) => {
        e.preventDefault(); // Evitar el comportamiento por defecto en móviles
        sendCommand(id.toLowerCase());
      });
    });

    // Lógica de Joysticks
    let joystickLeft = document.getElementById("joystickLeftInner");
    let joystickRight = document.getElementById("joystickRightInner");

    let leftJoystick = { x: 0, y: 0 };
    let rightJoystick = { x: 0, y: 0 };

    let isDraggingLeft = false;
    let isDraggingRight = false;
    let leftInterval = null;
    let rightInterval = null;

    function startLeftInterval() {
      if (leftInterval) clearInterval(leftInterval);
      leftInterval = setInterval(() => handleLeftJoystick(leftJoystick), 100);
    }

    function stopLeftInterval() {
      clearInterval(leftInterval);
      leftInterval = null;
      resetJoystick(joystickLeft, leftJoystick);
    }

    function startRightInterval() {
      if (rightInterval) clearInterval(rightInterval);
      rightInterval = setInterval(() => handleRightJoystick(rightJoystick), 100);
    }

    function stopRightInterval() {
      clearInterval(rightInterval);
      rightInterval = null;
      resetJoystick(joystickRight, rightJoystick);
    }

    function resetJoystick(joystick, position) {
      position.x = 0;
      position.y = 0;
      joystick.style.transform = "translate(0px, 0px)";
    }

    function handleJoystickEvent(event, joystick, position) {
      const rect = joystick.parentElement.getBoundingClientRect();
      const centerX = rect.width / 2;
      const centerY = rect.height / 2;

      const isTouch = event.type.includes("touch");
      const clientX = isTouch ? event.touches[0].clientX : event.clientX;
      const clientY = isTouch ? event.touches[0].clientY : event.clientY;

      let dx = clientX - rect.left - centerX;
      let dy = clientY - rect.top - centerY;

      const distance = Math.sqrt(dx * dx + dy * dy);
      const maxRadius = rect.width / 2;

      if (distance > maxRadius) {
        dx = (dx / distance) * maxRadius;
        dy = (dy / distance) * maxRadius;
      }

      position.x = dx;
      position.y = -dy;
      joystick.style.transform = `translate(${dx}px, ${dy}px)`;
    }

    // Joystick izquierdo
    joystickLeft.addEventListener("mousedown", () => {
      isDraggingLeft = true;
      startLeftInterval();
    });
    joystickLeft.addEventListener("mousemove", (e) => {
      if (isDraggingLeft) handleJoystickEvent(e, joystickLeft, leftJoystick);
    });
    joystickLeft.addEventListener("touchstart", (e) => {
      e.preventDefault();
      isDraggingLeft = true;
      startLeftInterval();
    });
    joystickLeft.addEventListener("touchmove", (e) => {
      if (isDraggingLeft) handleJoystickEvent(e, joystickLeft, leftJoystick);
    });
    document.addEventListener("mouseup", () => {
      isDraggingLeft = false;
      stopLeftInterval();
    });
    document.addEventListener("touchend", () => {
      isDraggingLeft = false;
      stopLeftInterval();
    });

    // Joystick derecho
    joystickRight.addEventListener("mousedown", () => {
      isDraggingRight = true;
      startRightInterval();
    });
    joystickRight.addEventListener("mousemove", (e) => {
      if (isDraggingRight) handleJoystickEvent(e, joystickRight, rightJoystick);
    });
    joystickRight.addEventListener("touchstart", (e) => {
      e.preventDefault();
      isDraggingRight = true;
      startRightInterval();
    });
    joystickRight.addEventListener("touchmove", (e) => {
      if (isDraggingRight) handleJoystickEvent(e, joystickRight, rightJoystick);
    });
    document.addEventListener("mouseup", () => {
      isDraggingRight = false;
      stopRightInterval();
    });
    document.addEventListener("touchend", () => {
      isDraggingRight = false;
      stopRightInterval();
    });

    function handleLeftJoystick(position) {
      let command = '';

      if (position.y > 20) command = 'up'; // Adelante
      if (position.y < -20) command = 'down'; // Atrás
      if (position.x < -20) command = 'left'; // Izquierda
      if (position.x > 20) command = 'right'; // Derecha

      if (command) sendCommand(command);
    }

    let activeServo = null; // Variable para rastrear el servo activo (S9 o S10)
    
    // Lógica para el control del drone (Joystick derecho)
    function handleRightJoystickDrone(position) {
      let command = '';
    
      if (position.y > 20) command = 'above'; // Arriba
      if (position.y < -20) command = 'below'; // Abajo
      if (position.x < -20) command = 'speed'; // setear velocidad
      if (position.x > 20) command = 'battery'; // estado de bateria
    
      if (command) sendCommand(command);
    }
    
    // Lógica para el control del robot (Joystick derecho)
    function handleRightJoystickRobot(position) {
      let command = '';
    
      // Calcular el ángulo en función de la posición Y del joystick (de -100 a 100)
      let angle = Math.round(((position.y + 100) / 200) * 180); // Mapea el valor y (de -100 a 100) a un ángulo de 0 a 180
    
      // Si el joystick se mueve a la izquierda (S9)
      if (position.x < -20 && activeServo !== 'S9') {
        // Desactiva el servo S10 y activa el servo S9
        command = `S10:0`; // Desactiva S10
        sendCommand(command);
        activeServo = 'S9'; // Establece S9 como el servo activo
      }
      // Si el joystick se mueve a la derecha (S10)
      else if (position.x > 20 && activeServo !== 'S10') {
        // Desactiva el servo S9 y activa el servo S10
        command = `S9:0`; // Desactiva S9
        sendCommand(command);
        activeServo = 'S10'; // Establece S10 como el servo activo
      }
    
      // Si se ha activado un servo (S9 o S10), enviar el ángulo correspondiente
      if (activeServo) {
        command = `${activeServo}:${angle}`; // Manda el comando con el ángulo calculado
        sendCommand(command);
      }
    }
    
    // Función que detecta qué dispositivo se está controlando y ejecuta la lógica correspondiente
    function handleRightJoystick(position) {
      if (controlType === 'drone' && mode === 'manual')  {
        handleRightJoystickDrone(position); // Lógica para el drone (joystick derecho)
      } else if (controlType === 'robot' && mode === 'manual') {
        handleRightJoystickRobot(position); // Lógica para el robot (joystick derecho)
      }
    }

    // Enviar comandos (Dron o Robot)
    function sendCommand(command) {
      let cmd = '';

      // Mapeo de comandos para el drone
      if ((controlType === 'drone') && (mode === 'manual')) {
        if (command === 'up') cmd = 'forward 50';
        else if (command === 'down') cmd = 'back 50';
        else if (command === 'left') cmd = 'left 50';
        else if (command === 'right') cmd = 'right 50';
        else if (command === 'triangle') cmd = 'flip f';
        else if (command === 'square') cmd = 'flip l';
        else if (command === 'circle') cmd = 'flip r';
        else if (command === 'cross') cmd = 'flip b';
        else if (command === 'l1') cmd = 'takeoff';
        else if (command === 'l2') cmd = 'land';
        else if (command === 'r1') cmd = 'cw 90';
        else if (command === 'r2') cmd = 'ccw 90';
        else if (command === 'psbutton') cmd = 'command';
        else if (command === 'speed') cmd = 'speed 20';
        else if (command === 'above') cmd = 'up 50';
        else if (command === 'below') cmd = 'down 50';
        else if (command === 'battery') cmd = 'battery?';
      }

      // Mapeo de comandos para el robot
      else if ((controlType === 'robot') && (mode === 'manual')) {
        if (command === 'up') cmd = 'F';
        else if (command === 'down') cmd = 'B';
        else if (command === 'left') cmd = 'L';
        else if (command === 'right') cmd = 'R';
        else if (command === 'triangle') cmd = 'x';
        else if (command === 'square') cmd = 'y';
        else if (command === 'circle') cmd = 'Y';
        else if (command === 'cross') cmd = 'X';
        else if (command === 'l1') cmd = 'u';
        else if (command === 'l2') cmd = 'v';
        else if (command === 'r1') cmd = 'alarma on';
        else if (command === 'r2') cmd = 'alarma off';
        else if (command === 'psbutton') cmd = 'E';
        else if (command.startsWith('S')) cmd = command;
      }
      // Lógica del modo
      if (mode === 'manual') {
        if (cmd !== '') {
          if (controlType === 'drone') fetch(`/drone?msg=` + cmd);
          else fetch(`/robot?cmd=` + cmd);

          console.log(`Comando enviado: ${cmd}`);
        } else {
          console.log('No se envió comando: cmd vacío.');
        }
      } else if (mode === 'automatic') {
        console.log('Modo automático activado.');
      }
    }
  </script>
</body>

</html>)rawliteral";
    request->send(200, "text/html", htmlContent);
  });

  // Comandos para el dron
  server.on("/drone", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasParam("msg")) {
      String msg = request->getParam("msg")->value();
      udp.beginPacket(droneIP, dronePort);
      udp.print(msg);
      udp.endPacket();
      Serial.printf("Comando enviado al drone: %s\n", msg.c_str());
      request->send(200, "text/plain", "Comando enviado al drone: " + msg);
    } else {
      request->send(400, "text/plain", "Mensaje no especificado");
    }
  });

  // Comandos para el robot
  server.on("/robot", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      SerialArduino.print(cmd);
      Serial.printf("Comando enviado al carro robot: %s\n", cmd.c_str());
      request->send(200, "text/plain", "Comando enviado al carro robot: " + cmd);
    } else {
      request->send(400, "text/plain", "Comando no especificado");
    }
  });

  // Iniciar el servidor
  server.begin();
}

void loop() {

}
