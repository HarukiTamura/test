/*******************************************************************************
* �^�C�g��	:�v���C���[01.cpp
* �t�@�C����:player01.cpp
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
#include "input.h"
#include "player01.h"
#include "bullet.h"
#include "unoDirectX.h"
#include "comboNumber.h"
#include "explosion.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define PLAYER01_VERTEX_MAX		( 4 )	/*�ō����_��*/
#define PLAYER01_POLYGON_NUM	( 2 )	/*���@�{����*/

#define PLAYER01_ANIM_MAX		( 3 )	/*�A�j���[�V�����R�}��*/
#define PLAYER01_ANIM_SPAN	( 5 )	/*�A�j���[�V�����Ԋu*/
#define PLAYER01_ANIM_DIV_X	( 3 )	/*��������*/
#define PLAYER01_ANIM_DIV_Y	( 2 )	/*����������*/

#define PLAYER01_OUTPUT_SIZE	( 0.4 ) /*�����\���{��*/ 

#define MOVE_NUM ( 0.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.12f )	/*������*/

#define SHOT_COOLDOWN_OVERHEAT	( 50 )	/*�V���b�g�ɂ��r�oLIAR�l*/
#define SHOT_COOLDOWN_HYPERMODE	( 3 )	/*�V���b�g�ɂ��r�oLIAR�l*/
#define SHOT_COOLDOWN_NUM		( 80 )	/*�V���b�g�ɂ��r�oLIAR�l*/

#define DAMAGE_OUTPUT_FPS ( 60 )	/*�_���[�W���_��FPS*/
/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffplayer01 = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureplayer01;	/*�v���C���[01�e�N�X�`���̃|�C���^*/
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
static int outputSizeX , outputSizeY;
static int colorChange;	//�e�������Ƃ̐F�����p���l
static bool colorHalationFlag = false;
static int damageColorChanger = 0; 

