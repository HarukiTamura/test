/*******************************************************************************
* タイトル	:課題04「ランニングマン」
* ファイル名:main.cpp
* 作成者	:田村春樹
* 作成日	:2015/05/29
********************************************************************************
* 更新履歴	ver 1.00	基本部分の完成
*			ver 1.10	スクロール背景の追加
*			ver 1.20	回る章先生を追加
*			ver 1.30	ミサイル発射システムを追加
*			ver 1.40	エフェクトを追加
*			ver 1.42	エフェクトを世界観に合わせ修正
*			ver 1.44	章先生の画像を使用していた物をカーソルに変更
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "input.h"
#include "fade.h"

#include "sound.h"
#include "soundGraphic.h"

//モードごとの読み込み
#include "title.h"
#include "menu.h"
#include "game.h"
#include "result.h"
#include "afResult.h"
#include "afGame.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
LRESULT CALLBACK Wndproc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam );

/*DirectX*/
HRESULT Init( HINSTANCE hInstance , HWND hWnd , bool bWindow );
void Uninit( void );
void Update( void );
void Draw( void );

/*デバイス復旧*/
HRESULT DeviceRescue( HINSTANCE hInstance , HWND hWnd , bool bWindow  );

/*WindowsAPI*/
void Error( char *errorTxt );

/*FPS表示*/
void DrawFPS( void );

/*******************************************************************************
* グローバル変数
*******************************************************************************/
LPDIRECT3D9 g_pD3D = NULL;			/*Direct3Dオブジェクトポインタ*/
LPDIRECT3DDEVICE9 g_pD3DDevice;		/*Direct3Dデバイスへのポインタ*/
LPD3DXFONT g_pFont;		/*フォントへのポインタ*/
int g_nCountFPS;	/*FPSカウンター*/

MODE g_mode = MODE_TITLE;	/*現在のモード*/

