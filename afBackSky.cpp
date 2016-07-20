/*******************************************************************************
* �^�C�g��	:�w�i.cpp
* �t�@�C����:afBackSky.cpp
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
#include "unoDirectX.h"
#include "unoHitChecker.h"
#include "afBackSky.h"
#include "afPointConvert.h"
#include "AfPlayer.h"
/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define BACKSKY_USE_TRIANGLE_NUM		( 16 )	//�g�p�O�p�`��
#define USE_VERTEX_NUM	( BACKSKY_USE_TRIANGLE_NUM + 2 )		//�g�p���_��

#define DRAW_RANGE_X	( 1300 )	//�`�攼�aX
#define DRAW_RANGE_Y	( 900 )	//�`�攼�aY
#define DRAW_Y_PLUS		( 30 )	//�`�攼�aY�ǉ���

#define ANIMATION_PATTERN	( 2 )	//PATTERN��
#define ANIMATION_MAX	( 60 )		//�A�j���[�V�������[�v�b��
#define ANIMATION_SPAN	( 2 )		//�P�R�}��FPS��
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPolygon = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureBack;	/*�w�i�e�N�X�`���̃|�C���^*/

static float writeVector = 0;
static float writePosX = 12; 
static float writePosY = 300;

static int nowAnimationFps = 0;
static int nowAnimationPattern = 0;

static float posX1,posY1,posX2,posY2;

/*******************************************************************************
* �֐���	:HRESULT InitAfBackSky( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfBackSky( void )
{
	HRESULT result = InitDirectAfBackSky();
	InitValueAfBackSky();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfBackSky( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfBackSky( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * USE_VERTEX_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffPolygon , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}

	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/backSky.png.png" , &g_pTextureBack );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBuffPolygon->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInit( pVtx , USE_VERTEX_NUM );
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffPolygon->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfBackSky( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfBackSky( void )
{
	writeVector = 0;
	writePosX = SCREEN_WIDTH / 2; 
	writePosY = SCREEN_HEIGHT / 2;

	nowAnimationFps = 0;
	nowAnimationPattern = 0;
}

/*******************************************************************************
* �֐���	:void UninitAfBackSky( void )
*		�w�i�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfBackSky( void )
{
	if( g_pVtxBuffPolygon != NULL )
	{
		g_pVtxBuffPolygon->Release();
		g_pVtxBuffPolygon = NULL;
	}
	if( g_pTextureBack != NULL )
	{
		g_pTextureBack->Release();
		g_pTextureBack = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfBackSky( void )
*		�w�i�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfBackSky( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float frameVertex = 0.0f , drawRangeX = 0.0f , drawRangeY = 0.0f;
	float texturePosPlus = 0.0f;

	writeVector = getViewStatus()->centerVector + D3DX_PI;
	writePosX = getViewStatus()->centerPosX;
	writePosY = SCREEN_HEIGHT - getViewStatus()->centerPosY + DRAW_Y_PLUS;


	//�A�j���[�V�����̐���
	nowAnimationFps = ( nowAnimationFps + 1 >= ANIMATION_MAX ? 0 : nowAnimationFps + 1 );
	if( nowAnimationFps % ANIMATION_SPAN )
	{
		nowAnimationPattern = ( nowAnimationPattern + 1 > ANIMATION_MAX ? 0 : nowAnimationPattern + 1 );
	}
	texturePosPlus = 1.0f / ANIMATION_PATTERN * nowAnimationPattern;

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffPolygon->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	//�`�撆�S�_�̐���
	pVtx[0].pos = D3DXVECTOR3( writePosX , writePosY , 0.0f );	//���_���W�ݒ�
	pVtx[0].col = D3DCOLOR_RGBA( 122 , 122 , 255 , 22 );	//���_�J���[�ݒ�
	pVtx[0].tex = D3DXVECTOR2( 0.0f , 0.0f );		//�e�N�X�`�����W�ݒ�

	//�c��̒��_�̐ݒ�
	for( int cnt = 1; cnt < USE_VERTEX_NUM; cnt++ )
	{
		float posX , posY;
		float texX , texY;

		posX = writePosX + DRAW_RANGE_X * cosf( D3DX_PI * frameVertex + writeVector );
		posY = writePosY + DRAW_RANGE_Y * sinf( D3DX_PI * frameVertex + writeVector );
		
		//���_���W�ݒ�
		pVtx[cnt].pos.x = posX;
		pVtx[cnt].pos.y = posY;
		pVtx[cnt].pos.z = 0.0f;
		
		if( cnt == 1 )
		{
			posX1 = posX;
			posY1 = posY;
		}
		if( cnt == USE_VERTEX_NUM - 1 )
		{
			posX2 = posX;
			posY2 = posY;
		}

		//���_�J���[�ݒ�
		pVtx[cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		
		//�e�N�X�`�����W�ݒ�1
		pVtx[cnt].tex = D3DXVECTOR2( 9.8f , 0.8f );
		//�����FpVtx[0].tex�ł��ƋC�����������ʔ�������������B

		frameVertex += 1.0f / BACKSKY_USE_TRIANGLE_NUM;
		
	}
	

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffPolygon->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfBackSky( void )
*		�w�i�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfBackSky( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureBack );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	for( int cnt = 0; cnt < USE_VERTEX_NUM; cnt++ )
	{
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffPolygon , cnt , sizeof( VERTEX_2D ) );
		/*�w�i�̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN , 0 , BACKSKY_USE_TRIANGLE_NUM );
	}


}

