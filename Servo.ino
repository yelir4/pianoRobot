#include <Servo.h> 
/*
 * final robot project: piano player robot
 * ENGR1-L section 05, group 5
 * 3/14/22
 */


// pin variables
int piezo = 11; // speaker
int songSelector = 12; // pushbutton for selecting song
int led = 13; // led for demo guide purpose
Servo Servo1; // 180 degree servo motor

// hard coded values for frequencies
int freqs[] = {261, 293, 329, 349, 392, 440, 493, 523};

/*
 * collection of various songs, each of which can be demo'd by our robot
 * numbers correlate to notes
 * i.e. C = 0, D = 1, E = 2 ... C octave higher = 7
 * "8" correlates to pause in song
 */
String test = "081828384858687";
String twinkle = "080848485858488838382828181808884848383828281";
String birthday = "0010328001043800754328555343";
String rowrow = "00012821234800044433300043210";
String mcdonald = "0004554822110840004554822110";
String mary = "21012228811188244882101222211210";
String spider = "4000122821012082234432342800122101200";
String baabaa = "004455554833221108444332221";

/*
 * @function setup()
 * enables servo, declares button pins as inputs, etc
 */
void setup() {
  Servo1.attach(3);
  pinMode(13, OUTPUT);
  pinMode(12, INPUT);
  pinMode(11, OUTPUT); 
  pinMode(10, INPUT); 
  pinMode(9, INPUT); 
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(3, OUTPUT);
  pinMode(2, INPUT);
  pinMode(A0, INPUT);

  Serial.begin(9600);
}

/*
 * @function loop()
 * our piano player functions and loops autonomously
 * 
 * first portion is a freeplay / song selection mode
 * user plays desired notes, which rotates servo
 * if desired, user presses button to begin song demonstration
 * servo rotation at time of button press determines song to be demo'd
 * 
 * the demo portion of our robot plays the correct notes with pauses
 * servo rotates throughout demo displaying which notes are currently being played
 * 
 * following the demo the piano teaches the user to play the song
 * servo rotates to the next note to be played
 * upon playing correct note, note sounds and servo rotates to next position
 * this continues until the song is complete
 * 
 * after this the loop returns to the beginning
 * user can freely play notes and select a new song to demo
 * thus our robot loops autonomously
 * 
 * throughout operation, our robot checks its light sensor
 * if it is dark, the piano will mute itself, however operation will continue
 */
void loop() {
  digitalWrite(13, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(A0, HIGH);

  // initializing photoresistor sensor value
  // mutes piano upon insufficient lighting
  int thres = analogRead(A0)/2;
  Serial.print("light threshold is ");
  Serial.println(thres);
  volume(thres);

  while(digitalRead(12) != LOW) { // until song selector is chosen, freeplay mode is enabled
    if(digitalRead(10) == LOW) {
      tone(piezo, freqs[0]);
      Servo1.write(0);
    } else if(digitalRead(9) == LOW) {
      tone(piezo, freqs[1]);
      Servo1.write(22.5);
    } else if(digitalRead(8) == LOW) {
      tone(piezo, freqs[2]);
      Servo1.write(45);
    } else if(digitalRead(7) == LOW) {
      tone(piezo, freqs[3]);
      Servo1.write(67.5);
    } else if(digitalRead(6) == LOW) {
      tone(piezo, freqs[4]);
      Servo1.write(90);
    } else if(digitalRead(5) == LOW) {
      tone(piezo, freqs[5]);
      Servo1.write(112.5);
    } else if(digitalRead(4) == LOW) {
      tone(piezo, freqs[6]);
      Servo1.write(135);
      while(digitalRead(4) == LOW) {} // added loop to deal with fluctuating noise
    } else if(digitalRead(2) == LOW) {
      tone(piezo, freqs[7]);
      Servo1.write(157.5);
      while(digitalRead(2) == LOW) {}
    } else {
      noTone(piezo);
    }
    volume(thres);
    delay(100);
  }  
  demo(thres); // run demonstration
}

/*
 * @function demo()
 * first the robot goes through the song selected, playing each note
 * then the robot guides the user through playing the song
 * @param thres is for the photoresistor sensor
 */
void demo(int thres) {
  String song;
  int rotation = Servo1.read();
  Serial.print("servo rotation is ");
  Serial.println(rotation);

  // song rotation is determined by servo rotation
  // a little leeway is given in case of improper rotation
  if(rotation < 10) { //c
    song = test;
  } else if(rotation<30) { //d
    song = twinkle;
  } else if(rotation<50) { //e
    song = birthday;
  } else if(rotation<75) { //f
    song = rowrow;
  } else if(rotation<95) { //g
    song = mcdonald;
  } else if(rotation<120) { //a
    song = mary;
  } else if(rotation<140) { //b
    song = spider;
  } else { //x
    song = baabaa;
  }
  
  // demonstration portion
  for(int i=0; i<song.length(); i++) { // iterate through all notes in song
    int in = song.charAt(i) - '0';
    
    if(in == 8) { // pause in song
      delay(100);
    } else {
      tone(piezo, freqs[in]);
      Servo1.write(in*22.5);
      delay(500);
      noTone(piezo);
    }
    delay(150);
    volume(thres);
  }
  delay(100);

  // guide portion
  for(int i=0; i<song.length(); i++) {
    int in = song.charAt(i) - '0';
    int pins[] = {10, 9, 8, 7, 6, 5, 4, 2}; // pins correlate to the pitch to be played next

    if(in != 8) {
      Servo1.write(in*22.5);
      while(digitalRead(pins[in]) == HIGH) {} // wait for correct note to be played
      digitalWrite(led, HIGH);
      tone(piezo, freqs[in]);
      delay(500);
      digitalWrite(led, LOW);
      noTone(piezo);
    }
    volume(thres);
  }
}

/*
 * @function volume
 * called throughout our robot's run to determine
 * whether there is sufficient lighting to play or not
 * @param thres minimum lighting
 */
void volume(int thres) {
  int light = analogRead(A0);
//  Serial.print("current light is ");    testing purposes
//  Serial.println(light);
  if(light < thres) {
    piezo = 0; // mute the piezo speaker by routing it to unwired pin
  } else {
    piezo = 11;
  }
}
