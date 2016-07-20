/*******************************************************************************
* �T�E���h	:�T�E���h�O���t�B�b�N.cpp
* �t�@�C����:soundGraphic.cpp
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
#include "sound.h"
#include "soundGraphic.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define SOUND_GRAPHIC_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define SOUND_GRAPHIC_PICKUP_FPS	( 45 )

#define SOUND_GRAPHIC_OUTPUT_POS_X	( SCREEN_WIDTH * 0.5 )		/*�\��X���W*/
#define SOUND_GRAPHIC_OUTPUT_POS_Y	( SCREEN_HEIGHT * 0.5 )		/*�\��Y���W*/

#define SOUND_GRAPHIC_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define SOUND_GRAPHIC_ANIM_SPAN		( 0 )	/*�A�j���[�V�����Ԋu*/
#define SOUND_GRAPHIC_ANIM_DIV_X	( 1 )	/*��������*/
#define SOUND_GRAPHIC_ANIM_DIV_Y	( 3 )	/*����������*/

#define SOUND_GRAPHIC_OUTPUT_SIZE	( 0.65 ) /*�����\���{��*/ 

#define MOVE_NUM ( 1.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.0f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.025f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 1.0f )	/*�g����E*/
#define MAG_MIN 	( 0.8f )	/*�k�����E*/

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSoundGraphic = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureSoundGraphic;	/*�T�E���h�O���t�B�b�N�e�N�X�`���̃|�C���^*/
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

static int outputStatus = SOUND_NORMAL;
static int g_soundStatus;
static int pastState;
static int writeCnt;
/*******************************************************************************
* �֐���	:HRESULT InitSoundGraphic( void )
*		�T�E���h�O���t�B�b�N�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitSoundGraphic( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * SOUND_GRAPHIC_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffSoundGraphic , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/soundGraphic.png" , &g_pTextureSoundGraphic );
	D3DXGetImageInfoFromFile( "data/TEXTURE/soundGraphic.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffSoundGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / SOUND_GRAPHIC_ANIM_DIV_X ) * SOUND_GRAPHIC_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / SOUND_GRAPHIC_ANIM_DIV_Y ) * SOUND_GRAPHIC_OUTPUT_SIZE );

	g_posPolygon.x = SOUND_GRAPHIC_OUTPUT_POS_X;
	g_posPolygon.y = SOUND_GRAPHIC_OUTPUT_POS_Y;
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
	g_pVtxBuffSoundGraphic->Unlock();

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

	outputStatus = SOUND_NORMAL;
	g_soundStatus = SOUND_NORMAL;
	pastState = SOUND_NORMAL;
	writeCnt = 0;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitSoundGraphic( void )
*		�T�E���h�O���t�B�b�N�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitSoundGraphic( void )
{
	if( g_pVtxBuffSoundGraphic != NULL )
	{
		g_pVtxBuffSoundGraphic->Release();
		g_pVtxBuffSoundGraphic = NULL;
	}
	if( g_pTextureSoundGraphic != NULL )
	{
		g_pTextureSoundGraphic->Release();
		g_pTextureSoundGraphic = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateSoundGraphic( int soundStatus )
*		�T�E���h�O���t�B�b�N�X�V�����֐��ł��B
* ����		:int soundStatus	�T�E���h�̏��
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateSoundGraphic( int soundStatus )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX = 0, nowPatternY = 0;
	
	/*��O����*/ 
	if( g_pVtxBuffSoundGraphic == NULL )
	{
		return;
	}

	writeCnt = ( writeCnt >= 0 ? writeCnt - 1 : writeCnt );
	g_soundStatus = soundStatus;
	//�T�E���h�X�e�[�^�X���ύX����A���ꂪ�O�̂��̂ƈႤ�ꍇ�ɂ̂ݓ���B
	if( soundStatus != SOUND_NORMAL && soundStatus != pastState )
	{
		writeCnt = SOUND_GRAPHIC_PICKUP_FPS;
		outputStatus = soundStatus;
		magNum = MAG_MAX;
	}
	pastState = soundStatus;

	if( ++g_nCounterAnim == SOUND_GRAPHIC_ANIM_SPAN )
	{
		g_nCounterAnim = 0;
		g_nPatternAnim = ( ++g_nPatternAnim == SOUND_GRAPHIC_ANIM_MAX ? 0 : g_nPatternAnim ); 
	}
	
	texWidth = 1.0f / SOUND_GRAPHIC_ANIM_DIV_X;
	texHeight = 1.0f / SOUND_GRAPHIC_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % SOUND_GRAPHIC_ANIM_DIV_X );

	switch( outputStatus )
	{
		case SOUND_MUTEON:
			{	
				nowPatternY = ( float )( ( int )( g_nPatternAnim / SOUND_GRAPHIC_ANIM_DIV_Y ) ) + 1;
				break;
			}
		case SOUND_MUTEOFF:
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / SOUND_GRAPHIC_ANIM_DIV_Y ) );
				break;
			}
		case SOUND_VOLCHANGE:
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / SOUND_GRAPHIC_ANIM_DIV_Y ) ) + 2;
				break;
			}
		default:
			{
				break;
			}
	}
	
	g_pVtxBuffSoundGraphic->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*�e�N�X�`�����W���f*/
	pVtx[0].tex.x = texWidth * nowPatternX;
	pVtx[0].tex.y = texHeight * nowPatternY;
	pVtx[1].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[1].tex.y = texHeight * nowPatternY;
	pVtx[2].tex.x = texWidth * nowPatternX;
	pVtx[2].tex.y = texHeight * nowPatternY + texHeight;
	pVtx[3].tex.x = texWidth * nowPatternX + texWidth;
	pVtx[3].tex.y = texHeight * nowPatternY + texHeight;
	

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

	if( writeCnt >= SOUND_GRAPHIC_PICKUP_FPS * 0.25 )
	{
		magNum = ( magNum - MAG_SPEED < MAG_MIN ? MAG_MIN : magNum - MAG_SPEED );
	}
	polygonMag = 1.0f + sin( magNum );

	pVtx[0].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );
	pVtx[1].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );
	pVtx[2].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );
	pVtx[3].col = D3DCOLOR_RGBA( 22 , 22 , 255 , 244 );

	
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
	g_pVtxBuffSoundGraphic->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawSoundGraphic( void )
*		�T�E���h�O���t�B�b�N�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawSoundGraphic( void )
{
	if( g_soundStatus == SOUND_NORMAL && writeCnt < 0 )
	{
		return;
	}

	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureSoundGraphic );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffSoundGraphic , 0 , sizeof( VERTEX_2D ) );

	/*�T�E���h�O���t�B�b�N�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}