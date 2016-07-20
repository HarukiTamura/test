/*******************************************************************************
* タイトル	:入力.c
* ファイル名:cursor.c
* 作成者	:田村春樹
* 作成日	:2015/06/10
********************************************************************************
* 更新履歴	ver 1.00
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
//キーボード用
#define MAX_KEY_INPUT_NUM	( 256 )

//マウス用
#define MAX_MOUSE_INPUT_NUM ( 8 )

//ゲームパッド（ジョイスティック用）
#define USE_GAMEPAD_NUM		( 1 )
#define GAMEPAD_ANGLE_NUM	( 4 )
#define GAMEPAD_BUTTON_NUM	( 32 )

#define KEY_ON	( 0x80 )
#define KEY_OFF	( 0x00 )

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
//ゲームパッド用コールバック関数
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance , VOID *pContext );
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* , VOID* );

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static LPDIRECTINPUT8 g_pInput = NULL;		/*DirectInputオブジェクトへのポインタ*/
static LPDIRECTINPUTDEVICE8 g_pDevKeyboard = NULL;	/*入力デバイス（キーボード）へのポインタ*/	
static LPDIRECTINPUTDEVICE8 g_pDevMouse = NULL;		/*入力デバイス（マウス）へのポインタ*/	
static LPDIRECTINPUTDEVICE8 g_pDevGamePad = NULL;	/*入力デバイス（ゲームパッド）へのポインタ*/

/*キーボードの入力情報ワーク*/
static BYTE g_aKeyState[MAX_KEY_INPUT_NUM];	/*Press*/
static BYTE g_aKeyStateTrigger[MAX_KEY_INPUT_NUM];	/*Trigger*/
static BYTE g_aKeyStateRelease[MAX_KEY_INPUT_NUM];	/*Release*/
static BYTE g_aKeyStateRepeat[MAX_KEY_INPUT_NUM];	/*Repeat */
static int g_aKeyStateRepeatCount[MAX_KEY_INPUT_NUM];	/*キーリピートカウンタ*/

/*マウスの入力情報ワーク*/
static LONG g_mouseLocateX;
static LONG g_mouseLocateY;
static LONG g_mouseWheelState;
static BYTE g_aMouseState[MAX_MOUSE_INPUT_NUM];
static BYTE g_aMouseStateTrigger[MAX_MOUSE_INPUT_NUM];
static BYTE g_aMouseStateRelease[MAX_MOUSE_INPUT_NUM];

/*ゲームパッドの入力情報ワーク*/
static DIDEVCAPS g_diGamePadCaps[USE_GAMEPAD_NUM];		/*デバイスのキャパシティ*/
static DIJOYSTATE g_aGamePadState[USE_GAMEPAD_NUM];			/*Press*/
static DIJOYSTATE g_aGamePadStateTrigger[USE_GAMEPAD_NUM];	/*Trigger*/
static DIJOYSTATE g_aGamePadStateRelease[USE_GAMEPAD_NUM];	/*Release*/
static DIJOYSTATE g_aGamePadStateRepeat[USE_GAMEPAD_NUM];	/*Repeat*/
static int g_aGamePadStateRepeatCount[USE_GAMEPAD_NUM][GAMEPAD_BUTTON_NUM];	/*キーリピートカウンタ*/

/*スティック方向簡易取得*/
static bool g_aGamePadInputAngle[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];	
static bool g_aGamePadInputAngleTrigger[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];
static bool g_aGamePadInputAngleRelease[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];
static bool g_aGamePadInputAngleRepeat[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];
static int g_aGamePadInputAngleRepeatCount[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];	/*キーリピートカウンタ*/
static bool g_pushAnyButton[USE_GAMEPAD_NUM];

/*なんらかの操作確認フラグ*/
static bool g_pushAnykey;
static int g_nKeyRepeat = DEFAULT_KEY_REPEAT_FPS;

/*******************************************************************************
* 関数名	:HRESULT InitInput( void )
*		入力初期化関数です。
* 引数		:HINSTANCE hInstance	インスタンスのハンドル
*			 HWND hWnd				ウィンドウのハンドル
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitInput( HINSTANCE hInstance , HWND hWnd )
{
	g_nKeyRepeat = DEFAULT_KEY_REPEAT_FPS;
	if( g_pInput == NULL )
	{
		if( FAILED( DirectInput8Create( hInstance , DIRECTINPUT_VERSION , IID_IDirectInput8 , ( void** )&g_pInput , NULL ) ) )
		{
			Error("入力デバイスの生成に失敗しました。");
			return E_FAIL;
		}
	}
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitInput( void )
*		入力終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitInput( void )
{
	if( g_pInput != NULL )
	{
		g_pInput->Release();
		g_pInput = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateInput( void )
*		入力更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void UpdateInput( void )
{
	g_pushAnykey = false;
}


/* ここから　キーボード */

