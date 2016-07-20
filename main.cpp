/*******************************************************************************
* �^�C�g��	:�ۑ�04�u�����j���O�}���v
* �t�@�C����:main.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00	��{�����̊���
*			ver 1.10	�X�N���[���w�i�̒ǉ�
*			ver 1.20	���͐搶��ǉ�
*			ver 1.30	�~�T�C�����˃V�X�e����ǉ�
*			ver 1.40	�G�t�F�N�g��ǉ�
*			ver 1.42	�G�t�F�N�g�𐢊E�ςɍ��킹�C��
*			ver 1.44	�͐搶�̉摜���g�p���Ă��������J�[�\���ɕύX
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "input.h"
#include "fade.h"

#include "sound.h"
#include "soundGraphic.h"

//���[�h���Ƃ̓ǂݍ���
#include "title.h"
#include "menu.h"
#include "game.h"
#include "result.h"
#include "afResult.h"
#include "afGame.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
LRESULT CALLBACK Wndproc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam );

/*DirectX*/
HRESULT Init( HINSTANCE hInstance , HWND hWnd , bool bWindow );
void Uninit( void );
void Update( void );
void Draw( void );

/*�f�o�C�X����*/
HRESULT DeviceRescue( HINSTANCE hInstance , HWND hWnd , bool bWindow  );

/*WindowsAPI*/
void Error( char *errorTxt );

/*FPS�\��*/
void DrawFPS( void );

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
LPDIRECT3D9 g_pD3D = NULL;			/*Direct3D�I�u�W�F�N�g�|�C���^*/
LPDIRECT3DDEVICE9 g_pD3DDevice;		/*Direct3D�f�o�C�X�ւ̃|�C���^*/
LPD3DXFONT g_pFont;		/*�t�H���g�ւ̃|�C���^*/
int g_nCountFPS;	/*FPS�J�E���^�[*/

MODE g_mode = MODE_TITLE;	/*���݂̃��[�h*/

HWND hWndMiller;	/*�A���[�g�\���֐��ȈՉ��p�̃~���[*/
HINSTANCE *g_phInstance;
D3DPRESENT_PARAMETERS *g_pd3dpp;
/*******************************************************************************
* �֐���	:int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int hCmdShow )
*		���C���֐��ł��B
* ����		:HINSTANCE hInstance		
*			 HINSTANCE hPrevInstance	
*			 LPSTR lpCmdLine			
*			 int hCmdShow 
* �߂�l	:����I��:0
* ����		:�v���O�����̊�ƂȂ郁�C���֐��ł��B
*******************************************************************************/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	DWORD dwFrameCount;
	DWORD dwCurrentTime;
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	
	g_phInstance = &hInstance;

	/*�E�B���h�E�T�C�Y�K��*/
	RECT windowSizeRect = { 0 , 0 , WINDOW_WIDTH , WINDOW_HEIGHT };
	/*�E�B���h�E�T�C�Y����*/
	AdjustWindowRect( &windowSizeRect , WS_OVERLAPPEDWINDOW , false );

	WNDCLASSEX wcex =
	{
		sizeof( WNDCLASSEX ),	/*�������T�C�Y*/
		CS_CLASSDC,				/*�E�B���h�E�X�^�C��*/
		Wndproc,				/*Wndproc�֐��̃A�h���X*/
		0,				/*���g�p*/
		0,				/*���g�p*/
		hInstance,				/*�C���X�^���X�n���h��*/
		NULL,					/*�g�p����A�C�R���w��*/
		LoadCursor( NULL , IDC_ARROW ),		/*�}�E�X�J�[�\���w��*/
		( HBRUSH )( COLOR_WINDOW + 1 ),		/*�N���C�A���g�̈�w�i�w��*/
		NULL,								/*���j���[�ݒ�*/
		"Window001",			/*�E�B���h�E�N���X��*/
		NULL					/*�k���A�C�R�����*/
	};

	HWND hWnd;	//�E�B���h�E�n���h��
	MSG msg;	//���b�Z�[�W�p�ϐ�

	RegisterClassEx( &wcex );	/*�E�B���h�E�N���X�o�^*/
	
	hWnd = CreateWindowEx( 
		0,				/*���g�p*/
		"Window001",	/*�E�B���h�E�N���X��*/
		"AIriSphere",	/*�E�B���h�E�\����*/
		WS_OVERLAPPEDWINDOW,	/*�E�B���h�E�X�^�C��*/
		CW_USEDEFAULT,	/*����w*/
		CW_USEDEFAULT,	/*����x*/
		windowSizeRect.right - windowSizeRect.left,	/*��*/
		windowSizeRect.bottom - windowSizeRect.top,	/*����*/
		NULL,	/*�e�E�B���h�E�̃n���h��*/
		NULL,	/*���j���[���͎q�E�B���h�EID*/
		hInstance,
		NULL	/*�E�B���h�E�쐬�f�[�^*/
	);

	/*����������*/
	if( FAILED( Init( hInstance , hWnd , SCREEN_TYPE ) ) )
	{
		/*�G���[����*/
		Error( "�������Ɏ��s���܂����B\n�v���O�������I�����܂��B" );
		return -1;
	}

	/*��������FPS�ݒ�*/
	timeBeginPeriod( 1 );	/*����\��ݒ�*/
	
	/*�K�v���l�̑��*/
	dwFrameCount = 
	dwCurrentTime = 0;
	dwExecLastTime =
	dwFPSLastTime = timeGetTime();
	
	/*�����܂�FPS�ݒ�*/

	ShowWindow( hWnd , nCmdShow );	/*�E�B���h�E����*/
	UpdateWindow( hWnd );			/*�E�B���h�E�\���A�b�v�f�[�g*/
	
	hWndMiller = hWnd;	/*�~���[�ւ̕���*/
	
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
				Update();	/*�X�V����*/
				Draw();		/*�`�揈��*/
				dwFrameCount++;
			}
			else
			{
				Sleep(( 1000 / FPS_RATE ) - ( dwCurrentTime - dwExecLastTime ));
			}
		}
	}

	timeEndPeriod( 1 );	/*����\��߂�*/

	/*�E�B���h�E�N���X�̉��*/
	UnregisterClass( "Window001" , wcex.hInstance );
	/*�I������*/
	Uninit();
	return ( int )msg.wParam;
}

