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

osg:: ref_ptr<osg::Node> createAxes( void ) {

        // This method should be made more succinct by using arrays.

        const osg::Vec4f red( 1.0, 0.0, 0.0, 1.0 ), green( 0.0, 1.0, 0.0, 1.0 ), blue ( 0.0, 0.0, 1.0, 1.0 );
        osg::Matrix mR, mT;

        osg::ref_ptr< osg::Group > root = new osg::Group;
        osg::ref_ptr< osg::MatrixTransform > mt = NULL;
        osg::ref_ptr< osg::Geode > geode = NULL;

        osg::ref_ptr< osg::Cylinder > axis = new osg::Cylinder( osg::Vec3f( 0.0, 0.0, 0.0 ), 2.0, 500.0 );
        osg::ref_ptr< osg::Cone > arrow = new osg::Cone( osg::Vec3f( 0.0, 0.0, 0.0 ), 10.0, 20.0 );

        // Draw the X axis in Red

        osg::ref_ptr< osg::ShapeDrawable > xAxis = new osg::ShapeDrawable( axis );
        xAxis->setColor( red );
        geode = new osg::Geode;
        geode->addDrawable( xAxis.get( ) );
        mT.makeTranslate( 250.0, 0.0, 0.0 );
        mR.makeRotate( 3.14159 / 2.0, osg::Vec3f( 0.0, 1.0, 0.0 ) );
        mt = new osg::MatrixTransform;
        mt->setMatrix( mR * mT );
        mt->addChild( geode.get( ) );
        root->addChild( mt.get( ) );

        osg::ref_ptr< osg::ShapeDrawable > xArrow= new osg::ShapeDrawable( arrow );
        xArrow->setColor( red );
        geode = new osg::Geode;
        geode->addDrawable( xArrow.get( ) );
        mT.makeTranslate( 500.0, 0.0, 0.0 );
        mR.makeRotate( 3.14159 / 2.0, osg::Vec3f( 0.0, 1.0, 0.0 ) );
        mt = new osg::MatrixTransform;
        mt->setMatrix( mR * mT );
        root->addChild( mt.get( ) );
        mt->addChild( geode.get( ) );

        // Then the Y axis in green
        osg::ref_ptr< osg::ShapeDrawable > yAxis = new osg::ShapeDrawable( axis );
        yAxis->setColor( green );
        geode = new osg::Geode;
        geode->addDrawable( yAxis.get( ) );
        mT.makeTranslate( 0.0, 250.0, 0.0 );
        mR.makeRotate( 3.14159 / 2.0, osg::Vec3f( 1.0, 0.0, 0.0 ) );
        mt = new osg::MatrixTransform;
        mt->setMatrix( mR * mT );
        mt->addChild( geode.get( ) );
        root->addChild( mt.get( ) );

        osg::ref_ptr< osg::ShapeDrawable > yArrow= new osg::ShapeDrawable( arrow );
        yArrow->setColor( green );
        geode = new osg::Geode;
        geode->addDrawable( yArrow.get( ) );
        mT.makeTranslate( 0.0, 500.0, 0.0 );
        mR.makeRotate( 3.14159 / 2.0, osg::Vec3f( -1.0, 0.0, 0.0 ) );
        mt = new osg::MatrixTransform;
        mt->setMatrix( mR * mT );
        root->addChild( mt.get( ) );
        mt->addChild( geode.get( ) );

        // And the Z axis in blue

        osg::ref_ptr< osg::ShapeDrawable > zAxis = new osg::ShapeDrawable( axis );
        zAxis->setColor( blue );
        geode = new osg::Geode;
        geode->addDrawable( zAxis.get( ) );
        mT.makeTranslate( 0.0, 0.0, 250.0 );
        //mR.makeRotate( 3.14159 / 2.0, osg::Vec3f( 0.0, 1.0, 0.0 ) );
        mt = new osg::MatrixTransform;
        mt->setMatrix( mT );
        mt->addChild( geode.get( ) );
        root->addChild( mt.get( ) );

        osg::ref_ptr< osg::ShapeDrawable > zArrow= new osg::ShapeDrawable( arrow );
        zArrow->setColor( blue );
        geode = new osg::Geode;
        geode->addDrawable( zArrow.get( ) );
        mT.makeTranslate( 0.0, 0.0, 500.0 );
        //mR.makeRotate( 3.14159 / 2.0, osg::Vec3f( 0.0, 1.0, 0.0 ) );
        mt = new osg::MatrixTransform;
        mt->setMatrix(mT );
        root->addChild( mt.get( ) );
        mt->addChild( geode.get( ) );

        return root.get( );

} // createAxes

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

//================================================================================

class OsgbBasicDemo : public EngineModule
{
public:
	OsgbBasicDemo()
	{
		myOsgModule = new OsgModule();
		ModuleServices::addModule(myOsgModule);
		myWorld = initBtPhysicsWorld();
	}

	virtual void initialize();
	virtual void update(const UpdateContext& context);

	btDynamicsWorld* initBtPhysicsWorld();

private:

	Ref<OsgModule> myOsgModule;
	btDynamicsWorld* myWorld;
	Ref<SceneNode> mySceneNode;
	Actor* myInteractor;
	OsgSceneObject* myOsgSceneObj;

