
// LeakpressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Leakpress.h"
#include "LeakpressDlg.h"
#include "afxdialogex.h"
#include "FileManager.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static pthread_mutex_t r_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t ala_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t plc_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t ateq_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

UINT  WINAPI ThreadInit(LPVOID pParam);
UINT  WINAPI ThreadALAListener(LPVOID pParam);
UINT (WINAPI *pThread[NUM]) (LPVOID pParam) = {Thread1, Thread2, Thread3, Thread4, Thread5, Thread6};
// CLeakpressDlg �Ի���

CLeakpressDlg::CLeakpressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLeakpressDlg::IDD, pParent)
	, fins(new Fins(TransportType::Udp))
	, isWindowLoaded(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#ifdef _DEBUG
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif
}

CLeakpressDlg::~CLeakpressDlg()
{
#ifdef _DEBUG
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	FreeConsole();
#endif

	for (int i=0;i<NUM;i++)
	{
		if (mDlgChannleShow[i])
		{
			delete mDlgChannleShow[i];
			mDlgChannleShow[i] = NULL;
		}
	}

	delete fins;
}

void CLeakpressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLeakpressDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_USER_EVENT_MSG, &CLeakpressDlg::OnAteqEventMsg)
END_MESSAGE_MAP()


// CLeakpressDlg ��Ϣ�������

BOOL CLeakpressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	LoadConfig();
	InitTabShow();

	isWindowLoaded = true;
	MoveCtrl();

	Init();
	//AfxBeginThread((AFX_THREADPROC)ThreadInit, this, THREAD_PRIORITY_IDLE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLeakpressDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLeakpressDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLeakpressDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	MoveCtrl();
}

void CLeakpressDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// ���ƴ�����С�ߴ�
	lpMMI->ptMinTrackSize.x = 780;
	lpMMI->ptMinTrackSize.y = 530;

	// ���ƴ������ߴ�
	lpMMI->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
	lpMMI->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CLeakpressDlg::LoadConfig()
{
	CString path = FileManager::GetAppPath();
	CString addrIni = path + "plcaddress.ini";
	CString configIni = path + "config.ini";

	memset(addr, 0, sizeof(addr));

	// ��ȡվ��
	vector<CString> sections = FileManager::ReadSectionNames(addrIni);
	for (int k = 0; k < sections.size(); k++) {
		para.deviceName[k] = sections[k];
	}

	// ��ȡ PLC �Ĵ�������
	for (int i = 0; i < NUM; i++) {
		vector<CString> childs = FileManager::ReadChildsOnGroup(addrIni, sections[i]);

		for (int k = 0; k < childs.size() && k < PLCADDSIZE; k++) {
			CString mKeyValueStr = childs[k];
			CString mkeyStr = mKeyValueStr.Left(mKeyValueStr.Find("="));
			CString mValueStr = mKeyValueStr.Mid(mKeyValueStr.Find("=") + 1);
			addr[i].address[k] = (BYTE)atoi(mValueStr);
		}
	}

	// ��ȡ�������
	vector<CString> childs = FileManager::ReadChildsOnGroup(configIni, "COMMON");
	for (int k = 0; k < childs.size(); k++) {
		CString mKeyValueStr = childs[k];
		CString mkeyStr = mKeyValueStr.Left(mKeyValueStr.Find("="));
		CString mValueStr = mKeyValueStr.Mid(mKeyValueStr.Find("=") + 1);
		if (mkeyStr == "IP") 
		{
			para.ip = mValueStr;
		} 
		else if (strstr(mkeyStr, "COM"))
		{
			int port = (atoi)(mkeyStr.Right(1)) - 1;
			para.coms[port] = mValueStr;
		}
		else if (strstr(mkeyStr, "FILE_SAVE_DIR")) 
		{
			para.fileSaveDir = mValueStr;
		}
	}
}

