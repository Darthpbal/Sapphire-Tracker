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

  switchModulePower();                    // switches the module ON
  delay(3000);
  configureBoard();
} 


void loop() {
  gpsError = false;
  delay(5000);
  answer = sendATcommand("AT+CGPSINFO","+CGPSINFO:",1000);    // request info from GPS
  Serial.print("ans= ");
  Serial.println(answer);
  if (answer == 1){
    char* dataString = readGpsData();
    
    Serial.print("dataStr= ");
    Serial.println(dataString);

    parseGpsOrErr(dataString);

    Serial.print("gpsErr= ");
    Serial.println(gpsError);

    // prints GPS data to serial
    if(!gpsError) transmit();
    // sends GPS data to server
    if(!gpsError) sendRequest();
  }
  else{
    Serial.println("module err");
  }
}  




// Turns cookinghacks module on.
void switchModulePower(){
  Serial.println("switchModulePower");
    //pin 2 switch on the module (without pressing on-board button) 
    pinMode(2, OUTPUT);

    digitalWrite(2,HIGH);
    delay(2000);
    digitalWrite(2,LOW);

    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(2,HIGH);
        delay(3000);
        digitalWrite(2,LOW);

        // waits for an answer from the module
        while(answer == 0){    
            // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
}


void configureBoard(){
  Serial.print("config...");
  // start up the GPS system
  answer = sendATcommand("AT+CGPS=1,1","OK",1000);    
  if (answer == 0)
    {
        // This error check is usually only needed if the module is off at this point
        do{
          Serial.println("gpsInitErr, reswitch...");
          switchModulePower();
          answer = sendATcommand("AT+CGPS=1,1","OK",1000);
        }
        while(answer == 0);
    }

  // set up the wireless network
  while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );
  delay(500);
  // Set APN and IP PDP context
  sendATcommand("AT+CGSOCKCONT=1,\"IP\",\"fast.t-mobile.com\"", "OK", 2000);
  delay(500);
  // Set APN username and password
  sendATcommand("AT+CSOCKAUTH=1,1,\"\",\"\"", "OK", 2000);
  delay(500);
}


void sendRequest(){
  Serial.println("sendReq");
  answer = sendATcommand("AT+CHTTPACT=gps.rubyride.co,80", "+CHTTPACT: REQUEST", 60000);
  Serial.print("ans= ");
  Serial.println(answer);
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
  
  Serial.print("speedInKnots=");
  Serial.print(speedInKnots);
  Serial.print(" HTTP/1.1\r\nHost: gps.rubyride.co\r\nContent-Length: 0\r\n\r\n");

  // Sends <Ctrl+Z>
  Serial.write(0x1A);
}


//reads GPS data and stores it in char array
// parse gps must come after
char* readGpsData(){
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
  return gpsData;
}


/* Parses the GPS string into seperate variables or returns an error.
 * 
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
  Serial.println("parseGpsOrErr");
  Serial.println(gpsData);
  Serial.println(gpsData[0]);
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


/* Print GPS variables to the serial COM
 * 
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
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command 


  int x = 0;
  previous = millis();

  // this loop waits for the answer
  do{

    if(Serial.available() != 0){    
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)    
      {
        Serial.println(response);
        answer = 1;
        Serial.print("ans= ");
        Serial.println(answer);
        delay(500);
      }
    }
    // Waits for the answer with time out
  }
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}


int availableMemory()
{
  int size = 8192;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
} 