HWND hWndMiller;	/*アラート表示関数簡易化用のミラー*/
HINSTANCE *g_phInstance;
D3DPRESENT_PARAMETERS *g_pd3dpp;
/*******************************************************************************
* 関数名	:int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int hCmdShow )
*		メイン関数です。
* 引数		:HINSTANCE hInstance		
*			 HINSTANCE hPrevInstance	
*			 LPSTR lpCmdLine			
*			 int hCmdShow 
* 戻り値	:正常終了:0
* 説明		:プログラムの基幹となるメイン関数です。
*******************************************************************************/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	DWORD dwFrameCount;
	DWORD dwCurrentTime;
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	
	g_phInstance = &hInstance;

	/*ウィンドウサイズ規定*/
	RECT windowSizeRect = { 0 , 0 , WINDOW_WIDTH , WINDOW_HEIGHT };
	/*ウィンドウサイズ調整*/
	AdjustWindowRect( &windowSizeRect , WS_OVERLAPPEDWINDOW , false );

	WNDCLASSEX wcex =
	{
		sizeof( WNDCLASSEX ),	/*メモリサイズ*/
		CS_CLASSDC,				/*ウィンドウスタイル*/
		Wndproc,				/*Wndproc関数のアドレス*/
		0,				/*未使用*/
		0,				/*未使用*/
		hInstance,				/*インスタンスハンドル*/
		NULL,					/*使用するアイコン指定*/
		LoadCursor( NULL , IDC_ARROW ),		/*マウスカーソル指定*/
		( HBRUSH )( COLOR_WINDOW + 1 ),		/*クライアント領域背景指定*/
		NULL,								/*メニュー設定*/
		"Window001",			/*ウィンドウクラス名*/
		NULL					/*縮小アイコン情報*/
	};

	HWND hWnd;	//ウィンドウハンドル
	MSG msg;	//メッセージ用変数

	RegisterClassEx( &wcex );	/*ウィンドウクラス登録*/
	
	hWnd = CreateWindowEx( 
		0,				/*未使用*/
		"Window001",	/*ウィンドウクラス名*/
		"AIriSphere",	/*ウィンドウ表示名*/
		WS_OVERLAPPEDWINDOW,	/*ウィンドウスタイル*/
		CW_USEDEFAULT,	/*左上Ｘ*/
		CW_USEDEFAULT,	/*左上Ｙ*/
		windowSizeRect.right - windowSizeRect.left,	/*幅*/
		windowSizeRect.bottom - windowSizeRect.top,	/*高さ*/
		NULL,	/*親ウィンドウのハンドル*/
		NULL,	/*メニュー又は子ウィンドウID*/
		hInstance,
		NULL	/*ウィンドウ作成データ*/
	);

	/*初期化処理*/
	if( FAILED( Init( hInstance , hWnd , SCREEN_TYPE ) ) )
	{
		/*エラー処理*/
		Error( "初期化に失敗しました。\nプログラムを終了します。" );
		return -1;
	}

	/*ここからFPS設定*/
	timeBeginPeriod( 1 );	/*分解能を設定*/
	
	/*必要数値の代入*/
	dwFrameCount = 
	dwCurrentTime = 0;
	dwExecLastTime =
	dwFPSLastTime = timeGetTime();
	
	/*ここまでFPS設定*/

	ShowWindow( hWnd , nCmdShow );	/*ウィンドウ生成*/
	UpdateWindow( hWnd );			/*ウィンドウ表示アップデート*/
	
	hWndMiller = hWnd;	/*ミラーへの複製*/
	
	while( 1 )
	{
		if( PeekMessage( &msg , NULL , 0 , 0 , PM_REMOVE ) != 0 )
		{
			if( msg.message == WM_QUIT )
			{
				break;
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if( ( dwCurrentTime - dwFPSLastTime ) >= 500 )
			{
				g_nCountFPS = ( dwFrameCount * 1000 ) / ( dwCurrentTime - dwFPSLastTime );
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			if( ( dwCurrentTime - dwExecLastTime ) >= ( 1000 / FPS_RATE ) )
			{
				dwExecLastTime = dwCurrentTime;
				Update();	/*更新処理*/
				Draw();		/*描画処理*/
				dwFrameCount++;
			}
			else
			{
				Sleep(( 1000 / FPS_RATE ) - ( dwCurrentTime - dwExecLastTime ));
			}
		}
	}

	timeEndPeriod( 1 );	/*分解能を戻す*/

	/*ウィンドウクラスの解放*/
	UnregisterClass( "Window001" , wcex.hInstance );
	/*終了処理*/
	Uninit();
	return ( int )msg.wParam;
}

/*******************************************************************************
* 関数名	:LRESULT CALLBACK Wndproc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
*		ウィンドウプロシージャの処理。
* 引数		:HWND hWnd		ウィンドウハンドル
*			 UINT uMsg		メッセージ
*			 WPARAM wParam	パラメータ
*			 LPARAM lParam	パラメータ
* 戻り値	:正常終了:0
* 説明		:ウィンドウプロシージャに関する処理を行います。
*******************************************************************************/
LRESULT CALLBACK Wndproc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
{
	int nID;
	switch( uMsg )
	{
		case WM_CREATE:/*ウィンドウ生成時*/
			{
				//PlaySound( "./data/BGM/free0602.wav" , NULL , SND_FILENAME | SND_ASYNC | SND_LOOP );
				break;
			}
		case WM_CLOSE:/*×ボタン受諾*/
			{
				nID = MessageBox( hWnd ,"アプリケーションを終了しますか。","終了確認", MB_YESNO );
				if( nID == IDYES )
				{
					Uninit();
					DestroyWindow( hWnd );	/*ウィンドウを破棄する*/
				}
				else
				{
					uMsg = 0;
				}
				break;
			}
		case WM_DESTROY:	/*ウィンドウを破棄*/
			{	
				Uninit();
				PostQuitMessage( 0 );
				break;
			}
		case WM_PAINT:/*描画されたら*/
			{
				break;
			}
		case WM_KEYDOWN:
			{
				switch( wParam )
				{

					case VK_ESCAPE:
						nID = MessageBox( hWnd ,"アプリケーションを終了しますか。","終了確認", MB_YESNO );
						if( nID == IDYES )
						{
							Uninit();
							DestroyWindow( hWnd );	/*ウィンドウを破棄する*/
						}
						break;
				}
			}
			break;
	}
	return DefWindowProc( hWnd , uMsg , wParam , lParam );
}

/*******************************************************************************
* 関数名	:HRESULT Init( HINSTANCE hInstance , HWND hWnd , bool bWindow  )
*		全体初期化関数です。
* 引数		:HINSTANCE hInstance	インスタンスのハンドル
*			 HWND hWnd				ウィンドウのハンドル
*			 bool bWindow			ウィンドウをフルスクリーンにするかどうか
* 戻り値	:HRESULT	関数の動作結果
* 説明		:変数等の初期化を実行する起動毎に一度のみ動作する関数です。
*******************************************************************************/
HRESULT Init( HINSTANCE hInstance , HWND hWnd , bool bWindow )
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;	/*パソコン側のディスプレイデータ*/

	/*Direct3Dオブジェクトの生成*/
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( g_pD3D == NULL )	/*エラー処理*/
	{
		Error("Direct3Dオブジェクトの生成に失敗しました。\nInit g_pD3D Error");
		return E_FAIL;
	}
	/*ディスプレイモードの取得*/
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm ) ) )
	{
		Error("ディスプレイモードの取得に失敗しました。\nInit Error");
		return E_FAIL;
	}
	ZeroMemory( &d3dpp , sizeof( d3dpp ) );	/*0クリア処理*/
	
	d3dpp.BackBufferWidth = SCREEN_WIDTH;			/*画面幅設定*/
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;			/*画面高さ設定*/
	d3dpp.BackBufferFormat = d3ddm.Format;			/*バックバッファ形式指定*/
	d3dpp.BackBufferCount = 1;						/*バックバッファの数指定*/
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		/*デプスバッファを使用する指定*/
	d3dpp.EnableAutoDepthStencil = TRUE;			/*ステンシルバッファを使用する指定*/
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		/*デプスバッファとして16bitを使用する指定*/
	d3dpp.Windowed = bWindow;						/*Init引数からウィンドウかフルスクリーンを指定する*/
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		/*リフレッシュレート*/
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		/*プレゼント送信間隔*/
	
	//復帰用コピー
	g_pd3dpp = &d3dpp;

	/*Direct3DDeviceの生成*/
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
		D3DCREATE_HARDWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
		{
			if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_REF , hWnd ,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
			{
				Error("Direct3Dデバイスの生成に失敗しました");
				return E_FAIL;
			}
		}
	}

	/*レンダーステートの設定*/
	g_pD3DDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );	/*カリング設定*/
	g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/

	/*サンプラーステートの設定（テクスチャ補正）*/
	/*
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_NONE );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_NONE );
	*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	

	/*ラップテキスチャの設定*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );

	/*テクスチャステージステートαの設定*/
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_CURRENT );

	/*フォント設定*/
	D3DXCreateFont( g_pD3DDevice , 18 , 0 , 0 , 0 , FALSE , SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS , DEFAULT_QUALITY , DEFAULT_PITCH , "Terminal" , &g_pFont );
	
	/*キー入力の設定*/
	if( InitKeyboard( hInstance , hWnd ) != S_OK )
	{
		Error("入力(キーボード)の初期化に失敗しました。");
	}
	/*マウス入力の設定*/
	if( InitMouse( hInstance , hWnd ) != S_OK )
	{
		Error("入力(マウス)の初期化に失敗しました。");
	}
	/*ゲームパッド入力の設定*/
	if( InitGamePad( hInstance , hWnd ) != S_OK )
	{
		Error("入力(ゲームパッド)の初期化に失敗しました。");
	}
	/*サウンドの設定*/
	if( InitSound( hWnd ) != S_OK )
	{
		Error("サウンドの初期化に失敗しました。");
	}
	/*サウンド表示の設定*/
	if( InitSoundGraphic() != S_OK )
	{
		Error("サウンド表示の初期化に失敗しました。");
	}
	/*フェードの設定*/
	InitFade();
