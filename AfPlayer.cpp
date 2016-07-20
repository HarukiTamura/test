/*******************************************************************************
* �^�C�g��	:�v���C���[.cpp
* �t�@�C����:AfPlayer.cpp
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
#include "AfPlayer.h"
#include "AfCursor.h"
#include "AfBulletPlayer.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_PLAYER_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_PLAYER_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_PLAYER_DEFAULT_VECTOR	( 0.0f )
#define AF_PLAYER_DEFAULT_POSZ	( 1.0f )
#define AF_PLAYER_DEFAULT_LIFE		( 100 )


#define AF_PLAYER_SPIN	( 1.23 )

#define AF_PLAYER_VERTEX_MAX		( 4 )	/*�ō����_��*/
#define AF_PLAYER_POLYGON_NUM	( 2 )	/*���@�{����*/

#define AF_PLAYER_ANIM_MAX		( 2 )	/*�A�j���[�V�����R�}��*/
#define AF_PLAYER_ANIM_SPAN	( 1 )	/*�A�j���[�V�����Ԋu*/
#define AF_PLAYER_ANIM_DIV_X	( 2 )	/*��������*/
#define AF_PLAYER_ANIM_DIV_Y	( 5 )	/*����������*/

#define AF_PLAYER_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 0.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define SPEED_MAX	( 0.100f )	/*�g����E*/
#define SPEED_MIN 	( 0.001f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.23f )	/*������*/
#define ROT_ATTEN		( 0.85f )	/*������*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define DAMAGE_OUTPUT_FPS ( 60 )	/*�_���[�W���_��FPS*/

#define PGPCHANGE_HEIGHT_0	( SCREEN_HEIGHT * 0.5 + 22 )
#define PGPCHANGE_HEIGHT_1	( SCREEN_HEIGHT * 0.5 + 3 )
#define PGPCHANGE_HEIGHT_3	( SCREEN_HEIGHT * 0.5 - 3 )
#define PGPCHANGE_HEIGHT_4	( SCREEN_HEIGHT * 0.5 - 22 )

#define MOVE_PLUS_SPEED		( 5.4f )
#define MOVE_MINUS_SPEED	( -5.4f )
#define MOVE_PLUS_MAX	( 122.0f )
#define MOVE_PLUS_MIN	( -122.0f )

typedef enum
{
	PGP_H_PULL_UP = 0,
	PGP_L_PULL_UP,
	PGP_NEUTRAL,
	PGP_L_PULL_DOWN,
	PGP_H_PULL_DOWN
}PLAYER_GRAPHIC_PATTERN;

/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfPlayer = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfPlayer;	/*�v���C���[�e�N�X�`���̃|�C���^*/
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

