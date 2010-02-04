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

// dMesh.cpp: implementation of the dMesh class.
//
//////////////////////////////////////////////////////////////////////
#include "dAnimationStdAfx.h"
#include "dBone.h"
#include "dMatrix.h"
#include "dSkinModifier.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

dSkinModifier::dSkinModifier(const dMesh* mesh)
	:dMeshInstance::dModifier()
{
	m_bonesCount = 0;
	m_rootBone = NULL;
	m_skinnedBones = NULL;
	m_bindingMatrices = NULL;
	m_vertexWeight = (dVector*) malloc (mesh->m_vertexCount * sizeof (dVector));
	m_boneWeightIndex = (dBoneWeightIndex*) malloc (mesh->m_vertexCount * sizeof (dBoneWeightIndex));
	memset (m_vertexWeight, 0, mesh->m_vertexCount * sizeof (dVector));
	memset (m_boneWeightIndex, 0, mesh->m_vertexCount * sizeof (dBoneWeightIndex));
}

dSkinModifier::~dSkinModifier()
{
	free (m_boneWeightIndex);
	free (m_vertexWeight);
	if (m_bindingMatrices) {
		free (m_bindingMatrices);
	}
	if (m_skinnedBones) {
		free (m_skinnedBones);
	}
}

void dSkinModifier::SetRootNode (const dBone* bone)
{
	m_rootBone = bone;
}

void dSkinModifier::SetBindingPose(dMesh* mesh, const dModel& model, dBoneVertexWeightData* skinData, int skinDataCount)  
{
	int skinBoneCount;
	int boneIndex[1024];
	dBone* boneArray[1024];

	memset (m_vertexWeight, 0, mesh->m_vertexCount * sizeof (dVector));
	memset (m_boneWeightIndex, 0, mesh->m_vertexCount * sizeof (dBoneWeightIndex));

	skinBoneCount = 0;
	memset (boneIndex, 0xff, sizeof (boneIndex));
	for (int i = 0; i < skinDataCount; i ++) {
		_ASSERTE (skinData[i].m_weight > 0.0f);
		int boneID = skinData[i].m_boneId;
		if (boneIndex[boneID] == -1) {
			boneIndex[boneID] = skinBoneCount;
			boneArray[skinBoneCount] = model.FindBone (boneID);
			skinBoneCount ++;
		}

		int vertexIndex = skinData[i].m_vertexIndex;
		for (int j = 0; j < 4; j ++) {
			if (m_vertexWeight[vertexIndex][j] == 0.0f) {
				_ASSERTE (boneIndex[boneID] != -1);
				m_vertexWeight[vertexIndex][j] = skinData[i].m_weight;
				m_boneWeightIndex[vertexIndex].m_index[j] = boneIndex[boneID];
				break;
			}
		}
	}

	if (m_skinnedBones) {
		free (m_skinnedBones);
		free (m_bindingMatrices);
	}

	m_bonesCount = skinBoneCount;
	m_skinnedBones = (const dBone**) malloc (m_bonesCount * sizeof (dBone*));
	m_bindingMatrices = (dMatrix*) malloc (m_bonesCount * sizeof (dMatrix));

	for (int i = 0; i < m_bonesCount; i ++) {
		dBone* bone;
		bone = boneArray[i]; 
		dMatrix matrix (bone->CalcGlobalMatrix ());

		m_skinnedBones[i] = bone;
		m_bindingMatrices[i] = matrix.Inverse();
	}
}

void dSkinModifier::ApplyGlobalScale(dFloat scale) 
{
	dMatrix scaleMatrix (GetIdentityMatrix());
	scaleMatrix[0][0] = scale;
	scaleMatrix[1][1] = scale;
	scaleMatrix[2][2] = scale;

	dMatrix transformInv (scaleMatrix);
	transformInv[0][0] = 1.0f/transformInv[0][0];
	transformInv[1][1] = 1.0f/transformInv[1][1];
	transformInv[2][2] = 1.0f/transformInv[2][2];

	for (int i = 0; i < m_bonesCount; i ++) {
		m_bindingMatrices[i] = transformInv * m_bindingMatrices[i] * scaleMatrix;
	}
}


void dSkinModifier::ApplyGlobalTransform (const dMatrix& transform)
{
	dMatrix transformInv (transform.Inverse());

	for (int i = 0; i < m_bonesCount; i ++) {
		m_bindingMatrices[i] = transformInv * m_bindingMatrices[i] * transform;
	}
}

