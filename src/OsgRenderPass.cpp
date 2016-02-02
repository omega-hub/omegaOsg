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
#include "omega/RenderTarget.h"
#include "omega/Engine.h"
#include "omegaOsg/SceneView.h"
#include "omegaOsg/OsgRenderPass.h"
#include "omegaOsg/OsgModule.h"
#include "omegaOsg/OsgDebugOverlay.h"

#include <osgViewer/Renderer>

using namespace omegaOsg;

Lock sInitLock;

////////////////////////////////////////////////////////////////////////////////
inline osg::Matrix buildOsgMatrix( const omega::Matrix4f& matrix )
{
    return osg::Matrix( matrix( 0, 0 ), matrix( 1, 0 ),
                        matrix( 2, 0 ), matrix( 3, 0 ),
                        matrix( 0, 1 ), matrix( 1, 1 ),
                        matrix( 2, 1 ), matrix( 3, 1 ),
                        matrix( 0, 2 ), matrix( 1, 2 ),
                        matrix( 2, 2 ), matrix( 3, 2 ),
                        matrix( 0, 3 ), matrix( 1, 3 ),
                        matrix( 2, 3 ), matrix( 3, 3 ));
}

////////////////////////////////////////////////////////////////////////////////
OsgRenderPass::OsgRenderPass(Renderer* client, const String& name): RenderPass(client, name), 
        myCompileGLOBjects(false),
        myTriangleCountStat(NULL),
        myCullTimeStat(NULL),
        myDrawTimeStat(NULL)
{
    myDebugOverlay = new OsgDebugOverlay();
    myDrawInfo = new OsgDrawInformation();
}

////////////////////////////////////////////////////////////////////////////////
OsgRenderPass::~OsgRenderPass()
{
    //osg::GraphicsContext::incrementContextIDUsageCount(mySceneView->getState()->getContextID());

    mySceneView = NULL;
    //myDebugOverlay->unref();
    myDebugOverlay = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void OsgRenderPass::initialize() 
{
    sInitLock.lock();

    RenderPass::initialize();

    myModule = (OsgModule*)getUserData();

    uint ctxid = getClient()->getGpuContext()->getId();
    //osg::GraphicsContext::incrementContextIDUsageCount(ctxid);

    // Initialize standard scene view
    mySceneView = new SceneView(myModule->getDatabasePager());
    mySceneView->initialize();
    mySceneView->getState()->setContextID(osg::GraphicsContext::createNewContextID());

    // Initialize far scene view (for depth partitioning) 
    myFarSceneView = new SceneView(myModule->getDatabasePager());
    myFarSceneView->initialize();
    myFarSceneView->getState()->setContextID(ctxid);

    StatsManager* sm = SystemManager::instance()->getStatsManager();
    myTriangleCountStat = sm->createStat(ostr("osg tris %1%", %ctxid), StatsManager::Primitive);
    myCullTimeStat = sm->createStat(ostr("osg cull %1%", %ctxid), StatsManager::Time);
    myDrawTimeStat = sm->createStat(ostr("osg draw %1%", %ctxid), StatsManager::Time);

    sInitLock.unlock();
}

////////////////////////////////////////////////////////////////////////////////
void OsgRenderPass::drawView(SceneView* view, const DrawContext& context, bool getstats, OsgDrawInformation::DepthPartitionMode dpm)
{
    // Bush the current transform state so we can restore it once osg drawing
    // is done.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    //view->getState()->setContextID(context.tile->id);

    osg::Camera* cam = view->getCamera();

    // Pass draw information to the camera
    myDrawInfo->context = &context;
    myDrawInfo->depthPartitionMode = dpm;
    cam->setUserData(myDrawInfo);

    // Set the camera culling mode
    if(context.camera->isCullingEnabled()) cam->setCullingMode(osg::CullSettings::VIEW_FRUSTUM_CULLING);
    else cam->setCullingMode(osg::CullSettings::NO_CULLING);

    cam->setViewport( context.viewport.x(), context.viewport.y(), context.viewport.width(), context.viewport.height() );
    cam->setProjectionMatrix(buildOsgMatrix(context.projection.matrix()));
    cam->setViewMatrix(buildOsgMatrix(context.modelview.matrix()));
    mySceneView->setAutoNearFar(myModule->getAutoNearFar());

    // Adjust camera parameters to take depth partitioning into account
    if(dpm == OsgDrawInformation::DepthPartitionNearOnly)
    {
        double left, right, bottom, top, zNear, zFar;
        cam->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);

        // Near camera: clean the Z buffer and use the depth partition Z as the far plane.
        cam->setProjectionMatrixAsFrustum(left, right, bottom, top, zNear, myModule->getDepthPartitionZ());
        cam->setClearMask(GL_DEPTH_BUFFER_BIT);
    }
    else if(dpm == OsgDrawInformation::DepthPartitionFarOnly)
    {
        double left, right, bottom, top, zNear, zFar;
        cam->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);

        // Far camera: do not clean the Z buffer and set the near plane to the depth partition Z. Rescale the 
        // projection matrix accordingly.
        double nr = myModule->getDepthPartitionZ() / zNear;
        cam->setProjectionMatrixAsFrustum(left * nr, right * nr, bottom * nr, top * nr, myModule->getDepthPartitionZ(), zFar);
        cam->setClearMask(0);
    }

    // Update the scene view root node if needed.
    if(view->getSceneData() == NULL || 
        view->getSceneData() != myModule->getRootNode())
    {
        osg::Node* root = myModule->getRootNode();
        view->setSceneData(root);
    }
    view->setFrameStamp(myModule->getFrameStamp());
    
    // Cull
    myCullTimeStat->startTiming();
    view->cull(context.eye);
    myCullTimeStat->stopTiming();

    // Draw
    myDrawTimeStat->startTiming();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    view->draw();
    glPopAttrib();
    myDrawTimeStat->stopTiming();

    // Collect triangle count statistics (every 10 frames)
    // NOTE: The following line has a major performance impact on applications.
    // It should be called sparingly, and only when user explicitly wants
    // statistics on triangle counts.
    //myTriangleCountStat->addSample(view->getTriangleCount());

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

