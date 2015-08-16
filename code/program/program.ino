int answer;
int onModulePin = 2;
int gpsError = 1;
char gpsData[100];
int counter;



void setup(){
  Serial.begin(115200);
  Serial.println("init");

  powerOn();
  delay(2000);
  configure();
  delay(1000);
}


void loop(){
  //pos:   1      2       3       4   5   6     7     8
  char latitude[13],northSouth[2],longitude[13],eastWest[2],date[7],UTC_time[9],altitude[7],speedInKnots[7];
  gpsError = readGPS();
  Serial.print("gpsError= ");
  Serial.println(gpsError);
  if (gpsError == 0)
  {
    // displayGpsData(gpsData);
    parseGps(gpsData,latitude);
    delay(1000);
    // Serial.println(latitude);
    // delay(1000);
    Serial.println(availableMemory());
    // sendRequest();
  }
  delay(500);
}


/*=================================================================================
 *=================================================================================
 *=================================================================================
 *=================================================================================
 *=================================================================================
 *=================================================================================
 *=================================================================================
 *=================================================================================*/



void powerOn(){
  answer=0;

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


void configure(){
  while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) ||
  sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );
  delay(500);
  // sets APN
  sendATcommand("AT+CGSOCKCONT=1,\"IP\",\"fast.t-mobile.com\"", "OK", 2000);
  delay(500);
  // Sets user name and password
  sendATcommand("AT+CSOCKAUTH=1,1,\"\",\"\"", "OK", 2000);
  delay(500);
  answer = sendATcommand("AT+CGPS=1,1","OK",1000);    
  if (answer == 0)
  {   
    Serial.println("Error starting the GPS");
    Serial.println("The code stucks here!!");
    while(1);
  }
}


int readGPS(){
  answer = sendATcommand("AT+CGPSINFO","+CGPSINFO:",1000);    // request info from GPS
  if (answer == 1){

    counter = 0;
    do{
      while(Serial.available() == 0);
      gpsData[counter] = Serial.read();
      counter++;
    }
    while(gpsData[counter - 1] != '\r');
    gpsData[counter] = '\0';
    if(gpsData[0] == ',')
    {
      // Serial.println("No GPS data available");
      // Serial.println("");
      return 1;
    }
    else
    {
      // Serial.print("GPS data:");
      // Serial.print(gpsData);  
      // Serial.println("");
      return 0;
    }       

  }
  else
  {
    Serial.println("noGpsAns");
    return 1;
  }
}


void displayGpsData(char gpsData[]){
  Serial.print("GPS data:");
  Serial.print(gpsData);  
  Serial.println("");
}


int sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout) {

  int x = 0;
  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (Serial.available() != 0) {
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}


void sendRequest(){
  char request[ ]="GET /index.php?a=1&b=2 HTTP/1.1\r\nHost: gps.rubyride.co\r\nContent-Length: 0\r\n\r\n";
  answer = sendATcommand("AT+CHTTPACT=\"gps.rubyride.co\",80", "+CHTTPACT: REQUEST", 60000);
  // Sends the request
  Serial.println(request);
  // Sends <Ctrl+Z>
  Serial.write(0x1A);
  delay(5000);
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
void parseGps(char gpsData[],char latitude[]/*,char northSouth[],char longitude[],char eastWest[],char date[],char UTC_time[],char altitude[],char speedInKnots[]*/){
  Serial.println("parseGpsOrErr");
  Serial.println(gpsData);
  int varStep = 0;
  int varPosition = 0;

  // for (int i = 0; i < sizeof(gpsData)-1; ++i)
  // {
  //   if(gpsData[i] == ','){
  //     latitude[varStep] = '\0';
  //     varStep = 0;
  //     varPosition++;
  //   }
  //   else{
  //     switch(varPosition){

  //       // Latitude
  //       case 0:
  //         latitude[varStep] = gpsData[i];
  //         break;
  //       default:
  //         delay(100);

  //     }
  //     varStep++;
  //   }
  // }
  int i = 0;
  while(gpsData[i] != ','){
  	latitude[varStep] = gpsData[i];
  	varStep++;
  	i++;
  }
  latitude[varStep] = '\0';



  Serial.println(latitude);
}

int availableMemory()
{
  int size = 8192;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
} 