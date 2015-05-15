/*	���͵����� �ʽð� ������Ʈ
	(��)Ž�� ��α� ��ǰ���ߴ��
		mingyu@tamseng.co.kr
	2015/05/08 v0.01
		- �⺻ ���� ����
	2015/05/11 v0.02
		- ���� ����
	2015/05/15 v0.03
		- Ÿ�̸� ���� �߰�
*/

/*
�Ƶ��̳� ������������ �̿��ϸ� 4�ڸ��� FND���÷��̸� BCD->7Segment ���ڴ��� �̿��Ͽ� ���
�Է� ��ư�� �޾Ƽ� ���� (���� Ǯ������ ���)
�ð�� ĳ�ҵ� ���� 4�ڸ� FND�� ����ϸ�, ���ڴ��� 4511Ĩ ���
4511 �����ͽ�Ʈ : http://www.farnell.com/datasheets/1758124.pdf
FND ����ó : http://www.devicemart.co.kr/13349
����̹� ����ó : http://www.devicemart.co.kr/1056675

* 4511����̹� truth table *
D3	 D2	 D1	 D0	 A B C D E F G ���
L	 L	 L	 L	 H H H H H H L 0
L	 L	 L	 H	 L H H L L L L 1
L	 L	 H	 L	 H H L H H L H 2
L	 L	 H	 H	 H H H H L L H 3
L	 H	 L	 L	 L H H L L H H 4
L	 H	 L	 H	 H L H H L H H 5
L	 H	 H	 L	 L L H H H H H 6
L	 H	 H	 H	 H H H L L L L 7
H	 L	 L	 L	 H H H H H H H 8
H	 L	 L	 H	 H H H L L H H 9

*/

/*
�Ƶ��̳��� Timer2�� �̿��Ͽ� 5ms������ ���ͷ�Ʈ�� �����Ͽ� ���
���� ������ ���� MsTimer2�� �̿�
���̺귯�� �ּ� : http://www.pjrc.com/teensy/td_libs_MsTimer2.html

Methods

MsTimer2::set(unsigned long ms, void (*f)())
this function sets a time on ms for the overflow. Each overflow, "f" will be called. "f" has to be declared void with no parameters.

MsTimer2::start()
enables the interrupt.

MsTimer2::stop()
disables the interrupt.

*/

#include "MsTimer2.h"

#define refresh_delay 5

#define FND0 A0	//���ڴ� �Է� 1 (D0)
#define FND1 A1	//���ڴ� �Է� 2 (D1)
#define FND2 A2	//���ڴ� �Է� 3 (D2)
#define FND3 A3	//���ڴ� �Է� 4 (D3)

#define digit_1 8	//1��° �ڸ� (���� ����)
#define digit_2 9	//2��° �ڸ�
#define digit_3 10	//3��° �ڸ�
#define digit_4 11	//4��° �ڸ�

#define SW1 2	//��ư�Է� 1 (�Ͻ�����)
#define SW2 3	//��ư�Է� 2 (����)

unsigned int digit1;	//1��°�ڸ��� ǥ���� ����
unsigned int digit2;	//2��°�ڸ��� ǥ���� ����
unsigned int digit3;	//3��°�ڸ��� ǥ���� ����
unsigned int digit4;	//4��°�ڸ��� ǥ���� ����

unsigned long digit_counter = 0;

//4�ڸ��� ���ڸ� �޾Ƽ� digit1,2,3,4�� �� �ڸ��� ����� ����
void quadWrite(int value)
{
	digit4 = value % 10;
	digit3 = ((value % 100) - digit4) / 10;
	digit2 = ((value % 1000) - digit3 * 10 - digit4) / 100;
	digit1 = (value - digit2 * 100 - digit3 * 10 - digit4) / 1000;
}