void CLeakpressDlg::InitTabShow()
{
	for (int i=0;i<NUM;i++) {
		mDlgChannleShow[i]=new DlgChannleShow(this);
		mDlgChannleShow[i]->Create(IDD_DlgChannleShow, this);
		mDlgChannleShow[i]->ShowWindow(SW_SHOW);
		mDlgChannleShow[i]->SetChannle(i+1, para.deviceName[i]);
	}
}

void CLeakpressDlg::MoveCtrl()
{
	if (!isWindowLoaded){
		return;
	}

	int nTop=0;
	CRect rcClient;
	GetClientRect(rcClient);
	int nWindowWidth=rcClient.Width();
	int nWindowHeight=rcClient.Height();
	int nLeft=2;
	int nWidth=nWindowWidth-nLeft;
	CRect mRect;
	if (mDlgChannleShow[2]->GetSafeHwnd()) {
		CRect rcMove;
		rcMove.left =nLeft;
		rcMove.top = 0;
		rcMove.right =rcMove.left+nWidth/3;
		rcMove.bottom =rcMove.top+ nWindowHeight/2;
		mDlgChannleShow[2]->MoveWindow(rcMove,TRUE);
		mRect=rcMove;
	}

	if (mDlgChannleShow[3]->GetSafeHwnd()) {
		CRect rcMove;
		rcMove.left =mRect.right;
		rcMove.top = 0;
		rcMove.right =rcMove.left+nWidth/3;
		rcMove.bottom =rcMove.top+ nWindowHeight/2;
		mDlgChannleShow[3]->MoveWindow(rcMove,TRUE);
	}

	if (mDlgChannleShow[4]->GetSafeHwnd()) {
		CRect rcMove;
		rcMove.left =mRect.right + nWidth/3;
		rcMove.top = 0;
		rcMove.right =rcMove.left+nWidth/3;
		rcMove.bottom = rcMove.top+nWindowHeight/2;
		mDlgChannleShow[4]->MoveWindow(rcMove,TRUE);
	}

	if (mDlgChannleShow[0]->GetSafeHwnd()) {
		CRect rcMove;
		rcMove.left =mRect.left;
		rcMove.top = mRect.bottom;
		rcMove.right =rcMove.left+nWidth/3;
		rcMove.bottom =rcMove.top+ nWindowHeight/2;
		mDlgChannleShow[0]->MoveWindow(rcMove,TRUE);
	}

	if (mDlgChannleShow[1]->GetSafeHwnd()) {
		CRect rcMove;
		rcMove.left =mRect.right;
		rcMove.top = mRect.bottom;
		rcMove.right =rcMove.left+nWidth/3;
		rcMove.bottom = rcMove.top+nWindowHeight/2;
		mDlgChannleShow[1]->MoveWindow(rcMove,TRUE);
	}

	if (mDlgChannleShow[5]->GetSafeHwnd()) {
		CRect rcMove;
		rcMove.left =mRect.right + nWidth/3;
		rcMove.top = mRect.bottom;
		rcMove.right =rcMove.left+nWidth/3;
		rcMove.bottom = rcMove.top+nWindowHeight/2;
		mDlgChannleShow[5]->MoveWindow(rcMove,TRUE);
	}
}

void CLeakpressDlg::Init()
{
	bool initSuccess = PLCConnect() && AteqConnect();
}

bool CLeakpressDlg::PLCConnect()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return false;
	}
	else
		printf("The Winsock 2.2 dll was found okay\n");


	fins->SetRemote(para.ip.GetBuffer(0));
	if (!fins->Connect())
	{
		WSACleanup();
		MessageBox("PLC���Ӵ���", "PLC����", MB_OK);
		return false;
	}

	AfxBeginThread((AFX_THREADPROC)ThreadALAListener, this, THREAD_PRIORITY_IDLE);
	return true;
}