/*******************************************************************************
* �֐���	:LRESULT CALLBACK Wndproc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
*		�E�B���h�E�v���V�[�W���̏����B
* ����		:HWND hWnd		�E�B���h�E�n���h��
*			 UINT uMsg		���b�Z�[�W
*			 WPARAM wParam	�p�����[�^
*			 LPARAM lParam	�p�����[�^
* �߂�l	:����I��:0
* ����		:�E�B���h�E�v���V�[�W���Ɋւ��鏈�����s���܂��B
*******************************************************************************/
LRESULT CALLBACK Wndproc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
{
	int nID;
	switch( uMsg )
	{
		case WM_CREATE:/*�E�B���h�E������*/
			{
				//PlaySound( "./data/BGM/free0602.wav" , NULL , SND_FILENAME | SND_ASYNC | SND_LOOP );
				break;
			}
		case WM_CLOSE:/*�~�{�^�����*/
			{
				nID = MessageBox( hWnd ,"�A�v���P�[�V�������I�����܂����B","�I���m�F", MB_YESNO );
				if( nID == IDYES )
				{
					Uninit();
					DestroyWindow( hWnd );	/*�E�B���h�E��j������*/
				}
				else
				{
					uMsg = 0;
				}
				break;
			}
		case WM_DESTROY:	/*�E�B���h�E��j��*/
			{	
				Uninit();
				PostQuitMessage( 0 );
				break;
			}
		case WM_PAINT:/*�`�悳�ꂽ��*/
			{
				break;
			}
		case WM_KEYDOWN:
			{
				switch( wParam )
				{

					case VK_ESCAPE:
						nID = MessageBox( hWnd ,"�A�v���P�[�V�������I�����܂����B","�I���m�F", MB_YESNO );
						if( nID == IDYES )
						{
							Uninit();
							DestroyWindow( hWnd );	/*�E�B���h�E��j������*/
						}
						break;
				}
			}
			break;
	}
	return DefWindowProc( hWnd , uMsg , wParam , lParam );
}

