// Definition der Pins
// Sensoren
#define SensRight A1
#define SensLeft A2

#define US_echo A5
#define US_trig 4

// Motoren
#define MotorIn1 9
#define MotorIn2 6
#define MotorIn3 11
#define MotorIn4 10
// Taster
#define Taster A0

// Globale Variablen, welche ihren Zustand über Schleifendurchgänge hinweg behalten sollen
bool auto_aktiv;             // gibt an, ob das auto grade in einem aktiven Zustand ist oder "schlaeft"
bool knopf_zuvor_gedrueckt;  // gibt an, ob der taster im letzten durchgang gedrueckt war.
int us_echo;

// Die setup()-Funktion wird 1x am Anfang ausgeführt
void setup() {
  Serial.begin(9600);
  // Modus der Pins setzen (Ein-/Ausgabe)
  pinMode(MotorIn1, OUTPUT);
  pinMode(MotorIn2, OUTPUT);
  pinMode(MotorIn3, OUTPUT);
  pinMode(MotorIn4, OUTPUT);
  pinMode(SensLeft, INPUT);
  pinMode(SensRight, INPUT);
  pinMode(US_echo, INPUT);
  pinMode(US_trig, OUTPUT);
  // Der Taster ist ein Input-Pull-Up Pin, heißt wenn nichts angeschlossen ist zeigt er auf 1 (up)
  // Ein gedrückter Knopf ist damit ein Taster-Pin auf 0, da er dann mit GND verbunden ist
  pinMode(Taster, INPUT_PULLUP);
  // Initiales setzen der globalen Variablen
  auto_aktiv = true;
  knopf_zuvor_gedrueckt = false;
}



// Die loop()-Funktion läuft unendlich oft hintereinander
void loop() {
  // Auslesen der sensoren
  int linker_sensor = analogRead(SensLeft);
  int rechter_sensor = analogRead(SensRight);
  bool linker_sensor_auf_weiss = !digitalRead(SensLeft);
  bool rechter_sensor_auf_weiss = !digitalRead(SensLeft);
  bool knopf_gedrueckt = !digitalRead(A0);

  // Falls der Knopf gedrueckt wird, zuvor aber nicht gedrueckt wurde, wechsel den aktiv-zustand des Autos
  if (knopf_gedrueckt && !knopf_zuvor_gedrueckt) {
    auto_aktiv = !auto_aktiv;
  }
  knopf_zuvor_gedrueckt = knopf_gedrueckt;

  digitalWrite(US_trig, HIGH);
  delay(10);
  digitalWrite(US_trig, LOW);
  // Read the echoPin, returns the sound wave travel time in microseconds
  long duration = .5*duration + .5*pulseIn(US_echo, HIGH);
  // Calculate the distance in centimeters
  int distance = duration * 0.034 / 2;
  distance = min(distance, 20);

  // Fahrlogik
  String fahrzustand = "Unbekannt";
  if (auto_aktiv) {  // Nur durchführen, wenn das auto grade aktiv ist
    if (linker_sensor_auf_weiss && rechter_sensor_auf_weiss) {
      vorwaerts();  // Beide sensoren auf weiss -> forwarts fahren
      fahrzustand = "vorwaerts";
    } else if (!linker_sensor_auf_weiss && rechter_sensor_auf_weiss) {
      drehen_links();  // Linker sensor auf schwarz, rechter auf weiss -> links drehen
      fahrzustand = "drehen_links";
    } else if (linker_sensor_auf_weiss && !rechter_sensor_auf_weiss) {
      drehen_rechts();  // Rechter sensor auf schwarz, linker auf weiss -> rechts drehen
      fahrzustand = "drehen_rechts";
    } else {
      motoren_aus();  // ansonsten (beide auf schwarz) -> stehen bleiben
      fahrzustand = "auf schwarz";
    }
  } else {  // falls das auto nicht aktiv ist, werden die motoren gestoppt
    motoren_aus();
    fahrzustand = "Inaktiv";
  }

  // Zustand auf Konsole ausgeben
  konsolenausgabe(linker_sensor, rechter_sensor, linker_sensor_auf_weiss, rechter_sensor_auf_weiss, knopf_gedrueckt, fahrzustand, distance);
}

void vorwaerts() {
  analogWrite(MotorIn1, 255);
  analogWrite(MotorIn3, 255);
}

void drehen_links() {
  analogWrite(MotorIn1, 130);
  analogWrite(MotorIn3, 200);
}

void drehen_rechts() {
  analogWrite(MotorIn1, 200);
  analogWrite(MotorIn3, 130);
}

void motoren_aus() {
  analogWrite(MotorIn1, 0);
  analogWrite(MotorIn3, 0);
}

void konsolenausgabe(int linker_sensor, int rechter_sensor, bool linker_sensor_auf_weiss, bool rechter_sensor_auf_weiss, bool knopf_gedrueckt, String fahrzustand, int distance) {
  Serial.print("Sensor links="),
    Serial.print(linker_sensor);
  if (linker_sensor_auf_weiss) {
    Serial.print(": weiss   | ");
  } else {
    Serial.print(": schwarz | ");
  }
  Serial.print("Sensor rechts=");
  Serial.print(rechter_sensor);
  if (rechter_sensor_auf_weiss) {
    Serial.print(": weiss   | ");
  } else {
    Serial.print(": schwarz | ");
  }
  Serial.print("Taster=");
  if (knopf_gedrueckt) {
    Serial.print("ON  | ");
  } else {
    Serial.print("OFF | ");
  }
  Serial.print("auto_aktiv=");
  if (auto_aktiv) {
    Serial.print("ON  | ");
  } else {
    Serial.print("OFF | ");
  }
  Serial.print("fahrzustand=");
  Serial.print(fahrzustand);

  Serial.print(" | Distance=");
  Serial.print(distance);
  Serial.print(" cm |");
  for(int c = 0; c < distance; c++) Serial.print(" ");
  Serial.print("X");
  for(int c = distance; c <= 20; c++) Serial.print(" ");
  Serial.print("|");
  Serial.println();

  analogWrite(MotorIn2, 0);
  analogWrite(MotorIn4, 0);
}