const float RAIN_MM = 0.2794;
const float WIND_SPEED_KMH = 2.4;
const int WIND_SPEED_PIN = 27;
const int WIND_DIR_PIN = 35;
const int RAIN_PIN = 23;

int rotation = 0;
int rain_ticks = 0;

void read_wind_speed() {
  float wind_speed = WIND_SPEED_KMH * rotation;
  Serial.print("wind_speed (km/h): ");
  Serial.println(wind_speed);
}

void read_rain() {
  float rain = RAIN_MM * rain_ticks;
  Serial.print("rain (mm): ");
  Serial.println(rain);
}

void get_wind_direction() {
    int analog_value = analogRead(WIND_DIR_PIN);
   
    switch(analog_value) {
      case 2225-50 ... 2225+50:
        Serial.print("Wind direction: ");
        Serial.println("Nord");
        break;
      case 3830-50 ... 3830+50:
        Serial.print("Wind direction: ");
        Serial.println("Nord-Est 30 deg");
        break;
      case 3472-50 ... 3472+50:
        Serial.print("Wind direction: ");
        Serial.println("Nord-Est 60 deg");
        break;
      case 2640-50 ... 2640+50:
        Serial.print("Wind direction: ");
        Serial.println("Est");
        break;
      case 986-50 ... 986+50:
        Serial.print("Wind direction: ");
        Serial.println("Nord-Ouest 300 deg");
        break;
      case 2359 ... 2359 + 1:
        Serial.print("Wind direction: ");
        Serial.println("Nord-Ouest 330 deg");
        break;
      case 200-1 ... 200+1:
        Serial.print("Wind direction: ");
        Serial.println("Nord-Ouest");
        break;
      case 2990-50 ... 2990+50:
        Serial.print("Wind direction: ");
        Serial.println("Sud-Est 120 deg");
        break;
      case 1459-50 ... 1459+50:
        Serial.print("Wind direction: ");
        Serial.println("Sud-Est 150 deg");
        break;
      case 1658-50 ... 1658+50:
        Serial.print("Wind direction: ");
        Serial.println("Sud");
        break;
      case 194-1 ... 194+1:
        Serial.print("Wind direction: ");
        Serial.println("Sud-Ouest 210 deg");
        break;
      case 563-50 ... 563+50:
        Serial.print("Wind direction: ");
        Serial.println("Sud-Ouest 240 deg");
        break;
      case 342-50 ... 342+50:
        Serial.print("Wind direction: ");
        Serial.println("Ouest");
        break;   
    }
}

void get_humidity() {
  int i, j;
  int duree[42];
  unsigned long pulse;
  byte data[5];
  float humidite;
  float temperature;
  int broche = 16;
  
  pinMode(broche, OUTPUT_OPEN_DRAIN);
  digitalWrite(broche, HIGH);
  delay(250);
  digitalWrite(broche, LOW);
  delay(20);
  digitalWrite(broche, HIGH);
  delayMicroseconds(40);
  pinMode(broche, INPUT_PULLUP);
  
  while (digitalRead(broche) == HIGH);
  i = 0;

  do {
        pulse = pulseIn(broche, HIGH);
        duree[i] = pulse;
        i++;
  } while (pulse != 0);
 
  if (i != 42) 
    Serial.printf(" Erreur timing \n"); 

  for (i=0; i<5; i++) {
    data[i] = 0;
    for (j = ((8*i)+1); j < ((8*i)+9); j++) {
      data[i] = data[i] * 2;
      if (duree[j] > 50) {
        data[i] = data[i] + 1;
      }
    }
  }

  if ( (data[0] + data[1] + data[2] + data[3]) != data[4] ) 
    Serial.println(" Erreur checksum");

  humidite = data[0] + (data[1] / 256.0);
  temperature = data [2] + (data[3] / 256.0);
  Serial.printf(" Humidite = %4.0f \%%  Temperature = %4.2f degreC \n", humidite, temperature);
}


void read_light() {
  int value = analogRead(34);
  float voltage = value * (3.3 / 4095.0);
  float current = voltage / 68.0;

  float lux = (float)current * 100.0 / 0.2395;
  
  Serial.println("Luminosity (lux) : ", lux);
}

void add_wind_tour() { rotation++; }
void add_rain_tick() { rain_ticks++; }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);

  attachInterrupt(WIND_SPEED_PIN, add_wind_tour, RISING);
  attachInterrupt(RAIN_PIN, add_rain_tick, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  read_wind_speed();
  read_rain();
  read_light();
  get_humidity();
  get_wind_direction();

  // reset tours for next loop to ensure 1 sec delay
  rotation = 0;
  rain_ticks = 0;
  delay(1000);
}
