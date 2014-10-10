/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory,  
 * University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. Redistributions in binary 
 * form must reproduce the above copyright notice, this list of conditions and 
 * the following disclaimer in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE  GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * What's in this file
 *	OsgSceneObject: the connection point between an osg node and an omegalib
 *	scene node. OsgSceneObject is can be attached to an omegalib scene node and
 *	contains an osg node and transformation.
 *****************************************************************************/
#ifndef __OSG_SCENE_OBJECT_H__
#define __OSG_SCENE_OBJECT_H__

#include "oosgbase.h"
#include "omega/NodeComponent.h"
#include "omega/SceneNode.h"

#include <osg/Matrix>

namespace osg
{
	class MatrixTransform;
	class Node;
}

namespace omegaOsg
{
	using namespace omega;

	///////////////////////////////////////////////////////////////////////////
	//!	OsgSceneObject: the connection point between an osg node and an omegalib
	//!	scene node. OsgSceneObject is can be attached to an omegalib scene node and
	//!	contains an osg node and transformation.
	class OOSG_API OsgSceneObject: public NodeComponent, SceneNodeListener
	{
    public:
        //! SceneNode flag bit used to enable point picking for osg objects
        //! attached to the scene node. Default flag is 1 << 16 but can be 
        //! changed by users to avoid overlap with other custom flags.
        static uint SceneNodeHitPointsFlag;

    public:
		//! Creates a new OsgSceneObject wrapping an osg Node.
		//! If th osg Node is already a matrix transform, OsgSceneObject will RESET any transformation 
		//! currently applied to the node, and use the transformation of the SceneNode owning this object instead.
		OsgSceneObject(osg::Node* node);
		~OsgSceneObject();

		virtual void update(const UpdateContext& context);

		virtual void onVisibleChanged(SceneNode* source, bool value);
		virtual void onSelectedChanged(SceneNode* source, bool value);

		virtual const AlignedBox3* getBoundingBox();
		virtual bool hasBoundingBox() { return true; }

		osg::MatrixTransform* getTransformedNode() { return myTransform; }

		virtual bool isInitialized() { return myInitialized; }
		virtual void initialize(Engine* server) { myInitialized = true; }

		virtual bool hasCustomRayIntersector() { return true; }
		virtual bool intersectRay(const Ray& ray, Vector3f* hitPoint);

		virtual void onAttached(SceneNode*);
		virtual void onDetached(SceneNode*);

		//! When set to true, the osg object will use the SceneNode local transform instead of its
		//! absolute one. This is useful when the osg object is already part of a node hierarchy,
		//! and we only want to modify its local transformation. 
		void useLocalTransform(bool value) { myUseLocalTransform = value; }
		bool usesLocalTransform() { return myUseLocalTransform; }

	private:
		Ref<osg::Node> myNode;
		Ref<osg::MatrixTransform> myTransform;
		AlignedBox3 myBBox;
		bool myInitialized;

		bool myUseLocalTransform;
	};
};
#endif