bool CLeakpressDlg::AteqConnect()
{
	for (int i = 0; i < NUM; i++)
	{
		vector<CString> commPara = Util::SpiltString(para.coms[i], ' ');

		if (commPara.size() >= 2) {
			int port = (atoi)(commPara[0]);
			CString mCommStr = commPara[1];

			ateqs[i].connect(port, mCommStr.GetBuffer(0));
			ateqs[i].setPara(i, this);
		}	
	}

	CString info;
	for (int i = 0; i < NUM; i++) {
		vector<CString> commPara = Util::SpiltString(para.coms[i], ' ');

		if (ateqs[i].isConnect()) {
			ateqFlag[i] = ATEQ_REST;
			mDlgChannleShow[i]->setConnectState(true);
			AfxBeginThread((AFX_THREADPROC)pThread[i], this, THREAD_PRIORITY_IDLE);
		} else {
			info += para.deviceName[i] + "  ";
		}
	}

	if (info.GetLength() > 0) 
	{
		info += "����ʧ��";
		MessageBox(info, "�豸����", MB_OK);
		return false;
	}

	return true;
}

void CLeakpressDlg::StartTest(int id)
{
	pthread_mutex_lock(&plc_mutex);
	fins->WriteDM((uint16_t)addr[id].address[CTRL], (uint16_t)(PC_StartLowTest));
	pthread_mutex_unlock(&plc_mutex);
}

bool CLeakpressDlg::IsStartState(int id)
{
	pthread_mutex_lock(&plc_mutex);
	WORD urData = 0xff;
	fins->ReadDM((uint16_t)addr[id].address[MES], urData);
	pthread_mutex_unlock(&plc_mutex);

	return urData == PLC_Start;
}

bool CLeakpressDlg::IsGetResult(int id)
{
	pthread_mutex_lock(&plc_mutex);
	WORD urData = 0xff;
	fins->ReadDM((uint16_t)addr[id].address[MES], urData);
	pthread_mutex_unlock(&plc_mutex);

	return urData == PLC_RequestResult;
}

bool CLeakpressDlg::IsEndState(int id)
{
	pthread_mutex_lock(&plc_mutex);
	WORD urData = 0xff;
	fins->ReadDM((uint16_t)addr[id].address[MES], urData);
	pthread_mutex_unlock(&plc_mutex);

	return urData == PLC_End;
}

bool CLeakpressDlg::IsALAState(int id)
{
	pthread_mutex_lock(&plc_mutex);
	WORD urData = 0xff;
	fins->ReadDM((uint16_t)addr[id].address[ALA], urData);
	pthread_mutex_unlock(&plc_mutex);

	return urData == PLC_ALA;
}

RESULT CLeakpressDlg::getResult(int id)
{
	pthread_mutex_lock(&r_mutex);
	RESULT r = mDlgChannleShow[id]->getResult();
	pthread_mutex_unlock(&r_mutex);
	return r;
}

void CLeakpressDlg::setResult(int id, RESULT *r)
{
	pthread_mutex_lock(&r_mutex);
	mDlgChannleShow[id]->setResult(*r);
	pthread_mutex_unlock(&r_mutex);
}


