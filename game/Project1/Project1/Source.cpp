#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <cmath>
#include <ctime>
#include <windows.h>
#include <iostream>
using namespace std;


enum class Cmd
{
	cmd_down,
	cmd_rotate,
	cmd_left,
	cmd_right,
	cmd_boom, //use boom
	cmd_space
};

class square
{
public:
	int dir[4][4][2]; //定義方塊種類 //美個方塊4種形狀 4個點 2個坐標系
	int boom_dir[1][2] = { 0,1 };
	COLORREF sqcolors[8] = { RGB(255,0,0),RGB(0,0,255),RGB(0,100,0),RGB(139,69,19),RGB(255,0,255),RGB(255,165,0),RGB(255,255,0),RGB(128,0,128) }; //方塊顏色
};

class drawTetris : protected square
{
public:
	drawTetris() {
		initgraphic(); //初始化介面
	}
	//定義方塊座標

	square sqs[7] = {
	 { 0, -2, 0, -1, 0, 0, 1, 0, 0, -1, 1, -1, 2, -1, 0, 0, 0, -2, 1, -2, 1, -1, 1, 0, 0, 0, 1, 0, 2, 0, 2, -1 },
	 { 0, 0, 1, 0, 1, -1, 1, -2, 0, -1, 0, 0, 1, 0, 2, 0, 0, -2, 0, -1, 0, 0, 1, -2, 0, -1, 1, -1, 2, -1, 2, 0 },
	 { 0, -1, 0, 0, 1, -1, 1, 0, 0, -1, 0, 0, 1, -1, 1, 0, 0, -1, 0, 0, 1, -1, 1, 0, 0, -1, 0, 0, 1, -1, 1, 0 },
	  { 0, 0, 1, -1, 1, 0, 2, 0, 1, -2, 1, -1, 1, 0, 2, -1, 0, -1, 1, -1, 1, 0, 2, -1, 0, -1, 1, -2, 1, -1, 1, 0 },
	{ 0, -3, 0, -2, 0, -1, 0, 0, 0, -3, 1, -3, 2, -3, 3, -3, 0, -3, 0, -2, 0, -1, 0, 0, 0, -3, 1, -3, 2, -3, 3, -3 },
	{ 0, -1, 1, -1, 1, 0, 2, 0, 0, -1, 0, 0, 1, -2, 1, -1, 0, -1, 1, -1, 1, 0, 2, 0, 0, -1, 0, 0, 1, -2, 1, -1 },
	 { 0, 0, 1, -1, 1, 0, 2, -1, 1, -2, 1, -1, 2, -1, 2, 0, 0, 0, 1, -1, 1, 0, 2, -1, 1, -2, 1, -1, 2, -1, 2, 0 }
	};

	const double pi = acos(-1);
	COLORREF tmp_line_color;
	LINESTYLE tmp_line_style;
	COLORREF tmp_text_color;
	LOGFONT tmp_text_style;
	COLORREF tmp_fill_color;
	int map[10][20]; //地圖寬高
	int map_tmp[10][20];
	COLORREF map_c[10][20]; //地圖顏色
	COLORREF map_c_tmp[10][20]; //佔存地圖顏色

	DWORD time_now;
	int flag_continue; //是否繼續生成方塊
	int flag_gameover; //是否結束遊戲
	int s_idx; //選到的方塊形狀
	int d_idx; //選到的方塊形狀方向
	int c_idx; //選到的方塊顏色
	int ns_idx; //下一個方塊形狀
	int nd_idx; //下一個方塊形狀方向
	int nc_idx; //下一個方塊顏色
	int now_mapx; //目前的x位置
	int now_mapy; //目前的y位置
	int score; //分數
	int speed = 500; //速度
	int maxscore; //最高分數
	int level = 0; //速度等級
	int y = 100; //避免重複加速及增加


	int boom = 0;
	bool dtboom = false;
	bool dtec = false;
	wchar_t maxscore_tps[30];
	wchar_t boom_tps[30];
	wchar_t lelvel_tps[30];
	wchar_t score_tps[30];

