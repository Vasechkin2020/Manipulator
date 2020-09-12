

#include "Adafruit_PWMServoDriver.h"
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 60 // Analog servos run at ~50 Hz updates
#define SERVO_Harakter ((float)1000000/SERVO_FREQ/4096.0) // ���-�� ����������� ������� ��������� �������� ������ �� 1 ������ ��������. ������� �� ������� ������� � ����������� ����������� ����� ����������


struct Struct_Manipul_ServoMotor
{
	byte port;	 //���� ����� �������� ���������� �������������
	byte pin;	 //���������� ��� �� �������� �������� ����� �� �������������
	int ugol;    //���� �� ������� �������������� �����
	int ugol_max;    //���� �� ������� ����� ������������� ����� � ����
	int ugol_min;    //���� �� ������� ����� ������������� ����� � ����
	int min_impuls;				 //����������� ������ �� �������� �������� 500 
	int max_impuls;				 //������������ �������	 �� ��������  �������� 2500
	float position_raschet;			//������� ���������		��� �� 	��������� ��������� ��� ������������� (�������)  � ��������	
	float position_target;		//��������� � ������� ���� ������������	  � ��������
	float position_potenciometr;		//��������� �� ������ �������������	  � ��������
	int min_potenciom;	   // �������� ������������ ��� ������� ���������
	int max_potenciom;   // �������� ������������ ��� ������� ���������

	float time_pulse = 0;		   //����� ��� �������� �� 1 ������

	int SERVOMIN;                 // ����������� ��������� 	 ������� ���	 500 / (1000000/60/4096)  � �������� ��������
	int SERVOMAX;	              // ������������ ��������� 	 ������� ���	 1000000/60/4096*2500	 � �������� ��������
	int SERVOSER;	              // �������  ��������� 	0 ��������  	 � �������� ��������
	int SERVOTEK;	              // �������  ��������� 	 � �������� ��������
	int SERVOZAD;	              // �������� ��������� 	 � �������� ��������
	float SERVOTIME;		      //��������� ��������� ��� ��� �������� �� 1 ������  � �������� ��������

	long time_izmen = 0;                // ����� ����� ������ ��������� ��� ����
};

byte const col_mot = 5;   // ���-�� ������� �� ������������

Struct_Manipul_ServoMotor Manipul[col_mot];		  // ������ �� 6 ����������



void set_servo_Position(byte num_servo, float gradus)
{
	Manipul[num_servo].SERVOZAD = Manipul[num_servo].SERVOSER - (gradus * Manipul[num_servo].SERVOTIME);
	pwm.setPWM(Manipul[num_servo].port, 0, Manipul[num_servo].SERVOZAD);

	Manipul[num_servo].SERVOTEK = Manipul[num_servo].SERVOZAD;
	Manipul[num_servo].position_raschet = gradus;
	Manipul[num_servo].position_target = gradus;
	//Serial.print(" polojenie= ");	Serial.println(Manipul[num_servo].SERVOZAD);

}
void set_servo_Position_speed(byte num_servo, int8_t speed_, float koeff_)
{
	float gradus = Manipul[num_servo].position_target + speed_ * koeff_;		// ����� ������ ���� � ����������� ��� �� ������� �������� ��������������� ���������� ��������� � ������������
	if (gradus > Manipul[num_servo].ugol_max)		  // ���� ���� ��������� �������� ����� ������������ ����
	{
		gradus = Manipul[num_servo].ugol_max;
	}
	if (gradus < Manipul[num_servo].ugol_min)		  // ���� ���� ��������� �������� ����� ������������ ����
	{
		gradus = Manipul[num_servo].ugol_min;
	}
	Serial.print(" position_target= ");	Serial.println(Manipul[num_servo].position_target);
	Serial.print(" gradus= ");	Serial.println(gradus);

	set_servo_Position(num_servo, gradus);
}


void set_servo_Position_time(byte num_servo, float gradus, float time) // ����� � ��������
{
	//Serial.print(" num_servo= ");	Serial.println(num_servo);
	//Serial.print(" SERVOTIME= ");	Serial.println(Manipul[num_servo].SERVOTIME);
	//Serial.print(" SERVOTEK= ");	Serial.println(Manipul[num_servo].SERVOTEK);
	float dif_pulse = abs((gradus - Manipul[num_servo].position_raschet) * Manipul[num_servo].SERVOTIME);
	//Serial.print(" dif_pulse= ");	Serial.println(dif_pulse);
	Manipul[num_servo].time_pulse = (time * 1000000) / dif_pulse;		   //����� ���� ���������� ��� ���������� ���������� �� 1 �������
	//Serial.print(" time_pulse= ");	Serial.println(Manipul[num_servo].time_pulse);

	Manipul[num_servo].position_target = gradus;			  //������ ����� ���������	  � ��������
	Manipul[num_servo].SERVOZAD = Manipul[num_servo].SERVOSER - (gradus * Manipul[num_servo].SERVOTIME);
	Manipul[num_servo].time_izmen = micros();  // ���������� ����� � ������� ���� ���� �������

}

