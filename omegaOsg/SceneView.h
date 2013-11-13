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
#ifndef OOSG_SCENEVIEW
#define OOSG_SCENEVIEW

#include <osg/Node>
#include <osg/StateSet>
#include <osg/Light>
#include <osg/FrameStamp>
#include <osg/DisplaySettings>
#include <osgDB/DatabasePager>
#include <osg/CollectOccludersVisitor>
#include <osg/CullSettings>
#include <osg/Camera>

#include <osgUtil/CullVisitor>

///////////////////////////////////////////////////////////////////////////////
class SceneView : public osg::Object, public osg::CullSettings
{
    public:

        /** Construct a default scene view.*/
        SceneView(osgDB::DatabasePager* db = NULL);

        SceneView(const SceneView& sceneview, const osg::CopyOp& copyop = osg::CopyOp());

        META_Object(osgUtil, SceneView);

        /** Set the camera used to represent the camera view of this SceneView.*/
        //void setCamera(osg::Camera* camera, bool assumeOwnershipOfCamera = true);

        /** Get the camera used to represent the camera view of this SceneView.*/
        osg::Camera* getCamera() { return _camera.get(); }

        /** Get the const camera used to represent the camera view of this SceneView.*/
        //const osg::Camera* getCamera() const { return _camera.get(); }

        /** Set the data to view. The data will typically be
         *  an osg::Scene but can be any osg::Node type.
         */
        void setSceneData(osg::Node* node);

        void compileGLObjects();
        
        /** Get the scene data to view. The data will typically be
         *  an osg::Scene but can be any osg::Node type.
         */
        osg::Node* getSceneData(unsigned int childNo=0) 
		{ 
			return (_camera->getNumChildren()>childNo) ? _camera->getChild(childNo) : 0; 
		}

		void setAutoNearFar(bool value);
        
        enum ActiveUniforms
        {
            FRAME_NUMBER_UNIFORM            = 1,
            FRAME_TIME_UNIFORM              = 2,
            DELTA_FRAME_TIME_UNIFORM        = 4,
            SIMULATION_TIME_UNIFORM         = 8,
            DELTA_SIMULATION_TIME_UNIFORM   = 16,
            VIEW_MATRIX_UNIFORM             = 32,
            VIEW_MATRIX_INVERSE_UNIFORM     = 64,
            DEFAULT_UNIFORMS                = FRAME_NUMBER_UNIFORM |
                                              FRAME_TIME_UNIFORM |
                                              DELTA_FRAME_TIME_UNIFORM |
                                              SIMULATION_TIME_UNIFORM |
                                              DELTA_SIMULATION_TIME_UNIFORM |
                                              VIEW_MATRIX_UNIFORM |
                                              VIEW_MATRIX_INVERSE_UNIFORM,
            ALL_UNIFORMS                    = 0x7FFFFFFF
        };

        /** Set the uniforms that SceneView should set set up on each frame.*/        
        void setActiveUniforms(int activeUniforms) { _activeUniforms = activeUniforms; }

        /** Get the uniforms that SceneView should set set up on each frame.*/
        int getActiveUniforms() const { return _activeUniforms; }

        void updateUniforms(int eye);
        
        void setState(osg::State* state) { _renderInfo.setState(state); }
        osg::State* getState() { return _renderInfo.getState(); }
        const osg::State* getState() const { return _renderInfo.getState(); }
        
        void setView(osg::View* view) { _camera->setView(view); }
        osg::View* getView() { return _camera->getView(); }
        const osg::View* getView() const { return _camera->getView(); }

        void setRenderInfo(osg::RenderInfo& renderInfo) { _renderInfo = renderInfo; }
        osg::RenderInfo& getRenderInfo() { return _renderInfo; }
        const osg::RenderInfo& getRenderInfo() const { return _renderInfo; }

        //void setInitVisitor(osg::NodeVisitor* av) { _initVisitor = av; }
        //osg::NodeVisitor* getInitVisitor() { return _initVisitor.get(); }
        //const osg::NodeVisitor* getInitVisitor() const { return _initVisitor.get(); }

        void setCullVisitor(osgUtil::CullVisitor* cv) { _cullVisitor = cv; }
        osgUtil::CullVisitor* getCullVisitor() { return _cullVisitor.get(); }
        const osgUtil::CullVisitor* getCullVisitor() const { return _cullVisitor.get(); }

        void setCollectOccludersVisitor(osg::CollectOccludersVisitor* cov) { _collectOccludersVisitor = cov; }
        osg::CollectOccludersVisitor* getCollectOccludersVisitor() { return _collectOccludersVisitor.get(); }
        const osg::CollectOccludersVisitor* getCollectOccludersVisitor() const { return _collectOccludersVisitor.get(); }


