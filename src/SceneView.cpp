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
 *	A customized wrapper around an openscenegraph scene
 ******************************************************************************/
#include "omega/osystem.h"
#include "omegaOsg/SceneView.h"

#include <osgUtil/Statistics>
#include <osgUtil/UpdateVisitor>
#include <osgUtil/GLObjectsVisitor>

#include <osg/Timer>
#include <osg/GLExtensions>
#include <osg/GLObjects>
#include <osg/Notify>
#include <osg/Texture>
#include <osg/AlphaFunc>
#include <osg/TexEnv>
#include <osg/ColorMatrix>
#include <osg/LightModel>
#include <osg/CollectOccludersVisitor>

#include <osg/GLU>

#include <iterator>

using namespace osg;
using namespace osgUtil;

///////////////////////////////////////////////////////////////////////////////
SceneView::SceneView( osgDB::DatabasePager* dp)
{
    //_displaySettings = ds;
    _databasePager = dp;

    //_prioritizeTextures = false;
    
    _camera = new Camera;
    _camera->setViewport(new Viewport);
    
    _initCalled = false;

    _camera->setDrawBuffer(GL_BACK);

    _requiresFlush = true;
    
    _activeUniforms = DEFAULT_UNIFORMS;
    
    _previousFrameTime = 0;
    _previousSimulationTime = 0;
    
    _dynamicObjectCount = 0;
}

///////////////////////////////////////////////////////////////////////////////
SceneView::SceneView(const SceneView& rhs, const osg::CopyOp& copyop):
    osg::Object(rhs,copyop),
    osg::CullSettings(rhs)
{
    //_displaySettings = rhs._displaySettings;

    //_prioritizeTextures = rhs._prioritizeTextures;
    
    _camera = rhs._camera;
    //_cameraWithOwnership = rhs._cameraWithOwnership;
    
    _initCalled = false;

    _requiresFlush = rhs._requiresFlush;
    
    _activeUniforms = rhs._activeUniforms;
    
    _previousFrameTime = 0;
    _previousSimulationTime = 0;
    
    _dynamicObjectCount = 0;
}

///////////////////////////////////////////////////////////////////////////////
SceneView::~SceneView()
{
    omega::omsg("~SceneView");
}

//class NH: public osg::NotifyHandler
//{
//public:
//    virtual void notify(osg::NotifySeverity severity, const char *message)
//	{
//		message[strlen(message) - 1] = '\0';
//		omega::omsg(message);
//	}
//};

///////////////////////////////////////////////////////////////////////////////
//void SceneView::setCamera(osg::Camera* camera, bool assumeOwnershipOfCamera)
//{
//    if (camera)
//    {
//        _camera = camera;
//    }
//    else
//    {
//        osg::notify(osg::NOTICE)<<"Warning: attempt to assign a NULL camera to SceneView not permitted."<<std::endl;
//    }
//    
//    if (assumeOwnershipOfCamera)
//    {
//        _cameraWithOwnership = _camera.get();
//    }
//    else
//    {
//        _cameraWithOwnership = 0;
//    }
//}