////////////////////////////////////////////////////////////////////////////////
void OsgRenderPass::render(Renderer* client, const DrawContext& context)
{
    // Compile GL objects if needed.
    if(myCompileGLOBjects)
    {
        myCompileGLOBjects = false;
        mySceneView->compileGLObjects();
    }

    //sInitLock.lock();
    if(context.task == DrawContext::SceneDrawTask)
    {
        bool getstats = false;
        // Collect statistics every 10 frames
        if(context.frameNum % 10 == 0) getstats = true;

        myModule->getDatabasePager()->signalBeginFrame(myModule->getFrameStamp());

        OsgDrawInformation::DepthPartitionMode dpm = myModule->getDepthPartitionMode();
        if(dpm == OsgDrawInformation::DepthPartitionOff)
        {
            drawView(mySceneView, context, getstats, OsgDrawInformation::DepthPartitionOff);
        }
        else
        {
            // When depth partitioning is on, we draw the scene twice using the two embedded SceneView instances.
            if(dpm == OsgDrawInformation::DepthPartitionOn || dpm == OsgDrawInformation::DepthPartitionFarOnly)
            {
                drawView(mySceneView, context, getstats, OsgDrawInformation::DepthPartitionFarOnly);
            }
            if(dpm == OsgDrawInformation::DepthPartitionOn || dpm == OsgDrawInformation::DepthPartitionNearOnly)
            {
                drawView(mySceneView, context, getstats, OsgDrawInformation::DepthPartitionNearOnly);
            }
        }

        myModule->getDatabasePager()->signalEndFrame();
    }
    else if(context.task == DrawContext::OverlayDrawTask)
    {
        if(myModule->myDisplayDebugOverlay)
        {
            myDebugOverlay->update(mySceneView);
            myDebugOverlay->draw(context);
        }
        if(myModule->setRenderPassListener())
        {
            myModule->setRenderPassListener()->onFrameFinished(client, context, mySceneView);
        }
    }
    //sInitLock.unlock();
}
