//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// basic Hierarchical Scene Node Class
//********************************************************************


#include <dAnimationStdAfx.h>
#include <time.h>


#include <dList.h>
#include <dTree.h>
#include <dBone.h>
#include <dMesh.h>
#include <dModel.h>
#include <dSkinModifier.h>
#include <dQuaternion.h>
#include <Newton.h>
#include <dGlueToCollada.h>




//#include <toolbox_stdafx.h>
//#include "dCRC.h"

//#include "dVector.h"
//#include "dMatrix.h"
//#include "OpenGlUtil.h"
//#include "dSceneNode.h"

//#include "planecollision.h"
//#include "RenderPrimitive.h"
//#include "TargaToOpenGl.h"

//#include "CustomGear.h"
//#include "Custom6DOF.h"
//#include "CustomHinge.h"
//#include "CustomPulley.h"
//#include "CustomSlider.h"
//#include "CustomUpVector.h"
//#include "CustomWormGear.h"
//#include "CustomCorkScrew.h"
//#include "CustomUniversal.h"
//#include "CustomBallAndSocket.h"

#include <dae.h>
#include <dae/domAny.h>
#include <dom/domCOLLADA.h>
#include <dom/domConstants.h>
#include <dom/domProfile_COMMON.h>


#define D_TEXTURE_PREFIX_NAME			"texture_"
#define D_MATERIAL_PREFIX_NAME			"material_"
#define D_MATERIAL_PREFIX_NAME_EFFECT	"materialFx_"
#define	D_ROOT_NODE_NAME				"rootNode_"
#define	D_NODE_NAME						"node_"


#ifdef _LINUX_VER 
#define stricmp strcasecmp
#endif


class dColladaFileParcel
{
/*
	enum JointType
	{
		m_gear,
		m_hinge,
		m_slider,
		m_pulley,
		m_upVetor,
		m_wormGear,
		m_universal,
		m_corkScrew,
		m_ballsocket,
		m_generic6DOF,
	};


	class dNodePhysicsRigidBody: public dTree<domRigid_body*, unsigned>
	{
	};

	class dRigidBodyMap: public dTree<NewtonBody*, domRigid_body*>
	{
	};


	class dNodeToColladaNodeMap: public dTree<domNode*, dSceneNode*>
	{
	};

	class NewtonCollisionList: public dTree<domGeometry*, const NewtonCollision*> 
	{
	};


	class NewtonBodyList: public dTree<domRigid_body*, const NewtonBody*>
	{
		public:
		static void BodyIterator (const NewtonBody* body)
		{
			NewtonBodyList* list;
			// get the collada database and add this bod y to the world
			list = (NewtonBodyList*) NewtonWorldGetUserData(NewtonBodyGetWorld(body));
			list->Insert ((domRigid_body*)NULL, body);
		}
	};

	class NewtonJointList: public dTree<domRigid_constraint*, const NewtonJoint*>
	{
		public:
		static void JointIterator (const NewtonJoint* joint)
		{
			const NewtonBody* body;
			NewtonJointRecord info;
			NewtonJointList* list;
			
			NewtonJointGetInfo (joint, &info);

			body = info.m_attachBody_0;
			if (!body) {
				body = info.m_attachBody_1;
			}
			
			// get the collada database and add this bod y to the world
			list = (NewtonJointList*) NewtonWorldGetUserData(NewtonBodyGetWorld(body));
			list->Insert ((domRigid_constraint*) NULL, joint);
		}
	};

	class NewtonFloatExt: public domTargetableFloat
	{
		public:
		NewtonFloatExt ()
		{
		}
	};
*/

	struct SourceBuffer
	{
		int m_count;
		int m_stride;
		const char* m_id;
		domFloat *m_data;
	};



	struct CollImage
	{
//		GLuint m_texture;
		char m_textureName[D_NAME_STRING_LENGTH/2];
		char m_texturePathName[D_NAME_STRING_LENGTH];
	};

	class CollImageCache: public dTree<CollImage, int>
	{
		public: 

		CollImageCache()
			:dTree<CollImage, int>()
		{
		}

		void AddTexture (const char* name)
		{
			CollImage texture;
			const char* ptr;

			ptr = strrchr (name, '/');
			if (ptr) {
				ptr ++;
			} else {
				ptr = strrchr (name, '\\');
				if (ptr) {
					ptr ++;
				} else {
					ptr = name;
				}
			}

//			LoadTexture(ptr, texture.m_texture);
			sprintf (texture.m_textureName, "%s", ptr);
			sprintf (texture.m_texturePathName, "%s", name);
			Insert (texture, dCRC(name));
		}

		const CollImage& GetTexture (const char* name)  const
		{
			dTreeNode* node;

			node = Find (dCRC(name));
			_ASSERTE (node);
			return node->GetInfo();
		}
	};


	struct CollMaterial
	{
		//	GLuint m_texture;
		//	char m_name[64];
		CollImage m_texture;
	};

	class CollMaterialCache: public dTree<CollMaterial, int>
	{
		public: 
		CollMaterialCache()
			:dTree<CollMaterial, int>()
		{
		}

		void AddMaterial (const CollMaterial& material, const char* name)
		{
			Insert (material, dCRC(name));
		}

		CollMaterial* GetMaterial (const char* name) const
		{
			dTreeNode* node;

			node = Find (dCRC(name));
			return node ? &node->GetInfo() : NULL;
		}
	};

	class CollGeoCache: public dTree<dMesh*, domGeometry*>
	{
		public:

		dMesh* FindByName (const char* name) const
		{
			domGeometry* key;
			Iterator iter (*this);
			for (iter.Begin(); iter; iter ++) {
				key = iter.GetKey();
				if (!stricmp (name, key->getId())) {
					return iter.GetNode()->GetInfo();
				}
			}
			return NULL;
		}

		~CollGeoCache ()
		{
			dMesh* geometry;

			while (GetRoot()) {
				geometry = GetRoot()->GetInfo();
				Remove(GetRoot());
				_ASSERTE (geometry);
				geometry->Release();
			}
		}
	};


	class VerterMap
	{
		public:
		int m_vertexIndexInColladaMesh;
		int m_vertexIndexInDMesh;
	};
	
	class ModifierVertexCache: public dTree<VerterMap*, dMesh*>
	{
		public:
		ModifierVertexCache()
		{
		}

		~ModifierVertexCache()
		{	
			Iterator iter (*this);
			for (iter.Begin(); iter; iter ++) {
				VerterMap* map = (*iter);
				delete[] map;
			}
		}
	};

	class ColladaNodeTodSceneNodeMap: public dTree<dBone*, domNode*>
	{
	};


	public:
//	dColladaFileParcel (DAE* collada, NewtonWorld* world)
	dColladaFileParcel (DAE* collada, const dMatrix& globalRotation, dFloat scale)
		:m_globalRotation (globalRotation), m_scale (scale) 
	{
//		m_world = world; 
		m_nodeIndex = 0;
		m_collada = collada; 
//		m_rootNode = NULL;
//		m_loaderContext = NULL;
//		m_cameraNode = NULL;
//		m_collCamera = NULL;
	}

	~dColladaFileParcel ()
	{
	}


	void AddAsset (daeDocument *document)
	{
		domAsset *asset;
		domCOLLADA *domRoot;
		domAsset::domCreated *created;
		domAsset::domModified *modified;
		domAsset::domUnit *unit;
		domAsset::domUp_axis* axis;
		domAsset::domContributor* contributor;
		domAsset::domContributor::domAuthor* autor;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		asset = daeSafeCast<domAsset>(domRoot->createAndPlace(COLLADA_ELEMENT_ASSET));


		contributor = daeSafeCast<domAsset::domContributor>(asset->createAndPlace(COLLADA_ELEMENT_CONTRIBUTOR));
		autor = daeSafeCast<domAsset::domContributor::domAuthor>(contributor->createAndPlace(COLLADA_ELEMENT_AUTHOR));
		autor->setValue("Newton Game Dynamics 2.00 Collada Export");

		created = daeSafeCast<domAsset::domCreated>(asset->createAndPlace(COLLADA_ELEMENT_CREATED));

		time_t rawtime;
		tm * ptm;
		time ( &rawtime );
		ptm = gmtime ( &rawtime );

		char date[256];
		sprintf (date, "%d-%02d-%02dT%02d:%02d:%02dZ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

		//created->setValue("2010-01-04T15:56:43Z");
		created->setValue(date);
		
		modified = daeSafeCast<domAsset::domModified>(asset->createAndPlace(COLLADA_ELEMENT_MODIFIED));
		modified->setValue(date);

		unit = daeSafeCast<domAsset::domUnit>(asset->createAndPlace(COLLADA_ELEMENT_UNIT));
		unit->setName("meter");
		unit->setMeter (1.0);

		axis = daeSafeCast<domAsset::domUp_axis>(asset->createAndPlace(COLLADA_ELEMENT_UP_AXIS));
		axis->setValue(UPAXISTYPE_Y_UP);
//		axis->setValue(UPAXISTYPE_Z_UP);
	}

	void AddCameraLibrary (daeDocument* document, const dModel* model)
	{
/*
		domCOLLADA *domRoot;
		CameraNode* camera;
		domLibrary_cameras *library;
		domCamera::domOptics* optics;
		domCamera::domOptics::domTechnique_common* technique;
		domCamera::domOptics::domTechnique_common::domPerspective* perpective;

		camera = (CameraNode*) rootNode->Find(CAMERA_NAME);
		//		_ASSERTE (camera);
		if (camera) {
			domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

			library = daeSafeCast<domLibrary_cameras>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_CAMERAS));

			m_collCamera = daeSafeCast<domCamera>(library->createAndPlace(COLLADA_ELEMENT_CAMERA));
			m_collCamera->setId(CAMERA_NAME);
			optics = daeSafeCast<domCamera::domOptics>(m_collCamera->createAndPlace(COLLADA_ELEMENT_OPTICS));
			technique = daeSafeCast<domCamera::domOptics::domTechnique_common>(optics->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			perpective = daeSafeCast<domCamera::domOptics::domTechnique_common::domPerspective>(technique->createAndPlace(COLLADA_ELEMENT_PERSPECTIVE));

			domTargetableFloat* fov; 
			domTargetableFloat* aspect; 
			domTargetableFloat* farPlane;
			domTargetableFloat* nearPlane;

			fov = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_YFOV));
			aspect = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_ASPECT_RATIO));
			nearPlane = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_ZNEAR));
			farPlane = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_ZFAR));

			fov->setValue(camera->m_fov);
			aspect->setValue(camera->m_aspect);
			nearPlane->setValue(camera->m_nearPlane);
			farPlane->setValue(camera->m_farPlane);
		}
*/
	}


	void AddImageLibrary (daeDocument* document, const dModel* model, dTree<const char*, unsigned>& textureFilter)
	{
		domCOLLADA *domRoot;
		domLibrary_images* library;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		if (domRoot->getLibrary_images_array().getCount()) {
			library = domRoot->getLibrary_images_array()[0];
		} else {
			library = daeSafeCast<domLibrary_images>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_IMAGES));
		}

		for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
			dMeshInstance& intance = node->GetInfo();
			if (!intance.IsIntance()) {
				dMesh* mesh = intance.m_mesh;
				for (dMesh::dListNode* subMeshNode = mesh->GetFirst(); subMeshNode; subMeshNode = subMeshNode->GetNext()) {
					int crcCode;
					dSubMesh* subMesh = &subMeshNode->GetInfo();
					char texName[256];
//					const char* texName = context->GetTextureName(mesh, subMesh);
					mesh->GetTextureName(subMesh, texName);
					crcCode = dCRC (texName);
					if (crcCode && textureFilter.Insert(texName, crcCode)) {

						char name[256];
						domImage* image;
						domImage::domInit_from* imagefrom;

						sprintf (name, "%s%x", D_TEXTURE_PREFIX_NAME, crcCode);
						image = daeSafeCast<domImage>(library->createAndPlace(COLLADA_ELEMENT_IMAGE));
						image->setId (name);

//						const char* ptrName = strrchr (texName, '/');
//						if (ptrName) {
//							ptrName ++;
//						} else {
//							ptrName = strrchr (texName, '\\');
//							if (ptrName) {
//								ptrName ++;
//							} else {
//								ptrName = texName;
//							}
//						}
//						sprintf (name, "./%s", ptrName);
						imagefrom = daeSafeCast<domImage::domInit_from>(image->createAndPlace(COLLADA_ELEMENT_INIT_FROM));
						imagefrom->setValue (texName);
					}
				}
			}
		}
	}

	void AddDefaultMaterial (domLibrary_effects* effectLibrary, domLibrary_materials* materialLibrary)
	{
		domEffect *effect;
		domMaterial* material;
		domInstance_effect *instanceEffect;
		char materialName[128];

		sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, dCRC (0));

		material = daeSafeCast<domMaterial>(materialLibrary->createAndPlace(COLLADA_ELEMENT_MATERIAL));
		material->setId (materialName);

		// create an effect and add instance to the material
		effect = daeSafeCast<domEffect>(effectLibrary->createAndPlace(COLLADA_ELEMENT_EFFECT));
		sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME_EFFECT, dCRC (0));
		effect->setId (materialName);

		instanceEffect = daeSafeCast<domInstance_effect>(material->createAndPlace(COLLADA_ELEMENT_INSTANCE_EFFECT ));
		daeURI uri (*effect);
		uri.set("", "", "", "", effect->getId());
		//uri.setElement (effect);
		//uri.resolveURI();
		instanceEffect->setUrl (uri);

		// add properties to the effect
		// need to copy the material properties into the effect, 
		//const dgMaxMaterial& myMaterial = *iter;
		domProfile_COMMON *profile = daeSafeCast<domProfile_COMMON>(effect->createAndPlace(COLLADA_ELEMENT_PROFILE_COMMON));

		domProfile_COMMON::domTechnique *teqchnique = daeSafeCast<domProfile_COMMON::domTechnique>(profile->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
		teqchnique->setSid("common");
		domProfile_COMMON::domTechnique::domLambert *lambert = daeSafeCast<domProfile_COMMON::domTechnique::domLambert>(teqchnique->createAndPlace(COLLADA_ELEMENT_LAMBERT));

		domCommon_color_or_texture_type *diffuse = daeSafeCast<domCommon_color_or_texture_type>(lambert->createAndPlace(COLLADA_ELEMENT_DIFFUSE));
		domCommon_color_or_texture_type::domColor *color = daeSafeCast<domCommon_color_or_texture_type::domColor>(diffuse->createAndPlace(COLLADA_ELEMENT_COLOR));
		domFx_color_common& value = color->getValue();
		color->getValue().append(0.7f);
		color->getValue().append(0.7f);
		color->getValue().append(0.7f);
		color->getValue().append(1.0f);
		value.setCount(4);
		value[0] = 0.7f;
		value[1] = 0.7f;
		value[2] = 0.7f;
		value[3] = 1.0f;
	}

	domMaterial* FindMaterial (domLibrary_materials* library, const char*name) const
	{
		unsigned i;
		domMaterial* material;

		if (name && library) {
			const domMaterial_Array& array = library->getMaterial_array();
			for (i = 0; i < array.getCount(); i ++) {
				material = array[i];
				if (!stricmp (name, material->getId())) {
					return material;
				}
			}
		}
		return NULL;
	}

	domEffect* FindEffect (domLibrary_effects* library, const char*name) const
	{
		unsigned i;
		domEffect* effect;
		if (name && library) {
			const domEffect_Array& array = library->getEffect_array();
			for (i = 0; i < array.getCount(); i ++) {
				effect = array[i];
				if (effect->getId()) {
					if (!strcmp (name, effect->getId())) {
						return effect;
					}
				}
			}
		}
		return NULL;
	}


	void AddMaterialLibrary (daeDocument* document, const dModel* model)
	{
		domCOLLADA *domRoot;
		domLibrary_effects* effectLibrary;
		domLibrary_materials* materialLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot() );

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

		// create an material and effect library
		if (domRoot->getLibrary_effects_array().getCount()) {
			effectLibrary = domRoot->getLibrary_effects_array()[0];
		} else {
			effectLibrary = daeSafeCast<domLibrary_effects>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_EFFECTS));
		}

		if (domRoot->getLibrary_materials_array().getCount()) {
			materialLibrary = domRoot->getLibrary_materials_array()[0];
		} else {
			materialLibrary = daeSafeCast<domLibrary_materials>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_MATERIALS));
			AddDefaultMaterial (effectLibrary, materialLibrary);
		}

		for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
			dMeshInstance& instance = node->GetInfo();
			if (!instance.IsIntance()) {
				dMesh* mesh = instance.m_mesh;
				for (dMesh::dListNode* subMeshNode = mesh->GetFirst(); subMeshNode; subMeshNode = subMeshNode->GetNext()) {
					dMesh* mesh = node->GetInfo().m_mesh;
					int crcCode;
					dSubMesh* subMesh = &subMeshNode->GetInfo();

					char name[256];
					char materialName[256];

					mesh->GetTextureName(subMesh, name);
					crcCode = dCRC (name);
					sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, crcCode);
					if (!FindMaterial(materialLibrary, materialName)) {
						domEffect *effect;
						domMaterial* material;
						domInstance_effect *instanceEffect;

						material = daeSafeCast<domMaterial>(materialLibrary->createAndPlace(COLLADA_ELEMENT_MATERIAL));
						material->setId (materialName);

						// create an effect and add instance to the material
						effect = daeSafeCast<domEffect>(effectLibrary->createAndPlace(COLLADA_ELEMENT_EFFECT));
						sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME_EFFECT, crcCode);
						effect->setId (materialName);

						instanceEffect = daeSafeCast<domInstance_effect>(material->createAndPlace(COLLADA_ELEMENT_INSTANCE_EFFECT ));
						daeURI uri (*effect);
						uri.set("", "", "", "", effect->getId());
//						uri.resolveURI();
						instanceEffect->setUrl( uri );

						// add properties to the effect
						// need to copy the material properties into the effect, 
						//const dgMaxMaterial& myMaterial = *iter;
						domProfile_COMMON *profile = daeSafeCast<domProfile_COMMON>(effect->createAndPlace(COLLADA_ELEMENT_PROFILE_COMMON));

						domProfile_COMMON::domTechnique *teqchnique = daeSafeCast<domProfile_COMMON::domTechnique>(profile->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
						teqchnique->setSid("common");
						domProfile_COMMON::domTechnique::domLambert *lambert = daeSafeCast<domProfile_COMMON::domTechnique::domLambert>(teqchnique->createAndPlace(COLLADA_ELEMENT_LAMBERT));

						domCommon_color_or_texture_type *diffuse = daeSafeCast<domCommon_color_or_texture_type>(lambert->createAndPlace(COLLADA_ELEMENT_DIFFUSE));
						if (!crcCode) {
							domCommon_color_or_texture_type::domColor *color = daeSafeCast<domCommon_color_or_texture_type::domColor>(diffuse->createAndPlace(COLLADA_ELEMENT_COLOR));
							domFx_color_common& value = color->getValue();
							value.setCount(4);
							value[0] = 0.7f;
							value[1] = 0.7f;
							value[2] = 0.7f;
							value[3] = 1.0f;
	//							domCommon_color_or_texture_typeRef elemEmission;
	//							domCommon_color_or_texture_typeRef elemAmbient;
	//							domCommon_color_or_texture_typeRef elemDiffuse;
	//							domCommon_color_or_texture_typeRef elemReflective;
	//							domCommon_float_or_param_typeRef elemReflectivity;
	//							domCommon_transparent_typeRef elemTransparent;
	//							domCommon_float_or_param_typeRef elemTransparency;
	//							domCommon_float_or_param_typeRef elemIndex_of_refraction;

						} else {
							char nameID [128];
							char nameIDimage [128];
							domCommon_newparam_type *np;

							sprintf (nameID, "%s%x", D_TEXTURE_PREFIX_NAME, crcCode);							
							sprintf (nameIDimage, "%s_image", nameID);

							np = daeSafeCast<domCommon_newparam_type>(profile->createAndPlace(COLLADA_ELEMENT_NEWPARAM));
							np->setSid (nameIDimage);
							domFx_surface_common *surface = daeSafeCast<domFx_surface_common>(np->createAndPlace(COLLADA_ELEMENT_SURFACE));
							surface->setType( FX_SURFACE_TYPE_ENUM_2D );
							domFx_surface_init_from_common *surfIF = daeSafeCast<domFx_surface_init_from_common>(surface->createAndPlace(COLLADA_ELEMENT_INIT_FROM));
							surfIF->setValue (nameID);
							domFx_surface_common_complexType::domFormat *format = daeSafeCast<domFx_surface_common_complexType::domFormat>(surface->createAndPlace(COLLADA_ELEMENT_FORMAT));
							format->setValue("A8R8G8B8");

							np = daeSafeCast<domCommon_newparam_type>(profile->createAndPlace(COLLADA_ELEMENT_NEWPARAM));
							//np->setSid( "sampler" );
							np->setSid (nameID);
							domFx_sampler2D_common *sampler = daeSafeCast<domFx_sampler2D_common>(np->createAndPlace(COLLADA_ELEMENT_SAMPLER2D));
							domFx_sampler2D_common::domSource *sampSrc = daeSafeCast<domFx_sampler2D_common::domSource>(sampler->createAndPlace(COLLADA_ELEMENT_SOURCE));
							sampSrc->setValue( nameIDimage );

							domCommon_color_or_texture_type::domTexture *texture = daeSafeCast<domCommon_color_or_texture_type::domTexture>(diffuse->createAndPlace(COLLADA_ELEMENT_TEXTURE));
							texture->setTexture( nameID );
							texture->setTexcoord ("CHANNEL1");
						}
					}
				}
			}
		}
	}

	domGeometry *AddGeometry (daeDocument* document, const dMesh* geometry)
	{
		domCOLLADA *domRoot;
		domMesh *colladaMesh;
		domGeometry *collGeometry;
		domLibrary_geometries *library;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		if (domRoot->getLibrary_geometries_array().getCount()) {
			library = domRoot->getLibrary_geometries_array()[0];
		} else {
			library = daeSafeCast<domLibrary_geometries>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_GEOMETRIES));
		}
		_ASSERTE (library);

		collGeometry = daeSafeCast<domGeometry>(library->createAndPlace(COLLADA_ELEMENT_GEOMETRY ));
//		collGeometry->setId(geometry->m_name);

		char meshName[256];
