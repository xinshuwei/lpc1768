
// lpc1768_ontestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "lpc1768_ontest.h"
#include "lpc1768_ontestDlg.h"
#include "afxdialogex.h"
#include <io.h>
#include <list>
using namespace std;


#include <DbgHelp.h>
#pragma comment (lib,"dbghelp.lib")
//#pragma comment(lib,"libcserialportd.lib")

#include "winsock.h"
#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib")

#pragma  comment(lib,"Pcomm.lib")


#define  WM_UPDATEDATA   1000+1  // 更新界面 
#define  MaxBLock    4096



static HANDLE hExit;










void Uart_thread(LPCTSTR param)
{
	DWORD dRes;
	Clpc1768_ontestDlg* pthis = (Clpc1768_ontestDlg *)param;
	byte * data =NULL;
	byte bt;
	int recv_cout = 0;
	CString str;
	chrono::microseconds timespan(1000000);
	while (pthis->Uart_Open_flag)
	{
		dRes = WaitForSingleObject(hExit, 10);
		switch (dRes) {
		case WAIT_OBJECT_0://检测到退出线程命令
			return;
		default:
			break;
		}

		recv_cout = sio_iqueue(pthis->Uart_port);
		//有数据读到
		if (recv_cout > 0)
		{

			data = new byte [recv_cout];
			sio_read(pthis->Uart_port, (char*)data, recv_cout);

			//sprintf_s(str.GetBuffer(100), 100, data);
			for (int i = 0; i < recv_cout; i++)
			{
				bt = data[i];
				if (pthis->m_RecvHex.GetCheck())
					str.Format("0x%02x ", bt);
				else
					str.Format("%c", bt);
				pthis->m_strEditRecv += str;

			}
			pthis->m_rxlen += recv_cout;
			//pthis->GetDlgItem(IDC_EDIT_Recv)->SetWindowText(pthis->m_strEditRecv);
			SendMessage(pthis->m_hWnd, WM_UPDATEDATA, FALSE, FALSE);
			delete[]data;
			//str.ReleaseBuffer();
			str.Empty();
			pthis->DispStatus();
		}
		//else
		//{
		//	this_thread::sleep_for(timespan);
		//}



	}



}




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//波特率
int BaudRateArray[] = {9600,19200, 38400, 57600, 115200,230400,460800,921600 };
int CANBaudRate[] = { 50000,100000,125000,250000,500000 };
//CSerialPort m_Serial_Port;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()

END_MESSAGE_MAP()


// Clpc1768_ontestDlg 对话框



Clpc1768_ontestDlg::Clpc1768_ontestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LPC1768_ONTEST_DIALOG, pParent)
	, uart_baud_select(0)
	, m_strEditRecv(_T(""))
	, m_strEditSend(_T("www.xinshuwei.top"))
	, m_Web_port(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Clpc1768_ontestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_Uart_baud, m_Uart_Baud);
	DDX_Control(pDX, IDC_COMBO_Uart_port, m_Uart_Number);
	DDX_Control(pDX, IDC_BUTTON_OpenClose, m_OpenClose);
	DDX_Control(pDX, IDC_STATIC_state, m_state);
	DDX_Control(pDX, IDC_COMBO_WEB_Addr, m_Web_Ip);
	DDX_CBIndex(pDX, IDC_COMBO_Uart_baud, uart_baud_select);
	DDX_Control(pDX, IDC_COMBO_CAN_Baud, m_CAN_Baud);
	DDX_Control(pDX, IDC_COMBO_CAN_PORT, m_CAN_Port);
	DDX_Text(pDX, IDC_EDIT_Recv, m_strEditRecv);
	DDX_Text(pDX, IDC_EDIT_Send, m_strEditSend);
	DDX_Control(pDX, IDC_CHECK_RecvHex, m_RecvHex);
	DDX_Control(pDX, IDC_BUTTON_Send, m_SendButton);
	DDX_Control(pDX, IDC_HexSend, m_HexSend);
	DDX_Control(pDX, IDC_CHECK_AutoSend, m_AutoSend);
	DDX_Control(pDX, IDC_BUTTON_ClearSend, m_ClearSendButton);
	DDX_Control(pDX, IDC_COMBO_WEB_Type, m_Web_Type);
	DDX_Text(pDX, IDC_EDIT_WEB_port, m_Web_port);
}