#ifdef _DEBUG
	//g_mode = MODE_MENU;
#endif
	SetMode( g_mode );

	return S_OK;
}

/*******************************************************************************
* 関数名	:void Uninit( void )
*		全体終了処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:終了時に実行する関数を内包する関数で終了時に一度のみ動作。
*******************************************************************************/
void Uninit( void )
{
	/*入力の解放*/
	UninitKeyboard();
	UninitMouse();
	UninitGamePad();

	/*フェードの解放*/
	UninitFade();

	/*各モードごとの解放*/
	UninitTitle();
	UninitMenu();
	UninitGame();
	UninitResult();
	UninitAfGame();
	UninitAfResult();
	
	/*サウンド表示の解放*/
	UninitSoundGraphic();

	/*サウンドの解放*/
	StopSound();
	UninitSound();

	if( g_pD3DDevice != NULL )	/*デバイスが存在するならば*/
	{
		g_pD3DDevice->Release();	/*デバイスの解放*/
		g_pD3DDevice = NULL;
	}
	if( g_pD3D != NULL )	/*デバイスが存在するならば*/
	{
		g_pD3D->Release();	/*デバイスの解放*/
		g_pD3D = NULL;
	}
}

/*******************************************************************************
* 関数名	:void Update( void )
*		更新処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:処理周回毎の処理内容を記述します。実質的なメイン関数です。
*******************************************************************************/
void Update( void )
{
	int soundMode = SOUND_NORMAL;
	/*入力の更新*/
	UpdateInput();
	UpdateKeyboard();
	UpdateMouse();
	UpdateGamePad();

	if( GetKeyboardTrigger( DIK_M ) || GetGamePadTrigger( 8 ) )
	{
		if( SoundMute() )
		{
			soundMode = SOUND_MUTEON;
		}
		else
		{
			soundMode = SOUND_MUTEOFF;
		}
	}

	switch( g_mode )
	{
		case MODE_TITLE :
			{
				UpdateTitle();
				break;
			}
		case MODE_MENU :
			{
				UpdateMenu();
				break;
			}
		case MODE_GAME :
			{
				UpdateGame();
				break;
			}
		case MODE_RESULT:
			{
				UpdateResult();
				break;
			}
		case MODE_AF_GAME :
			{
				UpdateAfGame();
				break;
			}
		case MODE_AF_RESULT:
			{
				UpdateAfResult();
				break;
			}
		default :
			{
				Error("意図せぬモードが更新されました");
				break;
			}
	}

	UpdateFade();	//フェードの更新
	UpdateSoundGraphic( soundMode );	//サウンド表示の更新
}

