//this is the just trying stuff sketch
// GPS Data:3325.569166,N,11214.238544,W,010815,013155.0,99.5,0

char data[60]; 
int counter,x,y; 
char* latitude;
char* longitude;
char* northSouth;
char* eastWest;
char* date;
char* UTC_time;
char* speedInKnots;
char* altitude;

void setup() {
  Serial.begin(9600);
  char string[] = "3325.569166,N,11214.238544,W,010815,013155.0,99.5,0";
  Serial.print("GPS data string size = ");
  Serial.println(sizeof(string));
  Serial.println("Started up..."); 
  Serial.println(availableMemory());
} 

void loop() {
  Serial.println("Starting loop...");
  counter = 0;
  do{
    while(Serial.available() == 0);
    data[counter] = Serial.read();
    counter++;
  }
  while(data[counter - 1] != '\r');
  data[counter] = '\0';
  Serial.println("received data:");
  Serial.println(data);
//  Serial.println("");
  parseGpsOrErr();
  displayVars();
  Serial.println();
  Serial.println();
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
  if(data[0]!=','){ // Checks if there's data in the GPS string yet.
    latitude = strtok(data,",");
    northSouth = strtok(NULL, ",");
    longitude = strtok(NULL, ",");
    eastWest = strtok(NULL, ",");
    date = strtok(NULL, ",");
    UTC_time = strtok(NULL, ",");
    altitude = strtok(NULL, ",");
    speedInKnots = strtok(NULL, ",");
  Serial.println("Done Parsing...");
 }
 else{
   Serial.println("GPS information not available, please wait...");
 }
}    


/*
 * Print GPS variables to the serial COM
 */
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
}

int availableMemory()
{
  int size = 8192;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
} 
 