BEGIN_MESSAGE_MAP(Clpc1768_ontestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &Clpc1768_ontestDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_OpenClose, &Clpc1768_ontestDlg::OnBnClickedButtonOpenclose)
	ON_BN_CLICKED(IDC_RADIO_UART, &Clpc1768_ontestDlg::OnBnClickedRadioUart)
	ON_BN_CLICKED(IDC_RADIO_CAN, &Clpc1768_ontestDlg::OnBnClickedRadioCan)
	ON_BN_CLICKED(IDC_RADIO_WEB, &Clpc1768_ontestDlg::OnBnClickedRadioWeb)
	ON_BN_CLICKED(IDC_BUTTON_SeachPort, &Clpc1768_ontestDlg::OnBnClickedButtonSeachport)
	ON_BN_CLICKED(IDC_BUTTON_SeachIP, &Clpc1768_ontestDlg::OnBnClickedButtonSeachip)
	ON_CBN_SELCHANGE(IDC_COMBO_Uart_baud, &Clpc1768_ontestDlg::OnCbnSelchangeComboUartbaud)
	ON_CBN_SELCHANGE(IDC_COMBO_Uart_port, &Clpc1768_ontestDlg::OnCbnSelchangeComboUartport)
	ON_CBN_SELCHANGE(IDC_COMBO_CAN_Baud, &Clpc1768_ontestDlg::OnCbnSelchangeComboCanBaud)
	ON_CBN_SELCHANGE(IDC_COMBO_CAN_PORT, &Clpc1768_ontestDlg::OnCbnSelchangeComboCanPort)
	//ON_MESSAGE(WM_COMM_RXCHAR, OnComm)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATEDATA, &Clpc1768_ontestDlg::OnUpdatedata)
	ON_BN_CLICKED(IDC_CHECK_RecvHex, &Clpc1768_ontestDlg::OnBnClickedCheckRecvhex)
	ON_BN_CLICKED(IDC_BUTTON_ClearRecv, &Clpc1768_ontestDlg::OnBnClickedButtonClearrecv)
	ON_BN_CLICKED(IDC_BUTTON_Send, &Clpc1768_ontestDlg::OnBnClickedButtonSend)
	//ON_BN_CLICKED(IDC_BUTTON2, &Clpc1768_ontestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_ClearSend, &Clpc1768_ontestDlg::OnBnClickedButtonClearsend)
	ON_BN_CLICKED(IDC_CHECK_AutoSend, &Clpc1768_ontestDlg::OnBnClickedCheckAutosend)
END_MESSAGE_MAP()






//是否是数据段必备模块
inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
	if (pModuleName == 0)
	{
		return FALSE;
	}

	WCHAR szFileName[_MAX_FNAME] = L"";

	//_wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);
	_wsplitpath_s(pModuleName, NULL, NULL, szFileName,NULL,NULL,NULL,NULL,NULL);

	if (_wcsicmp(szFileName, L"ntdll") == 0)
		return TRUE;

	return FALSE;
}




//Dump文件回调
inline BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT   pInput, PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
	if (pInput == 0 || pOutput == 0)
		return FALSE;

	switch (pInput->CallbackType)
	{
	case ModuleCallback:
		if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
			if (!IsDataSectionNeeded(pInput->Module.FullPath))
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		return TRUE;
	default:;
	}

	return FALSE;
}

//创建dump文件
inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = NULL;

		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam = 0;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);

		CloseHandle(hFile);
	}
}
// 当出现崩溃时，会创建core.dmp 崩溃文件
LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	CreateMiniDump(pExceptionInfo, _T("core.dmp"));

	return EXCEPTION_EXECUTE_HANDLER;
}

// Clpc1768_ontestDlg 消息处理程序