/*******************************************************************************
* 関数名	:void Draw( void )
*		描画処理関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:画面出力に関わる処理を内包します。
*******************************************************************************/
void Draw( void )
{
	/*もしg_pD3DDeviceがNULLだった場合描画を行わない*/
	if( g_pD3DDevice == NULL )
	{
		return;
	}

	/*画面のクリア*/
	g_pD3DDevice->Clear( 0 , NULL , ( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ) , D3DCOLOR_RGBA_BACK , 1.0f , 0 );

	/*描画開始*/
	if( SUCCEEDED( g_pD3DDevice->BeginScene() ) )/*描画に成功した場合は*/
	{
		/*描画部ここから*/
		switch( g_mode )
		{
			case MODE_TITLE :
				{
					DrawTitle();
					break;
				}
			case MODE_MENU :
				{
					DrawMenu();
					break;
				}
			case MODE_GAME :
				{
					DrawGame();
					break;
				}
			case MODE_RESULT:
				{
					DrawResult();
					break;
				}
			case MODE_AF_GAME :
				{
					DrawAfGame();
					break;
				}
			case MODE_AF_RESULT:
				{
					DrawAfResult();
					break;
				}
			default :
				{
					Error("意図せぬモードが描画されました");
					break;
				}
		}

		DrawFade();	//フェードの描画
		DrawSoundGraphic();	//サウンド表示の表示

#ifdef _DEBUG
	//	DrawFPS();
#endif
		/*描画部ここまで*/
		g_pD3DDevice->EndScene();	/*描画終了処理*/
	}
	else
	{
		Error("描画の開始に失敗しました。\nDraw BeginScene Error");
	}

	if( g_pD3DDevice->Present( NULL , NULL , NULL , NULL ) == D3DERR_DEVICELOST )
	{
		
		// 復帰可能の場合
		if( g_pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			if( FAILED( g_pD3DDevice->Reset( g_pd3dpp ) ) )
			{
				Error("デバイスのリセットに失敗しました。");
				Uninit();
				Init( *g_phInstance , GetWnd() ,  SCREEN_TYPE );
				//DeviceRescue( *g_phInstance , GetWnd() ,  SCREEN_TYPE );
			}
		}
	}
}

/*******************************************************************************
* 関数名	:void SetMode( MODE mode )
*		モードの設定と初期化を行います。
* 引数		:MODE mode	変更先のモード
* 戻り値	:void 戻り値なし
* 説明		:モードの設定と初期化を行います。
*******************************************************************************/
void SetMode( MODE mode )
{
	/*前モード破棄*/
	switch( g_mode )
	{
		case MODE_TITLE :
			{
				UninitTitle();
				break;
			}
		case MODE_MENU :
			{
				UninitMenu();
				break;
			}
		case MODE_GAME :
			{
				UninitGame();
				break;
			}
		case MODE_RESULT:
			{
				UninitResult();
				break;
			}
		case MODE_AF_GAME :
			{
				UninitAfGame();
				break;
			}
		case MODE_AF_RESULT:
			{
				UninitAfResult();
				break;
			}
		default :
			{
				Error("意図せぬモードが初期化されました");
				break;
			}
	}
	StopSound();
	/*モード読み込み*/
	switch( mode )
	{
		case MODE_TITLE :
			{
				InitTitle();
				break;
			}
		case MODE_MENU :
			{
				InitMenu();
				break;
			}
		case MODE_GAME :
			{
				InitGame();
				break;
			}
		case MODE_RESULT:
			{
				InitResult();
				break;
			}
		case MODE_AF_GAME :
			{
				InitAfGame();
				break;
			}
		case MODE_AF_RESULT:
			{
				InitAfResult();
				break;
			}
		default :
			{
				Error("意図せぬモードが初期化されました");
				break;
			}
	}
	/*モード更新*/
	g_mode = mode;
}

