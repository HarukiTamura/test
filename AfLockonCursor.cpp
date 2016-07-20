/*******************************************************************************
* �^�C�g��	:���e.cpp
* �t�@�C����:AfLockonCursor.cpp
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
#include "sound.h"
#include "input.h"
#include "AfLockonCursor.h"
#include "AfMissilePlayer.h"
#include "AfEnemy.h"
#include "AfPlayer.h"
#include "AfCursor.h"
#include "unoDirectX.h"
#include "AfExplosion.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_LOCKON_CURSOR_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_LOCKON_CURSOR_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_LOCKON_CURSOR_DEFAULT_VECTOR	( 0.0f )
#define AF_LOCKON_CURSOR_DEFAULT_POSZ	( 0.0f )
#define AF_LOCKON_CURSOR_DEFAULT_LIFE		( 100 )
#define AF_LOCKON_CURSOR_DEFAULT_SCORE		( 10 )

#define AF_LOCKON_CURSOR_OUTRANGE	( 1.20f )

#define AF_LOCKON_CURSOR_SPIN	( 1.23 )

#define AF_LOCKON_CURSOR_VERTEX_MAX		( 4 )	/*�ō����_��*/
#define AF_LOCKON_CURSOR_POLYGON_NUM	( LOCKON_CURSOR_MAX_NUM )	/*���@�{����*/

#define AF_LOCKON_CURSOR_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_LOCKON_CURSOR_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_LOCKON_CURSOR_ANIM_DIV_X	( 1 )	/*��������*/
#define AF_LOCKON_CURSOR_ANIM_DIV_Y	( 4 )	/*����������*/

#define AF_LOCKON_CURSOR_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 0.5f )	/*�ړ���*/
#define SPIN_SPEED	( D3DX_PI * 0.25 )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.30f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.23f )	/*������*/
#define ROT_ATTEN		( 0.85f )	/*������*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define MISSILE_LAUNCHER_OUTLONG ( 72 )
#define MISSILE_MOVE	( 1.0f )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfLockonCursor = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfLockonCursor;	/*���e�e�N�X�`���̃|�C���^*/
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

static LOCKON_CURSOR_STATUS lockonCursorState[LOCKON_CURSOR_MAX_NUM]; 

static int lockonFps = 0;