/*******************************************************************************
* �֐���	:HRESULT InitPlayer01( void )
*		�v���C���[01�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitPlayer01( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * PLAYER01_VERTEX_MAX * PLAYER01_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffplayer01 , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/player01.png" , &g_pTextureplayer01 );
	D3DXGetImageInfoFromFile( "data/TEXTURE/player01.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffplayer01->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , PLAYER01_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffplayer01->Unlock();
	
	outputSizeX = ( int )( ( info.Width / PLAYER01_ANIM_DIV_X ) * PLAYER01_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / PLAYER01_ANIM_DIV_Y ) * PLAYER01_OUTPUT_SIZE );

	g_posPolygon.x = PLAYER01_OUTPUT_POS_X;
	g_posPolygon.y = PLAYER01_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );


	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	memset( &playerState , 0 , sizeof( PLAYER ) );
	
	colorChange = 0;
	colorHalationFlag = false;
	damageColorChanger = 0; 

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitPlayer01( void )
*		�v���C���[01�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitPlayer01( void )
{
	if( g_pVtxBuffplayer01 != NULL )
	{
		g_pVtxBuffplayer01->Release();
		g_pVtxBuffplayer01 = NULL;
	}
	if( g_pTextureplayer01 != NULL )
	{
		g_pTextureplayer01->Release();
		g_pTextureplayer01 = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdatePlayer01( void )
*		�v���C���[01�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdatePlayer01( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	static int fpsCnt;
	bool shotFlag = false;

	fpsCnt = ( fpsCnt + 1 > 60 ? 0 : fpsCnt + 1 );

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffplayer01->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	/*���[�h���̏���*/
	if( playerState.mode !=	PLAYER_MODE_START )
	{
		if( playerState.liarNum >= LIAR_MAX )
		{
			playerState.mode = PLAYER_MODE_OVERHEAT;
		}
		if( playerState.mode == PLAYER_MODE_NORMAL )
		{
			if( ( GetKeyboardTrigger( DIK_C ) || GetGamePadTrigger( 1 ) )&& playerState.liarNum > LIAR_MIN )
			{
				colorChange = 0;
				playerState.mode = PLAYER_MODE_HYPERMODE;
				colorHalationFlag = false;
			}
			playerState.liarNum = ( playerState.liarNum < LIAR_MAX ? playerState.liarNum + 1 : playerState.liarNum );
			//playerState.liarBarrier = false;
			
			colorChange = ( colorChange >= 0 ? 0 : colorChange - 5 );
			pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );

			pVtx[PLAYER01_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );



			if( damageColorChanger > 0 )
			{
				damageColorChanger--;
				if( damageColorChanger % 10 >= 5 )
				{
					pVtx[0].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
					pVtx[1].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
					pVtx[2].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
					pVtx[3].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
				}
				if( damageColorChanger > DAMAGE_OUTPUT_FPS * 0.33 )
				{
					if( damageColorChanger % 10 >= 5 )
					{
						pVtx[ 0].col = D3DCOLOR_RGBA( 222 , 12 , 0 , 245 );
						pVtx[ 1].col = D3DCOLOR_RGBA( 222 , 12 , 0 , 245 );
						pVtx[ 2].col = D3DCOLOR_RGBA( 222 , 12 , 0 , 245 );
						pVtx[ 3].col = D3DCOLOR_RGBA( 222 , 12 , 0 , 245 );
					}
					else
					{
						pVtx[ 0].col = D3DCOLOR_RGBA( 0 , 155 , 155 , 255 );
						pVtx[ 1].col = D3DCOLOR_RGBA( 0 , 155 , 155 , 255 );
						pVtx[ 2].col = D3DCOLOR_RGBA( 0 , 155 , 155 , 255 );
						pVtx[ 3].col = D3DCOLOR_RGBA( 0 , 155 , 155 , 255 );
					}
				}
			}
		}
		else if( playerState.mode == PLAYER_MODE_DAMAGE )
		{
			damageColorChanger = DAMAGE_OUTPUT_FPS;
			
			pVtx[PLAYER01_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 ,122 , 122 , 255 );
			playerState.mode = PLAYER_MODE_NORMAL;
		}
		else if( playerState.mode == PLAYER_MODE_OVERHEAT )
		{
			pVtx[0].col = D3DCOLOR_RGBA( 0 , 0 , 255 , 255 );
			pVtx[1].col = D3DCOLOR_RGBA( 0 , 0 , 255 , 255 );
			pVtx[2].col = D3DCOLOR_RGBA( 0 , 0 , 255 , 255 );
			pVtx[3].col = D3DCOLOR_RGBA( 0 , 0 , 255 , 255 );

			pVtx[PLAYER01_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			
			if( playerState.liarNum <= LIAR_MIN )
			{
				playerState.mode = PLAYER_MODE_NORMAL;
			}
			playerState.liarNum = ( playerState.liarNum > LIAR_MIN + SHOT_COOLDOWN_OVERHEAT ? playerState.liarNum - SHOT_COOLDOWN_OVERHEAT : LIAR_MIN );
			if( playerState.liarNum >= LIAR_MIN * 0.7 )
			{
				DeleteBullet( playerState.intPosX , playerState.intPosY , 300000 );
			}
			playerState.liarBarrier = false;
		}
		else if( playerState.mode == PLAYER_MODE_HYPERMODE )
		{
			playerState.liarNum = ( playerState.liarNum > LIAR_MIN + SHOT_COOLDOWN_HYPERMODE ? playerState.liarNum - SHOT_COOLDOWN_HYPERMODE : LIAR_MIN );
			playerState.liarBarrier = true;
			
			if( playerState.liarNum >= 310 )
			{
				if( colorHalationFlag == true )
				{
					colorChange += 35;
					if( colorChange >= 255 )
					{
						colorChange = 255;
						colorHalationFlag = false;
					}
				}
				else
				{	
					colorChange -= 35;
					if( colorChange	<= 0 )
					{
						colorChange = 0;
						colorHalationFlag = true;
					}
				}

			}
			else
			{
				colorChange = ( colorChange >= 255 ? 255 : colorChange + 5 );
			}
			pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );

			pVtx[PLAYER01_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
			pVtx[PLAYER01_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 - colorChange , 255 - colorChange , 255 );
		}
	}
	else
	{
		//��ʊO�����ʂɐN�����Ă��鉉�o��ǉ��\��
	}

	if( ++g_nCounterAnim == PLAYER01_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == PLAYER01_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / PLAYER01_ANIM_DIV_X;
	texHeight = 1.0f / PLAYER01_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % PLAYER01_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / PLAYER01_ANIM_DIV_X ) );
	
	
	//�e�N�X�`�����W�ݒ�
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	
	nowPatternY = ( float )( ( int )( g_nPatternAnim / PLAYER01_ANIM_DIV_X ) ) + 1;

	pVtx[PLAYER01_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[PLAYER01_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY + 1;
	pVtx[PLAYER01_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[PLAYER01_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY + 1;
	pVtx[PLAYER01_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[PLAYER01_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + 1 + texHeight;
	pVtx[PLAYER01_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[PLAYER01_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + 1 + texHeight;


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
	playerState.posX = g_posPolygon.x;
	playerState.posY = g_posPolygon.y;
	playerState.intPosX = ( int )( g_posPolygon.x * 1000 );
	playerState.intPosY = ( int )( g_posPolygon.y * 1000 );


	if( g_posPolygon.x < FIELD_LEFT )
	{
		g_posPolygon.x = FIELD_LEFT;
	}
	if( g_posPolygon.x > FIELD_RIGHT )
	{
		g_posPolygon.x = FIELD_RIGHT;
	}
	if( g_posPolygon.y < FIELD_TOP )
	{
		g_posPolygon.y = FIELD_TOP;
	}
	if( g_posPolygon.y > FIELD_HEIGHT )
	{
		g_posPolygon.y = FIELD_HEIGHT;
	}
	/*�ړ��ʌ���*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 	
		
	/*�|���S����]*/
	/*
	if( GetKeyboardPress( DIK_LEFT ) )
	{
		g_rotPolygon.z -= SPIN_SPEED;
	}
	if( GetKeyboardPress( DIK_RIGHT ) )
	{
		g_rotPolygon.z += SPIN_SPEED;
	}
	*/
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}

	////�����Ɏ��@���ړ������ɉ񂵂Ă݂�
	//g_movePolygon.x *= 1.06;
	//g_movePolygon.y *= 1.06;
	//g_rotPolygon.z = atan2f( -g_movePolygon.x , -g_movePolygon.y );
	
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
	
	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[PLAYER01_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[PLAYER01_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[PLAYER01_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[PLAYER01_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffplayer01->Unlock();
	
	if( ( GetKeyboardPress( DIK_B ) || GetGamePadPress( 0 ) ) && fpsCnt % 3 == 0 )
	{
		shotFlag = true;
	}
#ifdef _DEBUG
	if( GetKeyboardPress( DIK_V ) )
	{
		shotFlag = true;
	}
#endif
	if( GetKeyboardTrigger( DIK_SPACE ) || GetGamePadTrigger( 0 ) )
	{
		fpsCnt = 0;
		shotFlag = true;
	}
	if( ( GetKeyboardRepeat( DIK_SPACE ) || GetGamePadRepeat( 0 ) ) && fpsCnt % 15 == 0 )
	{
		shotFlag = true;
	}
	if( shotFlag == true )
	{
		playerState.liarNum = ( playerState.liarNum > LIAR_MIN + SHOT_COOLDOWN_NUM ? playerState.liarNum - SHOT_COOLDOWN_NUM : LIAR_MIN );
		SetBulletPlayer( g_posPolygon.x * 1000 , g_posPolygon.y * 1000 , 0 , 0 , -10000 , 0 , 12 );
		SetBulletPlayer( g_posPolygon.x * 1000 , g_posPolygon.y * 1000 , 0 , 2000 , -8000 , 0 , 12 );
		SetBulletPlayer( g_posPolygon.x * 1000 , g_posPolygon.y * 1000 , 0 , -2000 , -8000 , 0 , 12 );
	}
#ifdef _DEBUG
	if( GetKeyboardTrigger( DIK_Z ) )
	{
		SetBulletLiar( g_posPolygon.x * 1000 , g_posPolygon.y * 1000 , 0 , 0 , -10000 , 0 , 26 , BULLET_HOMING );
	}
	if( GetKeyboardTrigger( DIK_X ) )
	{
		for( int i = 0; i<= 20; i++ )
		{
			SetBulletEnemy( g_posPolygon.x * 1000 , g_posPolygon.y * 1000 - 2000 , 0 , 900 * sin( D3DX_PI * 0.05 * i ) , 900  * cos( D3DX_PI * 0.05 * i ) , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}
#endif
}

/*******************************************************************************
* �֐���	:void DrawPlayer01( void )
*		�v���C���[01�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawPlayer01( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureplayer01 );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffplayer01 , 0 , sizeof( VERTEX_2D ) );

	/*�v���C���[01�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* �֐���	:void DrawPlayer01core( void )
*		�v���C���[01�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawPlayer01core( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureplayer01 );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffplayer01 , 0 , sizeof( VERTEX_2D ) );
	
	
	/*�v���C���[01�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + PLAYER01_VERTEX_MAX , 2 );
}

/*******************************************************************************
* �֐���	:PLAYER* GetPlayerState01( void )
*		�v���C���[01�̃X�e�[�^�X���擾���܂��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�v���C���[01�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
PLAYER* GetPlayerState01( void )
{
	return &playerState;
}