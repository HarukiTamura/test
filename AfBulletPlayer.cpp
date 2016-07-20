/*******************************************************************************
* �^�C�g��	:���e.cpp
* �t�@�C����:AfBulletPlayer.cpp
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
#include "AfBulletPlayer.h"
#include "AfPlayer.h"
#include "AfCursor.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_BULLET_PLAYER_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_BULLET_PLAYER_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_BULLET_PLAYER_DEFAULT_VECTOR	( 0.0f )
#define AF_BULLET_PLAYER_DEFAULT_POSZ	( 0.0f )
#define AF_BULLET_PLAYER_DEFAULT_LIFE		( 100 )
#define AF_BULLET_PLAYER_DEFAULT_SCORE		( 10 )

#define AF_BULLET_PLAYER_SPEED	( 0.016f )
#define AF_BULLET_PLAYER_OUTSCREEN	( 1.20f )
#define AF_BULLET_PLAYER_DELETE		( 0.00f )

#define AF_BULLET_PLAYER_SPIN	( 1.23 )

#define AF_BULLET_PLAYER_VERTEX_MAX		( 4 )	/*�ō����_��*/
#define AF_BULLET_PLAYER_POLYGON_NUM	( BULLET_PLAYER_MAX_NUM )	/*���@�{����*/

#define AF_BULLET_PLAYER_ANIM_MAX		( 3 )	/*�A�j���[�V�����R�}��*/
#define AF_BULLET_PLAYER_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_BULLET_PLAYER_ANIM_DIV_X	( 3 )	/*��������*/
#define AF_BULLET_PLAYER_ANIM_DIV_Y	( 5 )	/*����������*/

#define AF_BULLET_PLAYER_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 0.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.23f )	/*������*/
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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfBulletPlayer = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfBulletPlayer;	/*���e�e�N�X�`���̃|�C���^*/
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

static BULLET_PLAYER_STATUS bulletPlayerState[BULLET_PLAYER_MAX_NUM]; 

