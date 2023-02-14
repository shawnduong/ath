#define R 3
#define G 9
#define B 10

#define DS 4
#define ST_CP 5
#define SH_CP 6

#define BTN_R A0
#define BTN_G A1
#define BTN_B A2

uint8_t pin[3];
int16_t color[3];
int64_t state[3];
int64_t lastT[3];
int64_t button[3];

uint16_t ctr;
uint8_t index;

uint64_t bound[3];

/* Define this to be your byte sequence. The leftmost bit is the rightmost LED. */
byte sequence[] = {
	0b0000, 0b1111
};

char buffer[256];

void setup()
{
	Serial.begin(9600);

	pinMode(DS, OUTPUT);
	pinMode(ST_CP, OUTPUT);
	pinMode(SH_CP, OUTPUT);

	pin[0] = R; pin[1] = G; pin[2] = B;
	button[0] = BTN_R; button[1] = BTN_G; button[2] = BTN_B;

	for (uint8_t i = 0; i < 3; i++)
	{
		pinMode(pin[i], OUTPUT);
		pinMode(button[i], INPUT);
		color[i] = 100;
		state[i] = 0;
		lastT[i] = 0;
		bound[i] = 1000;
	}

	Serial.println("Setup complete.");
}

void loop()
{
	/* Prevent negatives, but allow zero values with greater tolerance. */
	for (uint8_t i = 0; i < 3; i++)
	{
		if (color[i] > 0) analogWrite(pin[i], color[i]);
		else              analogWrite(pin[i], 0);
	}

	/* Read the button states. */
	for (uint8_t i = 0; i < 3; i++)
		state[i] = analogRead(button[i]);

	for (uint8_t i = 0; i < 3; i++)
	{
		if (state[i] > bound[i] && millis() > lastT[i]+500)
		{
			lastT[i] = millis();
			color[i] -= 10;
			if (color[i] < -50) color[i] = 255;

			sprintf(buffer, "Changing color[%d] value to %d. %d", i, color[i], state[i]);
			Serial.println(buffer);
		}
	}

	if (ctr >= 25)
	{
		if (index > sizeof(sequence)/sizeof(sequence[0]) - 1)  index = 0;

		sprintf(buffer, "Writing sequence index %d.", index);
		Serial.println(buffer);

		/* Clear it out. */
		digitalWrite(ST_CP, LOW);
		shiftOut(DS, SH_CP, MSBFIRST, 0b00000000);
		digitalWrite(ST_CP, HIGH);
		delay(10);

		/* Write the item. */
		digitalWrite(ST_CP, LOW);
		shiftOut(DS, SH_CP, MSBFIRST, sequence[index]);
		digitalWrite(ST_CP, HIGH);

		ctr = 0;
		index++;
	}
	else  ctr++;

	delay(100);
}