/*******************************************************************************
* 関数名	:HRESULT InitKeyboard( HINSTANCE hInstance , HWND hWnd )
*		キーボード入力の初期化関数です。
* 引数		:HINSTANCE hInstance	インスタンスのハンドル
*			 HWND hWnd				ウィンドウのハンドル
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitKeyboard( HINSTANCE hInstance , HWND hWnd )
{
	if( FAILED( InitInput( hInstance , hWnd ) ) )
	{
		Error("入力デバイスの生成に失敗しました。（キーボードからの呼び出し）");
		return E_FAIL;
	}
	if( FAILED( g_pInput->CreateDevice( GUID_SysKeyboard , &g_pDevKeyboard , NULL ) ) )
	{
		Error("キーボード入力デバイスの生成に失敗しました。");
		return E_FAIL;
	}
	if( FAILED( g_pDevKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
	{
		Error("データフォーマットの設定");
		return E_FAIL;
	}
	if( FAILED( g_pDevKeyboard->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("強調モードの設定");
		return E_FAIL;
	}
	g_pDevKeyboard->Acquire();	/*アクセス権の取得*/

	memset( &g_aKeyState[0]			, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateTrigger[0]	, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateRelease[0]	, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateRepeat[0]	, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateRepeatCount[0] , 0 , sizeof( int ) * MAX_KEY_INPUT_NUM );

	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitKeyboard( void )
