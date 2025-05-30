// CGameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "LLK.h"
#include "GameDlg.h"
#include "afxdialogex.h"
#include "LLKDlg.h"


// CGameDlg 对话框

IMPLEMENT_DYNAMIC(CGameDlg, CDialogEx)

CGameDlg::CGameDlg(CWnd* pParent /*=nullptr*/) 
	: CDialogEx(IDD_GAME_DIALOG, pParent)
{
	//初始化起始点坐标
	m_ptGameTop.x = 50;
	m_ptGameTop.y = 50;

	//初始化图片元素大小
	m_sizeElem.cx = 80;
	m_sizeElem.cy = 80;

	//初始化游戏更新区域
	m_rtGameRect.top = m_ptGameTop.y;
	m_rtGameRect.left = m_ptGameTop.x;
	m_rtGameRect.right = m_rtGameRect.left + m_sizeElem.cx * MAX_COL;
	m_rtGameRect.bottom = m_rtGameRect.top + m_sizeElem.cy * MAX_ROW;

	//初始化图标选中状态
	m_bFirstPoint = true;
	m_bPlaying = false;
	GameMode=0;

}

CGameDlg::~CGameDlg()
{
}

void CGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_TIME, m_GameProgress);
}


BEGIN_MESSAGE_MAP(CGameDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_START, &CGameDlg::OnClickedButtonStart)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_TIPS, &CGameDlg::OnClickedButtonTips)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CGameDlg::OnClickedButtonRestart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CGameDlg::OnClickedButtonStop)
	ON_EN_CHANGE(IDC_EDIT_TIME, &CGameDlg::OnChangeEditTime)
	ON_BN_CLICKED(IDC_BUTTON_HELP, &CGameDlg::OnBnClickedButtonHelp)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_GAME_TIME, &CGameDlg::OnNMCustomdrawGameTime)
END_MESSAGE_MAP()




//初始化窗口背景和大小
void CGameDlg::InitBackground()
{	// TODO: 在此处添加实现代码.

	//加载BMP图片资源
	HANDLE Backbmp = ::LoadImage(NULL, _T("theme\\picture\\BG1(1).bmp"), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	
	//获得当前对话框的视频内容
	CClientDC dc(this);

	//创建与视频内容兼容的内存DC
	m_dcBG.CreateCompatibleDC(&dc);

	//将位图资源选入DC
	m_dcBG.SelectObject(Backbmp);

	//初始化内存DC
	m_dcMem.CreateCompatibleDC(&dc);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(&dc, 800, 600);
	m_dcMem.SelectObject(&bmpMem);

	//hu绘制背景到内存DC中
	m_dcMem.BitBlt(0, 0, 800, 600, &m_dcBG, 0, 0, SRCCOPY);

	//设置窗口大小
	UpdateWindow();
}


BOOL CGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	this->ModifyStyleEx(0, WS_ACTIVECAPTION);

	// 初始化背景和元素
	InitBackground();
	InitElement();

	// 调用 InitMode 设置控件状态（不再默认隐藏）
	InitMode(this->GameMode);

	return TRUE;
}
void CGameDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	dc.BitBlt(0, 0, 800, 600, &m_dcMem, 0, 0, SRCCOPY);
}

