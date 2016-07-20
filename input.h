/*******************************************************************************
* �^�C�g��	:����.h
* �t�@�C����:cursor.h
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define DEFAULT_KEY_REPEAT_FPS	( 60 )

typedef enum
{
	GP_ANGLE_UP = 0,
	GP_ANGLE_DOWN,
	GP_ANGLE_LEFT,
	GP_ANGLE_RIGHT,
}INPUT_GAMEPAD_ANGLE;

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/


/*******************************************************************************
* �֐���	:void UpdateInput( void )
*		���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateInput( void );

/*******************************************************************************
* �֐���	:HRESULT InitKeyboard( HINSTANCE hInstance , HWND hWnd )
*		�L�[�{�[�h���͂̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitKeyboard( HINSTANCE hInstance , HWND hWnd );

/*******************************************************************************
* �֐���	:void UninitKeyboard( void )
*		�L�[�{�[�h���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitKeyboard( void );

/*******************************************************************************
* �֐���	:void UpdateKeyboard( void )
*		�L�[�{�[�h���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
void UpdateKeyboard( void );

/*******************************************************************************
* �֐���	:bool GetKeyboardPress( int nKey )
*		�L�[�{�[�h�����Press���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardPress( int nKey );
/*******************************************************************************
* �֐���	:bool GetKeyboardTrigger( int nKey )
*		�L�[�{�[�h�����Trigger���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardTrigger( int nKey );

/*******************************************************************************
* �֐���	:bool GetKeyboardRelease( int nKey )
*		�L�[�{�[�h�����Release���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardRelease( int nKey );

/*******************************************************************************
* �֐���	:bool GetKeyboardRepeat( int nKey )
*		�L�[�{�[�h�����Repeat���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetKeyboardRepeat( int nKey );

/*******************************************************************************
* �֐���	:HRESULT InitMouse( HINSTANCE hInstance , HWND hWnd )
*		�}�E�X���͂̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMouse( HINSTANCE hInstance , HWND hWnd );

/*******************************************************************************
* �֐���	:void UninitMouse( void )
*		�}�E�X���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMouse( void );

/*******************************************************************************
* �֐���	:void UpdateMouse( void )
*		�}�E�X���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
void UpdateMouse( void );

/*******************************************************************************
* �֐���	:bool GetMousePress( int nKey )
*		�}�E�X�����Press���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetMousePress( int nKey );

/*******************************************************************************
* �֐���	:bool GetMouseTrigger( int nKey )
*		�}�E�X�����Trigger���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetMouseTrigger( int nKey );

/*******************************************************************************
* �֐���	:bool GetMouseRelease( int nKey )
*		�}�E�X�����Release���͂��擾���܂��B
* ����		:int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetMouseRelease( int nKey );

/*******************************************************************************
* �֐���	:HRESULT InitGamePad( HINSTANCE hInstance , HWND hWnd )
*		�Q�[���p�b�h���͂̏������֐��ł��B
* ����		:HINSTANCE hInstance	�C���X�^���X�̃n���h��
*			 HWND hWnd				�E�B���h�E�̃n���h��
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitGamePad( HINSTANCE hInstance , HWND hWnd );

/*******************************************************************************
* �֐���	:void UninitGamePad( void )
*		�Q�[���p�b�h���͏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitGamePad( void );

/*******************************************************************************
* �֐���	:void UpdateGamePad( void )
*		�Q�[���p�b�h���͍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
void UpdateGamePad( void );

/*******************************************************************************
* �֐���	:bool GetGamePadPress( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Press���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadPress( int nKey );
//�v���C���[�w���
bool GetGamePadPress( int gamePadNum , int nKey );

/*******************************************************************************
* �֐���	:bool GetGamePadTrigger( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Trigger���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadTrigger( int nKey );
//�v���C���[�w���
bool GetGamePadTrigger( int gamePadNum , int nKey );

/*******************************************************************************
* �֐���	:bool GetGamePadRelease( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Release���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadRelease( int nKey );
//�v���C���[�w���
bool GetGamePadRelease( int gamePadNum , int nKey );

/*******************************************************************************
* �֐���	:bool GetGamePadRepeat( int gamePadNum , int nKey )
*		�Q�[���p�b�h�����Repeat���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 int nKey	��������L�[�ԍ��B(DirectInput�ɒ�`�ς݁uDIK_XXX�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadRepeat( int nKey );
//�v���C���[�w���
bool GetGamePadRepeat( int gamePadNum , int nKey );

/*******************************************************************************
* �֐���	:bool GetGamePadAnglePress( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Press���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAnglePress( INPUT_GAMEPAD_ANGLE GpAngle );
//�v���C���[�w���
bool GetGamePadAnglePress( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle );

/*******************************************************************************
* �֐���	:bool GetGamePadAngleTrigger( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Trigger���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAngleTrigger( INPUT_GAMEPAD_ANGLE GpAngle );
//�v���C���[�w���
bool GetGamePadAngleTrigger( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle );

/*******************************************************************************
* �֐���	:bool GetGamePadAngleRelease( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Release���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAngleRelease( INPUT_GAMEPAD_ANGLE GpAngle );
//�v���C���[�w���
bool GetGamePadAngleRelease( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle );

/*******************************************************************************
* �֐���	:bool GetGamePadAngleRepeat( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle )
*		�Q�[���p�b�h�����Repeat���͂��擾���܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
*			 INPUT_GAMEPAD_ANGLE GpAngle	�擾��������B(Input.h�ɒ�`�ς݁uINPUT_GP�`�v)
* �߂�l	:bool	true:�L��	false:����
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetGamePadAngleRepeat( INPUT_GAMEPAD_ANGLE GpAngle );
//�v���C���[�w���
bool GetGamePadAngleRepeat( int gamePadNum , INPUT_GAMEPAD_ANGLE GpAngle );

/*******************************************************************************
* �֐���	:bool GetAnyButtonPress( int gamePadNum )
*		�w��Q�[���p�b�h���牽������̓��͂����ꂽ����Ԃ��܂��B
* ����		:int gamePadNum	�Q�[���p�b�h�̔ԍ�
* �߂�l	:bool	true:���͂���	false:���͂Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetAnyButtonPress( int gamePadNum );

/*******************************************************************************
* �֐���	:bool GetAnykeyPress( void )
*		��������̓��͂����ꂽ����Ԃ��܂��B
* ����		:void	��������
* �߂�l	:bool	true:���͂���	false:���͂Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B
*******************************************************************************/
bool GetAnykeyPress( void );

/*******************************************************************************
* �֐���	:void setRepeatFps( int setNewFps )
*		���s�[�g���͂̎擾�^�C�~���O�𒲐����܂��B
* ����		:int	setNewFps	�V�K�ݒ�l�i�󔒂̏ꍇ�̓f�t�H���g�j
* �߂�l	:void	�߂�l�Ȃ�
* ����		:���s�[�g���͂̎擾�^�C�~���O�𒲐����܂��B
*******************************************************************************/
void setRepeatFps( int setNewFps );
void setRepeatFps( void );

#endif