*		キーボード入力終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitKeyboard( void )
{
	if( g_pDevKeyboard != NULL )
	{
		g_pDevKeyboard->Unacquire();	/*アクセス権の解放*/
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateKeyboard( void )
*		キーボード入力更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
void UpdateKeyboard( void )
{
	int nCntKey = 0;
	BYTE aKeyState[MAX_KEY_INPUT_NUM];

	if( SUCCEEDED( g_pDevKeyboard->GetDeviceState( sizeof( aKeyState ) , &aKeyState[0] ) ) )
	{
		for( nCntKey = 0; nCntKey < MAX_KEY_INPUT_NUM; nCntKey++ )
		{
			g_aKeyStateTrigger[nCntKey] = ~g_aKeyState[nCntKey] & aKeyState[nCntKey];
			g_aKeyStateRelease[nCntKey] = g_aKeyState[nCntKey] & ~aKeyState[nCntKey];
			if( g_aKeyStateRepeat[nCntKey] == KEY_ON )
			{
				g_aKeyStateRepeat[nCntKey] = g_aKeyState[nCntKey];
				g_aKeyStateRepeatCount[nCntKey] = 0;
			}
			else
			{
				g_aKeyStateRepeat[nCntKey] = ( ++g_aKeyStateRepeatCount[nCntKey] >= g_nKeyRepeat ? g_aKeyState[nCntKey] : KEY_OFF );
			}
			g_aKeyState[nCntKey] = aKeyState[nCntKey];
			if( aKeyState[nCntKey] == KEY_ON )
			{
				g_pushAnykey = true;
			}
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();	/*アクセス権の取得*/
	}
}

/*******************************************************************************
* 関数名	:bool GetKeyboardPress( int nKey )
*		キーボードからのPress入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardPress( int nKey )
{
	return ( ( g_aKeyState[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetKeyboardTrigger( int nKey )
*		キーボードからのTrigger入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardTrigger( int nKey )
{
	return ( ( g_aKeyStateTrigger[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetKeyboardRelease( int nKey )
*		キーボードからのRelease入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardRelease( int nKey )
{
	return ( ( g_aKeyStateRelease[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetKeyboardRepeat( int nKey )
*		キーボードからのRepeat入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetKeyboardRepeat( int nKey )
{
	return ( ( g_aKeyStateRepeat[nKey] & 0x80 ) ? true : false );
}


/* ここから　マウス */

/*******************************************************************************
* 関数名	:HRESULT InitMouse( HINSTANCE hInstance , HWND hWnd )
*		マウス入力の初期化関数です。
* 引数		:HINSTANCE hInstance	インスタンスのハンドル
*			 HWND hWnd				ウィンドウのハンドル
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitMouse( HINSTANCE hInstance , HWND hWnd )
{
	if( FAILED( InitInput( hInstance , hWnd ) ) )
	{
		Error("入力デバイスの生成に失敗しました。（マウスからの呼び出し）");
		return E_FAIL;
	}
	if( FAILED( g_pInput->CreateDevice( GUID_SysMouse , &g_pDevMouse , NULL ) ) )
	{
		Error("マウス入力デバイスの生成に失敗しました。");
		return E_FAIL;
	}
	if( FAILED( g_pDevMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		Error("データフォーマットの設定");
		return E_FAIL;
	}
	if( FAILED( g_pDevMouse->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("強調モードの設定");
		return E_FAIL;
	}
	g_pDevMouse->Acquire();	/*アクセス権の取得*/

	
	g_mouseLocateX = 0;
	g_mouseLocateY = 0;
	g_mouseWheelState = 0;
	for( int mouseButtonCnt = 0; mouseButtonCnt < MAX_MOUSE_INPUT_NUM; mouseButtonCnt++ )
	{
		g_aMouseState[mouseButtonCnt] = 0;
		g_aMouseStateTrigger[mouseButtonCnt] = 0;
		g_aMouseStateRelease[mouseButtonCnt] = 0;
	}
	return S_OK;
}

/*******************************************************************************
* 関数名	:void UninitMouse( void )
*		マウス入力終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitMouse( void )
{
	if( g_pDevMouse != NULL )
	{
		g_pDevMouse->Unacquire();	/*アクセス権の解放*/
		g_pDevMouse->Release();
		g_pDevMouse = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateMouse( void )
*		マウス入力更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
void UpdateMouse( void )
{
	DIMOUSESTATE2 mouseState;
	BYTE aMouseButtonState[MAX_MOUSE_INPUT_NUM];

	// マウスの状態を取得
	if( SUCCEEDED( g_pDevMouse->GetDeviceState( sizeof( DIMOUSESTATE2 ), &mouseState ) ) )
	{
		for( int mouseButtonCnt = 0; mouseButtonCnt < MAX_MOUSE_INPUT_NUM; mouseButtonCnt++ )
		{
			aMouseButtonState[mouseButtonCnt] = mouseState.rgbButtons[mouseButtonCnt];
			g_aMouseStateTrigger[mouseButtonCnt] = ~g_aMouseState[mouseButtonCnt] & aMouseButtonState[mouseButtonCnt];
			g_aMouseStateRelease[mouseButtonCnt] = g_aMouseState[mouseButtonCnt] & ~aMouseButtonState[mouseButtonCnt];
			g_aMouseState[mouseButtonCnt] = aMouseButtonState[mouseButtonCnt];
			if( aMouseButtonState[mouseButtonCnt] == KEY_ON )
			{
				g_pushAnykey = true;
			}
		}
	}
	else
	{
		g_pDevMouse->Acquire();	/*アクセス権の取得*/
	}
}

/*******************************************************************************
* 関数名	:bool GetMousePress( int nKey )
*		マウスからのPress入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetMousePress( int nKey )
{
	return ( ( g_aMouseState[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetMouseTrigger( int nKey )
*		マウスからのTrigger入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetMouseTrigger( int nKey )
{
	return ( ( g_aMouseStateTrigger[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetMouseRelease( int nKey )
*		マウスからのRelease入力を取得します。
* 引数		:int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetMouseRelease( int nKey )
{
	return ( ( g_aMouseStateRelease[nKey] & 0x80 ) ? true : false );
}

//ゲームパッド入力関数

/*******************************************************************************
* 関数名	:HRESULT InitGamePad( HINSTANCE hInstance , HWND hWnd )
*		ゲームパッド入力の初期化関数です。
* 引数		:HINSTANCE hInstance	インスタンスのハンドル
*			 HWND hWnd				ウィンドウのハンドル
* 戻り値	:HRESULT 関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT InitGamePad( HINSTANCE hInstance , HWND hWnd )
{
	if( FAILED( InitInput( hInstance , hWnd ) ) )
	{
		Error("入力デバイスの生成に失敗しました。（ゲームパッドからの呼び出し）");
		return E_FAIL;
	}
	if( FAILED( g_pInput->EnumDevices( DI8DEVCLASS_GAMECTRL , EnumJoysticksCallback , NULL , DIEDFL_ATTACHEDONLY ) ) )
	{
		Error("入力デバイスの生成に失敗しました。（ジョイスティック）");
		return E_FAIL;
	}
	if( g_pDevGamePad == NULL )
	{
		return S_OK;
	}
	if( FAILED( g_pDevGamePad->SetDataFormat( &c_dfDIJoystick ) ) )
	{
		Error("データフォーマットの設定");
		return E_FAIL;
	}
	if( FAILED( g_pDevGamePad->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("強調モードの設定");
		return E_FAIL;
	}
	g_diGamePadCaps[0].dwSize = sizeof( DIDEVCAPS );	//キャパシティのサイズ取得
	if( FAILED( g_pDevGamePad->GetCapabilities( &g_diGamePadCaps[0] ) ) )
	{
		Error("キャパシティサイズの取得に失敗");
		return E_FAIL;
	}
	if( FAILED( g_pDevGamePad->EnumObjects( EnumAxesCallback , (VOID*)hWnd , DIDFT_AXIS ) ) )
	{
		Error("キャパシティサイズの取得に失敗");
		return E_FAIL;
	}
	if( FAILED( g_pDevGamePad->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("デバイス設定取得に失敗");	//キー範囲の取得など
		return E_FAIL;
	}
	g_pDevGamePad->Acquire();
	
	return S_OK;
}

/*******************************************************************************
* 関数名	:BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance , VOID *pContext )
*		コールバック関数です。
* 引数		:const DIDEVICEINSTANCE *pdidInstance
*			 VOID *pContext
* 戻り値	:BOOL	関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance , VOID *pContext )
{
	if( FAILED( g_pInput->CreateDevice( pdidInstance->guidInstance , &g_pDevGamePad , NULL ) ) )	//コールバック
	{
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}


/*******************************************************************************
* 関数名	:BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE *pdidoi , VOID *pContext )
*		アクセスコールバック関数です。
* 引数		:const DIDEVICEOBJECTINSTANCE *pdidoi
*			 VOID *pContext
* 戻り値	:BOOL	関数の実行結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE *pdidoi , VOID *pContext )
{
	HRESULT     hr;
	DIPROPRANGE diprg;
	
	diprg.diph.dwSize       = sizeof( DIPROPRANGE );
	diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	diprg.diph.dwHow        = DIPH_BYID;
	diprg.diph.dwObj        = pdidoi->dwType;
	diprg.lMin              = 0 - 1000;
	diprg.lMax              = 0 + 1000;
	hr = g_pDevGamePad->SetProperty( DIPROP_RANGE , &diprg.diph );
	
	if ( FAILED( hr ) ) return DIENUM_STOP;
	
	return DIENUM_CONTINUE;
}

/*******************************************************************************
* 関数名	:void UninitGamePad( void )
*		ゲームパッド入力終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void UninitGamePad( void )
{
	if( g_pDevGamePad != NULL )
	{
		g_pDevGamePad->Unacquire();	/*アクセス権の解放*/
		g_pDevGamePad->Release();
		g_pDevGamePad = NULL;
	}
}