	//繪圖
	void initgraphic()
	{
		initgraph(600, 720); //初始化界面
		HWND hwnd = GetHWnd();
		SetWindowText(hwnd, L"Tetris"); //設定視窗標題
		SetWindowPos(hwnd, HWND_TOP, 700, 20, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		setbkmode(TRANSPARENT); //設定字背景透明

		srand(time(NULL)); //設定隨機亂數去選方塊及顏色
	}



	void nextround() //如果有進入下一回合全部歸零
	{
		memset(map, 0, sizeof(map));
		memset(map, 0, sizeof(map_c));
		if (score > maxscore) //最高分數
		{
			maxscore = score;
		}
		time_now = GetTickCount(); //gettickcout為計算時間來控制方塊下降速度
		ns_idx = rand() % 7;
		nd_idx = rand() % 4;
		nc_idx = rand() % 8; //從sqcolors 中隨機選出顏色
		flag_gameover = 0;
		level = 0;
		score = 0; //設定分數為0
		dtboom = false;
		boom = 0;
		dtec = false;
		y = 100;
		speed = 500;
	}
	void nowround_square() {
		now_mapx = 5;
		now_mapy = -1;
		flag_continue = 0;
		if (dtec == true) //不重複炸彈
		{
			dtec = false;
		}

		if (dtboom == true)
		{
			dtboom = false;
			dtec = true;
		}

		//取下一個方塊

		s_idx = ns_idx;
		d_idx = nd_idx;
		c_idx = nc_idx;
		ns_idx = rand() % 7;
		nd_idx = rand() % 4;
		nc_idx = rand() % 8;


		if (score >= y) //每100分加一顆炸彈
		{
			boom++;
			y += 100;
			speed -= 15;
			level++;
		}

	}
	void map_gamebackground() {
		COLORREF tmp = getlinecolor();
		for (int i = 0; i < 721; ++i)
		{
			setlinecolor(RGB(0, 0, 0)); //設定背景色
			line(0, 720 - i, 360, 720 - i);
		}
		setlinecolor(tmp);
	}
	void drawItem(int x, int y, COLORREF c)
	{
		tmp_fill_color = getfillcolor();

		const int r = 6;
		const int p = 2;

		int up_l_x = x + p + r;
		int up_r_x = x + 36 - p - r;
		int up___y = y + p;

		int down_l_x = x + p + r;
		int down_r_x = x + 36 - p - r;
		int down___y = y + 36 - p;

		int left_u_y = y + p + r;
		int left_d_y = y + 36 - p - r;
		int left___x = x + p;

		int right_u_y = y + p + r;
		int right_d_y = y + 36 - p - r;
		int right___x = x + 36 - p;

		line(up_l_x, up___y, up_r_x, up___y);
		line(down_l_x, down___y, down_r_x, down___y);
		line(left___x, left_u_y, left___x, left_d_y);
		line(right___x, right_u_y, right___x, right_d_y);
		arc(x + p, y + p, x + p + 2 * r, y + p + 2 * r, pi / 2, pi);
		arc(x + 36 - p, y + p, x + 36 - p - 2 * r, y + p + 2 * r, 0, pi / 2);
		arc(x + p, y + 36 - p, x + p + 2 * r, y + 36 - p - 2 * r, -pi, -pi / 2);
		arc(x + 36 - p, y + 36 - p, x + 36 - p - 2 * r, y + 36 - p - 2 * r, -pi / 2, 0);

		setfillcolor(c);
		floodfill(x + p + r + 1, y + p + r + 1, WHITE);

		setfillcolor(tmp_fill_color);
	}
	void drawBoom(int x, int y)
	{
		tmp_fill_color = getfillcolor();
		setfillcolor(WHITE);
		circle(x + 17, y, 18);

		setfillcolor(WHITE);
		floodfill(x, y, WHITE);

		setfillcolor(tmp_fill_color);
	}
	void DrawSquareInside()
	{
		//畫出方塊
		if (dtec == true)
		{
			drawBoom((now_mapx + boom_dir[0][0]) * 36, (now_mapy + boom_dir[0][1]) * 36);
		}
		else
		{
			drawItem((now_mapx + sqs[s_idx].dir[d_idx][0][0]) * 36, (now_mapy + sqs[s_idx].dir[d_idx][0][1]) * 36, sqcolors[c_idx]);
			drawItem((now_mapx + sqs[s_idx].dir[d_idx][1][0]) * 36, (now_mapy + sqs[s_idx].dir[d_idx][1][1]) * 36, sqcolors[c_idx]);
			drawItem((now_mapx + sqs[s_idx].dir[d_idx][2][0]) * 36, (now_mapy + sqs[s_idx].dir[d_idx][2][1]) * 36, sqcolors[c_idx]);
			drawItem((now_mapx + sqs[s_idx].dir[d_idx][3][0]) * 36, (now_mapy + sqs[s_idx].dir[d_idx][3][1]) * 36, sqcolors[c_idx]);
		}
	}
	void DrawSquareNext()
	{
		int tmp_x = 408;
		int tmp_y = 164;
		COLORREF c = sqcolors[nc_idx];
		if (dtboom == true)
		{
			IMAGE boompic;
			loadimage(&boompic, L"boom.jpeg", 50, 50);
			putimage(tmp_x + 50, tmp_y - 50, &boompic);
		}
		else
		{
			drawItem(tmp_x + sqs[ns_idx].dir[nd_idx][0][0] * 36, tmp_y + sqs[ns_idx].dir[nd_idx][0][1] * 36, sqcolors[nc_idx]);
			drawItem(tmp_x + sqs[ns_idx].dir[nd_idx][1][0] * 36, tmp_y + sqs[ns_idx].dir[nd_idx][1][1] * 36, sqcolors[nc_idx]);
			drawItem(tmp_x + sqs[ns_idx].dir[nd_idx][2][0] * 36, tmp_y + sqs[ns_idx].dir[nd_idx][2][1] * 36, sqcolors[nc_idx]);
			drawItem(tmp_x + sqs[ns_idx].dir[nd_idx][3][0] * 36, tmp_y + sqs[ns_idx].dir[nd_idx][3][1] * 36, sqcolors[nc_idx]);
		}
	}
	void map_sidebackground() {

		getlinestyle(&tmp_line_style);
		tmp_text_color = getlinecolor();
		gettextstyle(&tmp_text_style);
		tmp_line_color = getlinecolor();

		for (int i = 0; i < 721; ++i)
		{
			setlinecolor(RGB(135, 206, 235));
			line(361, i, 600, i);
		}
		settextcolor(RGB(0, 0, 0)); //設定字體顏色
		settextstyle(30, 0, L"標楷體"); //設定字體樣式
		outtextxy(408, 20, L"下一個方塊");
		setlinecolor(BLACK);
		roundrect(400, 52, 560, 205, 25, 25);
		setlinecolor(WHITE);
		DrawSquareNext();

		IMAGE boompic;
		loadimage(&boompic, L"boom.jpeg", 50, 50);
		putimage(400, 390, &boompic);
		swprintf(boom_tps, 30, L"x  %d ", boom);
		outtextxy(490, 400, boom_tps);
		swprintf(score_tps, 15, L"得分： %d", score);
		outtextxy(400, 240, score_tps);
		swprintf(maxscore_tps, 30, L"最高分數：%d", maxscore);
		outtextxy(400, 290, maxscore_tps);
		swprintf(lelvel_tps, 30, L"LEVEL： %d", level);
		outtextxy(405, 340, lelvel_tps);

		setlinecolor(tmp_line_color);
		setlinestyle(&tmp_line_style);
		settextcolor(tmp_text_color);
		settextstyle(&tmp_text_style);
	}
	void drawMap()
	{
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				if (map[i][j])
				{
					drawItem(i * 36, j * 36, map_c[i][j]); //drawmap squard
				}
			}
		}
	}


	~drawTetris()
	{
		closegraph();
	}

};