void CLeakpressDlg::SendResult(int id)
{
	RESULT r = getResult(id);

	CTime curTime = CTime::GetCurrentTime();
	CString csCurTime = curTime.Format("%Y-%m-%d %H:%M:%S");
	CString dateString = curTime.Format("%m%d");
	CString fileName = para.deviceName[id].Left(2) + dateString;
	memcpy(r.fileName, fileName, FILE_NAME_LENGTH);

	CString device_prefix = this->getDevicePrefix(id);

	if ("G" == device_prefix) {
		pthread_mutex_lock(&plc_mutex);
		fins->WriteDM((uint16_t)addr[id].address[MESHighLeakPCcontrol], (uint16_t)(10));
		fins->WriteDM((uint16_t)addr[id].address[HighLeakPCPress], (uint16_t)(r.dwLeakPress));
		fins->WriteDM((uint16_t)addr[id].address[HighLeakPCLeakValue], (uint16_t)(r.dwLeakValue));
		fins->WriteDM((uint16_t)addr[id].address[HighLeakPCFileName], r.fileName, FILE_NAME_LENGTH);
		
		// ��ȡ Workpress
		WORD urData = 0xff;
		fins->ReadDM((uint16_t)addr[id].address[HighLeakPCInPress], urData); r.dwWorkPress = urData * 1000;
		pthread_mutex_unlock(&plc_mutex);
	}
	else if ("D" == device_prefix) {
		pthread_mutex_lock(&plc_mutex);
		fins->WriteDM((uint16_t)addr[id].address[MESLowLeakPCcontrol], (uint16_t)(PC_ResultSended));
		fins->WriteDM((uint16_t)addr[id].address[LowLeakPCPress], (uint16_t)(r.dwLeakPress / 10));
		fins->WriteDM((uint16_t)addr[id].address[LowLeakPCLeakValue], (uint16_t)(r.dwLeakValue / 100));
		fins->WriteDM((uint16_t)addr[id].address[LowLeakPCFileName], r.fileName, FILE_NAME_LENGTH);

		// ��ȡ P1 P2
		WORD urData = 0xff;
		fins->ReadDM((uint16_t)addr[id].address[LowLeakPCValueTest1], urData); r.dwTestPress1 = urData * 10;
		fins->ReadDM((uint16_t)addr[id].address[LowLeakPCValueTest2], urData); r.dwTestPress2 = urData * 10;
		pthread_mutex_unlock(&plc_mutex);
	}
	else if ("Y" == device_prefix) {
		pthread_mutex_lock(&plc_mutex);
		fins->WriteDM((uint16_t)addr[id].address[MESPressPCcontrol], (uint16_t)(PC_ResultSended));
		fins->WriteDM((uint16_t)addr[id].address[PressPCPressData], (uint16_t)(r.dwPress / 10));
		fins->WriteDM((uint16_t)addr[id].address[PressPCPressPosition], (uint16_t)(r.dwPosition / 100));
		fins->WriteDM((uint16_t)addr[id].address[PressPCFileName], r.fileName, FILE_NAME_LENGTH);
		pthread_mutex_unlock(&plc_mutex);
	}
	else {
		return;
	}

	setResult(id, &r);
	WriteResultToFile(para.fileSaveDir, csCurTime, r, fileName);
}

void CLeakpressDlg::WriteResultToFile(CString dir, CString dt, RESULT r, CString fileName)
{
	long total_lines = 1;
	CString lineString;

	CString device_prefix = fileName.Left(1);

	// ����Ŀ¼
	if ("G" == device_prefix) {
		dir += "\\HighLeakpress";
	} else if ("D" == device_prefix) {
		dir += "\\LowLeakpress";
	} else if ("Y" == device_prefix) {
		dir += "\\Press";
	} else {
		return;
	}

	// Ŀ¼�����ڣ�����
	if (!FileManager::CheckFolderExist(dir)) {
		FileManager::CreateMuliteDirectory(dir);
	}

	// �ļ�ͷ���к�
	CString path = dir + "\\" + fileName + ".csv";
	if (FileManager::CheckFileExist(path)) {
		total_lines = FileManager::FileTotalLines(path);

	} else {
		if ("G" == device_prefix) {
			lineString.Append("NO, DATETIME, PRESS, LEAK, Workpress\n");
		} else if ("D" == device_prefix) {
			lineString.Append("NO, DATETIME, PRESS, LEAK, P1, P2, P1-P2\n");
		} else if ("Y" == device_prefix) {
			lineString.Append("NO, DATETIME, PRESS, Position\n");
		}
	}

	// �ļ�����
	CString temp;	
	temp.Format("%d, ", total_lines);
	lineString.Append(temp);
	lineString.Append(dt);
	lineString.Append(", ");
	
	if ("G" == device_prefix) {
		temp.Format("%ld, %ld, %ld\n", r.dwLeakPress, r.dwLeakValue, r.dwWorkPress);
	} else if ("D" == device_prefix) {
		temp.Format("%ld, %ld, %ld, %ld, %ld\n", r.dwLeakPress, r.dwLeakValue, r.dwTestPress1, r.dwTestPress2, r.dwTestPress1 - r.dwTestPress2);
	} else if ("Y" == device_prefix) {
		temp.Format("%ld, %ld\n", r.dwPress, r.dwPosition);
	}
	
	lineString.Append(temp);

	FileManager::SaveFile(lineString, path);
}

