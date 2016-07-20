/*******************************************************************************
* �^�C�g��	:����.c
* �t�@�C����:cursor.c
* �쐬��	:�c���t��
* �쐬��	:2015/06/10
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
//�L�[�{�[�h�p
#define MAX_KEY_INPUT_NUM	( 256 )

//�}�E�X�p
#define MAX_MOUSE_INPUT_NUM ( 8 )

//�Q�[���p�b�h�i�W���C�X�e�B�b�N�p�j
#define USE_GAMEPAD_NUM		( 1 )
#define GAMEPAD_ANGLE_NUM	( 4 )
#define GAMEPAD_BUTTON_NUM	( 32 )

#define KEY_ON	( 0x80 )
#define KEY_OFF	( 0x00 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
//�Q�[���p�b�h�p�R�[���o�b�N�֐�
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance , VOID *pContext );
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* , VOID* );

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECTINPUT8 g_pInput = NULL;		/*DirectInput�I�u�W�F�N�g�ւ̃|�C���^*/
static LPDIRECTINPUTDEVICE8 g_pDevKeyboard = NULL;	/*���̓f�o�C�X�i�L�[�{�[�h�j�ւ̃|�C���^*/	
static LPDIRECTINPUTDEVICE8 g_pDevMouse = NULL;		/*���̓f�o�C�X�i�}�E�X�j�ւ̃|�C���^*/	
static LPDIRECTINPUTDEVICE8 g_pDevGamePad = NULL;	/*���̓f�o�C�X�i�Q�[���p�b�h�j�ւ̃|�C���^*/

/*�L�[�{�[�h�̓��͏�񃏁[�N*/
static BYTE g_aKeyState[MAX_KEY_INPUT_NUM];	/*Press*/
static BYTE g_aKeyStateTrigger[MAX_KEY_INPUT_NUM];	/*Trigger*/
static BYTE g_aKeyStateRelease[MAX_KEY_INPUT_NUM];	/*Release*/
static BYTE g_aKeyStateRepeat[MAX_KEY_INPUT_NUM];	/*Repeat */
static int g_aKeyStateRepeatCount[MAX_KEY_INPUT_NUM];	/*�L�[���s�[�g�J�E���^*/

/*�}�E�X�̓��͏�񃏁[�N*/
static LONG g_mouseLocateX;
static LONG g_mouseLocateY;
static LONG g_mouseWheelState;
static BYTE g_aMouseState[MAX_MOUSE_INPUT_NUM];
static BYTE g_aMouseStateTrigger[MAX_MOUSE_INPUT_NUM];
static BYTE g_aMouseStateRelease[MAX_MOUSE_INPUT_NUM];

/*�Q�[���p�b�h�̓��͏�񃏁[�N*/
static DIDEVCAPS g_diGamePadCaps[USE_GAMEPAD_NUM];		/*�f�o�C�X�̃L���p�V�e�B*/
static DIJOYSTATE g_aGamePadState[USE_GAMEPAD_NUM];			/*Press*/
static DIJOYSTATE g_aGamePadStateTrigger[USE_GAMEPAD_NUM];	/*Trigger*/
static DIJOYSTATE g_aGamePadStateRelease[USE_GAMEPAD_NUM];	/*Release*/
static DIJOYSTATE g_aGamePadStateRepeat[USE_GAMEPAD_NUM];	/*Repeat*/
static int g_aGamePadStateRepeatCount[USE_GAMEPAD_NUM][GAMEPAD_BUTTON_NUM];	/*�L�[���s�[�g�J�E���^*/

/*�X�e�B�b�N�����ȈՎ擾*/
static bool g_aGamePadInputAngle[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];	
static bool g_aGamePadInputAngleTrigger[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];
static bool g_aGamePadInputAngleRelease[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];
static bool g_aGamePadInputAngleRepeat[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];
static int g_aGamePadInputAngleRepeatCount[USE_GAMEPAD_NUM][GAMEPAD_ANGLE_NUM];	/*�L�[���s�[�g�J�E���^*/
static bool g_pushAnyButton[USE_GAMEPAD_NUM];

/*�Ȃ�炩�̑���m�F�t���O*/
static bool g_pushAnykey;
static int g_nKeyRepeat = DEFAULT_KEY_REPEAT_FPS;