class beginTetris : public drawTetris
{
public:
	Cmd getCmd()
	{
		while (true)
		{
			DWORD time_tmp = GetTickCount();
			if (time_tmp - time_now >= speed)
			{
				time_now = time_tmp;
				return Cmd::cmd_down;
			}


			if (_kbhit())
			{
				switch (_getch())
				{
					//方向鍵ascii的值
				case 72: //上
					return Cmd::cmd_rotate;
				case 75: //左
					return Cmd::cmd_left;
				case 77: //右
					return Cmd::cmd_right;
				case 80: //下
					return Cmd::cmd_down;
				case 69: //e
					return Cmd::cmd_boom;
				case 101: //e
					return Cmd::cmd_boom;
				case 32: //space
					return Cmd::cmd_space;
				}
			}
			Sleep(20);
		}
	}
	bool check(int map_x, int map_y, int d_idx)
	{
		//4x4範圍
		int x[4];
		int y[4];
		int bmx[1];
		int bmy[1];
		if (dtec == true)
		{
			bmx[0] = map_x + boom_dir[0][0];
			bmy[0] = map_y + boom_dir[0][1];
			for (int i = 0; i < 1; i++)
			{
				if (bmx[i] < 0 || bmx[i] > 9 || bmy[i] > 19) //判斷是否超過左右邊 及 碰到上面
					return false;
				if (bmy[i] < 0)
					continue;
				if (map[bmx[i]][bmy[i]]) //判斷是否存在方塊
					return false;
			}
		}
		else
		{
			for (int i = 0; i < 4; ++i) //放入x y 內
			{
				x[i] = map_x + sqs[s_idx].dir[d_idx][i][0];
				y[i] = map_y + sqs[s_idx].dir[d_idx][i][1];
			}

			for (int i = 0; i < 4; ++i)
			{
				if (x[i] < 0 || x[i] > 9 || y[i] > 19) //判斷是否超過左右邊 及 碰到上面
					return false;
				if (y[i] < 0) //判斷是否觸碰下框
					continue;
				if (map[x[i]][y[i]]) //判斷是否存在方塊
					return false;
			}
			return true;
		}

	}
	void moverotate()
	{
		for (int i = 1; i <= 3; ++i)
			if (check(now_mapx, now_mapy, (d_idx + i) % 4))
			{
				d_idx = (d_idx + i) % 4;
				break;
			}
	}
	void moveright()
	{
		if (check(now_mapx + 1, now_mapy, d_idx))
			++now_mapx; //座標右移
	}
	void moveleft()
	{
		if (check(now_mapx - 1, now_mapy, d_idx))
			--now_mapx; //座標左移
	}
	void quickmovedown()
	{
		while (check(now_mapx, now_mapy + 1, d_idx) == true)
		{
			++now_mapy;
		}
		flag_continue = 1;
		recordmap();
		squareclear();
		if (checkover())
			flag_gameover = 1;
	}
	void movedown()
	{
		if (check(now_mapx, now_mapy + 1, d_idx))
		{
			++now_mapy; //座標下移
			return;
		}
		flag_continue = 1;
		recordmap();
		squareclear();
		if (checkover())
			flag_gameover = 1;
	}

