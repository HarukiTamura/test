/*******************************************************************************
* �^�C�g��	:�J�[�\��.cpp
* �t�@�C����:AfCursor.cpp
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
#include "AfCursor.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_CURSOR_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_CURSOR_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_CURSOR_DEFAULT_VECTOR	( 0.0f )

#define AF_CURSOR_SPIN	( 1.23 )

#define AF_CURSOR_VERTEX_MAX	( 4 )	/*�ō����_��*/
#define AF_CURSOR_POLYGON_NUM	( 4 )	/*�g�p�|���S����*/

#define AF_CURSOR_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_CURSOR_ANIM_SPAN		( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_CURSOR_ANIM_DIV_X	( 2 )	/*��������*/
#define AF_CURSOR_ANIM_DIV_Y	( 2 )	/*����������*/

#define AF_CURSOR_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 1.3f )	/*�ړ���*/
#define SPIN_SPEED	( 0.06f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.12f )	/*������*/
#define ROT_ATTEN		( 0.85f )	/*������*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define DAMAGE_OUTPUT_FPS ( 60 )	/*�_���[�W���_��FPS*/

#define PGPCHANGE_HEIGHT_0	( SCREEN_HEIGHT * 0.5 + 22 )
#define PGPCHANGE_HEIGHT_1	( SCREEN_HEIGHT * 0.5 + 3 )
#define PGPCHANGE_HEIGHT_3	( SCREEN_HEIGHT * 0.5 - 3 )
#define PGPCHANGE_HEIGHT_4	( SCREEN_HEIGHT * 0.5 - 22 )


/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfCursor = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfCursor;	/*�J�[�\���e�N�X�`���̃|�C���^*/
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

static CURSOR cursorState; 

/*******************************************************************************
* �֐���	:HRESULT InitAfCursor( void )
*		�J�[�\���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfCursor( void )
{
	HRESULT result;
	result = InitDirectAfCursor();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfCursor();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfCursor( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfCursor( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_CURSOR_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfCursor , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfCursor.png" , &g_pTextureAfCursor );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfCursor.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_CURSOR_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfCursor->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfCursor( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfCursor( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_CURSOR_ANIM_DIV_X ) * AF_CURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_CURSOR_ANIM_DIV_Y ) * AF_CURSOR_OUTPUT_SIZE );

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
	memset( &cursorState , 0 , sizeof( CURSOR ) );

	cursorState.posStatus.posX = AF_CURSOR_DEFAULT_POSX;
	cursorState.posStatus.posY = AF_CURSOR_DEFAULT_POSY;
	cursorState.posStatus.vector = AF_CURSOR_DEFAULT_VECTOR;
	
	cursorState.intPosX = ( int )( cursorState.posStatus.posX ) * 1000;
	cursorState.intPosY = ( int )( cursorState.posStatus.posY ) * 1000;

	cursorState.mode = CURSOR_MODE_NORMAL;   
	
	g_posPolygon.x = AF_CURSOR_DEFAULT_POSX;
	g_posPolygon.y = AF_CURSOR_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* �֐���	:void UninitAfCursor( void )
*		�J�[�\���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfCursor( void )
{
	if( g_pVtxBuffAfCursor != NULL )
	{
		g_pVtxBuffAfCursor->Release();
		g_pVtxBuffAfCursor = NULL;
	}
	if( g_pTextureAfCursor != NULL )
	{
		g_pTextureAfCursor->Release();
		g_pTextureAfCursor = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfCursor( void )
*		�J�[�\���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfCursor( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	int nowPatternNum = 0;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtx[AF_CURSOR_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_CURSOR_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_CURSOR_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_CURSOR_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	
	if( cursorState.mode == CURSOR_MODE_NORMAL )
	{
		g_movePolygon.z += SPIN_SPEED;
		nowPatternNum = 0;
	}
	else
	{
		nowPatternNum = 1;
	}

	if( GetKeyboardPress( DIK_W ) || GetGamePadAnglePress( GP_ANGLE_UP ) )
	{
		if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
		{	
			g_movePolygon.x += ( float )( -sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_movePolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{
			g_movePolygon.x += ( float )( sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_movePolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else
		{
			g_movePolygon.y -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
	{
		if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
			g_movePolygon.x += -sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_movePolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{

		}
		else
		{	
			g_movePolygon.x -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
	{
		if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{	
			g_movePolygon.x += sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_movePolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else
		{
			g_movePolygon.y += MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
	{
		g_movePolygon.x += MOVE_NUM;
	}
	else
	{

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
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_CURSOR_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[AF_CURSOR_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_CURSOR_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_CURSOR_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );


	//�A�j���[�V�����ݒ�
	if( ++g_nCounterAnim == AF_CURSOR_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_CURSOR_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_CURSOR_ANIM_DIV_X;
	texHeight = 1.0f / AF_CURSOR_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_CURSOR_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_CURSOR_ANIM_DIV_X ) ) + nowPatternNum;
	
	//�e�N�X�`�����W�ݒ�
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_CURSOR_ANIM_DIV_X ) ) + 1;

	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfCursor->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfCursor( void )
*		�J�[�\���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfCursor( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfCursor );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfCursor , 0 , sizeof( VERTEX_2D ) );

	/*�J�[�\���̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* �֐���	:void DrawAfCursorcore( void )
*		�J�[�\���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfCursorcore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfCursor );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfCursor , 0 , sizeof( VERTEX_2D ) );
	
	
	/*�J�[�\���̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_CURSOR_VERTEX_MAX , 2 );

#ifdef _DEBUG
	char txt[60];
	RECT rectW = { ( LONG )( 0 ) , ( LONG )( 100 ) , WINDOW_WIDTH , WINDOW_HEIGHT };
	sprintf( &txt[0] , "PposX:%3.6f \nPposY:%3.6f \nProtV:%3.6f", 
		cursorState.posStatus.posX , cursorState.posStatus.posY , cursorState.posStatus.vector );
	getFont()->DrawText( NULL , &txt[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 222 , 222 , 255 ) );
#endif
}

/*******************************************************************************
* �֐���	:CURSOR* GetCursorState( void )
*		�J�[�\���̃X�e�[�^�X���擾���܂��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�J�[�\���̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
CURSOR* GetCursorState( void )
{
	return &cursorState;
}