static PLAYER playerState; 
static float g_BarrelRoll;
static float movePlus = 0;
/*******************************************************************************
* �֐���	:HRESULT InitAfPlayer( void )
*		�v���C���[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfPlayer( void )
{
	HRESULT result;
	result = InitDirectAfPlayer();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfPlayer();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfPlayer( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfPlayer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_PLAYER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfPlayer , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfPlayer.png" , &g_pTextureAfPlayer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfPlayer.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_PLAYER_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfPlayer->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfPlayer( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfPlayer( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_PLAYER_ANIM_DIV_X ) * AF_PLAYER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_PLAYER_ANIM_DIV_Y ) * AF_PLAYER_OUTPUT_SIZE );

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
	memset( &playerState , 0 , sizeof( PLAYER ) );

	playerState.posStatus.posX = AF_PLAYER_DEFAULT_POSX;
	playerState.posStatus.posY = AF_PLAYER_DEFAULT_POSY;
	playerState.posStatus.vector = AF_PLAYER_DEFAULT_VECTOR;
	
	g_BarrelRoll = 0.0f;
	

	playerState.intPosX = ( int )( playerState.posStatus.posX ) * 1000;
	playerState.intPosY = ( int )( playerState.posStatus.posY ) * 1000;

	playerState.nLife = AF_PLAYER_DEFAULT_LIFE;
	playerState.nCombo = 0;
	playerState.mode = PLAYER_MODE_NORMAL;   
	playerState.speed = SPEED_MIN;  

	g_posPolygon.x = AF_PLAYER_DEFAULT_POSX;
	g_posPolygon.y = AF_PLAYER_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* �֐���	:void UninitAfPlayer( void )
*		�v���C���[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfPlayer( void )
{
	if( g_pVtxBuffAfPlayer != NULL )
	{
		g_pVtxBuffAfPlayer->Release();
		g_pVtxBuffAfPlayer = NULL;
	}
	if( g_pTextureAfPlayer != NULL )
	{
		g_pTextureAfPlayer->Release();
		g_pTextureAfPlayer = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfPlayer( void )
*		�v���C���[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfPlayer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	PLAYER_GRAPHIC_PATTERN nowPatternNum = PGP_NEUTRAL;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	if( GetKeyboardPress( DIK_W ) || GetGamePadAnglePress( GP_ANGLE_UP ) )
	{
		if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
		{	
			g_movePolygon.z += SPIN_SPEED;
		}
		else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{
			g_movePolygon.z -= SPIN_SPEED;
		}
		else
		{
		}
	}
	else if( GetKeyboardPress( DIK_A ) || GetGamePadAnglePress( GP_ANGLE_LEFT ) )
	{
		if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
		{
			g_movePolygon.z += SPIN_SPEED;
		}
		else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{

		}
		else
		{	
			g_movePolygon.z += SPIN_SPEED;
		}
	}
	else if( GetKeyboardPress( DIK_S ) || GetGamePadAnglePress( GP_ANGLE_DOWN ) )
	{
		if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
		{	
			g_movePolygon.z -= SPIN_SPEED;
		}
		else
		{
		}
	}
	else if( GetKeyboardPress( DIK_D ) || GetGamePadAnglePress( GP_ANGLE_RIGHT ) )
	{
		g_movePolygon.z -= SPIN_SPEED;
	}
	else
	{

	}
	
	/*
	if( GetGamePadPress( 4 ) && !GetGamePadPress( 5 ) )
	{
		movePlus = ( movePlus + MOVE_MINUS_SPEED <= MOVE_PLUS_MIN ? MOVE_PLUS_MIN : movePlus + MOVE_MINUS_SPEED );
	}
	if( GetGamePadPress( 5 ) && !GetGamePadPress( 4 ) )
	{
		movePlus = ( movePlus + MOVE_PLUS_SPEED	>= MOVE_PLUS_MAX ? MOVE_PLUS_MAX : movePlus + MOVE_PLUS_SPEED );
	}
	else
	{
		movePlus = 0.0f;
	}
	g_movePolygon.x += movePlus;
	*/

	/*�ړ��ʔ��f*/
	g_movePolygon.x = GetCursorState()->posStatus.DrawPosX - playerState.posStatus.DrawPosX;
	g_movePolygon.y = GetCursorState()->posStatus.DrawPosY - playerState.posStatus.DrawPosY;

	g_BarrelRoll = 0;

	g_posPolygon.x += g_movePolygon.x * MOVE_ATTEN;
	g_posPolygon.y += g_movePolygon.y * MOVE_ATTEN;
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
	
	if(	playerState.speed < SPEED_MIN )
	{
		playerState.speed = SPEED_MIN;  
	}
	if(	playerState.speed > SPEED_MAX )
	{
		playerState.speed = SPEED_MAX;  
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
	playerState.posStatus.DrawPosX = g_posPolygon.x;
	playerState.posStatus.DrawPosY = g_posPolygon.y;
	g_rotPolygon.z *= ROT_ATTEN;
	playerState.posStatus.DrawVector = g_rotPolygon.z * 0.5;
	playerState.intPosX = ( int )( g_posPolygon.x * 1000 );
	playerState.intPosY = ( int )( g_posPolygon.y * 1000 );

#ifdef _DEBUG
	if( GetKeyboardTrigger( DIK_SPACE ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
	if( GetKeyboardPress( DIK_C ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
#endif

	/*�ړ��ʌ���*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 
	g_movePolygon.z *= SIPIN_ROT_ATTEN;	

	/*�|���S���g��*/
	/*
	if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	*/
	
	float rot = 0.0f;
	rot = g_rotPolygon.z + g_BarrelRoll;

	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[AF_PLAYER_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_4 )
	{
		nowPatternNum = PGP_H_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_3 )
	{
		nowPatternNum = PGP_L_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_0 )
	{
		nowPatternNum = PGP_H_PULL_UP;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_1 )
	{
		nowPatternNum = PGP_L_PULL_UP;
	}

	//�A�j���[�V�����ݒ�
	if( ++g_nCounterAnim == AF_PLAYER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_PLAYER_ANIM_DIV_X;
	texHeight = 1.0f / AF_PLAYER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_PLAYER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + nowPatternNum;
	
	//�e�N�X�`�����W�ݒ�
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	//nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + 1;

	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + 1 + texHeight;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + 1 + texHeight;

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfPlayer->Unlock();
}

/*******************************************************************************
* �֐���	:void UpdateAfPlayer( void )
*		�v���C���[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfPlayer2( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	PLAYER_GRAPHIC_PATTERN nowPatternNum = PGP_NEUTRAL;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[AF_PLAYER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	
	
	/*
	if( GetGamePadPress( 4 ) && !GetGamePadPress( 5 ) )
	{
		movePlus = ( movePlus + MOVE_MINUS_SPEED <= MOVE_PLUS_MIN ? MOVE_PLUS_MIN : movePlus + MOVE_MINUS_SPEED );
	}
	if( GetGamePadPress( 5 ) && !GetGamePadPress( 4 ) )
	{
		movePlus = ( movePlus + MOVE_PLUS_SPEED	>= MOVE_PLUS_MAX ? MOVE_PLUS_MAX : movePlus + MOVE_PLUS_SPEED );
	}
	else
	{
		movePlus = 0.0f;
	}
	g_movePolygon.x += movePlus;
	*/

	/*�ړ��ʔ��f*/
	g_movePolygon.x = GetCursorState()->posStatus.DrawPosX - playerState.posStatus.DrawPosX;
	g_movePolygon.y = GetCursorState()->posStatus.DrawPosY - playerState.posStatus.DrawPosY;

	g_BarrelRoll = 0;
	playerState.posStatus.DrawPosX = SCREEN_WIDTH / 2 + playerState.posStatus.DrawPosX * 0.75;
	playerState.posStatus.DrawPosY = SCREEN_WIDTH / 2 + playerState.posStatus.DrawPosY * 0.75;
	

	g_posPolygon.x += g_movePolygon.x * MOVE_ATTEN;
	g_posPolygon.y += g_movePolygon.y * MOVE_ATTEN;
	if( polygonMag <= 0.01 )
	{
		getViewStatus()->centerPosY *= 0.95;
	}

	polygonMag *= 0.75;
	//g_rotPolygon.z += g_movePolygon.z;

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
	
	if(	playerState.speed < SPEED_MIN )
	{
		playerState.speed = SPEED_MIN;  
	}
	if(	playerState.speed > SPEED_MAX )
	{
		playerState.speed = SPEED_MAX;  
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
	playerState.posStatus.DrawPosX = g_posPolygon.x;
	playerState.posStatus.DrawPosY = g_posPolygon.y;
	g_rotPolygon.z *= ROT_ATTEN;
	playerState.posStatus.DrawVector = g_rotPolygon.z * 0.5;
	playerState.intPosX = ( int )( g_posPolygon.x * 1000 );
	playerState.intPosY = ( int )( g_posPolygon.y * 1000 );

#ifdef _DEBUG
	if( GetKeyboardTrigger( DIK_SPACE ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
	if( GetKeyboardPress( DIK_C ) == true )
	{
		SetBulletPlayerState( g_posPolygon.x , g_posPolygon.y , AF_PLAYER_DEFAULT_POSZ , 0.0f );
	}
#endif

	/*�ړ��ʌ���*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 
	g_movePolygon.z *= SIPIN_ROT_ATTEN;	

	/*�|���S���g��*/
	/*
	if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	*/
	
	float rot = 0.0f;
	rot = g_rotPolygon.z + g_BarrelRoll;

	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[AF_PLAYER_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[AF_PLAYER_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rot ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_4 )
	{
		nowPatternNum = PGP_H_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY < PGPCHANGE_HEIGHT_3 )
	{
		nowPatternNum = PGP_L_PULL_DOWN;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_0 )
	{
		nowPatternNum = PGP_H_PULL_UP;
	}
	else if( playerState.posStatus.DrawPosY > PGPCHANGE_HEIGHT_1 )
	{
		nowPatternNum = PGP_L_PULL_UP;
	}

	//�A�j���[�V�����ݒ�
	if( ++g_nCounterAnim == AF_PLAYER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_PLAYER_ANIM_DIV_X;
	texHeight = 1.0f / AF_PLAYER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_PLAYER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + nowPatternNum;
	
	//�e�N�X�`�����W�ݒ�
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	//nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_PLAYER_ANIM_DIV_X ) ) + 1;

	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY + 1;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[AF_PLAYER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + 1 + texHeight;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[AF_PLAYER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + 1 + texHeight;
	
	playerState.intPosX = ( int )( playerState.posStatus.posX ) * 1000;
	playerState.intPosY = ( int )( playerState.posStatus.posY ) * 1000;
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfPlayer->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfPlayer( void )
*		�v���C���[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfPlayer( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfPlayer );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfPlayer , 0 , sizeof( VERTEX_2D ) );

	/*�v���C���[�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* �֐���	:void DrawAfPlayercore( void )
*		�v���C���[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfPlayercore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfPlayer );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfPlayer , 0 , sizeof( VERTEX_2D ) );
	
	
	/*�v���C���[�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_PLAYER_VERTEX_MAX , 2 );

#ifdef _DEBUG
	char txt[60];
	RECT rectW = { ( LONG )( 0 ) , ( LONG )( 100 ) , WINDOW_WIDTH , WINDOW_HEIGHT };
	sprintf( &txt[0] , "PposX:%3.6f \nPposY:%3.6f \nProtV:%3.6f", 
		playerState.posStatus.posX , playerState.posStatus.posY , playerState.posStatus.vector );
	getFont()->DrawText( NULL , &txt[0] , -1 , &rectW , DT_LEFT , D3DCOLOR_RGBA( 255 , 222 , 222 , 255 ) );
#endif
}

/*******************************************************************************
* �֐���	:PLAYER* GetPlayerState( void )
*		�v���C���[�̃X�e�[�^�X���擾���܂��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�v���C���[�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
PLAYER* GetPlayerState( void )
{
	return &playerState;
}