/*******************************************************************************
* 関数名	:void UpdateGamePad( void )
*		ゲームパッド入力更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
void UpdateGamePad( void )
{
	DIJOYSTATE aGamePadStateBuff;
	bool g_aGamePadInputAngleBuff[GAMEPAD_ANGLE_NUM];
	if( g_pDevGamePad == NULL )
	{
		return;
	}
	g_pDevGamePad->Poll();

	// ゲームパッドの状態を取得
	if( SUCCEEDED( g_pDevGamePad->GetDeviceState( sizeof( DIJOYSTATE ) , &aGamePadStateBuff ) ) )
	{
		//ここから方向キー簡易用

		//上下左右ボタンが押されている時
		g_aGamePadInputAngleBuff[0] = ( aGamePadStateBuff.lY == -1000 ? true : false );
		g_aGamePadInputAngleBuff[1] = ( aGamePadStateBuff.lY ==  1000 ? true : false );
		g_aGamePadInputAngleBuff[2] = ( aGamePadStateBuff.lX == -1000 ? true : false );
		g_aGamePadInputAngleBuff[3] = ( aGamePadStateBuff.lX ==  1000 ? true : false );
		
		g_aGamePadInputAngleTrigger[0][0] = ( ~g_aGamePadInputAngle[0][0] & g_aGamePadInputAngleBuff[0] ? true : false );
		g_aGamePadInputAngleTrigger[0][1] = ( ~g_aGamePadInputAngle[0][1] & g_aGamePadInputAngleBuff[1] ? true : false );
		g_aGamePadInputAngleTrigger[0][2] = ( ~g_aGamePadInputAngle[0][2] & g_aGamePadInputAngleBuff[2] ? true : false );
		g_aGamePadInputAngleTrigger[0][3] = ( ~g_aGamePadInputAngle[0][3] & g_aGamePadInputAngleBuff[3] ? true : false );

		g_aGamePadInputAngleRelease[0][0] = ( g_aGamePadInputAngle[0][0] & ~g_aGamePadInputAngleBuff[0] ? true : false );
		g_aGamePadInputAngleRelease[0][1] = ( g_aGamePadInputAngle[0][1] & ~g_aGamePadInputAngleBuff[1] ? true : false );
		g_aGamePadInputAngleRelease[0][2] = ( g_aGamePadInputAngle[0][2] & ~g_aGamePadInputAngleBuff[2] ? true : false );
		g_aGamePadInputAngleRelease[0][3] = ( g_aGamePadInputAngle[0][3] & ~g_aGamePadInputAngleBuff[3] ? true : false );

		g_aGamePadInputAngle[0][0] = g_aGamePadInputAngleBuff[0];
		g_aGamePadInputAngle[0][1] = g_aGamePadInputAngleBuff[1];
		g_aGamePadInputAngle[0][2] = g_aGamePadInputAngleBuff[2];
		g_aGamePadInputAngle[0][3] = g_aGamePadInputAngleBuff[3];

		//方向リピート＆何か押したか
		for( int nCntKey = 0; nCntKey < GAMEPAD_ANGLE_NUM; nCntKey++ )
		{
			if( g_aGamePadInputAngleRepeat[0][nCntKey] == true )
			{
				g_aGamePadInputAngleRepeat[0][nCntKey] = g_aGamePadInputAngle[0][nCntKey] ;
				g_aGamePadInputAngleRepeatCount[0][nCntKey] = 0;	/*キーリピートカウンタ*/
			}
			else
			{
				g_aGamePadInputAngleRepeatCount[0][nCntKey] = ( ++g_aGamePadInputAngleRepeatCount[0][nCntKey] >= g_nKeyRepeat ? g_aGamePadInputAngleRepeatCount[0][nCntKey] : KEY_OFF );
			}
			if( g_aGamePadInputAngleBuff[nCntKey] == KEY_ON )
			{
				g_pushAnyButton[0] = true;
			}
		}
		
		g_aGamePadInputAngle[0][0] = g_aGamePadInputAngleBuff[0];
		g_aGamePadInputAngle[0][1] = g_aGamePadInputAngleBuff[1];
		g_aGamePadInputAngle[0][2] = g_aGamePadInputAngleBuff[2];
		g_aGamePadInputAngle[0][3] = g_aGamePadInputAngleBuff[3];

		for( int nCntKey = 0; nCntKey < GAMEPAD_BUTTON_NUM; nCntKey++ )
		{
			g_aGamePadStateTrigger[0].rgbButtons[nCntKey] = ~g_aGamePadState[0].rgbButtons[nCntKey] & aGamePadStateBuff.rgbButtons[nCntKey];
			g_aGamePadStateRelease[0].rgbButtons[nCntKey] = g_aGamePadState[0].rgbButtons[nCntKey] & ~aGamePadStateBuff.rgbButtons[nCntKey];
			if( g_aGamePadStateRepeat[0].rgbButtons[nCntKey] == KEY_ON )
			{
				g_aGamePadStateRepeat[0].rgbButtons[nCntKey] = aGamePadStateBuff.rgbButtons[nCntKey];
				g_aGamePadStateRepeatCount[0][nCntKey] = 0;
			}
			else
			{
				g_aKeyStateRepeat[nCntKey] = ( ++g_aGamePadStateRepeatCount[0][nCntKey] >= g_nKeyRepeat ? aGamePadStateBuff.rgbButtons[nCntKey] : KEY_OFF );
			}
			g_aGamePadState[0].rgbButtons[nCntKey] = aGamePadStateBuff.rgbButtons[nCntKey];
			if( aGamePadStateBuff.rgbButtons[nCntKey] == KEY_ON )
			{
				g_pushAnyButton[0] = true;
			}
		}
	}
	else
	{
		g_pDevGamePad->Acquire();	/*アクセス権の取得*/
	}
}

