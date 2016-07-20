/*******************************************************************************
* ���U���g	:�|�[�Y�O���t�B�b�N.cpp
* �t�@�C����:AfGameOver.cpp
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
#include "AfGameOver.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define FADE_NUM ( 5 )

#define AF_GAMEOVER_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_GAMEOVER_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*�\��X���W*/
#define AF_GAMEOVER_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*�\��Y���W*/

#define AF_GAMEOVER_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_GAMEOVER_ANIM_SPAN		( 0 )	/*�A�j���[�V�����Ԋu*/
#define AF_GAMEOVER_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_GAMEOVER_ANIM_DIV_Y	( 1 )	/*����������*/

#define AF_GAMEOVER_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define MOVE_NUM ( 1.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN	( 0.05f )	/*������*/
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfGameOver = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfGameOver;	/*�|�[�Y�O���t�B�b�N�e�N�X�`���̃|�C���^*/
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
static int fade = 0;
/*******************************************************************************
* �֐���	:HRESULT InitAfGameOver( void )
*		�|�[�Y�O���t�B�b�N�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfGameOver( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_GAMEOVER_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfGameOver , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfGameOver.png" , &g_pTextureAfGameOver );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfGameOver.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfGameOver->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / AF_GAMEOVER_ANIM_DIV_X ) * AF_GAMEOVER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_GAMEOVER_ANIM_DIV_Y ) * AF_GAMEOVER_OUTPUT_SIZE );

	g_posPolygon.x = AF_GAMEOVER_OUTPUT_POS_X;
	g_posPolygon.y = AF_GAMEOVER_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_TOP , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_RIGHT	, SCREEN_TOP , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( SCREEN_LEFT	, SCREEN_BOTTOM , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_BOTTOM , 0.0f );

	/*���W�ϊ��p�W���ݒ�*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*���_�J���[�ݒ�*/
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	/*�e�N�X�`�����W�ݒ�*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfGameOver->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	fade = 0;
	
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfGameOver( void )
*		�|�[�Y�O���t�B�b�N�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfGameOver( void )
{
	if( g_pVtxBuffAfGameOver != NULL )
	{
		g_pVtxBuffAfGameOver->Release();
		g_pVtxBuffAfGameOver = NULL;
	}
	if( g_pTextureAfGameOver != NULL )
	{
		g_pTextureAfGameOver->Release();
		g_pTextureAfGameOver = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfGameOver( void )
*		�|�[�Y�O���t�B�b�N�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfGameOver( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	
	fade = ( fade + FADE_NUM >= 255 ? 255 : fade + FADE_NUM );

	if( ++g_nCounterAnim == AF_GAMEOVER_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == AF_GAMEOVER_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / AF_GAMEOVER_ANIM_DIV_X;
	texHeight = 1.0f / AF_GAMEOVER_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % AF_GAMEOVER_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_GAMEOVER_ANIM_DIV_X ) );
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfGameOver->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	

	/*���_�J���[�ݒ�*/
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );

	/*�ړ��ʔ��f*/
	g_posPolygon.x += g_movePolygon.x;
	g_posPolygon.y += g_movePolygon.y;

	if( g_posPolygon.x < SCREEN_LEFT )
	{
		g_posPolygon.x = SCREEN_LEFT;
	}
	if( g_posPolygon.x > SCREEN_RIGHT )
	{
		g_posPolygon.x = SCREEN_RIGHT;
	}
	if( g_posPolygon.y < SCREEN_TOP )
	{
		g_posPolygon.y = SCREEN_TOP;
	}
	if( g_posPolygon.y > SCREEN_HEIGHT )
	{
		g_posPolygon.y = SCREEN_HEIGHT;
	}
	/*�ړ��ʌ���*/
	g_movePolygon.x += ( DEFAULT_MOVE - g_movePolygon.x ) * MOVE_ATTEN;
	g_movePolygon.y += ( DEFAULT_MOVE - g_movePolygon.y ) * MOVE_ATTEN;	 	
		
	/*�|���S����]*/
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
	pVtx[0].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_WIDTH , 0.0f , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( 0.0f , SCREEN_HEIGHT , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_WIDTH , SCREEN_HEIGHT , 0.0f );
								

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfGameOver->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfGameOver( void )
*		�|�[�Y�O���t�B�b�N�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfGameOver( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfGameOver );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfGameOver , 0 , sizeof( VERTEX_2D ) );

	/*�|�[�Y�O���t�B�b�N�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}