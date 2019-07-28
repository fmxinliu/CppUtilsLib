
// LeakpressDlg.h : ͷ�ļ�
//

#pragma once

#include "Ateq.h"
#include "fins.h"
#include "pthread.h"
#include "DlgChannleShow.h"
#include <vector>
#include <direct.h>

using namespace std;
using namespace OmronPlc;

// CLeakpressDlg �Ի���
class CLeakpressDlg : public CDialogEx
{
// ����
public:
	CLeakpressDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CLeakpressDlg();

	friend UINT WINAPI ThreadTestProcess(LPVOID pParam);

// �Ի�������
	enum { IDD = IDD_LEAKPRESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnAteqEventMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void LoadConfig();
	void Init();
	bool PLCConnect();
	bool AteqConnect();

	void StartTest(int id);
	bool IsStartState(int id);
	bool IsGetResult(int id);
	bool IsEndState(int id);
	bool IsALAState(int id);
	void SendResult(int id);
	void SendTestPress1(int id);
	void SendTestPress2(int id);
	void SendTestPress(int id);

	bool QueryAteqTest(int id); // ��ѯʵʱֵ
	bool QueryAteqResult(int id); // �����Խ��
	bool QueryPressResult(int id); // ��ѯѹ�����

	bool IsAteqStateMatch(int id, ATEQ_STATE state, bool reply = true);
	ATEQ_STATE QueryAteqState(int id);
	void ResetAteqState(int id);
	void SetAteqState(ATEQ_EVENT *e);

	RESULT getResult(int id);
	void setResult(int id, RESULT *r);

	ALA_TYPE getAlarmType(int id);
	void setAlarmType(int id, ALA_TYPE alarmType);

	void WriteALAResult(int id, ALA_TYPE alarmType);
	void WriteResult(int id, CString alarmStr = "", bool alarm = false);
	CString CreateFileName(int id, CString &dt);

	CString getDevicePrefix(int id); // ��ȡ�������
	CString getDevicePrefix2(int id);
	CString getALAString(ALA_TYPE alarmType); // ��ȡ�����ַ���

	void ResetClearAlarm(int id);
	bool needExit();

private:
	Fins *fins;
	Ateq ateqs[NUM];
	PLC_ADDR addr[NUM];
	CONFIG_PARA para;
	pthread_t pid;

	ATEQ_STATE ateqFlag[NUM];
	ALA_TYPE deviceAlarm[NUM];

	bool isWindowLoaded;
	DlgChannleShow* mDlgChannleShow[NUM];

	bool exit;
	vector<pair<int, void *>> mThreadParas;
	CWinThread *pthreads[NUM];
	CWinThread *pThreadListener;

	CString errorStr;

private:
	void MoveCtrl();
	void InitTabShow();
	void OnAlarm(int id);
	void OnTest(int id);

	void WriteResultToFile(CString dir, CString dt, RESULT r, CString fileName, CString alarmStr, bool alarm);
};