/*******************************************************************************
* �֐���	:HRESULT Init( HINSTANCE hInstance , HWND hWnd , bool bWindow  )
*		�S�̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
*			 bool bWindow			�E�B���h�E���t���X�N���[���ɂ��邩�ǂ���
* �߂�l	:HRESULT	�֐��̓��쌋��
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT Init( HINSTANCE hInstance , HWND hWnd , bool bWindow )
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;	/*�p�\�R�����̃f�B�X�v���C�f�[�^*/

	/*Direct3D�I�u�W�F�N�g�̐���*/
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( g_pD3D == NULL )	/*�G���[����*/
	{
		Error("Direct3D�I�u�W�F�N�g�̐����Ɏ��s���܂����B\nInit g_pD3D Error");
		return E_FAIL;
	}
	/*�f�B�X�v���C���[�h�̎擾*/
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm ) ) )
	{
		Error("�f�B�X�v���C���[�h�̎擾�Ɏ��s���܂����B\nInit Error");
		return E_FAIL;
	}
	ZeroMemory( &d3dpp , sizeof( d3dpp ) );	/*0�N���A����*/
	
	d3dpp.BackBufferWidth = SCREEN_WIDTH;			/*��ʕ��ݒ�*/
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;			/*��ʍ����ݒ�*/
	d3dpp.BackBufferFormat = d3ddm.Format;			/*�o�b�N�o�b�t�@�`���w��*/
	d3dpp.BackBufferCount = 1;						/*�o�b�N�o�b�t�@�̐��w��*/
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		/*�f�v�X�o�b�t�@���g�p����w��*/
	d3dpp.EnableAutoDepthStencil = TRUE;			/*�X�e���V���o�b�t�@���g�p����w��*/
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		/*�f�v�X�o�b�t�@�Ƃ���16bit���g�p����w��*/
	d3dpp.Windowed = bWindow;						/*Init��������E�B���h�E���t���X�N���[�����w�肷��*/
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		/*���t���b�V�����[�g*/
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		/*�v���[���g���M�Ԋu*/
	
	//���A�p�R�s�[
	g_pd3dpp = &d3dpp;

	/*Direct3DDevice�̐���*/
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
		D3DCREATE_HARDWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
		{
			if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_REF , hWnd ,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
			{
				Error("Direct3D�f�o�C�X�̐����Ɏ��s���܂���");
				return E_FAIL;
			}
		}
	}

	/*�����_�[�X�e�[�g�̐ݒ�*/
	g_pD3DDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );	/*�J�����O�ݒ�*/
	g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/

	/*�T���v���[�X�e�[�g�̐ݒ�i�e�N�X�`���␳�j*/
	/*
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_NONE );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_NONE );
	*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	

	/*���b�v�e�L�X�`���̐ݒ�*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );

	/*�e�N�X�`���X�e�[�W�X�e�[�g���̐ݒ�*/
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_CURRENT );

	/*�t�H���g�ݒ�*/
	D3DXCreateFont( g_pD3DDevice , 18 , 0 , 0 , 0 , FALSE , SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS , DEFAULT_QUALITY , DEFAULT_PITCH , "Terminal" , &g_pFont );
	
	/*�L�[���͂̐ݒ�*/
	if( InitKeyboard( hInstance , hWnd ) != S_OK )
	{
		Error("����(�L�[�{�[�h)�̏������Ɏ��s���܂����B");
	}
	/*�}�E�X���͂̐ݒ�*/
	if( InitMouse( hInstance , hWnd ) != S_OK )
	{
		Error("����(�}�E�X)�̏������Ɏ��s���܂����B");
	}
	/*�Q�[���p�b�h���͂̐ݒ�*/
	if( InitGamePad( hInstance , hWnd ) != S_OK )
	{
		Error("����(�Q�[���p�b�h)�̏������Ɏ��s���܂����B");
	}
	/*�T�E���h�̐ݒ�*/
	if( InitSound( hWnd ) != S_OK )
	{
		Error("�T�E���h�̏������Ɏ��s���܂����B");
	}
	/*�T�E���h�\���̐ݒ�*/
	if( InitSoundGraphic() != S_OK )
	{
		Error("�T�E���h�\���̏������Ɏ��s���܂����B");
	}
	/*�t�F�[�h�̐ݒ�*/
	InitFade();
#ifdef _DEBUG
	//g_mode = MODE_MENU;
#endif
	SetMode( g_mode );

	return S_OK;
}