/*******************************************************************************
* 関数名	:MODE GetMode( void )
*		現在実行中のモードIDを参照を行います。
* 引数		:void		引数なし
* 戻り値	:MODE mode	現在のモード
* 説明		:現在実行中のモードIDを参照を行います。
*******************************************************************************/
MODE GetMode( void )
{
	return g_mode;
}

/*******************************************************************************
* 関数名	:LPDIRECT3DDEVICE9 GetDevice( void )
*		Direct3Dデバイスを返す関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:Direct3Dデバイスを他のファイルから呼ぶ際に使用します。
*******************************************************************************/
LPDIRECT3DDEVICE9 GetDevice( void )
{
	return g_pD3DDevice;	/*Direct3Dデバイスへのポインタ*/
}

/*******************************************************************************
* 関数名	:HWND GetWnd( void )
*		hWndのミラーを返す関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:hWndを他のファイルから参照する際に使用します。
*******************************************************************************/
HWND GetWnd( void )
{
	return hWndMiller;	/*Direct3Dデバイスへのポインタ*/
}

/*******************************************************************************
* 関数名	:void Error( void )
*		エラーメッセージボックス表示関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:Errorを示すメッセージボックスを表示する関数です。
*******************************************************************************/
void Error( char *errorTxt )
{
	MessageBox( hWndMiller , errorTxt ,"エラー確認", MB_OK );	
}

/*******************************************************************************
* 関数名	:void DrawFPS( void )
*		FPS表示関数です。
* 引数		:void 引数なし
* 戻り値	:void 戻り値なし
* 説明		:FPSを画面に表示する関数です。
*******************************************************************************/
void DrawFPS( void )
{
	RECT rectB = { 0 , 0 , WINDOW_WIDTH , WINDOW_HEIGHT };	/*白文字用*/
	RECT rectW = { 1 , 1 , WINDOW_WIDTH , WINDOW_HEIGHT };	/*黒文字用*/
	char aStr[256];
	wsprintf( &aStr[0] , "FPS:%d\n" , g_nCountFPS );
	g_pFont->DrawText( NULL , &aStr[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 122 , 122 , 255 ) );
	g_pFont->DrawText( NULL , &aStr[0] , -1 , &rectB , DT_LEFT , D3DCOLOR_RGBA( 0 , 0 , 0 , 255 ) );
}		

/*******************************************************************************
* 関数名	:void DebugMessage( char *errorTxt , float x , float y )
*		デバッグメッセージ表示関数です。
* 引数		:char *errorTxt	表示するテキスト
*			 float x	表示するX座標
*			 float y	表示するY座標
* 戻り値	:void 戻り値なし
* 説明		:Errorを示すメッセージボックスを表示する関数です。
*******************************************************************************/
void DebugMessage( char *errorTxt , float x , float y )
{
	RECT rectB = { ( LONG )( x ) , ( LONG )( y ) , WINDOW_WIDTH , WINDOW_HEIGHT };	/*白文字用*/
	RECT rectW = { ( LONG )( x ) + 1 , ( LONG )( y ) + 1 , WINDOW_WIDTH , WINDOW_HEIGHT };	/*黒文字用*/
	char aStr[256];

	wsprintf( &aStr[0] , errorTxt , g_nCountFPS );
	g_pFont->DrawText( NULL , errorTxt , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 122 , 122 , 255 ) );
	g_pFont->DrawText( NULL , errorTxt , -1 , &rectB , DT_LEFT , D3DCOLOR_RGBA( 0 , 0 , 0 , 255 ) );
}

/*******************************************************************************
* 関数名	:LPD3DXFONT *getFont( void )
*		フォントの値をゲットする関数です。
* 引数		:void 引数なし
* 戻り値	:LPD3DXFONT*	g_pFontのアドレス
* 説明		:フォントの値をゲットする関数です。
*******************************************************************************/
LPD3DXFONT getFont( void )
{
	return g_pFont;
}

