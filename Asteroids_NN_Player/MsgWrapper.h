
#include "stdafx.h"

class CMsgWrapper : public _OVERLAPPED 
{
   public: // creation
       CMsgWrapper( /*CWnd **/HWND w, UINT m, WPARAM wP, LPARAM lP);
       /* non-virtual */ ~CMsgWrapper() {};// { /*...your code here...*/ };
       static BOOL Init();
       static void Close();

   public:  // methods
       BOOL Forward(); 
       static CMsgWrapper * GetQueuedMessage();
       static BOOL Post( /*CWnd **/HWND wnd, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0);

   protected:
       static HANDLE messagePort;

   public:
       HWND wnd; //CWnd * wnd;
       UINT msg;
       WPARAM wParam;
       LPARAM lParam;
};