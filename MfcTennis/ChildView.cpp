
// ChildView.cpp : CChildView 클래스의 구현
//

#include "stdafx.h"
#include "MfcTennis.h"
#include "ChildView.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//=============================================================================
// Enumeration
//-----------------------------------------------------------------------------
enum tag_GameStatus { GAMEOVER, RUNNING, PAUSE, WAITINGSERVE, DOINGSERVE };
enum tag_ScorePoint { ZERO = 0, FIRST = 15, SECOND = 30, THIRD = 40, ADVANTAGE = 65 };
enum tag_RefreeMsg	{ NONE, FAULT, FOOT_FAULT, DEUCE };
tag_GameStatus GameStatus;
tag_ScorePoint ScorePoint;
tag_RefreeMsg RefreeMsg;
//=============================================================================



// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_GAME_START, &CChildView::OnGameStart)
	ON_COMMAND(ID_GAME_PAUSE, &CChildView::OnGamePause)
	ON_COMMAND(ID_GAME_END, &CChildView::OnGameEnd)
	ON_UPDATE_COMMAND_UI(ID_GAME_PAUSE, &CChildView::OnUpdateGamePause)
	ON_WM_CREATE()
	ON_WM_INITMENU()
	ON_WM_DESTROY()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
//	ON_WM_SIZE()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CBrush brush;
	CRect rect;
	GetClientRect(&rect);
	CString str;

	// 게임 구조체에 저장된 픽셀값을 갱신
	m_TennisGame.nHResolution = rect.Width();
	m_TennisGame.nVResolution = rect.Height();

	dc.SelectStockObject(BLACK_BRUSH);
	dc.SelectStockObject(NULL_PEN);
	// 땅, 네트, 코트 그리기는 배경화면 적용 후에는 생략한다.
	dc.Rectangle(ReCalcWidth(LAND_LEFT), ReCalcHeight(LAND_TOP),
		ReCalcWidth(LAND_RIGHT), ReCalcHeight(HEIGHT));		// 땅 영역
	dc.SelectStockObject(BLACK_BRUSH);
	dc.SelectStockObject(BLACK_PEN);
	dc.Rectangle(ReCalcWidth(NET_LEFT), ReCalcHeight(NET_TOP),
		ReCalcWidth(NET_RIGHT), ReCalcHeight(HEIGHT));			// 네트 영역

	brush.CreateSolidBrush(RGB(0, 0, 255));
	dc.SelectObject(brush);
	dc.SelectStockObject(NULL_PEN);
	dc.Rectangle(ReCalcWidth(COURT_LEFT), ReCalcHeight(LAND_TOP),
		ReCalcWidth(COURT_RIGHT), ReCalcHeight(HEIGHT));	// 코트 영역
	brush.DeleteObject();

	brush.CreateSolidBrush(RGB(255, 0, 0));
	dc.SelectObject(brush);
	dc.Rectangle(ReCalcWidth(SERVICE_BOX_LEFT), ReCalcHeight(LAND_TOP),
		ReCalcWidth(SERVICE_BOX_RIGHT), ReCalcHeight(HEIGHT));	// 서비스 라인
	brush.DeleteObject();
	
	BITMAP bmpinfo[2];
	CDC dcmem[2];

	for(int i = 0; i < 2; i++) {
		// 선수 표시
		m_Player[0].bmpPlayer.GetBitmap(&bmpinfo[i]);
		dcmem[i].CreateCompatibleDC(&dc);
		dcmem[i].SelectObject(&m_Player[i].bmpPlayer);
		// 윈도우 크기가 기본값이면 원래 크기로 출력하고
		// 기본값이 아니면 크기 조정
		if( m_TennisGame.nHResolution == WIDTH &&
			m_TennisGame.nVResolution == HEIGHT)
			dc.BitBlt(m_Player[i].player_rect.left,
				m_Player[i].player_rect.top, PLAYER_WIDTH, PLAYER_HEIGHT,
				&dcmem[i], 0, 0, SRCCOPY);
		else
			dc.StretchBlt(ReCalcWidth(m_Player[i].player_rect.left),
				ReCalcHeight(m_Player[i].player_rect.top),
				ReCalcWidth(PLAYER_WIDTH), ReCalcHeight(PLAYER_HEIGHT),
				&dcmem[i], 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT, SRCCOPY);
		// 배경 그림이 완성되면 선수의 그림자도 만들어준다.
	}

	// 공 표시
	dc.SelectStockObject(NULL_PEN);
	dc.SelectStockObject(BLACK_BRUSH);
	dc.Ellipse(
		ReCalcWidth(m_Ball.ptCenter.x - BALL_RADIUS - 1),
		ReCalcHeight(m_Ball.ptCenter.y - BALL_RADIUS - 1),
		ReCalcWidth(m_Ball.ptCenter.x + BALL_RADIUS + 1),
		ReCalcHeight(m_Ball.ptCenter.y + BALL_RADIUS + 1));
	brush.CreateSolidBrush(RGB(0, 255, 0));
	dc.SelectObject(brush);
	dc.Ellipse(
		ReCalcWidth(m_Ball.ptCenter.x - BALL_RADIUS),
		ReCalcHeight(m_Ball.ptCenter.y - BALL_RADIUS),
		ReCalcWidth(m_Ball.ptCenter.x + BALL_RADIUS),
		ReCalcHeight(m_Ball.ptCenter.y + BALL_RADIUS));
	brush.DeleteObject();

	// 스코어 정보 표시
	/*str.Format(TEXT("%s : %d"), m_Player[0].strName, m_Player[0].nScore);
	dc.DrawText(str,
		CRect(ReCalcWidth(40), ReCalcHeight(30),
		ReCalcWidth(40) + 100, ReCalcHeight(30) + 50), DT_SINGLELINE);
	str.Format("%s : %d", m_Player[1].strName, m_Player[1].nScore);
	dc.DrawText(str,
		CRect(ReCalcWidth(600) - 100, ReCalcHeight(30),
		ReCalcWidth(600), ReCalcHeight(30) + 50),
		DT_RIGHT | DT_SINGLELINE);*/
	dc.SelectStockObject(BLACK_BRUSH);
	dc.SelectStockObject(WHITE_PEN);
	dc.Rectangle(ReCalcWidth(200), ReCalcHeight(30),
		ReCalcWidth(440), ReCalcHeight(210));
	dc.MoveTo(ReCalcWidth(200), ReCalcHeight(90));
	dc.LineTo(ReCalcWidth(440), ReCalcHeight(90));
	dc.MoveTo(ReCalcWidth(200), ReCalcHeight(110));
	dc.LineTo(ReCalcWidth(320), ReCalcHeight(110));
	dc.MoveTo(ReCalcWidth(200), ReCalcHeight(130));
	dc.LineTo(ReCalcWidth(320), ReCalcHeight(130));
	dc.MoveTo(ReCalcWidth(200), ReCalcHeight(150));
	dc.LineTo(ReCalcWidth(440), ReCalcHeight(150));
	dc.MoveTo(ReCalcWidth(224), ReCalcHeight(90));
	dc.LineTo(ReCalcWidth(224), ReCalcHeight(150));
	dc.MoveTo(ReCalcWidth(248), ReCalcHeight(90));
	dc.LineTo(ReCalcWidth(248), ReCalcHeight(150));
	dc.MoveTo(ReCalcWidth(272), ReCalcHeight(90));
	dc.LineTo(ReCalcWidth(272), ReCalcHeight(150));
	dc.MoveTo(ReCalcWidth(296), ReCalcHeight(90));
	dc.LineTo(ReCalcWidth(296), ReCalcHeight(150));
	dc.MoveTo(ReCalcWidth(320), ReCalcHeight(30));
	dc.LineTo(ReCalcWidth(320), ReCalcHeight(210));
	dc.MoveTo(ReCalcWidth(360), ReCalcHeight(30));
	dc.LineTo(ReCalcWidth(360), ReCalcHeight(210));
	dc.MoveTo(ReCalcWidth(400), ReCalcHeight(30));
	dc.LineTo(ReCalcWidth(400), ReCalcHeight(210));
	dc.SetTextColor(RGB(255, 255, 255));
	dc.SetBkColor(RGB(0, 0, 0));
	rect.SetRect(201, 31, 319, 89);
	dc.DrawText(m_Player[0].strName, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(201, 151, 319, 209);
	dc.DrawText(m_Player[1].strName, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	rect.SetRect(201, 111, 223, 128);
	dc.DrawText("0", rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(225, 111, 247, 128);
	dc.DrawText("15", rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(249, 111, 271, 128);
	dc.DrawText("30", rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(273, 111, 295, 128);
	dc.DrawText("40", rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(297, 111, 319, 128);
	dc.DrawText("A", rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// 심판 메시지 표시
	if(m_TennisGame.strRefree != "")
		dc.DrawText(m_TennisGame.strRefree,
			CRect(ReCalcWidth(120), ReCalcHeight(200),
			ReCalcWidth(120) + 100, ReCalcHeight(200) + 50), DT_SINGLELINE);

	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	// 프로그램 시작시에만 실행할 코드는 이 함수 안에 넣고
	// 나머지는 ResetGame() 안에 넣는다.
	//CRect crt;
	//SetRect(&crt, 0, 0, WIDTH, HEIGHT);
	////AdjustWindowRect(&crt, GetStyle(), FALSE);
	//AdjustWindowRect(&crt, GetWindowLong(GetSafeHwnd(), GWL_STYLE), FALSE);

	// 선수의 키보드 설정도 따로 지정
	// 키보드 설정 메뉴를 구현할 때
	//   키 값이 겹치지 않도록 할 것
	// 영문자로 설정할 경우 반드시 대문자로 할 것
	m_Player[0].key_set.key_left = 'A';
	m_Player[0].key_set.key_right = 'D';
	m_Player[0].key_set.key_swing = VK_CONTROL;
	m_Player[0].key_set.key_up = 'W';
	m_Player[1].key_set.key_left = VK_LEFT;
	m_Player[1].key_set.key_right = VK_RIGHT;
	m_Player[1].key_set.key_swing = VK_SPACE;
	m_Player[1].key_set.key_up = VK_UP;

	// 선수명 초기화
	m_Player[0].strName = "Player1";
	m_Player[1].strName = "Player2";

	m_TennisGame.nHResolution = WIDTH;
	m_TennisGame.nVResolution = HEIGHT;

	// 비트맵 불러오기
	// 선수 비트맵만 필요, 공은 Ellipse()로 표현
	m_Player[0].bmpPlayer.LoadBitmap(IDB_PLAYER1);
	m_Player[1].bmpPlayer.LoadBitmap(IDB_PLAYER2);

	// 각 영역을 초기화한다.
	// 공과 닿는 것까지 체크할 수 있도록
	// 실제보다 상하좌우 1pt씩 넓게 잡는다(코트, 서비스박스 가로는 제외).
	m_TennisGame.court.SetRect(COURT_LEFT, LAND_TOP - 1, COURT_RIGHT, HEIGHT);
	m_TennisGame.land.SetRect(0, LAND_TOP - 1, WIDTH, HEIGHT);
	m_TennisGame.net.SetRect(NET_LEFT - 1, NET_TOP - 1, NET_RIGHT + 1, HEIGHT);
	m_TennisGame.service_box.SetRect(SERVICE_BOX_LEFT, LAND_TOP - 1, SERVICE_BOX_RIGHT, HEIGHT);

	// 게임 초기화(게임 재시작시에만 실행)
	ResetGame();

	return 0;
}


void CChildView::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	KillTimer(1);
}


void CChildView::OnGameStart()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//if(GameStatus != GAMEOVER) return;
	ResetGame();	// 게임 초기화
	GameStatus = WAITINGSERVE;
	//SetTimer(1, 100, NULL);
}


void CChildView::OnGamePause()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 게임 상태가 running일 때만 pause로 전환 가능
	if(GameStatus == WAITINGSERVE || GameStatus == DOINGSERVE || GameStatus == RUNNING) {
		GameStatus = PAUSE;
		KillTimer(1);
	}
	else if(GameStatus == PAUSE) {
		GameStatus = RUNNING;
		SetTimer(1, 100, NULL);
	}
}


void CChildView::OnGameEnd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 게임 일시정지
	if(MessageBox("게임을 끝내고 초기 화면으로 돌아갑니다.", "게임 중단", MB_OKCANCEL) == MB_OK) { 
		KillTimer(1);
		ResetGame();
		GameStatus = GAMEOVER;
	}
}


void CChildView::OnInitMenu(CMenu* pMenu)
{
	CWnd::OnInitMenu(pMenu);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 공중에 떠 있을 때는 움직일 수 없다. 라켓 스윙만 가능.
	if(m_Player[0].player_rect.bottom >= LAND_TOP &&
		(GameStatus == RUNNING || GameStatus == DOINGSERVE ||
		GameStatus == WAITINGSERVE && m_TennisGame.bCurrentServe)) {
		if( UpperCase(nChar) == m_Player[0].key_set.key_left && 
			GetAsyncKeyState(UpperCase(m_Player[0].key_set.key_right) & 0x8000) == 0x0000)
			PlayerMove(0, VK_LEFT);
		if(UpperCase(nChar) == m_Player[0].key_set.key_right &&
			GetAsyncKeyState(UpperCase(m_Player[0].key_set.key_left) & 0x8000) == 0x0000)
			PlayerMove(0, VK_RIGHT);
		if(UpperCase(nChar) == m_Player[0].key_set.key_up)
			PlayerMove(0, VK_UP);
	}
	if(	UpperCase(nChar) == m_Player[0].key_set.key_swing) {
		switch(GameStatus) {
		case DOINGSERVE:
			m_TennisGame.bLastTouch = false;
			// 서브 넣은 사람이 다시 치면 fault
			if(m_TennisGame.bCurrentServe == m_TennisGame.bLastTouch) {
				Fault(0);
				break;
			}
			else {
				GameStatus = RUNNING;
				m_TennisGame.in_area.SetRect(m_TennisGame.net.right, LAND_TOP,
					m_TennisGame.court.right, HEIGHT);
				m_TennisGame.bLastTouch = false;
				m_Player[0].bmpPlayer.DeleteObject();
				m_Player[0].bmpPlayer.LoadBitmap(IDB_PLAYER1_SWING);
				if(!m_Player[0].bSwinging)
					if(Swing(0)) {
						m_Ball.ptVelosity.x = BALL_INITIAL_VELOSITY_X + m_Player[0].ptVelosity.x;
						m_Ball.ptVelosity.y = BALL_INITIAL_VELOSITY_Y + m_Player[0].ptVelosity.y;
					}
				break;
			}
		case WAITINGSERVE:
			if(GameStatus != WAITINGSERVE) break;
			if(m_TennisGame.bCurrentServe) break;
			m_TennisGame.bLastTouch = false;
			GameStatus = DOINGSERVE;
			SetTimer(1, 100, NULL);
			// 위 분기점에 해당할 경우
			// 아래 코드는 무조건 실행하므로 break는 넣지 않는다.
		case RUNNING:
			m_TennisGame.bLastTouch = false;
			m_Player[0].bmpPlayer.DeleteObject();
			m_Player[0].bmpPlayer.LoadBitmap(IDB_PLAYER1_SWING);
			if(!m_Player[0].bSwinging)
				if(Swing(0)) {
					m_Ball.ptVelosity.x = BALL_INITIAL_VELOSITY_X + m_Player[0].ptVelosity.x;
					m_Ball.ptVelosity.y = BALL_INITIAL_VELOSITY_Y + m_Player[0].ptVelosity.y;
				}
			break;
		}
	}

	if(m_Player[1].player_rect.bottom >= LAND_TOP &&
		(GameStatus == RUNNING || GameStatus == DOINGSERVE ||
		GameStatus == WAITINGSERVE && !m_TennisGame.bCurrentServe)) {
		if( UpperCase(nChar) == m_Player[1].key_set.key_left && 
			GetAsyncKeyState(UpperCase(m_Player[1].key_set.key_right) & 0x8000) == 0x0000)
			PlayerMove(1, VK_LEFT);
		if(UpperCase(nChar) == m_Player[1].key_set.key_right &&
			GetAsyncKeyState(UpperCase(m_Player[1].key_set.key_left) & 0x8000) == 0x0000)
			PlayerMove(1, VK_RIGHT);
		if(UpperCase(nChar) == m_Player[1].key_set.key_up)
			PlayerMove(1, VK_UP);
	}
	if(	UpperCase(nChar) == m_Player[1].key_set.key_swing) {
		switch(GameStatus) {
		case DOINGSERVE:
			m_TennisGame.bLastTouch = true;
			// 서브 넣은 사람이 다시 치면 fault
			if(m_TennisGame.bCurrentServe == m_TennisGame.bLastTouch) {
				Fault(1);
				break;
			}
			else {
				GameStatus = RUNNING;
				m_TennisGame.in_area.SetRect(m_TennisGame.court.left, LAND_TOP,
					m_TennisGame.net.left, HEIGHT);
				m_TennisGame.bLastTouch = true;
				m_Player[1].bmpPlayer.DeleteObject();
				m_Player[1].bmpPlayer.LoadBitmap(IDB_PLAYER2_SWING);
				if(!m_Player[1].bSwinging)
					if(Swing(1)) {
						m_Ball.ptVelosity.x = -BALL_INITIAL_VELOSITY_X + m_Player[1].ptVelosity.x;
						m_Ball.ptVelosity.y = BALL_INITIAL_VELOSITY_Y + m_Player[1].ptVelosity.y;
					}
				break;
			}
		case WAITINGSERVE:
			if(GameStatus != WAITINGSERVE) break;
			if(!m_TennisGame.bCurrentServe) break;
			m_TennisGame.bLastTouch = true;
			GameStatus = DOINGSERVE;
			m_TennisGame.bLastTouch = true;
			SetTimer(1, 100, NULL);
			// 위 분기점에 해당할 경우
			// 아래 코드는 무조건 실행하므로 break는 넣지 않는다.
		case RUNNING:
			m_TennisGame.bLastTouch = true;
			m_Player[1].bmpPlayer.DeleteObject();
			m_Player[1].bmpPlayer.LoadBitmap(IDB_PLAYER2_SWING);
			if(!m_Player[1].bSwinging)
				if(Swing(1)) {
					m_Ball.ptVelosity.x = -BALL_INITIAL_VELOSITY_X + m_Player[1].ptVelosity.x;
					m_Ball.ptVelosity.y = BALL_INITIAL_VELOSITY_Y + m_Player[1].ptVelosity.y;
				}
			break;
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if(GameStatus == WAITINGSERVE || GameStatus == DOINGSERVE || GameStatus == RUNNING) {
		if(UpperCase(nChar) == m_Player[0].key_set.key_left) {
			m_Player[0].ptAcceleration.x = 0;
		}
		else if(UpperCase(nChar) == m_Player[0].key_set.key_right) {
			m_Player[0].ptAcceleration.x = 0;
		}
		else if(UpperCase(nChar) == m_Player[0].key_set.key_swing) {
			m_Player[0].bmpPlayer.DeleteObject();
			m_Player[0].bmpPlayer.LoadBitmap(IDB_PLAYER1);
			m_Player[0].bSwinging = false;
		}
		else if(UpperCase(nChar) == m_Player[1].key_set.key_left) {
			m_Player[1].ptAcceleration.x = 0;
		}
		else if(UpperCase(nChar) == m_Player[1].key_set.key_right) {
			m_Player[1].ptAcceleration.x = 0;
		}
		else if(UpperCase(nChar) == m_Player[1].key_set.key_swing) {
			m_Player[1].bmpPlayer.DeleteObject();
			m_Player[1].bmpPlayer.LoadBitmap(IDB_PLAYER2);
			m_Player[1].bSwinging = false;
		}
	}





	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


//void CChildView::OnSize(UINT nType, int cx, int cy)
//{
//	CWnd::OnSize(nType, cx, cy);
//
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//	m_TennisGame.nHResolution = cx;
//	m_TennisGame.nVResolution = cy;
//	ResetSituation();
//}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(GameStatus == WAITINGSERVE || GameStatus == DOINGSERVE || GameStatus == RUNNING)
		UpdateSituation();

	CWnd::OnTimer(nIDEvent);
}


void CChildView::OnUpdateGamePause(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(GameStatus == PAUSE);
}


//==============================================================================
// 추가 함수들
//------------------------------------------------------------------------------

void CChildView::AddGame(bool bPlayer)
{
	m_Player[bPlayer].nScore = 0;
	m_Player[bPlayer].nGame[m_TennisGame.nCurrentSet]++;
	// 획득 게임 수가 6이면 1세트 획득
	if(m_Player[bPlayer].nGame[m_TennisGame.nCurrentSet] == 6) {
		m_TennisGame.nCurrentSet++;
		// 3세트까지 끝냈으면 게임 종료
		if(m_TennisGame.nCurrentSet == MAX_SET) {
			m_TennisGame.strRefree = "Game Over!";
			GameStatus = GAMEOVER;
			Invalidate();
			Sleep(2000);
			m_TennisGame.strRefree.Format("%s won!", m_Player[bPlayer].strName);
			ResetSituation();
			return;
		}
	}

	GameStatus = WAITINGSERVE;
	ResetSituation();
}


void CChildView::AddScore(bool bPlayer)
{
	// 공 바운드 수를 초기 상태로 되돌리고
	// 공격이 유효한 선수의 득점
	m_Ball.nBound = 0;
	switch(m_Player[bPlayer].nScore) {
	case ZERO:
		m_Player[bPlayer].nScore = FIRST;
		break;
	case FIRST:
		m_Player[bPlayer].nScore = SECOND;
		break;
	case SECOND:
		m_Player[bPlayer].nScore = THIRD;
		// 상대 선수가 40점일 경우 듀스
		if(m_Player[!bPlayer].nScore == THIRD) {
			m_TennisGame.strRefree = "Deuce!";
			m_Player[bPlayer].nScore = ADVANTAGE;
			Invalidate();
			Sleep(2000);
			GameStatus = WAITINGSERVE;
			m_TennisGame.strRefree = "";
			ResetSituation();
		}
		break;
	case THIRD:
		// 상대 선수가 30점 이하이면, 득점한 선수가 1게임 획득
		if(m_Player[!bPlayer].nScore < THIRD)
			AddGame(bPlayer);
		// 듀스
		else {
			m_TennisGame.strRefree = "Deuce!";
			m_Player[bPlayer].nScore = ADVANTAGE;
			// 상대 선수가 Advantage였을 경우 40점으로 내린다.
			if(m_Player[!bPlayer].nScore == ADVANTAGE)
				m_Player[!bPlayer].nScore = THIRD;

			Invalidate();
			Sleep(2000);
			GameStatus = WAITINGSERVE;
			m_TennisGame.strRefree = "";
			ResetSituation();
		}
		break;
	case ADVANTAGE:
		// 1게임을 따내고, 6게임일 경우 1세트 획득
		AddGame(bPlayer);
		break;
	}
}


void CChildView::CheckBall(void)
{
	// 공이 땅에 떨어졌을 때
	if(m_Ball.ball_rect.bottom >= LAND_TOP - 1) {
		// 첫번째 바운드
		if(m_Ball.nBound == 0)
		{
			m_Ball.nBound++;

			// 공이 유효영역 안에 떨어지면 in
			if(m_Ball.ptCenter.x >= m_TennisGame.in_area.left ||
				m_Ball.ptCenter.x <= m_TennisGame.in_area.right) {
				return;
			}

			// 공이 유효영역 밖에 떨어지면 fault 또는 아웃
			else {
					// 서브중이었을 때에는 fault
					if(GameStatus == DOINGSERVE) {
						Fault(m_TennisGame.bLastTouch);
					}
					// 서브중이 아니었을 때는 out
					// 마지막으로 친 선수의 상대편이 득점
					else {
						AddScore(!m_TennisGame.bLastTouch);
						return;
					}
			}
		}

		// 두 번째 바운드일 경우 공격이 유효한 다음이므로
		// 마지막으로 친 선수가 득점
		else {
			AddScore(m_TennisGame.bLastTouch);
			return;
		}
	}

	// 공이 네트에 맞았을 경우
	// 마지막으로 친 선수의 상대편이 득점
	if(OverlapCircleArea(m_Ball.ball_rect, m_TennisGame.net)) {
		AddScore(!m_TennisGame.bLastTouch);
		return;
	}
}


void CChildView::Fault(bool player_idx)
{
	// 서브중일 경우
	if(GameStatus == DOINGSERVE) {
		// 공통사항
		m_Ball.nBound = 0;
		m_TennisGame.strRefree = "Fault!";
		Invalidate();
		Sleep(2000);
		m_TennisGame.strRefree = "";
		Invalidate();
		GameStatus = WAITINGSERVE;
		ResetSituation();

		// 첫번째 fault일 경우
		if(!m_TennisGame.bFault) {
			m_TennisGame.bFault = true;
			return;
		}
		// 두번째 fault일 경우
		else if(m_TennisGame.bFault) {
			m_TennisGame.bFault = false;
			AddScore(!m_TennisGame.bCurrentServe);
			return;
		}
		// 예외상황
		else {

		}
	}
}


void CChildView::IntegratePoint(CPoint addPoint, CPoint* currentPoint)
{
	currentPoint->x += addPoint.x;
	currentPoint->y += addPoint.y;
}


int CChildView::IsThereWall(int player_idx)
{
	// 반환값
	//   0: 장애물이 없을 경우
	//   1: 장애물이 왼쪽에 있을 경우
	//   2: 장애물이 오른쪽에 있을 경우
	if(	!player_idx && m_Player[0].ptSituation.x <= 0 || 
		player_idx && m_Player[1].ptSituation.x < NET_RIGHT)
		return 1;
	if(	!player_idx && m_Player[0].ptSituation.x >= NET_LEFT - PLAYER_WIDTH ||
		player_idx &&  m_Player[1].ptSituation.x > WIDTH - PLAYER_WIDTH)
		return 2;
	return 0;
}


bool CChildView::OverlapCircleArea(CRect ellipse, CRect rect)
{
	CPoint center;
	center.SetPoint(	Round((ellipse.left + ellipse.right) / 2.0),
						Round((ellipse.top + ellipse.bottom) / 2.0));
	int radius = Round((ellipse.right - ellipse.left) / 2.0);

	// ellipse의 중심이 rect 영역 안에 있으면 무조건 겹친다.
	if(	center.x >= rect.left && center.x <= rect.right &&
		center.y >= rect.top && center.y <= rect.bottom)
		return true;
	// 중심은 밖에 있지만 rect의 왼쪽과 오른쪽 사이에 있을 경우
	if(center.x >= rect.left && center.x <= rect.right) {
		// ellipse의 top이나 bottom이 rect의 안에 있으면 겹친다.
		if(	ellipse.top >= rect.top && ellipse.top <= rect.bottom ||
			ellipse.bottom >= rect.top && ellipse.bottom <= rect.bottom)
			return true;
	}
	// 중심은 밖에 이지만 rect의 위와 아래 사이에 있을 경우
	if(center.y >= rect.top && center.y <= rect.bottom) {
		// ellipse의 왼쪽이나 오른쪽이 rect의 안에 있으면 겹친다.
		if( ellipse.left >= rect.left && ellipse.right <= rect.right ||
			ellipse.right >= rect.left && ellipse.right <= rect.right)
			return true;
	}
	// rect가 만드는 십자 영역 밖에 ellipse가 위치할 경우
	// ellipse와 가장 가까운 rect의 꼭짓점과 ellipse의 중심의 거리가
	// ellipse의 반지름보다 짧으면 겹친다.
	if(ellipse.top < rect.top && ellipse.left < rect.left &&
		sqrt(pow((double)(rect.left - ellipse.left), 2) + pow((double)(rect.top - ellipse.top), 2)) < radius)
		return true;
	if(ellipse.top < rect.top && ellipse.right > rect.right &&
		sqrt(pow((double)(ellipse.right - ellipse.right), 2) + pow((double)(rect.top - ellipse.top), 2)) < radius)
		return true;
	if(ellipse.bottom > rect.bottom && ellipse.left < rect.left &&
		sqrt(pow((double)(rect.left - ellipse.left), 2) + pow((double)(ellipse.bottom - rect.bottom), 2)) < radius)
		return true;
	if(ellipse.bottom > rect.bottom && ellipse.right > rect.right &&
		sqrt(pow((double)(ellipse.right - rect.right), 2) + pow((double)(ellipse.bottom - rect.bottom), 2)) < radius)
		return true;

	return false;
}


bool CChildView::OverlapRectArea(CRect rect1, CRect rect2)
{
	// rect1의 오른쪽 변의 x값이 rect2의 왼쪽 변의 x값과 같거나 왼쪽일 때
	if(rect1.right <= rect2.left) return false;

	// rect2의 오른쪽 변의 x값이 rect1의 왼쪽 변의 x값과 같거나 왼쪽일 때
	if(rect2.right <= rect1.left) return false;

	// rect1의 아래쪽 변의 y값이 rect2의 위쪽 변의 y값과 같거나 위일 때
	if(rect1.bottom <= rect2.top) return false;

	// rect2의 아래쪽 변의 y값이 rect1의 위쪽 변의 y값과 같거나 위일 때
	if(rect2.bottom <= rect1.top) return false;

	// 위의 경우에 모두 해당 안되면 겹침
	return true;
}


void CChildView::PlayerMove(int player_idx, UINT command)
{
	// 장애물에 부딪혔을 때는 y 방향만 움직인다.(낙하)
	if(m_Player[player_idx].ptSituation.y < LAND_TOP ||
		!(GameStatus == WAITINGSERVE || GameStatus == DOINGSERVE || GameStatus == RUNNING))
		return;
	switch(command) {
	case VK_LEFT:
			m_Player[player_idx].ptAcceleration.x = -PLAYER_MOVE_ACCELERATION;
		break;
	case VK_RIGHT:
			m_Player[player_idx].ptAcceleration.x = PLAYER_MOVE_ACCELERATION;
		break;
	case VK_UP:
		m_Player[player_idx].ptVelosity.y = PLAYER_JUMP_VELOSITY;
		break;
	}
}


int CChildView::ReCalcWidth(int width)
{
	return Round(width * m_TennisGame.nHResolution / (double)WIDTH);
}


int CChildView::ReCalcHeight(int height)
{
	return Round(height * m_TennisGame.nVResolution / (double)HEIGHT);
}


void CChildView::ResetGame(void)
{
	ResetScore();		// 스코어 초기화
	ResetSituation();	// 선수, 공 위치값 초기화

	// 게임 속성값 초기화
	// 위치값의 단위는 기본 크기를 기준으로 한다.
	// 픽셀값 재계산은 OnPaint 함수에서 실행
	m_TennisGame.nCurrentSet = 1;
	m_TennisGame.nCurrentGame = 1;
	m_TennisGame.bCurrentServe = false;
	m_TennisGame.bLastTouch = false;
	m_TennisGame.bFault = false;
	m_TennisGame.strRefree = "";

	// 공 속성값 초기화
	m_Ball.nBound = 0;

	// 유효범위 초기화
	// 서브권자가 P1일 경우 P2의 서비스박스,
	// 서브권자가 P2일 경우 P1의 서비스박스
	//m_TennisGame.in_area.CopyRect(m_TennisGame.service_box);
	m_TennisGame.in_area.SetRect(
		m_TennisGame.bCurrentServe ? m_TennisGame.service_box.left : m_TennisGame.net.right,
		LAND_TOP,
		m_TennisGame.bCurrentServe ? m_TennisGame.net.left : m_TennisGame.service_box.right,
		HEIGHT);

	// 자동으로 게임 시작(서브대기)
	GameStatus = WAITINGSERVE;
	SetTimer(1, 100, NULL);
}


void CChildView::ResetScore(void)
{
	m_Player[0].nScore = 0;
	m_Player[1].nScore = 0;
	for(int i = 0; i < MAX_SET; i++) {
		m_Player[0].nGame[i] = 0;
		m_Player[1].nGame[i] = 0;
	}
}


void CChildView::ResetSituation(void)
{
	// 이 코드는 프로그램 시작 또는 게임 리셋 때만 실행된다!
	// 윈도우 크기 변경에 따른 픽셀값 재조정은 OnPaint 함수에서 실행
	// 원래의 코드(윈도우 크기조절기능 적용 전)
	for(int i = 0; i < 2; i++) {
		m_Player[i].bSwinging = false;
		m_Player[i].ptAcceleration.x = 0;
		m_Player[i].ptAcceleration.y = GRAVITY_ACCELERATION;
		m_Player[i].ptSituation.y = LAND_TOP;
		m_Player[i].ptVelosity.x = 0;
		m_Player[i].ptVelosity.y = 0;
		m_Player[i].player_rect.bottom = m_Player[i].ptSituation.y;
		m_Player[i].player_rect.top = m_Player[i].ptSituation.y - 48;
	}
	// 선수의 x 위치값은 따로 지정
	m_Player[0].ptSituation.x = PLAYER1_SERVE_POS;
	m_Player[1].ptSituation.x = PLAYER2_SERVE_POS;
	m_Player[0].player_rect.left = m_Player[0].ptSituation.x;
	m_Player[0].player_rect.right = m_Player[0].ptSituation.x + 48;
	m_Player[1].player_rect.left = m_Player[1].ptSituation.x;
	m_Player[1].player_rect.right = m_Player[1].ptSituation.x + 48;

	// 공 속성값 초기화
	m_Ball.ptAcceleration.x = 0;
	m_Ball.ptAcceleration.y = GRAVITY_ACCELERATION;
	m_Ball.ptCenter.x = PLAYER1_SERVE_POS + 24;
	m_Ball.ptCenter.y = LAND_TOP - 40;
	m_Ball.ptVelosity.x = 0;
	m_Ball.ptVelosity.y = 0;

	Invalidate();
}


int CChildView::Round(double d)
{
	if((int)(d * 10) % 10 < 5)
		return (int)d;
	
	return (int)d + 1;
}


bool CChildView::Swing(int player_idx)
{
	if(m_Player[player_idx].bSwinging)
		return false;
	m_Player[player_idx].bSwinging = true;
	if(OverlapCircleArea(m_Ball.ball_rect, m_Player[player_idx].player_rect))
		return true;
	return false;
}


void CChildView::UpdateSituation(void)
{
	
	// 수정 전 : 윈도우 크기조절기능 적용 전 코드
	// 속도 재계산
	IntegratePoint(m_Player[0].ptAcceleration, &m_Player[0].ptVelosity);
	IntegratePoint(m_Player[1].ptAcceleration, &m_Player[1].ptVelosity);
	IntegratePoint(m_Ball.ptAcceleration, &m_Ball.ptVelosity);

	// 선수의 속도는 최고 속도를 넘지 않도록
	if(m_Player[0].ptVelosity.x > PLAYER_MAX_SPEED)
		m_Player[0].ptVelosity.x = PLAYER_MAX_SPEED;
	else if(m_Player[0].ptVelosity.x < -PLAYER_MAX_SPEED)
		m_Player[0].ptVelosity.x = -PLAYER_MAX_SPEED;
	if(m_Player[1].ptVelosity.x > PLAYER_MAX_SPEED)
		m_Player[1].ptVelosity.x = PLAYER_MAX_SPEED;
	else if(m_Player[1].ptVelosity.x < -PLAYER_MAX_SPEED)
		m_Player[1].ptVelosity.x = -PLAYER_MAX_SPEED;


	// 땅에 떨어지면 속도가 줄어듦
	if(m_Player[0].ptSituation.y >= LAND_TOP)
		if(GetKeyState('D') >= 0 && GetKeyState('d') >= 0 && m_Player[0].ptVelosity.x > 0)
			m_Player[0].ptVelosity.x -= FRICTIONAL_FORCE;
		else if(GetKeyState('A') >= 0 && GetKeyState('a') >= 0 && m_Player[0].ptVelosity.x < 0)
			m_Player[0].ptVelosity.x += FRICTIONAL_FORCE;
	if(m_Player[1].ptSituation.y >= LAND_TOP)
		if(GetKeyState(VK_RIGHT) >= 0 && m_Player[1].ptVelosity.x > 0)
			m_Player[1].ptVelosity.x -= FRICTIONAL_FORCE;
		else if(GetKeyState(VK_LEFT) >= 0 && m_Player[1].ptVelosity.x < 0)
			m_Player[1].ptVelosity.x += FRICTIONAL_FORCE;
	if(m_Ball.ptCenter.y >= LAND_TOP - BALL_RADIUS)
		if(m_Ball.ptVelosity.x > 0)
			m_Ball.ptVelosity.x -= FRICTIONAL_FORCE;
		else if(m_Ball.ptVelosity.x < 0)
			m_Ball.ptVelosity.x += FRICTIONAL_FORCE;

	// 땅에 다 떨어졌으면 y 방향의 속도를 0으로
	// 점프키를 눌렀을 때는 제외
	if(m_Player[0].ptSituation.y >= LAND_TOP && GetKeyState('W') >= 0 && GetKeyState('w') >= 0 && !IsThereWall(0))
			m_Player[0].ptVelosity.y = 0;
	if(m_Player[1].ptSituation.y >= LAND_TOP && GetKeyState(VK_UP) >= 0 && !IsThereWall(1))
			m_Player[1].ptVelosity.y = 0;
	// 공은 땅에 떨어지면 바운드되어야 한다.
	if(m_Ball.ptCenter.y >= LAND_TOP - BALL_RADIUS) {
		m_Ball.ptVelosity.y = Round(-m_Ball.ptVelosity.y * 0.555118);
		if(m_Ball.ptVelosity.y > 0)
			m_Ball.nBound++;
	}

	// 위치 재계산
	// 플레이 영역을 벗어났을 때는 x 방향의 위치 변화를 무효로 한다.
	// 공의 경우는 바로 시합을 정지하고 점수를 계산해야 하므로 위치 재계산은 필요없음
	IntegratePoint(m_Player[0].ptVelosity, &m_Player[0].ptSituation);
	IntegratePoint(m_Player[1].ptVelosity, &m_Player[1].ptSituation);
	IntegratePoint(m_Ball.ptVelosity, &m_Ball.ptCenter);
	if(IsThereWall(0) == 1)
		m_Player[0].ptSituation.x = 0;
	else if(IsThereWall(0) == 2)
		m_Player[0].ptSituation.x = NET_LEFT - PLAYER_WIDTH;
	if(IsThereWall(1) == 1)
		m_Player[1].ptSituation.x = NET_RIGHT;
	else if(IsThereWall(1) == 2)
		m_Player[1].ptSituation.x = WIDTH - PLAYER_WIDTH;
	

	// 땅밑으로 꺼지지 않도록 위치 조정
	if(m_Player[0].ptSituation.y > LAND_TOP)
		m_Player[0].ptSituation.y = LAND_TOP;
	if(m_Player[1].ptSituation.y > LAND_TOP)
		m_Player[1].ptSituation.y = LAND_TOP;
	if(m_Ball.ptCenter.y > LAND_TOP - BALL_RADIUS)
		m_Ball.ptCenter.y = LAND_TOP - BALL_RADIUS;

	// 서브 대기 상태일 때는 공의 위치가 서브할 선수의 위치를 따라간다.
	if(GameStatus == WAITINGSERVE) {
		m_Ball.ptCenter.x = m_Player[(int)m_TennisGame.bCurrentServe].ptSituation.x + 25;
		m_Ball.ptCenter.y = m_Player[(int)m_TennisGame.bCurrentServe].ptSituation.y - 40;
	}

	// 선수 범위값 재계산
	for(int i = 0; i < 2; i++) {
		m_Player[i].player_rect.left = m_Player[i].ptSituation.x;
		m_Player[i].player_rect.right = m_Player[i].ptSituation.x + 48;
		m_Player[i].player_rect.bottom = m_Player[i].ptSituation.y;
		m_Player[i].player_rect.top = m_Player[i].ptSituation.y - 48;
	}

	// 공 범위값 재계산
	m_Ball.ball_rect.left = m_Ball.ptCenter.x - BALL_RADIUS;
	m_Ball.ball_rect.right = m_Ball.ptCenter.x + BALL_RADIUS;
	m_Ball.ball_rect.top = m_Ball.ptCenter.y - BALL_RADIUS;
	m_Ball.ball_rect.bottom = m_Ball.ptCenter.y + BALL_RADIUS;

	Invalidate();

	// 득점 상황인지 검사
	CheckBall();
}


UINT CChildView::UpperCase(UINT nChar)
{
	if(nChar >= 'a' && nChar <= 'z')
		return nChar - 32;
	return nChar;
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	return CWnd::OnEraseBkgnd(pDC);
}