BOOL Clpc1768_ontestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	if (GetLastError() == ERROR_ALREADY_EXISTS)//程序已在运行
	{
		AfxMessageBox(_T("程序已在运行"));
		SendMessage(WM_CLOSE);//关闭窗口
		return FALSE;
	}

	//dump文件处理
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);



	CString temp;
	//添加波特率到下拉列表
	for (int i = 0; i < sizeof(BaudRateArray) / sizeof(int); i++)
	{
		temp.Format(_T("%d"), BaudRateArray[i]);
		m_Uart_Baud.AddString((LPCTSTR)temp);
	}



	for (int i = 0; i < sizeof(CANBaudRate) / sizeof(int); i++)
	{
		temp.Format(_T("%d"), CANBaudRate[i]);
		m_CAN_Baud.AddString((LPCTSTR)temp);
	}

	GetUartPort(m_Uart_Number);
	OnCbnSelchangeComboUartport();


	if ((_access(".\\SysCfg.ini", 0) == -1))//如果不存在
	{
		WritePrivateProfileString(_T("cfg"), _T("LptType"), _T("0"), _T(".\\SysCfg.ini"));
		WritePrivateProfileString(_T("Uart"), _T("Baud"), _T("115200"), _T(".\\SysCfg.ini"));
		WritePrivateProfileString(_T("Can"), _T("Port"), _T("0"), _T(".\\SysCfg.ini"));
		WritePrivateProfileString(_T("Can"), _T("Baud"), _T("100000"), _T(".\\SysCfg.ini"));
		WritePrivateProfileString(_T("Web"), _T("Type"), _T("0"), _T(".\\SysCfg.ini"));
		WritePrivateProfileString(_T("Web"), _T("Port"), _T("1530"), _T(".\\SysCfg.ini"));



	}
	LptType = ::GetPrivateProfileInt(_T("cfg"), _T("LptType"), 1, _T(".\\SysCfg.ini"));
	Can_port = ::GetPrivateProfileInt(_T("Can"), _T("Port"), 1, _T(".\\SysCfg.ini"));
	temp.Format(_T("%d"), ::GetPrivateProfileInt(_T("Uart"), _T("Baud"), 1, _T(".\\SysCfg.ini")));
	m_Uart_Baud.SetCurSel(m_Uart_Baud.FindString(0, temp));
	OnCbnSelchangeComboUartbaud();

	m_Web_Type.SetCurSel(::GetPrivateProfileInt(_T("Web"), _T("Type"), 1, _T(".\\SysCfg.ini")));
	
	m_Web_port = ::GetPrivateProfileInt(_T("Web"), _T("Port"), 1, _T(".\\SysCfg.ini"));

	temp.Format(_T("%d"), ::GetPrivateProfileInt(_T("Can"), _T("Baud"), 1, _T(".\\SysCfg.ini")));
	m_CAN_Baud.SetCurSel(m_CAN_Baud.FindString(0, temp));
	m_CAN_Port.SetCurSel(Can_port);


	


	//::GetPrivateProfileString("cfg", "ud_angle", " DefaultName", ud_angle_string.GetBuffer(100), 100, ".\\SysCfg.ini");
	//::GetPrivateProfileString("cfg", "lr_angle", " DefaultName", lr_angle_string.GetBuffer(100), 100, ".\\SysCfg.ini");
	//ud_angle_string.ReleaseBuffer();
	//lr_angle_string.ReleaseBuffer();
	//xFOVdeg = atof(lr_angle_string);
	//yFOVdeg = atof(ud_angle_string);



	switch (LptType)
	{
	case 0:
		CheckDlgButton(IDC_RADIO_UART, 1);
		OnBnClickedRadioUart();
		break;
	case 1:
		CheckDlgButton(IDC_RADIO_CAN, 1);
		OnBnClickedRadioCan();
		break;
	case 2:
		CheckDlgButton(IDC_RADIO_WEB, 1);
		OnBnClickedRadioWeb();
		break;
	default:
		CheckDlgButton(IDC_RADIO_UART, 1);
		OnBnClickedRadioUart();
		break;
	}
	GetDlgItem(IDC_RADIO_CAN)->EnableWindow(FALSE);
	//获取多网卡IP地址
	GetWebIp();



	









	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Clpc1768_ontestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Clpc1768_ontestDlg::OnPaint()
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