//		meshName = context->GetMeshName (geometry);
		geometry->GetName(meshName);
		collGeometry->setId(meshName);
		colladaMesh = daeSafeCast<domMesh>(collGeometry->createAndPlace(COLLADA_ELEMENT_MESH ));

		domVertices *verts;
		int* vertexArrayIndexList = new int[geometry->m_vertexCount];
		{
			// add the vertices
			char text[256];
			int vertexCount;
			domSource *posSource;
			float* floatPool = new float[geometry->m_vertexCount * 3];

			posSource = daeSafeCast<domSource>(colladaMesh->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_position", meshName);
			posSource->setId (text);

			for (int i = 0; i < geometry->m_vertexCount; i ++) {
				floatPool[i * 3 + 0] = m_scale * geometry->m_vertex[i * 3 + 0];
				floatPool[i * 3 + 1] = m_scale * geometry->m_vertex[i * 3 + 1];
				floatPool[i * 3 + 2] = m_scale * geometry->m_vertex[i * 3 + 2];
			}

			vertexCount = dModel::PackVertexArray (floatPool, 3, 3 * sizeof (dFloat), geometry->m_vertexCount, vertexArrayIndexList);

			m_globalRotation.TransformTriplex(floatPool, sizeof (float) * 3, floatPool, sizeof (float) * 3, geometry->m_vertexCount);
			domFloat_array *fa = daeSafeCast<domFloat_array>(posSource->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			fa->setId( text );
			fa->setCount (vertexCount * 3);

			domListOfFloats &posSrcArray = fa->getValue();
			for (int i = 0; i < vertexCount; i ++) {
				posSrcArray.append3( floatPool[i * 3 + 0], floatPool[i * 3 + 1], floatPool[i * 3 + 2]);
			}

			//create the accessor
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(posSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (vertexCount);
			acc->setStride (3);
			daeURI uri(*fa);
			uri.set("", "", "", "", fa->getId());
			//uri.setElement (fa);
			//uri.resolveURI();
			acc->setSource( uri );

			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "x" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "y" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "z" );
			param->setType( "float" );


			verts = daeSafeCast<domVertices>(colladaMesh->createAndPlace(COLLADA_ELEMENT_VERTICES));
			sprintf (text, "%sVertices", geometry->m_name);
			verts->setId (text);
			domInputLocal *inputLocal = daeSafeCast<domInputLocal>(verts->createAndPlace(COLLADA_ELEMENT_INPUT));
			inputLocal->setSemantic (COMMON_PROFILE_INPUT_POSITION);
			//uri.setElement (posSource);
			daeURI uri1(*posSource);
			uri1.set("", "", "", "", posSource->getId());
			//uri.resolveURI();
			inputLocal->setSource (uri1);

			delete[] floatPool;
		}


		domSource *normalSrc;
		int* normalArrayIndexList = new int[geometry->m_vertexCount];
		{
			char text[256];
			int vertexCount;
			float* floatPool = new float[geometry->m_vertexCount * 3];

			normalSrc = daeSafeCast<domSource>(colladaMesh->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_normal", meshName);
			normalSrc->setId (text);

			for (int i = 0; i < geometry->m_vertexCount; i ++) {
				floatPool[i * 3 + 0] = geometry->m_normal[i * 3 + 0];
				floatPool[i * 3 + 1] = geometry->m_normal[i * 3 + 1];
				floatPool[i * 3 + 2] = geometry->m_normal[i * 3 + 2];
			}
			vertexCount = dModel::PackVertexArray (floatPool, 3, 3 * sizeof (dFloat), geometry->m_vertexCount, normalArrayIndexList);
			dMatrix rot (m_globalRotation);
			rot.m_posit = dVector (0.0f, 0.0f, 0.0f, 1.0f);
			rot.TransformTriplex(floatPool, sizeof (float) * 3, floatPool, sizeof (float) * 3, geometry->m_vertexCount);

			domFloat_array *fa = daeSafeCast<domFloat_array>(normalSrc->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			fa->setId (text);
			fa->setCount (vertexCount * 3);

			domListOfFloats &posSrcArray = fa->getValue();
			for (int i = 0; i < vertexCount; i ++) {
				posSrcArray.append3( floatPool[i * 3 + 0], floatPool[i * 3 + 1], floatPool[i * 3 + 2]);
			}

			//create the accessors
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(normalSrc->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (vertexCount);
			acc->setStride (3);
			daeURI uri(*fa);
			uri.set("", "", "", "", fa->getId());
			//uri.setElement (fa);
			//uri.resolveURI();
			acc->setSource( uri );

			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "x" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "y" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "z" );
			param->setType( "float" );

			delete[] floatPool;
		}

		domSource *uvSource;
		int* uv0ArrayIndexList = new int[geometry->m_vertexCount];
		{
			char text[256];
			int vertexCount;
			float* floatPool = new float[geometry->m_vertexCount * 3];

			uvSource = daeSafeCast<domSource>(colladaMesh->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_uv", meshName);
			uvSource->setId (text);
			for (int i = 0; i < geometry->m_vertexCount; i ++) {
				floatPool[i * 3 + 0] = geometry->m_uv[i * 2 + 0];
				floatPool[i * 3 + 1] = geometry->m_uv[i * 2 + 1];
				floatPool[i * 3 + 2] = 0.0f;
			}
			vertexCount = dModel::PackVertexArray (floatPool, 3, 3 * sizeof (dFloat), geometry->m_vertexCount, uv0ArrayIndexList);

			domFloat_array *fa = daeSafeCast<domFloat_array>(uvSource->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			fa->setId (text);
			fa->setCount(vertexCount * 2);
			domListOfFloats &posSrcArray = fa->getValue();
			for (int i = 0; i < vertexCount; i ++) {
				posSrcArray.append2( floatPool[i * 3 + 0], floatPool[i * 3 + 1]);
			}

			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(uvSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (vertexCount);
			acc->setStride( 2 );
			daeURI uri(*fa);
			uri.set("", "", "", "", fa->getId());
			//uri.setElement( fa );
			//uri.resolveURI();
			acc->setSource( uri );

			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "s" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "t" );
			param->setType( "float" );

			delete[] floatPool;
		}
		
		for (dMesh::dListNode* node = geometry->GetFirst(); node; node = node->GetNext()) {
			char texName[256];
			char materialName[256];
			domTriangles *polys1;
			domInputLocalOffset *input;


			const dSubMesh& subMesh = node->GetInfo();
//			sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, dCRC (subMesh.m_textureName));
			geometry->GetTextureName(&subMesh, texName);
			sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, dCRC (texName));

			polys1 = daeSafeCast<domTriangles>(colladaMesh->createAndPlace(COLLADA_ELEMENT_TRIANGLES));
			polys1->setCount (subMesh.m_indexCount / 3);
			polys1->setMaterial (materialName);


			input = daeSafeCast<domInputLocalOffset>(polys1->createAndPlace(COLLADA_ELEMENT_INPUT ));
			input->setSemantic( COMMON_PROFILE_INPUT_VERTEX );
			input->setOffset( 0 );
			daeURI uri (*verts);
			uri.set("", "", "", "", verts->getId());
			//uri.setElement (verts);
			//uri.resolveURI();
			input->setSource (uri);


			input = daeSafeCast<domInputLocalOffset>(polys1->createAndPlace(COLLADA_ELEMENT_INPUT));
			input->setSemantic( COMMON_PROFILE_INPUT_NORMAL );
			input->setOffset (1);
			daeURI uri1 (*normalSrc);
			uri1.set("", "", "", "", normalSrc->getId());
			//uri.setElement (normalSrc);
			//uri.resolveURI();
			input->setSource (uri1);

			input = daeSafeCast<domInputLocalOffset>(polys1->createAndPlace(COLLADA_ELEMENT_INPUT));
			input->setSemantic (COMMON_PROFILE_INPUT_TEXCOORD);
			input->setOffset (2);
//			input->setSet (1);
			input->setSet (0);
			daeURI uri2 (*uvSource);
			uri2.set("", "", "", "", uvSource->getId());
			//uri.setElement (uvSource);
			//uri.resolveURI();
			input->setSource (uri2);

			domP *p = daeSafeCast<domP>(polys1->createAndPlace(COLLADA_ELEMENT_P));
			domListOfUInts &indices = p->getValue();
			for (int i = 0; i < subMesh.m_indexCount; i ++) {
				int index = subMesh.m_indexes[i];
				indices.append3 (vertexArrayIndexList[index], normalArrayIndexList[index], uv0ArrayIndexList[index]);
			}
		}
		
		delete[] uv0ArrayIndexList;
		delete[] normalArrayIndexList;
		delete[] vertexArrayIndexList;

		return collGeometry;
	}


	void AddGeometryLibrary (daeDocument* document, const dModel* model)
	{
		for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
			dMeshInstance& instance = node->GetInfo();
			if (!instance.IsIntance()) {
				dMesh* mesh = node->GetInfo().m_mesh;
				AddGeometry (document, mesh);
			}
		}
	}


	void AddController (daeDocument* document, dMeshInstance* meshInstance, const dModel* model)
	{
		_ASSERTE (0);
/*
		const char* meshName;
		daeURI uri;
		domSkin* skin;
		domGeometry* geoSource;
		domController *controller;
		dSkinModifier* skinData;
		dMatrix* bindingMatrices;
		const dBone** boneList;
		domCOLLADA *domRoot;
		domLibrary_geometries *geometryLibrary;
		domLibrary_controllers *controllerLibrary;
		char text[256];

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		if (!domRoot->getLibrary_controllers_array().getCount()) {
			daeSafeCast<domLibrary_geometries>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_CONTROLLERS));
		}
		controllerLibrary = domRoot->getLibrary_controllers_array()[0];
		_ASSERTE (controllerLibrary);

		geometryLibrary = domRoot->getLibrary_geometries_array()[0];
		_ASSERTE (geometryLibrary);

		
		skinData = (dSkinModifier*) mesh->GetModifier();
		boneList = skinData->m_skinnedBones;
		bindingMatrices = skinData->m_bindingMatrices;

		controller = daeSafeCast<domController>(controllerLibrary->createAndPlace(COLLADA_ELEMENT_CONTROLLER ));
		meshName = context->GetMeshName(mesh);
		sprintf (text, "%s_skin", meshName);
		controller->setId(text);

		skin = daeSafeCast<domSkin>(controller->createAndPlace(COLLADA_TYPE_SKIN));

		geoSource = NULL;
		const domGeometry_Array& array = geometryLibrary->getGeometry_array();
		for (int i = 0; i < int (array.getCount()); i ++) {
			geoSource = array[i];
			if (!stricmp (meshName, geoSource->getId())) {
				break;
			}
		}
		uri.setElement (geoSource);
		uri.resolveURI();
		skin->setSource (uri);

		dMatrix matrix (model->FindBone (mesh->m_boneID)->CalcGlobalMatrix());
		matrix = matrix.Transpose4X4();
		domSkin::domBind_shape_matrix* bindMatrix;
		bindMatrix = daeSafeCast<domSkin::domBind_shape_matrix>(skin->createAndPlace(COLLADA_TYPE_BIND_SHAPE_MATRIX));
		domFloat4x4& value = bindMatrix->getValue();
		value.setCount(16);
		float* ptr = &matrix[0][0];
		for (int i = 0; i < 16; i ++) {
			value[i] = ptr[i];
		}

		int indexCount;
		domSource *boneSource;
		domSource *bindSource;
		domSource *weightsSource;
		int* weightsIndexMap = new int[mesh->m_vertexCount * 4];
		{
			char text[256];

			boneSource = daeSafeCast<domSource>(skin->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_bones", meshName);
			boneSource->setId (text);

			domName_array *boneNames = daeSafeCast<domName_array>(boneSource->createAndPlace(COLLADA_ELEMENT_NAME_ARRAY));
			strcat (text, "Array");
			boneNames->setId (text);
			boneNames->setCount (skinData->m_bonesCount);
			domListOfNames &boneSrcArray = boneNames->getValue();
			for (int i = 0; i < skinData->m_bonesCount; i ++) {
				const char* name = boneList[i]->GetName();
				boneSrcArray.append(name);
			}

			//create the accessor
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(boneSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (skinData->m_bonesCount);
			acc->setStride (1);
			uri.setElement (boneNames);
			uri.resolveURI();
			acc->setSource (uri);
			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName ("bone");
			param->setType ("Name");
		}


		{
			char text[256];
			bindSource = daeSafeCast<domSource>(skin->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_bindpose", meshName);
			bindSource->setId (text);

			domFloat_array *boneMatrix = daeSafeCast<domFloat_array>(bindSource->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			boneMatrix->setId (text);
			boneMatrix->setCount (skinData->m_bonesCount * 16);
			domListOfFloats &boneSrcArray = boneMatrix->getValue();
			for (int i = 0; i < skinData->m_bonesCount; i ++) {
				dMatrix matrix (bindingMatrices[i]);
				matrix = matrix.Transpose4X4();
				float* data = &matrix[0][0];
				for (int j = 0; j < 16; j ++) {
					boneSrcArray.append(data[j]);
				}
			}


			//create the accessor
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(bindSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (skinData->m_bonesCount);
			acc->setStride (16);
			uri.setElement (bindSource);
			uri.resolveURI();
			acc->setSource (uri);
			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName ("bindMatrix");
			param->setType ("float4x4");
		}

		{
			char text[256];
			weightsSource = daeSafeCast<domSource>(skin->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_weights", meshName);
			weightsSource->setId (text);

			dVector* weightData = new dVector [mesh->m_vertexCount * 4];
			for (int i = 0; i < mesh->m_vertexCount; i ++) {
				for (int j = 0; j < 4; j ++) {
					weightData[i * 4 + j].m_x = skinData->m_vertexWeight[i][j];
					weightData[i * 4 + j].m_y = 0.0f;
					weightData[i * 4 + j].m_z = 0.0f;
				}
			}
			indexCount = dModel::PackVertexArray (&weightData[0].m_x, 3, sizeof (dVector), mesh->m_vertexCount * 4, weightsIndexMap);

			domFloat_array *weights = daeSafeCast<domFloat_array>(weightsSource->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			weights->setId (text);
			weights->setCount (indexCount);
			domListOfFloats &weightsSrcArray = weights->getValue();
			for (int i = 0; i < indexCount; i ++) {
				weightsSrcArray.append(weightData[i].m_x);
			}
			delete[] weightData;

			//create the accessor
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(weightsSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (indexCount);
			acc->setStride (1);
			uri.setElement (weightsSource);
			uri.resolveURI();
			acc->setSource (uri);
			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName ("weight");
			param->setType ("float");
		}


		domSkin::domJoints *joints;
		joints = daeSafeCast<domSkin::domJoints>(skin->createAndPlace(COLLADA_ELEMENT_JOINTS));
		domInputLocal *inputLocal = daeSafeCast<domInputLocal>(joints->createAndPlace(COLLADA_ELEMENT_INPUT));
		inputLocal->setSemantic (COMMON_PROFILE_INPUT_JOINT);
		uri.setElement (boneSource);
		uri.resolveURI();
		inputLocal->setSource (uri);

		inputLocal = daeSafeCast<domInputLocal>(joints->createAndPlace(COLLADA_ELEMENT_INPUT));
		inputLocal->setSemantic (COMMON_PROFILE_INPUT_INV_BIND_MATRIX);
		uri.setElement (bindSource);
		uri.resolveURI();
		inputLocal->setSource (uri);


		domSkin::domVertex_weights *vertexWeights;
		domInputLocalOffset* localInputOffset;
		vertexWeights = daeSafeCast<domSkin::domVertex_weights>(skin->createAndPlace(COLLADA_ELEMENT_VERTEX_WEIGHTS));
		localInputOffset = daeSafeCast<domInputLocalOffset>(vertexWeights->createAndPlace(COLLADA_ELEMENT_INPUT));
		localInputOffset->setSemantic (COMMON_PROFILE_INPUT_JOINT);
		localInputOffset->setOffset (0);
		uri.setElement (boneSource);
		uri.resolveURI();
		localInputOffset->setSource (uri);

		localInputOffset = daeSafeCast<domInputLocalOffset>(vertexWeights->createAndPlace(COLLADA_ELEMENT_INPUT));
		localInputOffset->setSemantic (COMMON_PROFILE_INPUT_WEIGHT);
		localInputOffset->setOffset (1);
		uri.setElement (weightsSource);
		uri.resolveURI();
		localInputOffset->setSource (uri);


		int vertexCount;
		int* vertexArrayIndexList = new int[mesh->m_vertexCount];
		dVector* floatPool = new dVector[mesh->m_vertexCount];
		for (int i = 0; i < mesh->m_vertexCount; i ++) {
			floatPool[i] = dVector (mesh->m_vertex[i * 3 + 0], mesh->m_vertex[i * 3 + 1], mesh->m_vertex[i * 3 + 2], dFloat(i));
		}
		vertexCount = dModel::PackVertexArray (&floatPool[0].m_x, 3, sizeof (dVector), mesh->m_vertexCount, vertexArrayIndexList);

		vertexWeights->setCount(vertexCount);
		domSkin::domVertex_weights::domV *v;
		domSkin::domVertex_weights::domVcount *vCount;
		v = daeSafeCast<domSkin::domVertex_weights::domV>(vertexWeights->createAndPlace(COLLADA_ELEMENT_V));
		vCount = daeSafeCast<domSkin::domVertex_weights::domVcount>(vertexWeights->createAndPlace(COLLADA_ELEMENT_VCOUNT));

		domListOfInts &vIndices = v->getValue();
		domListOfUInts &vCountIndices = vCount->getValue();
//		for (int i = 0; i < mesh->m_vertexCount; i ++) {
		for (int i = 0; i < vertexCount; i ++) {
			int index;
			index = int (floatPool[i].m_w);
			int count = 0;
			for (int j = 0; j < 4; j ++) {
				count += (skinData->m_vertexWeight[index][j] > 0.0f) ? 1 : 0;
			}
			vCountIndices.append(count);

			for (int j = 0; j < count; j ++) {
				int boneIndex = skinData->m_boneWeightIndex[index].m_index[j];
				vIndices.append(boneIndex);
				vIndices.append(weightsIndexMap[index * 4 + j]);
			}
		}

		delete[] floatPool; 
		delete[] vertexArrayIndexList;
		delete[] weightsIndexMap;
*/
	}

	void AddControllerLibrary(daeDocument* document, const dModel* model)
	{
		for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
			dMeshInstance& instance = node->GetInfo();
			if (instance.m_modifier) {
//				dMesh* mesh = node->GetInfo().m_mesh;
				AddController (document, &instance, model);
			}
		}
	}

	domNode* FindNodeNodeByName (domNode *root, const char* name)
	{
		int count;
		char nodeName[256];
		domNode* array[1024];

		sprintf (nodeName, "%s%s", D_NODE_NAME, name);
		count = 1;
		array[0] = root;
		while (count) {
			domNode* node;
			count --;

			node = array[count];
			if (!strcmp (nodeName, node->getId())) {
				return node;
			}

			daeTArray<daeSmartRef<daeElement> > nodes;
			node->getChildren (nodes);
			for (int i = 0; i < int (nodes.getCount()); i ++) {
				daeElement* data = nodes[i];
				if (data->getElementType() == COLLADA_TYPE::NODE) {
					array[count] = (domNode*) data;
					count ++;
				}
			}
		}
		return NULL;
	}


	void AddMeshInstance (daeDocument* document, daeElement *node, dMesh* mesh)
	{
		int geometryCount;
		domCOLLADA *domRoot;
		domLibrary_materials *materialLibrary;
		domLibrary_geometries *geometryLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		materialLibrary = domRoot->getLibrary_materials_array()[0];
		geometryLibrary = domRoot->getLibrary_geometries_array()[0];
		const domGeometry_Array &geometryArray =  geometryLibrary->getGeometry_array();
		geometryCount = int (geometryArray.getCount());

		for (int i = 0; i < geometryCount; i ++) {
			char name[256];
			domGeometry *meshGeometry;
			meshGeometry = geometryArray[i];

			mesh->GetName(name);
			if (!stricmp (meshGeometry->getId(), name)) {
				int polyCount;
				domMesh *colladaMesh;
				domBind_material *bindMat;
				domInstance_geometry *meshInstance;
				domBind_material::domTechnique_common *bmtc;

				meshInstance = daeSafeCast<domInstance_geometry>(node->createAndPlace(COLLADA_ELEMENT_INSTANCE_GEOMETRY));
				daeURI uri(*meshGeometry);
				uri.set("", "", "", "", meshGeometry->getId());
				//uri.setElement (meshGeometry);
				//uri.resolveURI();
				meshInstance->setUrl (uri);

				colladaMesh = meshGeometry->getMesh();
				const domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
				polyCount = int (trianglesArray.getCount());

				bindMat = daeSafeCast<domBind_material>(meshInstance->createAndPlace(COLLADA_ELEMENT_BIND_MATERIAL));
				bmtc = daeSafeCast<domBind_material::domTechnique_common>(bindMat->createAndPlace (COLLADA_ELEMENT_TECHNIQUE_COMMON));

				for (int j = 0; j < polyCount; j ++) {
					domMaterial* material;
					domTriangles* triangles; 
					domInstance_material *instMat;

					triangles = trianglesArray[j];
					instMat = daeSafeCast<domInstance_material>(bmtc->createAndPlace(COLLADA_ELEMENT_INSTANCE_MATERIAL ));
					material = FindMaterial (materialLibrary, triangles->getMaterial());
					_ASSERTE (material);
					daeURI uri(*material);
					uri.set("", "", "", "", material->getId());
					//uri.setElement (material);
					//uri.resolveURI();
					instMat->setSymbol(triangles->getMaterial());
					instMat->setTarget (uri);
				}
				break;
			}
		}
	}


	void AddControllerInstance (daeDocument* document, domNode *root, domNode *node, dMesh* mesh)
	{
		_ASSERTE (0);
/*
		char name[256];
		domCOLLADA *domRoot;
		domLibrary_controllers *controllerLibrary;

		sprintf (name, "%sSkin", context->GetMeshName (mesh));

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		controllerLibrary = domRoot->getLibrary_controllers_array()[0];
		const domController_Array &controllerArray = controllerLibrary->getController_array();

		int controllersCount = int (controllerArray.getCount());
		for (int i = 0; i < controllersCount; i ++) {
			daeURI uri;
			domController *meshController;
		
			meshController = controllerArray[i];
			if (!stricmp (meshController->getId(), name)) {
				int geometryCount;
				domNode* rootBone;
				domLibrary_materials *materialLibrary;
				domLibrary_geometries *geometryLibrary;
				domInstance_controller *controllerInstance;
				domInstance_controller::domSkeleton* skeleton;

				controllerInstance = daeSafeCast<domInstance_controller>(node->createAndPlace(COLLADA_ELEMENT_INSTANCE_CONTROLLER));
				uri.setElement (meshController);
				uri.resolveURI();
				controllerInstance->setUrl (uri);

				skeleton = daeSafeCast<domInstance_controller::domSkeleton>(controllerInstance->createAndPlace(COLLADA_ELEMENT_SKELETON));

				rootBone = FindNodeNodeByName (root, ((dSkinModifier*) mesh->GetModifier())->m_rootBone->GetName());
				uri.setElement (rootBone);
				uri.resolveURI();
				skeleton->setValue(uri);

				materialLibrary = domRoot->getLibrary_materials_array()[0];
				geometryLibrary = domRoot->getLibrary_geometries_array()[0];
				const domGeometry_Array &geometryArray = geometryLibrary->getGeometry_array();
				geometryCount = int (geometryArray.getCount());

				for (int i = 0; i < geometryCount; i ++) {
					domGeometry *meshGeometry;
					meshGeometry = geometryArray[i];

					if (!stricmp (meshGeometry->getId(), context->GetMeshName(mesh))) {
						int polyCount;
						domMesh *colladaMesh;
						domBind_material *bindMat;
						domBind_material::domTechnique_common *bmtc;

						colladaMesh = meshGeometry->getMesh();
						const domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
						polyCount = int (trianglesArray.getCount());

						bindMat = daeSafeCast<domBind_material>(controllerInstance->createAndPlace(COLLADA_ELEMENT_BIND_MATERIAL));
						bmtc = daeSafeCast<domBind_material::domTechnique_common>(bindMat->createAndPlace (COLLADA_ELEMENT_TECHNIQUE_COMMON));

						for (int j = 0; j < polyCount; j ++) {
							domMaterial* material;
							domTriangles* triangles; 
							domInstance_material *instMat;

							triangles = trianglesArray[j];
							instMat = daeSafeCast<domInstance_material>(bmtc->createAndPlace(COLLADA_ELEMENT_INSTANCE_MATERIAL ));
							material = FindMaterial (materialLibrary, triangles->getMaterial());
							_ASSERTE (material);
							uri.setElement (material);
							uri.resolveURI();
							instMat->setSymbol(triangles->getMaterial());
							instMat->setTarget (uri);
						}
						break;
					}
				}
				break;
			}
		}
*/
	}

	dMatrix GetMatrix (domNode* node) const
	{
		dMatrix matrix (GetIdentityMatrix());

		domMatrix_Array matrixArray = node->getMatrix_array();
		if (matrixArray.getCount()) {
			for (int i = 0; i < int (matrixArray.getCount()); i ++) {
				const domFloat4x4& data = matrixArray[i]->getValue();
				dMatrix colladaMatrix; 
				for (int j = 0; j < 4; j ++) {
					for (int k = 0; k < 4; k ++) {
						colladaMatrix[k][j] = dFloat (data[j * 4 + k]);
					}
				}
				matrix = colladaMatrix * matrix;
			}

		} else {
			domTranslate_Array &translationArray = node->getTranslate_array();
			for (int i = 0; i < int (translationArray.getCount()); i ++) {
				const domFloat3& data = translationArray[i]->getValue();
				matrix[3][0] += dFloat (data[0]);
				matrix[3][1] += dFloat (data[1]);
				matrix[3][2] += dFloat (data[2]);
			}

			domRotate_Array &rotationArray = node->getRotate_array();
			for (int i = 0; i < int (rotationArray.getCount()); i ++) {
				dFloat angle;
				const domFloat4& data = rotationArray[i]->getValue();
				angle = dFloat (data[3]) * 3.1316f / 180.0f;
				dFloat x = dFloat (data[0]);
				dFloat y = dFloat (data[1]);
				dFloat z = dFloat (data[2]);
				dVector axis (x, y, z, 0.0f);
				axis = axis.Scale (1.0f / sqrtf (axis % axis));
				dQuaternion rot (axis, angle);
				dMatrix colladaMatrix (rot, dVector (0.0f, 0.0f, 0.0f, 1.0f));
				matrix = colladaMatrix * matrix;
			}
		}
		return matrix;
	}


	void AddNodes(daeDocument* document, dBone* rootNode, daeElement *parent, const dModel* model)
	{
		domNode *tmChild; 
		char name[128];

		if (rootNode->GetName()[0]) {
			sprintf (name, "%s%s", D_NODE_NAME, rootNode->GetName());
		} else {
			sprintf (name, "%s%04d", D_NODE_NAME, m_nodeIndex);
			m_nodeIndex ++;
		}

		tmChild = daeSafeCast<domNode>(parent->createAndPlace(COLLADA_ELEMENT_NODE));
		tmChild->setId (name);
		tmChild->setSid(rootNode->GetName());
		tmChild->setType ((rootNode->GetType() == dBone::m_bone) ? NODETYPE_JOINT : NODETYPE_NODE);
		

		// add this node to the map
//		domInstance_camera* cameraIntance;
//		m_visualNodeMap.Insert (tmChild, mesh);
		//	if this is a camera node add the camera instance
//		if ((mesh == m_cameraNode) && m_collCamera) {
//			cameraIntance = daeSafeCast<domInstance_camera>(tmChild->createAndPlace(COLLADA_ELEMENT_INSTANCE_CAMERA));
//			uri.setElement (m_collCamera);
//			uri.resolveURI();
//			cameraIntance->setUrl (uri);
//		}

		dMatrix matrix (m_globalRotation.Inverse() * rootNode->GetMatrix() * m_globalRotation);

		domTranslate* position = daeSafeCast<domTranslate>(tmChild->createAndPlace(COLLADA_ELEMENT_TRANSLATE));
		domFloat3& positValue = position->getValue();
		positValue.setCount(3);
		positValue[0] = matrix.m_posit.m_x * m_scale;
		positValue[1] = matrix.m_posit.m_y * m_scale;
		positValue[2] = matrix.m_posit.m_z * m_scale;

		domRotate* rotation = daeSafeCast<domRotate>(tmChild->createAndPlace(COLLADA_ELEMENT_ROTATE));
		domFloat4& rotateValue = rotation->getValue();
		rotateValue.setCount(4);
		dQuaternion rot (matrix);
		dVector dir (rot.m_q1, rot.m_q2, rot.m_q3, 0.0f);
		float mag2 = dir % dir;
		if (mag2 > 1.0e-6f) {
			dir = dir.Scale (1.0f / dSqrt (mag2));
		} else {
			dir = dVector (1.0f, 0.0f, 0.0f, 0.0f);
		}
		rotateValue[0] = dir.m_x;
		rotateValue[1] = dir.m_y;
		rotateValue[2] = dir.m_z;
		rotateValue[3] = (2.0f * dAcos (rot.m_q0)) * (180.0f / 3.14159265f);

//		dMesh* mesh;
//		mesh = model->FindMesh(rootNode->GetName());
//		if (mesh) {
//			if (mesh->GetType() == dMesh::D_STATIC_MESH) {
//				AddMeshInstance (document, tmChild, mesh);
//			} else {
//				AddControllerInstance (document, tmChild, mesh);
//			}
//		}

		for (dBone* ptr = rootNode->GetChild(); ptr; ptr = ptr->GetSibling()) {
			AddNodes(document, ptr, tmChild, model);
		}
	}


	void AddMeshInstaceToScene(daeDocument* document, daeElement *parent, const dModel* model, dMeshInstance* meshInstance)
	{
		if (meshInstance->m_mesh->m_hasBone) {
			dBone* bone;
			domNode* node;
			bone = model->FindBone(meshInstance->m_boneID);
			_ASSERTE (bone);
			node = FindNodeNodeByName ((domNode *)parent, bone->GetName());
			_ASSERTE (node);
			if (meshInstance->m_modifier) {
				_ASSERTE (0);
				//AddControllerInstance (document, (domNode*) parent, node, mesh);
			} else {
				AddMeshInstance (document, node, meshInstance->m_mesh);
			}
		} else {
			_ASSERTE (0);
/*
			dBone tmpBone(NULL);
			tmpBone.SetMatrix(model->m_matrix);
			tmpBone.SetNameID(context->GetMeshName(mesh));
			AddNodes(document, &tmpBone, parent, model, context);
			node = FindNodeNodeByName ((domNode *)parent, context->GetMeshName(mesh));
*/
		}
	}


	void AddVisualSceneLibrary (daeDocument* document, const dModel* model)
	{
		domCOLLADA *domRoot;
		domVisual_scene *scene;

		scene = NULL;
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		if (!domRoot->getLibrary_visual_scenes_array().getCount()) {
			char sceneName [256];
			domLibrary_visual_scenes *library;
			sprintf (sceneName, "%sVisual", D_ROOT_NODE_NAME);
			library = daeSafeCast<domLibrary_visual_scenes>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_VISUAL_SCENES ));
			scene = daeSafeCast<domVisual_scene>(library->createAndPlace(COLLADA_ELEMENT_VISUAL_SCENE));
			scene->setId (sceneName);
		}
		scene = domRoot->getLibrary_visual_scenes_array()[0]->getVisual_scene_array()[0];
		_ASSERTE (scene);

//		m_cameraNode = (CameraNode*) rootNode->Find(CAMERA_NAME);;
/*
		if (saveRoot) {
			_ASSERTE(0);
			AddNodes (document, rootNode, scene);
		} else {
			for (node = rootNode->GetChild(); node; node = node->GetSibling()) {
				AddNodes (document, node, scene);
			}
		}
*/
	
		// save skeleton hierarchy 
		for (ModelComponentList<dList<dBone*> >::dListNode* list = model->m_skeleton.GetFirst(); list; list = list->GetNext()) {
			for (dList<dBone*>::dListNode* node = list->GetInfo().m_data.GetFirst(); node; node = node->GetNext()) { 
				dBone* rootNode = node->GetInfo();
				AddNodes(document, rootNode, scene, model);
			}
		}

		// same all mesh referenced by each node
		for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
			AddMeshInstaceToScene(document, scene, model, &node->GetInfo());
		}
	}


	void AddScene (daeDocument* document)
	{
		domCOLLADA *domRoot;
		domInstanceWithExtra *ivs;
		domCOLLADA::domScene *sceneInstance;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		sceneInstance = daeSafeCast<domCOLLADA::domScene>(domRoot->createAndPlace(COLLADA_ELEMENT_SCENE));

		if (domRoot->getLibrary_visual_scenes_array().getCount()) {
			domLibrary_visual_scenes *visualLibrary;
			visualLibrary = domRoot->getLibrary_visual_scenes_array()[0];
			domVisual_scene_Array& visualScenes = visualLibrary->getVisual_scene_array();
			for (int i = 0; i <  int (visualScenes.getCount()); i ++ ) {
				
				domVisual_scene *visualScene;
				visualScene = visualScenes[0];
				ivs = daeSafeCast<domInstanceWithExtra>(sceneInstance->createAndPlace(COLLADA_ELEMENT_INSTANCE_VISUAL_SCENE));
				daeURI uri (*visualScene);
				uri.set("", "", "", "", visualScene->getId());
				//uri.setElement (visualScene);
				//uri.resolveURI();
				ivs->setUrl (uri);
			}
		}

		if (domRoot->getLibrary_physics_scenes_array().getCount()) {
			domLibrary_physics_scenes *physicsLibrary;
			physicsLibrary = domRoot->getLibrary_physics_scenes_array()[0];
			domPhysics_scene_Array& physicsScenes = physicsLibrary->getPhysics_scene_array();
			for (int i = 0; i <  int (physicsScenes.getCount()); i ++ ) {
				domPhysics_scene *physicsScene;
				physicsScene = physicsScenes[0];
				ivs = daeSafeCast<domInstanceWithExtra>(sceneInstance->createAndPlace(COLLADA_ELEMENT_INSTANCE_PHYSICS_SCENE));
				daeURI uri (*physicsScene);
				uri.set("", "", "", "", physicsScene->getId());
				//uri.setElement (physicsScene);
				//uri.resolveURI();
				ivs->setUrl (uri);
			}
		}
	}


	void AddPhysicsModelLibrary (daeDocument* document)
	{
/*
		int bodyEnumeration; 
		int jointEnumeration; 
		void* oldUserData;
		domCOLLADA *domRoot;
		domPhysics_model* model;
		domRigid_body* sentinel;
		domRigid_body* rigidBody;
		domRigid_constraint* rigidConstraint;
		const NewtonBody* body;
		const NewtonBody* newtonBody;
		const NewtonJoint* newtonJoint;
		const NewtonCollision* collision;
		domLibrary_physics_models* modelLibrary;
		domLibrary_physics_materials* materialLibrary;
		NewtonBodyList::dTreeNode* bodyNode;


		// save the wold user data and save the collada database object instead
		oldUserData = NewtonWorldGetUserData (m_world);
		NewtonWorldSetUserData(m_world, &m_bodyList);


		// create a sentinel body for joint attachment
		collision = NewtonCreateSphere (m_world, 0.0f, 0.0f, 0.0f, NULL);
		body = NewtonCreateBody (m_world, collision);
		NewtonReleaseCollision(m_world, collision);


		// iterate over all bodies in the scene 
		NewtonWorldForEachBodyDo (m_world, NewtonBodyList::BodyIterator);

		// create the image library
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot() );
		modelLibrary = daeSafeCast<domLibrary_physics_models>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_PHYSICS_MODELS));
		materialLibrary = daeSafeCast<domLibrary_physics_materials>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_PHYSICS_MATERIALS));

		// add the physics models to the scene.
		model = daeSafeCast<domPhysics_model>(modelLibrary->createAndPlace(COLLADA_ELEMENT_PHYSICS_MODEL));
		model->setId (D_PHYSICS_MODEL_NAME);

		sentinel = NULL;
		bodyEnumeration = 0;
		NewtonBodyList::Iterator bodyIter (m_bodyList);
		for (bodyIter.Begin(); bodyIter; bodyIter ++) {
			bodyNode = bodyIter.GetNode();
			newtonBody = bodyIter.GetKey();
			rigidBody = AddRigidBody (document, model, newtonBody, bodyEnumeration);
			if (!sentinel) {
				if (body == newtonBody) {
					sentinel = rigidBody;
				}
			}
			bodyEnumeration ++;
			bodyNode->GetInfo() = rigidBody;
		}

		// now add all constraints
		NewtonWorldSetUserData(m_world, &m_jointList);
		NewtonWorldForEachJointDo (m_world, NewtonJointList::JointIterator);


		jointEnumeration = 0;
		NewtonJointList::Iterator constIter (m_jointList);
		for (constIter.Begin(); constIter; constIter ++) {
			newtonJoint = constIter.GetNode()->GetKey();
			rigidConstraint = AddRigidConstraint (document, sentinel, model, newtonJoint, jointEnumeration);
			constIter.GetNode()->GetInfo() = rigidConstraint;
			jointEnumeration ++;
		}

		// restore the original user data
		NewtonWorldSetUserData(m_world, oldUserData);

		// destroy the semtinelBody
		NewtonDestroyBody(m_world, body);
*/
	}



	void SceneToCollada (const dModel* model)
	{
		daeInt error;
		daeDatabase* database;
		daeDocument *document;
		dTree<const char*, unsigned> textureFilter;

		//set the default IOPlugin and Database
		m_collada->setIOPlugin (NULL);
		m_collada->setDatabase (NULL);

		document = NULL;
		database = m_collada->getDatabase();
		error = database->insertDocument ("mainDoc", &document);

		_ASSERTE (error == DAE_OK);
		_ASSERTE (document);

		document = database->getDocument(daeUInt (0));

		// save graphical part
		AddAsset (document);
		AddCameraLibrary (document, model);
		AddImageLibrary (document, model, textureFilter);
		AddMaterialLibrary (document, model);
		AddGeometryLibrary (document, model);
		AddControllerLibrary(document, model);
		AddVisualSceneLibrary (document, model);

		// save physics part
//		AddPhysicsModelLibrary (document);
//		AddPhysicsSceneLibrary (document);

		// add the scene instance
		AddScene (document);
	}

	void SceneToCollada (const NewtonWorld* world)
	{
		_ASSERTE (0);
/*
		daeInt error;
		daeDatabase* database;
		daeDocument *document;
		dTree<const char*, unsigned> textureFilter;
		

		//set the default IOPlugin and Database
		m_collada->setIOPlugin (NULL);
		m_collada->setDatabase (NULL);

		document = NULL;
		database = m_collada->getDatabase();
		error = database->insertDocument ("mainDoc", &document);
		_ASSERTE (error == DAE_OK);
		_ASSERTE (document);
		document = database->getDocument(daeUInt (0));

		// save graphical part
		AddAsset (document);
//		AddCameraLibrary (document, model);

		for (NewtonBody* body = NewtonWorldGetFirstBody (world); body; body = NewtonWorldGetNextBody(world, body)) {
			dModel* model;

			model = context->GetModel(body);
			if (model) {
				AddImageLibrary (document, model, context, textureFilter);
				AddMaterialLibrary (document, model, context);
				AddGeometryLibrary (document, model, context);
				AddControllerLibrary(document, model, context);
				AddVisualSceneLibrary (document, model, context);
				context->DisposeModel(model);
			}
		}
		

		// save physics part
		AddPhysicsModelLibrary (document);
//		AddPhysicsSceneLibrary (document);

		// add the scene instance
		AddScene (document);
*/
	}




	// check image duplicates in the library
	domImage* FindImage (domLibrary_images* library, const char*name) const
	{
		unsigned i;
		domImage* image;

		if (name) {
			const domImage_Array& array = library->getImage_array();
			for (i = 0; i < array.getCount(); i ++) {
				image = array[i];
				if (!strcmp (name, image->getId())) {
					return image;
				}
			}
		}
		return NULL;
	}

	void LoadTextures(daeDocument *document, CollImageCache& imageCache)
	{
		domCOLLADA *domRoot;
	
		// scan the image library and save each texture
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		if (domRoot->getLibrary_images_array().getCount()) {
			int count; 
			domLibrary_images *imageLibrary;
			imageLibrary = domRoot->getLibrary_images_array()[0];
			_ASSERTE (imageLibrary);

			const domImage_Array &imageArray = imageLibrary->getImage_array();
			count = int (imageArray.getCount());
			for (int i = 0; i < count; i ++ ) {
				domImage* image;
				domImage::domInit_from* initFrom;
				image = imageArray[i];

				// add this unique texture to the texture list
				initFrom = image->getInit_from();
				if (initFrom->getValue().getOriginalURI()) {
					const char* name;
					xsAnyURI uri (initFrom->getValue());
					name = uri.getURI();
					imageCache.AddTexture(name);
				}
			}
		}
	}

	void LoadMaterials (daeDocument *document, const CollImageCache& imageCache, CollMaterialCache& materialCache)
	{
		int effectCount; 
		domCOLLADA *domRoot;
		domLibrary_images* imageLibrary;
		domLibrary_effects* effectLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

		imageLibrary = NULL;
		if (domRoot->getLibrary_images_array().getCount()) {
			imageLibrary = domRoot->getLibrary_images_array()[0];
			_ASSERTE (imageLibrary);
		}

		if (domRoot->getLibrary_effects_array().getCount()) {
			effectLibrary = domRoot->getLibrary_effects_array()[0];
			_ASSERTE (effectLibrary);

			const domEffect_Array &effectArray = effectLibrary->getEffect_array();
			effectCount = int (effectArray.getCount());

			// get all of the material effects
			for (int i = 0; i < effectCount; i ++) {
				domEffect* effect;
				domProfile_COMMON *profile; 

				effect = effectArray[i];
				_ASSERTE (effect);
				const domFx_profile_abstract_Array &profileArray = effect->getFx_profile_abstract_array();

				profile = daeSafeCast<domProfile_COMMON> (profileArray[0]);
				if (profile) {
					domCommon_color_or_texture_type* diffuse;
					domProfile_COMMON::domTechnique* teqchnique;

					// and the diffuse color from the type form the material type
					teqchnique = profile->getTechnique();
					_ASSERTE (teqchnique);

					diffuse = NULL;
					if (teqchnique->getLambert()) {
						domProfile_COMMON::domTechnique::domLambert* parameters;

						parameters = teqchnique->getLambert();
						_ASSERTE (parameters);
						diffuse = parameters->getDiffuse();
					} else if (teqchnique->getPhong()) {
						domProfile_COMMON::domTechnique::domPhong* parameters;
						parameters = teqchnique->getPhong();
						_ASSERTE (parameters);
						diffuse = parameters->getDiffuse();
					} else if (teqchnique->getBlinn()) {
						domProfile_COMMON::domTechnique::domBlinn* parameters;
						parameters = teqchnique->getBlinn();
						_ASSERTE (parameters);
						diffuse = parameters->getDiffuse();
					} else {
						_ASSERTE (0);
					}

					// since this is a prove of concept we only support the diffuse color texture feature
					// more complex material are unsupported at these time
					if (diffuse) {
						domCommon_color_or_texture_type::domTexture *texture;
						texture = diffuse->getTexture();
						if (texture) {
							const char* textName;
							textName = NULL;
							const char* textSample = texture->getTexture();
							domCommon_newparam_type_Array& paramaArray = profile->getNewparam_array();

							// find the texture sample 
							for (int j = 0; !textName && (j < int (paramaArray.getCount())); j ++) {
								const char* name = paramaArray[j]->getSid();
								if (!stricmp (name, textSample)) {
									domFx_sampler2D_common* sampler2d;
									domCommon_newparam_type_complexType* type;
									domFx_sampler2D_common_complexType::domSource* source;

									type = paramaArray[j];
									sampler2d = type->getSampler2D();
									source = sampler2d->getSource();
									const char* textSample2d = source->getValue();

									// find the image name for this diffuse effect
									for (int k = 0; !textName && (k < int (paramaArray.getCount())); k ++) {
										const char* name = paramaArray[k]->getSid(); 
										if (!stricmp (name, textSample2d)) {
											domFx_surface_common* surface;

											type = paramaArray[k];
											surface = type->getSurface();
											//domFx_surface_common::domInit_from_Array& initFromArray = surface->getInit_from_array();

											domFx_surface_init_commonRef surfInit = surface->getFx_surface_init_common();
											domFx_surface_init_from_common_Array initFromArray = surfInit->getInit_from_array();

											// finally get the name of the texture and bail out from form all loops
											for (int m = 0; !textName && (m < int (paramaArray.getCount())); m ++) {
												const xsIDREF& id = initFromArray[m]->getValue();
												textName = id.getID();
											}
										}
									}
								}
							}

							// if we have a texture name save this material as one of the material used by the geometries
							if (textName) {
								domImage* image;
								domImage::domInit_from* initfrom;

								image = FindImage (imageLibrary, textName);
								_ASSERTE (image);

								initfrom = image->getInit_from();
								_ASSERTE (initfrom);

								if (initfrom->getValue().getOriginalURI()) {
									CollMaterial collMatreial;
									xsAnyURI uri (initfrom->getValue());
									textName = uri.getURI();

									collMatreial.m_texture = imageCache.GetTexture (textName);
									materialCache.AddMaterial(collMatreial, effect->getId());
								}
							}

						} else {
							// no technique for this segment only color
							//_ASSERTE (0);
						}
					}
				}
			}
		}
	}

	SourceBuffer *FindBufferSouce (domMesh *colladaMesh, daeString type, SourceBuffer* sourceBuffers)const
	{
		domInputLocal* vertexInput;
		
		domVertices *vertices;

		vertexInput = NULL;
		vertices = colladaMesh->getVertices();
		domInputLocal_Array &inputArray = vertices->getInput_array();
		for (int i = 0; i < int (inputArray.getCount()); i ++) {
			domInputLocal* offset;
			offset = inputArray[i];
			if (!stricmp (offset->getSemantic(), type)) {
				vertexInput = offset;
				break;
			}
		}
		if (vertexInput) {
			int count;
			daeURI& uri = vertexInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i <  count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		domInputLocalOffset* triangleInput = NULL;
		domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
		if (trianglesArray.getCount()) {
			domTriangles* triangles = trianglesArray[0];
			domInputLocalOffset_Array &inputArray = triangles->getInput_array();
			for (int i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}
		if (triangleInput) {
			int count;
			daeURI& uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i < count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		domPolygons_Array &polygonArray = colladaMesh->getPolygons_array();
		if (polygonArray.getCount()) {
			domPolygons* polygons = polygonArray[0];
			domInputLocalOffset_Array &inputArray = polygons->getInput_array();
			for (int i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}
		if (triangleInput) {
			int count;
			daeURI& uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i < count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		domPolylist_Array &polyListArray = colladaMesh->getPolylist_array();
		if (polyListArray.getCount()) {
			domPolylist* polygons = polyListArray[0];
			domInputLocalOffset_Array &inputArray = polygons->getInput_array();
			for (int i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}
		if (triangleInput) {
			int count;
			daeURI& uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i < count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}

		return NULL;
	}


	SourceBuffer *FindBufferSouce (domConvex_mesh *colladaMesh, daeString type, SourceBuffer* sourceBuffers)const
	{
		domInputLocal* vertexInput;

		domVertices *vertices;

		vertexInput = NULL;
		vertices = colladaMesh->getVertices();
		domInputLocal_Array &inputArray = vertices->getInput_array();
		for (int i = 0; i < int (inputArray.getCount()); i ++) {
			domInputLocal* offset;
			offset = inputArray[i];
			if (!stricmp (offset->getSemantic(), type)) {
				vertexInput = offset;
				break;
			}
		}
		if (vertexInput) {
			int count;
			daeURI& uri = vertexInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i <  count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		domInputLocalOffset* triangleInput = NULL;
		domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
		if (trianglesArray.getCount()) {
			domTriangles* triangles = trianglesArray[0];
			domInputLocalOffset_Array &inputArray = triangles->getInput_array();
			for (int i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}
		if (triangleInput) {
			int count;
			daeURI& uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i < count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		domPolygons_Array &polygonArray = colladaMesh->getPolygons_array();
		if (polygonArray.getCount()) {
			domPolygons* polygons = polygonArray[0];
			domInputLocalOffset_Array &inputArray = polygons->getInput_array();
			for (int i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}
		if (triangleInput) {
			int count;
			daeURI& uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i < count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		domPolylist_Array &polyListArray = colladaMesh->getPolylist_array();
		if (polyListArray.getCount()) {
			domPolylist* polygons = polyListArray[0];
			domInputLocalOffset_Array &inputArray = polygons->getInput_array();
			for (int i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}
		if (triangleInput) {
			int count;
			daeURI& uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (int i = 0; i < count; i ++) {
				if (uri.getID()) {
					if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
						return &sourceBuffers[i];
					}
				}
			}
		}


		return NULL;
	}



	struct MaterialTringlePair
	{
		int m_faceIndex;
		CollMaterial* m_material;
	};

	static int SortMatrialTriangles (const void *A, const void *B) 
	{
		const MaterialTringlePair* vertexA = (MaterialTringlePair*) A;
		const MaterialTringlePair* vertexB = (MaterialTringlePair*) B;

		if (vertexA[0].m_material < vertexB[0].m_material) {
			return -1;
		} else if (vertexA[0].m_material > vertexB[0].m_material) {
			return 1;
		} else {
			return 0;
		}
	}

	dMesh* LoadMesh (daeDocument *document, ModifierVertexCache& modifierVertexMap, domGeometry *colladaGeometry,
					 const CollMaterialCache& materialCache, dLoaderContext* context)
	{
		dMesh* geometry;
		domCOLLADA *domRoot;
		domMesh *colladaMesh;
		SourceBuffer *uvSource;
		SourceBuffer *posSource;
		SourceBuffer *normalSrc;
		SourceBuffer sourceBuffers[6];
		domLibrary_effects* effectLibrary;
		domLibrary_materials *materialLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

		materialLibrary = NULL;
		if (domRoot->getLibrary_materials_array().getCount()) {
			materialLibrary = domRoot->getLibrary_materials_array()[0];
			_ASSERTE (materialLibrary);
		}

		effectLibrary = NULL;
		if (domRoot->getLibrary_effects_array().getCount()) {
			effectLibrary = domRoot->getLibrary_effects_array()[0];
			_ASSERTE (effectLibrary);
		}

		memset (sourceBuffers, 0, sizeof (sourceBuffers));

		colladaMesh = colladaGeometry->getMesh();
		_ASSERTE (colladaMesh);

		geometry = context->CreateMesh();
		sprintf (geometry->m_name, "%s", colladaGeometry->getId());

//		isSimpleVertex = 0;
//		domExtra_Array& extraArray = colladaGeometry->getExtra_array();
//		if (extraArray.getCount()) {
//			// if the geometry have extras, the search the Veter format option 
//			for (j = 0; !isSimpleVertex && (j < int (extraArray.getCount())); j ++) {
//				extra = extraArray[j];
//				const domTechnique_Array& techniqueArray = extra->getTechnique_array();
//				for (k = 0; !isSimpleVertex && (k < int (techniqueArray.getCount())); k ++) {
//					technique = techniqueArray[k];
//					if (!stricmp (technique->getProfile(), D_VERTEX_FORMAT)) {
//						// if have the vertex format option this mean this is a 
//						// previously newton saved file with and flat vertex format.
//						isSimpleVertex = 1;
//					}
//				}
//			}
//		}

		domSource_Array &sourceArray = colladaMesh->getSource_array();
		for (int j = 0; j <  int (sourceArray.getCount()); j ++) {
			domSource *source;
			domAccessor *accessor;
			domFloat_array *floatArray;
			domSource::domTechnique_common *technique;

			source = sourceArray[j];
			floatArray = source->getFloat_array();
			domListOfFloats &srcArray = floatArray->getValue();

			technique = source->getTechnique_common();
			_ASSERTE (technique);

			accessor = technique->getAccessor();
			_ASSERTE (accessor);
			sourceBuffers[j].m_id = source->getId();
			sourceBuffers[j].m_stride = int (accessor->getStride());
			sourceBuffers[j].m_count = int (floatArray->getCount());
			sourceBuffers[j].m_data = &srcArray[0];
		}

		posSource = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_POSITION, sourceBuffers);
		normalSrc = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_NORMAL, sourceBuffers);
		uvSource = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_TEXCOORD, sourceBuffers);
		_ASSERTE (posSource);

		struct MESH_POINT
		{
			dFloat m_vertex[3];
			dFloat m_normal[3];
			dFloat m_uv[2];
			dFloat m_originalIndex;
		};
		int vertexCount;

		vertexCount = 0;

		// count polygons in polygons list array
		domPolylist_Array &polygonListArray = colladaMesh->getPolylist_array();
		for (int j = 0; j < int (polygonListArray.getCount()); j ++) {
			domPolylist* polygon;
			domPolylist::domVcount* vcount;
			
			polygon = polygonListArray[j];
			vcount = polygon->getVcount();
			const domListOfUInts& vIndices = vcount->getValue();
			for (int k = 0; k < int (vIndices.getCount()); k ++) {
				int count = int (vIndices[k]);
				vertexCount += (count - 2) * 3;
			}
		}


		// count polygons
		domPolygons_Array &polygonArray = colladaMesh->getPolygons_array();
		for (int j = 0; j < int (polygonArray.getCount()); j ++) {
			int stride;
			int polygonCount;
			domPolygons* polygons;
			polygons = polygonArray[j];

			polygonCount = int (polygons->getCount());

			stride = int (polygons->getInput_array().getCount());
			domP_Array& indexArray = polygons->getP_array();

			_ASSERTE (int (indexArray.getCount()) == polygonCount);
			for (int k = 0; k < polygonCount; k ++) {
				int vCount;
				domPRef pIndices = indexArray[k];
				domListOfUInts& dataIndices = pIndices->getValue();
				vCount = int (dataIndices.getCount() / stride);
				vertexCount += (vCount - 2) * 3;
			}
		}

		// count triangles
		domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
		for (int j = 0; j < int (trianglesArray.getCount()); j ++) {
			domTriangles* triangles;
			triangles = trianglesArray[j];
			vertexCount += int (triangles->getCount()) * 3;
		}


		int totalTriangleCount = vertexCount / 3;
		MaterialTringlePair* materialTriangle = new MaterialTringlePair[totalTriangleCount + 1];

		int* indexList = new int[vertexCount];
		MESH_POINT* points = new MESH_POINT[vertexCount];
		vertexCount = 0;


		// load the polygonList array
		for (int j = 0; j < int (polygonListArray.getCount()); j ++) {
			int stride;
			domPolylist* polygon;

			polygon = polygonListArray[j];
			CollMaterial* matId = NULL;
			if (polygon->getMaterial()) {
				domMaterial* material = FindMaterial (materialLibrary, polygon->getMaterial());
				if (material) {
					domInstance_effect* instanceEffect;
					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						domEffect* effect;

						daeURI uri (instanceEffect->getUrl());
						effect = FindEffect (effectLibrary, uri.getID());
						if (effect) {
							matId = materialCache.GetMaterial (effect->getId());
						}
					}
				}
			}

			const domInputLocalOffset_Array& inputArray = polygon->getInput_array();
			stride = int (inputArray.getCount());
			int uvIndicesOffset = 0;
			int vertexIndicesOffset = 0;
			int normalIndicesOffset = 0;
			for (int k = 0; k < int (inputArray.getCount()); k ++) {
				domInputLocalOffset* offset;
				offset = inputArray[k];
				if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_VERTEX)) {
					vertexIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
					normalIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
					uvIndicesOffset = int (offset->getOffset());
				}
			}

			domPolylist::domVcount* vcount;
			vcount = polygon->getVcount();
			const domListOfUInts& vIndices = vcount->getValue();

			domPRef elemP = polygon->getP();
			const domListOfUInts& dataIndices = elemP->getValue();
			const domUint* indexP = &dataIndices[0];

			int pointsStride = int (posSource->m_stride);
			int pointcount = int (vIndices.getCount());
			for (int k = 0; k < pointcount; k ++) {
				int count = int (vIndices[k]);
				for (int i = 2; i < count; i ++) {
					int t;
					int m;

					t = vertexCount/3;
					materialTriangle[t].m_faceIndex = t;
					materialTriangle[t].m_material = matId;

					m = int (indexP[0 + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);

					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (indexP[0 + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}

					if (uvSource) {
						int uvStride;
						m = int (indexP[0 + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (indexP[(i - 1) * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (indexP[(i - 1) * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (indexP[(i - 1) * stride  + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (indexP[i * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (indexP[i * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (indexP[i * stride + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;
				}
				indexP += count * stride;
			}
		}

		// load the polygons
		for (int j = 0; j < int (polygonArray.getCount()); j ++) {
			int stride;
			int polygonCount;
			domPolygons* polygons;

			polygons = polygonArray[j];

			CollMaterial* matId = NULL;
			if (polygons->getMaterial()) {
				domMaterial* material = FindMaterial (materialLibrary, polygons->getMaterial());
				if (material) {
					domInstance_effect* instanceEffect;
					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						domEffect* effect;

						daeURI uri (instanceEffect->getUrl());
						effect = FindEffect (effectLibrary, uri.getID());
						if (effect) {
							matId = materialCache.GetMaterial (effect->getId());
						}
					}
				}
			}


			polygonCount = int (polygons->getCount());
			const domInputLocalOffset_Array& inputArray = polygons->getInput_array();

			stride = int (inputArray.getCount());
			int uvIndicesOffset = 0;
			int vertexIndicesOffset = 0;
			int normalIndicesOffset = 0;
			for (int k = 0; k < int (inputArray.getCount()); k ++) {
				domInputLocalOffset* offset;
				offset = inputArray[k];
				if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_VERTEX)) {
					vertexIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
					normalIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
					uvIndicesOffset = int (offset->getOffset());
				}
			}

			domP_Array& indexArray = polygons->getP_array();
			_ASSERTE (int (indexArray.getCount()) == polygonCount);
			for (int k = 0; k < polygonCount; k ++) {
				int vCount;
				int pointsStride;

				domPRef pIndices = indexArray[k];
				domListOfUInts& dataIndices = pIndices->getValue();

				vCount = int (dataIndices.getCount() / stride);
				pointsStride = int (posSource->m_stride);
				
				for (int i = 2; i < vCount; i ++) {
					int t;
					int m;

					t = vertexCount/3;
					materialTriangle[t].m_faceIndex = t;
					materialTriangle[t].m_material = matId;

					m = int (dataIndices[0 + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[0 + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[0 + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (dataIndices[(i - 1) * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[(i - 1) * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[(i - 1) * stride  + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (dataIndices[i * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[i * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[i * stride + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;
				}
			}
		}

		// load the triangles
		for (int j = 0; j < int (trianglesArray.getCount()); j ++) {
			int index;
			int triangleCount;
			int triangleInputStride;
			domP *p; 
			
			domTriangles* triangles;
			domUint* vertexIndices;
			domUint* normalIndices;
			domUint* uvIndices;

			triangles = trianglesArray[j];

			CollMaterial* matId = NULL;
			if (triangles->getMaterial()) {
				domMaterial* material = FindMaterial (materialLibrary, triangles->getMaterial());
				if (material) {
					domInstance_effect* instanceEffect;
					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						domEffect* effect;

						daeURI uri (instanceEffect->getUrl());
						effect = FindEffect (effectLibrary, uri.getID());
						if (effect) {
							matId = materialCache.GetMaterial (effect->getId());
						}
					}
				}
			}


			triangleCount = int (triangles->getCount());
			p = triangles->getP();
			domListOfUInts& indices = p->getValue();

			uvIndices = NULL;
			vertexIndices = NULL;
			normalIndices = NULL;

			const domInputLocalOffset_Array& inputArray = triangles->getInput_array();
			triangleInputStride = int (inputArray.getCount());
			for (int k = 0; k < int (inputArray.getCount()); k ++) {
				domInputLocalOffset* offset;
				offset = inputArray[k];
				if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_VERTEX)) {
					vertexIndices = &indices[int (offset->getOffset())];
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
					normalIndices = &indices[int (offset->getOffset())];
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
					uvIndices = &indices[int (offset->getOffset())];
				}
			}

			_ASSERTE (vertexIndices);
			index = vertexCount;
			vertexCount += triangleCount * 3;
			int indexCount = int (indices.getCount());
			for (int k = 0; k < indexCount; k += triangleInputStride) {
				int t;
				int m;
				int stride;

				t = index/3;
				materialTriangle[t].m_faceIndex = t;
				materialTriangle[t].m_material = matId;


				m = int (vertexIndices[k]);
				stride = int (posSource->m_stride);
				points[index].m_vertex[0] = dFloat (posSource->m_data[m * stride + 0]);
				points[index].m_vertex[1] = dFloat (posSource->m_data[m * stride + 1]);
				points[index].m_vertex[2] = dFloat (posSource->m_data[m * stride + 2]);
				points[index].m_originalIndex = dFloat(m);

				if (normalIndices && normalSrc) {
					m = int (normalIndices[k]);
					stride = int (posSource->m_stride);
					points[index].m_normal[0] = dFloat (normalSrc->m_data[m * stride + 0]);
					points[index].m_normal[1] = dFloat (normalSrc->m_data[m * stride + 1]);
					points[index].m_normal[2] = dFloat (normalSrc->m_data[m * stride + 2]);
				} else {
					points[index].m_normal[0] = 0.0f;
					points[index].m_normal[1] = 1.0f;
					points[index].m_normal[2] = 0.0f;
				}


				if (uvIndices && uvSource) {
					m = int (uvIndices[k]);
					stride = int (uvSource->m_stride);
					points[index].m_uv[0] = dFloat (uvSource->m_data[m * stride + 0]);
					points[index].m_uv[1] = dFloat (uvSource->m_data[m * stride + 1]);
				} else {
					points[index].m_uv[0] = 0.0f;
					points[index].m_uv[1] = 0.0f;
				}

				index ++;
			}
		}

		vertexCount = dModel::PackVertexArray (&points[0].m_vertex[0], 8, sizeof (MESH_POINT), vertexCount, indexList);
		// create space to store the vertices, normals and uvs
		geometry->AllocVertexData (vertexCount);
		VerterMap* vertexMap = new VerterMap[vertexCount + 1];

		vertexMap[vertexCount].m_vertexIndexInDMesh = 0x7fffffff;
		vertexMap[vertexCount].m_vertexIndexInColladaMesh = 0x7fffffff;
		modifierVertexMap.Insert(vertexMap, geometry);

		for (int j = 0; j < vertexCount; j ++) {
			dVector p(points[j].m_vertex[0], points[j].m_vertex[1], points[j].m_vertex[2], 0.0f);
			geometry->m_vertex[j * 3 + 0] = p.m_x;
			geometry->m_vertex[j * 3 + 1] = p.m_y;
			geometry->m_vertex[j * 3 + 2] = p.m_z;

			dVector n(points[j].m_normal[0], points[j].m_normal[1], points[j].m_normal[2], 0.0f);
			geometry->m_normal[j * 3 + 0] = n.m_x;
			geometry->m_normal[j * 3 + 1] = n.m_y;
			geometry->m_normal[j * 3 + 2] = n.m_z;

			geometry->m_uv[j * 2 + 0] = points[j].m_uv[0];
			geometry->m_uv[j * 2 + 1] = points[j].m_uv[1];

			vertexMap[j].m_vertexIndexInDMesh = j;
			vertexMap[j].m_vertexIndexInColladaMesh = int (points[j].m_originalIndex);
		}


		materialTriangle[totalTriangleCount].m_material = (CollMaterial*) 0x7fffffff;
		qsort(materialTriangle, totalTriangleCount, sizeof (MaterialTringlePair), SortMatrialTriangles);
		for (int j = 0; j < totalTriangleCount; ) {
			int i ;
			int triangleCount;
			CollMaterial* material;
			i = j;
			material = materialTriangle[i].m_material;
			while (material == materialTriangle[j].m_material) j ++;

			triangleCount = j - i;

			dSubMesh& subMesh = geometry->Append()->GetInfo() ;
			subMesh.AllocIndexData(triangleCount * 3);

			if (material) {
				strcpy (subMesh.m_textureName, material->m_texture.m_textureName);
			}
			for (int k = 0; k < triangleCount; k ++) {
				int index;
				index = materialTriangle[k + i].m_faceIndex;
				subMesh.m_indexes[k * 3 + 0] = indexList[index * 3 + 0];
				subMesh.m_indexes[k * 3 + 1] = indexList[index * 3 + 1];
				subMesh.m_indexes[k * 3 + 2] = indexList[index * 3 + 2];
			}
		}
		
		delete[] indexList;
		delete[] points;
		delete[] materialTriangle;

		return geometry;

	}


	dMesh* LoadExplicitConvexMesh (daeDocument *document, ModifierVertexCache& modifierVertexMap, domGeometry *colladaGeometry,
					 const CollMaterialCache& materialCache, dLoaderContext* context)
	{
		_ASSERTE (0);
		return NULL;
/*

		dMesh* geometry;
		domCOLLADA *domRoot;
		domConvex_mesh *colladaMesh;
		SourceBuffer *uvSource;
		SourceBuffer *posSource;
		SourceBuffer *normalSrc;
		SourceBuffer sourceBuffers[6];
		domLibrary_effects* effectLibrary;
		domLibrary_materials *materialLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

		materialLibrary = NULL;
		if (domRoot->getLibrary_materials_array().getCount()) {
			materialLibrary = domRoot->getLibrary_materials_array()[0];
			_ASSERTE (materialLibrary);
		}

		effectLibrary = NULL;
		if (domRoot->getLibrary_effects_array().getCount()) {
			effectLibrary = domRoot->getLibrary_effects_array()[0];
			_ASSERTE (effectLibrary);
		}

		memset (sourceBuffers, 0, sizeof (sourceBuffers));

		colladaMesh = colladaGeometry->getConvex_mesh();
		_ASSERTE (colladaMesh);

		geometry = context->CreateMesh();
		sprintf (geometry->m_name, "%s", colladaGeometry->getId());

//		isSimpleVertex = 0;
//		domExtra_Array& extraArray = colladaGeometry->getExtra_array();
//		if (extraArray.getCount()) {
//			// if the geometry have extras, the search the Veter format option 
//			for (j = 0; !isSimpleVertex && (j < int (extraArray.getCount())); j ++) {
//				extra = extraArray[j];
//				const domTechnique_Array& techniqueArray = extra->getTechnique_array();
//				for (k = 0; !isSimpleVertex && (k < int (techniqueArray.getCount())); k ++) {
//					technique = techniqueArray[k];
//					if (!stricmp (technique->getProfile(), D_VERTEX_FORMAT)) {
//						// if have the vertex format option this mean this is a 
//						// previously newton saved file with and flat vertex format.
//						isSimpleVertex = 1;
//					}
//				}
//			}
//		}

		domSource_Array &sourceArray = colladaMesh->getSource_array();
		for (int j = 0; j <  int (sourceArray.getCount()); j ++) {
			domSource *source;
			domAccessor *accessor;
			domFloat_array *floatArray;
			domSource::domTechnique_common *technique;

			source = sourceArray[j];
			floatArray = source->getFloat_array();
			domListOfFloats &srcArray = floatArray->getValue();

			technique = source->getTechnique_common();
			_ASSERTE (technique);

			accessor = technique->getAccessor();
			_ASSERTE (accessor);
			sourceBuffers[j].m_id = source->getId();
			sourceBuffers[j].m_stride = int (accessor->getStride());
			sourceBuffers[j].m_count = int (floatArray->getCount());
			sourceBuffers[j].m_data = &srcArray[0];
		}

		posSource = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_POSITION, sourceBuffers);
		normalSrc = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_NORMAL, sourceBuffers);
		uvSource = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_TEXCOORD, sourceBuffers);
		_ASSERTE (posSource);

		struct MESH_POINT
		{
			dFloat m_vertex[3];
			dFloat m_normal[3];
			dFloat m_uv[2];
			dFloat m_originalIndex;
		};
		int vertexCount;

		vertexCount = 0;

		// count polygons in polygons list array
		domPolylist_Array &polygonListArray = colladaMesh->getPolylist_array();
		for (int j = 0; j < int (polygonListArray.getCount()); j ++) {
			domPolylist* polygon;
			domPolylist::domVcount* vcount;
			
			polygon = polygonListArray[j];
			vcount = polygon->getVcount();
			const domListOfUInts& vIndices = vcount->getValue();
			for (int k = 0; k < int (vIndices.getCount()); k ++) {
				int count = int (vIndices[k]);
				vertexCount += (count - 2) * 3;
			}
		}


		// count polygons
		domPolygons_Array &polygonArray = colladaMesh->getPolygons_array();
		for (int j = 0; j < int (polygonArray.getCount()); j ++) {
			int stride;
			int polygonCount;
			domPolygons* polygons;
			polygons = polygonArray[j];

			polygonCount = int (polygons->getCount());

			stride = int (polygons->getInput_array().getCount());
			domP_Array& indexArray = polygons->getP_array();

			_ASSERTE (int (indexArray.getCount()) == polygonCount);
			for (int k = 0; k < polygonCount; k ++) {
				int vCount;
				domPRef pIndices = indexArray[k];
				domListOfUInts& dataIndices = pIndices->getValue();
				vCount = int (dataIndices.getCount() / stride);
				vertexCount += (vCount - 2) * 3;
			}
		}

		// count triangles
		domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
		for (int j = 0; j < int (trianglesArray.getCount()); j ++) {
			domTriangles* triangles;
			triangles = trianglesArray[j];
			vertexCount += int (triangles->getCount()) * 3;
		}


		int totalTriangleCount = vertexCount / 3;
		MaterialTringlePair* materialTriangle = new MaterialTringlePair[totalTriangleCount + 1];

		int* indexList = new int[vertexCount];
		MESH_POINT* points = new MESH_POINT[vertexCount];
		vertexCount = 0;


		// load the polygonList array
		for (int j = 0; j < int (polygonListArray.getCount()); j ++) {
			int stride;
			domPolylist* polygon;

			polygon = polygonListArray[j];
			CollMaterial* matId = NULL;
			if (polygon->getMaterial()) {
				domMaterial* material = FindMaterial (materialLibrary, polygon->getMaterial());
				if (material) {
					domInstance_effect* instanceEffect;
					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						daeURI uri;
						domEffect* effect;

						uri = instanceEffect->getUrl();
						effect = FindEffect (effectLibrary, uri.getID());
						if (effect) {
							matId = materialCache.GetMaterial (effect->getId());
						}
					}
				}
			}

			const domInputLocalOffset_Array& inputArray = polygon->getInput_array();

			stride = int (inputArray.getCount());
			int uvIndicesOffset = 0;
			int vertexIndicesOffset = 0;
			int normalIndicesOffset = 0;
			for (int k = 0; k < int (inputArray.getCount()); k ++) {
				domInputLocalOffset* offset;
				offset = inputArray[k];
				if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_VERTEX)) {
					vertexIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
					normalIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
					uvIndicesOffset = int (offset->getOffset());
				}
			}

			domPolylist::domVcount* vcount;
			vcount = polygon->getVcount();
			const domListOfUInts& vIndices = vcount->getValue();

			domPRef elemP = polygon->getP();
			const domListOfUInts& dataIndices = elemP->getValue();

			int pointsStride = int (posSource->m_stride);

			int pointcount = int (vIndices.getCount());
			if (pointcount > 1) pointcount = 1;
			for (int k = 0; k < pointcount; k ++) {
				int count = int (vIndices[k]);
				for (int i = 2; i < count; i ++) {
					int t;
					int m;

					t = vertexCount/3;
					materialTriangle[t].m_faceIndex = t;
					materialTriangle[t].m_material = matId;

					m = int (dataIndices[0 + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[0 + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[0 + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (dataIndices[(i - 1) * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[(i - 1) * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[(i - 1) * stride  + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (dataIndices[i * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[i * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[i * stride + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;
				}
			}
		}



		// load the polygons
		for (int j = 0; j < int (polygonArray.getCount()); j ++) {
			int stride;
			int polygonCount;
			domPolygons* polygons;

			polygons = polygonArray[j];

			CollMaterial* matId = NULL;
			if (polygons->getMaterial()) {
				domMaterial* material = FindMaterial (materialLibrary, polygons->getMaterial());
				if (material) {
					domInstance_effect* instanceEffect;
					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						daeURI uri;
						domEffect* effect;

						uri = instanceEffect->getUrl();
						effect = FindEffect (effectLibrary, uri.getID());
						if (effect) {
							matId = materialCache.GetMaterial (effect->getId());
						}
					}
				}
			}


			polygonCount = int (polygons->getCount());
			const domInputLocalOffset_Array& inputArray = polygons->getInput_array();

			stride = int (inputArray.getCount());
			int uvIndicesOffset = 0;
			int vertexIndicesOffset = 0;
			int normalIndicesOffset = 0;
			for (int k = 0; k < int (inputArray.getCount()); k ++) {
				domInputLocalOffset* offset;
				offset = inputArray[k];
				if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_VERTEX)) {
					vertexIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
					normalIndicesOffset = int (offset->getOffset());
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
					uvIndicesOffset = int (offset->getOffset());
				}
			}

			domP_Array& indexArray = polygons->getP_array();
			_ASSERTE (int (indexArray.getCount()) == polygonCount);
			for (int k = 0; k < polygonCount; k ++) {
				int vCount;
				int pointsStride;

				domPRef pIndices = indexArray[k];
				domListOfUInts& dataIndices = pIndices->getValue();

				vCount = int (dataIndices.getCount() / stride);
				pointsStride = int (posSource->m_stride);
				
				for (int i = 2; i < vCount; i ++) {
					int t;
					int m;

					t = vertexCount/3;
					materialTriangle[t].m_faceIndex = t;
					materialTriangle[t].m_material = matId;

					m = int (dataIndices[0 + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[0 + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[0 + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (dataIndices[(i - 1) * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[(i - 1) * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[(i - 1) * stride  + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;


					m = int (dataIndices[i * stride + vertexIndicesOffset]);
					points[vertexCount].m_vertex[0] = dFloat (posSource->m_data[m * pointsStride + 0]);
					points[vertexCount].m_vertex[1] = dFloat (posSource->m_data[m * pointsStride + 1]);
					points[vertexCount].m_vertex[2] = dFloat (posSource->m_data[m * pointsStride + 2]);
					points[vertexCount].m_originalIndex = dFloat(m);
					if (normalSrc) {
						int normalStride;
						normalStride = int (normalSrc->m_stride);
						m = int (dataIndices[i * stride + normalIndicesOffset]);
						normalStride = int (normalSrc->m_stride);
						points[vertexCount].m_normal[0] = dFloat (normalSrc->m_data[m * normalStride + 0]);
						points[vertexCount].m_normal[1] = dFloat (normalSrc->m_data[m * normalStride + 1]);
						points[vertexCount].m_normal[2] = dFloat (normalSrc->m_data[m * normalStride + 2]);
					} else {
						points[vertexCount].m_normal[0] = 0.0f;
						points[vertexCount].m_normal[1] = 1.0f;
						points[vertexCount].m_normal[2] = 0.0f;
					}
					if (uvSource) {
						int uvStride;
						m = int (dataIndices[i * stride + uvIndicesOffset]);
						uvStride = int (uvSource->m_stride);
						points[vertexCount].m_uv[0] = dFloat (uvSource->m_data[m * uvStride + 0]);
						points[vertexCount].m_uv[1] = dFloat (uvSource->m_data[m * uvStride + 1]);
					} else {
						points[vertexCount].m_uv[0] = 0.0f;
						points[vertexCount].m_uv[1] = 0.0f;
					}
					vertexCount ++;
				}
			}
		}

		// load the triangles
		for (int j = 0; j < int (trianglesArray.getCount()); j ++) {
			int index;
			int triangleCount;
			int triangleInputStride;
			domP *p; 
			
			domTriangles* triangles;
			domUint* vertexIndices;
			domUint* normalIndices;
			domUint* uvIndices;

			triangles = trianglesArray[j];

			CollMaterial* matId = NULL;
			if (triangles->getMaterial()) {
				domMaterial* material = FindMaterial (materialLibrary, triangles->getMaterial());
				if (material) {
					domInstance_effect* instanceEffect;
					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						daeURI uri;
						domEffect* effect;

						uri = instanceEffect->getUrl();
						effect = FindEffect (effectLibrary, uri.getID());
						if (effect) {
							matId = materialCache.GetMaterial (effect->getId());
						}
					}
				}
			}


			triangleCount = int (triangles->getCount());
			p = triangles->getP();
			domListOfUInts& indices = p->getValue();

			uvIndices = NULL;
			vertexIndices = NULL;
			normalIndices = NULL;

			const domInputLocalOffset_Array& inputArray = triangles->getInput_array();
			triangleInputStride = int (inputArray.getCount());
			for (int k = 0; k < int (inputArray.getCount()); k ++) {
				domInputLocalOffset* offset;
				offset = inputArray[k];
				if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_VERTEX)) {
					vertexIndices = &indices[int (offset->getOffset())];
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
					normalIndices = &indices[int (offset->getOffset())];
				} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
					uvIndices = &indices[int (offset->getOffset())];
				}
			}

			_ASSERTE (vertexIndices);
			index = vertexCount;
			vertexCount += triangleCount * 3;
			int indexCount = int (indices.getCount());
			for (int k = 0; k < indexCount; k += triangleInputStride) {
				int t;
				int m;
				int stride;

				t = index/3;
				materialTriangle[t].m_faceIndex = t;
				materialTriangle[t].m_material = matId;

				m = int (vertexIndices[k]);
				stride = int (posSource->m_stride);
				points[index].m_vertex[0] = dFloat (posSource->m_data[m * stride + 0]);
				points[index].m_vertex[1] = dFloat (posSource->m_data[m * stride + 1]);
				points[index].m_vertex[2] = dFloat (posSource->m_data[m * stride + 2]);
				points[index].m_originalIndex = dFloat(m);
				if (normalIndices && normalSrc) {
					m = int (normalIndices[k]);
					stride = int (posSource->m_stride);
					points[index].m_normal[0] = dFloat (normalSrc->m_data[m * stride + 0]);
					points[index].m_normal[1] = dFloat (normalSrc->m_data[m * stride + 1]);
					points[index].m_normal[2] = dFloat (normalSrc->m_data[m * stride + 2]);
				} else {
					points[index].m_normal[0] = 0.0f;
					points[index].m_normal[1] = 1.0f;
					points[index].m_normal[2] = 0.0f;
				}
				if (uvIndices && uvSource) {
					m = int (uvIndices[k]);
					stride = int (uvSource->m_stride);
					points[index].m_uv[0] = dFloat (uvSource->m_data[m * stride + 0]);
					points[index].m_uv[1] = dFloat (uvSource->m_data[m * stride + 1]);
				} else {
					points[index].m_uv[0] = 0.0f;
					points[index].m_uv[1] = 0.0f;
				}

				index ++;
			}
		}

		vertexCount = dModel::PackVertexArray (&points[0].m_vertex[0], 8, sizeof (MESH_POINT), vertexCount, indexList);
		// create space to store the vertices, normals and uvs
		geometry->AllocVertexData (vertexCount);
		VerterMap* vertexMap = new VerterMap[vertexCount + 1];

		vertexMap[vertexCount].m_vertexIndexInDMesh = 0x7fffffff;
		vertexMap[vertexCount].m_vertexIndexInColladaMesh = 0x7fffffff;
		modifierVertexMap.Insert(vertexMap, geometry);

		for (int j = 0; j < vertexCount; j ++) {
			dVector p(points[j].m_vertex[0], points[j].m_vertex[1], points[j].m_vertex[2], 0.0f);
			geometry->m_vertex[j * 3 + 0] = p.m_x;
			geometry->m_vertex[j * 3 + 1] = p.m_y;
			geometry->m_vertex[j * 3 + 2] = p.m_z;

			dVector n(points[j].m_normal[0], points[j].m_normal[1], points[j].m_normal[2], 0.0f);
			geometry->m_normal[j * 3 + 0] = n.m_x;
			geometry->m_normal[j * 3 + 1] = n.m_y;
			geometry->m_normal[j * 3 + 2] = n.m_z;

			geometry->m_uv[j * 2 + 0] = points[j].m_uv[0];
			geometry->m_uv[j * 2 + 1] = points[j].m_uv[1];

			vertexMap[j].m_vertexIndexInDMesh = j;
			vertexMap[j].m_vertexIndexInColladaMesh = int (points[j].m_originalIndex);
		}


		materialTriangle[totalTriangleCount].m_material = (CollMaterial*) 0x7fffffff;
		qsort(materialTriangle, totalTriangleCount, sizeof (MaterialTringlePair), SortMatrialTriangles);
		for (int j = 0; j < totalTriangleCount; ) {
			int i ;
			int triangleCount;
			CollMaterial* material;
			i = j;
			material = materialTriangle[i].m_material;
			while (material == materialTriangle[j].m_material) j ++;

			triangleCount = j - i;

			dSubMesh& subMesh = geometry->Append()->GetInfo() ;
			subMesh.AllocIndexData(triangleCount * 3);

			if (material) {
				strcpy (subMesh.m_textureName, material->m_texture.m_textureName);
			}
			for (int k = 0; k < triangleCount; k ++) {
				int index;
				index = materialTriangle[k + i].m_faceIndex;
				subMesh.m_indexes[k * 3 + 0] = indexList[index * 3 + 0];
				subMesh.m_indexes[k * 3 + 1] = indexList[index * 3 + 1];
				subMesh.m_indexes[k * 3 + 2] = indexList[index * 3 + 2];
			}
		}
		
		delete[] indexList;
		delete[] points;
		delete[] materialTriangle;

		return geometry;
*/
	}


//	dGeometry* LoadConvexMesh (daeDocument *document, domGeometry *colladaGeometry)
	dMesh* LoadConvexMesh (daeDocument *document, ModifierVertexCache& modifierVertexMap, domGeometry *colladaGeometry, 
						    const CollMaterialCache& materialCache, dLoaderContext* context)
	{
//		const char* meshName;
//		dGeometry* sourceGeo;
		dMesh* geometry;
		domConvex_mesh* convexMesh;

		convexMesh = colladaGeometry->getConvex_mesh();
		_ASSERTE (convexMesh);

		geometry = NULL;
		const xsAnyURI& uri = convexMesh->getConvex_hull_of();
		const char* meshName = uri.getOriginalURI();
		if (meshName) {
			_ASSERTE (0);
/*
			sourceGeo = m_geometryCache.FindByName (meshName);
			_ASSERTE (sourceGeo);
			geometry = dGeometry::MakeConvexHull (m_world, sourceGeo);
			sprintf (geometry->m_name, "%s", colladaGeometry->getId());
*/
		} else {
//			sourceGeo = LoadExplicitConvex (document, colladaGeometry);
//			geometry = dGeometry::MakeConvexHull (m_world, sourceGeo);
//			sourceGeo->Release();

			geometry = LoadExplicitConvexMesh (document, modifierVertexMap, colladaGeometry, materialCache, context);
		}
		return geometry;
	}



	void LoadGeometries (daeDocument *document, dModel* model, ModifierVertexCache& modifierVertexMap, CollGeoCache& meshCache, 
						 const CollMaterialCache& materialCache, dLoaderContext* context)
	{
		int count; 
		domCOLLADA *domRoot;
		
		domLibrary_geometries *geometryLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		geometryLibrary = domRoot->getLibrary_geometries_array()[0];
		_ASSERTE (geometryLibrary);

		const domGeometry_Array &geomteryArray = geometryLibrary->getGeometry_array();
		count = int (geomteryArray.getCount());
		for (int i = 0; i < count; i ++ ) {
			dMesh* geometry;
			domGeometry *colladaGeometry;

			colladaGeometry = geomteryArray[i];
			_ASSERTE (colladaGeometry);

			geometry = NULL;
			if (colladaGeometry->getMesh()) {
				geometry = LoadMesh (document, modifierVertexMap, colladaGeometry, materialCache, context);
			} else if (colladaGeometry->getConvex_mesh()){
				geometry = LoadConvexMesh (document, modifierVertexMap, colladaGeometry, materialCache, context);
			} else {
				_ASSERTE (0);
//				geometry = new dGeometry;
			}

			if (geometry) {
				model->AddMesh(geometry);
				meshCache.Insert(geometry, colladaGeometry);
			}
		}
	}


	void LoadNodeData (dModel* model, dBone* bone, domNode* colNode, CollGeoCache& meshCache)	
	{
		if (colNode->getSid()) {
			bone->SetNameID(colNode->getSid());
		} else if (colNode->getName()) {
			bone->SetNameID(colNode->getName());
		} else {
			bone->SetNameID(colNode->getId());
		}
		

		dMatrix matrix (GetMatrix (colNode));
		bone->SetMatrix(matrix);

		if (colNode->getType() == NODETYPE_JOINT) {
			bone->SetType(dBone::m_bone);
		}

/*
		const domInstance_controller_Array& controllerArray = colNode->getInstance_controller_array();
		if (controllerArray.getCount()) {
			dMesh* mesh;
			domSkin* skin;
			const daeElement* element;
			domController* controller;
			domGeometry* collGeometry;
			domInstance_controller* instance;

			instance = controllerArray[0];
			const daeURI& uri = instance->getUrl();
			element = uri.getElement();
			controller = (domController*) element;
			skin = controller->getSkin();

			const daeURI& uriMesh = skin->getSource();
			element = uriMesh.getElement();
			collGeometry = (domGeometry*) element;
			_ASSERTE (meshCache.Find(collGeometry));

			mesh = meshCache.Find(collGeometry)->GetInfo();
			if (!mesh->m_hasBone) {
				mesh->m_hasBone = 1;
				mesh->m_boneID = bone->m_boneID;
				for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
					if (node->GetInfo().m_mesh == mesh) {
						node->GetInfo().m_boneID = bone->m_boneID;
					}
				}
			} else {
				model->AddMesh(mesh);
				model->m_meshList.GetLast()->GetInfo().m_boneID = bone->m_boneID;
			}
		}
*/
		const domInstance_geometry_Array& gemeortyArray = colNode->getInstance_geometry_array();
		if (gemeortyArray.getCount()) {
			dMesh* mesh;
			const daeElement* element;
			domGeometry* collGeometry;
			domInstance_geometry* instance;

			instance = gemeortyArray[0];

			const daeURI& uri = instance->getUrl();
			element = uri.getElement();
			collGeometry = (domGeometry*) element;
			_ASSERTE (meshCache.Find(collGeometry));

			mesh = meshCache.Find(collGeometry)->GetInfo();

			if (!mesh->m_hasBone) {
				mesh->m_hasBone = 1;
				mesh->m_boneID = bone->m_boneID;
				for (dList<dMeshInstance>::dListNode* node = model->m_meshList.GetFirst(); node; node = node->GetNext()) { 
					if (node->GetInfo().m_mesh == mesh) {
						node->GetInfo().m_boneID = bone->m_boneID;
					}
				}
			} else {
				model->AddMesh(mesh);
				model->m_meshList.GetLast()->GetInfo().m_boneID = bone->m_boneID;
			}
		}
	}


	void LoadScene (dModel* model, domVisual_scene* visualScene, ColladaNodeTodSceneNodeMap& nodeMap, CollGeoCache& meshCache)
	{
		int stack;
		int boneId;
		dBone* rootBone;
		dBone* parentBones[1024];
		domNode* nodePool[1024];
		
		rootBone = new dBone (NULL);

		stack = 0;
		const domNode_Array& nodeArray = visualScene->getNode_array();
		for (int i = 0; i < int (nodeArray.getCount()); i ++) {
			parentBones[stack] = rootBone;
			nodePool[stack] = nodeArray[i];
			stack ++;
		}

		boneId = 0;
		while (stack) {
			dBone* node;
			dBone* parent;
			domNode* colNode;

			stack --;
			colNode = nodePool[stack];
			parent = parentBones[stack]; 

			node = new dBone(parent);
			nodeMap.Insert(node, colNode);

			node->m_boneID = boneId;
			boneId ++;

			LoadNodeData (model, node, colNode, meshCache);

//			dMatrix boneMatrix (invMatrix * node->GetMatrix() * matrix);
//			node->SetMatrix (boneMatrix);
//			if (mesh == m_cameraNode) {
//				mesh->SetNameID(CAMERA_NAME);
//			}

			domInstance_node_Array& instansceNodeArray = colNode->getInstance_node_array();
			for (int i = 0; i < int (instansceNodeArray.getCount()); i ++) {
				daeElement* element;
				domNode* childNode;
				domInstance_node* instansceNode = instansceNodeArray[0];
				daeURI uri (instansceNode->getUrl());
//				uri.resolveURI();
				element = uri.getElement ();
				childNode = (domNode *)element;

				parentBones[stack] = node;
				nodePool[stack] = childNode;
				stack ++;
			}


			const domNode_Array &nodeArray = colNode->getNode_array();
			for (int i = 0; i < int (nodeArray.getCount()); i ++) {
				parentBones[stack] = node;
				nodePool[stack] = nodeArray[i];
				stack ++;
			}
		}


		ModelComponent<dList<dBone*> >& boneList = model->m_skeleton.Append()->GetInfo();
		while (rootBone->GetChild()) {
			dBone *bone;
			bone = rootBone->GetChild();
			bone->Detach();
			boneList.m_data.Append (bone);
		}
//		model->BindMeshToBonesByName();
		delete rootBone;
	}


	void LoadVisualScene(daeDocument* document, dModel* model, ColladaNodeTodSceneNodeMap& nodeMap, CollGeoCache& meshCache)
	{
		domCOLLADA *domRoot;
		domInstanceWithExtra *ivs;
		domVisual_scene* visualScene;
		domCOLLADA::domScene *sceneInstance;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		_ASSERTE (domRoot );

		sceneInstance = domRoot->getScene();
		_ASSERTE (sceneInstance);

		visualScene = NULL;
		ivs = sceneInstance->getInstance_visual_scene();
		if (ivs) {
			daeURI uri (ivs->getUrl());
			const char* sceneName = uri.getID(); 
			const domLibrary_visual_scenes_Array &libraryArray = domRoot->getLibrary_visual_scenes_array ();

			for (int i = 0;  (!visualScene) && (i < int (libraryArray.getCount())); i ++) {
				const domVisual_scene_Array& visualSceneArray = libraryArray[i]->getVisual_scene_array();
				for (int j = 0; j < int (visualSceneArray.getCount()); j ++) {
					domVisual_scene* tmp;			
					tmp = visualSceneArray [j];
					if (!stricmp (sceneName, tmp->getId())) {
						visualScene = tmp;
						break;
					}
				}
			}
		} else {
			// a scene may not have visual representation but must have nodes 
			// load any scene from the scene library as a scene

			const domLibrary_visual_scenes_Array &libraryArray = domRoot->getLibrary_visual_scenes_array ();
			_ASSERTE (libraryArray.getCount());
			const domVisual_scene_Array& visualSceneArray = libraryArray[0]->getVisual_scene_array();
			_ASSERTE (visualSceneArray.getCount());
			visualScene = visualSceneArray [0];
		}

		if (visualScene) {
//			m_cameraNode = (CameraNode*) m_rootNode->Find(CAMERA_NAME);

			LoadScene (model, visualScene, nodeMap, meshCache);
		}
	}

	static int SortVertexMap (const void *A, const void *B) 
	{
		const VerterMap* vertexA = (VerterMap*) A;
		const VerterMap* vertexB = (VerterMap*) B;

		if (vertexA[0].m_vertexIndexInColladaMesh < vertexB[0].m_vertexIndexInColladaMesh) {
			return -1;
		} else if (vertexA[0].m_vertexIndexInColladaMesh > vertexB[0].m_vertexIndexInColladaMesh) {
			return 1;
		} else {
			return 0;
		}
	}


	dSkinModifier* LoadSkinController (daeDocument* document, domSkin* skin, dMesh* mesh, const dModel* model, ModifierVertexCache& modifierVertexMap)
	{
		_ASSERTE (0);
		return NULL;
/*
		daeURI uri;
		dSkinModifier* skinModifier;
		dSkinModifier::dBoneVertexWeightData* skinData;
		const daeElement* element;
		domSource *boneSource;
		domSource *weightSource;
		domSkin::domVertex_weights::domV *v;
		domSkin::domVertex_weights::domVcount *vCount;
		int BoneIndices[1024];


		skinModifier = new dSkinModifier (mesh);

//		mesh->SetModifier (skinModifier);

		domSkin::domVertex_weights *vertexWeights = skin->getVertex_weights();
		domInputLocalOffset* weights = vertexWeights->getInput_array()[0];
		domInputLocalOffset* jointInputs = vertexWeights->getInput_array()[1];
		if (strcmp (jointInputs->getSemantic(), COMMON_PROFILE_INPUT_JOINT)) {
			domInputLocalOffset *tmp = weights;
			weights = jointInputs;
			jointInputs = tmp;
		}
		_ASSERTE (!strcmp (weights->getSemantic(), COMMON_PROFILE_INPUT_WEIGHT));
		_ASSERTE (!strcmp (jointInputs->getSemantic(), COMMON_PROFILE_INPUT_JOINT));


		domSkin::domBind_shape_matrix* bindMatrix = skin->getBind_shape_matrix();
		if (bindMatrix) {
			domFloat4x4& values = bindMatrix->getValue();
			dMatrix mat;

			for (int i = 0; i < 4; i ++) {
				for (int j = 0; j < 4; j ++) {
					mat[j][i] = dFloat (values[i * 4 + j]);
				}
			}
			mat.TransformTriplex (mesh->m_vertex, 3 * sizeof (dFloat), mesh->m_vertex, 3 * sizeof (dFloat), mesh->m_vertexCount);
		}
		

		uri = jointInputs->getSource();
		uri.resolveURI();
		element = uri.getElement ();
		boneSource = (domSource *)element;
		domName_array *boneNames = boneSource->getName_array();
		domListOfNames &boneSrcArray = boneNames->getValue();
		for (int i = 0; i < int (boneSrcArray.getCount()); i ++) {
			dBone* bone;
			bone = model->FindBone(boneSrcArray[i]);
			BoneIndices[i] = bone->GetBoneID();
		}
		
		uri = weights->getSource();
		uri.resolveURI();
		element = uri.getElement ();
		weightSource = (domSource *)element;
		domFloat_array *weightValue = weightSource->getFloat_array();
		domListOfFloats &weightValueArray = weightValue->getValue();

		skinData = new dSkinModifier::dBoneVertexWeightData[mesh->m_vertexCount * 4];

		v = vertexWeights->getV();
		vCount = vertexWeights->getVcount();

		domListOfInts &vIndices = v->getValue();
		domListOfUInts &vCountIndices = vCount->getValue();
		int jointOffset = int (jointInputs->getOffset());
		int weightOffset = int (weights->getOffset());

		VerterMap* vertexMap = modifierVertexMap.Find(mesh)->GetInfo();
		qsort(vertexMap, mesh->m_vertexCount, sizeof (VerterMap), SortVertexMap);


		int vertexIndex;
		int weightsCount;
		int weightStartIndex;

		weightsCount = 0;
		vertexIndex = 0;
		weightStartIndex = 0;
		for (int collVertexIndex = 0; collVertexIndex < (int) vCountIndices.getCount(); collVertexIndex ++) {
			int count = int (vCountIndices[collVertexIndex]);
			for (int j = 0; j < count; j ++) {
				int boneIndex = int (vIndices[weightStartIndex * 2 + jointOffset]);
				int weightIndex = int (vIndices[weightStartIndex * 2 + weightOffset]);
				int boneId = BoneIndices[boneIndex];
				dFloat weightValue = dFloat (weightValueArray[weightIndex]);
				weightStartIndex ++;

				if (weightValue > 1.0e-3f) {
					for (int k = vertexIndex; vertexMap[k].m_vertexIndexInColladaMesh <= collVertexIndex; k ++) {
						skinData[weightsCount].m_boneId = boneId;
						skinData[weightsCount].m_weight = weightValue;
						skinData[weightsCount].m_vertexIndex = vertexMap[k].m_vertexIndexInDMesh;
						weightsCount ++;
					}
				}
			}

			do {
				vertexIndex ++;
			} while (vertexMap[vertexIndex].m_vertexIndexInColladaMesh == collVertexIndex);
		}


		skinModifier->SetBindingPose(mesh, *model, skinData, weightsCount);


		const dBone* rootBone = NULL;
		for (int i = 0; !rootBone && (i < mesh->m_vertexCount); i ++) {
			for (int j = 0; j < 4; j ++) {
				if (skinModifier->m_vertexWeight[i][j] > 0.0f) {
					int boneIndex = skinModifier->m_boneWeightIndex[i].m_index[j];
					for (rootBone = skinModifier->m_skinnedBones[boneIndex]; rootBone->GetParent() && (rootBone->GetParent()->GetType() == dBone::m_bone); rootBone = rootBone->GetParent());
					skinModifier->SetRootNode(rootBone);

					// remove all of the matrix offset form the skin by baking the transform into the mesh 
					domSkin::domJoints* joints = skin->getJoints();
					domInputLocal *jointName = joints->getInput_array()[0];
					domInputLocal *invBindMatrices = joints->getInput_array()[1];
					if (strcmp (invBindMatrices->getSemantic(), COMMON_PROFILE_INPUT_INV_BIND_MATRIX)) {
						domInputLocal *tmp = jointName;
						jointName = invBindMatrices;
						invBindMatrices = tmp;
					}
					_ASSERTE (!strcmp (jointName->getSemantic(), COMMON_PROFILE_INPUT_JOINT));
					_ASSERTE (!strcmp (invBindMatrices->getSemantic(), COMMON_PROFILE_INPUT_INV_BIND_MATRIX));

					uri = jointName->getSource();
					uri.resolveURI();
					element = uri.getElement ();
					boneSource = (domSource *)element;
					domName_array *boneNames = boneSource->getName_array();
					domListOfNames &boneSrcArray = boneNames->getValue();

					for (int k = 0; k < int (boneSrcArray.getCount()); k ++) {
						//const char* name = boneSrcArray[k];
						if (!strcmp (boneSrcArray[k], rootBone->GetName())) {

							uri = invBindMatrices->getSource();
							uri.resolveURI();
							element = uri.getElement ();
							domSource *source = (domSource *)element;
							domFloat_array *invMatricesValue = source->getFloat_array();
							domListOfFloats &values = invMatricesValue->getValue();
							dMatrix mat;
							for (int i = 0; i < 4; i ++) {
								for (int j = 0; j < 4; j ++) {
									mat[j][i] = dFloat (values[i * 4 + j + k * 16]);
								}
							}

							dMatrix skinMatrix (GetIdentityMatrix());
							for (int i = 0; i < skinModifier->m_bonesCount; i ++) {
								if (rootBone == skinModifier->m_skinnedBones[i]) {
									skinMatrix = skinModifier->m_bindingMatrices[i];
									break;
								}
							}

							dMatrix diff (mat * skinMatrix.Inverse());
							diff.TransformTriplex (mesh->m_vertex, 3 * sizeof (dFloat), mesh->m_vertex, 3 * sizeof (dFloat), mesh->m_vertexCount);
							break;
						}
					}
					break;
				}
			}
		}

		delete[] skinData;

		return skinModifier;
*/
	}


	void LoadMeshControllers (daeDocument* document, ModifierVertexCache& modifierVertexMap, dModel* model, ColladaNodeTodSceneNodeMap& nodeMap, CollGeoCache& meshCache)	
	{
/*
		int count; 
		domCOLLADA *domRoot;
		domLibrary_controllers *controllerLibrary;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		controllerLibrary = domRoot->getLibrary_controllers_array()[0];
		_ASSERTE (controllerLibrary);

		const domController_Array &geomteryArray = controllerLibrary->getController_array();
		count = int (geomteryArray.getCount());
		for (int i = 0; i < count; i ++ ) {
			dMesh* mesh;
			domSkin* skin;
			const daeElement* element;
			domController* controller;
			domGeometry* collGeometry;

			controller = geomteryArray[i];
			skin = controller->getSkin();

			const daeURI& uriMesh = skin->getSource();
			element = uriMesh.getElement();
			collGeometry = (domGeometry*) element;
			_ASSERTE (meshCache.Find(collGeometry));

			mesh = meshCache.Find(collGeometry)->GetInfo();
			LoadSkinController (document, skin, mesh, model, modifierVertexMap);
		}

		domCOLLADA *domRoot;
		domInstanceWithExtra *ivs;
		domVisual_scene* visualScene;
		domCOLLADA::domScene *sceneInstance;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		_ASSERTE (domRoot );

		sceneInstance = domRoot->getScene();
		_ASSERTE (sceneInstance);

		visualScene = NULL;
		ivs = sceneInstance->getInstance_visual_scene();
		if (ivs) {
			daeURI uri;
			uri = ivs->getUrl();
			const char* sceneName = uri.getID(); 
			const domLibrary_visual_scenes_Array &libraryArray = domRoot->getLibrary_visual_scenes_array ();

			for (int i = 0;  (!visualScene) && (i < int (libraryArray.getCount())); i ++) {
				const domVisual_scene_Array& visualSceneArray = libraryArray[i]->getVisual_scene_array();
				for (int j = 0; j < int (visualSceneArray.getCount()); j ++) {
					domVisual_scene* tmp;			
					tmp = visualSceneArray [j];
					if (!stricmp (sceneName, tmp->getId())) {
						visualScene = tmp;
						break;
					}
				}
			}
		} else {
			// a scene may not have visual representation but must have nodes 
			// load any scene from the scene library as a scene

			const domLibrary_visual_scenes_Array &libraryArray = domRoot->getLibrary_visual_scenes_array ();
			_ASSERTE (libraryArray.getCount());
			const domVisual_scene_Array& visualSceneArray = libraryArray[0]->getVisual_scene_array();
			_ASSERTE (visualSceneArray.getCount());
			visualScene = visualSceneArray [0];
		}

		if (visualScene) {
			int stack;
			domNode* nodePool[1024];

			stack = 0;
			const domNode_Array& nodeArray = visualScene->getNode_array();
			for (int i = 0; i < int (nodeArray.getCount()); i ++) {
				nodePool[stack] = nodeArray[i];
				stack ++;
			}


			while (stack) {
				domNode* colNode;

				stack --;
				colNode = nodePool[stack];

				const domInstance_controller_Array& controllerArray = colNode->getInstance_controller_array();
				if (controllerArray.getCount()) {
					dMesh* mesh;
					dBone* node;				
					domSkin* skin;
					dMeshInstance* instance;
					const daeElement* element;
					domController* controller;
					domGeometry* collGeometry;
					domInstance_controller* collInstance;

					collInstance = controllerArray[0];
					const daeURI& uri = collInstance->getUrl();
					element = uri.getElement();
					controller = (domController*) element;
					skin = controller->getSkin();

					const daeURI& uriMesh = skin->getSource();
					element = uriMesh.getElement();
					collGeometry = (domGeometry*) element;
					
					mesh = meshCache.Find(collGeometry)->GetInfo();
					_ASSERTE (mesh);

					node = nodeMap.Find(colNode)->GetInfo();
					
					instance = model->FindMeshInstance (mesh);
					instance->m_boneID = node->m_boneID;
					instance->m_modifier = LoadSkinController (document, skin, mesh, model, modifierVertexMap);
					mesh->m_hasBone = 1;
				}


				domInstance_node_Array& instansceNodeArray = colNode->getInstance_node_array();
				for (int i = 0; i < int (instansceNodeArray.getCount()); i ++) {
					daeURI uri;
					daeElement* element;
					domNode* childNode;
					domInstance_node* instansceNode = instansceNodeArray[0];
					uri = instansceNode->getUrl();
					uri.resolveURI();
					element = uri.getElement ();
					childNode = (domNode *)element;
					nodePool[stack] = childNode;
					stack ++;
				}

				const domNode_Array &nodeArray = colNode->getNode_array();
				for (int i = 0; i < int (nodeArray.getCount()); i ++) {
					nodePool[stack] = nodeArray[i];
					stack ++;
				}
			}
		}
*/
	}

	void ColladatToScene (dModel* model, dLoaderContext* context)
	{
		domAsset *asset;
		domCOLLADA *domRoot;
		daeDatabase* database;
		daeDocument *document;
		CollGeoCache meshCache;
		CollImageCache imageCache;
		domAsset::domUnit* unit;
		domAsset::domUp_axis* axis;
		CollMaterialCache materialCache;
		ColladaNodeTodSceneNodeMap nodeMap;
		ModifierVertexCache modifierVertexMapCache;

		database = m_collada->getDatabase();
		_ASSERTE (database);
		document = database->getDocument(daeUInt (0));
		_ASSERTE (document);


		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		_ASSERTE (domRoot);

		asset = domRoot->getAsset();
		_ASSERTE (domRoot);

		unit = asset->getUnit();
		if (unit) {
			m_scale *= dFloat (unit->getMeter());
		}

		axis = asset->getUp_axis();
		if (axis) {
			const domUpAxisType& value = axis->getValue();
			switch (value)
			{
				case UPAXISTYPE_X_UP:
				{
					m_globalRotation = dYawMatrix(3.14159265f * 0.5f) * m_globalRotation;
					break;
				}

				case UPAXISTYPE_Z_UP:
				{
					m_globalRotation = dPitchMatrix(-3.14159265f * 0.5f) * m_globalRotation;
					break;
				}

				case UPAXISTYPE_Y_UP:
				default:
				{
				}
			}
		}



//		LoadCamera(document);
		LoadTextures(document, imageCache);
		LoadMaterials(document, imageCache, materialCache);
		LoadGeometries(document, model, modifierVertexMapCache, meshCache, materialCache, context);

/*
		switch (mode)
		{
			case m_meshOnly:
			{
				LoadVisualScene(document);
				break;
			}

			case m_fullScene:
			{
				LoadVisualScene(document);

				// set default material properties
				if (colladaMaterialIndex == -1) {
					colladaMaterialIndex = NewtonMaterialCreateGroupID (m_world);
					NewtonMaterialSetCollisionCallback (m_world, colladaMaterialIndex, colladaMaterialIndex, NULL, ColladaContactBegin, ColladaContactProcess, ColladaContactEnd); 
				}
				LoadPhysicsScene(document);
				break;
			}

			case m_physicCollisionAndVisual:
			{
				LoadVisualScene(document);

				// set default material properties
				if (colladaMaterialIndex == -1) {
					colladaMaterialIndex = NewtonMaterialCreateGroupID (m_world);
					NewtonMaterialSetCollisionCallback (m_world, colladaMaterialIndex, colladaMaterialIndex, NULL, ColladaContactBegin, ColladaContactProcess, ColladaContactEnd); 
				}
				LoadPhysicsScene(document);

				ReplaceVisulaMeshWithCollisionProxis();
				break;
			}
		}
*/
		LoadVisualScene(document, model, nodeMap, meshCache);
		if (domRoot->getLibrary_controllers_array().getCount()) {
			LoadMeshControllers (document, modifierVertexMapCache, model, nodeMap, meshCache);
		}

		model->ApplyGlobalTransform(m_globalRotation);
		model->ApplyGlobalScale(m_scale);

		context->LoaderFixup (model);
	}


#if 0
	SourceBuffer *FindBufferSouce (domConvex_mesh *colladaMesh, daeString type, SourceBuffer* sourceBuffers)const
	{
		int i;
		int count;
		daeURI uri;
		domInputLocal* vertexInput;
		domInputLocalOffset* triangleInput;
		domVertices *vertices;
		domTriangles* triangles;

		vertexInput = NULL;
		vertices = colladaMesh->getVertices();
		domInputLocal_Array &inputArray = vertices->getInput_array();
		for (i = 0; i < int (inputArray.getCount()); i ++) {
			domInputLocal* offset;
			offset = inputArray[i];
			if (!stricmp (offset->getSemantic(), type)) {
				vertexInput = offset;
				break;
			}
		}

		if (vertexInput) {
			uri = vertexInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (i = 0; i <  count; i ++) {
				if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
					return &sourceBuffers[i];
				}
			}
		}


		triangleInput = NULL;
		domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
		if (trianglesArray.getCount()) {
			triangles = trianglesArray[0];
			domInputLocalOffset_Array &inputArray = triangles->getInput_array();
			for (i = 0; i < int (inputArray.getCount()); i ++) {
				domInputLocalOffset* offset;
				offset = inputArray[i];
				if (!stricmp (offset->getSemantic(), type)) {
					triangleInput = offset;
					break;
				}
			}
		}

		if (triangleInput) {
			uri = triangleInput->getSource();
			domSource_Array &sourceArray = colladaMesh->getSource_array();
			count = int (sourceArray.getCount());
			for (i = 0; i <  count; i ++) {
				if (!stricmp (uri.getID(), sourceBuffers[i].m_id)) {
					return &sourceBuffers[i];
				}
			}
		}

		return NULL;
	}

	void MakeNodeList (dSceneNode* root, dList<dSceneNode*>& nodeList)
	{
		dSceneNode* node;

		nodeList.Append(root);
		for (node = root->GetChild(); node; node = node->GetSibling()) {
			MakeNodeList (node, nodeList);
		}
	}

	domGeometry *AddConvexGeometry (daeDocument* document, const dGeometry* geometry)
	{
		int i;
		domExtra* extra;
		domCOLLADA *domRoot;
		domVertices *verts;
		domSource *uvSource;
		domSource *posSource;
		domSource *normalSrc;
		domTriangles *polys1;
		domGeometry *collGeometry;
		domConvex_mesh *colladaMesh;
		domLibrary_geometries *library;
		dList<dGeometrySegment>::dListNode *segments;	
		char text[256];

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

		library = domRoot->getLibrary_geometries_array()[0];
		_ASSERTE (library);

		collGeometry = daeSafeCast<domGeometry>(library->createAndPlace(COLLADA_ELEMENT_GEOMETRY ));
		collGeometry->setId(geometry->m_name);

		extra = daeSafeCast<domExtra>(collGeometry->createAndPlace(COLLADA_ELEMENT_EXTRA ));
		domTechnique *technique = daeSafeCast<domTechnique>(extra->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
		technique->setProfile(D_VERTEX_FORMAT);
//		colladaMesh = daeSafeCast<domMesh>(collGeometry->createAndPlace(COLLADA_ELEMENT_MESH ));
		colladaMesh = daeSafeCast<domConvex_mesh>(collGeometry->createAndPlace(COLLADA_ELEMENT_CONVEX_MESH ));

		// add the vertices
		float* floatPool = new float[geometry->m_vertexCount * 3];
		{
			posSource = daeSafeCast<domSource>(colladaMesh->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%s_position", geometry->m_name);
			posSource->setId (text);

			for (i = 0; i < geometry->m_vertexCount; i ++) {
				floatPool[i * 3 + 0] = geometry->m_vertex[i * 3 + 0];
				floatPool[i * 3 + 1] = geometry->m_vertex[i * 3 + 1];
				floatPool[i * 3 + 2] = geometry->m_vertex[i * 3 + 2];
			}
			m_axisRotation.TransformTriplex(floatPool, sizeof (float) * 3, floatPool, sizeof (float) * 3, geometry->m_vertexCount);
			domFloat_array *fa = daeSafeCast<domFloat_array>(posSource->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			fa->setId( text );
			fa->setCount( geometry->m_vertexCount * 3 );

			domListOfFloats &posSrcArray = fa->getValue();
			for (i = 0; i < geometry->m_vertexCount; i ++) {
				posSrcArray.append3( floatPool[i * 3 + 0], floatPool[i * 3 + 1], floatPool[i * 3 + 2]);
			}

			//create the accessor
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(posSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (geometry->m_vertexCount );
			acc->setStride (3);
			daeURI uri;
			uri.setElement (fa);
			uri.resolveURI();
			acc->setSource( uri );

			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "x" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "y" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "z" );
			param->setType( "float" );


			verts = daeSafeCast<domVertices>(colladaMesh->createAndPlace(COLLADA_ELEMENT_VERTICES));
			sprintf (text, "%sVertices", geometry->m_name);
			verts->setId (text);
			domInputLocal *inputLocal = daeSafeCast<domInputLocal>(verts->createAndPlace(COLLADA_ELEMENT_INPUT));
			inputLocal->setSemantic (COMMON_PROFILE_INPUT_POSITION);
			uri.setElement (posSource);
			uri.resolveURI();
			inputLocal->setSource (uri);
		}


		{
			normalSrc = daeSafeCast<domSource>(colladaMesh->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%sNormal", geometry->m_name);
			normalSrc->setId (text);

			for (i = 0; i < geometry->m_vertexCount; i ++) {
				floatPool[i * 3 + 0] = geometry->m_normal[i * 3 + 0];
				floatPool[i * 3 + 1] = geometry->m_normal[i * 3 + 1];
				floatPool[i * 3 + 2] = geometry->m_normal[i * 3 + 2];
			}
			m_axisRotation.TransformTriplex(floatPool, sizeof (float) * 3, floatPool, sizeof (float) * 3, geometry->m_vertexCount);
			domFloat_array *fa = daeSafeCast<domFloat_array>(normalSrc->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			fa->setId (text);
			fa->setCount (geometry->m_vertexCount * 3);

			domListOfFloats &posSrcArray = fa->getValue();
			for (i = 0; i < geometry->m_vertexCount; i ++) {
				posSrcArray.append3( floatPool[i * 3 + 0], floatPool[i * 3 + 1], floatPool[i * 3 + 2]);
			}

			//create the accessors
			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(normalSrc->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (geometry->m_vertexCount);
			acc->setStride (3);
			daeURI uri;
			uri.setElement (fa);
			uri.resolveURI();
			acc->setSource( uri );

			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "x" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "y" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "z" );
			param->setType( "float" );
		}

		{
			uvSource = daeSafeCast<domSource>(colladaMesh->createAndPlace(COLLADA_ELEMENT_SOURCE));
			sprintf (text, "%sUV", geometry->m_name);
			uvSource->setId (text);
			for (i = 0; i < geometry->m_vertexCount; i ++) {
				floatPool[i * 2 + 0] = geometry->m_uv[i * 2 + 0];
				floatPool[i * 2 + 1] = geometry->m_uv[i * 2 + 1];
			}
			domFloat_array *fa = daeSafeCast<domFloat_array>(uvSource->createAndPlace(COLLADA_ELEMENT_FLOAT_ARRAY));
			strcat (text, "Array");
			fa->setId (text);
			fa->setCount(geometry->m_vertexCount * 2);
			domListOfFloats &posSrcArray = fa->getValue();
			for (i = 0; i < geometry->m_vertexCount; i ++) {
				posSrcArray.append2( floatPool[i * 2 + 0], floatPool[i * 2 + 1]);
			}

			domSource::domTechnique_common *srcTeqC = daeSafeCast<domSource::domTechnique_common>(uvSource->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			domAccessor *acc = daeSafeCast<domAccessor>(srcTeqC->createAndPlace(COLLADA_ELEMENT_ACCESSOR));
			acc->setCount (geometry->m_vertexCount);
			acc->setStride( 2 );
			daeURI uri;
			uri.setElement( fa );
			uri.resolveURI();
			acc->setSource( uri );

			domParam *param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "s" );
			param->setType( "float" );
			param = daeSafeCast<domParam>(acc->createAndPlace(COLLADA_ELEMENT_PARAM));
			param->setName( "t" );
			param->setType( "float" );
		}
		delete[] floatPool;

		for (segments = geometry->GetFirst(); segments; segments = segments->GetNext()) {
			daeURI uri;
			char materialName[128];
			domInputLocalOffset *input;
			dGeometrySegment& segment = segments->GetInfo();
			sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, dCRC (segment.m_textureName));

			polys1 = daeSafeCast<domTriangles>(colladaMesh->createAndPlace(COLLADA_ELEMENT_TRIANGLES));

			polys1->setCount (segment.m_indexCount / 3);
			polys1->setMaterial (materialName);

			input = daeSafeCast<domInputLocalOffset>(polys1->createAndPlace(COLLADA_ELEMENT_INPUT ));
			input->setSemantic( COMMON_PROFILE_INPUT_VERTEX );
			input->setOffset( 0 );
			//uri.setElement (posSource);
			uri.setElement (verts);
			uri.resolveURI();
			input->setSource (uri);

			input = daeSafeCast<domInputLocalOffset>(polys1->createAndPlace(COLLADA_ELEMENT_INPUT));
			input->setSemantic( COMMON_PROFILE_INPUT_NORMAL );
			input->setOffset (0);
			uri.setElement (normalSrc);
			uri.resolveURI();
			input->setSource (uri);

			input = daeSafeCast<domInputLocalOffset>(polys1->createAndPlace(COLLADA_ELEMENT_INPUT));
			input->setSemantic (COMMON_PROFILE_INPUT_TEXCOORD);
			input->setOffset (0);
			//					input->setSet (1);
			input->setSet (0);
			uri.setElement (uvSource);
			uri.resolveURI();
			input->setSource (uri);

			domP *p = daeSafeCast<domP>(polys1->createAndPlace(COLLADA_ELEMENT_P));
			domListOfUInts &indices = p->getValue();
			for (i = 0; i < segment.m_indexCount; i += 3) {
				indices.append3 (segment.m_indexes[i + 0], segment.m_indexes[i + 1], segment.m_indexes[i + 2]);
			}
		}
		return collGeometry;
	}





	void AddCameraLibrary (daeDocument* document, dSceneNode* rootNode)
	{
		domCOLLADA *domRoot;
		CameraNode* camera;
		domLibrary_cameras *library;
		domCamera::domOptics* optics;
		domCamera::domOptics::domTechnique_common* technique;
		domCamera::domOptics::domTechnique_common::domPerspective* perpective;

		camera = (CameraNode*) rootNode->Find(CAMERA_NAME);
//		_ASSERTE (camera);
		if (camera) {
			domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

			library = daeSafeCast<domLibrary_cameras>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_CAMERAS));

			m_collCamera = daeSafeCast<domCamera>(library->createAndPlace(COLLADA_ELEMENT_CAMERA));
			m_collCamera->setId(CAMERA_NAME);
			optics = daeSafeCast<domCamera::domOptics>(m_collCamera->createAndPlace(COLLADA_ELEMENT_OPTICS));
			technique = daeSafeCast<domCamera::domOptics::domTechnique_common>(optics->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
			perpective = daeSafeCast<domCamera::domOptics::domTechnique_common::domPerspective>(technique->createAndPlace(COLLADA_ELEMENT_PERSPECTIVE));

			domTargetableFloat* fov; 
			domTargetableFloat* aspect; 
			domTargetableFloat* farPlane;
			domTargetableFloat* nearPlane;
			
			fov = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_YFOV));
			aspect = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_ASPECT_RATIO));
			nearPlane = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_ZNEAR));
			farPlane = daeSafeCast<domTargetableFloat>(perpective->createAndPlace(COLLADA_ELEMENT_ZFAR));

			fov->setValue(camera->m_fov);
			aspect->setValue(camera->m_aspect);
			nearPlane->setValue(camera->m_nearPlane);
			farPlane->setValue(camera->m_farPlane);
		}
	}


	dMatrix GetOffssetMatrix (const domTranslate_Array &translationArray, const domRotate_Array &rotationArray)
	{
		int i;
		dMatrix matrix (GetIdentityMatrix());

		for (i = 0; i < int (translationArray.getCount()); i ++) {
			const domFloat3& data = translationArray[i]->getValue();
			matrix[3][0] += dFloat (data[0]);
			matrix[3][1] += dFloat (data[1]);
			matrix[3][2] += dFloat (data[2]);
		}

		for (i = 0; i < int (rotationArray.getCount()); i ++) {
			dFloat angle;
			const domFloat4& data = rotationArray[i]->getValue();
			angle = dFloat (data[3]) * 3.1316f / 180.0f;
			dVector axis (dFloat (data[0]), dFloat (data[1]), dFloat (data[2]), 0.0f);
			axis = axis.Scale (1.0f / sqrtf (axis % axis));
			dQuaternion rot (axis, angle);
			dMatrix colladaMatrix (rot, dVector (0.0f, 0.0f, 0.0f, 1.0f));
			matrix = colladaMatrix * matrix;
		}
		matrix = m_axisRotationInv * matrix * m_axisRotation;
		return matrix;
	}



	void AddOffssetMatrix  (daeElement* parent, const dMatrix& offsetMatrix)
	{
		dFloat mag2;
		dFloat angle;
		domRotate* rotation;
		domTranslate* translation;

		dMatrix matrix (m_axisRotationInv * offsetMatrix * m_axisRotation);

		dQuaternion rot (matrix);
		angle = 2.0f * acosf (rot.m_q0);
		dVector dir (rot.m_q1, rot.m_q2, rot.m_q3, 0.0f);

		mag2 = dir % dir;
		if (mag2 > 1.0e-6f) {
			dir = dir.Scale(1.0f / sqrtf (dir % dir));
		} else {
			dir = dVector (1.0f, 0.0f, 0.0f, 0.0f);
		}

		translation = daeSafeCast<domTranslate>(parent->createAndPlace(COLLADA_ELEMENT_TRANSLATE)); 
		translation->getValue().append (matrix.m_posit.m_x);
		translation->getValue().append (matrix.m_posit.m_y);
		translation->getValue().append (matrix.m_posit.m_z);

		rotation = daeSafeCast<domRotate>(parent->createAndPlace(COLLADA_ELEMENT_ROTATE)); 
		rotation->getValue().append (dir.m_x);
		rotation->getValue().append (dir.m_y);
		rotation->getValue().append (dir.m_z);
		rotation->getValue().append (angle * 180.0f / 3.14159265f);
	}

	void ApplyMatrixAligment (dMatrix& matrix)
	{
		matrix = dRollMatrix(-0.5f * 3.14159265f) * matrix;
	}


//	void AddShape (daeDocument* document, domRigid_body::domTechnique_common* technique, const NewtonBody* newtonBody)
	void AddShape (daeDocument* document, domRigid_body::domTechnique_common* technique, const NewtonCollision* collision)
	{
		int isSinglePrimitive;
//		const NewtonCollision* collision;
		NewtonCollisionInfoRecord collisionInfo;
		domRigid_body::domTechnique_common::domShape *shape;


		// get the collision information
//		collision = NewtonBodyGetCollision (newtonBody);
		NewtontCollisionGetInfo (collision, &collisionInfo);

		shape = NULL;
		isSinglePrimitive = 0;
		if (!strcmp (collisionInfo.m_collisionType, "box")) {
			domBox* box;
			domBox::domHalf_extents* halfExtend;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			box = daeSafeCast<domBox>(shape->createAndPlace(COLLADA_ELEMENT_BOX)); 
			halfExtend = daeSafeCast<domBox::domHalf_extents>(box->createAndPlace(COLLADA_ELEMENT_HALF_EXTENTS)); 

			dVector size (collisionInfo.m_box.m_x * 0.5f, collisionInfo.m_box.m_y * 0.5f, collisionInfo.m_box.m_z * 0.5f, 0.0f);
			size = m_axisRotation.RotateVector(size);
			halfExtend->getValue().append (size.m_x);
			halfExtend->getValue().append (size.m_y);
			halfExtend->getValue().append (size.m_z);

		} else if (!strcmp (collisionInfo.m_collisionType, "sphere")) {
			domSphere* sphere;
			domSphere::domRadius* halfExtend;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			sphere = daeSafeCast<domSphere>(shape->createAndPlace(COLLADA_ELEMENT_SPHERE)); 
			halfExtend = daeSafeCast<domSphere::domRadius>(sphere->createAndPlace(COLLADA_ELEMENT_RADIUS)); 
			halfExtend->setValue(collisionInfo.m_sphere.m_r0);

		} else if (!strcmp (collisionInfo.m_collisionType, "cylinder")) {
			domCylinder* cylinder;
			domCylinder::domHeight* height;
			domCylinder::domRadius* radius;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			cylinder = daeSafeCast<domCylinder>(shape->createAndPlace(COLLADA_ELEMENT_CYLINDER)); 

			height = daeSafeCast<domCylinder::domHeight>(cylinder->createAndPlace(COLLADA_ELEMENT_HEIGHT)); 
			height->setValue(collisionInfo.m_cylinder.m_height * 0.5f);

			radius = daeSafeCast<domCylinder::domRadius>(cylinder->createAndPlace(COLLADA_ELEMENT_RADIUS)); 
			radius->getValue().append(collisionInfo.m_cylinder.m_r0);
			radius->getValue().append(collisionInfo.m_cylinder.m_r1);

			dMatrix& matrix = *((dMatrix*) &collisionInfo.m_offsetMatrix[0][0]);
			 ApplyMatrixAligment (matrix);

		} else if (!strcmp (collisionInfo.m_collisionType, "capsule")) {
			domCapsule* capsule;
			domCapsule::domHeight* height;
			domCapsule::domRadius* radius;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			capsule = daeSafeCast<domCapsule>(shape->createAndPlace(COLLADA_ELEMENT_CAPSULE)); 

			height = daeSafeCast<domCapsule::domHeight>(capsule->createAndPlace(COLLADA_ELEMENT_HEIGHT)); 
			height->setValue(collisionInfo.m_capsule.m_height * 0.5f - collisionInfo.m_capsule.m_r0);

			radius = daeSafeCast<domCapsule::domRadius>(capsule->createAndPlace(COLLADA_ELEMENT_RADIUS)); 
			radius->getValue().append(collisionInfo.m_capsule.m_r0);
			radius->getValue().append(collisionInfo.m_capsule.m_r1);

			dMatrix& matrix = *((dMatrix*) &collisionInfo.m_offsetMatrix[0][0]);
			 ApplyMatrixAligment (matrix);

		} else if (!strcmp (collisionInfo.m_collisionType, "cone")) {
			domTapered_cylinder* cone;
			domTapered_cylinder::domHeight* height;
			domTapered_cylinder::domRadius1* radius1;
			domTapered_cylinder::domRadius2* radius2;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			cone = daeSafeCast<domTapered_cylinder>(shape->createAndPlace(COLLADA_ELEMENT_TAPERED_CYLINDER)); 

			height = daeSafeCast<domTapered_cylinder::domHeight>(cone->createAndPlace(COLLADA_ELEMENT_HEIGHT)); 
			height->setValue(collisionInfo.m_cone.m_height * 0.5f);

			radius1 = daeSafeCast<domTapered_cylinder::domRadius1>(cone->createAndPlace(COLLADA_ELEMENT_RADIUS1)); 
			radius1->getValue().append(collisionInfo.m_cone.m_r);

			radius2 = daeSafeCast<domTapered_cylinder::domRadius2>(cone->createAndPlace(COLLADA_ELEMENT_RADIUS2)); 
			radius2->getValue().append(0.0f);

			dMatrix& matrix = *((dMatrix*) &collisionInfo.m_offsetMatrix[0][0]);
			 ApplyMatrixAligment (matrix);

		} else  if (!strcmp (collisionInfo.m_collisionType, "chamferCylinder")) {
			domExtra* extra;
			domCylinder* cylinder;
			domTechnique *extraTechnique;
			domCylinder::domHeight* height;
			domCylinder::domRadius* radius;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			cylinder = daeSafeCast<domCylinder>(shape->createAndPlace(COLLADA_ELEMENT_CYLINDER)); 

			height = daeSafeCast<domCylinder::domHeight>(cylinder->createAndPlace(COLLADA_ELEMENT_HEIGHT)); 
			height->setValue(collisionInfo.m_chamferCylinder.m_height * 0.5);

			radius = daeSafeCast<domCylinder::domRadius>(cylinder->createAndPlace(COLLADA_ELEMENT_RADIUS)); 
			radius->getValue().append(collisionInfo.m_chamferCylinder.m_r);
			radius->getValue().append(collisionInfo.m_chamferCylinder.m_r);

			extra = daeSafeCast<domExtra>(cylinder->createAndPlace(COLLADA_ELEMENT_EXTRA));
			extraTechnique = daeSafeCast<domTechnique>(extra->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
			extraTechnique->setProfile("chamferCylinder");

			dMatrix& matrix = *((dMatrix*) &collisionInfo.m_offsetMatrix[0][0]);
			 ApplyMatrixAligment (matrix);

		} else if (!strcmp (collisionInfo.m_collisionType, "convexHull")) {
			
			daeURI uri;
			dGeometry* geometry;
			domCOLLADA *domRoot;
			domMaterial* material;
			domBind_material *bindMat;
			domGeometry *collGeometry;
			domInstance_material *instMat;
			domInstance_geometry* instance;
			domLibrary_materials* materialLibrary;
			domBind_material::domTechnique_common *bmtc;
			char materialName[128];

			static int convex_meshID = 0;

			isSinglePrimitive = 1;

			if (m_collisionList.Find (collision)) {
				collGeometry = m_collisionList.Find (collision)->GetInfo();
			} else {
				geometry = dGeometry::MakeMeshFromCollision (m_world, collision, NULL);
				geometry->ConvertToSimpleMesh ();
				sprintf (geometry->m_name, "%s%04d", D_CONVEX_MESH_NAME, convex_meshID);
				convex_meshID ++;
	//			collGeometry = AddGeometry (document, geometry);
				collGeometry = AddConvexGeometry (document, geometry);
				geometry->Release();
				m_collisionList.Insert (collGeometry, collision);
			}
			
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			instance = daeSafeCast<domInstance_geometry>(shape->createAndPlace (COLLADA_ELEMENT_INSTANCE_GEOMETRY)); 
			uri.setElement(collGeometry);
			uri.resolveURI();
			instance->setUrl (uri);

			bindMat = daeSafeCast<domBind_material>(instance->createAndPlace(COLLADA_ELEMENT_BIND_MATERIAL));
			bmtc = daeSafeCast<domBind_material::domTechnique_common>(bindMat->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON ));
			instMat = daeSafeCast<domInstance_material>(bmtc->createAndPlace(COLLADA_ELEMENT_INSTANCE_MATERIAL ));

			sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, dCRC (0));
			domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot() );
			materialLibrary = domRoot->getLibrary_materials_array()[0];
			material = FindMaterial (materialLibrary, materialName);
			_ASSERTE (material);
			uri.setElement (material);
			uri.resolveURI();
			instMat->setSymbol(materialName);
			instMat->setTarget (uri);

		} else if (!strcmp (collisionInfo.m_collisionType, "staticTree")) {
			int i;
			int j;
			int indexCount;
			int vertexCount;
			int vertexStride;
			int newVertexCount;
			int*  vertexMask;
			const dFloat* vertexArray;
//			int indexArray[1024 * 128];
			daeURI uri;
			dGeometry* geometry;
			domCOLLADA *domRoot;
			domMaterial* material;
			domBind_material *bindMat;
			domGeometry *collGeometry;
			domInstance_material *instMat;
			domInstance_geometry* instance;
			domLibrary_materials* materialLibrary;
			domBind_material::domTechnique_common *bmtc;
			char materialName[128];

			static int static_meshID = 0;

			isSinglePrimitive = 1;

			if (m_collisionList.Find (collision)) {
				collGeometry = m_collisionList.Find (collision)->GetInfo();
			} else {
				#define TREE_INDEX_BUFFER_SIZE 1024 * 256

				int* indexArray = new int [TREE_INDEX_BUFFER_SIZE];


				dVector minBox (-1.0e10f, -1.0e10f, -1.0e10f);
				dVector maxBox ( 1.0e10f,  1.0e10f,  1.0e10f);
				indexCount = NewtonTreeCollisionGetVertexListIndexListInAABB (collision, &minBox.m_x, &maxBox.m_x,
																	&vertexArray, &vertexCount, &vertexStride, 
																	indexArray, TREE_INDEX_BUFFER_SIZE); 

				geometry = new dGeometry;
				sprintf (geometry->m_name, "%s%04d", D_STATIC_MESH_NAME, static_meshID);
				static_meshID ++;

				// pack the vertex list
				vertexMask = new int [vertexCount];
				memset (vertexMask, -1, vertexCount * sizeof (int));
				newVertexCount = 0;
				for (i = 0; i < indexCount; i ++) {
					j = indexArray[i];
					if (vertexMask[j] == -1) {
						vertexMask[j] = newVertexCount;
						newVertexCount ++;
					}
				}

				vertexStride = vertexStride / sizeof (dFloat);
				geometry->AllocVertexData(newVertexCount);
				for (i = 0; i < vertexCount; i ++ ) {
					j = vertexMask[i];
					if (j != -1) {
						_ASSERTE (j < newVertexCount);
						geometry->m_vertex[j * 3 + 0] = GLfloat (vertexArray[i * vertexStride + 0]);
						geometry->m_vertex[j * 3 + 1] = GLfloat (vertexArray[i * vertexStride + 1]);
						geometry->m_vertex[j * 3 + 2] = GLfloat (vertexArray[i * vertexStride + 2]);

						geometry->m_normal[j * 3 + 0] = GLfloat (0.0f);
						geometry->m_normal[j * 3 + 1] = GLfloat (1.0f);
						geometry->m_normal[j * 3 + 2] = GLfloat (0.0f);

						geometry->m_normal[j * 2 + 0] = GLfloat (0.0f);
						geometry->m_normal[j * 2 + 1] = GLfloat (0.0f);
					}
				}

				geometry->Append();
				dGeometrySegment& segment = geometry->GetLast()->GetInfo();
				segment.AllocIndexData(dInt32 (indexCount));
				for (i = 0; i < indexCount; i ++) {
					j = indexArray[i];
					_ASSERTE (vertexMask[j] >= 0);
					_ASSERTE (vertexMask[j] < newVertexCount);
					segment.m_indexes[i] = GLushort (vertexMask[j]);
				}
				delete[] vertexMask; 

				collGeometry = AddGeometry (document, geometry);
				geometry->Release();
				m_collisionList.Insert (collGeometry, collision);

				delete[] indexArray;
			}


			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			instance = daeSafeCast<domInstance_geometry>(shape->createAndPlace (COLLADA_ELEMENT_INSTANCE_GEOMETRY)); 
			uri.setElement(collGeometry);
			uri.resolveURI();
			instance->setUrl (uri);

			bindMat = daeSafeCast<domBind_material>(instance->createAndPlace(COLLADA_ELEMENT_BIND_MATERIAL));
			bmtc = daeSafeCast<domBind_material::domTechnique_common>(bindMat->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON ));
			instMat = daeSafeCast<domInstance_material>(bmtc->createAndPlace (COLLADA_ELEMENT_INSTANCE_MATERIAL));

			sprintf (materialName, "%s%x", D_MATERIAL_PREFIX_NAME, dCRC (0));
			domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot() );
			materialLibrary = domRoot->getLibrary_materials_array()[0];
			material = FindMaterial (materialLibrary, materialName);
			_ASSERTE (material);
			uri.setElement (material);
			uri.resolveURI();
			instMat->setSymbol(materialName);
			instMat->setTarget (uri);

		} else if (!strcmp (collisionInfo.m_collisionType, "plane")) {
			domPlane* plane;
			domPlane::domEquation* equation;

			isSinglePrimitive = 1;
			shape = daeSafeCast<domRigid_body::domTechnique_common::domShape>(technique->createAndPlace(COLLADA_ELEMENT_SHAPE));
			plane = daeSafeCast<domPlane>(shape->createAndPlace(COLLADA_ELEMENT_PLANE)); 
			equation = daeSafeCast<domPlane::domEquation>(plane->createAndPlace(COLLADA_ELEMENT_EQUATION)); 

			dVector surface (collisionInfo.m_paramArray[0], collisionInfo.m_paramArray[1], collisionInfo.m_paramArray[2], collisionInfo.m_paramArray[3]);
			surface = m_axisRotation.TransformPlane (surface);
			equation->getValue().append (surface.m_x);
			equation->getValue().append (surface.m_y);
			equation->getValue().append (surface.m_z);
			equation->getValue().append (surface.m_w);

		} else if (!strcmp (collisionInfo.m_collisionType, "compound")) {
			int i;
			isSinglePrimitive = 0;
			for (i = 0; i < collisionInfo.m_compoundCollision.m_chidrenCount; i ++) {
				AddShape (document, technique, collisionInfo.m_compoundCollision.m_chidren[i]);
			}

		} else {
			_ASSERTE (0);
		}

		if (isSinglePrimitive ) {
			dMatrix& matrix = *((dMatrix*) &collisionInfo.m_offsetMatrix[0][0]);
			AddOffssetMatrix  (shape, matrix);
		}
	}


	domPhysics_material* AddPhysicsMaterialLibrary(daeDocument* document, const NewtonBody* newtonBody, int enumID)
	{
		char name[256];
		domCOLLADA *domRoot;
		dSceneNode* primitive;
		domPhysics_material* material;
		domLibrary_physics_materials* library;
		domPhysics_material::domTechnique_common* technique;

		ColladaFrictionRestituionMaterial* frictionRestituionMaterial;


		// scan the image library and save each texture
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		_ASSERTE (domRoot->getLibrary_physics_materials_array().getCount());
		library = domRoot->getLibrary_physics_materials_array()[0];
		_ASSERTE (library);

		sprintf (name, "%s%d", D_PHYSICS_MATERIAL_NAME, enumID);

		material = daeSafeCast<domPhysics_material>(library->createAndPlace(COLLADA_ELEMENT_PHYSICS_MATERIAL));
		material->setId (name);

		technique = daeSafeCast<domPhysics_material::domTechnique_common>(material->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
		_ASSERTE (technique);

		daeSafeCast<domTargetableFloat>(technique->createAndPlace(COLLADA_ELEMENT_RESTITUTION));
		daeSafeCast<domTargetableFloat>(technique->createAndPlace(COLLADA_ELEMENT_STATIC_FRICTION));
		daeSafeCast<domTargetableFloat>(technique->createAndPlace(COLLADA_ELEMENT_DYNAMIC_FRICTION));

		// get the graphic object form the rigid body
		primitive = (RenderPrimitive*) NewtonBodyGetUserData (newtonBody);
		if (primitive  && primitive->GetUserData()) {
			frictionRestituionMaterial = (ColladaFrictionRestituionMaterial*) primitive->GetUserData();
			technique->getRestitution()->setValue(frictionRestituionMaterial->m_restitution);
			technique->getStatic_friction()->setValue(frictionRestituionMaterial->m_staticFriction);
			technique->getDynamic_friction()->setValue(frictionRestituionMaterial->m_dynamicFriction);
		} else {
			technique->getRestitution()->setValue(0.3f);
			technique->getStatic_friction()->setValue(0.6f);
			technique->getDynamic_friction()->setValue(0.4f);
		}

		return material;
	}



	domRigid_body* AddRigidBody (daeDocument* document, domPhysics_model* model, const NewtonBody* newtonBody, int enumID)
	{
		dFloat mass;
		dVector com;
		dVector inertia;
		daeURI uri;
		bool isDynamic;
		domCOLLADA *domRoot;
		domRotate* rotation;
		domTranslate* translation;
		domRigid_body* rigidBody;
		domTargetableFloat* bodyMass;
//		domPhysics_material* material;
		domTargetableFloat3* bodyInertia;
		domInstance_physics_material* intanceMaterial;
//		domLibrary_physics_materials* materialLibrary;
		domRigid_body::domTechnique_common* techniqueCommon;
		domRigid_body::domTechnique_common::domDynamic* dynamicBody;
		domRigid_body::domTechnique_common::domMass_frame* massFrame;
		char tmpName[256];


		// create the image library
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot() );

		// add a body to this model
		rigidBody = daeSafeCast<domRigid_body>(model->createAndPlace(COLLADA_ELEMENT_RIGID_BODY));
		sprintf (tmpName, "%s%04d", D_PHYSICS_RIGIDBODY_NAME, enumID);
		rigidBody->setSid(tmpName);

		// add rigid body info
		techniqueCommon = daeSafeCast<domRigid_body::domTechnique_common>(rigidBody->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
		_ASSERTE (techniqueCommon);

		// set the mass and the body type
		NewtonBodyGetMassMatrix (newtonBody, &mass, &inertia.m_x, &inertia.m_y, &inertia.m_z);

		isDynamic = (mass > 1.0e-3f) ? true : false; 
		dynamicBody = daeSafeCast<domRigid_body::domTechnique_common::domDynamic>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_DYNAMIC));
		dynamicBody->setValue(isDynamic);

		bodyMass = daeSafeCast<domTargetableFloat>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_MASS));
		bodyMass->setValue(mass);

		NewtonBodyGetCentreOfMass (newtonBody, &com.m_x);
		massFrame = daeSafeCast<domRigid_body::domTechnique_common::domMass_frame>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_MASS_FRAME));

		com.m_w = 0.0f;
		com = m_axisRotation.RotateVector(com);
		translation = daeSafeCast<domTranslate>(massFrame->createAndPlace(COLLADA_ELEMENT_TRANSLATE)); 
		translation->getValue().append (com.m_x);
		translation->getValue().append (com.m_y);
		translation->getValue().append (com.m_z);

		rotation = daeSafeCast<domRotate>(massFrame->createAndPlace(COLLADA_ELEMENT_ROTATE)); 
		rotation->getValue().append (0.0f);
		rotation->getValue().append (0.0f);
		rotation->getValue().append (1.0f);
		rotation->getValue().append (0.0f);

		inertia.m_w = 0.0f;
		inertia = m_axisRotation.RotateVector(inertia);
		bodyInertia = daeSafeCast<domTargetableFloat3>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_INERTIA));
		bodyInertia->getValue().append(inertia.m_x);
		bodyInertia->getValue().append(inertia.m_y);
		bodyInertia->getValue().append(inertia.m_z);

		// material for this body
		intanceMaterial = daeSafeCast<domInstance_physics_material>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_INSTANCE_PHYSICS_MATERIAL));
		_ASSERTE (intanceMaterial);
//		uri.setElement (material);
		uri.setElement (AddPhysicsMaterialLibrary(document, newtonBody, enumID));
		uri.resolveURI();
		intanceMaterial->setUrl(uri);

		// add collision shape for this body
		AddShape (document, techniqueCommon, NewtonBodyGetCollision (newtonBody));

		domExtra* extra;
		domTechnique *technique;

		extra = daeSafeCast<domExtra>(rigidBody->createAndPlace(COLLADA_ELEMENT_EXTRA));
		technique = daeSafeCast<domTechnique>(extra->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
		technique->setProfile("Newton");

		int materialGroudlID;
		int continueCollision;
		int collideWithCollision;
	
		float linearDragValue;
		float angularDragValue[4];
		daeElement* element;
		domAny *domExtraExtension;


		element = (daeElement*)technique->createAndPlace( "BindingExternalForceFunction" ); 
		domExtraExtension = (domAny*)element; 
		sprintf (tmpName, "%s", "ApplyGravity");
		domExtraExtension->setValue (tmpName);

		element = (daeElement*)technique->createAndPlace( "BindingSetTransformFunction" ); 
		domExtraExtension = (domAny*)element; 
		sprintf (tmpName, "%s", "Default");
		domExtraExtension->setValue (tmpName);

//		element = (daeElement*)technique->createAndPlace( "AutoSleepMode" ); 
//		domExtraExtension = (domAny*)element; 
//		//autoSleepMode = NewtonBodyGetAutoFreeze (newtonBody);
//		autoSleepMode = false;
//		sprintf (tmpName, "%s", autoSleepMode ? "true" : "false");
//		domExtraExtension->setValue (tmpName);

		element = (daeElement*)technique->createAndPlace( "InternalLinearDrag" ); 
		domExtraExtension = (domAny*)element; 
		linearDragValue = NewtonBodyGetLinearDamping (newtonBody);
		sprintf (tmpName, "%f", linearDragValue);
		domExtraExtension->setValue (tmpName);

		element = (daeElement*)technique->createAndPlace( "InternalAngularDrag" ); 
		domExtraExtension = (domAny*)element; 
		NewtonBodyGetAngularDamping (newtonBody, angularDragValue);
		sprintf (tmpName, "%f", angularDragValue[0]);
		domExtraExtension->setValue (tmpName);

//		element = (daeElement*)technique->createAndPlace( "InternalGyroscopicForces" ); 
//		domExtraExtension = (domAny*)element; 
//		//gyroscopyForces = NewtonBodyGetGyroscopicForcesMode (newtonBody);
//		gyroscopyForces = false;
//		sprintf (tmpName, "%s", gyroscopyForces ? "true" : "false");
//		domExtraExtension->setValue (tmpName);

		element = (daeElement*)technique->createAndPlace( "InternalContinueCollision" ); 
		domExtraExtension = (domAny*)element; 
		continueCollision = NewtonBodyGetContinuousCollisionMode (newtonBody);
		sprintf (tmpName, "%s", continueCollision ? "true" : "false");
		domExtraExtension->setValue (tmpName);

		element = (daeElement*)technique->createAndPlace( "RecursivelyCollideWithLinkedBodies" ); 
		domExtraExtension = (domAny*)element; 
		collideWithCollision = NewtonBodyGetJointRecursiveCollision (newtonBody);
		sprintf (tmpName, "%s", collideWithCollision ? "true" : "false");
		domExtraExtension->setValue (tmpName);

		element = (daeElement*)technique->createAndPlace( "UnilateralMaterialID" ); 
		domExtraExtension = (domAny*)element; 
		materialGroudlID = NewtonBodyGetMaterialGroupID (newtonBody);
		sprintf (tmpName, "%d", materialGroudlID);
		domExtraExtension->setValue (tmpName);

		return rigidBody;
	}

	domRigid_constraint* AddRigidConstraint (
		daeDocument* document, 
		domRigid_body* sentinel,
		domPhysics_model* model, 
		const NewtonJoint* newtonJoint, 
		int enumID)
	{
		int i;
		const NewtonBody* body0;
		const NewtonBody* body1;
		domRigid_body* rigidBody0;
		domRigid_body* rigidBody1;
		domRigid_constraint* rigidConstraint;
		domRigid_constraint::domAttachment* ref0;
		domRigid_constraint::domRef_attachment* ref1;

		domRigid_constraint::domTechnique_common* techniqueCommon;
		NewtonJointRecord info;
		char tmpName[256];

		_ASSERTE (sentinel);

		NewtonJointGetInfo (newtonJoint, &info);
		rigidConstraint = daeSafeCast<domRigid_constraint>(model->createAndPlace(COLLADA_ELEMENT_RIGID_CONSTRAINT));
		sprintf (tmpName, "%s%04d", D_PHYSICS_RIGIDCONSTRAINT_NAME, enumID);
		rigidConstraint->setSid(tmpName);


//		domExtra* extra;
//		domTechnique *technique;
//		extra = daeSafeCast<domExtra>(rigidConstraint->createAndPlace(COLLADA_ELEMENT_EXTRA ));
//		technique = daeSafeCast<domTechnique>(extra->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
//		technique->setProfile(info.m_discriptionType);

		JointType jointType;

		jointType = m_generic6DOF;
		if (!stricmp (info.m_discriptionType, "gear")) {
			jointType = m_pulley;
		} else if (!stricmp (info.m_discriptionType, "pulley")) {
			jointType = m_gear;
		} else if (!stricmp (info.m_discriptionType, "wormGear")) {
			jointType = m_wormGear;
//		} else if (!stricmp (technique->getProfile(), "pulley")) {
//			jointType = m_upVetor;
//		} else if (!stricmp (technique->getProfile(), "hinge")) {
//			jointType = m_hinge;
//		} else if (!stricmp (technique->getProfile(), "slider")) {
//			jointType = m_slider;
//		} else if (!stricmp (technique->getProfile(), "corkScrew")) {
//			jointType = m_corkScrew;
//		} else if (!stricmp (technique->getProfile(), "universal")) {
//			jointType = m_universal;
//		} else if (!stricmp (technique->getProfile(), "generic6dof")) {
//			jointType = m_universal;
//		} else if (!stricmp (technique->getProfile(), "generic6dof")) {
//			jointType = m_generic6DOF;
		}

		switch (jointType) 
		{
			case m_gear:
			case m_pulley:
			case m_wormGear:
			{
				domExtra* extra;
				daeElement* element;
				domTechnique *technique;
				domAny *domExtraExtension;
				char tmpName[512];

				extra = daeSafeCast<domExtra>(rigidConstraint->createAndPlace(COLLADA_ELEMENT_EXTRA));
				technique = daeSafeCast<domTechnique>(extra->createAndPlace(COLLADA_ELEMENT_TECHNIQUE));
				technique->setProfile("Newton");

				element = (daeElement*)technique->createAndPlace( "SpecialConstraint" ); 
				domExtraExtension = (domAny*)element; 
				domExtraExtension->setValue (info.m_discriptionType);

				tmpName[0] = 0;
				for (int i = 0; i < sizeof (info.m_extraParameters) / sizeof (info.m_extraParameters[0]); i ++) {
					char text[64];
					sprintf (text, "%f ", info.m_extraParameters[i]);
					strcat (tmpName, text);
				}

				element = (daeElement*)technique->createAndPlace( "parameters" ); 
				domExtraExtension = (domAny*)element; 
				domExtraExtension->setValue (tmpName);
				break;
			}

		}


		rigidBody0 = NULL;
		ref1 = daeSafeCast<domRigid_constraint::domRef_attachment>(rigidConstraint->createAndPlace(COLLADA_ELEMENT_REF_ATTACHMENT));
		body1 = info.m_attachBody_1;
		if (body1) {
			rigidBody1 = m_bodyList.Find(body1)->GetInfo();
		} else {
			rigidBody1 = sentinel;
		}
		daeURI uri1(rigidBody1->getSid());
		ref1->setRigid_body(uri1);
		dMatrix& matrix1 = *((dMatrix*) &info.m_attachmenMatrix_1);
		AddOffssetMatrix (ref1, matrix1);
		
		rigidBody0 = NULL;
		ref0 = daeSafeCast<domRigid_constraint::domAttachment>(rigidConstraint->createAndPlace(COLLADA_ELEMENT_ATTACHMENT));
		body0 = info.m_attachBody_0;
		if (body0) {
			rigidBody0 = m_bodyList.Find(body0)->GetInfo();
		} else {
			rigidBody0 = sentinel;
		}
		daeURI uri0 (rigidBody0->getSid());
//		uri.setElement (rigidBody0);
//		uri.resolveURI();
		ref0->setRigid_body(uri0);
		dMatrix& matrix0 = *((dMatrix*) &info.m_attachmenMatrix_0);
		AddOffssetMatrix (ref0, matrix0);

		// add rigid constraint info
		techniqueCommon = daeSafeCast<domRigid_constraint::domTechnique_common>(rigidConstraint->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));
		_ASSERTE (techniqueCommon);

		if (NewtonJointGetCollisionState (newtonJoint)) {
			domRigid_constraint::domTechnique_common::domInterpenetrate* collEnable;
			collEnable = daeSafeCast<domRigid_constraint::domTechnique_common::domInterpenetrate>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_INTERPENETRATE));
			collEnable->setValue(false);
		}

		domRigid_constraint::domTechnique_common::domLimits* limits;
		limits = daeSafeCast<domRigid_constraint::domTechnique_common::domLimits>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_LIMITS));

		if (!info.m_bodiesCollisionOn) {
			domRigid_constraint::domTechnique_common::domInterpenetrate* interpenetrate;

			interpenetrate = daeSafeCast<domRigid_constraint::domTechnique_common::domInterpenetrate>(techniqueCommon->createAndPlace(COLLADA_ELEMENT_INTERPENETRATE));
			interpenetrate->setValue(true);
		}

		domTargetableFloat3* minLinear;
		domTargetableFloat3* maxLinear;
		domTargetableFloat3* minAngular;
		domTargetableFloat3* maxAngular;

		domRigid_constraint::domTechnique_common::domLimits::domLinear* linear; 
		domRigid_constraint::domTechnique_common::domLimits::domSwing_cone_and_twist* angular; 
		linear = daeSafeCast<domRigid_constraint::domTechnique_common::domLimits::domLinear>(limits->createAndPlace(COLLADA_ELEMENT_LINEAR));
		minLinear = daeSafeCast<domTargetableFloat3>(linear->createAndPlace(COLLADA_ELEMENT_MIN));
		maxLinear = daeSafeCast<domTargetableFloat3>(linear->createAndPlace(COLLADA_ELEMENT_MAX));

		angular = daeSafeCast<domRigid_constraint::domTechnique_common::domLimits::domSwing_cone_and_twist>(limits->createAndPlace(COLLADA_ELEMENT_SWING_CONE_AND_TWIST));
		minAngular = daeSafeCast<domTargetableFloat3>(angular->createAndPlace(COLLADA_ELEMENT_MIN));
		maxAngular = daeSafeCast<domTargetableFloat3>(angular->createAndPlace(COLLADA_ELEMENT_MAX));

		dVector minLinearDof (info.m_minLinearDof[0], info.m_minLinearDof[1], info.m_minLinearDof[2]);
		dVector maxLinearDof (info.m_maxLinearDof[0], info.m_maxLinearDof[1], info.m_maxLinearDof[2]);
		dVector minAngularDof (info.m_minAngularDof[0], info.m_minAngularDof[1], info.m_minAngularDof[2]);
		dVector maxAngularDof (info.m_maxAngularDof[0], info.m_maxAngularDof[1], info.m_maxAngularDof[2]);

		minLinearDof = m_axisRotation.TransformVector(minLinearDof);
		maxLinearDof = m_axisRotation.TransformVector(maxLinearDof);
		minAngularDof = m_axisRotation.TransformVector(minAngularDof);
		maxAngularDof = m_axisRotation.TransformVector(maxAngularDof);
		for (i = 0; i < 3; i ++) {
			minLinear->getValue().append (minLinearDof[i]);
			maxLinear->getValue().append (maxLinearDof[i]);
			minAngular->getValue().append (minAngularDof[i]);
			maxAngular->getValue().append (maxAngularDof[i]);
		}

		return rigidConstraint;
	}

	void AddPhysicsSceneLibrary (daeDocument* document)
	{
		daeURI uri;
		domNode* target;
		domCOLLADA *domRoot;
		dSceneNode* dVisual;
		domPhysics_scene *scene;
		domRigid_body* rigidBody;
		domRigid_constraint* rigidConstraint;
		const NewtonBody* newtonBody;
		domPhysics_model* physicsModel;
		domLibrary_physics_scenes *library;
		domLibrary_physics_models* modelLibrary;
		domInstance_physics_model* instanceModel;
		domInstance_rigid_body* instanceRigidBody;
		domInstance_rigid_constraint* instanceRigidContraint;
		
		NewtonBodyList::dTreeNode* bodyNode;
		NewtonJointList::dTreeNode* jointNode;
		dNodeToColladaNodeMap::dTreeNode* visualNode;
		domPhysics_scene::domTechnique_common* technique;
		char sceneName [128];

		// create the image library
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		sprintf (sceneName, "%s_physic", D_ROOT_NODE_NAME);

		library = daeSafeCast<domLibrary_physics_scenes>(domRoot->createAndPlace(COLLADA_ELEMENT_LIBRARY_PHYSICS_SCENES));
		scene = daeSafeCast<domPhysics_scene>(library->createAndPlace(COLLADA_ELEMENT_PHYSICS_SCENE));
		scene->setId (sceneName);

		technique = daeSafeCast<domPhysics_scene::domTechnique_common>(scene->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));

		domTargetableFloat3* gravity;
		gravity = daeSafeCast<domTargetableFloat3>(technique->createAndPlace(COLLADA_ELEMENT_GRAVITY));

		dVector g (m_axisRotation.RotateVector(dVector (dFloat (0.0f), dFloat (-10.0f), dFloat (0.0f)))); 
		gravity->getValue().append(g.m_x);
		gravity->getValue().append(g.m_y);
		gravity->getValue().append(g.m_z);

		domTargetableFloat* tiemStep;
		tiemStep = daeSafeCast<domTargetableFloat>(technique->createAndPlace(COLLADA_ELEMENT_TIME_STEP));
		tiemStep->setValue(1.0f / 60.0f);

		modelLibrary = domRoot->getLibrary_physics_models_array()[0];
		physicsModel = modelLibrary->getPhysics_model_array()[0];

		instanceModel = daeSafeCast<domInstance_physics_model>(scene->createAndPlace(COLLADA_TYPE_INSTANCE_PHYSICS_MODEL));
		uri.setElement (physicsModel);
		uri.resolveURI();
		instanceModel->setUrl (uri);

		NewtonBodyList::Iterator bodyIter (m_bodyList);
		for (bodyIter.Begin(); bodyIter; bodyIter ++) {
			domInstance_rigid_body::domTechnique_common* technique; 
			domInstance_rigid_body::domTechnique_common::domVelocity* velocity;
			domInstance_rigid_body::domTechnique_common::domAngular_velocity* angularVelocity;

			bodyNode = bodyIter.GetNode();
			rigidBody = bodyNode->GetInfo();
			newtonBody = bodyIter.GetKey();

			instanceRigidBody = daeSafeCast<domInstance_rigid_body>(instanceModel->createAndPlace(COLLADA_TYPE_INSTANCE_RIGID_BODY));
			instanceRigidBody->setBody(rigidBody->getSid());

			dVisual = (dSceneNode*) NewtonBodyGetUserData(newtonBody);
			visualNode = m_visualNodeMap.Find (dVisual);
			if (visualNode) {
				target = visualNode->GetInfo();

				uri.setElement (target);
				uri.resolveURI();
				instanceRigidBody->setTarget (uri);

				technique = daeSafeCast<domInstance_rigid_body::domTechnique_common>(instanceRigidBody->createAndPlace(COLLADA_ELEMENT_TECHNIQUE_COMMON));

				dVector veloc;
				velocity = daeSafeCast<domInstance_rigid_body::domTechnique_common::domVelocity>(technique->createAndPlace(COLLADA_ELEMENT_VELOCITY));
				NewtonBodyGetVelocity(newtonBody, &veloc.m_x);
				veloc = m_axisRotation.RotateVector(veloc); 
				velocity->getValue().append (veloc.m_x);
				velocity->getValue().append (veloc.m_y);
				velocity->getValue().append (veloc.m_z);

				dVector omega;
				angularVelocity = daeSafeCast<domInstance_rigid_body::domTechnique_common::domAngular_velocity>(technique->createAndPlace(COLLADA_ELEMENT_ANGULAR_VELOCITY));
				NewtonBodyGetOmega(newtonBody, &omega.m_x);
				omega = m_axisRotation.RotateVector(veloc); 
				angularVelocity->getValue().append (veloc.m_x);
				angularVelocity->getValue().append (veloc.m_y);
				angularVelocity->getValue().append (veloc.m_z);
			}
		}


		NewtonJointList::Iterator jointIter (m_jointList);
		for (jointIter.Begin(); jointIter; jointIter ++) {
//			daeURI uri;
			jointNode = jointIter.GetNode();
			rigidConstraint = jointNode->GetInfo();
			instanceRigidContraint = daeSafeCast<domInstance_rigid_constraint>(instanceModel->createAndPlace(COLLADA_TYPE_INSTANCE_RIGID_CONSTRAINT));
//			uri.setElement (rigidConstraint);
//			uri.resolveURI();
//			instanceRigidContraint->(uri);
			instanceRigidContraint->setConstraint(rigidConstraint->getSid());

		}
	}




	void LoadCamera(daeDocument *document)
	{
		CameraNode* camera;
		domCOLLADA *domRoot;
		domLibrary_cameras *library;
		domCamera::domOptics* optics;
		domCamera::domOptics::domTechnique_common* technique;
		domCamera::domOptics::domTechnique_common::domPerspective* perpective;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		if (domRoot->getLibrary_cameras_array().getCount()) {
			camera = (CameraNode*) m_rootNode->Find(CAMERA_NAME);
			_ASSERTE (camera);
			if (camera) {
				library = domRoot->getLibrary_cameras_array()[0];
				if (library->getCamera_array().getCount()) {
					m_collCamera = library->getCamera_array()[0];

					optics = m_collCamera->getOptics();
					if (optics) {
						technique = optics->getTechnique_common();
						if (technique) {
							perpective = technique->getPerspective();
							if (perpective) {
								domTargetableFloat* fov; 
								domTargetableFloat* aspect; 
								domTargetableFloat* farPlane;
								domTargetableFloat* nearPlane;

								fov = perpective->getYfov(); 
								if (fov) {
									camera->m_fov = dFloat (fov->getValue());
								}
								aspect = perpective->getAspect_ratio();
								if (aspect) {
									camera->m_aspect = dFloat (aspect->getValue());
								}

								farPlane = perpective->getZfar();
								if (farPlane) {
									camera->m_farPlane = dFloat (farPlane->getValue());
								}

								nearPlane = perpective->getZnear();
								if (nearPlane) {
									camera->m_nearPlane = dFloat (nearPlane->getValue());
								}
							}
						}
					}
				}
			}
		}
	}






	dGeometry* LoadExplicitConvex (daeDocument *document, domGeometry *colladaGeometry)
	{
		int j;
		int k;
		int m;
		int n;
		int index;
		int stride;
		int polyCount;
		int faceIndex;
		int vertexCount;
		int triangleCount;
		int isSimpleVertex;
		domP *p; 
		daeURI uri;
		domExtra* extra;
		domEffect* effect;
		dGeometry* geometry;
		domCOLLADA *domRoot;
		domConvex_mesh *colladaMesh;
		domMaterial* material;
		domPolygons* polygons;
		CollMaterial* collMat;
		SourceBuffer *uvSource;
		SourceBuffer *posSource;
		SourceBuffer *normalSrc;
		domTriangles* triangles;
		domTechnique *technique;
		SourceBuffer sourceBuffers[6];
		domInstance_effect* instanceEffect;
		domLibrary_effects* effectLibrary;
		domLibrary_materials *materialLibrary;

		CollMaterialCache& materialCache = *this;
		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());

		materialLibrary = domRoot->getLibrary_materials_array()[0];;
		_ASSERTE (materialLibrary);

		effectLibrary = domRoot->getLibrary_effects_array()[0];
		_ASSERTE (effectLibrary);

		memset (sourceBuffers, 0, sizeof (sourceBuffers));


		colladaMesh = colladaGeometry->getConvex_mesh();
		_ASSERTE (colladaMesh);

		geometry = new dGeometry ();
		sprintf (geometry->m_name, "%s", colladaGeometry->getId());

		isSimpleVertex = 0;
		domExtra_Array& extraArray = colladaGeometry->getExtra_array();
		if (extraArray.getCount()) {
			// if the geometry have extras, the search the Veter format option 
			for (j = 0; !isSimpleVertex && (j < int (extraArray.getCount())); j ++) {
				extra = extraArray[j];
				const domTechnique_Array& techniqueArray = extra->getTechnique_array();
				for (k = 0; !isSimpleVertex && (k < int (techniqueArray.getCount())); k ++) {
					technique = techniqueArray[k];
					if (!stricmp (technique->getProfile(), D_VERTEX_FORMAT)) {
						// if have the vertex format option this mean this is a 
						// previously newton saved file with and flat vertex format.
						isSimpleVertex = 1;
					}
				}
			}
		}

		domSource_Array &sourceArray = colladaMesh->getSource_array();
		for (j = 0; j <  int (sourceArray.getCount()); j ++) {
			domSource *source;
			domAccessor *accessor;
			domFloat_array *floatArray;
			domSource::domTechnique_common *technique;

			source = sourceArray[j];
			floatArray = source->getFloat_array();
			domListOfFloats &srcArray = floatArray->getValue();

			technique = source->getTechnique_common();
			_ASSERTE (technique);

			accessor = technique->getAccessor();
			_ASSERTE (accessor);
			sourceBuffers[j].m_id = source->getId();
			sourceBuffers[j].m_stride = accessor->getStride();
			sourceBuffers[j].m_count = floatArray->getCount();
			sourceBuffers[j].m_data = &srcArray[0];
		}

		posSource = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_POSITION, sourceBuffers);
		normalSrc = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_NORMAL, sourceBuffers);
		uvSource = FindBufferSouce(colladaMesh, COMMON_PROFILE_INPUT_TEXCOORD, sourceBuffers);
		_ASSERTE (posSource);

		if (isSimpleVertex) {
			vertexCount = int (sourceBuffers[0].m_count) / sourceBuffers[0].m_stride;
			geometry->AllocVertexData (dInt32 (vertexCount));

			for (j= 0; j < vertexCount; j ++) {
				geometry->m_vertex[j * 3 + 0] = GLfloat (posSource->m_data[j * posSource->m_stride + 0]);
				geometry->m_vertex[j * 3 + 1] = GLfloat (posSource->m_data[j * posSource->m_stride + 1]);
				geometry->m_vertex[j * 3 + 2] = GLfloat (posSource->m_data[j * posSource->m_stride + 2]);

				geometry->m_normal[j * 3 + 0] = GLfloat (normalSrc->m_data[j * normalSrc->m_stride + 0]);
				geometry->m_normal[j * 3 + 1] = GLfloat (normalSrc->m_data[j * normalSrc->m_stride + 1]);
				geometry->m_normal[j * 3 + 2] = GLfloat (normalSrc->m_data[j * normalSrc->m_stride + 2]);

				geometry->m_uv[j * 2 + 0] = GLfloat (uvSource->m_data[j * uvSource->m_stride + 0]);
				geometry->m_uv[j * 2 + 1] = GLfloat (uvSource->m_data[j * uvSource->m_stride + 1]);
			}
			m_axisRotation.TransformTriplex(geometry->m_vertex, 3 * sizeof (GLfloat), geometry->m_vertex, 3 * sizeof (GLfloat), vertexCount);
			m_axisRotation.TransformTriplex(geometry->m_normal, 3 * sizeof (GLfloat), geometry->m_normal, 3 * sizeof (GLfloat), vertexCount);


			domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
			polyCount = int (trianglesArray.getCount());
			for (j = 0; j < polyCount; j ++) {

				triangles = trianglesArray[j];

				triangleCount = int (triangles->getCount());
				geometry->Append() ;
				dGeometrySegment& segment = geometry->GetLast()->GetInfo();
				segment.AllocIndexData(dInt32 (triangleCount * 3));

				p = triangles->getP();
				domListOfUInts &indices = p->getValue();

				_ASSERTE (dInt32 (indices.getCount()) == triangleCount * 3);
				for (k = 0; k < triangleCount * 3; k ++) {
					segment.m_indexes[k] = GLushort (indices[k]);
				}

				material = FindMaterial (materialLibrary, triangles->getMaterial());
				_ASSERTE (material);

				instanceEffect = material->getInstance_effect();
				_ASSERTE (instanceEffect);
				if (instanceEffect) {
					uri = instanceEffect->getUrl();
					effect = FindEffect (effectLibrary, uri.getID());
					_ASSERTE (effect);

					collMat = materialCache.GetMaterial (effect->getId());
					if (collMat) {
						strcpy (segment.m_textureName, collMat->m_texture.m_textureName);
						strcpy (segment.m_texturePathName, collMat->m_texture.m_texturePathName);
						segment.m_texture = collMat->m_texture.m_texture;
					}
				}
			}

		} else {

			triangleCount = 0;
			domPolygons_Array &polygonArray = colladaMesh->getPolygons_array();
			polyCount = int (polygonArray.getCount());

			for (j = 0; j < polyCount; j ++) {
				polygons = polygonArray[j];
				_ASSERTE (0);
				//					polygons = colladaMesh->GetPolygons (j);
				//					_ASSERTE (polygon->GetType() == FCDGeometryPolygons::POLYGONS);
				//					faceCount = int (polygon->GetFaceCount());
				//					for (k = 0; k < faceCount; k ++) {
				//						triangleCount += int (polygon->GetFaceVertexCount(k)) - 2;
				//					}
			}

			domTriangles_Array &trianglesArray = colladaMesh->getTriangles_array();
			for (j = 0; j < int (trianglesArray.getCount()); j ++) {
				triangles = trianglesArray[j];
				triangleCount += int (triangles->getCount());
			}

			// create space to store the vertices, normals and uvs
			geometry->AllocVertexData (dInt32 (triangleCount * 3));
			for (j= 0; j < triangleCount * 3; j ++) {
				geometry->m_vertex[j * 3 + 0] = 0.0f;
				geometry->m_vertex[j * 3 + 1] = 0.0f;
				geometry->m_vertex[j * 3 + 2] = 0.0f;

				geometry->m_normal[j * 3 + 0] = 0.0f;
				geometry->m_normal[j * 3 + 1] = 1.0f;
				geometry->m_normal[j * 3 + 2] = 0.0f;

				geometry->m_uv[j * 2 + 0] = 0.0f;
				geometry->m_uv[j * 2 + 1] = 0.0f;
			}

			faceIndex = 0;
			// and triangles
			for (j = 0; j < int (trianglesArray.getCount()); j ++) {

				triangles = trianglesArray[j];
				const domInputLocalOffset_Array& inputArray = triangles->getInput_array();

				triangleCount = int (triangles->getCount());
				geometry->Append() ;
				dGeometrySegment& segment = geometry->GetLast()->GetInfo();
				segment.AllocIndexData(dInt32 (triangleCount * 3));

				p = triangles->getP();
				domListOfUInts& indices = p->getValue();
				stride = int (inputArray.getCount());

				domUint* vertexIndices;
				domUint* normalIndices;
				domUint* uvIndices;
				domVertices *vertices;


				vertexIndices = NULL;
				normalIndices = NULL;
				uvIndices = NULL;
				vertices = colladaMesh->getVertices();
				domInputLocal_Array &vertexInputArray = vertices->getInput_array();
				for (k = 0; k < int (vertexInputArray.getCount()); k ++) {
					domInputLocal* offset;
					offset = vertexInputArray[k];
					if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_POSITION)) {
						vertexIndices = &indices[0];
					} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
						normalIndices = &indices[0];
					} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
						uvIndices = &indices[0];
					}
				}
				_ASSERTE (vertexIndices);
				index = faceIndex;
				for (k = 0; k < int (indices.getCount()); k += stride) {
					segment.m_indexes[index] = index;

					m = vertexIndices[k];
					geometry->m_vertex[index * 3 + 0] = GLfloat (posSource->m_data[m * posSource->m_stride + 0]);
					geometry->m_vertex[index * 3 + 1] = GLfloat (posSource->m_data[m * posSource->m_stride + 1]);
					geometry->m_vertex[index * 3 + 2] = GLfloat (posSource->m_data[m * posSource->m_stride + 2]);

					if (normalIndices && normalSrc) {
						m = normalIndices[k];
						geometry->m_normal[index * 3 + 0] = GLfloat (normalSrc->m_data[m * normalSrc->m_stride + 0]);
						geometry->m_normal[index * 3 + 1] = GLfloat (normalSrc->m_data[m * normalSrc->m_stride + 1]);
						geometry->m_normal[index * 3 + 2] = GLfloat (normalSrc->m_data[m * normalSrc->m_stride + 2]);
					}

					if (uvIndices && normalSrc) {
						m = uvIndices[k];
						geometry->m_uv[index * 2 + 0] = GLfloat (uvSource->m_data[m * uvSource->m_stride + 0]);
						geometry->m_uv[index * 2 + 1] = GLfloat (uvSource->m_data[m * uvSource->m_stride + 1]);
					}
					index ++;
				}

				if (!(normalIndices && normalSrc)) {
					n = index;
					for (n = faceIndex; n < index; n += 3) {
						dVector p0 (geometry->m_vertex[(n + 0) * 3 + 0], geometry->m_vertex[(n + 0) * 3 + 1], geometry->m_vertex[(n + 0) * 3 + 2]);
						dVector p1 (geometry->m_vertex[(n + 1) * 3 + 0], geometry->m_vertex[(n + 1) * 3 + 1], geometry->m_vertex[(n + 1) * 3 + 2]);
						dVector p2 (geometry->m_vertex[(n + 2) * 3 + 0], geometry->m_vertex[(n + 2) * 3 + 1], geometry->m_vertex[(n + 2) * 3 + 2]);
						dVector e0 (p1 - p0);
						dVector e1 (p2 - p0);
						dVector normal (e0 * e1);
						normal = normal.Scale (1.0f / sqrtf (normal % normal));

						geometry->m_normal[(n + 0) * 3 + 0] = normal.m_x;
						geometry->m_normal[(n + 0) * 3 + 1] = normal.m_y;
						geometry->m_normal[(n + 0) * 3 + 2] = normal.m_z;

						geometry->m_normal[(n + 1) * 3 + 0] = normal.m_x;
						geometry->m_normal[(n + 1) * 3 + 1] = normal.m_y;
						geometry->m_normal[(n + 1) * 3 + 2] = normal.m_z;

						geometry->m_normal[(n + 2) * 3 + 0] = normal.m_x;
						geometry->m_normal[(n + 2) * 3 + 1] = normal.m_y;
						geometry->m_normal[(n + 2) * 3 + 2] = normal.m_z;
					}
				}


				// save all other vertex members (normal, uvs, etc)
				for (n = 1; n < stride; n ++) {
					domInputLocalOffset* offset;
					offset = inputArray[n];
					if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_NORMAL)) {
						if (normalSrc) {
							index = faceIndex;
							normalIndices = &indices[n];
							for (k = 0; k < int (indices.getCount()); k += stride) {
								m = normalIndices[k];
								geometry->m_normal[index * 3 + 0] = GLfloat (normalSrc->m_data[m * normalSrc->m_stride + 0]);
								geometry->m_normal[index * 3 + 1] = GLfloat (normalSrc->m_data[m * normalSrc->m_stride + 1]);
								geometry->m_normal[index * 3 + 2] = GLfloat (normalSrc->m_data[m * normalSrc->m_stride + 2]);
								index ++;
							}
						}
					} else if (!stricmp (offset->getSemantic(), COMMON_PROFILE_INPUT_TEXCOORD)) {
						if (normalSrc) {
							index = faceIndex;
							uvIndices = &indices[n];
							for (k = 0; k < int (indices.getCount()); k += stride) {
								m = normalIndices[k];
								geometry->m_uv[index * 2 + 0] = GLfloat (uvSource->m_data[m * uvSource->m_stride + 0]);
								geometry->m_uv[index * 2 + 1] = GLfloat (uvSource->m_data[m * uvSource->m_stride + 1]);
								index ++;
							}
						}
					}
				}

				faceIndex = index;

				material = FindMaterial (materialLibrary, triangles->getMaterial());
				if (material) {
					_ASSERTE (material);

					instanceEffect = material->getInstance_effect();
					_ASSERTE (instanceEffect);
					if (instanceEffect) {
						uri = instanceEffect->getUrl();
						effect = FindEffect (effectLibrary, uri.getID());
						_ASSERTE (effect);

						collMat = materialCache.GetMaterial (effect->getId());
						if (collMat) {
							strcpy (segment.m_textureName, collMat->m_texture.m_textureName);
							strcpy (segment.m_texturePathName, collMat->m_texture.m_texturePathName);
							segment.m_texture = collMat->m_texture.m_texture;
						}
					}
				}
			}


			for (j = 0; j < int (polygonArray.getCount()); j ++) {
				_ASSERTE (0);
				/*
				polygon = colladaMesh->GetPolygons (j);
				_ASSERTE (polygon->GetType() == FCDGeometryPolygons::POLYGONS);
				//					_ASSERTE (polygon->IsTriangles());


				faceCount = int (polygon->GetFaceCount());

				geometry->Append() ;
				dGeometrySegment& segment = geometry->GetLast()->GetInfo();

				triangleCount = 0;
				for (k = 0; k < faceCount; k ++) {
				triangleCount += (int (polygon->GetFaceVertexCount(k)) - 2);
				}
				segment.AllocIndexData(dInt32 (triangleCount) * 3);

				colladaMaterial = colladaMaterialLibrary->FindMaterial(polygon->GetMaterialSemantic());
				if (colladaMaterial) {
				effect = colladaMaterial->GetEffect();
				_ASSERTE (effect);
				if (effect) {
				collMat = materialCache.GetMaterial (effect->GetDaeId().c_str());
				if (collMat) {
				strcpy (segment.m_textureName, collMat->m_texture.m_textureName);
				strcpy (segment.m_texturePathName, collMat->m_texture.m_texturePathName);
				segment.m_texture = collMat->m_texture.m_texture;
				}
				}
				}

				const UInt32List* posIndices = polygon->FindIndices(positSource);
				const UInt32List* norIndices = normalSource ? polygon->FindIndices(normalSource) : NULL;
				const UInt32List* uvIndices = uvSource ? polygon->FindIndices(uvSource) : NULL;

				index = 0;
				triangleCount = 0;
				for (k = 0; k < faceCount; k ++) {
				int i0;
				int i1;
				int i2;

				int n0 = 0;
				int n1 = 0;
				int n2 = 0;

				int u0 = 0;
				int u1 = 0;
				int u2 = 0;


				i0 = (*posIndices)[index];
				if (normalSource) {
				n0 = (*norIndices)[index];
				}
				if (uvSource) {
				u0 = (*uvIndices)[index];
				}
				index ++;

				i1 = (*posIndices)[index];
				if (normalSource) {
				n1 = (*norIndices)[index];
				}
				if (uvSource) {
				u1 = (*uvIndices)[index];
				}
				index ++;

				for (m = 2; m < int (polygon->GetFaceVertexCount(k)); m ++) {
				segment.m_indexes[triangleCount * 3 + 0] = faceIndex * 3 + 0;
				segment.m_indexes[triangleCount * 3 + 1] = faceIndex * 3 + 1;
				segment.m_indexes[triangleCount * 3 + 2] = faceIndex * 3 + 2;
				triangleCount ++;

				i2 = (*posIndices)[index];
				geometry->m_vertex[faceIndex * 3 * 3 + 0] = (*positFloats)[i0 * vertexStride + 0];
				geometry->m_vertex[faceIndex * 3 * 3 + 1] = (*positFloats)[i0 * vertexStride + 1];
				geometry->m_vertex[faceIndex * 3 * 3 + 2] = (*positFloats)[i0 * vertexStride + 2];

				geometry->m_vertex[faceIndex * 3 * 3 + 3] = (*positFloats)[i1 * vertexStride + 0];
				geometry->m_vertex[faceIndex * 3 * 3 + 4] = (*positFloats)[i1 * vertexStride + 1];
				geometry->m_vertex[faceIndex * 3 * 3 + 5] = (*positFloats)[i1 * vertexStride + 2];

				geometry->m_vertex[faceIndex * 3 * 3 + 6] = (*positFloats)[i2 * vertexStride + 0];
				geometry->m_vertex[faceIndex * 3 * 3 + 7] = (*positFloats)[i2 * vertexStride + 1];
				geometry->m_vertex[faceIndex * 3 * 3 + 8] = (*positFloats)[i2 * vertexStride + 2];

				if (normalSource) {
				n2 = (*norIndices)[index];

				geometry->m_normal[faceIndex * 3 * 3 + 0] = (*normalFloats)[n0 * normalStride + 0];
				geometry->m_normal[faceIndex * 3 * 3 + 1] = (*normalFloats)[n0 * normalStride + 1];
				geometry->m_normal[faceIndex * 3 * 3 + 2] = (*normalFloats)[n0 * normalStride + 2];

				geometry->m_normal[faceIndex * 3 * 3 + 3] = (*normalFloats)[n1 * normalStride + 0];
				geometry->m_normal[faceIndex * 3 * 3 + 4] = (*normalFloats)[n1 * normalStride + 1];
				geometry->m_normal[faceIndex * 3 * 3 + 5] = (*normalFloats)[n1 * normalStride + 2];

				geometry->m_normal[faceIndex * 3 * 3 + 6] = (*normalFloats)[n2 * normalStride + 0];
				geometry->m_normal[faceIndex * 3 * 3 + 7] = (*normalFloats)[n2 * normalStride + 1];
				geometry->m_normal[faceIndex * 3 * 3 + 8] = (*normalFloats)[n2 * normalStride + 2];
				}

				if (uvSource) {
				u2 = (*uvIndices)[index];

				geometry->m_uv[faceIndex * 3 * 2 + 0] = (*uvFloats)[u0 * uvStride + 0];
				geometry->m_uv[faceIndex * 3 * 2 + 1] = (*uvFloats)[u0 * uvStride + 1];

				geometry->m_uv[faceIndex * 3 * 2 + 2] = (*uvFloats)[u1 * uvStride + 0];
				geometry->m_uv[faceIndex * 3 * 2 + 3] = (*uvFloats)[u1 * uvStride + 1];

				geometry->m_uv[faceIndex * 3 * 2 + 4] = (*uvFloats)[u2 * uvStride + 0];
				geometry->m_uv[faceIndex * 3 * 2 + 5] = (*uvFloats)[u2 * uvStride + 1];
				}

				i1 = i2;
				n1 = n2;
				u1 = u2;

				index ++;
				faceIndex ++;
				}
				}
				*/
			}

			m_axisRotation.TransformTriplex(geometry->m_vertex, 3 * sizeof (GLfloat), geometry->m_vertex, 3 * sizeof (GLfloat), faceIndex);
			m_axisRotation.TransformTriplex(geometry->m_normal, 3 * sizeof (GLfloat), geometry->m_normal, 3 * sizeof (GLfloat), faceIndex);
		}

		return geometry;
	}









	

	NewtonCollision* CreateCollision(const domRigid_body::domTechnique_common::domShape_Array& shapeArray, int isDynamics)
	{
		int i;
		int count;
		NewtonCollision* collision;
		NewtonCollision* collsionArray[256];

		domRigid_body::domTechnique_common::domShape* shape;

		collision = NULL;
		count = int (shapeArray.getCount());
		if (count == 1)	{
			shape = shapeArray[0];
			collision = CreateCollision(shape);
		} else {
			if (isDynamics) {
				_ASSERTE (count < sizeof (collsionArray) / sizeof (collsionArray[0]));
				for (i = 0; i < count; i ++) {
					shape = shapeArray[i];
					collsionArray[i] = CreateCollision(shape);
				}
				collision = NewtonCreateCompoundCollision (m_world, count, collsionArray);
				for (i = 0; i < count; i ++) {
					NewtonReleaseCollision (m_world, collsionArray[i]);
				}
			} else {
				_ASSERTE (0);
				shape = shapeArray[0];
				collision = CreateCollision(shape);
			}
		}
		return collision;
	}

	void UnapplyMatrixAligment (dMatrix& matrix)
	{
		matrix = dRollMatrix(0.5f * 3.14159265f) * matrix;
	}


	NewtonCollision* CreateCollision(domRigid_body::domTechnique_common::domShape* shape)
	{
		NewtonCollision* collision;
		collision = NULL;

		dMatrix matrix (GetOffssetMatrix (shape->getTranslate_array(), shape->getRotate_array()));
		

		if (shape->getBox()) {
			domBox* box;
			domBox::domHalf_extents* halfExtend;
			box = shape->getBox(); 
			halfExtend = box->getHalf_extents(); 
			dVector size (halfExtend->getValue()[0], halfExtend->getValue()[1], halfExtend->getValue()[2], 0.0f);
			size = m_axisRotation.RotateVector (size.Scale (2.0f));
			collision = NewtonCreateBox (m_world, size.m_x, size.m_y, size.m_z, &matrix[0][0]);

		} else if (shape->getSphere()) {
			dFloat radius;
			domSphere* sphere;

			sphere = shape->getSphere(); 
			radius = sphere->getRadius()->getValue();
			collision = NewtonCreateSphere (m_world, radius, radius, radius, &matrix[0][0]);

		} else if (shape->getCylinder()) {
			int isChampher;
			dFloat height;
			dFloat radius;
			domExtra* extra;
			domCylinder* cylinder;
			domTechnique *extraTechnique;

			isChampher = 0;
			cylinder = shape->getCylinder();
			if (cylinder->getExtra_array().getCount()) {
				extra = cylinder->getExtra_array()[0];
				if (extra->getTechnique_array().getCount()) {
					extraTechnique = extra->getTechnique_array()[0];
					if (!stricmp (extraTechnique->getProfile(), "chamferCylinder")) {
						isChampher = 1;
					}
				}
			}

			height = cylinder->getHeight()->getValue() * 2.0f; 
			radius = cylinder->getRadius()->getValue()[0];

			UnapplyMatrixAligment (matrix);
			if (!isChampher) {
				collision = NewtonCreateCylinder (m_world, radius, height, &matrix[0][0]);
			} else {
				collision = NewtonCreateChamferCylinder (m_world, radius, height, &matrix[0][0]);
			}

		} else if (shape->getCapsule()) {
			dFloat radius;
			dFloat height;
			domCapsule* capsule;
			
			capsule = shape->getCapsule(); 
			radius = capsule->getRadius()->getValue()[0]; 
			height = (capsule->getHeight()->getValue() + radius) * 2.0f;

			UnapplyMatrixAligment (matrix);
			collision = NewtonCreateCapsule (m_world, radius, height, &matrix[0][0]);

		} else if (shape->getTapered_cylinder()) {
			dFloat height;
			dFloat radius1;
			dFloat radius2;
			domTapered_cylinder* cone;

			cone = shape->getTapered_cylinder();
			height = cone->getHeight()->getValue() * 2.0f;
			radius1 = cone->getRadius1()->getValue()[0];
			radius2 = cone->getRadius2()->getValue()[0];
			if (radius1 >= radius2) {
				UnapplyMatrixAligment (matrix);
				collision = NewtonCreateCone (m_world, radius1, height, &matrix[0][0]);
			} else {

				UnapplyMatrixAligment (matrix);
				collision = NewtonCreateCone (m_world, radius2, height, &matrix[0][0]);
			}

		} else if (shape->getInstance_geometry()) {
			dGeometry* geometry;
			domGeometry *collGeometry;
			const daeElement* element;
			domInstance_geometry* instance;

			instance = shape->getInstance_geometry(); 

			const daeURI& uri = instance->getUrl();
			element = uri.getElement();
			collGeometry = (domGeometry*) element;
			if (collGeometry->getConvex_mesh()) {
				geometry = m_geometryCache.Find(collGeometry)->GetInfo();	
				collision = NewtonCreateConvexHull (m_world, geometry->m_vertexCount, geometry->m_vertex, 3 * sizeof (dFloat), 0.0f, &matrix[0][0]);
			} else {
				int i;
				int j;
				dVector face[3];
				
				dList<dGeometrySegment>::dListNode* node;  
				geometry = m_geometryCache.Find(collGeometry)->GetInfo();	

				collision = NewtonCreateTreeCollision (m_world, NULL);
				NewtonTreeCollisionBeginBuild (collision);
				for (node = geometry->GetFirst(); node; node = node->GetNext()) {
					const dGeometrySegment& segment = node->GetInfo();
					for (j = 0; j < segment.m_indexCount; j += 3) {
						i = segment.m_indexes[j] * 3;
						face[0].m_x = geometry->m_vertex[i + 0];
						face[0].m_y = geometry->m_vertex[i + 1];
						face[0].m_z = geometry->m_vertex[i + 2];

						i = segment.m_indexes[j + 1] * 3;
						face[1].m_x = geometry->m_vertex[i + 0];
						face[1].m_y = geometry->m_vertex[i + 1];
						face[1].m_z = geometry->m_vertex[i + 2];

						i = segment.m_indexes[j + 2] * 3;
						face[2].m_x = geometry->m_vertex[i + 0];
						face[2].m_y = geometry->m_vertex[i + 1];
						face[2].m_z = geometry->m_vertex[i + 2];
						NewtonTreeCollisionAddFace (collision, 3, &face[0].m_x, sizeof (dVector), 0);
					}
				}
				NewtonTreeCollisionEndBuild (collision, 1);	
			}
		} else if (shape->getPlane()) {
			domPlane* plane;
			domPlane::domEquation* equation;

			plane = shape->getPlane(); 
			equation = plane->getEquation(); 

			dVector surface (equation->getValue()[0], equation->getValue()[1], equation->getValue()[2], equation->getValue()[2]);
			surface = m_axisRotation.TransformPlane(surface);
			surface = matrix.TransformPlane (surface);
			collision = CreatePlaneCollidion (m_world, surface );
		} else {
			_ASSERTE (0);
		}
		return collision;
	}

	void LoadRigidBody (domInstance_rigid_body* instanceRigidBody)
	{
		int i;
		int j;
		int k;
		int isDynamicsBody;
		dFloat Ixx;
		dFloat Iyy;
		dFloat Izz;
		dFloat mass;
		dVector com;
		domNode* target;
		daeElement* element;
		NewtonBody* body;
		dSceneNode* viualNodePtr;
		NewtonCollision* collision;
		domRigid_body* rigidBody;
		domTargetableFloat* bodyMass;
		domTargetableFloat3* bodyInertia;
		dNodePhysicsRigidBody::dTreeNode* rigidBodyNode;
		ColladaNodeTodSceneNodeMap::dTreeNode* visualNode;
		domRigid_body::domTechnique_common* techniqueCommon;
		domRigid_body::domTechnique_common::domDynamic* dynamicBody;
		domRigid_body::domTechnique_common::domMass_frame* massFrame;

		element = instanceRigidBody->getTarget().getElement();
		target = (domNode*) element;

		visualNode = m_nodeTodNodeMap.Find(target);
//		_ASSERTE (visualNode);

		rigidBodyNode = m_rigidBodyCache.Find (dCRC (instanceRigidBody->getBody()));
		_ASSERTE (rigidBodyNode);

		rigidBody = rigidBodyNode->GetInfo();

		techniqueCommon = rigidBody->getTechnique_common();
		_ASSERTE (techniqueCommon);


		isDynamicsBody = 1;
		if (techniqueCommon->getMass()) {
			if (techniqueCommon->getMass()->getValue() == 0.0f) {
				isDynamicsBody = 0;
			}
		}

		if (isDynamicsBody && techniqueCommon->getDynamic()) {
			if (techniqueCommon->getDynamic()->getValue() == false) {
				isDynamicsBody = 0;
			}
		}

		collision = CreateCollision(techniqueCommon->getShape_array(), isDynamicsBody);
		body = NewtonCreateBody (m_world, collision);
		NewtonReleaseCollision (m_world, collision);

		m_rigidBodyMap.Insert(body, rigidBody);

		// set user data;
		NewtonBodySetUserData (body, visualNode ? visualNode->GetInfo() : NULL);

		NewtonBodySetTransformCallback (body, DefaultColladaPhysicsSetTransform);
		NewtonBodySetForceAndTorqueCallback (body, DefaultColladaApplyGravityForce);


		// by default disable collision with jointed objects
		NewtonBodySetJointRecursiveCollision (body, 0);
	
		mass = 0.0f;
		Ixx = 0.0f;
		Iyy = 0.0f;
		Izz = 0.0f;
		bodyMass = techniqueCommon->getMass();
		if (bodyMass) {
			mass = bodyMass->getValue();
			dynamicBody = techniqueCommon->getDynamic();
			if (dynamicBody) {
				if (dynamicBody->getValue() == false) {
					mass = 0.0f;
				}
			}
			if (mass > 0.0f) {
				massFrame = techniqueCommon->getMass_frame();
				bodyInertia = techniqueCommon->getInertia();
				if (massFrame && bodyInertia) {
					Ixx = bodyInertia->getValue()[0];
					Iyy = bodyInertia->getValue()[1];
					Izz = bodyInertia->getValue()[2];
					com.m_x = 0.0f;
					com.m_y = 0.0f;
					com.m_z = 0.0f;
					com.m_w = 0.0f;

					
					if (massFrame->getTranslate_array().getCount()) {
						domTranslate* translation;
						translation = massFrame->getTranslate_array()[0];
						
						com.m_x = translation->getValue()[0];
						com.m_y = translation->getValue()[0];
						com.m_z = translation->getValue()[0];
						com = m_axisRotation.RotateVector(com);
					}
				} else {
					dVector inertia;
					NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &com[0]);	
					Ixx = inertia.m_x * mass;
					Iyy = inertia.m_y * mass;
					Izz = inertia.m_z * mass;
				}
			}
		}

		if (mass > 0.0f) {
			NewtonBodySetMassMatrix (body, mass, Ixx, Iyy, Izz);
			NewtonBodySetCentreOfMass (body, &com[0]);
		}

		dMatrix matrix (GetIdentityMatrix());
		if (visualNode) {
			for (viualNodePtr = visualNode->GetInfo(); viualNodePtr; viualNodePtr = viualNodePtr->GetParent()) {
				matrix = matrix * viualNodePtr->GetMatrix();
			}
			NewtonBodySetMatrix (body, &matrix[0][0]);
		} else {
			matrix.m_posit.m_y = - 1000.0f;
		}

		// set a destructor for this rigid body
		NewtonBodySetDestructorCallback (body, PhysicsBodyDestructor);

		// all body have default Material ID		
		NewtonBodySetMaterialGroupID (body, colladaMaterialIndex);


		// add a moronic callada style material;
		if (visualNode) {
			ColladaFrictionRestituionMaterial* frictionRestituionMaterial;
			frictionRestituionMaterial = new ColladaFrictionRestituionMaterial;
			visualNode->GetInfo()->SetUserData (frictionRestituionMaterial);

			// material for this body
			domPhysics_material* physicMaterial;
			domInstance_physics_material* intanceMaterial;
			intanceMaterial = techniqueCommon->getInstance_physics_material();
			if (intanceMaterial) {
				const daeElement* element;
				domPhysics_material::domTechnique_common* technique;
				const daeURI& uri = intanceMaterial->getUrl();

				element = uri.getElement();
				if (element) {
					physicMaterial = (domPhysics_material*) element;
					technique = physicMaterial->getTechnique_common();
					frictionRestituionMaterial->m_staticFriction = technique->getStatic_friction()->getValue();
					frictionRestituionMaterial->m_restitution = technique->getRestitution()->getValue();
					frictionRestituionMaterial->m_dynamicFriction = technique->getDynamic_friction()->getValue();
				}
			}
		}


		domExtra* extra;
		domTechnique *technique = NULL;

		technique = NULL;
		domExtra_Array& extraArray = rigidBody->getExtra_array();
		if (extraArray.getCount()) {
			// if the geometry have extras, the search the Veter format option 
			for (j = 0; j < int (extraArray.getCount()); j ++) {
				extra = extraArray[j];
				const domTechnique_Array& techniqueArray = extra->getTechnique_array();
				for (k = 0; k < int (techniqueArray.getCount()); k ++) {
					domTechnique *tmptechnique;
					tmptechnique = techniqueArray[k];
					if (!stricmp (tmptechnique->getProfile(), "NEWTON")) {
						technique = tmptechnique;

						const daeElementRefArray& array = technique->getContents();
						for (i = 0; i < int (array.getCount()); i ++) {
							domAny *domExtraExtension;
							element = array[i];
							domExtraExtension = (domAny*)element; 
							if (!stricmp (domExtraExtension->getElementName(), "BindingExternalForceFunction")) {

							} else if (!stricmp (domExtraExtension->getElementName(), "BindingSetTransformFunction")) {


//							} else if (!stricmp (domExtraExtension->getElementName(), "AutoSleepMode")) {
//								if (!stricmp (domExtraExtension->getValue(), "true")) {
//									NewtonBodySetAutoFreeze (body, 1);
//								} else {
//									NewtonBodySetAutoFreeze (body, 0);
//								}

							} else if (!stricmp (domExtraExtension->getElementName(), "InternalLinearDrag")) {
								dFloat drag;
								sscanf (domExtraExtension->getValue(), "%f", &drag);
								NewtonBodySetLinearDamping(body, drag);

							} else if (!stricmp (domExtraExtension->getElementName(), "InternalAngularDrag")) {
								dFloat drag[4];
								sscanf (domExtraExtension->getValue(), "%f", &drag[0]);
								drag[1] = drag[0];
								drag[2] = drag[0];
								NewtonBodySetAngularDamping (body, &drag[0]);

							} else if (!stricmp (domExtraExtension->getElementName(), "InternalGyroscopicForces")) {
								if (!stricmp (domExtraExtension->getValue(), "true")) {
									//NewtonBodySetGyroscopicForcesMode(body, 1);
								} else {
									//NewtonBodySetGyroscopicForcesMode (body, 0);
								}

							} else if (!stricmp (domExtraExtension->getElementName(), "InternalContinueCollision")) {
								if (!stricmp (domExtraExtension->getValue(), "true")) {
									NewtonBodySetContinuousCollisionMode(body, 1);
								} else {
									NewtonBodySetContinuousCollisionMode (body, 0);
								}

							} else if (!stricmp (domExtraExtension->getElementName(), "RecursivelyCollideWithLinkedBodies")) {
								if (!stricmp (domExtraExtension->getValue(), "true")) {
									NewtonBodySetJointRecursiveCollision(body, 1);
								} else {
									NewtonBodySetJointRecursiveCollision (body, 0);
								}

							} else if (!stricmp (domExtraExtension->getElementName(), "UnilateralMaterialID")) {
								int materiaID;
								sscanf (domExtraExtension->getValue(), "%d", &materiaID);

								// for now since collada do not support realistic materials 
								materiaID = 0;
								NewtonBodySetMaterialGroupID (body, materiaID);
							}
						}

						break;
					}
				}
			}
		}
	}

	void LoadRigidConstraint (domRigid_constraint* rigidConstraint)
	{

		int j;
		int k;
		int interpenetrate;
		dFloat gearRatio;
		domExtra* extra;
		NewtonBody* body0;
		NewtonBody* body1;
		NewtonCustomJoint* customJoint;
		JointType jointType;
		domRigid_body* rigidBody0;
		domRigid_body* rigidBody1;
		domRigid_constraint::domAttachment* ref0;
		domRigid_constraint::domRef_attachment* ref1;
		domRigid_constraint::domTechnique_common* techniqueCommon;
		domRigid_constraint::domTechnique_common::domLimits* limits;
		
		gearRatio = 1.0f;
		jointType = m_generic6DOF;
		domExtra_Array& extraArray = rigidConstraint->getExtra_array();
		if (extraArray.getCount()) {
			for (j = 0; j < int (extraArray.getCount()); j ++) {
				extra = extraArray[j];
				const domTechnique_Array& techniqueArray = extra->getTechnique_array();
				for (k = 0; k < int (techniqueArray.getCount()); k ++) {
					domTechnique *technique;
					technique = techniqueArray[k];

					if (!stricmp (technique->getProfile(), "newton")) {
						daeElement* element;
						domAny *domExtraExtension;
//						char tmpName[512];


// createAndPlace( "SpecialConstraint" ); 
						const daeElementRefArray& array = technique->getContents();
						element = array[0];

						domExtraExtension = (domAny*)element; 
						if (!stricmp (domExtraExtension->getValue(), "gear")) {
							jointType = m_gear;
							element = array[1];
							domExtraExtension = (domAny*)element; 
							sscanf (domExtraExtension->getValue(), "%f", &gearRatio);

						} else if (!stricmp (domExtraExtension->getValue(), "pulley")) {
							jointType = m_pulley;
							element = array[1];
							domExtraExtension = (domAny*)element; 
							sscanf (domExtraExtension->getValue(), "%f", &gearRatio);

						} else if (!stricmp (domExtraExtension->getValue(), "wormGear")) {
							jointType = m_wormGear;
							element = array[1];
							domExtraExtension = (domAny*)element; 
							sscanf (domExtraExtension->getValue(), "%f", &gearRatio);


						} else if (!stricmp (domExtraExtension->getValue(), "upVector")) {
							jointType = m_upVetor;

						} else if (!stricmp (domExtraExtension->getValue(), "ballsocket")) {
							jointType = m_ballsocket;

						} else if (!stricmp (domExtraExtension->getValue(), "hinge")) {
							jointType = m_hinge;

						} else if (!stricmp (domExtraExtension->getValue(), "slider")) {
							jointType = m_slider;

						} else if (!stricmp (domExtraExtension->getValue(), "corkScrew")) {
							jointType = m_corkScrew;

						} else if (!stricmp (domExtraExtension->getValue(), "universal")) {
							jointType = m_universal;

						} else if (!stricmp (domExtraExtension->getValue(), "generic6dof")) {
							jointType = m_universal;

						} else if (!stricmp (domExtraExtension->getValue(), "generic6dof")) {
							jointType = m_generic6DOF;
						} else {
	//						_ASSERTE (0);
						}

					}
				}
			}
		}

		ref0 = rigidConstraint->getAttachment();
		ref1 = rigidConstraint->getRef_attachment();
		dMatrix localMatrix0 (GetOffssetMatrix (ref0->getTranslate_array(), ref0->getRotate_array()));
		dMatrix localMatrix1 (GetOffssetMatrix (ref1->getTranslate_array(), ref1->getRotate_array()));

		const daeURI& uri0(ref0->getRigid_body());
		const daeURI& uri1(ref1->getRigid_body());
		rigidBody0 = m_rigidBodyCache.Find (dCRC (uri0.getOriginalURI()))->GetInfo();
		rigidBody1 = m_rigidBodyCache.Find (dCRC (uri1.getOriginalURI()))->GetInfo();

		body0 = m_rigidBodyMap.Find(rigidBody0)->GetInfo();
		body1 = m_rigidBodyMap.Find(rigidBody1)->GetInfo();

		techniqueCommon = rigidConstraint->getTechnique_common();
		_ASSERTE (techniqueCommon);
		
		limits = techniqueCommon->getLimits();
		_ASSERTE (limits);

		dVector minLinearLimits;
		dVector maxLinearLimits;
		dVector minAngularLimits;
		dVector maxAngularLimits;
		for (j = 0; j < 3; j ++) {
			minLinearLimits[j] = limits->getLinear()->getMin()->getValue()[j];
			maxLinearLimits[j] = limits->getLinear()->getMax()->getValue()[j];
			minAngularLimits[j] = limits->getSwing_cone_and_twist()->getMin()->getValue()[j];
			maxAngularLimits[j] = limits->getSwing_cone_and_twist()->getMax()->getValue()[j];
		}

		minLinearLimits = m_axisRotation.RotateVector(minLinearLimits);
		maxLinearLimits = m_axisRotation.RotateVector(maxLinearLimits);
		minAngularLimits = m_axisRotation.RotateVector(minAngularLimits.Scale (3.14159265f / 180.0f));
		maxAngularLimits = m_axisRotation.RotateVector(maxAngularLimits.Scale (3.14159265f / 180.0f));

		customJoint = NULL;
		switch (jointType)
		{
			case m_gear:
			{
				dMatrix matrix0;
				dMatrix matrix1;
				CustomGear* joint;
				NewtonBodyGetMatrix (body0, &matrix0[0][0]);
				NewtonBodyGetMatrix (body1, &matrix1[0][0]);
				matrix0 =  localMatrix0 * matrix0;
				matrix1 =  localMatrix1 * matrix1;
				joint = new CustomGear(gearRatio, matrix0.m_front, matrix1.m_front, body0, body1); 
				customJoint = joint;
				break;
			}

			case m_pulley:
			{
				dMatrix matrix0;
				dMatrix matrix1;
				CustomPulley* joint;
				NewtonBodyGetMatrix (body0, &matrix0[0][0]);
				NewtonBodyGetMatrix (body1, &matrix1[0][0]);
				matrix0 =  localMatrix0 * matrix0;
				matrix1 =  localMatrix1 * matrix1;
				joint = new CustomPulley(gearRatio, matrix0.m_front, matrix1.m_front, body0, body1); 
				customJoint = joint;
				break;
			}

			case m_wormGear:
			{
				dMatrix matrix0;
				dMatrix matrix1;
				CustomWormGear* joint;
				NewtonBodyGetMatrix (body0, &matrix0[0][0]);
				NewtonBodyGetMatrix (body1, &matrix1[0][0]);
				matrix0 =  localMatrix0 * matrix0;
				matrix1 =  localMatrix1 * matrix1;
				joint = new CustomWormGear(gearRatio, matrix0.m_front, matrix1.m_front, body0, body1); 
				customJoint = joint;
				break;
			}


			case m_upVetor:
			{
				dMatrix matrix;
				CustomUpVector* joint;
				NewtonBodyGetMatrix (body0, &matrix[0][0]);
				matrix =  localMatrix0 * matrix;
				joint = new CustomUpVector (matrix.m_front, body0); 
				customJoint = joint;
				break;
			}

			case m_ballsocket:
			{
				dMatrix matrix;
				CustomBallAndSocket* joint;
				NewtonBodyGetMatrix (body0, &matrix[0][0]);
				matrix =  localMatrix0 * matrix;
				joint = new CustomBallAndSocket (matrix, body0, body1); 
				customJoint = joint;
				break;
			}

			case m_hinge:
			{
				dMatrix matrix;
				CustomHinge* joint;
				NewtonBodyGetMatrix (body0, &matrix[0][0]);
				matrix =  localMatrix0 * matrix;
				joint = new CustomHinge (matrix, body0, body1); 
				if ((minAngularLimits[0] > -1000.0f) || (maxAngularLimits[0] < 1000.0f)) {
					 joint->EnableLimits(true);
					 joint->SetLimis (minAngularLimits[0], maxAngularLimits[0]);
				}
				customJoint = joint;
				break;
			}

			case m_slider:
			{
				dMatrix matrix;
				CustomSlider* joint;
				NewtonBodyGetMatrix (body0, &matrix[0][0]);
				matrix =  localMatrix0 * matrix;
				joint = new CustomSlider (matrix, body0, body1); 
				if ((minLinearLimits[0] > -1000.0f) || (maxLinearLimits[0] < 1000.0f)) {
					joint->EnableLimits(true);
					joint->SetLimis (minLinearLimits[0], maxLinearLimits[0]);
				}
				customJoint = joint;
				break;
			}

			case m_corkScrew:
			{
				dMatrix matrix;
				CustomCorkScrew* joint;
				NewtonBodyGetMatrix (body0, &matrix[0][0]);
				matrix =  localMatrix0 * matrix;
				joint = new CustomCorkScrew (matrix, body0, body1); 
				if ((minLinearLimits[0] > -1000.0f) || (maxLinearLimits[0] < 1000.0f)) {
					joint->EnableLinearLimits(true);
					joint->SetLinearLimis (minLinearLimits[0], maxLinearLimits[0]);
				}

				if ((minAngularLimits[0] > -1000.0f) || (maxAngularLimits[0] < 1000.0f)) {
					joint->EnableAngularLimits(true);
					joint->SetAngularLimis (minAngularLimits[0], maxAngularLimits[0]);
				}
				customJoint = joint;
				break;
			}

			case m_universal:
			{
				dMatrix matrix0;
				dMatrix matrix1;
				CustomUniversal* joint;
				NewtonBodyGetMatrix (body0, &matrix0[0][0]);
				NewtonBodyGetMatrix (body1, &matrix1[0][0]);
				matrix0 =  localMatrix0 * matrix0;
				matrix1 =  localMatrix1 * matrix1;
				matrix0.m_up = matrix1.m_up; 
				matrix0.m_right = matrix0.m_front * matrix0.m_up;
				joint = new CustomUniversal (matrix0, body0, body1); 

				if ((minAngularLimits[0] > -1000.0f) || (maxAngularLimits[0] < 1000.0f)) {
					joint->EnableLimit_0(true);
					joint->SetLimis_0 (minAngularLimits[0], maxAngularLimits[0]);
				}

				if ((minAngularLimits[1] > -1000.0f) || (maxAngularLimits[1] < 1000.0f)) {
					joint->EnableLimit_1(true);
					joint->SetLimis_1 (minAngularLimits[1], maxAngularLimits[1]);
				}
				customJoint = joint;
				break;
			}

			case m_generic6DOF:
			{
				dMatrix matrix0;
				dMatrix matrix1;
				Custom6DOF* joint;
				NewtonBodyGetMatrix (body0, &matrix0[0][0]);
				NewtonBodyGetMatrix (body1, &matrix1[0][0]);
				matrix0 = localMatrix0 * matrix0;
				matrix1 = localMatrix1 * matrix1;
				joint = new Custom6DOF (matrix0, matrix1, 
										minLinearLimits, maxLinearLimits,
										minAngularLimits, maxAngularLimits,
										body0, body1); 
				customJoint = joint;
				break;
			}
		}

		if (customJoint) {
			interpenetrate = 0;
			if (techniqueCommon->getInterpenetrate()) {
				interpenetrate = techniqueCommon->getInterpenetrate()->getValue() ? 1 : 0;
			}

			_ASSERTE (customJoint);
			customJoint->SetBodiesCollisionState (!interpenetrate);
		}

	}

	void LoadPhysicsModelInstance (domInstance_physics_model* instancePhysicsModel, const dTree<domRigid_constraint*, unsigned>& constraintCache)
	{
		int m;
		int physicsRigidBodyCount;
		int physicsRigidConstraintCount;
		domRigid_constraint* rigidConstraint;
		domInstance_rigid_body* instanceRigidBody; 
		domInstance_rigid_constraint* instanceRigidConstraint; 

		// load individual rigid bodies instances
		const domInstance_rigid_body_Array& instanceRigidBodyArray = instancePhysicsModel->getInstance_rigid_body_array();
		physicsRigidBodyCount = int (instanceRigidBodyArray.getCount());
		for (m = 0 ; m < physicsRigidBodyCount; m ++) {
			instanceRigidBody = instanceRigidBodyArray[m];
			LoadRigidBody (instanceRigidBody);
		}

		// load all joints
		const domInstance_rigid_constraint_Array& instanceRigidConstraintArray = instancePhysicsModel->getInstance_rigid_constraint_array();
		physicsRigidConstraintCount = int (instanceRigidConstraintArray.getCount());
		for (m = 0 ; m < physicsRigidConstraintCount; m ++) {
			instanceRigidConstraint = instanceRigidConstraintArray[m];
			rigidConstraint = constraintCache.Find(dCRC (instanceRigidConstraint->getConstraint()))->GetInfo();
			LoadRigidConstraint (rigidConstraint);
		}
	}



	void LoadPhysicsScene(daeDocument* document)
	{
		int i;
		int j;
		int k;
		int m;
		int bodyCount; 
		int jointCount; 
		int modelCount; 
		int physicsSceneCount;
		int physicsModelsCount;
		int libraryPhysicsSceneCount;
		domCOLLADA *domRoot;
		domPhysics_model* model;
		domRigid_body* rigidBody;
		const daeElement* element;
		domPhysics_scene* physicsScene;
		domRigid_constraint* rigidConstraint;
		domLibrary_physics_models* modelLibrary;
		domLibrary_physics_scenes* libraryPhysicsScene;
		domInstance_physics_model* instancePhysicsModel; 
		dTree<domRigid_constraint*, unsigned> constraintCache;

		domRoot = daeSafeCast<domCOLLADA>(document->getDomRoot());
		_ASSERTE (domRoot);
		physicsModelsCount = int (domRoot->getLibrary_physics_models_array().getCount());
		for (i = 0; i < physicsModelsCount; i ++ ) {
			modelLibrary = domRoot->getLibrary_physics_models_array()[0];
			const domPhysics_model_Array &modelArray = modelLibrary->getPhysics_model_array();
			modelCount = int (modelArray.getCount());
			for (j = 0; j < modelCount; j ++) {
				model = modelArray[j];

				const domRigid_body_Array &bodyArray = model->getRigid_body_array();
				bodyCount = int (bodyArray.getCount());
				for (k = 0; k < bodyCount; k ++) {
					rigidBody = bodyArray[k];
					m_rigidBodyCache.Insert(rigidBody, dCRC (rigidBody->getSid()));
				}

				const domRigid_constraint_Array &jointArray = model->getRigid_constraint_array();
				jointCount = int (jointArray.getCount());
				for (k = 0; k < jointCount; k ++) {
					rigidConstraint = jointArray[k];
					constraintCache.Insert(rigidConstraint, dCRC (rigidConstraint->getSid()));
				}
			}
		}

		const domLibrary_physics_scenes_Array &libraryPhysicsScenesArray = domRoot->getLibrary_physics_scenes_array();
		libraryPhysicsSceneCount = int (libraryPhysicsScenesArray.getCount());
		for (i = 0; i < libraryPhysicsSceneCount; i ++ ) {
			libraryPhysicsScene = libraryPhysicsScenesArray[i];
			const domPhysics_scene_Array& physicsSceneArray = libraryPhysicsScene->getPhysics_scene_array();
			physicsSceneCount = int (physicsSceneArray.getCount()); 
			for (j = 0 ; j < physicsSceneCount; j ++) {
				physicsScene = physicsSceneArray[j];
				domInstance_physics_model_Array& instanceModelArray = physicsScene->getInstance_physics_model_array();
				physicsModelsCount = int (instanceModelArray.getCount());
				for (k = 0 ; k < physicsModelsCount; k ++) {
					instancePhysicsModel = instanceModelArray[k];
					LoadPhysicsModelInstance (instancePhysicsModel, constraintCache);
					
					const daeURI& uri = instancePhysicsModel->getUrl();
					element = uri.getElement();
					if (element) {
						model = (domPhysics_model*) element;

						//if this is a model instance then it will have some other models
						const domInstance_physics_model_Array& instanceModelsArray = model->getInstance_physics_model_array();
						if (instanceModelsArray .getCount()) {
							modelCount = int (instanceModelsArray.getCount());
							for (m = 0 ; m < modelCount; m ++) {
								LoadPhysicsModelInstance (instanceModelsArray[m], constraintCache);
							}
						}
					}
				}
			}
		}
	}

	void ReplaceVisulaMeshWithCollisionProxis()
	{
		int proxiNumber;
		NewtonBody* body;		
		dGeometry* proxiMesh;
		dSceneNode* visualMesh;
		NewtonCollision* collision;
		dTree<dGeometry*,  NewtonCollision*> collisionCache;

		proxiNumber = 0;
		dRigidBodyMap::Iterator iter (m_rigidBodyMap);
		for (iter.Begin(); iter; iter ++) {
			body = iter.GetNode()->GetInfo();
			collision = NewtonBodyGetCollision(body);
			if (!collisionCache.Find(collision)) {
				proxiMesh = dGeometry::MakeMeshFromCollision (m_world, collision, NULL);
				collisionCache.Insert(proxiMesh, collision);
				sprintf (proxiMesh->m_name, "proxi%04d", proxiNumber);
				proxiNumber ++;
			}
			proxiMesh = collisionCache.Find(collision)->GetInfo();
			if (proxiMesh->m_vertexCount == 0) {
				proxiMesh = NULL;
			}
			visualMesh = (RenderPrimitive*) NewtonBodyGetUserData (body);
			if (visualMesh) {
				visualMesh->SetGeometry(proxiMesh);
			}
		}

		dTree<dGeometry*,  NewtonCollision*>::Iterator cacheIter (collisionCache);
		for (cacheIter.Begin(); cacheIter; cacheIter ++) {
			proxiMesh = cacheIter.GetNode()->GetInfo();
			proxiMesh->Release();
		}
	}



	dMatrix m_axisRotation;
	dMatrix m_axisRotationInv;
	dSceneNode* m_rootNode;
	CameraNode* m_cameraNode;
	domCamera* m_collCamera;
	dLoaderContext* m_loaderContext;

	CollGeoCache m_geometryCache;

	dRigidBodyMap m_rigidBodyMap;
	dNodeToColladaNodeMap m_visualNodeMap;

	
	NewtonBodyList m_bodyList;
	NewtonJointList m_jointList;
	NewtonCollisionList m_collisionList;


