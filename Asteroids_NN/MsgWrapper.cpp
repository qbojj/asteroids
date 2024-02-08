
#include "stdafx.h"

#include "MsgWrapper.h"

/* static */ HANDLE CMsgWrapper::messagePort = NULL;

/****************************************************************************
*                           CMsgWrapper::CMsgWrapper
* Inputs:
*	HWND w: Window to which message is to be posted
*	UINT m: Message ID
*	WPARAM wP: message parameter
*	LPARAM lP: message parameter
* Effect: 
* 	Constructor
****************************************************************************/
CMsgWrapper::CMsgWrapper(HWND w, UINT m, WPARAM wP, LPARAM lP)
{
	hEvent = NULL;
	wnd = w;
	msg = m;
	wParam = wP;
	lParam = lP;
}


/****************************************************************************
*                            CMsgWrapper::Init
* Result: BOOL
*       TRUE if successful
*		FALSE if error
* Effect: 
*       Initializes the static members of the class
* Notes:
*	An attempt to initialize more than once returns FALSE with ::GetLastError
*	returning ERROR_INVALID_HANDLE
****************************************************************************/
/* static */ BOOL CMsgWrapper::Init()
{
	ASSERT(messagePort == NULL);
	if(messagePort != NULL)
	{/* already exists */
		::SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	} /* already exists */

	messagePort = CreateIoCompletionPort(	INVALID_HANDLE_VALUE, // no file 
											NULL, // create it 
											0, // no key 
											1); // max threads

	ASSERT(messagePort != NULL); 
	return messagePort != NULL;
}

/****************************************************************************
*                           CMsgWrapper::Close
* Result: void
* 	 
* Effect: 
* 	Closes the handle
****************************************************************************/
/* static */ void CMsgWrapper::Close()
{
	if(messagePort != NULL) ::CloseHandle( messagePort );

	messagePort = NULL;
}

/****************************************************************************
*                           CMsgWrapper::GetQueuedMessage
* Result: CMsgWrapper *
* 	NULL if the queue is empty 
* Effect: 
* 	Puts up wait cursor while doc/view is being created
****************************************************************************/
/* static */ CMsgWrapper * CMsgWrapper::GetQueuedMessage()
{
	ASSERT( messagePort != NULL );

	if( messagePort == NULL )
		return NULL; // nothing to handle,  no port exists

	DWORD bytesRead;	// ignored 
	DWORD key;			// ignored 
	LPOVERLAPPED ovl;	// CMsgWrapper

	if( !GetQueuedCompletionStatus(	messagePort, &bytesRead, (PULONG_PTR)&key, &ovl, 0 ) )			
	{
		// no more data
		return NULL;
	}

	return (CMsgWrapper *)ovl;
}

/****************************************************************************
*                           CMsgWrapper::Post
* Inputs:
*   HWND wnd: Window to which message is to be sent
*	UINT m: Message to send
*	WPARAM wParam: message parameter
*	LPARAM lParam: message parameter
* Result: BOOL
* 	TRUE if successful
*	FALSE if error 
* Effect: 
* 	Puts the message in the queue
****************************************************************************/
/* static */ BOOL CMsgWrapper::Post(HWND wnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	ASSERT( messagePort != NULL );
	if( messagePort == NULL ) return FALSE;

	ASSERT( wnd != NULL ); // must not be NULL! Must be a real window!

	CMsgWrapper * msg = new CMsgWrapper(wnd, m, wParam, lParam);

	return PostQueuedCompletionStatus(	messagePort,
										0, // bytes transferred
										0, 
										msg);     
}

/****************************************************************************
*                           CMsgWrapper::Forward
* Result: BOOL
* 	TRUE if successful
*	FALSE if error
* Effect: 
* 	Forwards the message to the target and deletes it
****************************************************************************/
BOOL CMsgWrapper::Forward()
{
	return ::PostMessage( wnd, msg, wParam, lParam);
}
