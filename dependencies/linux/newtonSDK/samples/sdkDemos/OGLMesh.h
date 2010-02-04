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

#ifndef __OGL_MESH_H__
#define __OGL_MESH_H__

#include <dMesh.h>
//#include <Newton.h>



class OGLMesh: public dMesh
{
public:
	OGLMesh();
	OGLMesh(const NewtonCollision* collision, const char* texture0, const char* texture1, const char* texture2);
	~OGLMesh(void);

	NewtonMesh* BuildMesh (const NewtonWorld* world) const;	
	void BuildFromMesh (const NewtonMesh* mesh);
	void Render(const dMatrix* matrixPallete = NULL);
	void OptimizeForRender ();

	private:
	void RenderSoftwareSimpleMesh() const;
	void RenderSoftwareSkin(const dMatrix* matrixPallete) const;

	virtual void GetName (char* nameOut) const;
	virtual void GetTextureName (const dSubMesh* subMesh, char* nameOut) const;


	public:
	static dMatrix m_matrixPalete[];
	static dFloat m_weigtedVertex[];
	static dFloat m_weigtedNormal[];

};

#endif

