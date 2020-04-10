
// LeakpressDlg.cpp : 实现文件
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

#define CHECK_THREAD_RESET(id) \
    if (getAlarmType(id) || needExit()) { \
        return; \
    } 

static pthread_mutex_t r_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t ala_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t plc_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static pthread_mutex_t ateq_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

UINT  WINAPI ThreadInit(LPVOID pParam);
UINT  WINAPI ThreadALAListener(LPVOID pParam);
// CLeakpressDlg 对话框

CLeakpressDlg::CLeakpressDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CLeakpressDlg::IDD, pParent)
    , fins(new Fins(TransportType::Udp))
    , isWindowLoaded(false)
    , errorStr("error")
    , exit(false)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    LoadConfig();

    if (para.recordType & CONSOLE) {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    for (int i = 0; i < NUM; i++) {
        mThreadParas.push_back(make_pair(i, this));
        setAlarmType(i, ALA_NO);
        mDlgChannleShow[i] = NULL;
         pthreads[i] = NULL;
        ResetAteqState(i);
        ResetClearAlarm(i);
    }
}

CLeakpressDlg::~CLeakpressDlg()
{
    if (para.recordType & CONSOLE) {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
    }

    exit = true;
    fins->Close();

    for (int i = 0; i < NUM; i++) {
        if (mDlgChannleShow[i]) {
            delete mDlgChannleShow[i];
            mDlgChannleShow[i] = NULL;
            if (pthreads[i]) {
                WaitForSingleObject(pthreads[i]->m_hThread, INFINITE);
            }
        }
    }
    
    WaitForSingleObject(pThreadListener->m_hThread, INFINITE);

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


// CLeakpressDlg 消息处理程序



BOOL CLeakpressDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);            // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    SetWindowText(_T("Leakpress v") + GetSoftwareVersion());

    //LoadConfig();
    InitTabShow();

    isWindowLoaded = true;
    MoveCtrl();

    Init();
    //AfxBeginThread((AFX_THREADPROC)ThreadInit, this, THREAD_PRIORITY_IDLE);
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLeakpressDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
    // 限制窗口最小尺寸
    lpMMI->ptMinTrackSize.x = 780;
    lpMMI->ptMinTrackSize.y = 530;

    // 限制窗口最大尺寸
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

    // 读取站名
    vector<CString> sections = FileManager::ReadSectionNames(addrIni);
    for (int k = 0; k < sections.size(); k++) {
        para.deviceName[k] = sections[k];
    }

    // 读取 PLC 寄存器配置
    for (int i = 0; i < NUM; i++) {
        vector<CString> childs = FileManager::ReadChildsOnGroup(addrIni, sections[i]);

        for (int k = 0; k < childs.size() && k < PLCADDSIZE; k++) {
            CString mKeyValueStr = childs[k];
            CString mkeyStr = mKeyValueStr.Left(mKeyValueStr.Find(_T("=")));
            CString mValueStr = mKeyValueStr.Mid(mKeyValueStr.Find(_T("=")) + 1);
            addr[i].address[k] = (UINT)_ttoi(mValueStr);
        }
    }

    // 获取软件配置
    vector<CString> childs = FileManager::ReadChildsOnGroup(configIni, _T("COMMON"));
    for (int k = 0; k < childs.size(); k++) {
        CString mKeyValueStr = childs[k];
        CString mkeyStr = mKeyValueStr.Left(mKeyValueStr.Find(_T("=")));
        CString mValueStr = mKeyValueStr.Mid(mKeyValueStr.Find(_T("=")) + 1);
        if (mkeyStr == "IP") 
        {
            para.ip = mValueStr;
        } 
        else if (_tcsstr(mkeyStr, _T("COM")))
        {
            int port = (_ttoi)(mkeyStr.Right(1)) - 1;
            para.coms[port] = mValueStr;
        }
        else if (_tcsstr(mkeyStr, _T("FILE_SAVE_DIR"))) 
        {
            para.fileSaveDir = mValueStr;
        }
        else if (_tcsstr(mkeyStr, _T("DEBUG")))
        {
            para.recordType = (RecordType)(_ttoi)(mValueStr);
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
    fins->SetRemote(para.ip.GetBuffer(0));
    if (!fins->Connect()) {
        MessageBox(_T("PLC连接错误"), _T("PLC连接"), MB_SYSTEMMODAL|MB_ICONEXCLAMATION|MB_OK);
        return false;
    }

    pThreadListener = AfxBeginThread((AFX_THREADPROC)ThreadALAListener, this, THREAD_PRIORITY_IDLE);
    return true;
}

bool CLeakpressDlg::AteqConnect()
{
    for (int i = 0; i < NUM; i++)
    {
        vector<CString> commPara = Util::SpiltString(para.coms[i], ' ');

        if (commPara.size() >= 2) {
            int port = (_ttoi)(commPara[0]);
            CString mCommStr = commPara[1];

            ateqs[i].connect(port, mCommStr.GetBuffer(0));
            ateqs[i].setPara(i, this);
        }    
    }

    CString info;
    for (int i = 0; i < NUM; i++) {
        vector<CString> commPara = Util::SpiltString(para.coms[i], ' ');

        if (ateqs[i].isConnect()) {
            mDlgChannleShow[i]->setConnectState(true);
            pthreads[i] = AfxBeginThread((AFX_THREADPROC)ThreadTestProcess, &mThreadParas[i], THREAD_PRIORITY_IDLE);
        } else {
            info += para.deviceName[i] + _T("  ");
        }
    }

    if (info.GetLength() > 0) 
    {
        info += _T("连接失败");
        MessageBox(info,  _T("设备连接"), MB_SYSTEMMODAL|MB_ICONEXCLAMATION|MB_OK); // 显示在所有窗口的最上面
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
    WriteResult(id);
}

void CLeakpressDlg::WriteALAResult(int id, ALA_TYPE alarmType)
{
    WriteResult(id, getALAString(alarmType), true);
}

CString CLeakpressDlg::CreateFileName(int id, CString &dt)
{
    RESULT r = getResult(id);

    CTime curTime = CTime::GetCurrentTime();
    dt = curTime.Format("%Y-%m-%d %H:%M:%S");
    CString dateString = curTime.Format("%m%d");
    CString fileName = para.deviceName[id].Left(2) + dateString;
    memcpy(r.fileName, fileName, FILE_NAME_LENGTH);
    
    setResult(id, &r);
    return fileName;
}

void CLeakpressDlg::WriteResult(int id, CString alarmStr, bool alarm)
{
    CString dt;
    CString fileName = CreateFileName(id, dt);
    RESULT r = getResult(id);
    if (!alarm) {
        CString device_prefix = this->getDevicePrefix(id);
        if ("G" == device_prefix) {
            pthread_mutex_lock(&plc_mutex);
            fins->WriteDM((uint16_t)addr[id].address[MESHighLeakPCcontrol], (uint16_t)(10));
            fins->WriteDM((uint16_t)addr[id].address[HighLeakPCPress], (uint16_t)(r.dwLeakPress));
            fins->WriteDM((uint16_t)addr[id].address[HighLeakPCLeakValue], (uint16_t)(r.dwLeakValue));
            fins->WriteDM((uint16_t)addr[id].address[HighLeakPCFileName], r.fileName, FILE_NAME_LENGTH);

            // 读取 Workpress
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


            // 读取 P1 P2
            WORD urData = 0xff;
            fins->ReadDM((uint16_t)addr[id].address[LowLeakPCValueTest1], urData); r.dwTestPress1 = urData * 10;
            fins->ReadDM((uint16_t)addr[id].address[LowLeakPCValueTest2], urData); r.dwTestPress2 = urData * 10;
            pthread_mutex_unlock(&plc_mutex);
        }
        else if ("Y" == device_prefix) {
            pthread_mutex_lock(&plc_mutex);

            uint16_t data = (r.dwPosition % 10 >= 5) ? (r.dwPosition / 10 + 1) : r.dwPosition / 10; // 四舍五入
            fins->WriteDM((uint16_t)addr[id].address[MESPressPCcontrol], (uint16_t)(PC_ResultSended));
            fins->WriteDM((uint16_t)addr[id].address[PressPCPressData], (uint16_t)(r.dwPress));
            fins->WriteDM((uint16_t)addr[id].address[PressPCPressPosition], (uint16_t)(data));
            fins->WriteDM((uint16_t)addr[id].address[PressPCFileName], r.fileName, FILE_NAME_LENGTH);
            pthread_mutex_unlock(&plc_mutex);
        }
        else {
            return;
        }
        setResult(id, &r);
    }
    else {
        _tprintf(_T("%s: errorCode = %s\n"), getDevicePrefix2(id), alarmStr);
    }

    WriteResultToFile(para.fileSaveDir, dt, r, fileName, alarmStr, alarm);
}

void CLeakpressDlg::WriteResultToFile(CString dir, CString dt, RESULT r, CString fileName, CString alarmStr, bool alarm)
{
    long total_lines = 1;
    CString lineString;

    CString device_prefix = fileName.Left(1);

    // 生成目录
    if ("G" == device_prefix) {
        dir += "\\HighLeakpress";
    } else if ("D" == device_prefix) {
        dir += "\\LowLeakpress";
    } else if ("Y" == device_prefix) {
        dir += "\\Press";
    } else {
        return;
    }

    // 目录不存在，创建
    if (!FileManager::CheckFolderExist(dir)) {
        FileManager::CreateMuliteDirectory(dir);
    }

    // 文件头，行号
    CString path = dir + "\\" + fileName + ".csv";
    CString bkPath = dir + "\\" + fileName + ".txt";
    if (FileManager::CheckFileExist(path)) {
        total_lines = FileManager::CheckFileExist(bkPath) ?
            FileManager::FileTotalLines(bkPath) : FileManager::FileTotalLines(path);

    } else {
        if ("G" == device_prefix) {
            lineString.Append(_T("NO, DATETIME, PRESS, LEAK, Workpress\n"));
        } else if ("D" == device_prefix) {
            lineString.Append(_T("NO, DATETIME, PRESS, LEAK, P1, P2, P1-P2\n"));
        } else if ("Y" == device_prefix) {
            lineString.Append(_T("NO, DATETIME, PRESS, Position\n"));
        }
    }

    // 文件内容
    CString temp;    
    temp.Format(_T("%d, "), total_lines);
    lineString.Append(temp);
    lineString.Append(dt);
    lineString.Append(_T(", "));
    
    if (alarm) {
        if ("G" == device_prefix) {
            temp.Format(_T("%s, %s, %s\n"), alarmStr, alarmStr, alarmStr);
        } else if ("D" == device_prefix) {
            temp.Format(_T("%s, %s, %s, %s, %s\n"), alarmStr, alarmStr, alarmStr, alarmStr, alarmStr);
        } else if ("Y" == device_prefix) {
            temp.Format(_T("%s, %s\n"), alarmStr, alarmStr);
        }
    } else {
        if ("G" == device_prefix) {
            temp.Format(_T("%ld, %ld, %ld\n"), r.dwLeakPress, r.dwLeakValue, r.dwWorkPress);
        } else if ("D" == device_prefix) {
            temp.Format(_T("%ld, %ld, %ld, %ld, %ld\n"), r.dwLeakPress, r.dwLeakValue, r.dwTestPress1, r.dwTestPress2, r.dwTestPress1 - r.dwTestPress2);
        } else if ("Y" == device_prefix) {
            temp.Format(_T("%ld, %ld\n"), r.dwPress, r.dwPosition);
        }
    }
    
    lineString.Append(temp);
    FileManager::SaveFile(lineString, path, bkPath);
}


CString CLeakpressDlg::getDevicePrefix(int id)
{
    CString device_prefix = para.deviceName[id].Left(1);
    return device_prefix;
}

CString CLeakpressDlg::getDevicePrefix2(int id)
{
    CString device_prefix = para.deviceName[id].Left(2);
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
    CString logString;

    switch (e->state)
    {
    case ATEQ_RESULT_1:
        r.dwLeakPress = e->value1;
        r.dwLeakValue = e->value2;
        logString = "ATEQ_RESULT_1";
        break;
    case ATEQ_STABLE_2:
        r.dwTestPress1 = e->value1;
        r.dwTestPress2 = e->value1; // 测试1、2发同一个通道
        logString = "ATEQ_STABLE_2";
        break;
    case PRESS_RESULT:
        r.dwPress = e->value1;
        r.dwPosition = e->value2;
        logString = "PRESS_RESULT";
    default:
        break;
    }

    this->setResult(e->id, &r);
    this->SetAteqState(e);

    CString logStr;
    logStr.Format(_T("[%s] [%s] [%d, %d]"), getDevicePrefix2(e->id), logString, e->value1, e->value2);
    OnRecord(logStr, _T("Ateq"));

    delete e;
    return 0;
}

void CLeakpressDlg::OnRecord(CString message, CString deviceName)
{
    if (para.recordType & LOG) {
        logRecorder.WriteLog(message, deviceName);
    } 
    
    if (para.recordType & CONSOLE) {
        //CString logString;
        CString dt = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
        //sprintf(logString.GetBuffer(0), "%s: [%s] %s\n", dt, deviceName, message);
        _tprintf(_T("%s: [%s] %s\n"), dt, deviceName, message);
    }
}

void CLeakpressDlg::OnTest(int id)
{
    bool bstart = false;
    CString device_prefix = getDevicePrefix(id);
    CString device_name = getDevicePrefix2(id);

    // 1.查询 PLC 复位信号
    do 
    {
        CHECK_THREAD_RESET(id)
        bstart = IsStartState(id);
        Sleep(500);
    } while (!bstart);

    OnRecord(_T("====start===="), device_name);
    ResetAteqState(id);

    if ("D" == device_prefix) {
        // 2.等待 2 段稳压
        do {
            CHECK_THREAD_RESET(id)
            QueryAteqTest(id);
            Sleep(100);
        } while (IsAteqStateMatch(id, ATEQ_REST));

        // 3.等待 2 段稳压结束
        bool error = true;
        while (IsAteqStateMatch(id, ATEQ_STABLE_1)) {
            CHECK_THREAD_RESET(id)
            QueryAteqTest(id);
            error = false;
            Sleep(100);
        }

        if (error) {
            OnRecord(_T("2 段漏气"), device_name);
            return;
        }
        else {
            OnRecord(_T("====2 段稳压结束===="), device_name);
            //Sleep(1000);
        }

        while (!IsAteqStateMatch(id, ATEQ_TEST_1, false)) {
            CHECK_THREAD_RESET(id)
            QueryAteqTest(id);
            Sleep(100);
        }

        OnRecord(_T("====查询2 段结果===="), device_name);

        // 4.查询 2 段结果
        do {
            CHECK_THREAD_RESET(id)
            QueryAteqResult(id);
            Sleep(100);
        } while (!IsAteqStateMatch(id, ATEQ_RESULT_1, false));

        OnRecord(_T("====等待 3 段稳压===="), device_name);

        // 5. 等待 3 段稳压
        do {
            CHECK_THREAD_RESET(id)
            QueryAteqTest(id);
            Sleep(100);
        } while (!IsAteqStateMatch(id, ATEQ_STABLE_2, false));

        if (QueryAteqState(id) == ATEQ_STABLE_2) {
            OnRecord(_T("sending test press"), device_name);
        }

        // 6. 3 段稳压，一直发送结果
        while (IsAteqStateMatch(id, ATEQ_STABLE_2, false)) {
            CHECK_THREAD_RESET(id)
            QueryAteqTest(id);
            Sleep(100);
            SendTestPress(id);
        }
        OnRecord(_T("====3 段稳压结束===="), device_name);
    }

    // 7.等待 PLC 获取结果
    OnRecord(_T("等待 PLC 读取结果"), device_name);
    while (!IsGetResult(id)) {
        CHECK_THREAD_RESET(id)
        Sleep(200);
    }

    // 8.查询压机结果
    if ("Y" == device_prefix) {
        OnRecord(_T("query press result"), device_name);
        QueryPressResult(id);
        while (!IsAteqStateMatch(id, PRESS_RESULT, false)) {
            CHECK_THREAD_RESET(id)
            //QueryPressResult(id);
            Sleep(100);
        }
    }  

    OnRecord(_T("send result"), device_name);
    SendResult(id); // 给 PLC 发送结果

    OnRecord(_T("waiting end"), device_name);

    // 8.等待 PLC 结束
    while (!IsEndState(id)) {
        CHECK_THREAD_RESET(id)
        Sleep(100);
    }

    bstart = false;

    OnRecord(_T("====end===="), device_name);
}

void CLeakpressDlg::OnAlarm(int id)
{
    ALA_TYPE alarmType;
    bool bFirstAlarm = false;
    while (!needExit() && (alarmType = getAlarmType(id))) {
        if (!bFirstAlarm) {
            OnRecord(_T("====报警===="), getDevicePrefix2(id));
            WriteALAResult(id, alarmType);
            ResetClearAlarm(id);
        }
        bFirstAlarm = true;
    }

    OnRecord(_T("====报警解除===="), getDevicePrefix2(id));
}


// 用于软件启动, 连接硬件（耗时操作）
UINT WINAPI ThreadInit(LPVOID pParam)
{
    CLeakpressDlg *pMain=(CLeakpressDlg*)pParam;
    pMain->Init();

    return 0;
}

UINT WINAPI ThreadTestProcess(LPVOID pParam)
{    
    pair<int, void *> *p = (pair<int, void *> *)pParam;
    CLeakpressDlg *pMain = (CLeakpressDlg *)p->second;

    while (!pMain->needExit()) {
        pMain->OnTest(p->first);
        pMain->OnAlarm(p->first);
    }

    return 0;
}

UINT WINAPI ThreadALAListener(LPVOID pParam)
{
    CLeakpressDlg *pMain=(CLeakpressDlg*)pParam;

    while (!pMain->needExit()) {
        for (int id = 0; !pMain->needExit() && id < NUM; id++) {
            PLC_ALA_TYPE alarmType = pMain->getAlarmType(id);
            if (pMain->getAlarmType(id) != alarmType) {
                pMain->setAlarmType(id, alarmType);
            }
            Sleep(1000);
        }
    }

    return 0;
}

ALA_TYPE CLeakpressDlg::getAlarmType(int id)
{
    pthread_mutex_lock(&plc_mutex);
    WORD urData = 0;
    fins->ReadDM((uint16_t)addr[id].address[ALA], urData);
    pthread_mutex_unlock(&plc_mutex);

    pthread_mutex_lock(&ala_mutex);
    deviceAlarm[id] = (ALA_TYPE)urData;
    ALA_TYPE alarmType = deviceAlarm[id];
    pthread_mutex_unlock(&ala_mutex);

    return alarmType;
}
void CLeakpressDlg::setAlarmType(int id, ALA_TYPE alarmType)
{
    pthread_mutex_lock(&ala_mutex);
    deviceAlarm[id] = alarmType;
    pthread_mutex_unlock(&ala_mutex);
}

void CLeakpressDlg::ResetClearAlarm(int id)
{
    pthread_mutex_lock(&plc_mutex);
    fins->WriteDM((uint16_t)addr[id].address[ALA], (uint16_t)(PC_ClearALA));
    pthread_mutex_unlock(&plc_mutex);
}


CString CLeakpressDlg::getALAString(ALA_TYPE alarmType)
{
    CString alarmStr;
    if (alarmType == ALA_NO) {
    } else if (alarmType == ALA_P1_TYPE1) {
        alarmStr = "Error";
    } else if (alarmType == ALA_P1_TYPE2) {
        alarmStr = "0";
    } else if(alarmType == ALA_P1_TYPE3){
        alarmStr = "";      
    }else if (alarmType == ALA_G1_TYPE1) {
        alarmStr = "Error";
    }else if (alarmType == ALA_G1_TYPE2) {
        alarmStr = "CYLError";
    }else if (alarmType == ALA_G1_TYPE3) {
        alarmStr = "50000";
    }else if (alarmType == ALA_G2_TYPE1) {
        alarmStr = "Error";
    }else if (alarmType == ALA_G2_TYPE2) {
        alarmStr = "CYLError";
    }else if (alarmType == ALA_G2_TYPE3) {
        alarmStr = "50000";
    }else if (alarmType == ALA_L1_TYPE1) {
        alarmStr = "Error";
    }else if (alarmType == ALA_L1_TYPE2) {
        alarmStr = "CYLError";
    }else if (alarmType == ALA_L1_TYPE3) {
        alarmStr = "50000";
    }else if (alarmType == ALA_L2_TYPE1) {
        alarmStr = "Error";
    }else if (alarmType == ALA_L2_TYPE2) {
        alarmStr = "CYLError";
    }else if (alarmType == ALA_L2_TYPE3) {
        alarmStr = "50000";
    }else if (alarmType == ALA_L3_TYPE1) {
        alarmStr = "Error";
    }else if (alarmType == ALA_L3_TYPE2) {
        alarmStr = "CYLError";
    }else if (alarmType == ALA_L3_TYPE3) {
        alarmStr = "50000";
    }
    return alarmStr;
}

bool CLeakpressDlg::needExit()
{
    return exit;
}

CString CLeakpressDlg::GetSoftwareVersion()
{
    TCHAR szFullPath[MAX_PATH];
    DWORD dwVerInfoSize = 0;
    DWORD dwVerHnd;
    VS_FIXEDFILEINFO * pFileInfo;

    ::GetModuleFileName(NULL, szFullPath, sizeof(szFullPath));
    dwVerInfoSize = ::GetFileVersionInfoSize(szFullPath, &dwVerHnd);
    if (dwVerInfoSize)
    {
        // If we were able to get the information, process it:
        HANDLE  hMem;
        LPVOID  lpvMem;
        unsigned int uInfoSize = 0;

        hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
        lpvMem = GlobalLock(hMem);
        GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);

        ::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

        WORD m_nProdVersion[4];

        // Product version from the FILEVERSION of the version info resource 
        m_nProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS); 
        m_nProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
        m_nProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
        m_nProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS); 

        CString strVersion ;
        //strVersion.Format(_T("The file's version : %d.%d.%d.%d"),m_nProdVersion[0],m_nProdVersion[1],m_nProdVersion[2],m_nProdVersion[3]);
        strVersion.Format(_T("%d.%d.%d.%d"),m_nProdVersion[0],m_nProdVersion[1],m_nProdVersion[2],m_nProdVersion[3]);

        GlobalUnlock(hMem);
        GlobalFree(hMem);

        //AfxMessageBox(strVersion);
        return strVersion;
    }
}