
// lpc1768_ontestDlg.h : 头文件
//

#pragma once
//#include "SerialPort.h"
//#include "SerialPortInfo.h"
#include "Pcomm.h"
#include "afxwin.h"
#include "stdio.h"
#include  <iostream>
#include <vector> 
#include <string.h>
#include <thread>

//using namespace itas109;
using namespace std;
// Clpc1768_ontestDlg 对话框
class Clpc1768_ontestDlg : public CDialogEx
{
// 构造
public:
	Clpc1768_ontestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LPC1768_ONTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonOpenclose();
	void Open_Uart(void);
	//void Uart_Cntlrq(void);
	afx_msg void OnBnClickedRadioUart();
	afx_msg void OnBnClickedRadioCan();
	afx_msg void OnBnClickedRadioWeb();
	afx_msg void OnBnClickedButtonSeachport();
	afx_msg void OnBnClickedButtonSeachip();
	afx_msg void OnCbnSelchangeComboUartbaud();
	afx_msg void OnCbnSelchangeComboUartport();
	afx_msg void OnCbnSelchangeComboCanBaud();
	afx_msg void OnCbnSelchangeComboCanPort();



	void GetUartPort(CComboBox & colComPort);
	void GetWebIp();
	bool Uart_Open_flag = false;
	bool bAutoSend = false;
	long m_txlen = 0;
	long m_rxlen = 0;
	//通讯类型
	int LptType; // 0 串口   2  web
	int Can_port; //0 CAN1  1  CAN2
	int Can_baud;
	// 串口波特率
	CComboBox m_Uart_Baud;
	// 串口号
	CComboBox m_Uart_Number;
	// 开启关闭按钮
	CButton m_OpenClose;
	// 状态显示
	CStatic m_state;
	CComboBox m_Web_Ip;
	int Uart_baud;
	int Uart_port;
	CComboBox m_CAN_Baud;
	int uart_baud_select;
	int String2int(CString str);
	CComboBox m_CAN_Port;
	//void Uart_Recv_thread(void);
	LPVOID LoactionParam;//udp priv
	CString m_strEditRecv;
	CString m_strEditSend;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnUpdatedata(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCheckRecvhex();
	//BOOL Uart_RecvHex;
	CButton m_RecvHex;
	afx_msg void OnBnClickedButtonClearrecv();
	int String2Hex(CString str, char * SendOut);
	char ConvertHexData(char ch);
	afx_msg void OnBnClickedButtonSend();
	CButton m_SendButton;
	CButton m_HexSend;
	CButton m_AutoSend;
	//afx_msg void OnBnClickedButton2();
	afx_msg
		void DispStatus(void);
	void OnBnClickedButtonClearsend();
	afx_msg void OnBnClickedCheckAutosend();
	// 清空发送
	CButton m_ClearSendButton;
	CComboBox m_Web_Type;
	int m_Web_port;
};