	osg::MatrixTransform* myGround;

	osg::MatrixTransform* myBox;

	osgbDynamics::MotionState* myGroundMotionState;

	osgbDynamics::MotionState* myBoxMotionState;

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

	//dw->setDebugDrawer(&gDebugDraw);
	dw->setGravity( btVector3(0, -10, 0) );

	return ( dw );
}

void OsgbBasicDemo::initialize()
{
	osg::Group* root = new osg::Group;

	//create a ground
	{
		osg::Vec3 halfLengths( 50., 50., 50. );
		osg::Vec3 center( 0., -50., 0 );

		btBoxShape* groundShape = new btBoxShape( osgbCollision::asBtVector3( halfLengths ) );

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin( osgbCollision::asBtVector3( center ) );

		myGround = new osg::MatrixTransform( osgbCollision::asOsgMatrix( groundTransform ) );
		myGround->addChild( osgBox( osg::Vec3(0,0,0), halfLengths ) );

		btScalar mass( 0.0 );
		btVector3 inertia( 0, 0, 0 );

		myGroundMotionState = new osgbDynamics::MotionState();
		myGroundMotionState->setWorldTransform(groundTransform);
		myGroundMotionState->setTransform(myGround);
		btRigidBody::btRigidBodyConstructionInfo rb( mass, myGroundMotionState, groundShape, inertia );
		btRigidBody* body = new btRigidBody( rb );

		myWorld->addRigidBody(body);
	}

	root->addChild(myGround);

	//create a few dynamic rigidbodies
	// Re-using the same collision is better for memory usage and performance
	{

		osg::Vec3 halfLengths(SCALING*1, SCALING*1,SCALING*1);

		btBoxShape* boxShape = new btBoxShape( osgbCollision::asBtVector3( halfLengths ) );

		/// Create Dynamic Objects
		btTransform startTransform;

		btScalar	mass(1.f);

		btVector3 localInertia(0,0,0);

		boxShape->calculateLocalInertia(mass,localInertia);

		float start_x = START_POS_X - ARRAY_SIZE_X/2.0;
		float start_y = START_POS_Y;
		float start_z = START_POS_Z - ARRAY_SIZE_Z/2.0;

		mySceneNode = new SceneNode(getEngine());

		for (int k=0;k<ARRAY_SIZE_Y;k++) {
			for (int i=0;i<ARRAY_SIZE_X;i++) {
				for(int j = 0;j<ARRAY_SIZE_Z;j++) {
					// first one at (-7.5, 15, -5.5)
					osg::Vec3 center( btScalar(2.0*i + start_x), btScalar(20+2.0*k + start_y), btScalar(2.0*j + start_z) );

					startTransform.setIdentity();
					startTransform.setOrigin( SCALING * osgbCollision::asBtVector3(center) );

					myBox = new osg::MatrixTransform( osgbCollision::asOsgMatrix(startTransform) );
					myBox->addChild( osgBox( osg::Vec3(0,0,0), halfLengths ) );

					root->addChild( myBox );

					myBoxMotionState = new osgbDynamics::MotionState();
					myBoxMotionState->setWorldTransform(startTransform);
					myBoxMotionState->setTransform(myBox);
					btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myBoxMotionState, boxShape, localInertia);
					btRigidBody* body = new btRigidBody(rbInfo);
					myWorld->addRigidBody(body);
				}
			}
		}
	}

	mySceneNode->setPosition(0,20,0);
    mySceneNode->setBoundingBoxVisible(true);
    //mySceneNode->setBoundingBoxVisible(false);
    getEngine()->getScene()->addChild(mySceneNode);
	//getEngine()->getDefaultCamera()->lookAt(omicron::Vector3f(0,-1,-100), omicron::Vector3f(0,0,-1));
	//getEngine()->getDefaultCamera()->setProjection(90, 1, 1, 200);
	getEngine()->getDefaultCamera()->setPosition(0,20,80);
	getEngine()->getDefaultCamera()->setOrientation(0.993938, -0.109776, -0.005964, -0.000659);

    // Set the interactor style used to manipulate meshes.
    if(SystemManager::settingExists("config/interactor"))
    {
        Setting& sinteractor = SystemManager::settingLookup("config/interactor");
        myInteractor = ToolkitUtils::createInteractor(sinteractor);
        if(myInteractor != NULL)
        {
            ModuleServices::addModule(myInteractor);
        }
    }

    if(myInteractor != NULL)
    {
        myInteractor->setSceneNode(mySceneNode);
    }

	//printf("Scene Node: (%lf, %lf, %lf)\n", mySceneNode->getPosition().x(), mySceneNode->getPosition().y(), mySceneNode->getPosition().z());

	root->addChild( createAxes( ).get( ) );

	// Set the osg node as the root node
    myOsgModule->setRootNode(root);

}

void OsgbBasicDemo::update(const UpdateContext& context)
{
	double elapsed = 1./30.;
	myWorld->stepSimulation( elapsed, 4, elapsed/2. );
}

int main(int argc, char** argv)
{
	Application<OsgbBasicDemo> app("osgbBasicDemo");
	return omain(app, argc, argv);
}
