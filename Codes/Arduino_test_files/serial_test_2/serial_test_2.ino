void setup() {
  Serial.begin(115200); // set the baud rate
  Serial.println("Ready"); // print "Ready" once
}
void loop() {
  if(Serial.available()){ // only send data back if data has been sent
    String str = Serial.readString(); // read the incoming data
    //Serial.println(str);
    if(str[0]=='i'){
      Serial.println("ip: "+str.substring(1, str.length())); // send the data back in a new line so that it is not all one long line
    }
    else if(str[0]=='w'){
      Serial.println("wifi: "+str.substring(1, str.length())); // send the data back in a new line so that it is not all one long line
    }
    else if(str[0]=='p'){
      Serial.println("password: "+str.substring(1, str.length())); // send the data back in a new line so that it is not all one long line
    }
  } 
}
