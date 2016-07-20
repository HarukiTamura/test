/*******************************************************************************
* �^�C�g��	:�e.cpp
* �t�@�C����:bullet.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/06/24
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "bullet.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define BULLET_OUTPUT_POS_X	( 320 )		/*�\��X���W*/
#define BULLET_OUTPUT_POS_Y	( 230 )		/*�\��Y���W*/

#define BULLET_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define BULLET_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define BULLET_ANIM_SPAN	( 0 )	/*�A�j���[�V�����Ԋu*/
#define BULLET_ANIM_DIV_X	( 1 )	/*��������*/
#define BULLET_ANIM_DIV_Y	( 1 )	/*����������*/

#define BULLET_OUTPUT_SIZE	( 0.5 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffbullet = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturebullet;	/*�e�̃e�N�X�`���̃|�C���^*/
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
* �֐���	:HRESULT InitBullet( void )
*		�e�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitBullet( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BULLET_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffbullet , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/bullet000.png" , &g_pTexturebullet );
	D3DXGetImageInfoFromFile( "data/TEXTURE/bullet000.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / BULLET_ANIM_DIV_X ) * BULLET_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / BULLET_ANIM_DIV_Y ) * BULLET_OUTPUT_SIZE );

	g_posPolygon.x = BULLET_OUTPUT_POS_X;
	g_posPolygon.y = BULLET_OUTPUT_POS_Y;
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
	g_pVtxBuffbullet->Unlock();

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
* �֐���	:void UninitBullet( void )
*		�e�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitBullet( void )
{
	if( g_pVtxBuffbullet != NULL )
	{
		g_pVtxBuffbullet->Release();
		g_pVtxBuffbullet = NULL;
	}
	if( g_pTexturebullet != NULL )
	{
		g_pTexturebullet->Release();
		g_pTexturebullet = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateBullet( void )
*		�e�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateBullet( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	
	if( ++g_nCounterAnim == BULLET_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == BULLET_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / BULLET_ANIM_DIV_X;
	texHeight = 1.0f / BULLET_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % BULLET_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) );
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
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
			g_posPolygon.x += ( float )( -sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_posPolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_S ) )
		{

		}
		else if( GetKeyboardPress( DIK_D ) )
		{
			g_posPolygon.x += ( float )( sinf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
			g_posPolygon.y += ( float )( cosf( D3DX_PI * 0.75 ) ) * MOVE_NUM;
		}
		else
		{
			g_posPolygon.y -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_A ) )
	{
		if( GetKeyboardPress( DIK_S ) )
		{
			g_posPolygon.x += -sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_posPolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else if( GetKeyboardPress( DIK_D ) )
		{

		}
		else
		{
			g_posPolygon.x -= MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_S ) )
	{
		if( GetKeyboardPress( DIK_D ) )
		{
			g_posPolygon.x += sinf( D3DX_PI * 0.25 ) * MOVE_NUM;
			g_posPolygon.y += cosf( D3DX_PI * 0.25 ) * MOVE_NUM;
		}
		else
		{
			g_posPolygon.y += MOVE_NUM;
		}
	}
	else if( GetKeyboardPress( DIK_D ) )
	{
		g_posPolygon.x += MOVE_NUM;
	}
	else
	{

	}

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
	/*�|���S����]*/
	if( GetKeyboardPress( DIK_LEFT ) )/*���v���*/
	{
		g_rotPolygon.z += SPIN_SPEED;
	}
	if( GetKeyboardPress( DIK_RIGHT ) )/*�t���v���*/
	{
		g_rotPolygon.z -= SPIN_SPEED;
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
	if( GetKeyboardTrigger( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardTrigger( DIK_DOWN ) )
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


	DebugMessage("����[��", 50 , 50 );
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffbullet->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawBullet( void )
*		�e�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawBullet( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturebullet );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffbullet , 0 , sizeof( VERTEX_2D ) );

	/*�e�̂̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}