
char* latitude;
char* longitude;
char* northSouth;
char* eastWest;
char* date;
char* UTC_time;
char* speedInKnots;
char* altitude;

bool gpsError;


int answer;


void setup() {
  // Serial.println(availableMemory());
  Serial.begin(115200);                // UART baud rate
  Serial.println("init..."); 
  delay(2000);  

  switchModule();                    // switches the module ON
  delay(3000);

  answer = sendATcommand("AT+CGPS=1,1","OK",1000);    
  if (answer == 0)
    {
        do{
          Serial.println("gpsInitErr, reswitch...");
          switchModule();
          answer = sendATcommand("AT+CGPS=1,1","OK",1000);
        }
        while(answer == 0);
    }
} 

void loop() {
  gpsError = false;
  delay(5000);
  answer = sendATcommand("AT+CGPSINFO","+CGPSINFO:",1000);    // request info from GPS

  if (answer == 1){
    readGpsData();
    if(!gpsError) transmit();
  }
  else{
    Serial.println("module err");
  }
}  



// Turns cookinghacks module on.
void switchModule(){
    int onModulePin = 2;        // the pin to switch on the module (without press on button) 
    pinMode(onModulePin, OUTPUT);

    digitalWrite(onModulePin,HIGH);
    delay(2000);
    digitalWrite(onModulePin,LOW);

    uint8_t answer=0;

    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);

        // waits for an answer from the module
        while(answer == 0){    
            // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
}


//reads GPS data and stores it in char array
// parse gps must come after
void readGpsData(){
  char gpsData[60]; 
  int counter = 0;
  counter = 0;
  do{
    while(Serial.available() == 0);
    gpsData[counter] = Serial.read();
    counter++;
  }
  while(gpsData[counter - 1] != '\r');
  gpsData[counter] = '\0';
  parseGpsOrErr(gpsData);
}


void sendRequest(){
  answer = sendATcommand("AT+CHTTPACT=gps.rubyride.co,80", "+CHTTPACT: REQUEST", 60000);

  // Sends the request
  // Serial.println(request);

  //  char request[ ]="GET /index.php?a=1&b=2 HTTP/1.1\r\nHost: gps.rubyride.co\r\nContent-Length: 0\r\n\r\n";
  Serial.print("GET /index.php?");

  Serial.print("latitude=");
  Serial.print(latitude);
  Serial.print("&");

  Serial.print("northSouth=");
  Serial.print(northSouth);
  Serial.print("&");
  
  Serial.print("longitude=");
  Serial.print(longitude);
  Serial.print("&");

  Serial.print("eastWest=");
  Serial.print(eastWest);
  Serial.print("&");
  
  Serial.print("date=");
  Serial.print(date);
  Serial.print("&");
  
  Serial.print("UTC_time=");
  Serial.print(UTC_time);
  Serial.print("&");
  
  Serial.print("altitude=");
  Serial.print(altitude);
  Serial.print("&");
  
  Serial.print("speed_OG=");
  Serial.print(speed_OG);
  Serial.print(" HTTP/1.1\r\nHost: gps.rubyride.co\r\nContent-Length: 0\r\n\r\n");

  // Sends <Ctrl+Z>
  Serial.write(0x1A);
}



/*
 * Parses the GPS string into seperate variables or returns an error.
 * This must come right after read gps data function
 * vars used:
 *   latitude
 *   longitude
 *   date
 *   UTC time
 *   altitude
 *   speed
 *   knots
 */
void parseGpsOrErr(char* gpsData){
  if(gpsData[0]!=','){ // Checks if there's data in the GPS string yet.
    latitude = strtok(gpsData,",");
    northSouth = strtok(NULL, ",");
    longitude = strtok(NULL, ",");
    eastWest = strtok(NULL, ",");
    date = strtok(NULL, ",");
    UTC_time = strtok(NULL, ",");
    altitude = strtok(NULL, ",");
    speedInKnots = strtok(NULL, ",");
    // return false; // return error false
 }
 else{
  Serial.println("gpsDataErr");
  gpsError = true;
   // return true;  // return error true
 }
}    


/*
 * Print GPS variables to the serial COM
 */
void transmit(){
  delay(1000);
  Serial.print("latitude : ");
  Serial.println(latitude); 
  delay(1000);
  Serial.print("northSouth : ");
  Serial.println(northSouth); 
  delay(1000);
  Serial.print("longitude : ");
  Serial.println(longitude); 
  delay(1000);
  Serial.print("eastWest : ");
  Serial.println(eastWest); 
  delay(1000);
  Serial.print("date : ");
  Serial.println(date); 
  delay(1000);
  Serial.print("UTC_time : ");
  Serial.println(UTC_time); 
  delay(1000);
  Serial.print("speedInKnots : ");
  Serial.println(speedInKnots);
  delay(1000);
  Serial.println();
  Serial.println();
}


// Sends an AT commands to the SIM5218 and waits for a response with a timeout
int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout){
  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command 


    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{

    if(Serial.available() != 0){    
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)    
      {
        answer = 1;
      }
    }
    // Waits for the answer with time out
  }
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}




