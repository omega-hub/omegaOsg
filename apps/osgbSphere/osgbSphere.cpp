#include <osgUtil/Optimizer>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Material>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaToolkit.h>
#include <omegaOsg/omegaOsg.h>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/GUIEventHandler>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osgwTools/Shapes.h>
#include <osgwTools/Version.h>
#include <osg/ShapeDrawable>

#include <osgbDynamics/MotionState.h>
#include <osgbCollision/CollisionShapes.h>
#include <osgbDynamics/RigidBody.h>
#include <osgbCollision/Utils.h>

#include <btBulletDynamicsCommon.h>

#include <string>

#include <osg/io_utils>
#include <iostream>

//#include "GLDebugDrawer.h"

using namespace omega;
using namespace omegaToolkit;
using namespace omegaOsg;

//static GLDebugDrawer gDebugDraw;

///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

//maximum number of objects (and allow user to shoot additional boxes)
#define MAX_PROXIES (ARRAY_SIZE_X*ARRAY_SIZE_Y*ARRAY_SIZE_Z + 1024)

///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3

// create a box
// 100% osg stuff
osg::Geode* osgBox( const osg::Vec3& center, const osg::Vec3& halfLengths )
{
    osg::Vec3 l( halfLengths * 2. );
    osg::Box* box = new osg::Box( center, l.x(), l.y(), l.z() );
    osg::ShapeDrawable* shape = new osg::ShapeDrawable( box );
    shape->setColor( osg::Vec4( 1., 1., 1., 1. ) );
    osg::Geode* geode = new osg::Geode();
    geode->addDrawable( shape );

    return( geode );
}

// create a sphere
// 100% osg stuff
osg::Geode* osgSphere( const osg::Vec3& center, const float& radius )
{
	osg::Sphere* sphere = new osg::Sphere(center, radius);
	osg::ShapeDrawable* shape = new osg::ShapeDrawable( sphere );
	shape->setColor( osg::Vec4( 1., 1., 0., 1. ) );
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable( shape );
	return ( geode );
}
//================================================================================

class OsgbBasicDemo : public EngineModule
{
public:
	OsgbBasicDemo()
	{
		myOsg = new OsgModule();
		ModuleServices::addModule(myOsg);
		myWorld = initBtPhysicsWorld();
	}

	virtual void initialize();
	virtual void update(const UpdateContext& context);

	btDynamicsWorld* initBtPhysicsWorld();

private:

	Ref<OsgModule> myOsg;
	btDynamicsWorld* myWorld;
	Ref<SceneNode> mySceneNode;
	Actor* myInteractor;
	OsgSceneObject* myOsgSceneObj;

	osg::MatrixTransform* myGround;

	osg::MatrixTransform* myCol;

	osgbDynamics::MotionState* myGroundMotionState;

	osgbDynamics::MotionState* myColMotionState;
	
};

// initial steps to create bullet dynamics world
// 100% bullet stuff
btDynamicsWorld* OsgbBasicDemo::initBtPhysicsWorld()
{
	btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher * dispatcher = new btCollisionDispatcher( collisionConfiguration );
	btBroadphaseInterface * inter = new btDbvtBroadphase();
	btConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDynamicsWorld * dw = new btDiscreteDynamicsWorld( dispatcher, inter, solver, collisionConfiguration );

	dw->setGravity( btVector3(0, -0.8, 0) );
 
	return ( dw );
}

void OsgbBasicDemo::initialize()
{
	osg::Group* root = new osg::Group;

	//create ground
	
	
	{
		osg::Vec3 halfLengths( 5., 0.05, 5. );
		osg::Vec3 center( 0., 0., -3. );
		btBoxShape* groundShape = new btBoxShape( osgbCollision::asBtVector3( halfLengths ) );

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin( osgbCollision::asBtVector3( center ) );

		myGround = new osg::MatrixTransform( osgbCollision::asOsgMatrix( groundTransform ) );
		myGround->addChild( osgBox( osg::Vec3(0,0,0), halfLengths ) );

		btScalar mass( 0.0 );
		btVector3 inertia( 0, 0, 0 );
		//
		myGroundMotionState = new osgbDynamics::MotionState();
		myGroundMotionState->setWorldTransform(groundTransform);
		myGroundMotionState->setTransform(myGround);
		btRigidBody::btRigidBodyConstructionInfo rb( mass, myGroundMotionState, groundShape, inertia );
		//*/
		btRigidBody* body = new btRigidBody( rb );

		myWorld->addRigidBody(body);

		root->addChild(myGround);
	}
	
	///create a few dynamic rigidbodies
	{
		btSphereShape* sphereShape = new btSphereShape( btScalar(0.1) );

		btTransform startTransform;

		btScalar	mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0,0,0);
		if (isDynamic)
			sphereShape->calculateLocalInertia(mass,localInertia);

		for (int i=0;i<10;i++) {
			osg::Vec3 center( btScalar(1.), btScalar(1.+i/5.0), btScalar(-3.) );
			startTransform.setIdentity();
			startTransform.setOrigin( osgbCollision::asBtVector3(center) );
			myCol = new osg::MatrixTransform( osgbCollision::asOsgMatrix(startTransform) );
			myCol->addChild( osgSphere( osg::Vec3(0,0,0), 0.1 ) );

			myColMotionState = new osgbDynamics::MotionState();
			myColMotionState->setWorldTransform(startTransform);
			myColMotionState->setTransform(myCol);
			btRigidBody::btRigidBodyConstructionInfo rb(mass, myColMotionState, sphereShape, localInertia);
			btRigidBody* body = new btRigidBody(rb);
			myWorld->addRigidBody(body);

			root->addChild( myCol );
		}
	}

	
	//mySceneNode = new SceneNode(getEngine());
    
	//mySceneNode->setPosition(0,20,0);
    //mySceneNode->setBoundingBoxVisible(true);
    //mySceneNode->setBoundingBoxVisible(false);
    //getEngine()->getScene()->addChild(mySceneNode);
	//getEngine()->getDefaultCamera()->lookAt(omicron::Vector3f(0,-1,-100), omicron::Vector3f(0,0,-1));
	//getEngine()->getDefaultCamera()->setProjection(90, 1, 1, 200);
	//getEngine()->getDefaultCamera()->focusOn(mySceneNode);
	getEngine()->getDefaultCamera()->setPosition(0,1,2);
	// Set the osg node as the root node
    myOsg->setRootNode(root);
}

void OsgbBasicDemo::update(const UpdateContext& context)
{
	myWorld->stepSimulation( context.dt, 4, context.dt/2. );
}

int main(int argc, char** argv)
{
	Application<OsgbBasicDemo> app("osgbBasicDemo");
	return omain(app, argc, argv);
}