/*******************************************************************************
* �^�C�g��	:�J�[�\��.cpp
* �t�@�C����:AfPauseButton.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "game.h"
#include "input.h"
#include "AfPauseButton.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_PAUSEBUTTON_DEFAULT_POSX	( 100 )
#define AF_PAUSEBUTTON_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_PAUSEBUTTON_DEFAULT_VECTOR	( 0.0f )

#define ICON_SPACE	( 100 )
#define AF_PAUSEBUTTON_SPIN	( 0 )
#define USE_ICON	( 3 )	//�\���A�C�R����

#define AF_PAUSEBUTTON_POLYGON_NUM	( USE_ICON )	/*�g�p�|���S����*/
#define AF_PAUSEBUTTON_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_PAUSEBUTTON_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_PAUSEBUTTON_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_PAUSEBUTTON_ANIM_DIV_X	( 3 )	/*��������*/
#define AF_PAUSEBUTTON_ANIM_DIV_Y	( 1 )	/*����������*/

#define AF_PAUSEBUTTON_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 1.3f )	/*�ړ���*/
#define SPIN_SPEED	( 0.06f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.12f )	/*������*/
#define ROT_ATTEN		( 0.85f )	/*������*/
#define SIPIN_ROT_ATTEN ( 0.12f )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfPauseButton = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfPauseButton;	/*�J�[�\���e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;
static int g_nPatternAnim = 0;

static PAUSEBUTTON cursorState;
static int selectCursorNum;