BOOL Clpc1768_ontestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE) || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}





//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Clpc1768_ontestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






void Clpc1768_ontestDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void Clpc1768_ontestDlg::GetUartPort(CComboBox &colComPort)
{
	
		HKEY hKey;

		if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &hKey) == ERROR_SUCCESS) // 打开串口注册表
		{
			int i = 0;
			wchar_t portName[256], commName[256];
			DWORD dwLong, dwSize;

			while (1)
			{
				dwLong = dwSize = sizeof(portName) / 2;   

				if (RegEnumValueW(hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize) == ERROR_NO_MORE_ITEMS) // 枚举串口
				{
					break;
				}

				colComPort.AddString(CString(commName)); // commName就是串口名字
				i++;
		}

			if (colComPort.GetCount() == 0)
			{
				//::PostMessage(GetMainFrame()->m_hWnd, WM_UPDATE_SYS_INFO, NULL, (LPARAM)SYSINFO_NO_COM);
				m_state.SetWindowText(_T("未发现串口"));
			}
			else
			{
				CString strTemp;

				strTemp.Format("共找到%d个串口可用...", colComPort.GetCount());  //display serached port, zhb 18/01/13
				m_state.SetWindowText(strTemp);

			}

			RegCloseKey(hKey);
	}


		if (m_Uart_Number.GetCount()>0)
		{
			m_Uart_Number.SetCurSel(0);
		}
}




void Clpc1768_ontestDlg::GetWebIp()
{	

	m_Web_Ip.ResetContent();//清除
	static int ADAPTERNUM = 10;
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO[ADAPTERNUM];// 10个网卡空间 足够了 
	unsigned long stSize = sizeof(IP_ADAPTER_INFO) * ADAPTERNUM;
	// 获取所有网卡信息，参数二为输入输出参数 
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	// 空间不足
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// 释放空间
		if (pIpAdapterInfo != NULL)
			delete[] pIpAdapterInfo;
		return;
	}
	PIP_ADAPTER_INFO cur = pIpAdapterInfo;
	// 多个网卡 通过链表形式链接起来的 
	while (cur) {
		//cout << "网卡描述：" << cur->Description << endl;
		switch (cur->Type) {
		case MIB_IF_TYPE_OTHER:
			break;
		case MIB_IF_TYPE_ETHERNET:
		{
			IP_ADDR_STRING *pIpAddrString = &(cur->IpAddressList);
			//cout << "IP:" << pIpAddrString->IpAddress.String << endl;
			//cout << "子网掩码:" << pIpAddrString->IpMask.String << endl;
			m_Web_Ip.AddString(pIpAddrString->IpAddress.String);

		}
		break;
		case MIB_IF_TYPE_TOKENRING:
			break;
		case MIB_IF_TYPE_FDDI:
			break;
		case MIB_IF_TYPE_PPP:
			break;
		case MIB_IF_TYPE_LOOPBACK:
			break;
		case MIB_IF_TYPE_SLIP:
			break;
		default://无线网卡,Unknown type
		{
			IP_ADDR_STRING *pIpAddrString = &(cur->IpAddressList);
			//cout << "IP:" << pIpAddrString->IpAddress.String << endl;
			//cout << "子网掩码:" << pIpAddrString->IpMask.String << endl;
			m_Web_Ip.AddString(pIpAddrString->IpAddress.String);
		}
		break;
		}
		//char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

		//// mac 地址一般6个字节 
		//// mac 二进制转16进制字符串
		//char macStr[18] = { 0 };//12+5+1
		//int k = 0;
		//for (int j = 0; j < cur->AddressLength; j++) {
		//	macStr[k++] = hex[(cur->Address[j] & 0xf0) >> 4];
		//	macStr[k++] = hex[cur->Address[j] & 0x0f];
		//	macStr[k++] = '-';
		//}
		//macStr[k - 1] = 0;

		//cout << "MAC:" << macStr << endl; // mac地址 16进制字符串表示 
		cur = cur->Next;
		//cout << "--------------------------------------------------" << endl;
	}

	// 释放空间
	if (pIpAdapterInfo != NULL)
		delete[] pIpAdapterInfo;
}