/*******************************************************************************
* �֐���	:void Uninit( void )
*		�S�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void Uninit( void )
{
	/*���͂̉��*/
	UninitKeyboard();
	UninitMouse();
	UninitGamePad();

	/*�t�F�[�h�̉��*/
	UninitFade();

	/*�e���[�h���Ƃ̉��*/
	UninitTitle();
	UninitMenu();
	UninitGame();
	UninitResult();
	UninitAfGame();
	UninitAfResult();
	
	/*�T�E���h�\���̉��*/
	UninitSoundGraphic();

	/*�T�E���h�̉��*/
	StopSound();
	UninitSound();

	if( g_pD3DDevice != NULL )	/*�f�o�C�X�����݂���Ȃ��*/
	{
		g_pD3DDevice->Release();	/*�f�o�C�X�̉��*/
		g_pD3DDevice = NULL;
	}
	if( g_pD3D != NULL )	/*�f�o�C�X�����݂���Ȃ��*/
	{
		g_pD3D->Release();	/*�f�o�C�X�̉��*/
		g_pD3D = NULL;
	}
}

/*******************************************************************************
* �֐���	:void Update( void )
*		�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void Update( void )
{
	int soundMode = SOUND_NORMAL;
	/*���͂̍X�V*/
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
				Error("�Ӑ}���ʃ��[�h���X�V����܂���");
				break;
			}
	}

	UpdateFade();	//�t�F�[�h�̍X�V
	UpdateSoundGraphic( soundMode );	//�T�E���h�\���̍X�V
}

/*******************************************************************************
* �֐���	:void Draw( void )
*		�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void Draw( void )
{
	/*����g_pD3DDevice��NULL�������ꍇ�`����s��Ȃ�*/
	if( g_pD3DDevice == NULL )
	{
		return;
	}

	/*��ʂ̃N���A*/
	g_pD3DDevice->Clear( 0 , NULL , ( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ) , D3DCOLOR_RGBA_BACK , 1.0f , 0 );

	/*�`��J�n*/
	if( SUCCEEDED( g_pD3DDevice->BeginScene() ) )/*�`��ɐ��������ꍇ��*/
	{
		/*�`�敔��������*/
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
					Error("�Ӑ}���ʃ��[�h���`�悳��܂���");
					break;
				}
		}

		DrawFade();	//�t�F�[�h�̕`��
		DrawSoundGraphic();	//�T�E���h�\���̕\��

#ifdef _DEBUG
	//	DrawFPS();
#endif
		/*�`�敔�����܂�*/
		g_pD3DDevice->EndScene();	/*�`��I������*/
	}
	else
	{
		Error("�`��̊J�n�Ɏ��s���܂����B\nDraw BeginScene Error");
	}

	if( g_pD3DDevice->Present( NULL , NULL , NULL , NULL ) == D3DERR_DEVICELOST )
	{
		
		// ���A�\�̏ꍇ
		if( g_pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			if( FAILED( g_pD3DDevice->Reset( g_pd3dpp ) ) )
			{
				Error("�f�o�C�X�̃��Z�b�g�Ɏ��s���܂����B");
				Uninit();
				Init( *g_phInstance , GetWnd() ,  SCREEN_TYPE );
				//DeviceRescue( *g_phInstance , GetWnd() ,  SCREEN_TYPE );
			}
		}
	}
}

/*******************************************************************************
* �֐���	:void SetMode( MODE mode )
*		���[�h�̐ݒ�Ə��������s���܂��B
* ����		:MODE mode	�ύX��̃��[�h
* �߂�l	:void �߂�l�Ȃ�
* ����		:���[�h�̐ݒ�Ə��������s���܂��B
*******************************************************************************/
void SetMode( MODE mode )
{
	/*�O���[�h�j��*/
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
				Error("�Ӑ}���ʃ��[�h������������܂���");
				break;
			}
	}
	StopSound();
	/*���[�h�ǂݍ���*/
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
				Error("�Ӑ}���ʃ��[�h������������܂���");
				break;
			}
	}
	/*���[�h�X�V*/
	g_mode = mode;
}

/*******************************************************************************
* �֐���	:MODE GetMode( void )
*		���ݎ��s���̃��[�hID���Q�Ƃ��s���܂��B
* ����		:void		�����Ȃ�
* �߂�l	:MODE mode	���݂̃��[�h
* ����		:���ݎ��s���̃��[�hID���Q�Ƃ��s���܂��B
*******************************************************************************/
MODE GetMode( void )
{
	return g_mode;
}