//	dNodePhysicsConstraintBody m_constraintBodyCache;
//	dNodePhysicsMaterialsCache m_phyicsMaterialCache;

	dNodePhysicsRigidBody m_rigidBodyCache;
	ColladaNodeTodSceneNodeMap m_nodeTodNodeMap;
	
	
	NewtonWorld* m_world; 
#endif

	dMatrix m_globalRotation;
	dFloat m_scale;
	int m_nodeIndex;
	DAE* m_collada; 
};


#if 0
static dMatrix GetUpMatrix (int updir)
{
	dMatrix matrix (GetIdentityMatrix());
	if (updir == 0) {
		_ASSERTE (0);

	} else if (updir == 2) {
		matrix[0] = dVector (0.0f, 1.0f, 0.0f, 0.0f);
		matrix[1] = dVector (0.0f, 0.0f, 1.0f, 0.0f);
		matrix[2] = dVector (1.0f, 0.0f, 0.0f, 0.0f);
	}
	return matrix;
}


void dSceneNode::LoadColladaScene (NewtonWorld* newtonWorld, const char *fileName, dLoaderContext& context, int upDir)
{
	DAE* dae;
	daeInt error;

//	dae = new DAE;
	dae = CreateDAE ();
	_ASSERTE (dae);

	error = dae->load(fileName);
	_ASSERTE (error == DAE_OK);
	if (error == DAE_OK) {
		dColladaFileParcel parcel(dae, newtonWorld);
		parcel.ColladatToScene (this, &context, m_fullScene, GetUpMatrix (upDir));
		SetMatrix (GetIdentityMatrix());
	}

	delete dae;
}

