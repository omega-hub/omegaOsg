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
#include "omega/PixelData.h"
#include "omegaOsg/OsgModule.h"
#include "omegaOsg/OsgRenderPass.h"
#include "omegaOsg/OsgSceneObject.h"

#include <osgUtil/Optimizer>
#include <osgUtil/UpdateVisitor>
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osgDB/DatabasePager>
#include <osg/Node>
#include <osg/FrameStamp>

#include "omegaOsg/ReaderFreeImage.h"
#ifdef OMEGAOSG_USE_INVENTOR
#include "Inventor/ReaderWriterIV.h"
#endif
#ifdef OMEGAOSG_USE_FBX
#include "fbx/ReaderWriterFBX.h"
#endif

using namespace omegaOsg;

OsgModule* OsgModule::mysInstance = NULL;
//bool OsgModule::mysAmbientOverrideHack = true;

///////////////////////////////////////////////////////////////////////////////
osg::Image* OsgModule::pixelDataToOsg(PixelData* img, bool transferBufferOwnership)
{
    bool leaveMemoryAlone = false;

    
    // Three cases here:
    // 1 - the pixel data does not own the pixel buffer. 
    // 2 - the pixel data owns the buffer and wants to transfer ownership.
    // 2 - the pixel data owns the buffer and wants to keep ownership.
    if(img->isDeleteDisabled())
    {
        // case 1 - tell osg to leave the buffer alone
        leaveMemoryAlone = true;
    }
    else
    {
        if(transferBufferOwnership)
        {
            // case 2 - the osg image is in charge of the buffer now
            img->setDeleteDisabled(true);
        }
        else
        {
            // case 3 - the omegalib pixel data object keeps the ownership
            // tell osg to leave the buffer alone.
            leaveMemoryAlone = true;
        }
    }
    
    int s = img->getWidth();
    int t = img->getHeight();
    int r = 1;

    int internalFormat = img->getBpp() / 8;

    unsigned int pixelFormat =
        internalFormat == 1 ? GL_LUMINANCE :
    internalFormat == 2 ? GL_LUMINANCE_ALPHA :
    internalFormat == 3 ? GL_RGB :
    internalFormat == 4 ? GL_RGBA : (GLenum)-1;

    unsigned int dataType = GL_UNSIGNED_BYTE;

    osg::Image* pOsgImage = new osg::Image;
    pOsgImage->setImage(s,t,r,
        internalFormat,
        pixelFormat,
        dataType,
        img->map(),
        leaveMemoryAlone ? osg::Image::NO_DELETE : osg::Image::USE_MALLOC_FREE);

    img->unmap();
    return pOsgImage;
}

///////////////////////////////////////////////////////////////////////////////
OsgModule* OsgModule::instance() 
{ 
    if(mysInstance == NULL)
    {
        mysInstance = new OsgModule();
        ModuleServices::addModule(mysInstance);
        mysInstance->doInitialize(Engine::instance());
    }
    return mysInstance; 
}

///////////////////////////////////////////////////////////////////////////////
OsgModule::OsgModule():
    EngineModule("OsgModule"),
        myDepthPartitionMode(OsgDrawInformation::DepthPartitionOff),
        myDepthPartitionZ(1000),
        myDisplayDebugOverlay(false)
{
    mysInstance = this;

    myAutoNearFar = false;

    myRootNode = NULL;
    //myRootSceneObject = NULL;

    myDatabasePager = osgDB::DatabasePager::create();

    myFrameStamp = new osg::FrameStamp;
    myUpdateVisitor = new osgUtil::UpdateVisitor;
    myUpdateVisitor->setFrameStamp( myFrameStamp );
    myUpdateVisitor->setDatabaseRequestHandler(myDatabasePager);
    
    // OsgDB: register the default path of plugin files
    // Database should always be a subdir of the current executable directory
    String execPath = ogetexecpath();
    String execName;
    StringUtils::splitFilename(execPath, execName, execPath);
    osgDB::Registry* reg = osgDB::Registry::instance();
    String libPath = execPath + String("/osg/osg/osgPlugins-3.3.0:") + execPath + String("/osg");
    ofmsg("OSG Plugin Path(s): %1%", %libPath);
    reg->setLibraryFilePathList(libPath);

    reg->addReaderWriter(new ReaderFreeImage());
#ifdef OMEGAOSG_USE_FBX
    reg->addReaderWriter(new ReaderWriterFBX());
#endif
    //osgDB::Registry::instance()->addReaderWriter(new ReaderWriterIV());
}