/*******************************************************************************
* �֐���	:HRESULT InitAfBulletPlayer( void )
*		���e�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfBulletPlayer( void )
{
	HRESULT result;
	result = InitDirectAfBulletPlayer();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfBulletPlayer();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfBulletPlayer( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfBulletPlayer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_BULLET_PLAYER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfBulletPlayer , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfBulletPlayer.png" , &g_pTextureAfBulletPlayer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfBulletPlayer.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfBulletPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_BULLET_PLAYER_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfBulletPlayer->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfBulletPlayer( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfBulletPlayer( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_BULLET_PLAYER_ANIM_DIV_X ) * AF_BULLET_PLAYER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_BULLET_PLAYER_ANIM_DIV_Y ) * AF_BULLET_PLAYER_OUTPUT_SIZE );

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
	memset( &bulletPlayerState , 0 , sizeof( BULLET_PLAYER_STATUS ) * BULLET_PLAYER_MAX_NUM );
	
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		bulletPlayerState[cnt].bUse = false;
		bulletPlayerState[cnt].posStatus.posX = AF_BULLET_PLAYER_DEFAULT_POSX;
		bulletPlayerState[cnt].posStatus.posY = AF_BULLET_PLAYER_DEFAULT_POSY;
		bulletPlayerState[cnt].posStatus.vector = AF_BULLET_PLAYER_DEFAULT_VECTOR;
		bulletPlayerState[cnt].moveZ = 0.0f;
		bulletPlayerState[cnt].posZ = AF_BULLET_PLAYER_DEFAULT_POSZ;

		bulletPlayerState[cnt].intPosX = ( int )( bulletPlayerState[cnt].posStatus.posX ) * 1000;
		bulletPlayerState[cnt].intPosY = ( int )( bulletPlayerState[cnt].posStatus.posY ) * 1000;
				  
		bulletPlayerState[cnt].nLife = AF_BULLET_PLAYER_DEFAULT_LIFE;
	}

	g_posPolygon.x = AF_BULLET_PLAYER_DEFAULT_POSX;
	g_posPolygon.y = AF_BULLET_PLAYER_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* �֐���	:void UninitAfBulletPlayer( void )
*		���e�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfBulletPlayer( void )
{
	if( g_pVtxBuffAfBulletPlayer != NULL )
	{
		g_pVtxBuffAfBulletPlayer->Release();
		g_pVtxBuffAfBulletPlayer = NULL;
	}
	if( g_pTextureAfBulletPlayer != NULL )
	{
		g_pTextureAfBulletPlayer->Release();
		g_pTextureAfBulletPlayer = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfBulletPlayer( void )
*		���e�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfBulletPlayer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfBulletPlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		if( bulletPlayerState[cnt].bUse == true )
		{
			//�s��
			bulletPlayerState[cnt].moveZ = -AF_BULLET_PLAYER_SPEED;
			
			//���S�m�F
			if( bulletPlayerState[cnt].nLife <= 0 || bulletPlayerState[cnt].posZ >= AF_BULLET_PLAYER_OUTSCREEN || bulletPlayerState[cnt].posZ <= AF_BULLET_PLAYER_DELETE )
			{
				bulletPlayerState[cnt].bUse = false;
			}
			//�����ʒu�m��

			//�`����W�E�T�C�Y�m��
			bulletPlayerState[cnt].posZ += bulletPlayerState[cnt].moveZ;
			bulletPlayerState[cnt].posStatus.DrawPosX = bulletPlayerState[cnt].posStatus.posX;
			bulletPlayerState[cnt].posStatus.DrawPosY = bulletPlayerState[cnt].posStatus.posY;
			bulletPlayerState[cnt].posStatus.DrawVector = bulletPlayerState[cnt].posStatus.vector - getViewStatus()->centerVector;
			polygonMag = bulletPlayerState[cnt].posZ;

			bulletPlayerState[cnt].posStatus.DrawPosX = ( bulletPlayerState[cnt].posStatus.DrawPosX * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + SCREEN_WIDTH  * 0.5 * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) ); 
			bulletPlayerState[cnt].posStatus.DrawPosY = ( bulletPlayerState[cnt].posStatus.DrawPosY * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + ( SCREEN_HEIGHT - getViewStatus()->centerPosY ) * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) );
			g_posPolygon.x = bulletPlayerState[cnt].posStatus.DrawPosX;
			g_posPolygon.y = bulletPlayerState[cnt].posStatus.DrawPosY;
			g_rotPolygon.z = bulletPlayerState[cnt].posStatus.DrawVector;
						
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


			//�A�j���[�V�����ݒ�
			if( ++g_nCounterAnim == AF_BULLET_PLAYER_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_BULLET_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_BULLET_PLAYER_ANIM_DIV_X;
			texHeight = 1.0f / AF_BULLET_PLAYER_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_BULLET_PLAYER_ANIM_DIV_X );
			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_BULLET_PLAYER_ANIM_DIV_X ) );
	
			//�e�N�X�`�����W�ݒ�
			pVtx[QUOTE_VERTEX * cnt].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.y = texHeight * nowPatternY + texHeight;

			//���_���W�ݒ�
			pVtx[QUOTE_VERTEX * cnt].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
			pVtx[QUOTE_VERTEX * cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		if( bulletPlayerState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfBulletPlayer->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfBulletPlayer( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfBulletPlayer( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfBulletPlayer );
	
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfBulletPlayer , 0 , sizeof( VERTEX_2D ) );

		/*���e�̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
	}
}

/*******************************************************************************
* �֐���	:void DrawAfBulletPlayercore( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfBulletPlayercore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfBulletPlayer );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfBulletPlayer , 0 , sizeof( VERTEX_2D ) );
	
	
	/*���e�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_BULLET_PLAYER_VERTEX_MAX , 2 );

}

/*******************************************************************************
* �֐���	:BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
{
	return &bulletPlayerState[num];
}

/*******************************************************************************
* �֐���	:BULLET_PLAYER_STATUS* GetBulletPlayerState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
void SetBulletPlayerState( float posX , float posY , float posZ , float moveZ )
{
	for( int cnt = 0; cnt < BULLET_PLAYER_MAX_NUM; cnt++ )
	{
		if( bulletPlayerState[cnt].bUse == false )
		{
			bulletPlayerState[cnt].bUse = true;
			bulletPlayerState[cnt].posStatus.posX = posX;
			bulletPlayerState[cnt].posStatus.posY = posY;
			bulletPlayerState[cnt].posStatus.vector = 0.0f;
			bulletPlayerState[cnt].moveZ = moveZ;
			bulletPlayerState[cnt].posZ = posZ;

			bulletPlayerState[cnt].intPosX = ( int )( bulletPlayerState[cnt].posStatus.posX ) * 1000;
			bulletPlayerState[cnt].intPosY = ( int )( bulletPlayerState[cnt].posStatus.posY ) * 1000;
				  
			bulletPlayerState[cnt].nLife = AF_BULLET_PLAYER_DEFAULT_LIFE;
			break;
		}
	}
}