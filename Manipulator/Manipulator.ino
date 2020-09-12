#include "Wire.h"
#include "Vspom.h"
#include "Control_Manipul.h"



void setup() 
{
	pip1();
	Init_PIN();
	Svet(3);
	//digitalWrite(PIN_OE, 0);       // Отключаем плату

	Timer1_Init();     // Таймер 1 на  милисекунд по совпадению А

	Serial.begin(115200);
	Serial.println(" Start ");

	Wire.begin();
	Wire.setClock(400000); // скорость передачи данных 400 кБит/с.


	I2C_test();
	Init_Manipul();

	// устанавливаем скорость передачи данных для последовательного порта, созданного библиотекой SoftwareSerial
	mySerial.begin(57600);

	//delay(1000000000000000);
	Svet(2);
	pip2();
	setStartTimeIzmen();   // Начальная установка временных меток
}

void loop() 
{
	Control_Manipul(); // Управление с помощью джойстика

	Loop_Manipul2();	// Движение сервомоторов при заданном времени

	if (flag_timer_AnalogRead == true)
	{
		flag_timer_AnalogRead = false;
		//Serial.print(millis());Serial.println(" ");
		//long a = micros();
		Read_AnalogPort();
		//long b= micros();
		//Serial.print("T=");Serial.println(b-a);
	}

	Pip_off();		//Выключение пищалки в заданное время
}
