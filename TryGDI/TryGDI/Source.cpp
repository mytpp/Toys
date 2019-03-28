#include <algorithm>
#include <array>
#include <vector>
#include <set>
#include <cmath>
#include <cassert>
#include <windows.h>

const double PI = 3.141592653589793238463;

using namespace std;

void DrawLine(HDC hdc, 
	int bx, int by, 
	int ex, int ey)
{
	int xDelta = abs(ex - bx);
	int yDelta = abs(ey - by);
	int e;

	if (yDelta <= xDelta) { //x grows slower
		if (ex < bx) {      //make x start from the small value to the big value
			swap(bx, ex);
			swap(by, ey);
		}
		e = -xDelta;
		while (bx <= ex) {
			SetPixel(hdc, bx, by, RGB(0, 0, 0));
			if (0 <= e) {
				e -= 2 * xDelta;
				if (by <= ey)
					by++;
				else
					by--;
			}
			e += 2 * yDelta;
			bx++;
		}
	} else {           //y grows slower
		if (ey < by) { //make y start from the small value to the big value
			swap(bx, ex);
			swap(by, ey);
		}
		e = -yDelta;
		while (by <= ey) {
			SetPixel(hdc, bx, by, RGB(0, 0, 0));
			if (0 <= e) {
				e -= 2 * yDelta;
				if (bx <= ex)
					bx++;
				else
					bx--;
			}
			e += 2 * xDelta;
			by++;
		}
	}
	
	
}

