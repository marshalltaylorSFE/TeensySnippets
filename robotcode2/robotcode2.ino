//
uint8_t FRLEDpin = 3;
uint8_t FGLEDpin = 4;
uint8_t BRLEDpin = 17;
uint8_t BGLEDpin = 16;

#define PACKET_LENGTH 10
#define START_SYMBOL '~'

char lastchar;
char packet[PACKET_LENGTH];
char lastPacket[PACKET_LENGTH];
char packetPending = 0;

char packet_ptr;

uint8_t ledSwap = 0;

// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.

void setup()
{
  Serial.begin(9600); // Initialize Serial Monitor USB
  Serial1.begin(115200); // Initialize hardware serial port, pins 0/1
  Serial2.begin(9600);
  pinMode(13, OUTPUT);
  delay(1000);
  // Send a welcome message to the serial monitor:
  Serial.println("Started");
  pinMode( FRLEDpin, OUTPUT );
  pinMode( FGLEDpin, OUTPUT );
  pinMode( BRLEDpin, OUTPUT );
  pinMode( BGLEDpin, OUTPUT );
  digitalWrite( FRLEDpin, 1 );
  digitalWrite( FGLEDpin, 0 );
  digitalWrite( BRLEDpin, 0 );
  digitalWrite( BGLEDpin, 1 );
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
  
  uint8_t changed = 0;
  
  //if the packet is full and the last char is LF or CR, *do something here*
  if((packetPending == 1) && ((packet_ptr == PACKET_LENGTH) && ((packet[PACKET_LENGTH - 1] == 0x0A) || (packet[PACKET_LENGTH - 1] == 0x0D))) )
  {
	  uint8_t stopCondition = 0;
    digitalWrite(13, digitalRead(13)^1);
    //check for new data
    packetPending = 0;
    for(int k = 0; k < PACKET_LENGTH; k++)
    {
       if(packet[k] != lastPacket[k])
      {
		  lastPacket[k] = packet[k];
		  changed = 1;
		  //Serial.println("change marked");
      } 
    }
	if( changed == 1 )
	{
		//Serial.println("changed");
		//Something changed
		if(packet[3] == '0')
		{
			//pressing forward
			if(packet[1] == '0')
			{
				//pressing left
				setDrive(9, -0.8);
			}
			else if(packet[2] == '0')
			{
				//pressing right
				setDrive(9, 0.8);
			}
			else if((packet[1] == '1') && (packet[1] == '1'))
			{
				//Just pressing forward
				setDrive(9, 0);
			}
		}
		else
		{
			stopCondition++;
		}
		if(packet[4] == '0')
		{
			//pressing forward
			if(packet[1] == '0')
			{
				//pressing left
				setDrive(-9, -0.8);
			}
			else if(packet[2] == '0')
			{
				//pressing right
				setDrive(-9, 0.8);
			}
			else if((packet[1] == '1') && (packet[1] == '1'))
			{
				//Just pressing back
				setDrive(-9, 0);
			}
		}
		else
		{
			stopCondition++;
		}
		if(packet[5] == '0')
		{
			//pressing select
			//Swap colors
			ledSwap ^= 1;
			if(ledSwap)
			{
				digitalWrite( FRLEDpin, 0 );
				digitalWrite( FGLEDpin, 1 );
				digitalWrite( BRLEDpin, 1 );
				digitalWrite( BGLEDpin, 0 );
			}
			else
			{
				digitalWrite( FRLEDpin, 1 );
				digitalWrite( FGLEDpin, 0 );
				digitalWrite( BRLEDpin, 0 );
				digitalWrite( BGLEDpin, 1 );
			}
		}
		if(stopCondition == 2)
		{
			if(packet[1] == '0')
			{
				//pressing left
				setDrive(9, -0.2);
			}
			else if(packet[2] == '0')
			{
				//pressing right
				setDrive(9, 0.2);
			}
			else
			{
				//send stop
				setDrive(0,0);
			}
		}
		changed = 0;
	}
  }
    if (Serial1.available()) // If data is sent from device
  {
    String toSend = ""; // Create a new string
    while (Serial1.available()) // While data is available
    {
      // Read from hardware port and add to the string:
      toSend += (char)Serial1.read();
    }
    // Print a message stating what we've received:
    SerialUSB.println("Received " + toSend + " from Serial1");
  }
}


void setDrive( int32_t speed, float direction)
{
	char tempSpeedCharL = '0';
	char tempSpeedCharR = '0';
	char tempSpeedDirL = 'R';
	char tempSpeedDirR = 'R';
	
	if(speed < 0)
	{
		//flip
		speed *= -1;
		tempSpeedDirL = 'F';
		tempSpeedDirR = 'F';
		direction *= 1;
	}
	
	if( direction > 0) //right
	{
		tempSpeedCharL += speed;
		tempSpeedCharR += speed*direction;
	}
	else if( direction < 0)
	{
		tempSpeedCharL += speed*(direction * -1);
		tempSpeedCharR += speed;
	}
	else //stright
	{
		tempSpeedCharL += speed;
		tempSpeedCharR += speed;
	}
	Serial1.print('1');
	Serial1.print(tempSpeedDirL);
	Serial1.print(tempSpeedCharL);
	Serial1.print('\r');
	Serial.print('1');
	Serial.print(tempSpeedDirL);
	Serial.print(tempSpeedCharL);
	Serial.print('\r');
	delay(25);
	
	Serial1.print('2');
	Serial1.print(tempSpeedDirR);
	Serial1.print(tempSpeedCharR);
	Serial1.print('\r');
	Serial.print('2');
	Serial.print(tempSpeedDirR);
	Serial.print(tempSpeedCharR);
	Serial.print('\r');
	delay(25);
}

