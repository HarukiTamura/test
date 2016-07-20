/*******************************************************************************
* �^�C�g��	:����DirectX�֗��nheader
* �t�@�C����:unoMath.h
* �쐬��	:�c���t��
* �쐬��	:2015/07/03
********************************************************************************
* �X�V����	ver 1.00 �����悻�̊���
*
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define QUADRANGLE_VERTEX_NUM ( 4 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐���	:void vertexInit( VERTEX_2D *p_vertex , int vertexNum )
*		VERTEX_2D�A���_�o�b�t�@�̒l�����������܂��B
* ����		:VERTEX_2D *p_vertex	����������o�b�t�@�̐擪�A�h���X	
*			 int vertexNum			���������钸�_��
* �߂�l	:void �߂�l�Ȃ�
* ����		:VERTEX_2D�̒l�����������܂��B
*******************************************************************************/
void vertexInit( VERTEX_2D *p_vertex , int vertexNum )
{
	for( int nCntVertex = 0; nCntVertex < vertexNum; nCntVertex++ )
	{
		/*���_���W�ݒ�*/
		p_vertex[nCntVertex].pos.x = 0.0f;
		p_vertex[nCntVertex].pos.y = 0.0f;
		p_vertex[nCntVertex].pos.z = 0.0f;

		/*���W�ϊ��p�W���ݒ�*/
		p_vertex[nCntVertex].rhw = 1.0f;
	
		/*���_�J���[�ݒ�*/
		p_vertex[nCntVertex].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*�e�N�X�`�����W�ݒ�*/
		p_vertex[nCntVertex].tex.x = 0.0f;
		p_vertex[nCntVertex].tex.y = 0.0f;
		p_vertex[nCntVertex].tex.x = 0.0f;
	}
}


/*******************************************************************************
* �֐���	:void vertexInitQuad( VERTEX_2D *p_vertex , int polygonNum )
*		VERTEX_2D�A���_�o�b�t�@�̒l���|���S���P�ʂŏ��������܂��B
* ����		:VERTEX_2D *p_vertex	����������o�b�t�@�̐擪�A�h���X	
*			 int vertexNum			���������钸�_��
* �߂�l	:void �߂�l�Ȃ�
* ����		:VERTEX_2D�̒l���|���S���P�ʂŏ��������܂��B
*******************************************************************************/
void vertexInitQuad( VERTEX_2D *p_vertex , int polygonNum = 1 )
{
	for( int nCntPolygon = 0; nCntPolygon < polygonNum; nCntPolygon++ )
	{
		/*���_���W�ݒ�*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].pos = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

		/*���W�ϊ��p�W���ݒ�*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].rhw = 1.0f;
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].rhw = 1.0f;
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].rhw = 1.0f;
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].rhw = 1.0f;
	
		/*���_�J���[�ݒ�*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
		/*�e�N�X�`�����W�ݒ�*/
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 0].tex = D3DXVECTOR2( 0 , 0 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 1].tex = D3DXVECTOR2( 1 , 0 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 2].tex = D3DXVECTOR2( 0 , 1 );
		p_vertex[nCntPolygon * QUADRANGLE_VERTEX_NUM + 3].tex = D3DXVECTOR2( 1 , 1 );
	}
}