//����̹��� 10������ BCD�� ��ȯ�Ͽ� ����
void writeFND(int value)
{
	switch (value)
	{
	case 0:
		digitalWrite(FND0, 0);
		digitalWrite(FND1, 0);
		digitalWrite(FND2, 0);
		digitalWrite(FND3, 0);
		break;
	case 1:
		digitalWrite(FND0, 1);
		digitalWrite(FND1, 0);
		digitalWrite(FND2, 0);
		digitalWrite(FND3, 0);
		break;
	case 2:
		digitalWrite(FND0, 0);
		digitalWrite(FND1, 1);
		digitalWrite(FND2, 0);
		digitalWrite(FND3, 0);
		break;
	case 3:
		digitalWrite(FND0, 1);
		digitalWrite(FND1, 1);
		digitalWrite(FND2, 0);
		digitalWrite(FND3, 0);
		break;
	case 4:
		digitalWrite(FND0, 0);
		digitalWrite(FND1, 0);
		digitalWrite(FND2, 1);
		digitalWrite(FND3, 0);
		break;
	case 5:
		digitalWrite(FND0, 1);
		digitalWrite(FND1, 0);
		digitalWrite(FND2, 1);
		digitalWrite(FND3, 0);
		break;
	case 6:
		digitalWrite(FND0, 0);
		digitalWrite(FND1, 1);
		digitalWrite(FND2, 1);
		digitalWrite(FND3, 0);
		break;
	case 7:
		digitalWrite(FND0, 1);
		digitalWrite(FND1, 1);
		digitalWrite(FND2, 1);
		digitalWrite(FND3, 0);
		break;
	case 8:
		digitalWrite(FND0, 0);
		digitalWrite(FND1, 0);
		digitalWrite(FND2, 0);
		digitalWrite(FND3, 1);
		break;
	case 9:
		digitalWrite(FND0, 1);
		digitalWrite(FND1, 0);
		digitalWrite(FND2, 0);
		digitalWrite(FND3, 1);
		break;
	}
}

//FND�� �ڸ��� ����
void setDigit(int value)
{
	switch (value)
	{
	case 1:
		digitalWrite(digit_1, HIGH);
		digitalWrite(digit_2, LOW);
		digitalWrite(digit_3, LOW);
		digitalWrite(digit_4, LOW);
	case 2:
		digitalWrite(digit_1, LOW);
		digitalWrite(digit_2, HIGH);
		digitalWrite(digit_3, LOW);
		digitalWrite(digit_4, LOW);
	case 3:
		digitalWrite(digit_1, LOW);
		digitalWrite(digit_2, LOW);
		digitalWrite(digit_3, HIGH);
		digitalWrite(digit_4, LOW);
	case 4:
		digitalWrite(digit_1, LOW);
		digitalWrite(digit_2, LOW);
		digitalWrite(digit_3, LOW);
		digitalWrite(digit_4, HIGH);

	}
}

//FND�� ���ִ� �Լ� �ֱ������� �����
void FNDrefresh()	
{
	switch (digit_counter % 4)
		{
			case 0:
				//1��° �ڸ�
				setDigit(1);
				writeFND(digit1);
				break;

			case 1:
				//2��° �ڸ�
				setDigit(2);
				writeFND(digit2);
				break;

			case 2:
				//3��° �ڸ�
				setDigit(3);
				writeFND(digit3);
				break;

			case 3:
				//4��° �ڸ�
				setDigit(4);
				writeFND(digit4);
				break;
		}
	digit_counter++;	
}

//setup�� �ڵ�� ���� �ο��� 1ȸ ����Ǵ� �ڵ��Դϴ�
void setup()
{
	//Pinmode�� �̿��Ͽ� ���ڴ��� ����� �� ����
	pinMode(FND0, OUTPUT);	//FND0���� ��¸��� ����
	pinMode(FND1, OUTPUT);	//FND1���� ��¸��� ����
	pinMode(FND2, OUTPUT);	//FND2���� ��¸��� ����
	pinMode(FND3, OUTPUT);	//FND3���� ��¸��� ����

	pinMode(digit_1, OUTPUT);	//digit_1���� ��¸��� ����
	pinMode(digit_2, OUTPUT);	//digit_2���� ��¸��� ����
	pinMode(digit_3, OUTPUT);	//digit_3���� ��¸��� ����
	pinMode(digit_4, OUTPUT);	//digit_4���� ��¸��� ����

	pinMode(SW1, INPUT_PULLUP);	//SW1���� �Է¸��� ����
	pinMode(SW2, INPUT_PULLUP);	//SW1���� �Է¸��� ����

	MsTimer2::set(refresh_delay, FNDrefresh);
	MsTimer2::start();

}

int myTime;

void loop()
{
	delay(1000);
	myTime++;
	quadWrite(myTime);
}
