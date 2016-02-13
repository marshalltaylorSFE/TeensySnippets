#define PACKET_LENGTH 10
#define START_SYMBOL '~'

char lastchar;
char packet[PACKET_LENGTH];
char lastPacket[PACKET_LENGTH];
char packetPending = 0;

char packet_ptr;

void setup()
{
  Serial.begin(9600); // Initialize Serial Monitor USB
  Serial1.begin(115200); // Initialize hardware serial port, pins 0/1
  Serial2.begin(9600);
  
  
  while (!Serial) ; // Wait for Serial monitor to open

  // Send a welcome message to the serial monitor:
  Serial.println("Started");
}

void loop()
{
  if (Serial2.available())
  {
    lastchar = Serial2.read();
    //look for packet start (START_SYMBOL)
    if( lastchar == START_SYMBOL )
    {
      //Flag that the packet needs to be serviced
      packetPending = 1;
      
      //Fill packet with null, reset the pointer
      for( int i = 0; i < PACKET_LENGTH; i++ )
      {
        packet[i] = 0;
      }
      //write the start char
      packet[0] = START_SYMBOL;
      //reset the pointer
      packet_ptr = 1;
    }
    else
    if( ( packet_ptr < PACKET_LENGTH ) && (packet_ptr > 0) )//check for room in the packet, and that the start char has been seen
    {
      //put the char in the packet
      packet[packet_ptr] = lastchar;
      //advance the pointer
      packet_ptr++;
      //turn on LED
    }
    else
    {
      //Just overwrite to the last position
      packet[PACKET_LENGTH - 1] = lastchar;
    }
  }
  
  //if the packet is full and the last char is LF or CR, *do something here*
  if((packetPending == 1) && ((packet_ptr == PACKET_LENGTH) && ((packet[PACKET_LENGTH - 1] == 0x0A) || (packet[PACKET_LENGTH - 1] == 0x0D))) )
  {
    digitalWrite(13, digitalRead(13)^1);
    //check for new data
    packetPending = 0;
    for(int k = 0; k < PACKET_LENGTH; k++)
    {
       if(packet[k] != lastPacket[k])
      {
        lastPacket[k] = packet[k];
        switch( k )
		{
			case 1:  //left
				if(packet[k] == '1')
				{
				//Do off things
				Serial1.print("2F0\r");
				delay(25);
				Serial1.print("1F0\r");
				delay(25);
				}
				if(packet[k] == '0')
				{
				//Do on things
				Serial1.print("2R9\r");
				delay(25);
				Serial1.print("1F9\r");
				delay(25);
				}
				break;
			case 2: //right
				if(packet[k] == '1')
				{
				//Do off things
				Serial1.print("2F0\r");
				delay(25);
				Serial1.print("1F0\r");
				delay(25);
				}
				if(packet[k] == '0')
				{
				//Do on things
				Serial1.print("2F9\r");
				delay(25);
				Serial1.print("1R9\r");
				delay(25);
				}
			break;
			case 3:
				if(packet[k] == '1')
				{
				//Do off things
				Serial1.print("2F0\r");
				delay(25);
				Serial1.print("1F0\r");
				delay(25);
				}
				if(packet[k] == '0')
				{
				//Do on things
				Serial1.print("2R7\r");
				delay(25);
				Serial1.print("1R9\r");
				delay(25);
				}
			break;
			case 4:
				if(packet[k] == '1')
				{
				//Do off things
				Serial1.print("2R0\r");
				delay(25);
				Serial1.print("1R0\r");
				delay(25);
				}
				if(packet[k] == '0')
				{
				//Do on things
				Serial1.print("2F7\r");
				delay(25);
				Serial1.print("1F9\r");
				delay(25);
				}
			break;
			case 5:
			break;
			case 6:
			break;
			case 7:
			break;
			case 8:
			break;
			default:
			break;
		}
      } 
    }
  }
  
//  if (SerialUSB.available()) // If data is sent to the monitor
//  {
//    String toSend = ""; // Create a new string
//    while (SerialUSB.available()) // While data is available
//    {
//      // Read from SerialUSB and add to the string:
//      toSend += (char)SerialUSB.read();
//    }
//    // Print a message stating what we're sending:
//    SerialUSB.println("Sending " + toSend + " to Serial1");
//
//    // Send the assembled string out over the hardware
//    // Serial1 port (TX pin 1).
//    Serial1.print(toSend);
//  }
//
//  if (Serial.available()) // If data is sent from device
//  {
//    String toSend = ""; // Create a new string
//    while (Serial.available()) // While data is available
//    {
//      // Read from hardware port and add to the string:
//      toSend += (char)Serial.read();
//    }
//    // Print a message stating what we've received:
//    SerialUSB.println("Received " + toSend + " from Serial1");
//  }
//  Serial1.print("1F5\r");
//  delay(1000);
//  Serial1.print("1F0\r");
//  delay(100);
//  Serial1.print("1R5\r");
//  delay(1000);
//  Serial1.print("1R0\r");
//  delay(100);
//  Serial1.print("2F5\r");
//  delay(1000);
//  Serial1.print("2F0\r");
//  delay(100);
//  Serial1.print("2R5\r");
//  delay(1000);
//  Serial1.print("2R0\r");
//  delay(1000);
}