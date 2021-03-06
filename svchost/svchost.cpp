// svchost.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "扩展函数.h"
#include "功能函数.h"
#include "寻路管理.h"
#include "鼠键函数.h"
#include "Asm.h"
#include "钩子管理.h"
#include "读写函数.h"

HWND hWnd;
DWORD dwProcessId;
HANDLE hProcess;
屏幕分辨率 _屏幕分辨率;

BOOL 自动开关 = false;

HHOOK g_hHook = NULL;
DWORD g_dwMainThreadId;
// 打开游戏进程 0成功 1 未找到窗口 2打开进程失败 
INT OpenGameProcess()
{
	//hWnd = FindWindow(L"地下城与勇士", L"地下城与勇士");
	hWnd = FindWindow(L"YodaoMainWndClass",L"网易有道词典");
	if (!hWnd) {
		printf("未找到窗口\n");
		return 1;
	}
	else {
		printf("已找到窗口 句柄=%x\n",(int)hWnd);
	}
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (!dwProcessId) {
		printf("进程ID获取错误\n");
	}
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessId);
	if (!hProcess) {
		printf("打开进程失败 error code <%d>\n", GetLastError());
		return 2;
	}
	else {
		printf("打开进程 成功\n");
	}
	return 0;
}

LRESULT CALLBACK 热键响应(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT* p = (CWPSTRUCT*)lParam;
	if (nCode == HC_ACTION)
	{
		switch (wParam) //wParam中是消息ID
		{
			// 键盘消息
			case WM_KEYDOWN:
			{
				switch (p->lParam)
				{
				case VK_HOME:
					自动开关 = !自动开关;
					坐标CALL(readInteger(人物基址),100,100,0);
					break;
				default:
					break;
				}
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL WINAPI ConsoleCtrlhandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		UnhookWindowsHookEx(g_hHook);
		PostThreadMessage(g_dwMainThreadId, WM_QUIT, 0, 0);
	}
	return TRUE;
}

VOID 全局键盘钩子开始()
{
	g_dwMainThreadId = GetCurrentThreadId();
	SetConsoleCtrlHandler(ConsoleCtrlhandler, TRUE);
	g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, 热键响应, GetModuleHandle(NULL), NULL);
	if (g_hHook == NULL)
		printf("hook failure!");
}

VOID 全局键盘钩子结束()
{
	SetConsoleCtrlHandler(ConsoleCtrlhandler, FALSE);
} 

VOID 窗口初始化()
{
	int width = 350;
	int height = 400;
	HWND conslehWnd = GetConsoleWindow();
	MoveWindow(conslehWnd, GetSystemMetrics(SM_CXFULLSCREEN)- width, GetSystemMetrics(SM_CYFULLSCREEN)- height, width, height,true);
	SetConsoleTitle(L"C:\\Windows\\system32\\svchost.exe");
}

BOOL 程序开始()
{
	if (!删除自身()) {
		printf("删除自身 失败\n");
	}
	/*窗口初始化();
	全局键盘钩子开始();
	if (OpenGameProcess() != 0) {
		return false;
	}*/
	//HookWindowsMsg();
	return true;
}

VOID 程序结束()
{
	//删除自身();
	全局键盘钩子结束();
}


int main()
{
	if (程序开始()) {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else {
		PAUSE;
	}
	程序结束();
	return 0;
}

