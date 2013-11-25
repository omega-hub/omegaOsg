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
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. Redistributions in binary 
 * form must reproduce the above copyright notice, this list of conditions and 
 * the following disclaimer in the documentation and/or other materials provided 
 * with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * What's in this file
 *	The core of the omegalib <-> OpenSceneGraph adapter
 ******************************************************************************/
#ifndef __OSG_MODULE_H__
#define __OSG_MODULE_H__

#include "oosgbase.h"
#include "OsgRenderPass.h"
#include "omega/Engine.h"
#include "omega/Application.h"

//! Vector conversion macros
#define OOSG_VEC3(v) osg::Vec3(v[0], v[1], v[2])
#define OOSG_VEC2(v) osg::Vec2(v[0], v[1])

namespace osg
{
    class Node;
    class FrameStamp;
    class NodeVisitor;
    class Image;
}

namespace osgDB
{
    class DatabasePager;
}

namespace omegaOsg
{
    using namespace omega;

    class OsgSceneObject;

    ///////////////////////////////////////////////////////////////////////////
    class OOSG_API OsgModule: public EngineModule
    {
    friend class OsgRenderPass;
    public:
        static OsgModule* instance();
        
        //! Convert an omegalib PixelData object to an osg::Image. PixelData and the osg Image
        //! will share the same pixel buffer.
        //! If transferBufferOwnership is set to true, the underlying pixel buffer will be owned
        //! by the osg image: deleting the PixelData object will not deallocate the buffer. By
        //! default, PixelData retains ownership of the buffer.
        static osg::Image* pixelDataToOsg(PixelData* img, bool transferBufferOwnership = false);

        //! Force
        void compileObjectsOnNextDraw();
        
    public:
        OsgModule();
        virtual ~OsgModule();

        virtual void initialize();
        virtual void dispose();
        virtual void initializeRenderer(Renderer* r);
        virtual void update(const UpdateContext& context);
        virtual void handleEvent(const Event& evt) {}
        virtual bool handleCommand(const String& command);
        //void loadScene(const String& filename);
        //void addToScene(SceneNode* node);

        osg::FrameStamp* getFrameStamp() { return myFrameStamp; }
        osg::Node* getRootNode() { return myRootNode; }
        void setRootNode(osg::Node* value);

        osgDB::DatabasePager* getDatabasePager() { return myDatabasePager; }

        // When set to true, osg will compute near and far planes automatically using scene geometry bounding boxes
        // NOTE: this will lead to inconsistent depth testing with other render passes. 
        void setAutoNearFar(bool value) { myAutoNearFar = value; }
        bool getAutoNearFar() { return myAutoNearFar; }

        void setDepthPartitionMode(OsgDrawInformation::DepthPartitionMode mode) { myDepthPartitionMode = mode; }
        OsgDrawInformation::DepthPartitionMode getDepthPartitionMode() { return myDepthPartitionMode; }
        void setDepthPartitionZ(float value) { myDepthPartitionZ = value; }
        float getDepthPartitionZ() { return myDepthPartitionZ; }

    private:
        static OsgModule* mysInstance;

        bool myAutoNearFar;

        bool myDisplayDebugOverlay;

        OsgDrawInformation::DepthPartitionMode myDepthPartitionMode;
        float myDepthPartitionZ;

        //OsgSceneObject* myRootSceneObject;
        Ref<osg::Node> myRootNode;
        Ref<osg::FrameStamp> myFrameStamp;
        Ref<osg::NodeVisitor> myUpdateVisitor;
        Ref<osgDB::DatabasePager> myDatabasePager;
        List< Ref<OsgRenderPass> > myRenderPasses;
    };
};
#endif