/*******************************************************************************
* �֐���	:LPDIRECT3DDEVICE9 GetDevice( void )
*		Direct3D�f�o�C�X��Ԃ��֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:Direct3D�f�o�C�X�𑼂̃t�@�C������ĂԍۂɎg�p���܂��B
*******************************************************************************/
LPDIRECT3DDEVICE9 GetDevice( void )
{
	return g_pD3DDevice;	/*Direct3D�f�o�C�X�ւ̃|�C���^*/
}

/*******************************************************************************
* �֐���	:HWND GetWnd( void )
*		hWnd�̃~���[��Ԃ��֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:hWnd�𑼂̃t�@�C������Q�Ƃ���ۂɎg�p���܂��B
*******************************************************************************/
HWND GetWnd( void )
{
	return hWndMiller;	/*Direct3D�f�o�C�X�ւ̃|�C���^*/
}

/*******************************************************************************
* �֐���	:void Error( void )
*		�G���[���b�Z�[�W�{�b�N�X�\���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:Error���������b�Z�[�W�{�b�N�X��\������֐��ł��B
*******************************************************************************/
void Error( char *errorTxt )
{
	MessageBox( hWndMiller , errorTxt ,"�G���[�m�F", MB_OK );	
}

/*******************************************************************************
* �֐���	:void DrawFPS( void )
*		FPS�\���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:FPS����ʂɕ\������֐��ł��B
*******************************************************************************/
void DrawFPS( void )
{
	RECT rectB = { 0 , 0 , WINDOW_WIDTH , WINDOW_HEIGHT };	/*�������p*/
	RECT rectW = { 1 , 1 , WINDOW_WIDTH , WINDOW_HEIGHT };	/*�������p*/
	char aStr[256];
	wsprintf( &aStr[0] , "FPS:%d\n" , g_nCountFPS );
	g_pFont->DrawText( NULL , &aStr[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 122 , 122 , 255 ) );
	g_pFont->DrawText( NULL , &aStr[0] , -1 , &rectB , DT_LEFT , D3DCOLOR_RGBA( 0 , 0 , 0 , 255 ) );
}		

/*******************************************************************************
* �֐���	:void DebugMessage( char *errorTxt , float x , float y )
*		�f�o�b�O���b�Z�[�W�\���֐��ł��B
* ����		:char *errorTxt	�\������e�L�X�g
*			 float x	�\������X���W
*			 float y	�\������Y���W
* �߂�l	:void �߂�l�Ȃ�
* ����		:Error���������b�Z�[�W�{�b�N�X��\������֐��ł��B
*******************************************************************************/
void DebugMessage( char *errorTxt , float x , float y )
{
	RECT rectB = { ( LONG )( x ) , ( LONG )( y ) , WINDOW_WIDTH , WINDOW_HEIGHT };	/*�������p*/
	RECT rectW = { ( LONG )( x ) + 1 , ( LONG )( y ) + 1 , WINDOW_WIDTH , WINDOW_HEIGHT };	/*�������p*/
	char aStr[256];

	wsprintf( &aStr[0] , errorTxt , g_nCountFPS );
	g_pFont->DrawText( NULL , errorTxt , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 122 , 122 , 255 ) );
	g_pFont->DrawText( NULL , errorTxt , -1 , &rectB , DT_LEFT , D3DCOLOR_RGBA( 0 , 0 , 0 , 255 ) );
}

/*******************************************************************************
* �֐���	:LPD3DXFONT *getFont( void )
*		�t�H���g�̒l���Q�b�g����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:LPD3DXFONT*	g_pFont�̃A�h���X
* ����		:�t�H���g�̒l���Q�b�g����֐��ł��B
*******************************************************************************/
LPD3DXFONT getFont( void )
{
	return g_pFont;
}