//初始化元素
void CGameDlg::InitElement() {
	CClientDC dc(this);

	//加载BMP图片资源
	HANDLE Elmbmp = ::LoadImage(NULL, _T("theme\\picture\\WPS03.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	m_dcElm.CreateCompatibleDC(&dc);

	m_dcElm.SelectObject(Elmbmp);

	//掩码图加载
	HANDLE Maskbmp = ::LoadImage(NULL, _T("theme\\picture\\WPS03(mask).bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	m_dcMask.CreateCompatibleDC(&dc);

	m_dcMask.SelectObject(Maskbmp);

	//加载掩盖图片
	//加载BMP图片资源
	HANDLE hCache = ::LoadImage(NULL, _T("theme\\picture\\pause1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//创建与视频内存兼容的内存DC
	m_dcCache.CreateCompatibleDC(&dc);
	//将位图资源选入DC
	m_dcCache.SelectObject(hCache);

}

//开始游戏
void CGameDlg::OnClickedButtonStart()
{
	if (m_bPause) return;

	m_gameControl.StartGame();
	m_bPlaying = true;
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	// 仅在非休闲模式下初始化进度条和定时器
	if (GameMode != 1) {
		timeCount = 300;
		m_GameProgress.SetRange(0, 60 * 5);
		m_GameProgress.SetStep(-1);
		m_GameProgress.SetPos(60 * 5);
		SetTimer(PLAY_TIMER_ID, 1000, NULL);
	}

	UpdateMap();
	Invalidate(FALSE);
}
//调整窗口大小
void CGameDlg::UpdateWindow()
{

	// TODO: 在此处添加实现代码.

	//调整窗口大小
	CRect rtWin;
	CRect rtClient;
	this->GetWindowRect(rtWin);		//获得窗口大小
	this->GetWindowRect(rtClient);	//获得客户区大小

									//标题栏和外边框的大小
	int nSpanWidth = rtWin.Width() - rtClient.Width();
	int nSpanHeight = rtWin.Width() - rtClient.Width();

	//设置窗口大小
	MoveWindow(0, 0, 800 + nSpanWidth, 600 + nSpanHeight);

	CenterWindow();
}


//更新地图
void CGameDlg::UpdateMap()
{
	// TODO: 在此处添加实现代码.

	//计算图片的顶点坐标与图片大小
	int nTop = m_ptGameTop.y;
	int nLeft = m_ptGameTop.x;
	int nElemW = m_sizeElem.cx;
	int nElemH = m_sizeElem.cy;

	m_dcMem.BitBlt(m_rtGameRect.left, m_rtGameRect.top,
		m_rtGameRect.Width(), m_rtGameRect.Height(), &m_dcBG, m_rtGameRect.left, m_rtGameRect.top, SRCCOPY);

	for (int i = 0; i < MAX_ROW; i++) {
		for (int j = 0; j < MAX_COL; j++) {

			int nInfo = m_gameControl.GetElement(i, j);       //当前元素图片的数值
			if (nInfo == BLANK)  continue;
			 
			
			//将背景与掩码相或，边保留，图像区域为1
			m_dcMem.BitBlt(nLeft + j * nElemW, nTop + i * nElemH, nElemW, nElemH, &m_dcMask, 0, nInfo * nElemH, SRCPAINT);
			
			//将元素图片相与，边保留，图像区域为元素图片
			m_dcMem.BitBlt(nLeft + j * nElemW, nTop + i * nElemH, nElemW, nElemH, &m_dcElm, 0, nInfo * nElemH, SRCAND);

		}
	}

}

void CGameDlg::SetGameMode(int mode)
{
	this->GameMode = mode;
}

void CGameDlg::InitMode(int mode)
{
	CProgressCtrl* pProgressCtrl = (CProgressCtrl*)GetDlgItem(IDC_GAME_TIME);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TIME);

	// 根据模式设置控件显示状态
	switch (mode) {
	case 0: // 基本模式：显示进度条和编辑框
		SetWindowTextW(_T("小然连连看 基本模式"));
		SetWindowTextW(_T("小然连连看 基本模式"));
		if (pProgressCtrl) pProgressCtrl->ShowWindow(SW_SHOW);
		if (pEdit) pEdit->ShowWindow(SW_SHOW);
		break;
	case 1: // 休闲模式：隐藏进度条和编辑框
		SetWindowTextW(_T("小然连连看 休闲模式"));
		if (pProgressCtrl) pProgressCtrl->ShowWindow(SW_HIDE);
		if (pEdit) pEdit->ShowWindow(SW_HIDE);
		break;
	default: // 关卡模式：可根据需要调整
		SetWindowTextW(_T("小然连连看 关卡模式"));
		break;
	}
}
void CGameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// TODO: 在此处添加实现代码.

	//去掉小于0状态
	if (point.x < m_ptGameTop.x || point.y < m_ptGameTop.y) {
		return CDialogEx::OnLButtonUp(nFlags, point);
	}
	
	//换算点击的坐标
	int nRow = (point.y - m_ptGameTop.y) / m_sizeElem.cy;//行号
	int nCol = (point.x - m_ptGameTop.x) / m_sizeElem.cx;

	//判断坐标的有效性
	if (nRow> MAX_ROW-1 || nCol > MAX_COL-1) {
		return CDialogEx::OnLButtonUp(nFlags, point);
	}

	//第一个点
	if (m_bFirstPoint) {
		//绘制提示框
		DrawTipFrame(nRow, nCol);
		m_gameControl.SetFirstPoint(nRow, nCol);

	}
	//第二个点
	else {
		//绘制提示框
		DrawTipFrame(nRow, nCol);
		m_gameControl.SetSecPoint(nRow, nCol);

		Vertex avPath[MAX_VERTEX_NUM];     //获得路径   16为路径最长（不会到16条路径的）
		int nVexnum = 0;      //顶点个数
		
		//判断是否是相同图片	
		if (m_gameControl.Link(avPath, nVexnum)) {

			//画提示线
			DrawTipLine(avPath, nVexnum);

			//更新地图
			UpdateMap();
		}
		Sleep(200);    //延迟
		InvalidateRect(m_rtGameRect,FALSE); //局部矩形更新
		
		//判断胜负
		JudgeWin();
		
		/*
		if (m_gameControl.IsWin()) {
			MessageBox(_T("\t获胜！"),_T("欢乐连连看　基本模式"));
			m_bPlaying = false;
			this->GetDlgItem(IDC_BUTTON_START)->EnableWindow(true); //游戏结束，将【开始游戏】按钮设置为可点击状态

			return; 
		}*/
		
	}
	m_bFirstPoint = !m_bFirstPoint; //赋反值

	CDialogEx::OnLButtonUp(nFlags, point);
}


//绘制矩形提示框
void CGameDlg::DrawTipFrame(int nRow, int nCol) {
	CClientDC dc(this);
	CBrush brush(RGB(233, 43, 43));
	CRect rtTipFrame;
	rtTipFrame.left = m_ptGameTop.x + nCol * m_sizeElem.cx;
	rtTipFrame.top = m_ptGameTop.y + nRow * m_sizeElem.cy;
	rtTipFrame.right = rtTipFrame.left + m_sizeElem.cx;
	rtTipFrame.bottom = rtTipFrame.top + m_sizeElem.cy;
	dc.FrameRect(rtTipFrame, &brush);
}

//绘制提示线
void CGameDlg::DrawTipLine(Vertex avPath[MAX_VERTEX_NUM], int nVexnum)
{
	//获取DC
	CClientDC dc(this);

	//设置画笔
	CPen penLine(PS_SOLID, 2, RGB(0, 255, 0));

	//将画笔选入DC
	CPen* pOldPen = dc.SelectObject(&penLine);

	dc.MoveTo(m_ptGameTop.x + avPath[0].col * m_sizeElem.cx + m_sizeElem.cx / 2,
		m_ptGameTop.y + avPath[0].row * m_sizeElem.cy + m_sizeElem.cy / 2);
	//绘制连接线
	for (int i = 0; i < nVexnum - 1; i++)
	{

		dc.LineTo(m_ptGameTop.x + avPath[i + 1].col * m_sizeElem.cx + m_sizeElem.cx / 2,
			m_ptGameTop.y + avPath[i + 1].row * m_sizeElem.cy + m_sizeElem.cy / 2);
	}

	dc.SelectObject(pOldPen);
}


void CGameDlg::OnClickedButtonTips()
{
	// TODO: 在此添加控件通知处理程序代码

	//如果没有进行游戏，则返回
	if (m_bPlaying == false)
		return;

	//如果能够连通，绘制提示框和连接线并更新游戏地图
	Vertex avPath[MAX_VERTEX_NUM];     //获得路径
	int nVexnum = 0;      //顶点个数
	if (m_gameControl.Help(avPath, nVexnum))
	{

		//画第一个点的提示框
		DrawTipFrame(avPath[0].row, avPath[0].col);

		//画第一个点的提示框
		DrawTipFrame(avPath[nVexnum - 1].row, avPath[nVexnum - 1].col);

		//画提示线
		DrawTipLine(avPath, nVexnum);

		Sleep(1000);    //延迟
						
		UpdateMap();	//更新地图

		InvalidateRect(m_rtGameRect, FALSE);    //局部矩形更新
	}
	//为了使用第二个提示框可以看到，暂停200ms后，再刷新界面
}


void CGameDlg::OnClickedButtonRestart()
{
	// TODO: 在此添加控件通知处理程序代码

	//调用CGameControl：：ResetGraph();
	m_gameControl.Reset();

	//更新地图，调用UpdateMap(),更新界面显示
	UpdateMap();

	//通知界面重绘
	InvalidateRect(m_rtGameRect, FALSE);    //局部矩形更新

}


void CGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	JudgeWin();

	/*if (nIDEvent == PLAY_TIMER_ID && m_bPlaying) {
		//游戏时间减少一秒
		m_GameProgress.StepIt();
	}*/

	//设置进度条的位置之前，需要判断计时器的编号是否是当前的计时器
	if (nIDEvent == PLAY_TIMER_ID && m_bPlaying && m_bPause == false)
	{
		//游戏时间减一秒
		if (timeCount > -1)
		{
			CString str;
			str.Format(_T("%d"), timeCount);
			SetDlgItemText(IDC_EDIT_TIME, str);

			UpdateData(false);
			timeCount--;

			m_GameProgress.StepIt();
		}
		if (timeCount == 0) {
			KillTimer(1);
			m_dcMem.BitBlt(0, 0, 800, 600, &m_dcBG, 0, 0, SRCCOPY);
			UpdateMap();
			InvalidateRect(FALSE);
			MessageBox(TEXT("时间到！游戏失败!"));
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}

//判断胜负
void CGameDlg::JudgeWin()
{
	// 休闲模式仅判断是否清空地图
	if (GameMode == 1) {
		if (m_gameControl.IsWin(-1) == GAME_SUCCESS) { // 传递-1表示忽略时间
			m_bPlaying = false;
			KillTimer(PLAY_TIMER_ID);
			MessageBox(_T("获胜！"), _T("休闲模式"));
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		}
		return;
	}

	// 其他模式保持原有逻辑
	BOOL bGameStatus = m_gameControl.IsWin(m_GameProgress.GetPos());
	if (bGameStatus != GAME_PLAY) {
		m_bPlaying = false;
		KillTimer(PLAY_TIMER_ID);
		CString strTitle;
		GetWindowTextW(strTitle);
		MessageBox(bGameStatus == GAME_SUCCESS ? _T("获胜！") : _T("时间到！"), strTitle);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	}
}
//点击【暂停游戏】按钮
void CGameDlg::OnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码

	//获得当前对话框的视频内容
	CClientDC dc(this);

	//游戏没在进行时，直接返回
	if (m_bPlaying == false)
		return;

	//如果游戏没有暂停
	if (m_bPause == false)
	{
		//hu绘制背景到内存DC中
		m_dcMem.BitBlt(MAP_LETF, MAP_TOP, 640, 400, &m_dcCache, 0, 0, SRCCOPY);

		GetDlgItem(IDC_BUTTON_TIPS)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_RESTART)->EnableWindow(0);
		InvalidateRect(m_rtGameRect, FALSE);    //局部矩形更新
		this->GetDlgItem(IDC_BUTTON_STOP)->SetWindowTextW(_T("继续游戏"));
	}

	//如果游戏暂停
	if (m_bPause == true)
	{
		UpdateMap();
		GetDlgItem(IDC_BUTTON_TIPS)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_RESTART)->EnableWindow(1);
		InvalidateRect(m_rtGameRect, FALSE);    //局部矩形更新

		this->GetDlgItem(IDC_BUTTON_STOP)->SetWindowTextW(_T("暂停游戏"));
	}

	m_bPause = !m_bPause;
}


void CGameDlg::OnChangeEditTime()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CGameDlg::OnBnClickedButtonHelp()
{
	// TODO: 在此添加控件通知处理程序代码

	CHelpDialog dlg;
	dlg.DoModal();
}


void CGameDlg::OnNMCustomdrawGameTime(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