/*******************************************************************************
* 関数名	:HRESULT DeviceRescue( HINSTANCE hInstance , HWND hWnd , bool bWindow  )
*		デバイス復旧関数です。
* 引数		:HINSTANCE hInstance	インスタンスのハンドル
*			 HWND hWnd				ウィンドウのハンドル
*			 bool bWindow			ウィンドウをフルスクリーンにするかどうか
* 戻り値	:HRESULT	関数の動作結果
* 説明		:デバイスを解放・再取得して復旧する関数です。
*******************************************************************************/
HRESULT DeviceRescue( HINSTANCE hInstance , HWND hWnd , bool bWindow  )
{
	//解放
	
	/*入力の解放*/
	UninitKeyboard();
	UninitMouse();
	UninitGamePad();

	/*フェードの解放*/
	UninitFade();

	/*各モードごとの解放*/
	UninitTitle();
	UninitGame();
	UninitResult();
	UninitAfResult();
	UninitAfGame();

	/*サウンドの解放*/
	StopSound();
	UninitSound();

	if( g_pD3DDevice != NULL )	/*デバイスが存在するならば*/
	{
		g_pD3DDevice->Release();	/*デバイスの解放*/
		g_pD3DDevice = NULL;
	}
	if( g_pD3D != NULL )	/*デバイスが存在するならば*/
	{
		g_pD3D->Release();	/*デバイスの解放*/
		g_pD3D = NULL;
	}
	//再取得
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;	/*パソコン側のディスプレイデータ*/

	/*Direct3Dオブジェクトの生成*/
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( g_pD3D == NULL )	/*エラー処理*/
	{
		Error("Direct3Dオブジェクトの生成に失敗しました。\nInit g_pD3D Error");
		return E_FAIL;
	}
	/*ディスプレイモードの取得*/
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm ) ) )
	{
		Error("ディスプレイモードの取得に失敗しました。\nInit Error");
		return E_FAIL;
	}
	ZeroMemory( &d3dpp , sizeof( d3dpp ) );	/*0クリア処理*/
	
	d3dpp.BackBufferWidth = SCREEN_WIDTH;			/*画面幅設定*/
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;			/*画面高さ設定*/
	d3dpp.BackBufferFormat = d3ddm.Format;			/*バックバッファ形式指定*/
	d3dpp.BackBufferCount = 1;						/*バックバッファの数指定*/
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		/*デプスバッファを使用する指定*/
	d3dpp.EnableAutoDepthStencil = TRUE;			/*ステンシルバッファを使用する指定*/
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		/*デプスバッファとして16bitを使用する指定*/
	d3dpp.Windowed = bWindow;						/*Init引数からウィンドウかフルスクリーンを指定する*/
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		/*リフレッシュレート*/
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		/*プレゼント送信間隔*/
	
	//復帰用コピー
	g_pd3dpp = &d3dpp;

	/*Direct3DDeviceの生成*/
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
		D3DCREATE_HARDWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
		{
			if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_REF , hWnd ,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
			{
				Error("Direct3Dデバイスの生成に失敗しました");
				return E_FAIL;
			}
		}
	}

	/*レンダーステートの設定*/
	g_pD3DDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );	/*カリング設定*/
	g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*アルファブレンド有効化*/
	g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*スクリーン側ブレンド設定*/
	g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*ポリゴン側ブレンド設定*/

	/*サンプラーステートの設定（テクスチャ補正）*/
	/*
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_NONE );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_NONE );
	*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	

	/*ラップテキスチャの設定*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );

	/*テクスチャステージステートαの設定*/
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_CURRENT );



	/*フォント設定*/
	D3DXCreateFont( g_pD3DDevice , 18 , 0 , 0 , 0 , FALSE , SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS , DEFAULT_QUALITY , DEFAULT_PITCH , "Terminal" , &g_pFont );
	
	/*キー入力の設定*/
	if( InitKeyboard( hInstance , hWnd ) != S_OK )
	{
		Error("入力(キーボード)の初期化に失敗しました。");
	}
	/*マウス入力の設定*/
	if( InitMouse( hInstance , hWnd ) != S_OK )
	{
		Error("入力(マウス)の初期化に失敗しました。");
	}
	/*サウンドの設定*/
	if( InitSound( hWnd ) != S_OK )
	{
		Error("サウンドの初期化に失敗しました。");
	}
	/*フェードの設定*/
	InitFade();

	SetMode( g_mode );

	return S_OK;
}