void dSceneNode::LoadColladaPhysics(NewtonWorld* newtonWorld, const char *fileName, dLoaderContext& context, int upDir)
{
	DAE* dae;
	daeInt error;

//	dae = new DAE;
	dae = CreateDAE ();
	_ASSERTE (dae);

	error = dae->load(fileName);
	_ASSERTE (error == DAE_OK);
	if (error == DAE_OK) {
		dColladaFileParcel parcel(dae, newtonWorld);
		parcel.ColladatToScene (this, &context, m_physicCollisionAndVisual, GetUpMatrix (upDir));
		SetMatrix (GetIdentityMatrix());
	}

	delete dae;
}
#endif









void ExportColladaScene (const char* fileName, const NewtonWorld* world)
{
	_ASSERTE (0);
/*
	DAE* dae;
	daeInt error;
	daeURI* uriName;
	daeDatabase* database;
	daeDocument* document;

	// create a new collada object
	dae = CreateDAE ();
	_ASSERTE (dae);

	dColladaFileParcel parcel(dae, );
	parcel.SceneToCollada (world, context);

	database = dae->getDatabase();
	if (database) {
		document = database->getDocument(daeUInt (0));
		if (document) {
			uriName = document->getDocumentURI();
			error = dae->saveAs(fileName, uriName->getURI());
			_ASSERTE (error == DAE_OK);
		}
	}

	delete dae;
*/
}