//     ����� �����, ��� �� ������� ���������� �����, �� ����� ���� ������������� �����, ����������� �������� ������� � ������������� (500), ����������� �������� ������������� ��� ����������� �������, ������������ �������� ������������� ���� ����������� ������� 2500
void setServoConfig(byte port_, byte pin_, int ugol_, int ugol_max_, int ugol_min_, int min_pulse_, int min_potenciom_, int max_potenciom_)
{
	Manipul[port_].port = port_;
	Manipul[port_].pin = pin_;
	Manipul[port_].min_potenciom = min_potenciom_;
	Manipul[port_].max_potenciom = max_potenciom_;

	Manipul[port_].ugol = ugol_;
	Manipul[port_].ugol_max = ugol_max_;
	Manipul[port_].ugol_min = ugol_min_;
	Manipul[port_].min_impuls = min_pulse_;
	Manipul[port_].max_impuls = Manipul[port_].min_impuls + 2000;
	Manipul[port_].SERVOMIN = Manipul[port_].min_impuls / SERVO_Harakter;	//   Serial.print (" SERVOMIN= ");	Serial.print(Manipul[0].SERVOMIN);
	Manipul[port_].SERVOMAX = Manipul[port_].max_impuls / SERVO_Harakter;	 //  Serial.print (" SERVOMAX= ");	Serial.print(Manipul[0].SERVOMAX);
	Manipul[port_].SERVOTIME = (Manipul[port_].SERVOMAX - Manipul[port_].SERVOMIN) / (float)Manipul[port_].ugol;	 // Serial.print (" SERVOTIME= ");	Serial.print(Manipul[0].SERVOTIME,4);
	Manipul[port_].SERVOSER = Manipul[port_].SERVOMIN + (Manipul[port_].SERVOTIME * (Manipul[port_].ugol / 2)); //  Serial.print (" SERVOSER= ");	Serial.println(Manipul[0].SERVOSER);
	Manipul[port_].SERVOSER = Manipul[port_].SERVOMIN + (Manipul[port_].SERVOMAX - Manipul[port_].SERVOMIN) / 2; //  Serial.print (" SERVOSER= ");	Serial.println(Manipul[0].SERVOSER);

	int zn = 0;
	zn = analogRead(Manipul[port_].pin);

	Serial.print("setServoConfig port");Serial.print(port_);Serial.print(" = "); Serial.print(zn);    // ���������� ���� ��� ���������� ��������� �����������

	Manipul[port_].position_potenciometr = map(zn, min_potenciom_, max_potenciom_, Manipul[port_].ugol / 2, -Manipul[port_].ugol / 2);;
	Manipul[port_].position_raschet = Manipul[port_].position_potenciometr;   // ��� ��������� ������� ��������� ����� ���������� �� �������������
	Manipul[port_].SERVOTEK = Manipul[port_].SERVOSER - (Manipul[port_].position_raschet * Manipul[port_].SERVOTIME);
	Serial.print(" Start Angle = "); Serial.println(Manipul[port_].position_raschet);    // ���������� ���� ��� ���������� ��������� �����������

}
void Read_AnalogPort()
{
	for (size_t port = 0; port < 4; port++)
	{
		int zn = 0;
		for (byte i = 0; i < 3; i++)
		{
			zn += analogRead(Manipul[port].pin);
		}
		zn = zn / 3;
		
		Manipul[port].position_potenciometr = map(zn, Manipul[port].min_potenciom, Manipul[port].max_potenciom, Manipul[port].ugol *0.5, -Manipul[port].ugol *0.5);
	//	Manipul[port].position_potenciometr = Manipul[port].position_potenciometr*0.0 + temp_position * 1.0;  //���������� ��������
		if (port == 3)
		{
			//Serial.print(" ");Serial.print(port);Serial.print(" = "); Serial.print(zn);    // ���������� ���� ��� ���������� ��������� �����������
			//Serial.print(" = "); Serial.println(Manipul[port].position_potenciometr);    // ���������� ���� ��� ���������� ��������� �����������
		}
	}
}
void Init_Manipul()
{
	Serial.println("Set Init_Manipul");
	pwm.begin();				//��� ������������ � ���������� �������������� ������� �� i2c 0x70
	pwm.setPWMFreq(SERVO_FREQ);  // ������� ���������� ��������� 60 ��
	
	Serial.println("Set PIN");
	pinMode(PIN__A0, INPUT);  analogRead(PIN__A0);	  // ����� ����� ������� ���� �.�. ������ ����� ���-�� ������ �� ������������� � �� ����� ������� ���������.
	pinMode(PIN__A1, INPUT);  analogRead(PIN__A1);
	pinMode(PIN__A2, INPUT);  analogRead(PIN__A2);
	pinMode(PIN__A3, INPUT);  analogRead(PIN__A3);
	pinMode(PIN__A6, INPUT);  analogRead(PIN__A6);

	delay(500);

	// �������� ��������� �������
	Serial.println("Set config");
	setServoConfig(0, PIN__A0, 270, 135, -135, 530, 55, 595);
	setServoConfig(1, PIN__A1, 270, 100, -100, 510, 560, 30);
	setServoConfig(2, PIN__A2, 270, 120, -120, 510, 560, 30);
	setServoConfig(3, PIN__A3, 270, 120, -120, 500, 50, 600);
	setServoConfig(4, PIN__A6, 180, 120, -120, 500, 560, 30);

	delay(500);
	Read_AnalogPort();

	Serial.println("Set start position");
	//set_servo_Position(0, 60);   // 	������ � ��������� ���������  � ������������ ���������
	//set_servo_Position(1, 60);   // 	������ � ��������� ���������  � ������������ ���������
	//delay(2000);
	set_servo_Position_time(0, 0, 3);	 //������� ������ � �������� ��������� ��������� �� ������ ����� � ����
	set_servo_Position_time(1, 30, 3);	 //������� ������ � �������� ��������� ��������� �� ������ ����� � ����
	set_servo_Position_time(2, -60, 3);	 //������� ������ � �������� ��������� ��������� �� ������ ����� � ����
	set_servo_Position_time(3, 30, 3);	 //������� ������ � �������� ��������� ��������� �� ������ ����� � ����
	set_servo_Position_time(4, 0, 3);	 //������� ������ � �������� ��������� ��������� �� ������ ����� � ����

	//delay(1000);
	//Read_AnalogPort();

	//set_servo_Position_time(1, 0, 2);	 //������� ������ � �������� ��������� ��������� �� ������ ����� � ����


	//delay(100000000000000);
}

