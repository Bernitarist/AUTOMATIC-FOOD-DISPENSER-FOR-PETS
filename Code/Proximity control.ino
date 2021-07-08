
int trig=11;
int echo=10;

float SoundSpeed=343;
float dist;
float ping;

void setup() {

  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
}

void loop() {
     digitalWrite(trig, LOW);
     delayMicroseconds(2000);
     digitalWrite(trig, HIGH);
     delayMicroseconds(20);
     digitalWrite(trig, LOW);

     ping = pulseIn(echo, HIGH);  
     dist = (SoundSpeed*ping)/2000000;

    
     Serial.print(dist);
     Serial.println("m");                                             
   }



