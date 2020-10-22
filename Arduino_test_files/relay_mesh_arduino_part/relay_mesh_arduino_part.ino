
int s1=2;
int s2=3;
int s3=4;
int s4=5;
int s5=6;
int s6=7;
int s7=8;
int s8=9;

int temp=0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  
  Serial.println("Start");
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(s4, OUTPUT);
  pinMode(s5, OUTPUT);
  pinMode(s6, OUTPUT);
  pinMode(s7, OUTPUT);
  pinMode(s8, OUTPUT);
  
}

void loop() {
  if (Serial.available()){
    String state = Serial.readString();    
    Serial.println(state);
    if(state.charAt(0)=='0'){
      Serial.println("All off");
      digitalWrite(s1,0);
      digitalWrite(s2,0);
      digitalWrite(s3,0);
      digitalWrite(s4,0);
      digitalWrite(s5,0);
      digitalWrite(s6,0);
      digitalWrite(s7,0);
      digitalWrite(s8,0);
    }
    else if(state.charAt(0)=='1'){
      Serial.println("All on");
      digitalWrite(s1,1);
      digitalWrite(s2,1);
      digitalWrite(s3,1);
      digitalWrite(s4,1);
      digitalWrite(s5,1);
      digitalWrite(s6,1);
      digitalWrite(s7,1);
      digitalWrite(s8,1);
    }
    else if(state.charAt(0)=='2'){  
      temp=(state.charAt(1))-'0';
      digitalWrite(s1,temp);
      temp=(state.charAt(2))-'0';
      digitalWrite(s2,temp);
      temp=(state.charAt(3))-'0';
      digitalWrite(s3,temp);
      temp=(state.charAt(4))-'0';
      digitalWrite(s4,temp);
      temp=(state.charAt(5))-'0';
      digitalWrite(s5,temp);
      temp=(state.charAt(6))-'0';
      digitalWrite(s6,temp);
      temp=(state.charAt(7))-'0';
      digitalWrite(s7,temp);
      temp=(state.charAt(8))-'0';
      digitalWrite(s8,temp);
    }
  }
}
