/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "toolbox_stdafx.h"
#include "OGLModel.h"
#include "OGLMesh.h"
#include "TargaToOpenGl.h"


dMatrix OGLModel::m_matrixPalete[MAX_PALETTE_MATRIX];

OGLLoaderContext::OGLLoaderContext()
{
}

OGLLoaderContext::~OGLLoaderContext()
{
}



dMesh* OGLLoaderContext::CreateMesh (int type)
{
_ASSERTE (0);
return 0;
/*
	OGLMesh* mesh;
	mesh = new OGLMesh (dMesh::dMeshType (type));
	return mesh;
*/
}

void OGLLoaderContext::LoaderFixup (dModel* model)
{
	_ASSERTE (0);
/*
	// load all textures
	for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
		OGLMesh* oglMesh = (OGLMesh*) node->GetInfo().m_mesh;
		oglMesh->OptimizeForRender ();

		for (dMesh::dListNode* subMesh = oglMesh->GetFirst(); subMesh; subMesh = subMesh->GetNext()) {
			dSubMesh& segment = subMesh->GetInfo();
			segment.m_textureHandle = LoadTexture(segment.m_textureName);
		}
	}

	// when using the collada loader the model need to be rotated by 90 degree in order to make the x axis the front
	dMatrix rot (dYawMatrix (-3.1416f * 0.5f));
	model->ApplyGlobalTransform(rot);
*/
}


OGLModel::OGLModel(void)
{
	m_node = NULL;
}

OGLModel::~OGLModel(void)
{
}




void OGLModel::Render()
{
	if (m_skeleton.GetCount()) {
		_ASSERTE (0);
/*
		UpdateMatrixPalette (m_matrix, m_matrixPalete, sizeof (m_matrixPalete) / sizeof (dMatrix));
		for (dList<dMeshInstance>::dListNode* node = m_meshList.GetFirst(); node; node = node->GetNext()) { 
			OGLMesh* mesh;
			mesh = (OGLMesh*)node->GetInfo();
			if (mesh->GetModifier()) {
				mesh->Render (m_matrixPalete);
			} else {
				glPushMatrix();
				glMultMatrix(&m_matrixPalete[mesh->m_boneID][0][0]);
				mesh->Render (NULL);
				glPopMatrix();
			}
		}
*/
	} else {
		glPushMatrix();
		glMultMatrix(&m_matrix[0][0]);
		for (dList<dMeshInstance>::dListNode* node = m_meshList.GetFirst(); node; node = node->GetNext()) { 
			OGLMesh* mesh;
			mesh = (OGLMesh*)node->GetInfo().m_mesh;
			mesh->Render (NULL);
		}
		glPopMatrix();
	}
}
