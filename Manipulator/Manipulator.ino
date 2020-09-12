#include "Wire.h"
#include "Vspom.h"
#include "Control_Manipul.h"



void setup() 
{
	pip1();
	Init_PIN();
	Svet(3);
	//digitalWrite(PIN_OE, 0);       // ��������� �����

	Timer1_Init();     // ������ 1 ��  ���������� �� ���������� �

	Serial.begin(115200);
	Serial.println(" Start ");

	Wire.begin();
	Wire.setClock(400000); // �������� �������� ������ 400 ����/�.


	I2C_test();
	Init_Manipul();

	// ������������� �������� �������� ������ ��� ����������������� �����, ���������� ����������� SoftwareSerial
	mySerial.begin(57600);

	//delay(1000000000000000);
	Svet(2);
	pip2();
	setStartTimeIzmen();   // ��������� ��������� ��������� �����
}

void loop() 
{
	Control_Manipul(); // ���������� � ������� ���������

	Loop_Manipul2();	// �������� ������������ ��� �������� �������

	if (flag_timer_AnalogRead == true)
	{
		flag_timer_AnalogRead = false;
		//Serial.print(millis());Serial.println(" ");
		//long a = micros();
		Read_AnalogPort();
		//long b= micros();
		//Serial.print("T=");Serial.println(b-a);
	}

	Pip_off();		//���������� ������� � �������� �����
}
