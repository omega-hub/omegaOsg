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
#ifndef __OSG_RENDER_PASS_H__
#define __OSG_RENDER_PASS_H__

#include "omega/RenderPass.h"

#include "oosgbase.h"
#include "OsgModule.h"


class SceneView;

namespace osg
{
	class Node;
}

namespace omegaOsg
{
	using namespace omega;

	///////////////////////////////////////////////////////////////////////////////////////////////
	class OOSG_API OsgRenderPass: public RenderPass
	{
	public:
		OsgRenderPass(Renderer* client, const String& name);
		static RenderPass* createInstance(Renderer* client) { return new OsgRenderPass(client, "OsgRenderPass"); }
	public:
		virtual ~OsgRenderPass();

		virtual void initialize();
		virtual void render(Renderer* client, const DrawContext& context);

	private:
		void drawView(SceneView* view, const DrawContext& context, bool getstats, OsgModule::DepthPartitionMode dpm);

	private:
		OsgModule* myModule;
		Ref<SceneView> mySceneView;
		Ref<SceneView> myFarSceneView;
		
		// Statistics
		Stat* myTriangleCountStat;
		Stat* myCullTimeStat;
		Stat* myDrawTimeStat;
	};
};
#endif