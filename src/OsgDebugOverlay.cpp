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
#include "omega/Renderer.h"
#include "omega/DrawInterface.h"
#include "omegaOsg/SceneView.h"
#include "omegaOsg/OsgDebugOverlay.h"

#include <osg/Drawable>
#include <osg/Geometry>
#include <osgUtil/RenderStage>
#include <osgUtil/RenderBin>
#include <osgUtil/RenderLeaf>
#include <osgUtil/StateGraph>

using namespace omegaOsg;

struct RenderLeafData: public ReferenceType
{
    String id;
    unsigned int vertices;
};

struct RenderBinData: public ReferenceType
{
    String id;
    bool isStage;
    bool isSorted;
    int index;
    List< Ref<RenderLeafData> > leafs;
    List< Ref<RenderBinData> > bins;
};

////////////////////////////////////////////////////////////////////////////////
void traverseRenderBin(RenderBinData* data, osgUtil::RenderBin* bin)
{
    data->id = bin->getName();
    data->isSorted = false;
    foreach(osgUtil::StateGraph* sg, bin->getStateGraphList())
    {
        foreach(osgUtil::RenderLeaf* lf, sg->_leaves)
        {
            RenderLeafData* lfd = new RenderLeafData();
            const osg::Drawable* dw = lf->getDrawable();
            lfd->id = ostr("%1%-%2$x", %dw->className() %(size_t)dw);
            const osg::Geometry* geo = dw->asGeometry();
            if(geo != NULL)
            {
                lfd->vertices = geo->getVertexArray()->getNumElements();
            }
            else
            {
                lfd->vertices = 0;
            }

            data->leafs.push_back(lfd);
        }
    }

    foreach(osgUtil::RenderLeaf* lf, bin->getRenderLeafList())
    {
        data->isSorted = true;
        RenderLeafData* lfd = new RenderLeafData();
        const osg::Drawable* dw = lf->getDrawable();
        lfd->id = ostr("%1%-%2$x", %dw->className() %(size_t)dw);
        const osg::Geometry* geo = dw->asGeometry();
        if(geo != NULL)
        {
            lfd->vertices = geo->getVertexArray()->getNumElements();
        }
        else
        {
            lfd->vertices = 0;
        }

        data->leafs.push_back(lfd);
    }
    typedef std::pair<int, osgUtil::RenderBin*> RenderBinItem;
    foreach(RenderBinItem rbi, bin->getRenderBinList())
    {
        RenderBinData* rbd = new RenderBinData();
        rbd->isStage = false;
        rbd->index = rbi.first;
        traverseRenderBin(rbd, rbi.second);
        data->bins.push_back(rbd);
    }
}

////////////////////////////////////////////////////////////////////////////////
void traverseRenderStage(RenderBinData* data, osgUtil::RenderStage* stage)
{
    data->isStage = true;

    foreach(osgUtil::RenderStage::RenderStageOrderPair rsop, stage->getPreRenderList())
    {
        RenderBinData* rbd = new RenderBinData();
        rbd->index = rsop.first;
        traverseRenderStage(rbd, rsop.second);
        data->bins.push_front(rbd);
    }
    traverseRenderBin(data, stage);
    foreach(osgUtil::RenderStage::RenderStageOrderPair rsop, stage->getPostRenderList())
    {
        RenderBinData* rbd = new RenderBinData();
        rbd->index = rsop.first;
        traverseRenderStage(rbd, rsop.second);
        data->bins.push_back(rbd);
    }
}

////////////////////////////////////////////////////////////////////////////////
OsgDebugOverlay::OsgDebugOverlay()
{
    myData = new RenderBinData();
}

////////////////////////////////////////////////////////////////////////////////
OsgDebugOverlay::~OsgDebugOverlay()
{
    myData->unref();
    myData = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void OsgDebugOverlay::update(SceneView* view)
{
    myData->bins.clear();
    myData->leafs.clear();

    osgUtil::RenderStage* rs = view->getRenderStage();
    
    traverseRenderStage(myData, rs);
}

////////////////////////////////////////////////////////////////////////////////
void drawRenderBin(RenderBinData* data, DrawInterface* di, Vector2f& pos)
{
    Font* ft = di->getDefaultFont();
    
    Vector2f txs = ft->computeSize("O");
    float charsz = txs.y() + 5;

    String name = "RENDER BIN";
    String sorted = "";
    if(data->isStage) name = "RENDER STAGE";
    if(data->isSorted) sorted = "SORTED";
    di->drawText(
        ostr("%1% %2% %3%", %sorted %name %data->id),
        ft, pos, Font::HALeft | Font::VAMiddle, 
        data->isStage ? Color::Lime : Color::Orange);

    pos += Vector2f(charsz * 4, charsz);

    foreach(RenderLeafData* rlf, data->leafs)
    {
        di->drawText(
            ostr("%1% vtx: %2%", %rlf->id %rlf->vertices),
		    ft, pos, Font::HALeft | Font::VAMiddle, Color::White);
        pos += Vector2f(0, charsz);
    }
    foreach(RenderBinData* rb, data->bins)
    {
        drawRenderBin(rb, di, pos);
    }
    pos -= Vector2f(charsz * 4, 0);
}

////////////////////////////////////////////////////////////////////////////////
void OsgDebugOverlay::draw(const DrawContext& context)
{
    if(context.task == DrawContext::OverlayDrawTask &&
		context.eye == DrawContext::EyeCyclop)
    {
        DrawInterface* di = context.renderer->getRenderer();
		di->beginDraw2D(context);

        Vector2f pos = Vector2f::Zero();
        Vector2f size= Vector2f(300, 400);

	    const DisplayTileConfig* tile = context.tile;
	    float cx = tile->offset.x();
	    float cy = tile->offset.y();
	    pos += Vector2f(cx, cy);

	    di->drawRect(pos, size, Color(0,0,0,0.8f));

	    pos[1] += 10;

        drawRenderBin(myData, di, pos);
		di->endDraw();
    }
}
