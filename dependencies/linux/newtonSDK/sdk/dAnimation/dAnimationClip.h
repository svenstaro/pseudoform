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

#ifndef _dAnimationClip_
#define _dAnimationClip_

#include <dList.h>
#include <dVector.h>
#include <dQuaternion.h>
#include <dRefCounter.h>


class TiXmlElement;
class dPoseGenerator;

class dKeyFrames
{
    public:
    dKeyFrames();
    ~dKeyFrames();

	void AllocaFrames (int count);

	int FindKey(float entry) const;

    char m_bindName[32];
    int m_keyFrameCounts;
    int* m_keys;
    dVector* m_posit;
    dQuaternion* m_rotation;
};

class dAnimationClip: public dList<dKeyFrames>, virtual public dRefCounter
{
	public:
	dAnimationClip(void);

#ifdef LOAD_SAVE_XML
	void LoadXML (const char* fileName);
	void SaveXML (const char* fileName) const;
#endif

	void RemoveNode (const char* fileName);

	int GetFramesCount() const;
	void SetFramesCount(int count);

	protected:
	~dAnimationClip(void);

	int m_framesCount;
	friend class dPoseGenerator;
};


#endif