void Loop_Manipul()
{
	for (byte i = 0; i < 2; i++)
	{
		if (Manipul[i].SERVOTEK != Manipul[i].SERVOZAD)	   //���� ��������� �� ���������, �����
		{
			if (micros() - Manipul[i].time_izmen > Manipul[i].time_pulse)			 // ���� ����� � �������� ��������� ���� ������ ��� ����� ����
			{
				Manipul[i].time_izmen = micros();					 //���������� ����� ��������� ����
				//Serial.print(" time= ");Serial.print(i); Serial.print("  ");	Serial.println(Manipul[i].time_izmen);
				if (Manipul[i].SERVOZAD > Manipul[i].SERVOTEK)
				{
					Manipul[i].SERVOTEK++;      // ����������� �� 1 ������
				}
				else
				{
					Manipul[i].SERVOTEK--;      // ��������� �� 1 ������
				}
				pwm.setPWM(i, 0, Manipul[i].SERVOTEK);			  // ���� �������  �� ������� �� 1 �������
				Manipul[i].position_raschet = (Manipul[i].SERVOSER - Manipul[i].SERVOTEK) / Manipul[i].SERVOTIME;	//��������� ��������� � ��������
				float zn = analogRead(Manipul[i].pin);
				Serial.print("port");Serial.print(i);Serial.print(" = "); Serial.print(zn);    // ���������� ���� ��� ���������� ��������� �����������
				float zd = map(zn, 50, 600, 135, -135);
				Serial.print(" Angle = "); Serial.print(zd);    // ���������� ���� ��� ���������� ��������� �����������

				Serial.print(" position_raschet= ");	Serial.println(Manipul[i].position_raschet);

			}
		}
	}
}
void setStartTimeIzmen()
{
	for (byte i = 0; i < col_mot; i++)
	{
		Manipul[i].time_izmen = micros();	//������������� ����� ������� � ����� ����� ������ ����� �����
	}
}