/*******************************************************************************
* �֐���	:HRESULT InitAfPauseButton( void )
*		�J�[�\���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfPauseButton( void )
{
	HRESULT result;
	result = InitDirectAfPauseButton();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfPauseButton();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfPauseButton( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfPauseButton( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_PAUSEBUTTON_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfPauseButton , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfPauseButton.png" , &g_pTextureAfPauseButton );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfPauseButton.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfPauseButton->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_PAUSEBUTTON_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfPauseButton->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfPauseButton( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfPauseButton( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_PAUSEBUTTON_ANIM_DIV_X ) * AF_PAUSEBUTTON_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_PAUSEBUTTON_ANIM_DIV_Y ) * AF_PAUSEBUTTON_OUTPUT_SIZE );

	//���O�T�C�Y�ϊ�
	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	
	//������
	memset( &cursorState , 0 , sizeof( PAUSEBUTTON ) );

	cursorState.posStatus.posX = AF_PAUSEBUTTON_DEFAULT_POSX;
	cursorState.posStatus.posY = AF_PAUSEBUTTON_DEFAULT_POSY;
	cursorState.posStatus.vector = AF_PAUSEBUTTON_DEFAULT_VECTOR;
	
	cursorState.intPosX = ( int )( cursorState.posStatus.posX ) * 1000;
	cursorState.intPosY = ( int )( cursorState.posStatus.posY ) * 1000;

	cursorState.mode = PAUSEBUTTON_MODE_NORMAL;   
	
	g_posPolygon.x = AF_PAUSEBUTTON_DEFAULT_POSX;
	g_posPolygon.y = AF_PAUSEBUTTON_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1.0f;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
	selectCursorNum = 0;
}

/*******************************************************************************
* �֐���	:void UninitAfPauseButton( void )
*		�J�[�\���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfPauseButton( void )
{
	if( g_pVtxBuffAfPauseButton != NULL )
	{
		g_pVtxBuffAfPauseButton->Release();
		g_pVtxBuffAfPauseButton = NULL;
	}
	if( g_pTextureAfPauseButton != NULL )
	{
		g_pTextureAfPauseButton->Release();
		g_pTextureAfPauseButton = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfPauseButton( void )
*		�J�[�\���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfPauseButton( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	int nowPatternNum = 0;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfPauseButton->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
	{	
		selectCursorNum = ( selectCursorNum - 1 < 0 ? 0 : selectCursorNum - 1 );
	}
	else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
	{
		selectCursorNum = ( selectCursorNum + 1 < USE_ICON ? USE_ICON : selectCursorNum + 1 );
	}
	
	g_posPolygon.x = AF_PAUSEBUTTON_DEFAULT_POSX;

	for( int cnt = 0; cnt < USE_ICON; cnt++ )
	{
		g_posPolygon.x += ICON_SPACE;
		if( cursorState.mode == PAUSEBUTTON_MODE_NORMAL )
		{
			polygonMag = 4.0f;

			pVtx[cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		else
		{
			polygonMag = 1.0f;//+= MAG_SPEED;

			pVtx[cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}

		/*�ړ��ʔ��f*/
		g_posPolygon.x += g_movePolygon.x;
		g_posPolygon.y += g_movePolygon.y;
		g_rotPolygon.z += g_movePolygon.z;

		//�I�[�o�[�C��
		if( g_posPolygon.x < SCREEN_LEFT )
		{
			g_posPolygon.x = SCREEN_LEFT;
		}					 
		if( g_posPolygon.x > SCREEN_RIGHT )
		{
			g_posPolygon.x = SCREEN_RIGHT;
		}
		if( g_posPolygon.y < FIELD_TOP )
		{
			g_posPolygon.y = FIELD_TOP;
		}
		if( g_posPolygon.y > FIELD_HEIGHT )
		{
			g_posPolygon.y = FIELD_HEIGHT;
		}
	
		if( g_rotPolygon.z > D3DX_PI )
		{
			g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
		}
		if( g_rotPolygon.z < -D3DX_PI )
		{
			g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
		}
	
		//���K�l�ݒ�
		cursorState.posStatus.DrawPosX = g_posPolygon.x;
		cursorState.posStatus.DrawPosY = g_posPolygon.y;
		//g_rotPolygon.z *= ROT_ATTEN;
		cursorState.posStatus.DrawVector = g_rotPolygon.z * 0.5;
		cursorState.intPosX = ( int )( g_posPolygon.x * 1000 );
		cursorState.intPosY = ( int )( g_posPolygon.y * 1000 );

		/*�ړ��ʌ���*/
		g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
		g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 
		g_movePolygon.z *= SIPIN_ROT_ATTEN;	
	
		/*���_���W�ݒ�*/
		pVtx[cnt].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
		pVtx[cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		//�A�j���[�V�����ݒ�
		if( ++g_nCounterAnim == AF_PAUSEBUTTON_ANIM_SPAN )
		{
			g_nCounterAnim = 0;
			g_nPatternAnim = ( ++g_nPatternAnim == AF_PAUSEBUTTON_ANIM_MAX ? 0 : g_nPatternAnim ); 
		}
	
		texWidth = 1.0f / AF_PAUSEBUTTON_ANIM_DIV_X;
		texHeight = 1.0f / AF_PAUSEBUTTON_ANIM_DIV_Y;
		nowPatternX = ( float )( g_nPatternAnim % AF_PAUSEBUTTON_ANIM_DIV_X );
		nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PAUSEBUTTON_ANIM_DIV_X ) ) + nowPatternNum;
	
		//�e�N�X�`�����W�ݒ�
		pVtx[cnt].tex.x = 0.33f * cnt;
		pVtx[cnt].tex.y = 0.0f;
		pVtx[cnt + 1].tex.x = 0.33f * cnt;
		pVtx[cnt + 1].tex.y = 0.0f;
		pVtx[cnt + 2].tex.x = 0.33f * cnt + 0.33f;
		pVtx[cnt + 2].tex.y = 1.0f;
		pVtx[cnt + 3].tex.x = 0.33f * cnt + 0.33f;
		pVtx[cnt + 3].tex.y = 1.0f;
	}

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfPauseButton->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfPauseButton( void )
*		�J�[�\���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfPauseButton( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfPauseButton );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfPauseButton , 0 , sizeof( VERTEX_2D ) );

	for( int cnt = 0; cnt < USE_ICON; cnt++ )
	{
		/*�J�[�\���̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , cnt * AF_PAUSEBUTTON_VERTEX_MAX , 2 );
	}
}
