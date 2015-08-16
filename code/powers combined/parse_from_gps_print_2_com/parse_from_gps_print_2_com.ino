int x,y,counter,gpsError;
int led = 13;
int onModulePin = 2;        // the pin to switch on the module (without press on button) 
int8_t answer;
 
// stores the raw GPS string
char gps_data[100];

// GPS string data listed here in the order it appears in the output of the SIM5218, each var delimited by a ","
char latitude[11],northSouth[1],longitude[12],eastWest[1],date[6],UTC_time[8],altitude[6],speedInKnots[6];


const char apn[] = "fast.t-mobile.com";
const char user_name[] = "";
const char password[] = "";


void setup(){
  Serial.begin(115200);                // UART baud rate
  delay(2000);
  pinMode(led, OUTPUT);
  pinMode(onModulePin, OUTPUT);
  switchModule();                    // switches the module ON
  delay(5000);
  answer = sendATcommand("AT+CGPS=1,1","OK",1000);    
  if (answer == 0)
    {
        Serial.println("gpsInitErr. Stopping.");
        while(1);
    }
}


void loop(){
  gpsError = false;
  delay(5000);
  answer = sendATcommand("AT+CGPSINFO","+CGPSINFO:",1000);    // request info from GPS

  if (answer == 1){
    
    readGpsData();
    parseGpsOrErr();
    if(gpsError) break;
    displayVars();
  }
  else{
    Serial.println("Error, no answer from module/...");
  }
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



// displays stored variables.
void displayVars(){
  Serial.print("latitude : ");
  Serial.println(latitude);

  Serial.print("northSouth : ");
  Serial.println(northSouth);
  
  Serial.print("longitude : ");
  Serial.println(longitude);

  Serial.print("eastWest : ");
  Serial.println(eastWest);
  
  Serial.print("date : ");
  Serial.println(date);
  
  Serial.print("UTC_time : ");
  Serial.println(UTC_time);
  
  Serial.print("speedInKnots : ");
  Serial.println(speedInKnots);
  
  Serial.println("");
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
void parseGpsOrErr(){
    Serial.print("Data String:||");
    Serial.print(gps_data);
    Serial.println("||");
    
    x=0;
    y=0;

    if(gps_data[x]!=','){
      Serial.println("parsing...");
    //   // stores latitude
    //    do{
    //        latitude[y]=gps_data[x];
    //        y++;
    //        x++;        
    //    }
    //    while(gps_data[x]!=',');
    //    x++;

    //    // stores northSouth direction
    //    y=0;
    //    northSouth[y] = gps_data[x];

    //    x+=2;
    //    y=0;
    //    // stores longitude
    //    do{
    //        longitude[y]=gps_data[x];
    //        y++;
    //        x++; 
    //    }
    //    while(gps_data[x]!=',');


    //    x++;

    //    // stores eastWest direction
    //    y=0;
    //    eastWest[y] = gps_data[x];

    //    x+=2;
    //    y=0;
    //    //stores the date
    //    do{
    //        date[y]=gps_data[x];
    //        y++;
    //        x++;        
    //    }
    //    while(gps_data[x]!=',');
    //    x++;

    //    y=0;
    //    //stores TC time
    //    do{
    //        UTC_time[y]=gps_data[x];
    //        y++;
    //        x++;        
    //    }
    //    while(gps_data[x]!=',');
    //    x++;

    //    y=0;
    //    // stores altitude
    //    do{
    //        x++;        
    //    }
    //    while(gps_data[x]!=',');
    //    x++; 

    //    y=0;
    //    // store speed
    //    do{
    //        speedInKnots[y]=gps_data[x];
    //        // Serial.print(gps_data[x]);
    //        y++;
    //        x++;        
    //    }
    //    while(gps_data[x]!=0x0D);
    }
    else{
        gpsError = true;
        Serial.println("noGpsDataYet");
    }   
    Serial.print("lon size=");
    Serial.println(size(longitude));
}



// Turns cookinghacks module on.
void switchModule(){
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
  counter = 0;
  do{
    while(Serial.available() == 0);
    gps_data[counter] = Serial.read();
    counter++;
  }
  while(gps_data[counter - 1] != '\r');
  gps_data[counter] = '\0';
  Serial.println("Parsing...");
}