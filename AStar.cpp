// AStar.cpp : Defines the entry point for the application.
//

#include "AStar.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



int __stdcall WinMain(
	HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand( GetTickCount() );

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if( !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	// Initialise everything else
	MSG msg = {0};
	ZeroMemory(&Map,sizeof(MAP));

	Map.CreateMap(40,40);

	// Main message loop:
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if( bCreatingWall == 1 )
			{
				NODE* node = Map.MousePosToNode();
				if( node ) { node->state = 3; Render(); }
			}
			else if( bCreatingWall == 2 )
			{
				NODE* node = Map.MousePosToNode();
				if( node ) { if( node->state == 3 ) node->state = 0; Render(); }
			}
			Sleep(1);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ASTAR) );
	wcex.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon( wcex.hInstance, MAKEINTRESOURCE(IDI_ASTAR) );

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	g_WindowRect.left	= 0;
	g_WindowRect.top	= 0;
	g_WindowRect.right	= 800;
	g_WindowRect.bottom	= 600;
	AdjustWindowRect(&g_WindowRect,WS_OVERLAPPEDWINDOW,0);

	g_hWnd = CreateWindowEx(0,szWindowClass,szTitle,
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN)/2)-((g_WindowRect.right-g_WindowRect.left)/2),
		(GetSystemMetrics(SM_CYSCREEN)/2)-((g_WindowRect.bottom-g_WindowRect.top)/2),
		g_WindowRect.right-g_WindowRect.left, g_WindowRect.bottom-g_WindowRect.top,
		0, 0, hInstance, 0);

	if( !g_hWnd ) return 0;

	g_txtResults = CreateWindowEx(
		0,TEXT("STATIC"),TEXT("No path yet calculated.\nPress SPACE to search."),
		WS_CHILD | WS_VISIBLE,
		616, 16, 168, 32, g_hWnd, 0, hInst, 0 );

	g_BClearMap = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Clear Map"),
		WS_CHILD | WS_VISIBLE,
		616, 64, 168, 32, g_hWnd, 0, hInst, 0 );

	g_BOpenMap = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Open Map"),
		WS_CHILD | WS_VISIBLE,
		616, 112, 168, 32, g_hWnd, 0, hInst, 0 );

	g_BSaveMap = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Save Map"),
		WS_CHILD | WS_VISIBLE,
		616, 160, 168, 32, g_hWnd, 0, hInst, 0 );

	g_BRandomMap = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Random Map"),
		WS_CHILD | WS_VISIBLE,
		616, 208, 168, 32, g_hWnd, 0, hInst, 0 );

	g_BGenerateMaze = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Generate Maze"),
		WS_CHILD | WS_VISIBLE,
		616, 256, 168, 32, g_hWnd, 0, hInst, 0 );

	g_BToggleDisplay = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Alt Node Display"),
		WS_CHILD | WS_VISIBLE,
		616, 304, 168, 32, g_hWnd, 0, hInst, 0 );

	g_CBMethod = CreateWindowEx(0,TEXT("COMBOBOX"),0,
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
		616, 352, 168, 32, g_hWnd, 0, hInst, 0 );
	SendMessage(g_CBMethod,CB_ADDSTRING,0,(LPARAM)TEXT("A*"));
	SendMessage(g_CBMethod,CB_ADDSTRING,0,(LPARAM)TEXT("Dijkstra"));
	SendMessage(g_CBMethod,CB_ADDSTRING,0,(LPARAM)TEXT("Heuristic"));
	SendMessage(g_CBMethod,CB_SETCURSEL,0,0);

	g_TBWidth = CreateWindowEx(
		WS_EX_CLIENTEDGE,TEXT("EDIT"),TEXT("40"),
		WS_CHILD | WS_VISIBLE | ES_NUMBER,
		616, 400, 76, 32, g_hWnd, 0, hInst, 0 );
	g_TBHeight = CreateWindowEx(
		WS_EX_CLIENTEDGE,TEXT("EDIT"),TEXT("40"),
		WS_CHILD | WS_VISIBLE | ES_NUMBER,
		708, 400, 76, 32, g_hWnd, 0, hInst, 0 );
	g_TBDelayTime = CreateWindowEx(
		WS_EX_CLIENTEDGE,TEXT("EDIT"),TEXT("0"),
		WS_CHILD | WS_VISIBLE | ES_NUMBER,
		616, 448, 168, 32, g_hWnd, 0, hInst, 0 );
	g_BApply = CreateWindowEx(
		0,TEXT("BUTTON"),TEXT("Apply Map Settings"),
		WS_CHILD | WS_VISIBLE | ES_NUMBER,
		616, 496, 168, 32, g_hWnd, 0, hInst, 0 );

	return 1;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	NODE* node = NULL;

	switch( message )
	{
	case WM_CREATE:
		GetClientRect(hWnd,&g_ClientRect);
		g_hdcFrontBuffer = GetDC(hWnd);
		g_hdcBackBuffer = CreateCompatibleDC(g_hdcFrontBuffer);
		g_hbmBackBuffer = CreateCompatibleBitmap( g_hdcFrontBuffer,
			g_ClientRect.right-g_ClientRect.left,
			g_ClientRect.bottom-g_ClientRect.top );
		g_hdcBackBuffer_or = SelectObject(g_hdcBackBuffer, g_hbmBackBuffer);
		break;
	case WM_COMMAND:
		wmEvent = wParam>>16;
		if( (HWND)lParam == g_BClearMap )
		{
			if( wmEvent == BN_CLICKED )
			{
				Map.Refresh();
				SetFocus( g_hWnd );
				Render();
			}
		}
		else if( (HWND)lParam == g_BOpenMap )
		{
			if( wmEvent == BN_CLICKED )
			{
				Map.LoadData();
				SetFocus( g_hWnd );
				Render();
			}
		}
		else if( (HWND)lParam == g_BSaveMap )
		{
			if( wmEvent == BN_CLICKED )
			{
				Map.SaveData();
				SetFocus( g_hWnd );
				Render();
			}
		}
		else if( (HWND)lParam == g_BRandomMap )
		{
			if( wmEvent == BN_CLICKED )
			{
				Map.GenerateRandom();
				SetFocus( g_hWnd );
				Render();
			}
		}
		else if( (HWND)lParam == g_BGenerateMaze )
		{
			if( wmEvent == BN_CLICKED )
			{
				Map.GenerateMaze(&pStart);
				SetFocus( g_hWnd );
				Render();
			}
		}
		else if( (HWND)lParam == g_BToggleDisplay )
		{
			if( wmEvent == BN_CLICKED )
			{
				// Toggle display mode
				bOtherNodeData = !bOtherNodeData;
				SetFocus( g_hWnd );
				Render();
			}
		}
		else if( (HWND)lParam == g_CBMethod )
		{
			if( wmEvent == CBN_SELCHANGE )
			{
				// If the user makes a selection from the list:
				//   Send CB_GETCURSEL message to get the index of the selected list item.
				//   Send CB_GETLBTEXT message to get the item.
				int ItemIndex = SendMessage((HWND)lParam,(UINT)CB_GETCURSEL,0,0);
				SendMessage((HWND)lParam,CB_GETLBTEXT,(WPARAM)ItemIndex,(LPARAM)GlobalString);
				if( GlobalString[0] == TEXT('A') ) bMethod = METHOD_ASTAR;
				else if( GlobalString[0] == TEXT('D') ) bMethod = METHOD_DIJKSTRA;
				else bMethod = METHOD_HEURISTIC;
			}
			else if( wmEvent == CBN_CLOSEUP )
			{
				SetFocus( g_hWnd );
			}
		}
		else if( (HWND)lParam == g_BApply )
		{
			if( wmEvent == BN_CLICKED )
			{
				DWORD dwWidth, dwHeight;
				UINT uiDims[2];
				NODE* prevmap;

				GetWindowTextA( g_TBWidth, GlobalString, 1024 );
				dwWidth = atol(GlobalString);
				GetWindowTextA( g_TBHeight, GlobalString, 1024 );
				dwHeight = atol(GlobalString);
				GetWindowTextA( g_TBDelayTime, GlobalString, 1024 );
				dwDelayTime = atol(GlobalString);

				Map.GetDimensions( uiDims );

				if( dwWidth != uiDims[0] || dwHeight != uiDims[1] )
				{
					if( Map.HasNodes() )
					{
						Map.Clone(&prevmap);
						if( prevmap )
						{
							Map.CreateMap(dwWidth,dwHeight);
							Map.Integrate(prevmap,uiDims[0]*uiDims[1]);
							delete[] prevmap;
						}
					}
					else Map.CreateMap(dwWidth,dwHeight);
				}

				SetFocus( g_hWnd );

				Render();
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			//RECT rctTmp;

			BeginPaint(hWnd, &ps);

			Map.Draw(&pStart,&pEnd);
			BitBlt(g_hdcFrontBuffer,0,0,
				g_MapRect.right-g_MapRect.left,
				g_MapRect.bottom-g_MapRect.top,
				g_hdcBackBuffer,0,0,SRCCOPY);

			/*rctTmp.left = g_PanelRect.left+16;
			rctTmp.top = g_PanelRect.top+32;
			rctTmp.right = g_PanelRect.right;
			rctTmp.bottom = g_PanelRect.bottom;
			sprintf_s(GlobalString,32,"Path G score: %u",dwHighestG);
			DrawTextA( g_hdcFrontBuffer, GlobalString, -1, &rctTmp, 0 );*/

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_SIZE:
		GetClientRect(hWnd,&g_ClientRect);
		SelectObject(g_hdcBackBuffer, g_hdcBackBuffer_or);
		DeleteObject(g_hbmBackBuffer);
		DeleteDC(g_hdcBackBuffer);
		g_hdcBackBuffer = CreateCompatibleDC(g_hdcFrontBuffer);
		g_hbmBackBuffer = CreateCompatibleBitmap( g_hdcFrontBuffer,
			g_ClientRect.right-g_ClientRect.left,
			g_ClientRect.bottom-g_ClientRect.top );
		g_hdcBackBuffer_or = SelectObject(g_hdcBackBuffer, g_hbmBackBuffer);
		Map.UpdateWindowRect();

		SetWindowPos( g_txtResults, 0,
			g_PanelRect.left+16, g_PanelRect.top+16,
			168, 32, 0 );
		SetWindowPos( g_BClearMap, 0,
			g_PanelRect.left+16, g_PanelRect.top+64,
			168, 32, 0 );
		SetWindowPos( g_BOpenMap, 0,
			g_PanelRect.left+16, g_PanelRect.top+112,
			168, 32, 0 );
		SetWindowPos( g_BSaveMap, 0,
			g_PanelRect.left+16, g_PanelRect.top+160,
			168, 32, 0 );
		SetWindowPos( g_BRandomMap, 0,
			g_PanelRect.left+16, g_PanelRect.top+208,
			168, 32, 0 );
		SetWindowPos( g_BGenerateMaze, 0,
			g_PanelRect.left+16, g_PanelRect.top+256,
			168, 32, 0 );
		SetWindowPos( g_BToggleDisplay, 0,
			g_PanelRect.left+16, g_PanelRect.top+304,
			168, 32, 0 );
		SetWindowPos( g_CBMethod, 0,
			g_PanelRect.left+16, g_PanelRect.top+352,
			168, 32, 0 );
		SetWindowPos( g_TBWidth, 0,
			g_PanelRect.left+16, g_PanelRect.top+400,
			76, 32, 0 );
		SetWindowPos( g_TBHeight, 0,
			g_PanelRect.left+108, g_PanelRect.top+400,
			76, 32, 0 );
		SetWindowPos( g_TBDelayTime, 0,
			g_PanelRect.left+16, g_PanelRect.top+448,
			168, 32, 0 );
		SetWindowPos( g_BApply, 0,
			g_PanelRect.left+16, g_PanelRect.top+496,
			168, 32, 0 );

		break;
	case WM_DESTROY:
		Map.DestroyMap();
		SelectObject(g_hdcBackBuffer, g_hdcBackBuffer_or);
		DeleteObject(g_hbmBackBuffer);
		DeleteDC(g_hdcBackBuffer);
		ReleaseDC(hWnd,g_hdcFrontBuffer);
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		bCreatingWall = 1;
		break;
	case WM_RBUTTONDOWN:
		bCreatingWall = 2;
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		bCreatingWall = 0;
		break;
	case WM_KEYDOWN:
		switch( wParam )
		{
		case 'S':
			node = Map.MousePosToNode();
			if( node )
			{
				pStart.x = node->x;
				pStart.y = node->y;
			}
			Render();
			break;
		case VK_SPACE:
			node = Map.MousePosToNode();
			if( node )
			{
				pEnd.x = node->x;
				pEnd.y = node->y;
				Map.ComputePath(pStart.x, pStart.y, pEnd.x, pEnd.y);
				Render();
			}
			break;
		}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



void Render()
{
	InvalidateRect(g_hWnd,0,0);
	UpdateWindow(g_hWnd);
}
void PeekRender()
{
	MSG unused = {0};
	PAINTSTRUCT ps;

	PeekMessage(&unused,0,0,0,PM_NOREMOVE);
	InvalidateRect(g_hWnd,&g_MapRect,0);

	BeginPaint(g_hWnd, &ps);
	Map.Draw(&pStart,&pEnd);
	BitBlt(g_hdcFrontBuffer,0,0,
		g_MapRect.right-g_MapRect.left,
		g_MapRect.bottom-g_MapRect.top,
		g_hdcBackBuffer,0,0,SRCCOPY);
	EndPaint(g_hWnd, &ps);
}



NodeStack::NodeStack()
{
	pNodes = new NODE*[32];
	for( DWORD i = 0; i < 32; i++ )
		pNodes[i] = 0;
	dwPoolSize = 32;
	dwLast = 0;
}
NodeStack::~NodeStack()
{
	delete[] pNodes;
}

bool NodeStack::IsEmpty()
{
	for( DWORD i = 0; i < dwPoolSize; i++ ) {
		if( pNodes[i] ) return false;
	} return true;
}
NODE* NodeStack::GetAt(DWORD dwElem)
{
	if( dwElem > dwPoolSize ) return 0;
	else return pNodes[dwElem];
}
NODE* NodeStack::GetFirst()
{
	for( DWORD i = 0; i < dwPoolSize; i++ ) {
		if( pNodes[i] ) return pNodes[i];
	} return 0;
}
NODE* NodeStack::GetLast()
{
	return pNodes[dwLast];
}
NODE* NodeStack::GetRandom()
{
	DWORD dwNumElems = 0, i, j;
	for( i = 1; i < dwPoolSize; i++ ) {
		if( pNodes[i] ) dwNumElems++;
	} if( !dwNumElems ) return 0;

	DWORD dwRandNode = rand() % (dwNumElems);
	for( i = 1, j = 0; i < dwPoolSize; i++ ) {
		if( pNodes[i] ) {
			if( j == dwRandNode )
				return pNodes[i];
			else j++;
		}
	} return pNodes[dwLast];
}
bool NodeStack::Push(NODE *pNode)
{
	DWORD dwEmpty = dwLast+1;
	if( dwEmpty >= dwPoolSize )
	{
		Compact();
		dwEmpty = dwLast+1;
		if( dwEmpty >= dwPoolSize )
			if( !Expand() ) return false;
	}
	pNodes[dwEmpty] = pNode;
	dwLast ++;
	return true;
}
bool NodeStack::Pop(DWORD dwElem)
{
	if( dwElem >= dwPoolSize )
		return false;
	pNodes[dwElem] = 0;
	if( dwElem == dwLast )
	{
		do {
			dwLast --;
			if( pNodes[dwLast] ) break;
		} while( dwLast );
	}
	return true;
}
bool NodeStack::Pop(NODE *pNode)
{
	for( DWORD i = 0; i < dwPoolSize; i++ ) {
		if( pNodes[i] == pNode ) {
			pNodes[i] = 0;
			if( i == dwLast ) goto L_EvalLast;
			return true;
		}
	} return false;
L_EvalLast:
	do {
		dwLast --;
		if( pNodes[dwLast] ) return true;
	} while( dwLast );
	return false;
}
bool NodeStack::Compact()
{
	DWORD i = 1, j = 0, dwNumElems = 0;
	for( ; i < dwPoolSize; i++ )
	{
		if( pNodes[i] ) dwNumElems++;
	}
	if( !dwNumElems ) return true;
	NODE **pTemp = new(std::nothrow) NODE*[dwNumElems];
	if( !pTemp ) return false;
	for( i = 1; i < dwPoolSize; i++ )
	{
		if( pNodes[i] )
		{
			pTemp[j] = pNodes[i];
			j++;
		}
	}
	for( i = 0; i < dwNumElems; i++ )
	{
		pNodes[i+1] = pTemp[i];
	}
	delete[] pTemp;
	for( ++i; i < dwPoolSize; i++ )
	{
		pNodes[i] = 0;
	}
	dwLast = dwNumElems;
	return true;
}
bool NodeStack::Reverse()
{
	NODE **pRevNodes;
	pRevNodes = new(std::nothrow) NODE*[dwPoolSize];
	if( !pRevNodes ) return false;
	DWORD i = 0, j = 0;
	while( i < dwPoolSize )
	{
		pRevNodes[i] = 0;
		i++;
	}
	i = dwLast;
	while( i )
	{
		if( pNodes[i] ) {
			pRevNodes[++j] = pNodes[i];
		} i--;
	}
	delete[] pNodes;
	pNodes = pRevNodes;
	dwLast = j;
	return true;
}
bool NodeStack::Expand()
{
	NODE **pNewNodes;
	DWORD dwNewPoolSize = dwPoolSize + 32;
	pNewNodes = new(std::nothrow) NODE*[dwNewPoolSize];
	if( !pNewNodes ) return false;
	DWORD i = 0;
	while( i < dwPoolSize )
	{
		pNewNodes[i] = pNodes[i];
		i++;
	}
	while( i < dwNewPoolSize )
	{
		pNewNodes[i] = 0;
		i++;
	}
	delete[] pNodes; pNodes = pNewNodes;
	dwPoolSize = dwNewPoolSize;
	return true;
}
bool NodeStack::Echo()
{
	char strbuf[512];
	char strnum[32];

	sprintf_s(strbuf,512,"NodeStack: (%i capacity)\n",dwPoolSize);
	
	for( DWORD i = 0; i < dwPoolSize; i++ )
	{
		itoa( int(pNodes[i]), strnum, 16 );
		strcat_s( strbuf, 512, strnum );
		if( i == dwLast ) strcat_s( strbuf, 512, " !!LAST!!\n" );
		else strcat_s( strbuf, 512, ", " );
	}

	MessageBoxA( g_hWnd, strbuf, "Nodestack.", MB_ICONINFORMATION );

	return true;
}



void MAP::GetDimensions(UINT *out)
{
	out[0] = W;
	out[1] = H;
}
void MAP::UpdateWindowRect()
{
	if( W == 0 || H == 0 ) return;

	g_MapRect.left = g_ClientRect.left;
	g_MapRect.top = g_ClientRect.top;
	g_MapRect.right = g_ClientRect.right-200;
	g_MapRect.bottom = g_ClientRect.bottom;

	g_PanelRect.left = g_MapRect.right;
	g_PanelRect.top = g_ClientRect.top;
	g_PanelRect.right = g_ClientRect.right;
	g_PanelRect.bottom = g_ClientRect.bottom;

	DW = (FLOAT)(g_MapRect.right-g_MapRect.left)/(FLOAT)W;
	DH = (FLOAT)(g_MapRect.bottom-g_MapRect.top)/(FLOAT)H;
}
bool MAP::CreateMap(UINT Width, UINT Height)
{
	if( nodes ) delete[] nodes;

	W = Width;
	H = Height;
	T = Width * Height;

	nodes = new(std::nothrow) NODE[T];
	if( !nodes ) return false;

	for( UINT i = 0; i < T; i++ )
	{
		nodes[i].f = nodes[i].g = nodes[i].h = 0;
		nodes[i].state = 0;
		nodes[i].parent = 0;
		nodes[i].x = i%W;
		nodes[i].y = i/W;
	}

	UpdateWindowRect();
	return true;
}
void MAP::DestroyMap()
{
	if( nodes ) {
		delete[] nodes;
		nodes = NULL;
	} W = H = T = 0;
}
NODE* MAP::GetAt(UINT X, UINT Y)
{
	if( X >= 0 && X < W )
		if( Y >= 0 && Y < H ) return &nodes[(Y*W)+X];
	return NULL;
}
NODE* MAP::GetAt(UINT At)
{
	if( At >= 0 && At < T ) return &nodes[At];
	return NULL;
}
NODE* MAP::GetFast(UINT X, UINT Y)
{
	return &nodes[(Y*W)+X];
}
NODE* MAP::MousePosToNode()
{
	if( DW == 0 || DH == 0 ) return 0;

	POINT pCur;
	GetCursorPos(&pCur);
	ScreenToClient(g_hWnd,&pCur);

	pCur.x = (long)floor(double(pCur.x)/double(DW));
	pCur.y = (long)floor(double(pCur.y)/double(DH));

	return GetAt(pCur.x, pCur.y);
}
BOOL MAP::GetNodesUnderCursor(LPPOINT pPrev, LPPOINT pNow, void (__stdcall *foo)(NODE*))
{
	if( !foo || !pPrev || !pNow ) return FALSE;

	POINT point1, point2;
	point1.x = (long)floor((double)(pPrev->x/DW));
	point1.y = (long)floor((double)(pPrev->y/DH));
	point2.x = (long)floor((double)(pNow ->x/DW));
	point2.y = (long)floor((double)(pNow ->y/DH));

	LONG dx, dy;
	double dv, dw = 0;
	dx = point2.x - point1.x;
	dy = point2.y - point1.y;

	if( dx > dy )
	{
		//
		// Low gradient
		//
		dv = dy/dx;

		POINT pEval = point1;
		NODE* pNode;
		while( pEval.x != point2.x && pEval.y != point2.y )
		{
			dw += dv;

			pEval.x = pNow->x + (long)((double)dx * dv);
			pNode = GetAt(
				(LONG)floor((double)(pEval.x/DW)),
				(LONG)floor((double)(pEval.y/DH)));
			if( pNode ) foo(pNode);
		}
	}
	else
	{
		//
		// Equal/High gradient
		//
		dv = dx/dy;

		POINT pEval = point1;
		NODE* pNode;
		while( pEval.x != point2.x && pEval.y != point2.y )
		{
			dw += dv;

			pEval.x = pNow->x + (long)((double)dx * dv);
			pNode = GetAt(
				(LONG)floor((double)(pEval.x/DW)),
				(LONG)floor((double)(pEval.y/DH)));
			if( pNode ) foo(pNode);
		}
	}

	return TRUE;
}
bool MAP::HasNodes()
{
	if( nodes ) return true;
	return false;
}
void MAP::Draw(LPPOINT _start, LPPOINT _end)
{
	if( !nodes ) return;

	RECT rct;
	FLOAT XOffset, YOffset;
	COLORREF col;

	col = RGB(192,192,192);
	SetDCBrushColor(g_hdcBackBuffer,col);
	FillRect(g_hdcBackBuffer,&g_MapRect,(HBRUSH)GetStockObject(DC_BRUSH));

	if( bOtherNodeData )
	{
		for( UINT i = 0; i < T; i++ )
		{
			switch( nodes[i].state )
			{
			case 1:
				col = RGB(0,255,255);
				break;
			case 2:
				{
					double dblLerp = dwHighestGScore > 0 ? double(nodes[i].g)/double(dwHighestGScore) : 1.0;
					col = Lerp(0xC00000,0x0000C0,dblLerp);
				}
				break;
			case 3:
				col = RGB(64,64,64);
				break;
			case 4:
				col = RGB(0,128,0);
				break;
			default:
				continue;
			}

			XOffset = float(nodes[i].x)*DW;
			YOffset = float(nodes[i].y)*DH;
			rct.left	= long(ceil(XOffset));
			rct.top		= long(ceil(YOffset));
			rct.right	= long(ceil(XOffset+DW));
			rct.bottom	= long(ceil(YOffset+DH));
			SetDCBrushColor(g_hdcBackBuffer,col);
			FillRect(g_hdcBackBuffer,&rct,(HBRUSH)GetStockObject(DC_BRUSH));
		}
	}
	else
	{
		for( UINT i = 0; i < T; i++ )
		{
			switch( nodes[i].state )
			{
			case 3:
				col = RGB(128,128,128);
				break;
			case 4:
				col = RGB(0,128,0);
				break;
			default:
				continue;
			}

			XOffset = float(nodes[i].x)*DW;
			YOffset = float(nodes[i].y)*DH;
			rct.left	= long(ceil(XOffset));
			rct.top		= long(ceil(YOffset));
			rct.right	= long(ceil(XOffset+DW));
			rct.bottom	= long(ceil(YOffset+DH));

			SetDCBrushColor(g_hdcBackBuffer,col);
			FillRect(g_hdcBackBuffer,&rct,(HBRUSH)GetStockObject(DC_BRUSH));
		}
	}

	XOffset = float(_start->x)*DW;
	YOffset = float(_start->y)*DH;
	rct.left	= long(ceil(XOffset));
	rct.top		= long(ceil(YOffset));
	rct.right	= long(ceil(XOffset+DW));
	rct.bottom	= long(ceil(YOffset+DH));
	SetDCBrushColor(g_hdcBackBuffer,RGB(0,255,0));
	FillRect(g_hdcBackBuffer,&rct,(HBRUSH)GetStockObject(DC_BRUSH));

	XOffset = float(_end->x)*DW;
	YOffset = float(_end->y)*DH;
	rct.left	= long(ceil(XOffset));
	rct.top		= long(ceil(YOffset));
	rct.right	= long(ceil(XOffset+DW));
	rct.bottom	= long(ceil(YOffset+DH));
	SetDCBrushColor(g_hdcBackBuffer,RGB(255,0,255));
	FillRect(g_hdcBackBuffer,&rct,(HBRUSH)GetStockObject(DC_BRUSH));
}
void MAP::Refresh()
{
	for( UINT i = 0; i < T; i++ )
	{
		nodes[i].f = 0;
		nodes[i].g = 0;
		nodes[i].h = 0;
		nodes[i].parent = 0;
		nodes[i].state = 0;
	}
}
void MAP::GenerateRandom()
{
	for( UINT i = 0; i < T; i++ )
	{
		if( !(rand()%4) ) nodes[i].state = 3;
		else nodes[i].state = 0;
	}
}
void MAP::GenerateMaze(LPPOINT start)
{
	/* Start by filling grid with walls
	and finding start point */
	NODE *current = GetAt(start->x,start->y);
	if( !current ) current = GetAt(W/2,H/2);
	for( UINT i = 0; i < T; i++ ) {
		nodes[i].f = 0;
		nodes[i].g = 0;
		nodes[i].h = 0;
		nodes[i].parent = 0;
		nodes[i].state = 3;
	} current->state = 0;

	/* Find appropriate direction to
	carve maze. Generate random direction
	(east, north, west, south) */
	bool bOldOtherNodeData = bOtherNodeData;
	bOtherNodeData = true;
	char bDir = rand() % 5;
	char bWind;
	POINT pntExamine;
	POINT pntDir;
	NODE *examine;
	NodeStack nodestack;

	while( true )
	{
		if( rand() > RAND_MAX/2 ) bWind = 1;
		else bWind = -1;

		for( BYTE i = 0; ; )
		{
			/* Determine whether direction is
			suitable to begin with */
			pntExamine.x = current->x;
			pntExamine.y = current->y;
			switch( bDir )
			{
			case 0: default: // East
				pntDir.x = 1;
				pntDir.y = 0;
				break;
			case 1: // North
				pntDir.x = 0;
				pntDir.y =-1;
				break;
			case 2: // West
				pntDir.x =-1;
				pntDir.y = 0;
				break;
			case 3: // South
				pntDir.x = 0;
				pntDir.y = 1;
				break;
			}
			pntExamine.x += pntDir.x;
			pntExamine.y += pntDir.y;
			if( examine = GetAt(pntExamine.x,pntExamine.y) )
			{
				if( examine->state != 3 )
				{
					bDir += bWind;
					if( bDir > 3 ) bDir = 0;
					else if( bDir < 0 ) bDir = 3;
					if( ++i >= 4 ) goto L_DeadEnd;
					else continue;
				}
				pntExamine.x += pntDir.x;
				pntExamine.y += pntDir.y;
				if(	examine = GetAt(pntExamine.x,pntExamine.y) )
				{
					if( examine->state != 3 )
					{
						bDir += bWind;
						if( bDir > 3 ) bDir = 0;
						else if( bDir < 0 ) bDir = 3;
						if( ++i >= 4 ) goto L_DeadEnd;
						else continue;
					}
					goto L_Carve;
				}
				else
				{
					bDir += bWind;
					if( bDir > 3 ) bDir = 0;
					else if( bDir < 0 ) bDir = 3;
					if( ++i >= 4 ) goto L_DeadEnd;
					else continue;
				}
			}
			else
			{
				bDir += bWind;
				if( bDir > 3 ) bDir = 0;
				else if( bDir < 0 ) bDir = 3;
				if( ++i >= 4 ) goto L_DeadEnd;
				else continue;
			}
		}

L_DeadEnd:
		/* Dead end. Backtrack */
		if( nodestack.IsEmpty() ) break;
		else
		{
			current = nodestack.GetRandom();
			nodestack.Pop(current);
			continue;
		}

L_Carve:
		/* Carve */
		current = GetAt(pntExamine.x,pntExamine.y);
		GetAt(pntExamine.x-pntDir.x,pntExamine.y-pntDir.y)->state = 0;
		current->state = 0;
		nodestack.Push( current );

		/* Turn in a random direction */
		switch( rand() % 3 )
		{
		case 0: // Turn left
			if( --bDir < 0 ) bDir = 3;
			break;
		case 2: // Turn right
			if( ++bDir > 3 ) bDir = 0;
			break;
		}

		if( dwDelayTime > 0 )
		{
			PeekRender();
			if( GetAsyncKeyState( VK_END ) ) dwDelayTime = 0;
			Sleep(dwDelayTime);
		}
	}

	bOtherNodeData = bOldOtherNodeData;
}
bool MAP::ComputePath(UINT X1, UINT Y1, UINT X2, UINT Y2)
{
	if( !nodes ) return false;
	if( X1 < 0 || X1 >= W || Y1 < 0 || Y1 >= H ) return false;
	if( X2 < 0 || X2 >= W || Y2 < 0 || Y2 >= H ) return false;

	bool bOldOtherNodeData = bOtherNodeData;
	std::vector<NODE*> openlist;
	NODE* current = GetFast(X1,Y1);
	NODE* examine;
	UINT uiOpenAt = 0, oSize;
	bool bLeft, bRight, bUp, bDown;

	openlist.push_back(current);
	bOtherNodeData = true;
	dwHighestGScore = 0;

	for( UINT i = 0; i < T; i++ )
	{
		NODE& node = nodes[i];
			
		if( node.state == 3 ) continue;

		int dx = (X2-node.x)*10, dy = (Y2-node.y)*10;
		dx = dx < 0 ? -dx : dx;
		dy = dy < 0 ? -dy : dy;
		node.f = node.g = 0;
		node.h = dx + dy;
		node.state = 0;
		node.parent = 0;
	}

	if( dwDelayTime > 0 )
	{
		PeekRender();
		Sleep(dwDelayTime);
	}

	UINT64 qw_t, qw_f, qw_c;
	QueryPerformanceFrequency((LARGE_INTEGER*)&qw_f);
	double dbl_f = double(qw_f)/1000.0;
	QueryPerformanceCounter((LARGE_INTEGER*)&qw_t);

	switch( bMethod )
	{
	case METHOD_DIJKSTRA:
		/* Use Dijkstra's algorithm for search */
		while( current )
		{
			if( current->g > dwHighestGScore ) dwHighestGScore = current->g;

			current->state = 2;
			openlist.erase(openlist.begin()+uiOpenAt);

			if( current->x == X2 && current->y == Y2 )
			{
				NODE* parent = current->parent;
				while( parent )
				{
					parent->state = 4;
					parent = parent->parent;
				}
				dwPathGScore = current->g;
				current = 0;
				break;
			}

			bLeft = bRight = bUp = bDown = false;

			if( examine = GetAt(current->x+1,current->y) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bRight = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bRight = true;
					}
				}
			}

			if( examine = GetAt(current->x,current->y-1) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bUp = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bUp = true;
					}
				}
			}

			if( examine = GetAt(current->x-1,current->y) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bLeft = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bLeft = true;
					}
				}
			}

			if( examine = GetAt(current->x,current->y+1) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bDown = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bDown = true;
					}
				}
			}

			if( bRight )
			{
				if( bUp )
				{
					if( examine = GetAt(current->x+1,current->y-1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
				if( bDown )
				{
					if( examine = GetAt(current->x+1,current->y+1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
			}

			if( bLeft )
			{
				if( bUp )
				{
					if( examine = GetAt(current->x-1,current->y-1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
				if( bDown )
				{
					if( examine = GetAt(current->x-1,current->y+1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
			}

			oSize = openlist.size();
			if( oSize > 0 )
			{
				current = openlist[0];
				uiOpenAt = 0;
				for( UINT i = 0; i < oSize; i++ )
				{
					if( openlist[i]->g < current->g )
					{
						current = openlist[i];
						uiOpenAt = i;
					}
				}
			}
			else current = NULL;

			if( dwDelayTime )
			{
				PeekRender();
				Sleep(dwDelayTime);
				if( GetAsyncKeyState(VK_END) & 0xf000 ) dwDelayTime = 0;
			}
		}
		
		break;
	case METHOD_HEURISTIC:
		/* Use heuristic score only for searching in algorithm */
		while( current )
		{
			if( current->g > dwHighestGScore ) dwHighestGScore = current->g;

			current->state = 2;
			openlist.erase(openlist.begin()+uiOpenAt);

			if( current->x == X2 && current->y == Y2 )
			{
				NODE* parent = current->parent;
				while( parent )
				{
					parent->state = 4;
					parent = parent->parent;
				}
				dwPathGScore = current->g;
				current = 0;
				break;
			}

			bLeft = bRight = bUp = bDown = false;

			if( examine = GetAt(current->x+1,current->y) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bRight = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bRight = true;
					}
				}
			}

			if( examine = GetAt(current->x,current->y-1) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bUp = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bUp = true;
					}
				}
			}

			if( examine = GetAt(current->x-1,current->y) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bLeft = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bLeft = true;
					}
				}
			}

			if( examine = GetAt(current->x,current->y+1) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->parent = current;
					openlist.push_back(examine);
					bDown = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->parent = current;
						openlist.push_back(examine);
						bDown = true;
					}
				}
			}

			if( bRight )
			{
				if( bUp )
				{
					if( examine = GetAt(current->x+1,current->y-1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
				if( bDown )
				{
					if( examine = GetAt(current->x+1,current->y+1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
			}

			if( bLeft )
			{
				if( bUp )
				{
					if( examine = GetAt(current->x-1,current->y-1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
				if( bDown )
				{
					if( examine = GetAt(current->x-1,current->y+1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
			}

			oSize = openlist.size();
			if( oSize > 0 )
			{
				current = openlist[0];
				uiOpenAt = 0;
				for( UINT i = 0; i < oSize; i++ )
				{
					if( openlist[i]->h < current->h )
					{
						current = openlist[i];
						uiOpenAt = i;
					}
				}
			}
			else current = NULL;

			if( dwDelayTime )
			{
				PeekRender();
				Sleep(dwDelayTime);
				if( GetAsyncKeyState(VK_END) & 0xf000 ) dwDelayTime = 0;
			}
		}
		break;
	default:
		/* Use the A* algorithm for search */
		while( current )
		{
			if( current->g > dwHighestGScore ) dwHighestGScore = current->g;

			current->state = 2;
			openlist.erase(openlist.begin()+uiOpenAt);

			if( current->x == X2 && current->y == Y2 )
			{
				NODE* parent = current->parent;
				while( parent )
				{
					parent->state = 4;
					parent = parent->parent;
				}
				dwPathGScore = current->g;
				current = 0;
				break;
			}

			bLeft = bRight = bUp = bDown = false;

			if( examine = GetAt(current->x+1,current->y) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->f = examine->g+examine->h;
					examine->parent = current;
					openlist.push_back(examine);
					bRight = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->f = examine->g+examine->h;
						examine->parent = current;
						openlist.push_back(examine);
						bRight = true;
					}
				}
			}

			if( examine = GetAt(current->x,current->y-1) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->f = examine->g+examine->h;
					examine->parent = current;
					openlist.push_back(examine);
					bUp = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->f = examine->g+examine->h;
						examine->parent = current;
						openlist.push_back(examine);
						bUp = true;
					}
				}
			}

			if( examine = GetAt(current->x-1,current->y) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->f = examine->g+examine->h;
					examine->parent = current;
					openlist.push_back(examine);
					bLeft = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->f = examine->g+examine->h;
						examine->parent = current;
						openlist.push_back(examine);
						bLeft = true;
					}
				}
			}

			if( examine = GetAt(current->x,current->y+1) )
			{
				if( examine->state == 0 )
				{
					examine->state = 1;
					examine->g = current->g+10;
					examine->f = examine->g+examine->h;
					examine->parent = current;
					openlist.push_back(examine);
					bDown = true;
				}
				else if( examine->state == 1 || examine->state == 2 )
				{
					if( current->g+10 < examine->g )
					{
						examine->state = 1;
						examine->g = current->g+10;
						examine->f = examine->g+examine->h;
						examine->parent = current;
						openlist.push_back(examine);
						bDown = true;
					}
				}
			}

			if( bRight )
			{
				if( bUp )
				{
					if( examine = GetAt(current->x+1,current->y-1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->f = examine->g+examine->h;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->f = examine->g+examine->h;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
				if( bDown )
				{
					if( examine = GetAt(current->x+1,current->y+1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->f = examine->g+examine->h;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->f = examine->g+examine->h;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
			}

			if( bLeft )
			{
				if( bUp )
				{
					if( examine = GetAt(current->x-1,current->y-1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->f = examine->g+examine->h;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->f = examine->g+examine->h;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
				if( bDown )
				{
					if( examine = GetAt(current->x-1,current->y+1) )
					{
						if( examine->state == 0 )
						{
							examine->state = 1;
							examine->g = current->g+14;
							examine->f = examine->g+examine->h;
							examine->parent = current;
							openlist.push_back(examine);
						}
						else if( examine->state == 1 || examine->state == 2 )
						{
							if( current->g+14 < examine->g )
							{
								examine->state = 1;
								examine->g = current->g+14;
								examine->f = examine->g+examine->h;
								examine->parent = current;
								openlist.push_back(examine);
							}
						}
					}
				}
			}

			oSize = openlist.size();
			if( oSize > 0 )
			{
				current = openlist[0];
				uiOpenAt = 0;
				for( UINT i = 0; i < oSize; i++ )
				{
					if( openlist[i]->f < current->f )
					{
						current = openlist[i];
						uiOpenAt = i;
					}
				}
			}
			else current = NULL;

			if( dwDelayTime )
			{
				PeekRender();
				Sleep(dwDelayTime);
				if( GetAsyncKeyState(VK_END) & 0xf000 ) dwDelayTime = 0;
			}
		}
		break;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&qw_c);
	dwCompletionTime = DWORD( double(qw_c-qw_t)/dbl_f );

	bOtherNodeData = bOldOtherNodeData;

	swprintf_s( (wchar_t*)GlobalString, 512, L"G Score: %u\nCalculated in %u ms", dwPathGScore, dwCompletionTime );
	SetWindowText( g_txtResults, (wchar_t*)GlobalString );

	return true;
}
bool MAP::Clone(NODE** _out_)
{
	*_out_ = new(std::nothrow) NODE[T];
	if( *_out_ )
	{
		memcpy_s((void*)*_out_,sizeof(NODE)*T,
			(void*)nodes,sizeof(NODE)*T);
		return true;
	}
	return false;
}
bool MAP::Integrate(NODE* _in_, size_t _size)
{		
	UINT _x, _y;
	NODE* cnode;
	for( size_t i = 0; i < _size; i++ )
	{
		_x = _in_[i].x;
		_y = _in_[i].y;

		cnode = GetAt(_x,_y);
		if( cnode ) *cnode = _in_[i];
	}
	return true;
}
void MAP::LoadData()
{
	FILE* data = NULL;
	UINT uiWidth = 0, uiHeight = 0, uiTotal = 0;
	WORD header = 0;

	fopen_s(&data,"Data.dat","rb");
	if( !data ) goto l_ErrorExit;

	if( fread((void*)&header,2,1,data) != 1 ) goto l_ErrorExit;
	if( header != 21313 ) goto l_ErrorExit; // AS

	if( fread((void*)&uiWidth,4,1,data)  != 1 ) goto l_ErrorExit;
	if( fread((void*)&uiHeight,4,1,data) != 1 ) goto l_ErrorExit;
	if( fread((void*)&pStart,sizeof(POINT),1,data) != 1 ) goto l_ErrorExit;
	if( fread((void*)&pEnd,sizeof(POINT),1,data)   != 1 ) goto l_ErrorExit;
	uiTotal = uiWidth * uiHeight;

	if( nodes ) delete[] nodes;
	nodes = new NODE[uiTotal];
	bool readIsWall; for( UINT i = 0; i < uiTotal; i++ )
	{
		if( fread((void*)&readIsWall,sizeof(bool),1,data) != 1 ) goto l_ErrorExit;
		nodes[i].state = readIsWall ? 3 : 0;
		nodes[i].x = i % uiWidth;
		nodes[i].y = i / uiWidth;
	}
	W = uiWidth;
	H = uiHeight;
	T = uiTotal;
	fclose(data);
	UpdateWindowRect();
	Render();
	goto l_Exit;

l_ErrorExit:
	MessageBoxA( NULL, "Failed to load data from file.", "Load error", MB_ICONERROR );
l_Exit:
	if( data ) fclose(data);
}
void MAP::SaveData()
{
	if( !nodes ) return;

	FILE* data = NULL;
	fopen_s(&data,"Data.dat","wb");
	if( !data ) goto l_ErrorExit;
	WORD header = 21313;

	if( fwrite((void*)&header,2,1,data) != 1 ) goto l_ErrorExit;
	if( fwrite((void*)&W,sizeof(UINT),1,data) != 1 ) goto l_ErrorExit;
	if( fwrite((void*)&H,sizeof(UINT),1,data) != 1 ) goto l_ErrorExit;
	if( fwrite((void*)&pStart,sizeof(POINT),1,data) != 1 ) goto l_ErrorExit;
	if( fwrite((void*)&pEnd,sizeof(POINT),1,data) != 1 ) goto l_ErrorExit;

	bool writeIsWall;
	for( UINT i = 0; i < T; i++ ) {
		writeIsWall = nodes[i].state == 3 ? TRUE : FALSE;
		if( fwrite((void*)&writeIsWall,sizeof(bool),1,data) != 1 ) goto l_ErrorExit;
	} goto l_SuccessExit;
l_ErrorExit:
	MessageBoxA(g_hWnd,"Failed to write data to file.","Write error.",MB_ICONERROR);
	goto l_Exit;
l_SuccessExit:
	MessageBoxA(g_hWnd,"Written data successfully to file.","Write success.",MB_ICONINFORMATION);
l_Exit:
	if( data ) fclose(data);
}



DWORD Lerp(DWORD Col1,DWORD Col2,double amount)
{
	WORD wR1, wG1, wB1, wA1;
	WORD wR2, wG2, wB2, wA2;
	WORD wR3, wG3, wB3, wA3;

	wR1 = WORD( (Col1<<24)>>24 );
	wR2 = WORD( (Col2<<24)>>24 );
	wG1 = WORD( (Col1<<16)>>24 );
	wG2 = WORD( (Col2<<16)>>24 );
	wB1 = WORD( (Col1<< 8)>>24 );
	wB2 = WORD( (Col2<< 8)>>24 );
	wA1 = WORD(  Col1     >>24 );
	wA2 = WORD(  Col2     >>24 );

	wR3 = WORD( double(wR2-wR1) * amount + double(wR1) );
	wG3 = WORD( double(wG2-wG1) * amount + double(wG1) );
	wB3 = WORD( double(wB2-wB1) * amount + double(wB1) );
	wA3 = WORD( double(wA2-wA1) * amount + double(wA1) );

	return wR3|(wG3<<8)|(wB3<<16)|(wA3<<24);
}