CString CLeakpressDlg::getDevicePrefix(int id)
{
	CString device_prefix = para.deviceName[id].Left(1);
	return device_prefix;
}

void CLeakpressDlg::SendTestPress1(int id)
{
	RESULT r = getResult(id);

	pthread_mutex_lock(&plc_mutex);
	fins->WriteDM((uint16_t)addr[id].address[Test1], (uint16_t)(r.dwTestPress1 / 10));
	pthread_mutex_unlock(&plc_mutex);
}

void CLeakpressDlg::SendTestPress2(int id)
{
	RESULT r = getResult(id);

	pthread_mutex_lock(&plc_mutex);
	fins->WriteDM((uint16_t)addr[id].address[Test2], (uint16_t)(r.dwTestPress2 / 10));
	pthread_mutex_unlock(&plc_mutex);
}

void CLeakpressDlg::SendTestPress(int id)
{
	RESULT r = getResult(id);

	pthread_mutex_lock(&plc_mutex);
	fins->WriteDM((uint16_t)addr[id].address[CTRL], (uint16_t)(PC_StartLowTest));
	fins->WriteDM((uint16_t)addr[id].address[Test1], (uint16_t)(r.dwTestPress1 / 10));
	pthread_mutex_unlock(&plc_mutex);
}

bool CLeakpressDlg::QueryPressResult(int id)
{
	BYTE cmd[66] = {0};
	cmd[0] = 0xAA;
	cmd[1] = 0x20;
	cmd[2] = 0x01;
	cmd[65] = 0xCB;
	return ateqs[id].Write(cmd, 66) == 66;
}

bool CLeakpressDlg::QueryAteqTest(int id)
{
	BYTE cmd[] = { 0xFF, 0x03, 0, 0x30, 0, 0x0D, 0x91, 0xDE };
	return ateqs[id].Write(cmd, 8) == 8;
}

bool CLeakpressDlg::QueryAteqResult(int id)
{
	BYTE cmd[] = { 0xFF, 0x03, 0, 0x11, 0, 0x0C, 0x00, 0x14 };
	return ateqs[id].Write(cmd, 8) == 8;
}

bool CLeakpressDlg::IsAteqStateMatch(int id, ATEQ_STATE state, bool reply)
{
	pthread_mutex_lock(&ateq_mutex);
	ATEQ_STATE state1 = ateqFlag[id];
	pthread_mutex_unlock(&ateq_mutex);

	if (state1 == ATEQ_REPLY) {
		return reply;
	}

	return state1 == state;
}

ATEQ_STATE CLeakpressDlg::QueryAteqState(int id)
{
	pthread_mutex_lock(&ateq_mutex);
	ATEQ_STATE state = ateqFlag[id];
	pthread_mutex_unlock(&ateq_mutex);

	return state;
}

void CLeakpressDlg::ResetAteqState(int id)
{
	pthread_mutex_lock(&ateq_mutex);
	ateqFlag[id] = ATEQ_REST;
	pthread_mutex_unlock(&ateq_mutex);
}

void CLeakpressDlg::SetAteqState(ATEQ_EVENT *e)
{
	pthread_mutex_lock(&ateq_mutex);
	ateqFlag[e->id] = e->state;
	pthread_mutex_unlock(&ateq_mutex);
}

afx_msg LRESULT CLeakpressDlg::OnAteqEventMsg(WPARAM wParam, LPARAM lParam)
{
	ATEQ_EVENT *e = (ATEQ_EVENT *)lParam;
	RESULT r = getResult(e->id);

	switch (e->state)
	{
	case ATEQ_RESULT_1:
		r.dwLeakPress = e->value1;
		r.dwLeakValue = e->value2;
		break;
	case ATEQ_STABLE_2:
		r.dwTestPress1 = e->value1;
		r.dwTestPress2 = e->value1; // ����1��2��ͬһ��ͨ��
		break;
	case PRESS_RESULT:
		r.dwPress = e->value1;
		r.dwPosition = e->value2;
	default:
		break;
	}

	this->setResult(e->id, &r);
	this->SetAteqState(e);
	delete e;
	return 0;
}