/*******************************************************************************
* �֐���	:HRESULT InitInput( void )
*		���͏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitInput( HINSTANCE hInstance , HWND hWnd )
{
	g_nKeyRepeat = DEFAULT_KEY_REPEAT_FPS;
	if( g_pInput == NULL )
	{
		if( FAILED( DirectInput8Create( hInstance , DIRECTINPUT_VERSION , IID_IDirectInput8 , ( void** )&g_pInput , NULL ) ) )
		{
			Error("���̓f�o�C�X�̐����Ɏ��s���܂����B");
			return E_FAIL;
		}
	}
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitInput( void )
*		���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
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
* �֐���	:void UpdateInput( void )
*		���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateInput( void )
{
	g_pushAnykey = false;
}


/* ��������@�L�[�{�[�h */

/*******************************************************************************
* �֐���	:HRESULT InitKeyboard( HINSTANCE hInstance , HWND hWnd )
*		�L�[�{�[�h���͂̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitKeyboard( HINSTANCE hInstance , HWND hWnd )
{
	if( FAILED( InitInput( hInstance , hWnd ) ) )
	{
		Error("���̓f�o�C�X�̐����Ɏ��s���܂����B�i�L�[�{�[�h����̌Ăяo���j");
		return E_FAIL;
	}
	if( FAILED( g_pInput->CreateDevice( GUID_SysKeyboard , &g_pDevKeyboard , NULL ) ) )
	{
		Error("�L�[�{�[�h���̓f�o�C�X�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	if( FAILED( g_pDevKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
	{
		Error("�f�[�^�t�H�[�}�b�g�̐ݒ�");
		return E_FAIL;
	}
	if( FAILED( g_pDevKeyboard->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("�������[�h�̐ݒ�");
		return E_FAIL;
	}
	g_pDevKeyboard->Acquire();	/*�A�N�Z�X���̎擾*/

	memset( &g_aKeyState[0]			, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateTrigger[0]	, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateRelease[0]	, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateRepeat[0]	, KEY_OFF , sizeof( BYTE ) * MAX_KEY_INPUT_NUM );
	memset( &g_aKeyStateRepeatCount[0] , 0 , sizeof( int ) * MAX_KEY_INPUT_NUM );

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitKeyboard( void )
*		�L�[�{�[�h���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitKeyboard( void )
{
	if( g_pDevKeyboard != NULL )
	{
		g_pDevKeyboard->Unacquire();	/*�A�N�Z�X���̉��*/
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateKeyboard( void )
*		�L�[�{�[�h���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
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
		g_pDevKeyboard->Acquire();	/*�A�N�Z�X���̎擾*/
	}
}

/*******************************************************************************
* �֐���	:bool GetKeyboardPress( int nKey )
*		�L�[�{�[�h�����Press���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardPress( int nKey )
{
	return ( ( g_aKeyState[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetKeyboardTrigger( int nKey )
*		�L�[�{�[�h�����Trigger���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardTrigger( int nKey )
{
	return ( ( g_aKeyStateTrigger[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetKeyboardRelease( int nKey )
*		�L�[�{�[�h�����Release���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardRelease( int nKey )
{
	return ( ( g_aKeyStateRelease[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetKeyboardRepeat( int nKey )
*		�L�[�{�[�h�����Repeat���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardRepeat( int nKey )
{
	return ( ( g_aKeyStateRepeat[nKey] & 0x80 ) ? true : false );
}


/* ��������@�}�E�X */

/*******************************************************************************
* �֐���	:HRESULT InitMouse( HINSTANCE hInstance , HWND hWnd )
*		�}�E�X���͂̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMouse( HINSTANCE hInstance , HWND hWnd )
{
	if( FAILED( InitInput( hInstance , hWnd ) ) )
	{
		Error("���̓f�o�C�X�̐����Ɏ��s���܂����B�i�}�E�X����̌Ăяo���j");
		return E_FAIL;
	}
	if( FAILED( g_pInput->CreateDevice( GUID_SysMouse , &g_pDevMouse , NULL ) ) )
	{
		Error("�}�E�X���̓f�o�C�X�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	if( FAILED( g_pDevMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		Error("�f�[�^�t�H�[�}�b�g�̐ݒ�");
		return E_FAIL;
	}
	if( FAILED( g_pDevMouse->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("�������[�h�̐ݒ�");
		return E_FAIL;
	}
	g_pDevMouse->Acquire();	/*�A�N�Z�X���̎擾*/

	
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
* �֐���	:void UninitMouse( void )
*		�}�E�X���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMouse( void )
{
	if( g_pDevMouse != NULL )
	{
		g_pDevMouse->Unacquire();	/*�A�N�Z�X���̉��*/
		g_pDevMouse->Release();
		g_pDevMouse = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateMouse( void )
*		�}�E�X���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
void UpdateMouse( void )
{
	DIMOUSESTATE2 mouseState;
	BYTE aMouseButtonState[MAX_MOUSE_INPUT_NUM];

	// �}�E�X�̏�Ԃ��擾
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
		g_pDevMouse->Acquire();	/*�A�N�Z�X���̎擾*/
	}
}

/*******************************************************************************
* �֐���	:bool GetMousePress( int nKey )
*		�}�E�X�����Press���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetMousePress( int nKey )
{
	return ( ( g_aMouseState[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetMouseTrigger( int nKey )
*		�}�E�X�����Trigger���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetMouseTrigger( int nKey )
{
	return ( ( g_aMouseStateTrigger[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetMouseRelease( int nKey )
*		�}�E�X�����Release���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetMouseRelease( int nKey )
{
	return ( ( g_aMouseStateRelease[nKey] & 0x80 ) ? true : false );
}

//�Q�[���p�b�h���͊֐�

/*******************************************************************************
* �֐���	:HRESULT InitGamePad( HINSTANCE hInstance , HWND hWnd )
*		�Q�[���p�b�h���͂̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitGamePad( HINSTANCE hInstance , HWND hWnd )
{
	if( FAILED( InitInput( hInstance , hWnd ) ) )
	{
		Error("���̓f�o�C�X�̐����Ɏ��s���܂����B�i�Q�[���p�b�h����̌Ăяo���j");
		return E_FAIL;
	}
	if( FAILED( g_pInput->EnumDevices( DI8DEVCLASS_GAMECTRL , EnumJoysticksCallback , NULL , DIEDFL_ATTACHEDONLY ) ) )
	{
		Error("���̓f�o�C�X�̐����Ɏ��s���܂����B�i�W���C�X�e�B�b�N�j");
		return E_FAIL;
	}
	if( g_pDevGamePad == NULL )
	{
		return S_OK;
	}
	if( FAILED( g_pDevGamePad->SetDataFormat( &c_dfDIJoystick ) ) )
	{
		Error("�f�[�^�t�H�[�}�b�g�̐ݒ�");
		return E_FAIL;
	}
	if( FAILED( g_pDevGamePad->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("�������[�h�̐ݒ�");
		return E_FAIL;
	}
	g_diGamePadCaps[0].dwSize = sizeof( DIDEVCAPS );	//�L���p�V�e�B�̃T�C�Y�擾
	if( FAILED( g_pDevGamePad->GetCapabilities( &g_diGamePadCaps[0] ) ) )
	{
		Error("�L���p�V�e�B�T�C�Y�̎擾�Ɏ��s");
		return E_FAIL;
	}
	if( FAILED( g_pDevGamePad->EnumObjects( EnumAxesCallback , (VOID*)hWnd , DIDFT_AXIS ) ) )
	{
		Error("�L���p�V�e�B�T�C�Y�̎擾�Ɏ��s");
		return E_FAIL;
	}
	if( FAILED( g_pDevGamePad->SetCooperativeLevel( hWnd , ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		Error("�f�o�C�X�ݒ�擾�Ɏ��s");	//�L�[�͈͂̎擾�Ȃ�
		return E_FAIL;
	}
	g_pDevGamePad->Acquire();
	
	return S_OK;
}

/*******************************************************************************
* �֐���	:BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance , VOID *pContext )
*		�R�[���o�b�N�֐��ł��B
* ����		:const DIDEVICEINSTANCE *pdidInstance
*			 VOID *pContext
* �߂�l	:BOOL	�֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance , VOID *pContext )
{
	if( FAILED( g_pInput->CreateDevice( pdidInstance->guidInstance , &g_pDevGamePad , NULL ) ) )	//�R�[���o�b�N
	{
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}


/*******************************************************************************
* �֐���	:BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE *pdidoi , VOID *pContext )
*		�A�N�Z�X�R�[���o�b�N�֐��ł��B
* ����		:const DIDEVICEOBJECTINSTANCE *pdidoi
*			 VOID *pContext
* �߂�l	:BOOL	�֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
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
* �֐���	:void UninitGamePad( void )
*		�Q�[���p�b�h���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitGamePad( void )
{
	if( g_pDevGamePad != NULL )
	{
		g_pDevGamePad->Unacquire();	/*�A�N�Z�X���̉��*/
		g_pDevGamePad->Release();
		g_pDevGamePad = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateGamePad( void )
*		�Q�[���p�b�h���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
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

	// �Q�[���p�b�h�̏�Ԃ��擾
	if( SUCCEEDED( g_pDevGamePad->GetDeviceState( sizeof( DIJOYSTATE ) , &aGamePadStateBuff ) ) )
	{
		//������������L�[�Ȉ՗p

		//�㉺���E�{�^����������Ă��鎞
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

		//�������s�[�g��������������
		for( int nCntKey = 0; nCntKey < GAMEPAD_ANGLE_NUM; nCntKey++ )
		{
			if( g_aGamePadInputAngleRepeat[0][nCntKey] == true )
			{
				g_aGamePadInputAngleRepeat[0][nCntKey] = g_aGamePadInputAngle[0][nCntKey] ;
				g_aGamePadInputAngleRepeatCount[0][nCntKey] = 0;	/*�L�[���s�[�g�J�E���^*/
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
		g_pDevGamePad->Acquire();	/*�A�N�Z�X���̎擾*/
	}
}

/*******************************************************************************
* �֐���	:bool GetGamePadPress( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Press���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadPress( int nKey )
{
	return ( ( g_aGamePadState[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//�v���C���[�w���
bool GetGamePadPress( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadState[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetGamePadTrigger( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Trigger���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadTrigger( int nKey )
{
	return ( ( g_aGamePadStateTrigger[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//�v���C���[�w���
bool GetGamePadTrigger( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadStateTrigger[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetGamePadRelease( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Release���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadRelease( int nKey )
{
	return ( ( g_aGamePadStateRelease[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//�v���C���[�w���
bool GetGamePadRelease( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadStateRelease[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetGamePadRepeat( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Repeat���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadRepeat( int nKey )
{
	return ( ( g_aGamePadStateRepeat[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}
//�v���C���[�w���
bool GetGamePadRepeat( int gamePadNum , int nKey )
{
	return ( ( g_aGamePadStateRepeat[0].rgbButtons[nKey] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetGamePadAnglePress( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Press���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAnglePress( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngle[0][GpAngle];
}
//�v���C���[�w���
bool GetGamePadAnglePress( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngle[gamePadNum][GpAngle];
}

/*******************************************************************************
* �֐���	:bool GetGamePadAngleTrigger( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Trigger���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAngleTrigger( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngleTrigger[0][GpAngle];
}
//�v���C���[�w���
bool GetGamePadAngleTrigger( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return g_aGamePadInputAngleTrigger[gamePadNum][GpAngle];
}

/*******************************************************************************
* �֐���	:bool GetGamePadAngleRelease( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Release���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAngleRelease( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRelease[0][GpAngle] & 0x80 ) ? true : false );
}
//�v���C���[�w���
bool GetGamePadAngleRelease( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRelease[gamePadNum][GpAngle] & 0x80 ) ? true : false );
}


/*******************************************************************************
* �֐���	:bool GetGamePadAngleRepeat( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Repeat���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAngleRepeat( INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRepeat[0][GpAngle] & 0x80 ) ? true : false );
}
//�v���C���[�w���
bool GetGamePadAngleRepeat( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
{
	return ( ( g_aGamePadInputAngleRepeat[gamePadNum][GpAngle] & 0x80 ) ? true : false );
}

/*******************************************************************************
* �֐���	:bool GetAnyButtonPress( int gamePadNum )
*		�w��Q�[���p�b�h���牽������̓��͂����ꂽ����Ԃ��܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
* �߂�l	:bool	true:���͂���	false:���͂Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetAnyButtonPress( int gamePadNum )
{
	return g_pushAnyButton[gamePadNum];
}

//�ėp���͊֐�

/*******************************************************************************
* �֐���	:bool GetAnykeyPress( void )
*		��������̓��͂����ꂽ����Ԃ��܂��B
* ����		:void	��������
* �߂�l	:bool	true:���͂���	false:���͂Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetAnykeyPress( void )
{
	return g_pushAnykey;
}

/*******************************************************************************
* �֐���	:void setRepeatFps( int setNewFps )
*		���s�[�g���͂̎擾�^�C�~���O�𒲐����܂��B
* ����		:int	setNewFps	�V�K�ݒ�l�i�󔒁E�}�C�i�X�̒l�̏ꍇ�̓f�t�H���g�j
* �߂�l	:void	�߂�l�Ȃ�
* ����		:���s�[�g���͂̎擾�^�C�~���O�𒲐����܂��B
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