/*******************************************************************************
* �֐���	:HRESULT DeviceRescue( HINSTANCE hInstance , HWND hWnd , bool bWindow  )
*		�f�o�C�X�����֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
*			 bool bWindow			�E�B���h�E���t���X�N���[���ɂ��邩�ǂ���
* �߂�l	:HRESULT	�֐��̓��쌋��
* ����		:�f�o�C�X������E�Ď擾���ĕ�������֐��ł��B
*******************************************************************************/
HRESULT DeviceRescue( HINSTANCE hInstance , HWND hWnd , bool bWindow  )
{
	//���
	
	/*���͂̉��*/
	UninitKeyboard();
	UninitMouse();
	UninitGamePad();

	/*�t�F�[�h�̉��*/
	UninitFade();

	/*�e���[�h���Ƃ̉��*/
	UninitTitle();
	UninitGame();
	UninitResult();
	UninitAfResult();
	UninitAfGame();

	/*�T�E���h�̉��*/
	StopSound();
	UninitSound();

	if( g_pD3DDevice != NULL )	/*�f�o�C�X�����݂���Ȃ��*/
	{
		g_pD3DDevice->Release();	/*�f�o�C�X�̉��*/
		g_pD3DDevice = NULL;
	}
	if( g_pD3D != NULL )	/*�f�o�C�X�����݂���Ȃ��*/
	{
		g_pD3D->Release();	/*�f�o�C�X�̉��*/
		g_pD3D = NULL;
	}
	//�Ď擾
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;	/*�p�\�R�����̃f�B�X�v���C�f�[�^*/

	/*Direct3D�I�u�W�F�N�g�̐���*/
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( g_pD3D == NULL )	/*�G���[����*/
	{
		Error("Direct3D�I�u�W�F�N�g�̐����Ɏ��s���܂����B\nInit g_pD3D Error");
		return E_FAIL;
	}
	/*�f�B�X�v���C���[�h�̎擾*/
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm ) ) )
	{
		Error("�f�B�X�v���C���[�h�̎擾�Ɏ��s���܂����B\nInit Error");
		return E_FAIL;
	}
	ZeroMemory( &d3dpp , sizeof( d3dpp ) );	/*0�N���A����*/
	
	d3dpp.BackBufferWidth = SCREEN_WIDTH;			/*��ʕ��ݒ�*/
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;			/*��ʍ����ݒ�*/
	d3dpp.BackBufferFormat = d3ddm.Format;			/*�o�b�N�o�b�t�@�`���w��*/
	d3dpp.BackBufferCount = 1;						/*�o�b�N�o�b�t�@�̐��w��*/
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		/*�f�v�X�o�b�t�@���g�p����w��*/
	d3dpp.EnableAutoDepthStencil = TRUE;			/*�X�e���V���o�b�t�@���g�p����w��*/
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		/*�f�v�X�o�b�t�@�Ƃ���16bit���g�p����w��*/
	d3dpp.Windowed = bWindow;						/*Init��������E�B���h�E���t���X�N���[�����w�肷��*/
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		/*���t���b�V�����[�g*/
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		/*�v���[���g���M�Ԋu*/
	
	//���A�p�R�s�[
	g_pd3dpp = &d3dpp;

	/*Direct3DDevice�̐���*/
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
		D3DCREATE_HARDWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd ,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
		{
			if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_REF , hWnd ,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &g_pD3DDevice ) ) )
			{
				Error("Direct3D�f�o�C�X�̐����Ɏ��s���܂���");
				return E_FAIL;
			}
		}
	}

	/*�����_�[�X�e�[�g�̐ݒ�*/
	g_pD3DDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );	/*�J�����O�ݒ�*/
	g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/

	/*�T���v���[�X�e�[�g�̐ݒ�i�e�N�X�`���␳�j*/
	/*
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_NONE );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_NONE );
	*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	

	/*���b�v�e�L�X�`���̐ݒ�*/
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	g_pD3DDevice->SetSamplerState( 0 , D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );

	/*�e�N�X�`���X�e�[�W�X�e�[�g���̐ݒ�*/
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	g_pD3DDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_CURRENT );



	/*�t�H���g�ݒ�*/
	D3DXCreateFont( g_pD3DDevice , 18 , 0 , 0 , 0 , FALSE , SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS , DEFAULT_QUALITY , DEFAULT_PITCH , "Terminal" , &g_pFont );
	
	/*�L�[���͂̐ݒ�*/
	if( InitKeyboard( hInstance , hWnd ) != S_OK )
	{
		Error("����(�L�[�{�[�h)�̏������Ɏ��s���܂����B");
	}
	/*�}�E�X���͂̐ݒ�*/
	if( InitMouse( hInstance , hWnd ) != S_OK )
	{
		Error("����(�}�E�X)�̏������Ɏ��s���܂����B");
	}
	/*�T�E���h�̐ݒ�*/
	if( InitSound( hWnd ) != S_OK )
	{
		Error("�T�E���h�̏������Ɏ��s���܂����B");
	}
	/*�t�F�[�h�̐ݒ�*/
	InitFade();

	SetMode( g_mode );

	return S_OK;
}