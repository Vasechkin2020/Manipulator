#include "SoftwareSerial.h"
SoftwareSerial mySerial(9, 10); // RX, TX


#define PIN_OE 2                //Пин для отключения платы управления сервоприводами по пину OE
#define PIN_signal 3		   //Пин для пищалки
#define PIN_Blue_svet 5			//Пин зажигающий синий   светодиод
#define PIN_Green_svet 6		//Пин зажигающий зеленый светодиод
#define PIN_Red_svet 7			//Пин зажигающий красный светодиод

#define PIN_uart_rx_1 9			//Пин програмного UARTA1
#define PIN_uart_tx_1 10		//Пин програмного UARTA1
#define PIN_uart_rx_2 11		//Пин програмного UARTA2
#define PIN_uart_tx_2 12		//Пин програмного UARTA2

#define PIN__A0 14				//Пин cчитывания аналогового сигнала с сервомотора_1
#define PIN__A1 15				//Пин cчитывания аналогового сигнала с сервомотора_2
#define PIN__A2 16				//Пин cчитывания аналогового сигнала с сервомотора_3
#define PIN__A3 17				//Пин cчитывания аналогового сигнала с сервомотора_4
#define PIN__A4 18				//Пин SDA  I2C
#define PIN__A5 19				//Пин SCL  I2C
#define PIN__A6 A6				//Пин cчитывания аналогового сигнала с сервомотора_5

long time_send = 0;			 // Тря отсчета времени

int8_t reg1 = 0;	  //Регистры в которых будут хранить данные
int8_t reg2 = 0;
int8_t reg3 = 0;
int8_t reg4 = 0;
int8_t reg5 = 0;
int8_t reg6 = 0;
int8_t reg7 = 0;
int8_t reg8 = 0;
int8_t reg9 = 0;

float kReg1 = 0.01;  // Коэффициент скорости сервопривода по регистру 1
float kReg2 = 0.01;  // Коэффициент скорости сервопривода по регистру 2
float kReg3 = 0.01;  // Коэффициент скорости сервопривода по регистру 3
float kReg4 = 0.01;  // Коэффициент скорости сервопривода по регистру 4
float kReg5 = 0.01;  // Коэффициент скорости сервопривода по регистру 5



int pip_time_off = 0; // Время в какой надо выключить пищалку
long pip_time = 0; // Время в которое включили пищалку
bool flag_pip = false;       //Флаг включенной пищалки

void pip(int time_)		   //Пищалка
{
	pip_time = millis();
	pip_time_off = time_;
	analogWrite(3, 3);
	flag_pip = true;
}
void pip1()		   //Пищалка
{
	analogWrite(3, 3);
	delay(100);
	analogWrite(3, 0);

}
void pip2()		   //Пищалка
{
	analogWrite(3, 3);
	delay(100);
	analogWrite(3, 0);
	delay(100);
	analogWrite(3, 3);
	delay(100);
	analogWrite(3, 0);
}
void Pip_off()
{
	if (flag_pip = true && (millis() - pip_time > pip_time_off))
	{
		analogWrite(3, 0);	 //Выключаем пищалку
		flag_pip = false;
	}
}


void Svet(byte kod_)   // Светодиод
{
	switch (kod_)
	{
		case 0:
		{
			digitalWrite(PIN_Blue_svet, LOW);  digitalWrite(PIN_Green_svet, LOW);	digitalWrite(PIN_Red_svet, LOW);
		}
			break;
		case 1:   //Red
		{
			digitalWrite(PIN_Blue_svet, LOW);  digitalWrite(PIN_Green_svet, LOW);	digitalWrite(PIN_Red_svet, HIGH);
		}
			break;
		case 2:	  //Green
		{
			digitalWrite(PIN_Blue_svet, LOW);  digitalWrite(PIN_Green_svet, HIGH);	digitalWrite(PIN_Red_svet, LOW);
		}

			break;
		case 3:	   //Blue
		{
			digitalWrite(PIN_Blue_svet, HIGH); digitalWrite(PIN_Green_svet, LOW);	digitalWrite(PIN_Red_svet, LOW);
		}
			break;
	}
}