void Clpc1768_ontestDlg::OnBnClickedButtonOpenclose()
{
	GetDlgItem(IDC_BUTTON_OpenClose)->EnableWindow(FALSE);
	CString temp;
	m_OpenClose.GetWindowText(temp);///获取按钮的文本
	UpdateData(true);
	if (temp =="关闭")///表示点击后是"关闭"，也就是此时串口或网络是打开的
		{
			switch (LptType)
			{
				case 0://串口
					//m_Serial_Port.ClosePort();//关闭
					sio_close(Uart_port);
					Uart_Open_flag = false;
					SetEvent(hExit);
					m_Uart_Baud.EnableWindow(TRUE);
					m_Uart_Number.EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_SeachPort)->EnableWindow(TRUE);
					m_state.SetWindowText("串口已关闭");
					break;
				case 2://web
					m_state.SetWindowTextA("WEB已关闭");
					break;
				default:
					break;
			}
			m_OpenClose.SetWindowText("开启");///设置按钮文字为"打开串口"
		}
	else//串口或网络未打开 界面显示是开启
	{


		switch (LptType)
		{
		case 0://串口
			Open_Uart();
			
			break;
		case 2://web
			m_state.SetWindowTextA("WEB已开启");
			break;
		default:
			break;
		}

	}



	GetDlgItem(IDC_BUTTON_OpenClose)->EnableWindow(TRUE);
}


void __stdcall Uart_Cntlrq(int port)
{

	//sio_read(port, ibuf, 20); // 接 收 数 据 
	WPARAM ch = sio_getch(port);
	BSTR b = (BSTR)ch;
	CString s(b);
	SysFreeString(b);
	//m_strEditRecv += s;
	//UpdateData(FALSE);//将接收到的字符显示到编辑框上
}


void Clpc1768_ontestDlg::Open_Uart(void)
{
	CString static_text;
	int select_baud = m_Uart_Baud.GetCurSel();


	if (m_Uart_Number.GetCount() > 0)///当前列表的内容个数
	{

		int stats = sio_open(Uart_port);

		if (stats==SIO_OK)
		{
			sio_ioctl(Uart_port, select_baud +12, P_NONE | BIT_8 | STOP_1);

			//void __stdcall(* p)(int);
			//p = Uart_Cntlrq;
			//sio_cnt_irq(Uart_port, Uart_Cntlrq, 1);

			Uart_Open_flag = TRUE;
			m_OpenClose.SetWindowText("关闭");///设置按钮文字为"关闭"
			m_Uart_Baud.EnableWindow(FALSE);
			m_Uart_Number.EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SeachPort)->EnableWindow(FALSE);
			static_text.Format("串口 已打开COM%d 波特率%d  8  N 1", Uart_port, BaudRateArray[select_baud]);
			m_state.SetWindowText(static_text);
			hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
			CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE) Uart_thread, this, 0, NULL);
		}
		else
		{
			AfxMessageBox("未发现此串口或被占用");
			Uart_Open_flag = false;
			m_OpenClose.SetWindowText("开启");///设置按钮文字为"开启"
		}


	}
	else
	{
		AfxMessageBox("未发现串口");
		Uart_Open_flag = false;
		m_OpenClose.SetWindowText("开启");///设置按钮文字为"开启"

	}

}





void Clpc1768_ontestDlg::OnBnClickedRadioUart()
{
	// TODO: 在此添加控件通知处理程序代码
	//CheckDlgButton(IDC_RADIO_CAN, 0);
	CheckDlgButton(IDC_RADIO_WEB, 0);
	LptType = 0;
	GetDlgItem(IDC_COMBO_Uart_baud)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_Uart_port)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SeachPort)->EnableWindow(TRUE);

	GetDlgItem(IDC_COMBO_WEB_Type)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_WEB_Addr)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WEB_port)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SeachIP)->EnableWindow(FALSE);


	//GetDlgItem(IDC_COMBO_CAN_Baud)->EnableWindow(FALSE);
	//GetDlgItem(IDC_COMBO_CAN_PORT)->EnableWindow(FALSE);
	m_state.SetWindowText("开启串口配置");
}