///////////////////////////////////////////////////////////////////////////////
void SceneView::setSceneData(osg::Node* node)
{
    // take a temporary reference to node to prevent the possibility
    // of it getting deleted when when we do the camera clear of children. 
    omega::Ref<osg::Node> temporaryRefernce = node;
    
    // remove pre existing children
    _camera->removeChildren(0, _camera->getNumChildren());
    
    // add the new one in.
    _camera->addChild(node);

    compileGLObjects();
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::compileGLObjects()
{
    if (_camera.valid() && _initVisitor.valid())
    {
        _initVisitor->reset();
        _initVisitor->setDatabaseRequestHandler(_databasePager);
        _initVisitor->setFrameStamp(_frameStamp.get());
        
        GLObjectsVisitor* dlv = dynamic_cast<GLObjectsVisitor*>(_initVisitor.get());
        if (dlv) dlv->setState(_renderInfo.getState());
        
        if (_frameStamp.valid())
        {
             _initVisitor->setTraversalNumber(_frameStamp->getFrameNumber());
        }
        
        _camera->accept(*_initVisitor.get());
        
    } 
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::initialize()
{
    _initCalled = true;

    //osg::setNotifyLevel(INFO);
    //osg::setNotifyHandler(new NH());
    osg::CullSettings::setDefaults();

    //if (!_globalStateSet) _globalStateSet = new osg::StateSet;
    //else _globalStateSet->clear();

    // enable lighting by default.
    //_globalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    osg::State* state = new State();
    _renderInfo.setState(state);
    
    state->setGlobalDefaultModeValue(GL_DEPTH_TEST, true);
    state->setGlobalDefaultModeValue(GL_LIGHTING, true);
    
    _stateGraph = new StateGraph;
    _renderStage = new RenderStage();

    //_updateVisitor = new UpdateVisitor;

    _cullVisitor = CullVisitor::create();
    _cullVisitor->setDatabaseRequestHandler(_databasePager);
    // Disable default computing of near/far plane: Equalizer takes care of this.

    _cullVisitor->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    _cullVisitor->setCullingMode(osg::CullSettings::VIEW_FRUSTUM_CULLING);

    _cullVisitor->setStateGraph(_stateGraph.get());
    _cullVisitor->setRenderStage(_renderStage.get());

    //_globalStateSet->setGlobalDefaults();

    // Do not clear the frame buffer - the omegalib engine takes care of this.
    _camera->setClearMask(0);
    
    _localStateSet = new osg::StateSet;
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::updateUniforms(int eye)
{
    if (!_localStateSet)
    {
        _localStateSet = new osg::StateSet;
    }

    if (!_localStateSet) return;
    
    if ((_activeUniforms & FRAME_NUMBER_UNIFORM) && _frameStamp.valid())
    {
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_FrameNumber",osg::Uniform::INT);
        uniform->set(static_cast<int>(_frameStamp->getFrameNumber()));        
    }
    
    if ((_activeUniforms & FRAME_TIME_UNIFORM) && _frameStamp.valid())
    {
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_FrameTime",osg::Uniform::FLOAT);
        uniform->set(static_cast<float>(_frameStamp->getReferenceTime()));
    }
    
    if ((_activeUniforms & DELTA_FRAME_TIME_UNIFORM) && _frameStamp.valid())
    {
        float delta_frame_time = (_previousFrameTime != 0.0) ? static_cast<float>(_frameStamp->getReferenceTime()-_previousFrameTime) : 0.0f;
        _previousFrameTime = _frameStamp->getReferenceTime();
        
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_DeltaFrameTime",osg::Uniform::FLOAT);
        uniform->set(delta_frame_time);
    }
    
    if ((_activeUniforms & SIMULATION_TIME_UNIFORM) && _frameStamp.valid())
    {
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_SimulationTime",osg::Uniform::FLOAT);
        uniform->set(static_cast<float>(_frameStamp->getSimulationTime()));
    }
    
    if ((_activeUniforms & DELTA_SIMULATION_TIME_UNIFORM) && _frameStamp.valid())
    {
        float delta_simulation_time = (_previousSimulationTime != 0.0) ? static_cast<float>(_frameStamp->getSimulationTime()-_previousSimulationTime) : 0.0f;
        _previousSimulationTime = _frameStamp->getSimulationTime();
        
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_DeltaSimulationTime",osg::Uniform::FLOAT);
        uniform->set(delta_simulation_time);
    }
    
    if (_activeUniforms & VIEW_MATRIX_UNIFORM)
    {
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_ViewMatrix",osg::Uniform::FLOAT_MAT4);
        uniform->set(_camera->getViewMatrix());
    }

    if (_activeUniforms & VIEW_MATRIX_INVERSE_UNIFORM)
    {
        osg::Uniform* uniform = _localStateSet->getOrCreateUniform("osg_ViewMatrixInverse",osg::Uniform::FLOAT_MAT4);
        uniform->set(osg::Matrix::inverse(_camera->getViewMatrix()));
    }

    // Add the eye uniform
    osg::Uniform* uniformEye = _localStateSet->getOrCreateUniform("unif_Eye",osg::Uniform::INT);
    uniformEye->set(eye);
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::cull(int eye)
{
    _dynamicObjectCount = 0;

    if (_camera->getNodeMask()==0) return;

    _renderInfo.setView(_camera->getView());
    _renderInfo.pushCamera(_camera.get());

    // update the active uniforms
    updateUniforms(eye);

    if (!_renderInfo.getState())
    {
        osg::notify(osg::INFO) << "Warning: no valid osgUtil::SceneView::_state attached, creating a default state automatically."<< std::endl;

        // note the constructor for osg::State will set ContextID to 0 which will be fine to single context graphics
        // applications which is ok for most apps, but not multiple context/pipe applications.
        _renderInfo.setState(new osg::State);
    }
    
    osg::State* state = _renderInfo.getState();
    
    // we in theory should be able to be able to bypass reset, but we'll call it just incase.
    //_state->reset();
   
    state->initializeExtensionProcs();
    state->setFrameStamp(_frameStamp.get());
    //state->setDisplaySettings(_displaySettings.get());

    _cullVisitor->setTraversalMask(_cullMask);
    bool computeNearFar = cullStage(_camera->getProjectionMatrix(),_camera->getViewMatrix(),_cullVisitor.get(),_stateGraph.get(),_renderStage.get(),_camera->getViewport());
    if (computeNearFar)
    {
        CullVisitor::value_type zNear = _cullVisitor->getCalculatedNearPlane();
        CullVisitor::value_type zFar = _cullVisitor->getCalculatedFarPlane();
        _cullVisitor->clampProjectionMatrix(_camera->getProjectionMatrix(),zNear,zFar);
    }
    _renderInfo.popCamera();
}

///////////////////////////////////////////////////////////////////////////////
bool SceneView::cullStage(
    const osg::Matrixd& projection,
    const osg::Matrixd& modelview,
    osgUtil::CullVisitor* cullVisitor, 
    osgUtil::StateGraph* rendergraph, 
    osgUtil::RenderStage* renderStage, 
    osg::Viewport *viewport)
{
    if (!_camera || !viewport) return false;

    osg::ref_ptr<RefMatrix> proj = new osg::RefMatrix(projection);
    osg::ref_ptr<RefMatrix> mv = new osg::RefMatrix(modelview);

    // collect any occluder in the view frustum.
    if (_camera->containsOccluderNodes())
    {
        if (!_collectOccludersVisitor) 
        {
            _collectOccludersVisitor = new osg::CollectOccludersVisitor;
        }
        _collectOccludersVisitor->inheritCullSettings(*this);
        _collectOccludersVisitor->reset();
        _collectOccludersVisitor->setFrameStamp(_frameStamp.get());

        // use the frame number for the traversal number.
        if (_frameStamp.valid())
        {
             _collectOccludersVisitor->setTraversalNumber(_frameStamp->getFrameNumber());
        }

        _collectOccludersVisitor->pushViewport(viewport);
        _collectOccludersVisitor->pushProjectionMatrix(proj.get());
        _collectOccludersVisitor->pushModelViewMatrix(mv.get(),osg::Transform::ABSOLUTE_RF);

        // traverse the scene graph to search for occluder in there new positions.
        _collectOccludersVisitor->traverse(*_camera);

        _collectOccludersVisitor->popModelViewMatrix();
        _collectOccludersVisitor->popProjectionMatrix();
        _collectOccludersVisitor->popViewport();
        
        // sort the occluder from largest occluder volume to smallest.
        _collectOccludersVisitor->removeOccludedOccluders();
        
        cullVisitor->getOccluderList().clear();
        std::copy(
            _collectOccludersVisitor->getCollectedOccluderSet().begin(),
            _collectOccludersVisitor->getCollectedOccluderSet().end(), std::back_insert_iterator<CullStack::OccluderList>(cullVisitor->getOccluderList()));
    }
    
    cullVisitor->reset();
    cullVisitor->setFrameStamp(_frameStamp.get());

    // use the frame number for the traversal number.
    if (_frameStamp.valid())
    {
         cullVisitor->setTraversalNumber(_frameStamp->getFrameNumber());
    }

    cullVisitor->inheritCullSettings(*this);
    cullVisitor->setStateGraph(rendergraph);
    cullVisitor->setRenderStage(renderStage);
    cullVisitor->setRenderInfo( _renderInfo );

    renderStage->reset();

    // comment out reset of rendergraph since clean is more efficient.
    //  rendergraph->reset();

    // use clean of the rendergraph rather than reset, as it is able to
    // reuse the structure on the rendergraph in the next frame. This
    // achieves a certain amount of frame cohereancy of memory allocation.
    rendergraph->clean();

    renderStage->setViewport(viewport);
    renderStage->setClearColor(_camera->getClearColor());
    renderStage->setClearDepth(_camera->getClearDepth());
    renderStage->setClearAccum(_camera->getClearAccum());
    renderStage->setClearStencil(_camera->getClearStencil());
    renderStage->setClearMask(_camera->getClearMask());
    
    renderStage->setCamera(_camera.get());

    //if (_globalStateSet.valid()) cullVisitor->pushStateSet(_globalStateSet.get());
    //if (_secondaryStateSet.valid()) cullVisitor->pushStateSet(_secondaryStateSet.get());
    if (_localStateSet.valid()) cullVisitor->pushStateSet(_localStateSet.get());

    cullVisitor->pushViewport(viewport);
    cullVisitor->pushProjectionMatrix(proj.get());

    // traverse the scene graph to generate the rendergraph.        
    cullVisitor->pushModelViewMatrix(mv.get(),osg::Transform::ABSOLUTE_RF);

    // If the camera has a cullCallback execute the callback which has the  
    // requirement that it must traverse the camera's children.
    {
       osg::NodeCallback* callback = _camera->getCullCallback();
       if (callback) (*callback)(_camera.get(), cullVisitor);
       else cullVisitor->traverse(*_camera);
    }


    cullVisitor->popModelViewMatrix();
    cullVisitor->popProjectionMatrix();
    cullVisitor->popViewport();

    if (_localStateSet.valid()) cullVisitor->popStateSet();
    //if (_secondaryStateSet.valid()) cullVisitor->popStateSet();
    //if (_globalStateSet.valid()) cullVisitor->popStateSet();
    
    renderStage->sort();

    // prune out any empty StateGraph children.
    // note, this would be not required if the rendergraph had been
    // reset at the start of each frame (see top of this method) but
    // a clean has been used instead to try to minimize the amount of
    // allocation and deleteing of the StateGraph nodes.
    rendergraph->prune();
    
    // set the number of dynamic objects in the scene.    
    _dynamicObjectCount += renderStage->computeNumberOfDynamicRenderLeaves();

    bool computeNearFar = 
        (cullVisitor->getComputeNearFarMode() != osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR) && 
        getSceneData() != 0;
    return computeNearFar;
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::releaseAllGLObjects()
{
    if (!_camera) return;
   
    _camera->releaseGLObjects(_renderInfo.getState());
    
    // we need to reset State as it keeps handles to Program objects.
    if (_renderInfo.getState()) _renderInfo.getState()->reset();
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::flushAllDeletedGLObjects()
{
    _requiresFlush = false;
    
    osg::flushAllDeletedGLObjects(getState()->getContextID());
 }

///////////////////////////////////////////////////////////////////////////////
void SceneView::flushDeletedGLObjects(double& availableTime)
{
    osg::State* state = _renderInfo.getState();

    _requiresFlush = false;

    double currentTime = state->getFrameStamp()?state->getFrameStamp()->getReferenceTime():0.0;

    osg::flushDeletedGLObjects(getState()->getContextID(), currentTime, availableTime);
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::draw()
{
    if (_camera->getNodeMask()==0) return;

    osg::State* state = _renderInfo.getState();
    state->initializeExtensionProcs();

    osg::Texture::TextureObjectManager* tom = osg::Texture::getTextureObjectManager(state->getContextID()).get();
    tom->newFrame(state->getFrameStamp());

    osg::GLBufferObjectManager* bom = osg::GLBufferObjectManager::getGLBufferObjectManager(state->getContextID()).get();
    bom->newFrame(state->getFrameStamp());

    if (!_initCalled) initialize();

    // note, to support multi-pipe systems the deletion of OpenGL display list
    // and texture objects is deferred until the OpenGL context is the correct
    // context for when the object were originally created.  Here we know what
    // context we are in so can flush the appropriate caches.
    
    if (_requiresFlush)
    {
        double availableTime = 0.005;
        flushDeletedGLObjects(availableTime);
    }

    // assume the the draw which is about to happen could generate GL objects that need flushing in the next frame.
    _requiresFlush = true;

    state->setInitialViewMatrix(new osg::RefMatrix(_camera->getViewMatrix()));

    RenderLeaf* previous = NULL;

    _localStateSet->setAttribute(_camera->getViewport());

    _renderInfo.setView(_camera->getView());
    _renderInfo.pushCamera(_camera.get());

    // Make sure depth test is enabled (on a linux install, it somewhat 
    // ignored the global default set in the osg::State)
    glEnable(GL_DEPTH_TEST);
    
    // bog standard draw.
    _renderStage->drawPreRenderStages(_renderInfo,previous);
    _renderStage->draw(_renderInfo,previous);
    
    _renderInfo.popCamera();

    // re apply the defalt OGL state.
    state->popAllStateSets();
    state->apply();

    if (state->getCheckForGLErrors()!=osg::State::NEVER_CHECK_GL_ERRORS)
    {
        if (state->checkGLErrors("end of SceneView::draw()"))
        {
            // go into debug mode of OGL error in a fine grained way to help
            // track down OpenGL errors.
            state->setCheckForGLErrors(osg::State::ONCE_PER_ATTRIBUTE);
        }
    }

// #define REPORT_TEXTURE_MANAGER_STATS
#ifdef REPORT_TEXTURE_MANAGER_STATS
    tom->reportStats();
    bom->reportStats();
#endif
}

///////////////////////////////////////////////////////////////////////////////
unsigned int SceneView::getTriangleCount()
{ 
    osgUtil::Statistics osgStats;
    osgStats.setType(osgUtil::Statistics::STAT_PRIMS);
    if(_renderStage->getStats(osgStats))
    {
        unsigned int count = osgStats.getPrimitiveCountMap()[GL_TRIANGLES];
        _triangleCount = count;
    }
    return _triangleCount;
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::collateReferencesToDependentCameras()
{
    if (_renderStage.valid()) _renderStage->collateReferencesToDependentCameras();
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::clearReferencesToDependentCameras()
{
    if (_renderStage.valid()) _renderStage->clearReferencesToDependentCameras();
}

///////////////////////////////////////////////////////////////////////////////
void SceneView::setAutoNearFar(bool value)
{
    if(value)
    {
        _cullVisitor->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
    }
    else
    {
        _cullVisitor->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    }
}
