/*	스터디헬퍼 초시계 프로젝트
	(주)탐생 김민규 제품개발담당
		mingyu@tamseng.co.kr
	2015/05/08 v0.01
		- 기본 구조 제작
	2015/05/11 v0.02
		- 자제 선정
	2015/05/15 v0.03
		- 타이머 설정 추가
*/

/*
아두이노 레오나르도를 이용하며 4자리의 FND디스플레이를 BCD->7Segment 디코더를 이용하여 출력
입력 버튼을 받아서 리셋 (내부 풀업저항 사용)
시계용 캐소드 공통 4자리 FND를 사용하며, 디코더는 4511칩 사용
4511 데이터시트 : http://www.farnell.com/datasheets/1758124.pdf
FND 구매처 : http://www.devicemart.co.kr/13349
드라이버 구매처 : http://www.devicemart.co.kr/1056675

* 4511드라이버 truth table *
D3	 D2	 D1	 D0	 A B C D E F G 결과
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
아두이노의 Timer2를 이용하여 5ms단위로 인터럽트를 설정하여 출력
쉬운 적용을 위해 MsTimer2를 이용
라이브러리 주소 : http://www.pjrc.com/teensy/td_libs_MsTimer2.html

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

#define FND0 A0	//디코더 입력 1 (D0)
#define FND1 A1	//디코더 입력 2 (D1)
#define FND2 A2	//디코더 입력 3 (D2)
#define FND3 A3	//디코더 입력 4 (D3)

#define digit_1 8	//1번째 자리 (가장 좌측)
#define digit_2 9	//2번째 자리
#define digit_3 10	//3번째 자리
#define digit_4 11	//4번째 자리

#define SW1 2	//버튼입력 1 (일시정지)
#define SW2 3	//버튼입력 2 (리셋)

unsigned int digit1;	//1번째자리에 표시할 숫자
unsigned int digit2;	//2번째자리에 표시할 숫자
unsigned int digit3;	//3번째자리에 표시할 숫자
unsigned int digit4;	//4번째자리에 표시할 숫자

unsigned long digit_counter = 0;

//4자리의 숫자를 받아서 digit1,2,3,4로 각 자리를 나누어서 저장
void quadWrite(int value)
{
	digit4 = value % 10;
	digit3 = ((value % 100) - digit4) / 10;
	digit2 = ((value % 1000) - digit3 * 10 - digit4) / 100;
	digit1 = (value - digit2 * 100 - digit3 * 10 - digit4) / 1000;
}

//드라이버에 10진수를 BCD로 변환하여 전송
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

//FND의 자리수 설정
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

//FND를 켜주는 함수 주기적으로 실행됨
void FNDrefresh()	
{
	switch (digit_counter % 4)
		{
			case 0:
				//1번째 자리
				setDigit(1);
				writeFND(digit1);
				break;

			case 1:
				//2번째 자리
				setDigit(2);
				writeFND(digit2);
				break;

			case 2:
				//3번째 자리
				setDigit(3);
				writeFND(digit3);
				break;

			case 3:
				//4번째 자리
				setDigit(4);
				writeFND(digit4);
				break;
		}
	digit_counter++;	
}

//setup의 코드는 전원 부여시 1회 실행되는 코드입니다
void setup()
{
	//Pinmode를 이용하여 디코더로 출력할 핀 설정
	pinMode(FND0, OUTPUT);	//FND0핀을 출력모드로 설정
	pinMode(FND1, OUTPUT);	//FND1핀을 출력모드로 설정
	pinMode(FND2, OUTPUT);	//FND2핀을 출력모드로 설정
	pinMode(FND3, OUTPUT);	//FND3핀을 출력모드로 설정

	pinMode(digit_1, OUTPUT);	//digit_1핀을 출력모드로 설정
	pinMode(digit_2, OUTPUT);	//digit_2핀을 출력모드로 설정
	pinMode(digit_3, OUTPUT);	//digit_3핀을 출력모드로 설정
	pinMode(digit_4, OUTPUT);	//digit_4핀을 출력모드로 설정

	pinMode(SW1, INPUT_PULLUP);	//SW1핀을 입력모드로 설정
	pinMode(SW2, INPUT_PULLUP);	//SW1핀을 입력모드로 설정

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
