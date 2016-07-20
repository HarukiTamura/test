/*******************************************************************************
* �^�C�g��	:�����j���O�}��.cpp
* �t�@�C����:RunningMan.cpp
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
#include "RunningMan.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define RUNNINGMAN_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define RUNNINGMAN_ANIM_MAX		( 10 )	/*�A�j���[�V�����R�}��*/
#define RUNNINGMAN_ANIM_SPAN	( 4 )	/*�A�j���[�V�����Ԋu*/
#define RUNNINGMAN_ANIM_DIV_X	( 5 )	/*��������*/
#define RUNNINGMAN_ANIM_DIV_Y	( 2 )	/*����������*/

#define RUNNINGMAN_OUTPUT_SIZE	( 0.5 ) /*�����\���{��*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRunningMan = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureRunningMan;	/*�����j���O�}���e�N�X�`���̃|�C���^*/
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

/*******************************************************************************
* �֐���	:HRESULT InitRunningMan( void )
*		�����j���O�}���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitRunningMan( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * RUNNINGMAN_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffRunningMan , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/runningman100.png" , &g_pTextureRunningMan );
	D3DXGetImageInfoFromFile( "data/TEXTURE/runningman100.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffRunningMan->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / RUNNINGMAN_ANIM_DIV_X ) * RUNNINGMAN_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / RUNNINGMAN_ANIM_DIV_Y ) * RUNNINGMAN_OUTPUT_SIZE );

	g_posPolygon.x = RUNNINGMAN_OUTPUT_POS_X;
	g_posPolygon.y = RUNNINGMAN_OUTPUT_POS_Y;
	g_posPolygon.z = 0.0f;

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );

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
	g_pVtxBuffRunningMan->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitRunningMan( void )
*		�����j���O�}���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitRunningMan( void )
{
	if( g_pVtxBuffRunningMan != NULL )
	{
		g_pVtxBuffRunningMan->Release();
		g_pVtxBuffRunningMan = NULL;
	}
	if( g_pTextureRunningMan != NULL )
	{
		g_pTextureRunningMan->Release();
		g_pTextureRunningMan = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateRunningMan( void )
*		�����j���O�}���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateRunningMan( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	
	if( ++g_nCounterAnim == RUNNINGMAN_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == RUNNINGMAN_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / RUNNINGMAN_ANIM_DIV_X;
	texHeight = 1.0f / RUNNINGMAN_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % RUNNINGMAN_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / RUNNINGMAN_ANIM_DIV_X ) );
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffRunningMan->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*�ǂݍ��񂾂��̂Ƃ͍��E���]*/

	pVtx[1].tex.x = texWidth * nowPatternX;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[0].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[3].tex.x = texWidth * nowPatternX;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[2].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;


	if( GetKeyboardPress( DIK_W ) )
	{
		if( GetKeyboardPress( DIK_A ) )
		{
			g_movePolygon.x += ( float )( -sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_movePolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_S ) )
		{

		}
		else if( GetKeyboardPress( DIK_D ) )
		{
			g_movePolygon.x += ( float )( sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_movePolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else
		{
			g_movePolygon.y -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_A ) )
	{
		if( GetKeyboardPress( DIK_S ) )
		{
			g_movePolygon.x += -sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_movePolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_D ) )
		{

		}
		else
		{
			g_movePolygon.x -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_S ) )
	{
		if( GetKeyboardPress( DIK_D ) )
		{
			g_movePolygon.x += sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_movePolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else
		{
			g_movePolygon.y += MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_D ) )
	{
		g_movePolygon.x += MOVE_NUM;
	}
	else
	{

	}
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
	if( GetKeyboardPress( DIK_LEFT ) )
	{
		g_rotPolygon.z -= SPIN_SPEED;
	}
	if( GetKeyboardPress( DIK_RIGHT ) )
	{
		g_rotPolygon.z += SPIN_SPEED;
	}
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}

	/*�|���S���g��*/
	if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}
	
	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y - polygonHeight / 2 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x - polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + polygonWidth / 2 , g_posPolygon.y + polygonHeight / 2 , 0.0f );
	*/
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffRunningMan->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawRunningMan( void )
*		�����j���O�}���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawRunningMan( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureRunningMan );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffRunningMan , 0 , sizeof( VERTEX_2D ) );

	/*�����j���O�}���̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}