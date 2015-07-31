
//Change here your data
const char apn[] = "fast.t-mobile.com";
const char user_name[] = "";
const char password[] = "";

char url[ ]="gps.rubyride.co";
int port= 80;
char request[ ]="GET /index.php?hello=world HTTP/1.1\r\nHost: gps.rubyride.co\r\nContent-Length: 0\r\n\r\n";


int8_t answer;
int onModulePin = 2, aux;
int data_size = 0;
int end_file = 0;
char aux_str[50];
char data[400];
int http_x;
int http_status; // 0: end, 1 waiting data with header, 2 waiting data without header, 3 timeout waiting data, 4 unknow response
int x = 0;
long previous;



void setup(){
  pinMode(onModulePin, OUTPUT);
  Serial.begin(115200);

  Serial.println("Starting...");
  power_on();

  delay(3000);

  while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) ||
    sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

  // set APN
  sprintf(aux_str, "AT+CGSOCKCONT=1,\"IP\",\"%s\"", apn);
  sendATcommand(aux_str, "OK", 2000);

  // Set username and password
  sprintf(aux_str, "AT+CSOCKAUTH=1,1,\"%s\",\"%s\"", user_name, password);
  sendATcommand(aux_str, "OK", 2000);

  delay(3000);

  // starts GPS session in stand alone mode
  answer = sendATcommand("AT+CGPS=1,1","OK",1000);    
  if (answer == 0)
  {
    Serial.println("Error starting the GPS");
    Serial.println("The code stucks here!!");
    while(1);
  }
}



void loop(){

  // request the url
  previous = millis();
  sprintf(aux_str, "AT+CHTTPACT=\"%s\",%d", url, port);
  answer = sendATcommand(aux_str, "+CHTTPACT: REQUEST", 60000);

  // Sends the request
  Serial.println(request);
  // Sends <Ctrl+Z>
  Serial.write(0x1A);

  previous = millis() - previous;
  
  Serial.print(F("previous in DEC: "));
  Serial.println(previous, DEC);
  

  delay(10000);

}



void power_on(){

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

int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
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

int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout)
{

  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

 // while( Serial.available() > 0) Serial.read();    // Clean the input buffer

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
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer2) != NULL)
      {
        answer = 2;
      }
    }
    // Waits for the asnwer with time out
  }
  while((answer == 0) && ((millis() - previous) < timeout));

  return answer;
} 


/*
 * This function takes the gps data and builds an http request
*/
void buildRequest()
{
  digitalWrite(13, HIGH);
  //
}

/*
 * This function sorts out the GPS data
*/
void parseGPS()
{
  digitalWrite(13, HIGH);
  //
}

void gps version main loop
{
  answer = sendATcommand("AT+CGPSINFO","+CGPSINFO:",1000);    // request info from GPS
  if (answer == 1)
  {

    counter = 0;
    do{
      while(Serial.available() == 0);
      gps_data[counter] = Serial.read();
      counter++;
    }
    while(gps_data[counter - 1] != '\r');
    gps_data[counter] = '\0';
    if(gps_data[0] == ',')
    {
      Serial.println("No GPS data available");
      Serial.println("");
    }
    else
    {
      Serial.print("GPS data:");
      Serial.print(gps_data);  
      Serial.println("");
    }       

  }
  else
  {
    Serial.println("Error"); 
  }

  delay(5000);
}