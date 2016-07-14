
// ChildView.h : CChildView 클래스의 인터페이스
//


#pragma once


// CChildView 창


//==============================================================================
// 상수값들
//------------------------------------------------------------------------------
#define WIDTH 640				// 화면의 너비
#define HEIGHT 480				// 화면의 높이
#define GRAVITY_ACCELERATION 2	// 중력가속도=2pt/1frame^2
#define BALL_MAX_SPEED 16		// 
#define BALL_INITIAL_VELOSITY_X 20	// 공을 치는 순간의 속도(x방향)
#define BALL_INITIAL_VELOSITY_Y -16	// 공을 치는 순간의 속도(y방향)
#define BALL_RADIUS 2			// 공의 반지름
#define FRICTIONAL_FORCE 1		// 마찰력

// 땅 영역
#define LAND_LEFT 0
#define LAND_RIGHT 640
#define LAND_TOP 460
//#define LAND_BOTTOM 480
// 네트 영역
#define NET_LEFT 318
#define NET_RIGHT 321
#define NET_TOP 440
//#define NET_BOTTOM 480
// 코트 영역
#define COURT_LEFT 99
#define COURT_RIGHT 541
//#define COURT_TOP 460
//#define COURT_BOTTOM 480

// 서비스라인 영역
#define SERVICE_BOX_LEFT 192
#define SERVICE_BOX_RIGHT 448

// 플레이어 관련 값들
#define PLAYER_MOVE_ACCELERATION 3	// 선수가 x방향으로 이동할 때의 가속도
#define PLAYER_JUMP_VELOSITY -9		// 점프시 초기속도
#define PLAYER_MAX_SPEED 9			// 선수의 x방향의 최대속도
#define PLAYER_WIDTH 48				// 플레이어 영역 너비
#define PLAYER_HEIGHT 48			// 플레이어 영역 높이

// 플레이어 관련 위치값
#define PLAYER1_SERVE_POS	21
#define PLAYER2_SERVE_POS	562

#define MAX_SET 2				// 한 선수가 2세트를 먼저 획득하면 시합 종료
#define MAX_GAME 6				// 한 선수가 6게임을 먼저 획득하면 1세트 끝*
#define LAST_SCORE 40			// 한 선수가 40점을 넘기면 1게임 끝*
// *원바이투일 경우는 예외
//==============================================================================

//==============================================================================
// 키 설정 구조체
//------------------------------------------------------------------------------
struct KeySetting {
	UINT key_up;
	UINT key_left;
	UINT key_right;
	UINT key_swing;
};
//==============================================================================

//==============================================================================
// 선수 구조체
//------------------------------------------------------------------------------
struct Player {
	// 위치값은 땅과의 거리를 계산하는 수고를 덜기 위해
	// 왼쪽 아래 지점을 기준으로 한다.
	CRect player_rect;		// 플레이어가 차지하는 부분
	CPoint ptSituation;		// 위치값
	CPoint ptVelosity;		// 속도값
	CPoint ptAcceleration;	// 가속도값
	CBitmap bmpPlayer;		// 선수 비트맵
	bool bSwinging;			// 스윙 중인지를 체크
	KeySetting key_set;		// 키보드 세팅값
	int nScore[MAX_SET][MAX_GAME];	// 세트/게임별 점수
	int nFootFault;			// Foot Fault 횟수
};
//==============================================================================

//==============================================================================
// 공 구조체
//------------------------------------------------------------------------------
struct Ball {
	// 위치값은 땅과의 거리를 계산하는 수고를 덜기 위해
	// 왼쪽 아래 지점을 기준으로 한다.
	CPoint ptCenter;		// 위치값
	CPoint ptVelosity;		// 속도값
	CPoint ptAcceleration;	// 가속도값
	CRect ball_rect;		// 공의 영역
	int nBound;			// 공의 바운드 횟수(0~2)
};
//==============================================================================

//==============================================================================
// 게임 구조체
// 
//------------------------------------------------------------------------------
struct TennisGame {
	bool bCurrentServe;	// 현재 서브(false:p1, true:p2)
	bool bLastTouch;	// 마지막으로 공을 친 사람(false:p1, true:p2)
	int nCurrentGame;	// 현재 게임
	int nCurrentSet;	// 현재 세트
	int nHResolution;	// 가로 픽셀값
	int nVResolution;	// 세로 픽셀값
	// 아래 영역들은 공이 닿으면 안 되는 영역
	// 초기화할 때, 실제 값보다 좌우상하 1pt씩 넓게 잡는다.
	CRect net;			// 네트 영역
	CRect land;			// 땅 영역
	CRect court;		// 코트 영역
	CRect service_box;		// 서비스 박스 영역
};
//==============================================================================



class CChildView : public CWnd
{
// 생성입니다.
public:
	CChildView();

// 특성입니다.
public:
	Player m_Player[2];			// 선수 구조체
	Ball m_Ball;				// 공 구조체
	TennisGame m_TennisGame;	// 게임 구조체

// 작업입니다.
public:

// 재정의입니다.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGameStart();
	afx_msg void OnGamePause();
	afx_msg void OnGameEnd();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateGamePause(CCmdUI *pCmdUI);
	
	// 득점 상황이면 득점시키고 서브 대기 상태로 전환
	void CheckBall(void);

	// 주어진 좌표값(pt)의 x 또는 y값을 현재 해상도에 맞는 픽셀값으로 변환
	// axis값 : false=x, true=y
	CPoint FitToResolution(CPoint pt);

	// 현재 좌표값에 새 좌표값을 가산
	//   1. 주어진 가속도로 속도를 재계산
	//   2. 주어진 속도로 위치를 재계산
	void IntegratePoint(CPoint addPoint, CPoint* currentPoint);

	//// 주어진 문자가 영문자이고
	//// 대소문자 구분 없이 같은 글자인지 검사
	//bool IsSameAlphabet(char c1, char c2);
	
	// 선수가 플레이 영역 내에 있는지 검사
	int IsThereWall(int player_idx);

	// 원이 주어진 영역(직사각형)과 겹치는지 체크
	bool OverlapCircleArea(CRect ellipse, CRect rect);

	// 선수를 움직일 때 실행
	// 해당 선수와 키를 받아 가속도값을 처리
	void PlayerMove(int player_idx, UINT command);

	//// x축 또는 y축 좌표값의 거리와 해당 축의 최대거리를 입력받아
	//// 픽셀값에 맞는 거리를 재계산
	//int ReCalcDistance(int dt, int maxDt);

	// 가로 길이값 재계산
	int ReCalcWidth(int width);

	// 세로 길이값 재계산
	int ReCalcHeight(int height);

	// 게임 초기화(프로그램 최초 시작 또는 게임 재시작시)
	void ResetGame(void);

	// 프로그램 초기화 또는 득점했을 경우 등
	// 선수와 공의 가속도, 속도, 위치값을 초기화
	void ResetSituation(void);

	// 스코어 초기화
	void ResetScore(void);	
	
	// 반올림
	int Round(double d);

	// 공을 칠 때 실행
	// 공을 맞추면 true, 못 맞추면 false를 반환
	bool Swing(int player_idx);

	// 선수와 공의 속도와 위치를 재계산
	void UpdateSituation(void);

	// 문자 데이터를 받아서 영문 소문자이면 대문자로 변환하고
	// 아니면 그대로 반환한다.
	UINT UpperCase(UINT nChar);
};