	void recordmap()
	{
		//紀錄方塊位置及顏色
		int x[4];
		int y[4];
		int bmx[1];
		int bmy[1];
		if (dtec == true)
		{
			for (int i = 0; i < 1; ++i)
			{
				bmx[i] = now_mapx + boom_dir[0][0];
				bmy[i] = now_mapy + boom_dir[0][1];
			}
			for (int i = 0; i < 1; ++i)
			{
				if (bmy[i] >= 0)
				{
					map[bmx[i]][bmy[i]] = 2;
					map_c[bmx[i]][bmy[i]] = WHITE;
				}
			}
		}
		else
		{
			for (int i = 0; i < 4; ++i)
			{
				x[i] = now_mapx + sqs[s_idx].dir[d_idx][i][0];
				y[i] = now_mapy + sqs[s_idx].dir[d_idx][i][1];
			}
			for (int i = 0; i < 4; ++i)
			{
				if (y[i] >= 0)
				{
					map[x[i]][y[i]] = 1;
					map_c[x[i]][y[i]] = sqcolors[c_idx];
				}
			}
		}
	}
	void useboom() {
		if (boom != 0 && dtboom != true)
		{
			boom--;
			dtboom = true;
		}
	}
	void squareclear()
	{
		memset(map_tmp, 0, sizeof(map_tmp));
		memset(map_c_tmp, 0, sizeof(map_c_tmp));
		int y = 19; //由高至低
		bool boomt = false;
		for (int i = 19; i >= 0; --i)
		{
			int cnt = 0;
			for (int j = 0; j < 10; ++j)
			{
				if (map[j][i]) //查看是否滿列
					++cnt;
				if (map[j][i] == 2)
				{
					cnt = 20;
					break;
				}
			}
			if (cnt < 10) //如果十格沒滿
			{
				for (int j = 0; j < 10; ++j)
				{
					map_tmp[j][y] = map[j][i];
					map_c_tmp[j][y] = map_c[j][i];
				}
				--y;
			}
			else if (cnt == 20)
			{
				continue;
			}
			else
			{
				score += 10;
			}
		}

		for (int i = 0; i < 20; ++i) //記錄圖像
		{
			for (int j = 0; j < 10; ++j)
			{
				map[j][i] = map_tmp[j][i];
				map_c[j][i] = map_c_tmp[j][i];
			}
		}
	}
	bool checkover()
	{
		for (int i = 0; i < 10; ++i)
		{
			if (map[i][0])
				return true;
		}
		return false;
	}

	void getcmd(Cmd cmd)
	{
		//取得cmd鍵盤內容並動作
		switch (cmd)
		{
		case Cmd::cmd_down:
			movedown();
			break;
		case Cmd::cmd_left:
			moveleft();
			break;
		case Cmd::cmd_right:
			moveright();
			break;
		case Cmd::cmd_rotate:
			moverotate();
			break;
		case Cmd::cmd_boom:
			useboom();
			break;
		case Cmd::cmd_space:
			quickmovedown();
			break;
		}
	}
};


int main()
{
	wchar_t end_tps[30];
	beginTetris a;
	while (true) //進入死迴圈
	{
		a.nextround();
		while (!a.flag_gameover) //當gameover 不等於1時
		{
			a.nowround_square();
			while (!a.flag_continue)
			{
				BeginBatchDraw();
				a.map_gamebackground();
				a.map_sidebackground();
				a.drawMap();
				a.DrawSquareInside();
				EndBatchDraw();
				Cmd cmd = a.getCmd();
				a.getcmd(cmd);
			}
		}
		swprintf(end_tps, 39, L"遊戲結束\n你的總分為：%d\n是否重新開始", a.score);
		if (MessageBox(GetHWnd(), end_tps, L"Game over!", MB_ICONQUESTION | MB_YESNO) == IDNO)
			break;
	}
	return 0;
}