/*******************************************************************************
* �^�C�g��	:�^�C�g���O���t�B�b�N.cpp
* �t�@�C����:TitleGraphic.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00	����Ƃ��Ă̊���
*			ver 1.01	�Y�[�����̑��x�{��define��`���ɓ���s�ǁA�����s���B
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "TitleGraphic.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define TITLE_GRAPHIC_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define TITLE_GRAPHIC_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*�\��X���W*/
#define TITLE_GRAPHIC_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*�\��Y���W*/

#define ZOOM_CORRECTION		( )�@//�Y�[�����̑��x�{��(define��`���ɓ���s�ǂ��m�F�F�����s���Ȉ׌��)

#define TITLE_GRAPHIC_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define TITLE_GRAPHIC_ANIM_SPAN		( 0 )	/*�A�j���[�V�����Ԋu*/
#define TITLE_GRAPHIC_ANIM_DIV_X	( 1 )	/*��������*/
#define TITLE_GRAPHIC_ANIM_DIV_Y	( 1 )	/*����������*/

#define TITLE_GRAPHIC_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define MOVE_NUM ( 1.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( D3DX_PI / 2 * 0.025f )	/*�g��X�s�[�h*/

#define MAG_MAX		( D3DX_PI / 2 * 1.0f )	/*�g����E*/
#define MAG_MIN 	( -D3DX_PI / 2 * 0.02f )	/*�k�����E*/

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitleGraphic = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureTitleGraphic;	/*�^�C�g���O���t�B�b�N�e�N�X�`���̃|�C���^*/
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
static int zoomSwitch, zoomCnt;
static float magNum;

/*******************************************************************************
* �֐���	:HRESULT InitTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitTitleGraphic( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * TITLE_GRAPHIC_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffTitleGraphic , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/titleBackGround.png" , &g_pTextureTitleGraphic );
	D3DXGetImageInfoFromFile( "data/TEXTURE/titleBackGround.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffTitleGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / TITLE_GRAPHIC_ANIM_DIV_X ) * TITLE_GRAPHIC_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TITLE_GRAPHIC_ANIM_DIV_Y ) * TITLE_GRAPHIC_OUTPUT_SIZE );

	g_posPolygon.x = TITLE_GRAPHIC_OUTPUT_POS_X;
	g_posPolygon.y = TITLE_GRAPHIC_OUTPUT_POS_Y;
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
	g_pVtxBuffTitleGraphic->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	zoomSwitch = 0;
	zoomCnt = 0;
	magNum = MAG_MAX;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitTitleGraphic( void )
{
	if( g_pVtxBuffTitleGraphic != NULL )
	{
		g_pVtxBuffTitleGraphic->Release();
		g_pVtxBuffTitleGraphic = NULL;
	}
	if( g_pTextureTitleGraphic != NULL )
	{
		g_pTextureTitleGraphic->Release();
		g_pTextureTitleGraphic = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateTitleGraphic( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	

	if( ++g_nCounterAnim == TITLE_GRAPHIC_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == TITLE_GRAPHIC_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / TITLE_GRAPHIC_ANIM_DIV_X;
	texHeight = 1.0f / TITLE_GRAPHIC_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % TITLE_GRAPHIC_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / TITLE_GRAPHIC_ANIM_DIV_X ) );
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffTitleGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

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
	
	g_rotPolygon.z += SPIN_SPEED;
	
	if( g_rotPolygon.z > D3DX_PI )
	{
		g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
	}
	if( g_rotPolygon.z < -D3DX_PI )
	{
		g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
	}

	if( zoomSwitch == 0 )
	{
		magNum = ( magNum + MAG_SPEED * 0.02 > MAG_MAX ? MAG_MAX : magNum + MAG_SPEED * 0.02);
		if( magNum == MAG_MAX )
		{
			zoomSwitch = 1;
		}
	}
	else
	{
		magNum = ( magNum - MAG_SPEED < MAG_MIN ? MAG_MIN : magNum - MAG_SPEED );
		if( magNum == MAG_MIN )
		{
			zoomSwitch = 0;
		}
	}
	polygonMag = 1.1f + sin( magNum );

	/*�|���S���g��*/
	/*if( GetKeyboardPress( DIK_UP ) )
	{
		polygonMag = ( polygonMag + MAG_SPEED > MAG_MAX ? MAG_MAX : polygonMag + MAG_SPEED );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		polygonMag = ( polygonMag - MAG_SPEED < MAG_MIN ? MAG_MIN : polygonMag - MAG_SPEED );
	}*/
	
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
	g_pVtxBuffTitleGraphic->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawTitleGraphic( void )
*		�^�C�g���O���t�B�b�N�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawTitleGraphic( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureTitleGraphic );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffTitleGraphic , 0 , sizeof( VERTEX_2D ) );

	/*�^�C�g���O���t�B�b�N�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}