        void setStateGraph(osgUtil::StateGraph* rg) { _stateGraph = rg; }
        osgUtil::StateGraph* getStateGraph() { return _stateGraph.get(); }
        const osgUtil::StateGraph* getStateGraph() const { return _stateGraph.get(); }

        void setRenderStage(osgUtil::RenderStage* rs) { _renderStage = rs; }
        osgUtil::RenderStage* getRenderStage() { return _renderStage.get(); }
        const osgUtil::RenderStage* getRenderStage() const { return _renderStage.get(); }

        /** search through any pre and post RenderStage that reference a Camera, and take a reference to each of these cameras to prevent them being deleted while they are still be used by the drawing thread.*/
        void collateReferencesToDependentCameras();

        /** clear the refence to any any dependent cameras.*/
        void clearReferencesToDependentCameras();


        /** Set the draw buffer value used at the start of each frame draw. */
        void setDrawBufferValue( GLenum drawBufferValue ) { _camera->setDrawBuffer(drawBufferValue); }

        /** Get the draw buffer value used at the start of each frame draw. */
        GLenum getDrawBufferValue() const { return _camera->getDrawBuffer(); }

        /** Set the frame stamp for the current frame.*/
        inline void setFrameStamp(osg::FrameStamp* fs) { _frameStamp = fs; }

        /** Get the frame stamp for the current frame.*/
        inline const osg::FrameStamp* getFrameStamp() const { return _frameStamp.get(); }

        /** Do init traversal of attached scene graph using Init NodeVisitor.
          * The init traversal is called once for each SceneView, and should
          * be used to compile display list, texture objects intialize data
          * not otherwise intialized during scene graph loading. Note, is
          * called automatically by update & cull if it hasn't already been called
          * elsewhere. Also init() should only ever be called within a valid
          * graphics context.*/
        virtual void initialize();

        /** Do cull traversal of attached scene graph using Cull NodeVisitor.*/
        virtual void cull(int eye);

        /** Do draw traversal of draw bins generated by cull traversal.*/
        virtual void draw();
        
        /** Compute the number of dynamic objects that will be held in the rendering backend */
       // unsigned int getReferenceTypeCount() const { return _dynamicObjectCount; }
        
        /** Release all OpenGL objects from the scene graph, such as texture objects, display lists etc.
          * These released scene graphs placed in the respective delete GLObjects cache, which
          * then need to be deleted in OpenGL by SceneView::flushAllDeleteGLObjects(). */
        virtual void releaseAllGLObjects();

        /** Flush all deleted OpenGL objects, such as texture objects, display lists etc.*/
        virtual void flushAllDeletedGLObjects();

        /** Flush deleted OpenGL objects, such as texture objects, display lists etc within specified available time.*/
        virtual void flushDeletedGLObjects(double& availableTime);
        
        /** Extract stats for current draw list. */
        bool getStats(osgUtil::Statistics& primStats); 

		unsigned int getTriangleCount();

    protected:

		virtual ~SceneView();

        /** Do cull traversal of attached scene graph using Cull NodeVisitor. Return true if computeNearFar has been done during the cull traversal.*/
        virtual bool cullStage(
			const osg::Matrixd& projection,
			const osg::Matrixd& modelview,
			osgUtil::CullVisitor* cullVisitor, 
			osgUtil::StateGraph* rendergraph, 
			osgUtil::RenderStage* renderStage, 
			osg::Viewport *viewport);
        
        //void clearArea(int x,int y,int width,int height,const osg::Vec4& color);

        osg::ref_ptr<osg::StateSet>                 _localStateSet;
        osg::RenderInfo                             _renderInfo;
        
        bool                                        _initCalled;
        //osg::ref_ptr<osg::NodeVisitor>              _initVisitor;
        osg::ref_ptr<osgUtil::CullVisitor>          _cullVisitor;
        osg::ref_ptr<osgUtil::StateGraph>           _stateGraph;
        osg::ref_ptr<osgUtil::RenderStage>          _renderStage;

        osg::ref_ptr<osg::CollectOccludersVisitor>  _collectOccludersVisitor;
        
        osg::ref_ptr<osg::FrameStamp>               _frameStamp;
        
        osg::observer_ptr<osg::Camera>              _camera;

		osg::ref_ptr<osgDB::DatabasePager>			_databasePager;

        bool                                        _prioritizeTextures;
        
        bool                                        _requiresFlush;
        
        int                                         _activeUniforms;        
        double                                      _previousFrameTime;
        double                                      _previousSimulationTime;
        
        unsigned int                                _dynamicObjectCount;        

		int _triangleCount;
};

#endif