void CLeakpressDlg::OnHighTest(int id, bool bstart, bool isAteqLow )
{
	// 1.��ѯ PLC ��λ�ź�
	if (!bstart) {
		bstart = IsStartState(id);
		Sleep(500);
		if (!bstart) {
			return;
		}
	}

	printf("====start %d====\n", id + 1);
	ResetAteqState(id);

	// 2.�ȴ� PLC ��ȡ���
	puts("wait plc get result");
	while (!IsGetResult(id)) {
		Sleep(200);
	}

	puts("send result");
	SendResult(id); // �� PLC ���ͽ��

	puts("waiting end");


}

void CLeakpressDlg::OnTest(int id, bool bstart, CString device_prefix)
{
	// 1.��ѯ PLC ��λ�ź�
	if (!bstart) {
		bstart = IsStartState(id);
		Sleep(500);
		if (!bstart) {
			return;
		}
	}

	printf("====start %d====\n", id + 1);
	ResetAteqState(id);

	if ("D" == device_prefix) {
		// 2.�ȴ� 2 ����ѹ
		do {
			QueryAteqTest(id);
			Sleep(100);
		} while (IsAteqStateMatch(id, ATEQ_REST));

		// 3.�ȴ� 2 ����ѹ����
		bool error = true;
		while (IsAteqStateMatch(id, ATEQ_STABLE_1)) {
			QueryAteqTest(id);
			error = false;
			Sleep(100);
		}

		if (error) {
			puts("2 ��©��");
			return;
		}
		else {
			puts("====2 ����ѹ����====");
			//Sleep(1000);
		}

		while (!IsAteqStateMatch(id, ATEQ_TEST_1, false)) {
			QueryAteqTest(id);
			Sleep(100);
		}

		puts("====��ѯ2 �ν��====");

		// 4.��ѯ 2 �ν��
		do {
			QueryAteqResult(id);
			Sleep(100);
		} while (!IsAteqStateMatch(id, ATEQ_RESULT_1, false));

		puts("�ȴ� 3 ����ѹ");

		// 5. �ȴ� 3 ����ѹ
		do {
			QueryAteqTest(id);
			Sleep(100);
		} while (!IsAteqStateMatch(id, ATEQ_STABLE_2, false));

		if (QueryAteqState(id) == ATEQ_STABLE_2) {
			puts("sending test press");
		}

		// 6. 3 ����ѹ��һֱ���ͽ��
		while (IsAteqStateMatch(id, ATEQ_STABLE_2, false)) {
			QueryAteqTest(id);
			Sleep(100);
			SendTestPress(id);
		}

		puts("====3 ����ѹ����====");
	}

	// 7.�ȴ� PLC ��ȡ���
	puts("�ȴ� PLC ��ȡ���");
	while (!IsGetResult(id)) {
		Sleep(200);
	}

	// 8.��ѯѹ�����
	puts("query press result");
	if ("Y" == device_prefix) {
		while (!IsAteqStateMatch(id, PRESS_RESULT, false)) {
			QueryPressResult(id);
			Sleep(100);
		}
	}  

	puts("send result");
	SendResult(id); // �� PLC ���ͽ��

	puts("waiting end");

	// 8.�ȴ� PLC ����
	while (!IsEndState(id)) {
		Sleep(100);
	}

	bstart = false;

	puts("====end====");
}

// �����������,����Ӳ��
UINT WINAPI ThreadInit(LPVOID pParam)
{
	CLeakpressDlg *pMain=(CLeakpressDlg*)pParam;

	if (NULL != pMain)
	{
		pMain->Init();
	}

	return 0;
}

