/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions 
 * and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/
#include "omega/RenderTarget.h"
#include "omegaOsg/SceneView.h"
#include "omegaOsg/OsgRenderPass.h"

using namespace omegaOsg;

Lock sInitLock;

///////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////
OsgRenderPass::OsgRenderPass(Renderer* client, const String& name): RenderPass(client, name), 
		myTriangleCountStat(NULL),
		myCullTimeStat(NULL),
		myDrawTimeStat(NULL)
{}

///////////////////////////////////////////////////////////////////////////////////////////////////
OsgRenderPass::~OsgRenderPass()
{
	mySceneView = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OsgRenderPass::initialize() 
{
	sInitLock.lock();

	RenderPass::initialize();

	myModule = (OsgModule*)getUserData();

	uint ctxid = getClient()->getGpuContext()->getId();

	// Initialize standard scene view
	mySceneView = new SceneView(myModule->getDatabasePager());
    mySceneView->initialize();
	mySceneView->getState()->setContextID(ctxid);

	// Initialize far scene view (for depth partitioning) 
	myFarSceneView = new SceneView(myModule->getDatabasePager());
    myFarSceneView->initialize();
	myFarSceneView->getState()->setContextID(ctxid);

	StatsManager* sm = SystemManager::instance()->getStatsManager();
	myTriangleCountStat = sm->createStat(ostr("osg tris %1%", %ctxid), Stat::Primitive);
	myCullTimeStat = sm->createStat(ostr("osg cull %1%", %ctxid), Stat::Time);
	myDrawTimeStat = sm->createStat(ostr("osg draw %1%", %ctxid), Stat::Time);

	sInitLock.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OsgRenderPass::drawView(SceneView* view, const DrawContext& context, bool getstats, OsgModule::DepthPartitionMode dpm)
{
	// Bush the current transform state so we can restore it once osg drawing
	// is done.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	view->getState()->setContextID(context.tile->id);

	osg::Camera* cam = view->getCamera();

	cam->setViewport( context.viewport.x(), context.viewport.y(), context.viewport.width(), context.viewport.height() );
	cam->setProjectionMatrix(buildOsgMatrix(context.projection.matrix()));
	cam->setViewMatrix(buildOsgMatrix(context.modelview.matrix()));
	mySceneView->setAutoNearFar(myModule->getAutoNearFar());

	// Adjust camera parameters to take depth partitioning into account
	if(dpm == OsgModule::DepthPartitionNearOnly)
	{
		double left, right, bottom, top, zNear, zFar;
		cam->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);

		// Near camera: clean the Z buffer and use the depth partition Z as the far plane.
		cam->setProjectionMatrixAsFrustum(left, right, bottom, top, zNear, myModule->getDepthPartitionZ());
		cam->setClearMask(GL_DEPTH_BUFFER_BIT);
	}
	else if(dpm == OsgModule::DepthPartitionFarOnly)
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
	myTriangleCountStat->addSample(view->getTriangleCount());

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OsgRenderPass::render(Renderer* client, const DrawContext& context)
{
	//sInitLock.lock();
	if(context.task == DrawContext::SceneDrawTask)
	{
		bool getstats = false;
		// Collect statistics every 10 frames
		if(context.frameNum % 10 == 0) getstats = true;

		myModule->getDatabasePager()->signalBeginFrame(myModule->getFrameStamp());

		OsgModule::DepthPartitionMode dpm = myModule->getDepthPartitionMode();
		if(dpm == OsgModule::DepthPartitionOff)
		{
			drawView(mySceneView, context, getstats, OsgModule::DepthPartitionOff);
		}
		else
		{
			// When depth partitioning is on, we draw the scene twice using the two embedded SceneView instances.
			if(dpm == OsgModule::DepthPartitionOn || dpm == OsgModule::DepthPartitionFarOnly)
			{
				drawView(mySceneView, context, getstats, OsgModule::DepthPartitionFarOnly);
			}
			if(dpm == OsgModule::DepthPartitionOn || dpm == OsgModule::DepthPartitionNearOnly)
			{
				drawView(mySceneView, context, getstats, OsgModule::DepthPartitionNearOnly);
			}
		}

		myModule->getDatabasePager()->signalEndFrame();

	}
	//sInitLock.unlock();
}