void Clpc1768_ontestDlg::OnBnClickedRadioCan()
{
	// TODO: 在此添加控件通知处理程序代码
	CheckDlgButton(IDC_RADIO_UART, 0);
	CheckDlgButton(IDC_RADIO_WEB, 0);
	LptType = 1;
	GetDlgItem(IDC_COMBO_Uart_baud)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_Uart_port)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SeachPort)->EnableWindow(FALSE);

	GetDlgItem(IDC_COMBO_WEB_Type)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_WEB_Addr)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WEB_port)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SeachIP)->EnableWindow(FALSE);


	GetDlgItem(IDC_COMBO_CAN_Baud)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_CAN_PORT)->EnableWindow(TRUE);
	m_state.SetWindowText("开启CAN配置");

}

void Clpc1768_ontestDlg::OnBnClickedRadioWeb()
{
	// TODO: 在此添加控件通知处理程序代码
	CheckDlgButton(IDC_RADIO_UART, 0);
	//CheckDlgButton(IDC_RADIO_CAN, 0);

	LptType = 2;
	GetDlgItem(IDC_COMBO_Uart_baud)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_Uart_port)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SeachPort)->EnableWindow(FALSE);

	GetDlgItem(IDC_COMBO_WEB_Type)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_WEB_Addr)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_WEB_port)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SeachIP)->EnableWindow(TRUE);


	//GetDlgItem(IDC_COMBO_CAN_Baud)->EnableWindow(FALSE);
	//GetDlgItem(IDC_COMBO_CAN_PORT)->EnableWindow(FALSE);
	m_state.SetWindowText("开启网络配置");

}

void Clpc1768_ontestDlg::OnBnClickedButtonSeachport()
{
	m_Uart_Number.ResetContent();
	// TODO: 在此添加控件通知处理程序代码
	GetUartPort(m_Uart_Number);

}

void Clpc1768_ontestDlg::OnBnClickedButtonSeachip()
{
	// TODO: 在此添加控件通知处理程序代码
	GetWebIp();
}

void Clpc1768_ontestDlg::OnCbnSelchangeComboUartbaud()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString text, sendtext;
	int nSel = m_Uart_Baud.GetCurSel();
	m_Uart_Baud.GetLBText(nSel, text);
	Uart_baud = _ttoi(text);
	//sendtext
	//sprintf_s(sendtext,)
	sendtext.Format("当前串口波特率%d", Uart_baud);
	SetDlgItemText(IDC_STATIC_state, sendtext);
	
}

void Clpc1768_ontestDlg::OnCbnSelchangeComboUartport()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString text, sendtext;
	int nSel = m_Uart_Number.GetCurSel();
	m_Uart_Number.GetLBText(nSel, text);
	Uart_port = String2int(text);
	sendtext.Format("当前串口号 %d", Uart_port);
	SetDlgItemText(IDC_STATIC_state, sendtext);
	
}


void Clpc1768_ontestDlg::OnCbnSelchangeComboCanBaud()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString text, sendtext;
	int nSel = m_CAN_Baud.GetCurSel();
	m_CAN_Baud.GetLBText(nSel, text);
	Can_baud = _ttoi(text);
	sendtext.Format("当前CAN波特率%d", Can_baud);
	SetDlgItemText(IDC_STATIC_state, sendtext);
}


void Clpc1768_ontestDlg::OnCbnSelchangeComboCanPort()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	Can_port = m_CAN_Port.GetCurSel();
	CString sendtext;
	sendtext.Format("当前CAN端口%d", Can_port+1);
	SetDlgItemText(IDC_STATIC_state, sendtext);
}



int Clpc1768_ontestDlg::String2int(CString str)
{
	int length;
	int i;
	TCHAR temp2[15];
	TCHAR temp[4];  // !!!! 尺寸小于temp2, 作其他用途必须改写 !!!!
	int count;

	lstrcpy(temp2, str);    //copies a string to a buffer

	count = 0;
	length = strlen(temp2);    //length保存了字符数组temp2的长度

	for (i = 0; i < length; i++)
	{
		if ((temp2[i] <= '9') && (temp2[i] >= '0'))
		{
			temp[count] = temp2[i];
			count++;
		}
	}

	temp[count] = 0;          //add string tail.\0'  zhb.2 18/01/23

	return (::atoi(temp));
}





