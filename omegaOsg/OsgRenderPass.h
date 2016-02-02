/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory,  
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
 *	A render pass that renders an OSG scene
 ******************************************************************************/
#ifndef __OSG_RENDER_PASS_H__
#define __OSG_RENDER_PASS_H__

#include "omega/RenderPass.h"

#include "oosgbase.h"
#include<osg/Referenced>

namespace osg
{
    class Node;
}

namespace omegaOsg
{
    using namespace omega;
    class OsgModule;
    class OsgDebugOverlay;
    class SceneView;

    ////////////////////////////////////////////////////////////////////////////
    //! Stores user data that will be passed along with the camera to osg nodes
    //! during cull traversal. This can be used by omegalib osg nodes to access
    //! the current omegalib draw context and other information about the 
    //! rendering settings
    struct OsgDrawInformation: public osg::Referenced
    {
        OsgDrawInformation():
            context(NULL), 
            depthPartitionMode(DepthPartitionOff)
            {}

        //! The mode to use for scene depth partitioning
        enum DepthPartitionMode
        {
            //! Depth partition is off. All nodes will be drawn using a single pair of near/far Z values.			
            DepthPartitionOff, 
            //! Depth partition is on. All nodes will be drawn in two batches, using the z value specified through setDepthPartitionZ at the threshold between near and far batches.			
            DepthPartitionOn, 
            //! Draw objects in the far depth partition batch only.
            DepthPartitionFarOnly,
            //! Draw objects in the near depth partition batch only.
            DepthPartitionNearOnly
        };

        const DrawContext* context;
        DepthPartitionMode depthPartitionMode;
    };

    ////////////////////////////////////////////////////////////////////////////
    class OOSG_API OsgRenderPass: public RenderPass
    {
    public:
        OsgRenderPass(Renderer* client, const String& name);
        static RenderPass* createInstance(Renderer* client) { return new OsgRenderPass(client, "OsgRenderPass"); }
    public:
        virtual ~OsgRenderPass();

        virtual void initialize();
        virtual void render(Renderer* client, const DrawContext& context);

        //! Tells the osg renderer to recompile GL objects on the next frame.
        void compileGLOjects() { myCompileGLOBjects = true; }

    private:
        void drawView(SceneView* view, const DrawContext& context, bool getstats, OsgDrawInformation::DepthPartitionMode dpm);

    private:
        OsgModule* myModule;
        Ref<SceneView> mySceneView;
        Ref<SceneView> myFarSceneView;

        Ref<OsgDrawInformation> myDrawInfo;

        bool myCompileGLOBjects;
        
        // Statistics
        Ref<Stat> myTriangleCountStat;
        Ref<Stat> myCullTimeStat;
        Ref<Stat> myDrawTimeStat;

        Ref<OsgDebugOverlay> myDebugOverlay;
    };
};
#endif