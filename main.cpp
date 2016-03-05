#include "stdafx.h"
#include "main.h"
#include "AppDelegate.h"
#include "cocos2d.h"
#include "resource.h"
#include "CocosUI/CCUI_Utility.h"

#pragma   comment(lib,   "comsuppw.lib")  //_com_util::ConvertBSTRToString
#include <comutil.h>

#include "MiniDump.h"


#include "vld/include/vld.h"
#pragma comment(lib,"vld/lib/Win32/vld.lib")

INT_PTR CALLBACK	SelectServer(HWND, UINT, WPARAM, LPARAM);


extern std::string g_strServerIp;
extern int g_nServerPort;
extern unsigned long long g_nPlayerID;
extern bool g_bIsClientView;

// uncomment below line, open debug console
// #define USE_WIN32_CONSOLE

//#ifdef USE_WIN32_CONSOLE
//#undef USE_WIN32_CONSOLE
//#endif

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
	CMiniDump dump;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

	CCLOG("*** star main");

    LPWSTR *szArgList;
    int argCount;
    szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (szArgList != NULL && argCount > 2)
    {
        char *ip = _com_util::ConvertBSTRToString(szArgList[1]);
        g_strServerIp = ip;
        free(ip);

        char *port = _com_util::ConvertBSTRToString(szArgList[2]);
        g_nServerPort = atoi(port);
        free(port);

		if (argCount >= 4)
		{
			char *id = _com_util::ConvertBSTRToString(szArgList[3]);
			g_nPlayerID = atoi(id);
			free(id);
		}      

        g_bIsClientView = false;
    }
	else
    {
        
        g_strServerIp = "10.0.19.8";
        g_nServerPort = 13101;
        g_nPlayerID = 0;
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_SELECT_SERVER), NULL, SelectServer);
	}//end if
    
	LocalFree(szArgList);

    // create the application instance
    AppDelegate app;
    int nRet = Application::getInstance()->run();

    FreeConsole();

    return nRet;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK SelectServer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
        {
            HWND hIp = GetDlgItem(hDlg, IDC_COMBO_IP);
            SendMessageA(hIp, CB_ADDSTRING, 0, (LPARAM)("10.0.19.8(内网服务器)"));
            std::string strIp = CCUserDefault::getInstance()->getStringForKey("ServerIP", "10.0.19.22(内网服务器)");
            SetWindowTextA(hIp, strIp.c_str());
            HWND hId = GetDlgItem(hDlg, IDC_COMBO_ID);
            std::string strID = CCUserDefault::getInstance()->getStringForKey("PlayerID", "0");
            SetWindowTextA(hId, strID.c_str());

            HWND hView = GetDlgItem(hDlg, IDC_CHECK_VIEW);
            g_bIsClientView = CCUserDefault::getInstance()->getBoolForKey("ClientView", true);
            CheckDlgButton(hDlg, IDC_CHECK_VIEW, g_bIsClientView ? BST_CHECKED : BST_UNCHECKED);

            return (INT_PTR)TRUE;
        }
        break;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                HWND hIp = GetDlgItem(hDlg, IDC_COMBO_IP);
                CHAR szIp[MAX_PATH];
                ::GetWindowTextA(hIp, szIp, MAX_PATH);
                std::string strIp = szIp;
                int nIndex = strIp.find("(");
                if (nIndex != -1)
                {
                    strIp = strIp.substr(0, nIndex);
                }
                g_strServerIp = strIp;
                CCUserDefault::getInstance()->setStringForKey("ServerIP", szIp);
                if (strIp == "10.0.19.22" || strIp == "120.33.34.2")
                {
                    srand(time(NULL));
                    g_nServerPort = 13101 + std::rand() % 2;
                }

                HWND hID = GetDlgItem(hDlg, IDC_COMBO_ID);
                CHAR szID[MAX_PATH];
                ::GetWindowTextA(hID, szID, MAX_PATH);
                g_nPlayerID = atoi(szID);
                CCUserDefault::getInstance()->setStringForKey("PlayerID", szID);
                
                g_bIsClientView = IsDlgButtonChecked(hDlg, IDC_CHECK_VIEW);
                CCUserDefault::getInstance()->setBoolForKey("ClientView", g_bIsClientView);

                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}