//LONG Clpc1768_ontestDlg::OnComm(WPARAM ch, LPARAM port)
//{
//	BSTR b = (BSTR)ch;
//	CString s(b);
//	SysFreeString(b);
//	m_strEditRecv += s;
//	UpdateData(FALSE);//将接收到的字符显示到编辑框上
//
//	return 0;
//
//
//}






//更新UI界面
afx_msg LRESULT Clpc1768_ontestDlg::OnUpdatedata(WPARAM wParam, LPARAM lParam)
{
	UpdateData(lParam);
	return 0;
}


void Clpc1768_ontestDlg::OnBnClickedCheckRecvhex()
{
	// TODO: 在此添加控件通知处理程序代码
}


void Clpc1768_ontestDlg::OnBnClickedButtonClearrecv()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strEditRecv.Empty();
	UpdateData(FALSE);
}



void Clpc1768_ontestDlg::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	char abOut[MaxBLock];
	char  sendOut[MaxBLock];
	int OutNum, length;
	if (LptType == 0)
	{
		if (!Uart_Open_flag)
		{
			AfxMessageBox("串口未打开");
			return;
		}
		m_SendButton.EnableWindow(FALSE);
		m_ClearSendButton.EnableWindow(FALSE);

		CString str;
		GetDlgItem(IDC_EDIT_Send)->GetWindowText(str);
		int len = str.GetLength();
		for (int i=0;i<len;i++)
		{
			abOut[i] = str.GetAt(i);
		}
		if (m_HexSend.GetCheck())
		{
			CString strHexdata;
			abOut[len] = NULL;
			strHexdata = CString(abOut);
			len = String2Hex(strHexdata, sendOut);
			length = sio_write(Uart_port, sendOut, len);
		}
		else
		{
			length = sio_write(Uart_port,abOut, len);
		}
		m_txlen += length;
		DispStatus();
		m_SendButton.EnableWindow(TRUE);
		m_ClearSendButton.EnableWindow(TRUE);

		return;

		
		

	}
	if (LptType ==2)
	{




		return;



	}



	return;
}




int Clpc1768_ontestDlg::String2Hex(CString str, char *SendOut)
{
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.GetLength();
	//SendOut.SetSize(len/2);
	for (int i = 0; i < len;)
	{
		char lstr, hstr = str[i];
		if (hstr == ' ' || hstr == '\r' || hstr == '\n')
		{
			i++;
			continue;
		}
		i++;
		if (i >= len)
			break;
		lstr = str[i];
		hexdata = ConvertHexData(hstr);
		lowhexdata = ConvertHexData(lstr);
		if ((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata * 16 + lowhexdata;
		i++;
		SendOut[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	//senddata.SetSize(hexdatalen);
	return hexdatalen;
}

char Clpc1768_ontestDlg::ConvertHexData(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return(-1);
}



void Clpc1768_ontestDlg::DispStatus(void)
{
	CWnd *static_txlen = GetDlgItem(IDC_STATIC_TxCount);
	CWnd *static_rxlen = GetDlgItem(IDC_STATIC_RxCount);
	CString txlen_str, rxlen_str;
	txlen_str.Format("Tx:%ld", m_txlen);
	static_txlen->SetWindowText(txlen_str);
	rxlen_str.Format("Rx:%ld", m_rxlen);
	static_rxlen->SetWindowText(rxlen_str);

}


void Clpc1768_ontestDlg::OnBnClickedButtonClearsend()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strEditSend.Empty();
	UpdateData(FALSE);
}


void Clpc1768_ontestDlg::OnBnClickedCheckAutosend()
{
	// TODO: 在此添加控件通知处理程序代码
	bAutoSend = !bAutoSend;
	if (bAutoSend)
	{
		SetTimer(1, 1000, NULL);
	} 
	else
	{
		KillTimer(1);
	}
}

void Clpc1768_ontestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==1)
	{
		OnBnClickedButtonSend();
	}
	CDialogEx::OnTimer(nIDEvent);
}