UINT WINAPI Thread1(LPVOID pParam)
{
	int id = 0;

	CLeakpressDlg *pMain = (CLeakpressDlg*)pParam;
	DlgChannleShow *pDlg = pMain->mDlgChannleShow[id];

	pDlg->m_flagThreadStart = true;

	bool bstart = false;
	while (!pDlg->m_flagThreadExit)
	{
		pMain->OnTest(id, bstart, pMain->getDevicePrefix(id));
	}

	pDlg->m_flagThreadStart = false;
	return 0;
}

UINT WINAPI Thread2(LPVOID pParam)
{
	int id = 1;

	CLeakpressDlg *pMain = (CLeakpressDlg*)pParam;
	DlgChannleShow *pDlg = pMain->mDlgChannleShow[id];

	pDlg->m_flagThreadStart = true;

	bool bstart = false;
	while (!pDlg->m_flagThreadExit)
	{
		pMain->OnTest(id, bstart, pMain->getDevicePrefix(id));
	}

	pDlg->m_flagThreadStart = false;
	return 0;
}

UINT WINAPI Thread3(LPVOID pParam)
{
	int id = 2;

	CLeakpressDlg *pMain = (CLeakpressDlg*)pParam;
	DlgChannleShow *pDlg = pMain->mDlgChannleShow[id];

	pDlg->m_flagThreadStart = true;

	bool bstart = false;
	while (!pDlg->m_flagThreadExit)
	{
		pMain->OnTest(id, bstart, pMain->getDevicePrefix(id));
	}

	pDlg->m_flagThreadStart = false;
	return 0;
}

UINT WINAPI Thread4(LPVOID pParam)
{
	int id = 3;

	CLeakpressDlg *pMain = (CLeakpressDlg*)pParam;
	DlgChannleShow *pDlg = pMain->mDlgChannleShow[id];

	pDlg->m_flagThreadStart = true;

	bool bstart = false;
	while (!pDlg->m_flagThreadExit)
	{
		pMain->OnTest(id, bstart, pMain->getDevicePrefix(id));
	}

	pDlg->m_flagThreadStart = false;
	return 0;
}

UINT WINAPI Thread5(LPVOID pParam)
{
	int id = 4;

	CLeakpressDlg *pMain = (CLeakpressDlg*)pParam;
	DlgChannleShow *pDlg = pMain->mDlgChannleShow[id];

	pDlg->m_flagThreadStart = true;

	bool bstart = false;
	while (!pDlg->m_flagThreadExit)
	{
		pMain->OnTest(id, bstart, pMain->getDevicePrefix(id));
	}

	pDlg->m_flagThreadStart = false;
	return 0;
}

UINT WINAPI Thread6(LPVOID pParam)
{
	int id = 5;

	CLeakpressDlg *pMain = (CLeakpressDlg*)pParam;
	DlgChannleShow *pDlg = pMain->mDlgChannleShow[id];

	pDlg->m_flagThreadStart = true;

	bool bstart = false;
	while (!pDlg->m_flagThreadExit)
	{
		pMain->OnTest(id, bstart, pMain->getDevicePrefix(id));
	}

	pDlg->m_flagThreadStart = false;
	return 0;
}


UINT WINAPI ThreadALAListener(LPVOID pParam)
{
	CLeakpressDlg *pMain=(CLeakpressDlg*)pParam;

	while (true)
	{
		for (int id = 0; id < NUM; id++) {	
			bool alarm = pMain->IsALAState(id);
			if (pMain->getAlarm(id) != alarm) {
				pMain->setAlarm(id, alarm);
			}
			Sleep(1000);
		}
	}

	return 0;
}

bool CLeakpressDlg::getAlarm(int id)
{
	pthread_mutex_lock(&ala_mutex);
	bool ala = deviceAlarm[id];
	pthread_mutex_unlock(&ala_mutex);

	return ala;
}
void CLeakpressDlg::setAlarm(int id, bool alarm)
{
	pthread_mutex_lock(&ala_mutex);
	deviceAlarm[id] = alarm;
	pthread_mutex_unlock(&ala_mutex);
}