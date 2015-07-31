
 
int led = 13;
int onModulePin = 2;        // the pin to switch on the module (without press on button) 
 
char data[255];
char latitude[11],longitude[12];
char date[6],UTC_time[8];
char speed_OG[6],altitude[6];
 
int x,y;

 
void setup(){
 
    Serial.begin(115200);                // UART baud rate
    delay(2000);
    pinMode(led, OUTPUT);
    pinMode(onModulePin, OUTPUT);
    switchModule();                    // switches the module ON
 
    for (int i=0;i< 5;i++){
        delay(5000);
    } 
 
    Serial.println("AT+CGPS=1,1");         // starts GPS session in stand-alone mode
    delay(100);
}
 
void loop(){
 
    delay(5000);
    Serial.println("AT+CGPSINFO"); // request GPS info
    Serial.flush();
    
    // counter used for stepping through the data char
    x=0;

    // Read GPS data and end on a 'K' when the character should be a comma following
    readGpsData();
 
    x=24;
    y=0;
    parseGpsOrErr();

    // print longitude var
    Serial.print("Lon var = ");
    Serial.print(longitude);

    // print longitude var
    Serial.print("Lon var = ");
    Serial.print(longitude);
}


// TUrns cookinghacks module on.
void switchModule(){
    digitalWrite(onModulePin,HIGH);
    delay(2000);
    digitalWrite(onModulePin,LOW);
}


//reads GPS data and stores it in char array
void readGpsData(){
    do{
        do{
            digitalWrite(led,HIGH);  
        }
        while(Serial.available()==0);
        digitalWrite(led,LOW);
        data[x]=Serial.read();  
        x++;                        
    }
    while(Serial.read()!='K');
}


/*
 * Parses the GPS string into seperate variables or returns an error.
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
    if(data[x]!=','){
        Serial.print("Latitude: ");    //shows actual latitude
        do{
            latitude[y]=data[x];
            Serial.print(data[x]);
            y++;
            x++;        
        }
        while(data[x]!=',');
        x++;
        Serial.print(" ");
        Serial.println(data[x]);    //north or south
 
        x+=2;
        y=0;
        Serial.print("Longitude: ");    //shows actual longitude
        do{
            longitude[y]=data[x];
            Serial.print(data[x]);
            y++;
            x++; 
        }
        while(data[x]!=',');


        x++;
        Serial.print(" ");
        Serial.println(data[x]);    //west or east
 
        x+=2;
        y=0;
        Serial.print("Date: ");    //shows date
        do{
            date[y]=data[x];
            Serial.print(data[x]);
            y++;
            x++;        
        }
        while(data[x]!=',');
        x++;
 
        y=0;
        Serial.print("\r\nUTC time: ");    //shows UTC time
        do{
            UTC_time[y]=data[x];
            Serial.print(data[x]);
            y++;
            x++;        
        }
        while(data[x]!=',');
        x++;
 
        y=0;
        Serial.print("\r\nAltitude: ");    //shows altitude
        do{
            UTC_time[y]=data[x];
            Serial.print(data[x]);
            y++;
            x++;        
        }
        while(data[x]!=',');
        Serial.println(" m");
        x++; 
 
        y=0;
        Serial.print("Speed: ");    //shows speed
        do{
            speed_OG[y]=data[x];
            Serial.print(data[x]);
            y++;
            x++;        
        }
        while(data[x]!=0x0D);
        Serial.println(" knots");
    }
    else{
        Serial.println("GPS information not available, please wait...");
    }   
}  