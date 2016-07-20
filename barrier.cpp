/*******************************************************************************
* �^�C�g��	:�o���A�[.cpp
* �t�@�C����:barrier.cpp
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
#include "barrier.h"
#include "player01.h"
#include "bullet.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define BARRIER_VERTEX_MAX	( 4 )	/*�ō����_��*/
#define BARRIER_POLYGON_NUM	( 2 )	/*���@�{����*/

#define BARRIER_ANIM_MAX	( 5 )	/*�A�j���[�V�����R�}��*/
#define BARRIER_ANIM_SPAN	( 5 )	/*�A�j���[�V�����Ԋu*/
#define BARRIER_ANIM_DIV_X	( 5 )	/*��������*/
#define BARRIER_ANIM_DIV_Y	( 1 )	/*����������*/

#define BARRIER_OUTPUT_SIZE	( 0.375 ) /*���E�\���{��*/ 
#define BARRIER_RADIUS	( 112 )		/*���E�g�厞���a*/

#define MOVE_NUM ( 0.4f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN	( 0.23f )	/*������*/
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffbarrier = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturebarrier;	/*�o���A�[�e�N�X�`���̃|�C���^*/
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

static int outputSizeX , outputSizeY;

float nowRadius = 0, radiusAccel = 0;
float nowRadiusInt = 0;
bool pastBarrier = false;	//�o���A�I���g���K�[�p

/*******************************************************************************
* �֐���	:HRESULT InitBarrier( void )
*		�o���A�[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitBarrier( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BARRIER_VERTEX_MAX * BARRIER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffbarrier , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/barrier.png" , &g_pTexturebarrier );
	D3DXGetImageInfoFromFile( "data/TEXTURE/barrier.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffbarrier->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , BARRIER_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffbarrier->Unlock();
	
	outputSizeX = ( int )( ( info.Width / BARRIER_ANIM_DIV_X ) * BARRIER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / BARRIER_ANIM_DIV_Y ) * BARRIER_OUTPUT_SIZE );

	g_posPolygon.x = 0;
	g_posPolygon.y = 0;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );


	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	nowRadius = 0;
	radiusAccel = 0;
	pastBarrier = false;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitBarrier( void )
*		�o���A�[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitBarrier( void )
{
	if( g_pVtxBuffbarrier != NULL )
	{
		g_pVtxBuffbarrier->Release();
		g_pVtxBuffbarrier = NULL;
	}
	if( g_pTexturebarrier != NULL )
	{
		g_pTexturebarrier->Release();
		g_pTexturebarrier = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateBarrier( void )
*		�o���A�[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateBarrier( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	float rebirthRotate = 0;

	//�o���A���L���ȏꍇ�̂݃o���A�g��F�����łȂ���Ώk��
	if( GetPlayerState01()->liarBarrier == true )
	{
		radiusAccel += 0.21f; // 10.5f;
	}
	else
	{
		radiusAccel -= 0.21f;
	}
	if( GetPlayerState01()->liarBarrier == false && pastBarrier == true )
	{
		radiusAccel = 0;
	}
	
	pastBarrier = GetPlayerState01()->liarBarrier;
	nowRadius += radiusAccel;
	if( nowRadius <= 0 )
	{
		nowRadius = 0;
		radiusAccel = 0;
	}
	if( nowRadius >= BARRIER_RADIUS )
	{
		nowRadius = BARRIER_RADIUS;
	}
	nowRadiusInt = ( int )( nowRadius * 1000 );
	polygonMag = nowRadius / BARRIER_RADIUS;

	if( ++g_nCounterAnim == BARRIER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == BARRIER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	

	texWidth = 1.0f / BARRIER_ANIM_DIV_X;
	texHeight = 1.0f / BARRIER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % BARRIER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / BARRIER_ANIM_DIV_X ) );

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffbarrier->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );


	pVtx[BARRIER_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[BARRIER_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[BARRIER_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
	pVtx[BARRIER_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
						
	//�e�N�X�`�����W�ݒ�
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;

	pVtx[BARRIER_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
	pVtx[BARRIER_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
	pVtx[BARRIER_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[BARRIER_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
	pVtx[BARRIER_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
	pVtx[BARRIER_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[BARRIER_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[BARRIER_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

	//�o���A�͎��@�ɒǏ]����B
	g_movePolygon.x = GetPlayerState01()->posX;
	g_movePolygon.x = GetPlayerState01()->posY;

	/*�ړ��ʔ��f*/
	g_posPolygon.x = GetPlayerState01()->posX;
	g_posPolygon.y = GetPlayerState01()->posY;

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
	g_rotPolygon.z += SPIN_SPEED;
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}


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
	
	rebirthRotate = D3DX_PI - g_rotPolygon.z;

	pVtx[BARRIER_VERTEX_MAX + 0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[BARRIER_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[BARRIER_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[BARRIER_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + rebirthRotate ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffbarrier->Unlock();

}

/*******************************************************************************
* �֐���	:void DrawBarrier( void )
*		�o���A�[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawBarrier( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturebarrier );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffbarrier , 0 , sizeof( VERTEX_2D ) );

	/*�o���A�[�̕`��*/
	
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + BARRIER_VERTEX_MAX , 2 );


}

/*******************************************************************************
* �֐���	:int GetBarrierRadiusInt( void )
*		�o���A�[���a��n�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:int�@�o���A���aint�ϊ��l
* ����		:�o���A�[�̔��a��n���֐��ł��B
*******************************************************************************/
int GetBarrierRadiusInt( void )
{
	return nowRadiusInt;
}