///////////////////////////////////////////////////////////////////////////////
OsgModule::~OsgModule()
{
    omsg("~OsgModule");
    mysInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void OsgModule::initialize()
{
}

///////////////////////////////////////////////////////////////////////////////
void OsgModule::dispose()
{
    getEngine()->removeRenderPass("OsgRenderPass");
}

///////////////////////////////////////////////////////////////////////////////
void OsgModule::initializeRenderer(Renderer* r)
{
    OsgRenderPass* osgrp = new OsgRenderPass(r, "OsgRenderPass");
    osgrp->setUserData(this);
    r->addRenderPass(osgrp);
    myRenderPasses.push_back(osgrp);
}

///////////////////////////////////////////////////////////////////////////////
void OsgModule::compileObjectsOnNextDraw()
{
    foreach(OsgRenderPass* rp, myRenderPasses)
    {
        rp->compileGLOjects();
    }
}

///////////////////////////////////////////////////////////////////////////////
void OsgModule::setRootNode(osg::Node* value) 
{ 
    myRootNode = value; 
}

///////////////////////////////////////////////////////////////////////////////
void OsgModule::update(const UpdateContext& context)
{
    myFrameStamp->setFrameNumber(context.frameNum);
    myFrameStamp->setReferenceTime(context.time);
    myFrameStamp->setSimulationTime(context.time);

    myUpdateVisitor->reset();
    myUpdateVisitor->setFrameStamp(myFrameStamp);
    myUpdateVisitor->setTraversalNumber(context.frameNum);
    if(myRootNode != NULL)
    {
        myRootNode->accept(*myUpdateVisitor);
    }
    myDatabasePager->updateSceneGraph(*myFrameStamp);
}

///////////////////////////////////////////////////////////////////////////////
bool OsgModule::handleCommand(const String& command)
{
    Vector<String> args = StringUtils::split(command);
    if(args[0] == "?"  && args.size() == 1)
    {
        // ?: print help
        omsg("OsgModule");
        omsg("\t autonearfar [on|off] - (experimental) toggle auto near far Z on or off");
        omsg("\t depthpart [on <value>|off|near|far] - set the depth partition mode and Z threshold");
        omsg("\t rs - toggle the rendering statistics overlay");
    }
    else if(args[0] == "rs")
    {
        myDisplayDebugOverlay = !myDisplayDebugOverlay;
    }
    else if(args[0] == "autonearfar")
    {
        if(args.size() > 1)
        {
            if(args[1] == "on") setAutoNearFar(true);
            else if(args[1] == "off") setAutoNearFar(false);
        }
        ofmsg("OsgModule: autoNearFar = %1%", %getAutoNearFar());
        // Mark command as handled
        return true;
    }
    else if(args[0] == "depthpart")
    {
        if(args.size() > 1)
        {
            if(args[1] == "on" && args.size() > 2)
            {
                float z = boost::lexical_cast<float>(args[2]);
                myDepthPartitionMode = OsgDrawInformation::DepthPartitionOn;
                myDepthPartitionZ = z;
            }
            else if(args[1] == "off")
            {
                myDepthPartitionMode = OsgDrawInformation::DepthPartitionOff;
            }
            else if(args[1] == "near")
            {
                myDepthPartitionMode = OsgDrawInformation::DepthPartitionNearOnly;
            }
            else if(args[1] == "far")
            {
                myDepthPartitionMode = OsgDrawInformation::DepthPartitionFarOnly;
            }
            else
            {
                ofwarn("OsgModule::handleCommand: unknown depth partition mode %1%", %command);
            }
        }
    }
    return false;
}