/*******************************************************************************
* �֐���	:HRESULT InitAfLockonCursor( void )
*		���e�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfLockonCursor( void )
{
	HRESULT result;
	result = InitDirectAfLockonCursor();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfLockonCursor();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfLockonCursor( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfLockonCursor( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_LOCKON_CURSOR_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfLockonCursor , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfLockonCursor.png" , &g_pTextureAfLockonCursor );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfLockonCursor.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfLockonCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_LOCKON_CURSOR_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfLockonCursor->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfLockonCursor( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfLockonCursor( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_LOCKON_CURSOR_ANIM_DIV_X ) * AF_LOCKON_CURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_LOCKON_CURSOR_ANIM_DIV_Y ) * AF_LOCKON_CURSOR_OUTPUT_SIZE );

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
	memset( &lockonCursorState , 0 , sizeof( LOCKON_CURSOR_STATUS ) * LOCKON_CURSOR_MAX_NUM );
	
	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		lockonCursorState[cnt].bUse = false;
		lockonCursorState[cnt].posStatus.posX = AF_LOCKON_CURSOR_DEFAULT_POSX;
		lockonCursorState[cnt].posStatus.posY = AF_LOCKON_CURSOR_DEFAULT_POSY;
		lockonCursorState[cnt].posStatus.vector = AF_LOCKON_CURSOR_DEFAULT_VECTOR;

		lockonCursorState[cnt].intPosX = ( int )( lockonCursorState[cnt].posStatus.posX ) * 1000;
		lockonCursorState[cnt].intPosY = ( int )( lockonCursorState[cnt].posStatus.posY ) * 1000;
				  
		lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_UNLOCK;
	}

	g_posPolygon.x = AF_LOCKON_CURSOR_DEFAULT_POSX;
	g_posPolygon.y = AF_LOCKON_CURSOR_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	lockonFps = 0;
}

/*******************************************************************************
* �֐���	:void UninitAfLockonCursor( void )
*		���e�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfLockonCursor( void )
{
	if( g_pVtxBuffAfLockonCursor != NULL )
	{
		g_pVtxBuffAfLockonCursor->Release();
		g_pVtxBuffAfLockonCursor = NULL;
	}
	if( g_pTextureAfLockonCursor != NULL )
	{
		g_pTextureAfLockonCursor->Release();
		g_pTextureAfLockonCursor = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfLockonCursor( void )
*		���e�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfLockonCursor( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfLockonCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	if( GetKeyboardPress( DIK_V ) || GetGamePadPress( 0 ) )
	{
		lockonFps = ( lockonFps + 1 >= 60 ? 0 : lockonFps + 1 );
		
		if( lockonFps % 3 == 0 )
		{
			for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
			{
				if( GetEnemyState(cnt)->bUse == true && lockonCursorState[cnt].mode != LOCKON_CURSOR_MODE_LOCK && GetEnemyState(cnt)->posZ < AF_LOCKON_CURSOR_LOST )	
				{
					GetEnemyState(cnt)->bLockOn = true;
					lockonCursorState[cnt].bUse = true;	
					lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_LOCK;
					PlaySound(SOUND_LABEL_SE_LOCKON);
					break;
				}	
				if( GetEnemyState(cnt)->posZ > AF_LOCKON_CURSOR_LOST )
				{
					
					lockonCursorState[cnt].bUse = false;	
					lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_UNLOCK;
					GetEnemyState(cnt)->bLockOn = false;
				}
			}
			/*if( lockonFps % 3 == 0 && lockonCursorState[cnt].mode != LOCKON_CURSOR_MODE_LOCK )
			{	
				if( GetEnemyState(cnt)->bUse == true &&  GetEnemyState(cnt)->posZ < AF_LOCKON_CURSOR_LOST )
				{
					GetEnemyState(cnt)->bLockOn = true;
					lockonCursorState[cnt].bUse = true;	
					lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_CHANGE;
					break;
				}	
			}

			if( lockonCursorState[cnt].mode == LOCKON_CURSOR_MODE_CHANGE )
			{
				lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_LOCK;
			}
			if( GetEnemyState(cnt)->bUse == false )
			{
				GetEnemyState(cnt)->bLockOn = false;
				lockonCursorState[cnt].bUse = false;
				break;
			}*/
		} 
	}
	else if( GetKeyboardRelease( DIK_V ) || GetGamePadRelease( 0 ) )
	{
		float posVec = -0.33f;
		int missileShotCnt = 0;
		for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
		{
			if( lockonCursorState[cnt].bUse == true )
			{
				missileShotCnt++;
				SetMissilePlayerState( GetPlayerState()->posStatus.DrawPosX + MISSILE_LAUNCHER_OUTLONG * sin( D3DX_PI * posVec ), GetPlayerState()->posStatus.DrawPosY + MISSILE_LAUNCHER_OUTLONG * cos( D3DX_PI * posVec ),
					1.0f , MISSILE_MOVE * sin( D3DX_PI * posVec ) , MISSILE_MOVE * cos( D3DX_PI * posVec ) , 0.0f );
				posVec += 0.2;
				SetAfExplosion( GetPlayerState()->posStatus.DrawPosX * 1000 + rand() % MISSILE_LAUNCHER_OUTLONG - rand() % MISSILE_LAUNCHER_OUTLONG
					, GetPlayerState()->posStatus.DrawPosY * 1000 + rand() % MISSILE_LAUNCHER_OUTLONG - rand() % MISSILE_LAUNCHER_OUTLONG , 1.2 , 0 , 0 , 300  , 4 , AF_EXPLOSION_002 );
				lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_UNLOCK;
			}
		}
		if( missileShotCnt > 0 )
		{
			PlaySound(SOUND_LABEL_SE_MISSILESHOOT);
		}
	}
	else
	{
		lockonFps = 0;
		for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
		{
			if( GetEnemyState(cnt)->bUse == true )
			{
				//GetEnemyState(cnt)->bLockOn = false;
			}
		}
		for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
		{
			lockonCursorState[cnt].bUse = false;
		}
	}

	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		if( lockonCursorState[cnt].bUse != true && GetEnemyState(cnt)->bUse == true && GetEnemyState(cnt)->bLockOn == false && GetEnemyState(cnt)->posZ < 0.0f )
		{
			lockonCursorState[cnt].mode = LOCKON_CURSOR_MODE_NONLOCK;
			lockonCursorState[cnt].bUse = true;
		}
		if( lockonCursorState[cnt].mode != LOCKON_CURSOR_MODE_UNLOCK && GetEnemyState(cnt)->bUse == false )
		{
			lockonCursorState[cnt].bUse = false;
		}
		if( lockonCursorState[cnt].bUse == true )
		{

			//�����ʒu�m��

			//�`����W�E�T�C�Y�m��
			lockonCursorState[cnt].posStatus.DrawPosX = GetEnemyState(cnt)->posStatus.DrawPosX;
			lockonCursorState[cnt].posStatus.DrawPosY = GetEnemyState(cnt)->posStatus.DrawPosY;
			lockonCursorState[cnt].posStatus.DrawVector = 0.0f;
			polygonMag = ( GetEnemyState(cnt)->posZ <= MAG_MIN ? MAG_MIN : GetEnemyState(cnt)->posZ );

			g_posPolygon.x = lockonCursorState[cnt].posStatus.DrawPosX;
			g_posPolygon.y = lockonCursorState[cnt].posStatus.DrawPosY;
			g_rotPolygon.z = lockonCursorState[cnt].posStatus.DrawVector;
			
			if( lockonCursorState[cnt].mode == LOCKON_CURSOR_MODE_LOCK )
			{
				g_rotPolygon.z += SPIN_SPEED;
			}
			
			if( lockonCursorState[cnt].mode == LOCKON_CURSOR_MODE_UNLOCK )
			{
				g_rotPolygon.z += SPIN_SPEED * 0.5f;
			}

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
			if( ++g_nCounterAnim == AF_LOCKON_CURSOR_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_LOCKON_CURSOR_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_LOCKON_CURSOR_ANIM_DIV_X;
			texHeight = 1.0f / AF_LOCKON_CURSOR_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_LOCKON_CURSOR_ANIM_DIV_X );

			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_LOCKON_CURSOR_ANIM_DIV_X ) ) + lockonCursorState[cnt].mode;

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
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 235 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 235 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 235 );
		}
		if( lockonCursorState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfLockonCursor->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfLockonCursor( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfLockonCursor( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfLockonCursor );
	
	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		
		if( lockonCursorState[cnt].bUse == true )
		{

			/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
			pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfLockonCursor , 0 , sizeof( VERTEX_2D ) );

			/*���e�̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
		}
	}
}

/*******************************************************************************
* �֐���	:void DrawAfLockonCursorcore( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfLockonCursorcore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfLockonCursor );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfLockonCursor , 0 , sizeof( VERTEX_2D ) );
	
	
	/*���e�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_LOCKON_CURSOR_VERTEX_MAX , 2 );

}

/*******************************************************************************
* �֐���	:LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
{
	return &lockonCursorState[num];
}

/*******************************************************************************
* �֐���	:LOCKON_CURSOR_STATUS* GetLockonCursorState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
void SetLockonCursorState( float posX , float posY , float posZ , float moveZ )
{
	for( int cnt = 0; cnt < LOCKON_CURSOR_MAX_NUM; cnt++ )
	{
		if( lockonCursorState[cnt].bUse == false )
		{
			lockonCursorState[cnt].bUse = true;
			lockonCursorState[cnt].posStatus.posX = posX;
			lockonCursorState[cnt].posStatus.posY = posY;
			lockonCursorState[cnt].posStatus.vector = 0.0f;

			lockonCursorState[cnt].intPosX = ( int )( lockonCursorState[cnt].posStatus.posX ) * 1000;
			lockonCursorState[cnt].intPosY = ( int )( lockonCursorState[cnt].posStatus.posY ) * 1000;
				  
			break;
		}
	}
}