void BresenhamArc(HDC hdc, 
	int x0, int y0, 
	int r, int direction)
{
	int x = 0;
	int y = r;
	int d = 3-2*r;
	while (x <= y) {
		int xi, yi;
		switch (direction)
		{
		case 0: xi = x0 + x; yi = y0 + y; break;
		case 1: xi = x0 + y; yi = y0 + x; break;
		case 2: xi = x0 + y; yi = y0 - x; break;
		case 3: xi = x0 + x; yi = y0 - y; break;
		case 4: xi = x0 - x; yi = y0 - y; break;
		case 5: xi = x0 - y; yi = y0 - x; break;
		case 6: xi = x0 - y; yi = y0 + x; break;
		case 7: xi = x0 - x; yi = y0 + y; break;
		default: break;
		}

		SetPixel(hdc, xi, yi, RGB(0, 0, 0));
		if (d < 0) {
			d = d + 4 * x + 6;
		} else {
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}

void DrawCircle(HDC hdc, 
	int x0, int y0, int r)
{
	for (int i = 0; i < 8; i++) {
		BresenhamArc(hdc, x0, y0, r, i);
	}
}

void DrawEllipseArc(HDC hdc, 
	int x0, int y0, 
	int a, int b, 
	double startRadian, double endRadian)
{
	double cost = 1;
	double sint = 0;
	double tDelta = PI / max(a, b) * 10;
	double cosDeltaT = cos(tDelta);
	double sinDeltaT = sin(tDelta);
	double xStart = a, yStart = 0;
	double xEnd, yEnd;

	while (startRadian <= endRadian) {
		//this method losts accuracy
		//cost = cost * cosDeltaT - sint * sinDeltaT;
		//sint = sint * cosDeltaT + cost * sinDeltaT;
		//xEnd = a * cost;
		//yEnd = b * sint;
		startRadian += tDelta;
		xEnd = a * cos(startRadian);
		yEnd = b * sin(startRadian);
		DrawLine(hdc, 
			x0 + xStart, y0 + yStart, 
			x0 + xEnd, y0 + yEnd);
		xStart = xEnd;
		yStart = yEnd;
	}
}

void DrawEllipse(HDC hdc, 
	int x0, int y0, 
	int a, int b)
{
	DrawEllipseArc(hdc, x0, y0, a, b, 0, 2 * PI);
}

struct Point
{
	int x;
	int y;
};

//the first line is from polygon[0] to polygon[1]
template <size_t numPoint>
using MyPolygon = array<Point, numPoint>;

template <size_t numPoint>
MyPolygon<numPoint> MovePolygon(
	const MyPolygon<numPoint> oldPolygon, int x, int y)
{
	MyPolygon<numPoint> newPolygon;
	for (int i = 0; i < numPoint; i++) {
		newPolygon[i].x = oldPolygon[i].x + x;
		newPolygon[i].y = oldPolygon[i].y + y;
	}
	return newPolygon;
}

template <size_t numPoint>
MyPolygon<numPoint> ZoomPolygon(
	const MyPolygon<numPoint> oldPolygon, double x, double y)
{
	MyPolygon<numPoint> newPolygon;
	for (int i = 0; i < numPoint; i++) {
		newPolygon[i].x = oldPolygon[i].x / x;
		newPolygon[i].y = oldPolygon[i].y / y;
	}
	return newPolygon;
}

template <size_t numPoint>
void DrawPolygon(HDC hdc, const MyPolygon<numPoint>& polygon)
{
	for (int i = 0; i < numPoint; i++) {
		DrawLine(hdc, polygon[i].x, polygon[i].y,
			polygon[(i + 1) % numPoint].x, polygon[(i + 1) % numPoint].y);
	}
}


template <size_t numPoint>
void PolyFillWithLines(HDC hdc, 
	const MyPolygon<numPoint>& polygon, 
	double slope, int bDelta)
{
	static_assert(2 < numPoint, "MyPolygon must have at least 3 Points");
	
	array<pair<int, int>, numPoint> intercepts;
	int minIntercept = INT32_MAX;
	int maxIntercept = INT32_MIN;
	for (int i = 0; i < numPoint; i++) {
		intercepts[i].first = polygon[i].y - slope * polygon[i].x;
		intercepts[i].second = polygon[(i + 1) % numPoint].y - slope * polygon[(i + 1) % numPoint].x;
		if (intercepts[i].second < intercepts[i].first)
			swap(intercepts[i].first, intercepts[i].second);
		minIntercept = min(minIntercept, intercepts[i].first);
		maxIntercept = max(maxIntercept, intercepts[i].second);
	}

	vector<Point> intersections;
	for (int b = minIntercept + bDelta; b < maxIntercept; b+= bDelta) {
		for (int j = 0; j < numPoint; j++) {
			if (intercepts[j].first <= b && b < intercepts[j].second) {
				int x = ( //there is a conversion from double to int
						polygon[j].x*polygon[(j + 1) % numPoint].y -       //Xp*Yq
						polygon[j].y*polygon[(j + 1) % numPoint].x +       //Yp*Xq
						b * (polygon[(j + 1) % numPoint].x - polygon[j].x) //Bj*(Xq-Xp)
					) / (
						(polygon[(j + 1) % numPoint].y - polygon[j].y) -
						slope * (polygon[(j + 1) % numPoint].x - polygon[j].x)
					);
				int y = slope * x + b;
				intersections.push_back({ x, y });
			}
		}
		assert(intersections.size() % 2 == 0);
		sort(intersections.begin(), intersections.end(), 
			[](const Point& lhs, const Point& rhs) { 
			return lhs.x < rhs.x; 
		});
		for (int i = 0; i < intersections.size(); i+=2) {
			DrawLine(hdc,
				intersections[i].x, intersections[i].y,
				intersections[i + 1].x, intersections[i + 1].y);
		}
		intersections.clear();
	}
}

struct EdgeEntry
{
	int y;
	int yMax;
	double x;
	double xDelta;
};

bool operator<(const EdgeEntry& lhs, const EdgeEntry& rhs)
{
	if(lhs.y < rhs.y)
		return true;
	if (lhs.y > rhs.y)
		return false;
	if (lhs.x < rhs.x)
		return true;
	if (lhs.x > rhs.x)
		return false;
	if (lhs.xDelta < rhs.xDelta)
		return true;
	return false;
}

template <size_t numPoint>
void PolyFill(HDC hdc, 
	const MyPolygon<numPoint>& polygon, COLORREF color) 
{
	multiset<EdgeEntry> edgeTable;
	vector<EdgeEntry> activeEdgeList;
	int yMin = INT32_MAX;
	int yMax = INT32_MIN;
	
	//construct ET
	for (int i = 0; i < numPoint; i++) { 
		EdgeEntry entry;
		//singular points are handled automatically
		if (polygon[i].y == polygon[(i + 1) % numPoint].y) { //ignore horizontal lines
			continue;
		} else if (polygon[i].y < polygon[(i + 1) % numPoint].y) {
			entry.y = polygon[i].y;
			entry.x = polygon[i].x;
			entry.yMax = polygon[(i + 1) % numPoint].y;
		} else {
			entry.y = polygon[(i + 1) % numPoint].y;
			entry.x = polygon[(i + 1) % numPoint].x;
			entry.yMax = polygon[i].y;
		}
		entry.xDelta = static_cast<double>
				(polygon[i].x - polygon[(i + 1) % numPoint].x) /
				(polygon[i].y - polygon[(i + 1) % numPoint].y);
		yMin = min(yMin, entry.y);
		yMax = max(yMax, entry.yMax);
		edgeTable.insert(entry);
	}

	for (int y = yMin ; y < yMax; y++) {
		auto it = remove_if(activeEdgeList.begin(), activeEdgeList.end(),
			[y](const EdgeEntry& entry) { return y == entry.yMax; });
		activeEdgeList.erase(it, activeEdgeList.end());
		auto beg = edgeTable.lower_bound({ y, 0, INT32_MIN, INT32_MIN });
		auto end = edgeTable.lower_bound({ y + 1, 0, INT32_MIN, INT32_MIN });
		activeEdgeList.insert(activeEdgeList.end(), beg, end);
		sort(activeEdgeList.begin(), activeEdgeList.end(), 
			[](const EdgeEntry& lhs, const EdgeEntry& rhs) {
			if (lhs.x < rhs.x)
				return true;
			if (lhs.x > rhs.x)
				return false;
			if (lhs.xDelta < rhs.xDelta)
				return true;
			return false;
		});

		assert(activeEdgeList.size() % 2 == 0);
		for (int i = 0; i < activeEdgeList.size(); i += 2) {
			for (int x = activeEdgeList[i].x; x < activeEdgeList[i + 1].x; x++)
				SetPixel(hdc, x, y, color);
			activeEdgeList[i].x += activeEdgeList[i].xDelta;
			activeEdgeList[i + 1].x += activeEdgeList[i + 1].xDelta;
		}
	}
}


// 用于注册的窗口类名
const char g_szClassName[] = "myWindowClass";

void TestPaint(HDC hdc) 
{
	DrawLine(hdc, 250, 200, 150, 150);
	DrawLine(hdc, 150, 150, 50, 200);
	DrawLine(hdc, 50, 200, 150, 350);
	DrawLine(hdc, 150, 350, 250, 200);

	DrawCircle(hdc, 150, 150, 30);

	DrawLine(hdc, 100, 400, 300, 400);
	DrawLine(hdc, 200, 350, 200, 450);
	DrawEllipse(hdc, 200, 400, 100, 50);

	MyPolygon<7> polygon1 = { {
		{100,20},
		{50,20},
		{20,50},
		{20,100},
		{80,70},
		{160,120},
		{160,60}
	} };
	DrawPolygon(hdc, polygon1);
	PolyFillWithLines(hdc, polygon1, 0.3, 5);

	MyPolygon<7> polygon2 = MovePolygon(polygon1, 400, 0);
	DrawPolygon(hdc, polygon2);
	PolyFill(hdc, polygon2, RGB(255, 0, 0));
}

void PaintSurname(HDC hdc)
{
	MyPolygon<12> part1 = { {
		{0, 75},
		{75, 75},
		{75, 0},
		{125, 0},
		{125, 75},
		{200, 75},
		{200, 100},
		{125, 100},
		{125, 500},
		{75, 500},
		{75, 100},
		{0, 100}
	} };
	part1 = ZoomPolygon(part1, 2, 2);
	part1 = MovePolygon(part1, 400, 300);
	DrawPolygon(hdc, part1);
	PolyFill(hdc, part1, RGB(255, 100, 100));

	DrawEllipse(hdc, 400 + 40 / 2, 300 + 225 / 2, 20 / 2, 75 / 2);
	DrawEllipse(hdc, 400 + 160 / 2, 300 + 225 / 2, 20 / 2, 75 / 2);

	DrawCircle(hdc, 400 + 375 / 2, 300 + 25 / 2, 12);

	MyPolygon<4> part2 = { {
		{250, 50},
		{500, 50},
		{500, 75},
		{250, 75}
	} };
	part2 = ZoomPolygon(part2, 2, 2);
	part2 = MovePolygon(part2, 400, 300);
	DrawPolygon(hdc, part2);
	PolyFill(hdc, part2, RGB(50, 20, 100));

	MyPolygon<10> part3 = { {
		{250, 275},
		{250, 100},
		{500, 100},
		{500, 275},
		{250, 275},
		{275, 250},
		{475, 250},
		{475, 125},
		{275, 125},
		{275, 250}
	} };
	part3 = ZoomPolygon(part3, 2, 2);
	part3 = MovePolygon(part3, 400, 300);
	DrawPolygon(hdc, part3);
	PolyFill(hdc, part3, RGB(150, 120, 100));

	MyPolygon<10> part4 = { {
		{325, 150},
		{425, 150},
		{425, 225},
		{325, 225},
		{325, 150},
		{350, 175},
		{350, 200},
		{400, 200},
		{400, 175},
		{350, 175}
	} };
	part4 = ZoomPolygon(part4, 2, 2);
	part4 = MovePolygon(part4, 400, 300);
	DrawPolygon(hdc, part4);
	PolyFill(hdc, part4, RGB(150, 120, 100));

	MyPolygon<10> part5 = { {
		{300, 300},
		{450, 300},
		{450, 425},
		{300, 425},
		{300, 300},
		{325, 325},
		{325, 400},
		{425, 400},
		{425, 325},
		{325, 325}
	} };
	part5 = ZoomPolygon(part5, 2, 2);
	part5 = MovePolygon(part5, 400, 300);
	DrawPolygon(hdc, part5);
	PolyFill(hdc, part5, RGB(250, 120, 50));

	MyPolygon<4> part6 = { {
		{325, 350},
		{425, 350},
		{425, 375},
		{325, 375}
	} };
	part6 = ZoomPolygon(part6, 2, 2);
	part6 = MovePolygon(part6, 400, 300);
	DrawPolygon(hdc, part6);
	PolyFill(hdc, part6, RGB(250, 120, 50));

	MyPolygon<4> part7 = { {
		{250, 450},
		{500, 450},
		{500, 475},
		{250, 475}
	} };
	part7 = ZoomPolygon(part7, 2, 2);
	part7 = MovePolygon(part7, 400, 300);
	DrawPolygon(hdc, part7);
	PolyFill(hdc, part7, RGB(50, 200, 50));
}

void Paint(HDC hdc)
{
	TestPaint(hdc);
	PaintSurname(hdc);
}


/*
 * 第四步,窗口过程
 */
LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// 窗口绘制消息
	case WM_PAINT:
		// paint struct 绘图结构体，存储目标窗口可以绘图的客户端区域(client area)
		PAINTSTRUCT ps;
		HDC hdc;   // DC(可画图的内存对象) 的句柄

		// 通过窗口句柄获取该窗口的 DC
		hdc = BeginPaint(hwnd, &ps);
		Paint(hdc);
		EndPaint(hwnd, &ps);
		break;

		// 窗口关闭消息
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
		// 窗口销毁消息
	case WM_DESTROY:
		PostQuitMessage(0); // 发送离开消息给系统
		break;
		// 其他消息
	default:
		// pass 给系统，咱不管
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

/*
 * 第一步,注册窗口类
 */
void RegisterMyWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	// 1)配置窗口属性
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = MyWindowProc; // 设置第四步的窗口过程回调函数
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// 2)注册
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, TEXT("窗口注册失败!"), TEXT("错误"), MB_ICONEXCLAMATION | MB_OK);
		exit(0); // 进程退出
	}
}

/*
 * 第二步，创建窗口
 */
HWND CreateMyWindow(HINSTANCE hInstance, int nCmdShow)
{
	HWND hwnd;
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		TEXT("我的窗口名称"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // 出现坐标 x,y 默认分配 窗口宽 400 高 300
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("窗口创建失败!"), TEXT("错误"), MB_ICONEXCLAMATION | MB_OK);
		exit(0); // 进程退出
	}

	// 显示窗口
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	return hwnd;
}

/*
 * 主函数
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG Msg;

	// 第一步,注册窗口类
	RegisterMyWindow(hInstance);
	// 第二步:创建窗口
	hwnd = CreateMyWindow(hInstance, nCmdShow);
	// 第三步:消息循环
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
