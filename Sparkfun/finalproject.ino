// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>
#include <SparkFunRHT03.h>
#include <math.h>

Servo myServo;

#define LightS A2
#define SoundS A0
#define LED D2
#define Servo D3
#define Buzzer D6
#define DustS D7
#define TouchB D4
#define Button DAC

float currHumidity;
float currTemp_F;

int currSound, currLight, didUpdate, cryThreshold, pos;

unsigned long duration, startTime, realStart, lowPulseOccupancy = 0, sampleTime_ms = 30000;  //sample 10s
float ratio = 0, concentration = 0;

const int RHT03_DATA_PIN = WKP; // RHT03 data pin
RHT03 rht;

HttpClient http;
http_header_t headers[] = {
    { "Accept" , "*/*"},
    { NULL, NULL }          // NOTE: Always terminate headers with NULL
};
http_request_t request;
http_response_t response;

String message, humidityS, tempS, soundS, lightS, airQS, isCrying;

int notesLength = 15; // the number of notes
char notes[] = "ccggaagffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

int brightness[] = {0, 50, 200};

String flags;


/* Helper Functions ---------------------------------------------------------- */

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(Buzzer, HIGH);
    delayMicroseconds(tone);
    digitalWrite(Buzzer, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playSong() {
    for (int i = 0; i < notesLength; i++) {
        if (notes[i] == ' ') {
          delay(beats[i] * tempo); // rest
        } 
        else {
          playNote(notes[i], beats[i] * tempo);
        }
        
        // pause between notes
        delay(tempo / 2);
  }
}

void fanOn() {
    for (pos = 0; pos <= 180; pos++) { // goes from 0 degrees to 180 degrees
    myServo.write(pos);             
    delay(50);                       
    }
    for (pos = 180; pos >= 0; pos--) { // goes from 180 degrees to 0 degrees
    myServo.write(pos);              
    delay(50);                       
    }
}

void adjustLight(char flag) {
    Serial.println("Adjusting light");
    Serial.println(flag);
    if(flag == '0')     analogWrite(LED, brightness[0]);
    if(flag == '1')     analogWrite(LED, brightness[1]);
    if(flag == '2')     analogWrite(LED, brightness[2]);
}

/* ------------------------------------------------------------------------------- */



void setup() {
    Serial.begin(9600);
    
    myServo.attach(Servo);      
    pinMode(LED, OUTPUT);  
    pinMode(Buzzer, OUTPUT);
    pinMode(DustS, INPUT);
    pinMode(TouchB, INPUT);
    pinMode(Button, INPUT);
    
    rht.begin(RHT03_DATA_PIN);
    
    startTime = millis();
    realStart = millis();
    
    request.hostname = "54.221.113.13";
    request.port = 5000;
}


void loop() {
    
    // Set crying threshold
    if(millis() < realStart+5000)
    {
        cryThreshold = analogRead(SoundS);
        return;
    }
    
    
    //Update Sensor Values
    
    currSound = analogRead(SoundS);
    currLight = analogRead(LightS);
    
	int didUpdate = rht.update();
	
	// If successful, the update() function will return 1 otherwise < 0
	if (didUpdate == 1)
	{
		currHumidity = rht.humidity();
		currTemp_F = rht.tempF();
	}
	else
	{
		delay(RHT_READ_INTERVAL_MS);
		return;
	}
	
	// Air Quality Sensor
	duration = pulseIn(DustS, LOW);
    lowPulseOccupancy = lowPulseOccupancy + duration;
 
    if ((millis()-startTime) > sampleTime_ms)//if the sampel time == 30s
    {
        ratio = lowPulseOccupancy/(sampleTime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        lowPulseOccupancy = 0;
        startTime = millis();
    }
	
	
    // Debug Printing (Locally)
    
    Serial.print("\n\nSurrounding Sound: ");
    Serial.println(currSound);
    
    Serial.print("Surrounding Light: ");
    Serial.println(currLight);
    
    Serial.print("Surrounding Humidity: ");
    Serial.println(currHumidity);
    
    Serial.print("Surrounding Temperature (F): ");
    Serial.println(currTemp_F);
    
    Serial.print("Surrounding Air Quality: ");
    Serial.print(lowPulseOccupancy);
    Serial.print(", ");
    Serial.print(ratio);
    Serial.print(", ");
    Serial.println(concentration);
    
    
    // Alerting System
    
    if(currSound > cryThreshold+800)
    {
        Serial.println("ALERT: The baby is crying!");
        isCrying = "1";
    }
    else
    {
        isCrying = "0";
    }
    
    
    // Create the message for sending across to AWS server
    
    humidityS = String(currHumidity, 2);
	tempS = String(currTemp_F, 2);
	airQS = String(concentration, 2);
	soundS = String(currSound);
	lightS = String(currLight);
	
    message = "/?sound=" + soundS + "&temp=" + tempS + "&humidity=" + humidityS + "&light=" + lightS + "&airQ=" + airQS + "&isCry=" + isCrying;
    request.path = message;
    
    
    // Debug Printing (Locally)
    
    Serial.println();
    Serial.println("Application>\tStart of Loop.");

    // Get request
    http.get(request, response, headers);
    flags = response.body;
    
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);

    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(flags);
    
    // Parental controls (fan, lighting, speaker)
    
    // Speaker Flag
    if(flags[0] == '1')
        playSong();
    
    // Fan flag
    if(flags[1] == '1')
        fanOn();
        
    // Light brightness flag
    adjustLight(flags[2]);
    
    
    delay(2000);
}