/*******************************************************************************
* 関数名	:bool GetGamePadPress( int gamePadNum , int nKey )
*		ゲームパッドからのPress入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadPress( int nKey )
{
	return ( ( g_aGamePadState[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//プレイヤー指定版
bool GetGamePadPress( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadState[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetGamePadTrigger( int gamePadNum , int nKey )
*		ゲームパッドからのTrigger入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadTrigger( int nKey )
{
	return ( ( g_aGamePadStateTrigger[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//プレイヤー指定版
bool GetGamePadTrigger( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadStateTrigger[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetGamePadRelease( int gamePadNum , int nKey )
*		ゲームパッドからのRelease入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadRelease( int nKey )
{
	return ( ( g_aGamePadStateRelease[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//プレイヤー指定版
bool GetGamePadRelease( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadStateRelease[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetGamePadRepeat( int gamePadNum , int nKey )
*		ゲームパッドからのRepeat入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 int nKey	所得するキー番号。(DirectInputに定義済み「DIK_XXX」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadRepeat( int nKey )
{
	return ( ( g_aGamePadStateRepeat[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//プレイヤー指定版
bool GetGamePadRepeat( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadStateRepeat[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetGamePadAnglePress( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		ゲームパッドからのPress入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 INPUT_GAMEPAD_ANGLE GpAngle	取得する方向。(Input.hに定義済み「INPUT_GP～」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadAnglePress( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngle[0][GpAngle];
}
//プレイヤー指定版
bool GetGamePadAnglePress( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngle[gamePadNum][GpAngle];
}

/*******************************************************************************
* 関数名	:bool GetGamePadAngleTrigger( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		ゲームパッドからのTrigger入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 INPUT_GAMEPAD_ANGLE GpAngle	取得する方向。(Input.hに定義済み「INPUT_GP～」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadAngleTrigger( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngleTrigger[0][GpAngle];
}
//プレイヤー指定版
bool GetGamePadAngleTrigger( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngleTrigger[gamePadNum][GpAngle];
}

/*******************************************************************************
* 関数名	:bool GetGamePadAngleRelease( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		ゲームパッドからのRelease入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 INPUT_GAMEPAD_ANGLE GpAngle	取得する方向。(Input.hに定義済み「INPUT_GP～」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadAngleRelease( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRelease[0][GpAngle] & 0x80 ) ? true : false );
}
//プレイヤー指定版
bool GetGamePadAngleRelease( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRelease[gamePadNum][GpAngle] & 0x80 ) ? true : false );
}


/*******************************************************************************
* 関数名	:bool GetGamePadAngleRepeat( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		ゲームパッドからのRepeat入力を取得します。
* 引数		:int gamePadNum	ゲームパッドの番号
*			 INPUT_GAMEPAD_ANGLE GpAngle	取得する方向。(Input.hに定義済み「INPUT_GP～」)
* 戻り値	:bool	true:有効	false:無効
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetGamePadAngleRepeat( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRepeat[0][GpAngle] & 0x80 ) ? true : false );
}
//プレイヤー指定版
bool GetGamePadAngleRepeat( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRepeat[gamePadNum][GpAngle] & 0x80 ) ? true : false );
}

/*******************************************************************************
* 関数名	:bool GetAnyButtonPress( int gamePadNum )
*		指定ゲームパッドから何かしらの入力がされたかを返します。
* 引数		:int gamePadNum	ゲームパッドの番号
* 戻り値	:bool	true:入力あり	false:入力なし
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetAnyButtonPress( int gamePadNum )
{
	return g_pushAnyButton[gamePadNum];
}

//汎用入力関数

/*******************************************************************************
* 関数名	:bool GetAnykeyPress( void )
*		何かしらの入力がされたかを返します。
* 引数		:void	引数無し
* 戻り値	:bool	true:入力あり	false:入力なし
* 説明		:処理周回毎の処理内容を記述します。
*******************************************************************************/
bool GetAnykeyPress( void )
{
	return g_pushAnykey;
}

/*******************************************************************************
* 関数名	:void setRepeatFps( int setNewFps )
*		リピート入力の取得タイミングを調整します。
* 引数		:int	setNewFps	新規設定値（空白・マイナスの値の場合はデフォルト）
* 戻り値	:void	戻り値なし
* 説明		:リピート入力の取得タイミングを調整します。
*******************************************************************************/
void setRepeatFps( int setNewFps )
{
	if( setNewFps > 0 )
	{
		g_nKeyRepeat = setNewFps;
	}
	else
	{
		g_nKeyRepeat = DEFAULT_KEY_REPEAT_FPS; 
	}
}
void setRepeatFps( void )
{
	g_nKeyRepeat = DEFAULT_KEY_REPEAT_FPS; 
}
