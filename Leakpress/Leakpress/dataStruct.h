#pragma once

#include <stdint.h>

#define NUM 6		  // �������
#define PLCADDSIZE 10     // ��������ҽӵ� PLC ���Ĵ�������
#define FILE_NAME_LENGTH 6 // PLC �ļ����ֽ���

#define	MASK(i)				(1<<(i))		// conversion bit--> masque
#define GETBIT(x,n)			(((x)>>(n))&1)
#define	BIT(x,n)			(((x) & MASK(n)) >> (n))

typedef struct LEAK_PARAMETERS
{
	int     nDataType;
	UINT	wTypeTest;
	UINT	wTestPrg;
	UINT    wTestStep;
	UINT	wTestState;
	UINT	wTpsFill;
	UINT	wTpsStab;
	UINT	wTpsTest;
	UINT	wTpsDump;
	UINT	wPress1Unit;
	UINT    wLeakPress;
	UINT    wLeakValue;

} LEAK_PARAMETERS;


typedef struct PLC_ADDR
{
	BYTE address[PLCADDSIZE];

} PLC_ADDR;


typedef struct CONFIG_PARA {
	CString ip; // plc ��ַ
	CString coms[NUM]; // �������ò���

	CString deviceName[NUM]; // վ��
	CString fileSaveDir; // �ļ�����Ŀ¼

} CONFIG_PARA;

typedef enum STATE {
	ATEQ_REST = 0x00,
	ATEQ_TEST_1 = 0x01,	  // 2 �β���
    ATEQ_STABLE_1 = 0x02, // 2 ����ѹ
	ATEQ_RESULT_1 = 0x04, // 2 �ν��
	ATEQ_ERROR_1  = 0x08, // 2 �α���

	ATEQ_TEST_2 = 0x10,   // 3 �β���
	ATEQ_STABLE_2 = 0x20, // 3 ����ѹ
	ATEQ_RESULT_2 = 0x40, // 3 �ν��
	ATEQ_ERROR_2 = 0x80,  // 3 �α���

	PRESS_RESULT = 0x0100, // ѹ��

	ATEQ_REPLY = 0xffff,
} ATEQ_STATE;

typedef struct ATEQ_EVENT
{
	int id;
	UINT value1;
	UINT value2;
	STATE state;

	ATEQ_EVENT(int id, STATE state, UINT value1 = 0, UINT value2 = 0)
	{
		this->id = id;
		this->value1 = value1;
		this->value2 = value2;
		this->state = state;
	}

}ATEQ_EVENT;

typedef struct RESULT
{
	UINT dwTestStep; 
	UINT dwLeakPress; // ѹ��
	UINT dwLeakValue; // й©��
	UINT dwTestPress1; // p1
	UINT dwTestPress2; // p2
	UINT dwWorkPress; // ��ѹ����ѹ��
	UINT dwPress; // ѹ��ѹ��
	UINT dwPosition; // ѹ��λ��

	uint8_t fileName[FILE_NAME_LENGTH]; // �ļ���

	void operator = (RESULT r)
	{
		dwTestStep = r.dwTestStep;
		dwLeakPress = r.dwLeakPress;
		dwLeakValue = r.dwLeakValue;
		dwTestPress1 = r.dwTestPress1;
		dwTestPress2 = r.dwTestPress2;
		dwWorkPress = r.dwWorkPress;
		dwPress = r.dwPress;
		dwPosition = r.dwPosition;
		memcpy(fileName, r.fileName, FILE_NAME_LENGTH);
	}

} RESULT;

// ״̬������
typedef enum CTRL_STATE {
	PLC_Start = 0,
	PLC_End = 8,
	PLC_RequestResult = 9,
	PLC_ALA = 9,
	PC_StartLowTest = 5,
	PC_ResultSended = 10,
	PC_ClearALA,
} CTRL_STATE;

// ͨ�üĴ���
typedef enum PLCADDR_COMMON {
	ALA = 0,
	MES = 1,
	CTRL = 2,
	Test1 = 5,
	Test2 = 6,

} PLCADDR_COMMON;

// ��ѹ�Ĵ�������
typedef enum PLCADDR_DEFINE_HIGH {
	ALALowLeakPCcontrol,
	MESHighLeakPCcontrol,
	HighLeakPCPress,
	HighLeakPCLeakValue,
	HighLeakPCFileName,
	HighLeakPCInPress,

} GADDR;

// ��ѹ�Ĵ�������
typedef enum PLCADDR_DEFINE_LOW {
	ALAHigthLeakPCcontrol,
	MESLowLeakPCcontrol,
	LowLeakPCcontrol,
	LowLeakPCPress,
	LowLeakPCLeakValue,
	LowLeakPCDataTest1,
	LowLeakPCDataTest2,
	LowLeakPCFileName,
	LowLeakPCValueTest1,
	LowLeakPCValueTest2,

} DADDR;

// ѹ���Ĵ�������
typedef enum PLCADDR_DEFINE_YPRESS {
	ALAPressPCcontrol,
	MESPressPCcontrol,
	PressPCPressData,
	PressPCPressPosition,
	PressPCFileName,

} WADDR;