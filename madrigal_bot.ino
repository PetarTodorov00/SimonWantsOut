//main code

//imports for servo and LED MATRIX
#include <Servo.h>
#include <LedControl.h>

//setup servo
Servo servo;

//lightsensor
int frequency = 0;

//pin for buzzer
int buzzer=19;

//angle for servo
int angle=10;

//how far away the wall is in 6 different directions
int scan[5];

//pins for ultrasound sensors
int trigPin= 5;
int echoPin =6;
//variables to calculate  distance
long duration;
int distance;

//is it trapped/jail cell
bool trapped=false;

//is the game running
bool gamerunning=true;

//how many chances at the simon game
int chances=0;

//symbols for led matrix
byte started[8]=   {0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00};
byte a[8]=  {0x7C,0x7C,0x60,0x7C,0x7C,0x60,0x7C,0x7C};
byte b[8]= {0x78,0x7C,0x66,0x66,0x66,0x66,0x7C,0x78};
byte c[8]= {0x66,0x66,0x66,0x66,0x66,0x66,0x7E,0x7E}; 
byte smile[8]=   {0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C};
byte frown[8]=   {0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C};
byte neutral[8]= {0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C};

//led matrix set up
LedControl lc=LedControl(13,15,14,0);


void setup() {
  // make pins output, some of them are anlog pins converted to digital pins 
  
  //LED MATRIX set up
  lc.shutdown(0,false);       //bootup
  lc.setIntensity(0,15);      // Setting the brightness to maximum value
  lc.clearDisplay(0);         //and clear the display
  
  //color sensor, s2, and s3, choose which color we are detecting, SOUT returns color value
  pinMode(12,INPUT);//SOUT
  pinMode(11,OUTPUT);// S3
  pinMode(10,OUTPUT);// S2
  
  //motors controlled by this inputs
  pinMode(9,OUTPUT);//left motor
  pinMode(8,OUTPUT);//right motor
  
  //servo motor connected to pin 7
  servo.attach(7);
  servo.write(angle);
  
  //ultrasound
  pinMode(echoPin,INPUT);  //echopin
  pinMode(trigPin,OUTPUT); //trigpin
  
  //button
  pinMode(4,INPUT);//C
  pinMode(3,INPUT);//B
  pinMode(2,INPUT);//A

  //for debuggin purposes
  Serial.begin(9600);
}

void loop() {
  //for debugging purposes
  noisealert();
  Serial.print(scan[2]);
  
  // main loop
  trapped=sense_color();
  if(!trapped && gamerunning){
    
    displaying(started);
    sense_enviroment();
    
    //only forward object detection implemented, move forward if there is nothing blocking
    if(scan[3]>10){
      digitalWrite(9,HIGH);
      digitalWrite(8,HIGH);
      delay(200);
      digitalWrite(9,LOW);
      digitalWrite(8,LOW);
    }  
     
  }else if(trapped){
    simongame();
  }else if(!gamerunning){
    noisealert();
    delay(5000);
  }
}
void sense_enviroment(){
   // scan from 10 to 180 degrees
  for(angle = 10; angle < 180; angle++)  
  {                                  
    servo.write(angle);               
    delay(15);     
            
    if(angle%30==0){
      if(angle==30){
          scan[0]=ultrasoundsense();
      }else if(angle==60){
          scan[1]=ultrasoundsense();
      }else if(angle==90){
          scan[2]=ultrasoundsense();
      }else if(angle==120){
          scan[3]=ultrasoundsense();
      }else if(angle==150){
          scan[4]=ultrasoundsense();
      }else if(angle==180){
          scan[5]=ultrasoundsense();
      }     
    }
          
  } 
  
  //go back                             
  servo.write(10);           
  delay(15);   
}

int ultrasoundsense(){  
  //returns the distance to the wall
  
  //clear the pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // return distance
  return distance;
}
boolean sense_color(){
  int red;
  int green;
  int blue;
  
  // Setting red filtered photodiodes to be read
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  // Reading the output frequency
  frequency = pulseIn(12, LOW);
  //scaling the frequency
  red = map(frequency, 25,72,300,0);

  // Setting Green filtered photodiodes to be read
  digitalWrite(10,HIGH);
  digitalWrite(11,HIGH);
  frequency = pulseIn(12, LOW);
  green= map(frequency, 25,72,300,0);
  
  // Setting Blue filtered photodiodes to be read
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  frequency = pulseIn(12, LOW);
  blue = map(frequency, 25,72,300,0);

  int result=(blue+green)/2;
  result=red/result;
  
  if (red>(green*1.25) && red >(blue*1.25)){
    return true;
  }else{
    return false;
  }
}

//unfinished code here, how to navigate the maze, decide between left and right, adjust movement
void moveoptimalroute(){
  
}

//displays on LED what ever is sent to it
void displaying(byte character[]){
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
  delay(30);
}

void simongame(){
  //alerts the user of simon game
  noisealert();
  //create 4 random numbers
  int sequence[4];
  sequence[0]=random(1,3);
  sequence[1]=random(1,3);
  sequence[2]=random(1,3);
  sequence[3]=random(1,3);
  
  //convert them into charcters to display
  int i = 0;
  for(i=0;i<4;i++)
  {
    //change to simon game charchters are,instead of smile,frown and neutral, (debugging purposes)
    if (sequence[i]==1){
      noisealert();
      displaying(smile);
      
      delay(300);
    }else if (sequence[i]==2){
      noisealert();
      displaying(frown);
      delay(300);
    }else if (sequence[i]==3){
      noisealert();
      displaying(neutral);
      
      delay(300);
    }
    Serial.print("random number " + sequence[i]);
    delay(500);
  }
  displaying(started);
  
  //important to delte, this for testing purposes
  sequence[0]=1;
  sequence[1]=1;
  sequence[2]=1;
  sequence[3]=1;
  
  //create a data of players guesses
  int sequencerrr[4];
  bool gameon=true;

  //only allow 4 presses
  int presses=-1;
  
  while(gameon){
    int a= digitalRead(2);
    int b=digitalRead(3);
    int c=digitalRead(4);
    
    if (a > 0){
      presses+=1;
      sequencerrr[presses]=1;   
      noisealert();
    }else if (b > 0){
      presses+=1;
      sequencerrr[presses]=2;
      
      noisealert();
    }else if (c> 0){
      presses+=1;
      sequencerrr[presses]=3;
      noisealert();
    }
    Serial.print(presses);
    if (presses==3){
      
      //compares if the player has guessed correctly
      bool areequal = array_cmp(sequencerrr,sequence);
      if (areequal){
        displaying(smile);
        noisealert();
        delay(1000);
        trapped=false;
        gamerunning=true;
        gameon=false;
      }else if(chances<3){
        chances+=1;
        simongame();
      }else{
        trapped=false;
        gamerunning=false;
      }
    }
  }
  
}
void noisealert(){
  tone(buzzer,450);
  delay(500);
  noTone(buzzer);
}
boolean array_cmp(int *a, int *b){
      int n;
      for (n=0;n<4;n++) if (a[n]!=b[n]) return false;
      return true;
}
