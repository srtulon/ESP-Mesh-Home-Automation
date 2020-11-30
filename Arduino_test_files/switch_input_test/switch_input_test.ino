

int sw1=22;
int sw2=21;
int sw3=19;
int sw4=18;


int s1=0;
int s2=0;
int s3=0;
int s4=0;


void setup() {
  Serial.begin(115200);
  Serial.println("Start");


  pinMode(sw1,INPUT);
  pinMode(sw2,INPUT);
  pinMode(sw3,INPUT);
  pinMode(sw4,INPUT);
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if(digitalRead(sw1)!=s1){
    s1=digitalRead(sw1);
    Serial.println("Toggled Switch 1. Status :"+s1);
  }
  if(digitalRead(sw2)!=s2){
    s2=digitalRead(sw2);
    Serial.println("Toggled Switch 2. Status :"+s2);
  }
  if(digitalRead(sw3)!=s3){
    s3=digitalRead(sw3);
    Serial.println("Toggled Switch 3. Status :"+s3);
  }
  if(digitalRead(sw4)!=s4){
    s4=digitalRead(sw4);
    Serial.println("Toggled Switch 4. Status :"+s4);
  }
}