void Init_PIN()
{
	pinMode(PIN_signal, OUTPUT);		   //Для пищалки
	pinMode(PIN_Blue_svet, OUTPUT);		   //Синий светодиод
	pinMode(PIN_Green_svet, OUTPUT);		   //Зеленый светодиод
	pinMode(PIN_Red_svet, OUTPUT);		   //Красный светодиод

}
void I2C_test()
{
	byte error, address;
	int nDevices;

	Serial.print("Scanning...");Serial.println(millis());

	nDevices = 0;
	for (address = 8; address < 127; address++) {
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0) {
			Serial.print(" Address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println(" !");

			nDevices++;
		}
		else if (error == 4) {
			Serial.print("Unknow error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C ааdevices found\n");
	else
	{
		Serial.print("done = ");
		Serial.println(nDevices);
	}

	//delay(1000);
}


static void Timer1_Init()     // Таймер 1 на 10 милисекунд по совпадению А
{
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << WGM12);                    // Режим CTC (сброс по совпадению) 
   // TCCR5B |= (1<<CS50);                       // Тактирование от CLK. 
															  // Если нужен предделитель : 
	// TCCR5B |= (1<<CS51);                   // CLK/8 
	TCCR1B |= (1 << CS10) | (1 << CS11); // CLK/64 //          Через 4 микросекунды считает 1/(16 000 000/64)
   // TCCR5B |= (1<<CS52);                   // CLK/256 
   // TCCR5B |= (1<<CS50)|(1<<CS52); // CLK/1024 
															// Верхняя граница счета. Диапазон от 0 до 65535. 
	OCR1A = 2499;    // Умножить на 4 микросекунды                         // Частота прерываний A будет = Fclk/(N*(1+OCR5A))  На 1 меньше так как считает от 0
  //  OCR5B = 15624;                                // Частота прерываний B будет = Fclk/(N*(1+OCR5B)) 
															 // где N - коэф. предделителя (1, 8, 64, 256 или 1024) 
	TIMSK1 = (1 << OCIE1A);                   // Разрешить прерывание по совпадению A 
   // TIMSK5 |= (1<<OCIE5B);                   // Разрешить прерывание по совпадению B 
  //  TIMSK5 |= (1<<TOIE5);                     // Разрешить прерывание по переполнению 
}


bool flag_timer_AnalogRead = false;
byte count_timer_AnalogRead = 0;

ISR(TIMER1_COMPA_vect)         // Обработчик прерывания таймера 4 по совпадению A 	1 раз в 10 милисекунд
{
	count_timer_AnalogRead++;
	if (count_timer_AnalogRead >= 5) // 50 milliseconds
	{
		count_timer_AnalogRead = 0;
		flag_timer_AnalogRead = true;
	}
}



uint8_t  ReadByte_I2C(uint8_t address, int8_t registr)
{
	Wire.beginTransmission(address);
	Wire.write(registr);
	byte reza = Wire.endTransmission();
	if (reza != 0)
	{
		Serial.print("!!! ReadByte_I2C_WriteMistake reza = ");Serial.println(reza);
		//Serial2.print("!!! ReadByte_I2C_WriteMistake reza = ");Serial2.println(reza);

	};
	byte rezb = Wire.requestFrom(address, (uint8_t)1);
	if (rezb == 1)
	{
		uint8_t data = Wire.read();             //read one byte of data
		return data;
	}
	else
	{
		Serial.print("!!! ReadByte_I2C_WriteMistake rezb = ");Serial.println(rezb);
		//Serial2.print("!!! ReadByte_I2C_WriteMistake rezb = ");Serial2.println(rezb);

		return 0;
	}
}
void  WriteByte_I2C(uint8_t address, int8_t registr, uint8_t data)
{
	Wire.beginTransmission(address);
	Wire.write(registr);
	Wire.write(data);
	Wire.endTransmission();
}

void print_djoistik()
{
	Serial.print(" reg1= ");  Serial.println(reg1);
	Serial.print(" reg2= ");  Serial.println(reg2);
	Serial.print(" reg3= ");  Serial.println(reg3);
	Serial.print(" reg4= ");  Serial.println(reg4);
	Serial.print(" reg5= ");  Serial.println(reg5);
	Serial.print(" reg6= ");  Serial.println(reg6);
	Serial.print(" reg7= ");  Serial.println(reg7);
	Serial.print(" reg8= ");  Serial.println(reg8);
	Serial.print(" reg9= ");  Serial.println(reg9);
	Serial.println("---");
}

void read_djiystik()			  // Считывание данных из джойстика, примерно 2 милисекунды при 57600 скорости
{
		mySerial.write(0x1D);
		while ((mySerial.available() != 9) && (millis() - time_send) < 5)
		{
			// Ждем ответа от контроллера пока в порту не будет 9 байт или пройдет 5 милисекунд 
		}
		long time_answer = millis() - time_send;	//сомтрим сколько ждали 9 байт
		if (time_answer >= 5)
		{
			Serial.print(" Time_answer = "); Serial.println(time_answer);
		}
		if (mySerial.available() >= 9)
		{
			reg1 = mySerial.read();
			reg2 = mySerial.read();
			reg3 = mySerial.read();
			reg4 = mySerial.read();
			reg5 = mySerial.read();
			reg6 = mySerial.read();
			reg7 = mySerial.read();
			reg8 = mySerial.read();
			reg9 = mySerial.read();
			while ((mySerial.available() != 0))	 // Если вдруг там лишние байты то вычитываем их в ненужную переменную чтобы очистить буфер в порту
			{
				Serial.print(" ERROR available2= ");  Serial.println(mySerial.available());
				int8_t temp = mySerial.read();
				Serial.print(" data_error= ");  Serial.println(temp);
			}
		}
		else
		{
			Serial.print(" ERROR available3= ");  Serial.println(mySerial.available());
		}
}