void dModel::SaveCollada (const char *fileName, const dMatrix& globalRotation, dFloat scale) const
{
	DAE* dae;
	daeInt error;
	daeURI* uriName;
	daeDatabase* database;
	daeDocument* document;

	// create a new collada object
	dae = new DAE;
	_ASSERTE (dae);

	
	dColladaFileParcel parcel(dae, globalRotation, scale);
	parcel.SceneToCollada (this);

	database = dae->getDatabase();
	if (database) {
		document = database->getDocument(daeUInt (0));
		if (document) {
			uriName = document->getDocumentURI();
			error = dae->saveAs(fileName, uriName->getURI());
			_ASSERTE (error == DAE_OK);
		}
	}
	delete dae;
}

void ExportColladaScene (
	const char* name, 
	const NewtonWorld* world, 
	dColladaSceneExportContext* context, 
	const dMatrix& globalRotation, 
	dFloat scale)
{

}


void dModel::LoadCollada (const char* fileName, dLoaderContext& context, const dMatrix& globalRotation, dFloat scale)
{
	DAE* dae;
	daeInt error;

	//	dae = new DAE;
	dae = new DAE;
	_ASSERTE (dae);

	error = dae->load(fileName);
	_ASSERTE (error == DAE_OK);

	if (error == DAE_OK) {
		dColladaFileParcel parcel(dae, globalRotation, scale);
		parcel.ColladatToScene (this, &context);
	}
	delete dae;
}