void Loop_Manipul2()
{
	for (byte i = 0; i < col_mot; i++)
	{
		if (Manipul[i].SERVOTEK != Manipul[i].SERVOZAD)	   //���� ��������� �� ���������, �����
		{
			long time_proshlo = micros() - Manipul[i].time_izmen;
			//Serial.print(" time_proshlo = "); Serial.println(time_proshlo);
			//Serial.print(" time_pulse = "); Serial.println(Manipul[i].time_pulse);

			if (time_proshlo > Manipul[i].time_pulse)
			{
				int shag = time_proshlo / Manipul[i].time_pulse;

				if (shag >= 1)			
				{
					Manipul[i].time_izmen = micros();	
					//Serial.print(" time= ");Serial.print(i); Serial.print("  ");	Serial.println(Manipul[i].time_izmen);
					if (Manipul[i].SERVOZAD > Manipul[i].SERVOTEK)
					{
						Manipul[i].SERVOTEK = Manipul[i].SERVOTEK + shag; 
						if (Manipul[i].SERVOTEK > Manipul[i].SERVOZAD) { Manipul[i].SERVOTEK = Manipul[i].SERVOZAD; }
					}
					else
					{
						Manipul[i].SERVOTEK = Manipul[i].SERVOTEK - shag; 
						if (Manipul[i].SERVOTEK < Manipul[i].SERVOZAD) { Manipul[i].SERVOTEK = Manipul[i].SERVOZAD; }

					}
					//Serial.print(" SERVOTEK= ");Serial.print(i); Serial.print("  ");	Serial.println(Manipul[i].SERVOTEK);
					float zn = analogRead(Manipul[i].pin);
					//Serial.print(" port");Serial.print(i);Serial.print("= "); Serial.print(zn);  
					//Serial.print(" position_raschet= ");	Serial.println(Manipul[i].position_raschet);
					//Serial.print(" Funcia");Serial.print(i);Serial.print("= "); Serial.println((zn-325)*0.5);

					Read_AnalogPort();

					pwm.setPWM(i, 0, Manipul[i].SERVOTEK);			 
					Manipul[i].position_raschet = (Manipul[i].SERVOSER - Manipul[i].SERVOTEK) / Manipul[i].SERVOTIME;	
					//float zd = map(zn, 50, 600, 135, -135);
					//Serial.print(" Angle = "); Serial.print(zd);  
					//Serial.print(" shag = "); Serial.print(shag);

				}
			}
		}
	}
}

void Control_Manipul()
{
	if (millis() - time_send > 50)   // ��������� 1 ��� � 50 ����������   ����� ���������� �������� 2 �����������
	{
		long a = micros();
		time_send = millis();
		read_djiystik();  // ����������� ������ �� ���������. 9 ���� ��� �������� ���������
		long c = micros();
		Serial.print(" time_read_data_djiystik= "); Serial.println(c - a);

		if (reg7 == 0 )			   // ������ �� ������ ���������   �����
		{
			set_servo_Position_speed(4, -100, kReg5);
		}
		if (reg9 == 0)			   // ������ �� ������ ���������   �������
		{
			set_servo_Position_speed(4, 100, kReg5);
		}

		if (reg8 == 0)			   // ������ �� ������� ���������
		{
			pip(1000);
			set_servo_Position_time(0, 0, 2);// ����������� � ������� ������� �� 2 �������
			set_servo_Position_time(1, 0, 2);

		}

		if (reg2 > 5 || reg2 < -5)        //��������� ���� ��������� �� ��������� ����������� � �� ����� ���� ������� ���� �� -5 �� 5
		{
			set_servo_Position_speed(0, reg2, kReg2);
		}						                                                                                                                                                                                                     

		if (reg1 > 5 || reg1 < -5)        // 1 ����� ���� ��������� �� ��������� ����������� � �� ����� ���� ������� ���� �� -5 �� 5
		{
			set_servo_Position_speed(1, reg1, kReg1);
		}
		if (reg3 > 5 || reg3 < -5)        // 2 ����� ���� ��������� �� ��������� ����������� � �� ����� ���� ������� ���� �� -5 �� 5
		{
			set_servo_Position_speed(2, reg3, kReg3);
		}
		if (reg5 > 5 || reg5 < -5)        // 2 ����� ���� ��������� �� ��������� ����������� � �� ����� ���� ������� ���� �� -5 �� 5
		{
			set_servo_Position_speed(3, reg5, kReg4);
		}
		if (reg6 > 5 || reg6 < -5)        // 2 ����� ���� ��������� �� ��������� ����������� � �� ����� ���� ������� ���� �� -5 �� 5
		{
		}

		print_djoistik();   // ������ ������ ������� ��������
	}

}
