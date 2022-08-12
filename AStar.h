#pragma once

#define _CRTDBG_MAP_ALLOC
#include <Windows.h>
#include <stdlib.h>
#include <crtdbg.h>
#include "resource.h"
#include <vector>

#pragma warning(disable:4996)
#pragma warning(disable:4102)



#define MAX_LOADSTRING		100
#define METHOD_ASTAR		0
#define METHOD_DIJKSTRA		1
#define METHOD_HEURISTIC	2



/* Global Variables */
HINSTANCE	hInst;
LPTSTR		szTitle = TEXT("Path finder.");
LPTSTR		szWindowClass = TEXT("ASTAR");

char		GlobalString[1024];

RECT		g_WindowRect;
RECT		g_ClientRect;
RECT		g_MapRect;
RECT		g_PanelRect;

HWND		g_hWnd				= 0;
HWND		g_txtResults		= 0;
HWND		g_BClearMap			= 0;
HWND		g_BOpenMap			= 0;
HWND		g_BSaveMap			= 0;
HWND		g_BRandomMap		= 0;
HWND		g_BGenerateMaze		= 0;
HWND		g_BToggleDisplay	= 0;
HWND		g_CBMethod			= 0;

HWND		g_MapSettings		= 0;
HWND		g_TBWidth			= 0;
HWND		g_TBHeight			= 0;
HWND		g_TBDelayTime		= 0;
HWND		g_BApply			= 0;

HLOCAL		g_htxtWidth			= 0;
HLOCAL		g_htxtHeight		= 0;

HDC			g_hdcBackBuffer		= 0;
HBITMAP		g_hbmBackBuffer		= 0;
HDC			g_hdcFrontBuffer	= 0;
LPVOID		g_hdcBackBuffer_or	= 0;

POINT		pStart				= {0,0};
POINT		pEnd				= {0,0};

CHAR		bCreatingWall		= 0;
bool		bOtherNodeData		= 0;
DWORD		dwDelayTime			= 0;
DWORD		dwCompletionTime	= 0;
DWORD		dwHighestGScore		= 0;
DWORD		dwPathGScore		= 0;
BYTE		bMethod				= METHOD_ASTAR;



/* Function declarations */
void Render();
void PeekRender();
DWORD Lerp(DWORD Col1,DWORD Col2,double amount);



/* Structures and classes */
struct NODE
{
	UINT f, g, h,
		x, y;
	BYTE state;
	NODE* parent;
};
class NodeStack
{
public:
	NodeStack();
	~NodeStack();

	bool IsEmpty();
	NODE* GetAt(DWORD dwElem);
	NODE* GetFirst();
	NODE* GetLast();
	NODE* GetRandom();
	bool Push(NODE *pNode);
	bool Pop(DWORD dwElem);
	bool Pop(NODE *pNode);
	bool Reverse();
	bool Compact();
	bool Echo();

private:

	bool Expand();

	NODE **pNodes;
	DWORD dwPoolSize;
	DWORD dwLast;
};
class MAP
{
protected:
	UINT W, H, T;
	FLOAT DW, DH;
	NODE* nodes;

public:
	MAP() : W(0), H(0), T(0), nodes(0),
		DW(0.0f), DH(0.0f) { }
	~MAP() { if( nodes ) delete[] nodes; }

	void GetDimensions(UINT *out);
	void UpdateWindowRect();
	bool CreateMap(UINT Width, UINT Height);
	void DestroyMap();
	NODE* GetAt(UINT X, UINT Y);
	NODE* GetAt(UINT At);
	NODE* GetFast(UINT X, UINT Y);
	NODE* MousePosToNode();
	BOOL GetNodesUnderCursor(LPPOINT pPrev, LPPOINT pNow, void (__stdcall *foo)(NODE*));
	bool HasNodes();
	void Draw(LPPOINT _start, LPPOINT _end);
	void Refresh();
	void GenerateRandom();
	void GenerateMaze(LPPOINT start);
	bool ComputePath(UINT X1, UINT Y1, UINT X2, UINT Y2);
	bool Clone(NODE** _out_);
	bool Integrate(NODE* _in_, size_t _size);
	void LoadData();
	void SaveData();

} Map;

