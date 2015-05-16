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
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. Redistributions in binary 
 * form must reproduce the above copyright notice, this list of conditions and 
 * the following disclaimer in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE  GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * What's in this file
 *	OsgSceneObject: the connection point between an osg node and an omegalib
 *	scene node. OsgSceneObject is can be attached to an omegalib scene node and
 *	contains an osg node and transformation.
 *****************************************************************************/
#include "omegaOsg/OsgSceneObject.h"
#include "omega/SceneNode.h"

#include <osg/Node> 
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osg/ComputeBoundsVisitor>

#include "PointIntersector.h"

using namespace omegaOsg;

uint OsgSceneObject::SceneNodeHitPointsFlag = 1 << 16;

///////////////////////////////////////////////////////////////////////////////
OsgSceneObject::OsgSceneObject(osg::Node* node): myNode(node), myInitialized(false), myUseLocalTransform(false)
{
    // if node is already a matrix transform, do not parent it to another matrix transform and use
    // it directly. NOTE that this will RESET any transformation currently applied to the node, and
    // use the transformation of the SceneNode owning this object instead.
    myTransform = dynamic_cast<osg::MatrixTransform*>(node);
    if(myTransform == NULL)
    {
        myTransform = new osg::MatrixTransform();
        myTransform->addChild( node );
    }
    myTransform->setDataVariance( osg::Object::DYNAMIC );
    requestBoundingBoxUpdate();
}

///////////////////////////////////////////////////////////////////////////////
OsgSceneObject::~OsgSceneObject()
{
    myTransform = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void OsgSceneObject::onAttached(SceneNode* node)
{
    node->addListener(this);
    // Force and update of the visible and selected states for this object.
    onVisibleChanged(node, node->isVisible());
    onSelectedChanged(node, node->isSelected());
}

///////////////////////////////////////////////////////////////////////////////
void OsgSceneObject::onDetached(SceneNode* node)
{
    node->removeListener(this);
}

///////////////////////////////////////////////////////////////////////////////
void OsgSceneObject::update(const UpdateContext& context)
{
    SceneNode* node = getOwner();
    osg::Matrix oxform;
    if(myUseLocalTransform)
    {
        const Vector3f& pos = node->getPosition();
        const Vector3f& sc = node->getScale();
        const Quaternion& o = node->getOrientation();
        oxform.setTrans(pos.x(), pos.y(), pos.z());
        oxform.setRotate(osg::Quat(o.x(), o.y(), o.z(), o.w()));
        oxform.scale(sc.x(), sc.y(), sc.z());
    }
    else
    {
        const AffineTransform3& xform =  node->getFullTransform();
        const Matrix4f& m = xform.matrix();
        oxform.set(m.data());
    }
    myTransform->setMatrix( oxform );
    requestBoundingBoxUpdate();

    //const osg::BoundingSphere& bs = myNode->getBound();
    //Vector3f center(bs.center()[0], bs.center()[1], bs.center()[2]);

    //float fradius = bs.radius();
    //if(fradius == -1) fradius = 0;

    //Vector3f radius(fradius, fradius, fradius);

    //ofmsg("&OsgRenderable center: %1%, size: %2%", %center %bs.radius());

    //myBBox.setExtents(center - radius, center + radius);
}

///////////////////////////////////////////////////////////////////////////////
const AlignedBox3* OsgSceneObject::getBoundingBox()
{
    osg::ComputeBoundsVisitor cbv;
    myNode->accept(cbv);

    osg::BoundingBox& bb = cbv.getBoundingBox();

    const osg::BoundingSphere& bs = myTransform->getBound();
    Vector3f center(bs.center()[0], bs.center()[1], bs.center()[2]);

    float fradius = bs.radius();
    if(fradius == -1) fradius = 0;

    Vector3f radius(fradius, fradius, fradius);

    if(bb.valid())
    {
        Vector3f min(bb._min.x(), bb._min.y(), bb._min.z());
        Vector3f max(bb._max.x(), bb._max.y(), bb._max.z());
        myBBox.setExtents(min, max);
    }
    return &myBBox;
}

///////////////////////////////////////////////////////////////////////////////
void OsgSceneObject::onVisibleChanged(SceneNode* source, bool value) 
{
    myTransform->setNodeMask(value ? ~0 : 0);
}

///////////////////////////////////////////////////////////////////////////////
void OsgSceneObject::onSelectedChanged(SceneNode* source, bool value) 
{}

///////////////////////////////////////////////////////////////////////////////
bool OsgSceneObject::intersectRay(const Ray& ray, Vector3f* hitPoint)
{
    Vector3f rstart = ray.getOrigin();
    
    // Compute reasonable ray length based on distance between ray origin and
    // owner scene node center.
    Vector3f center = getOwner()->getBoundCenter();
    float dir = (ray.getOrigin() - center).norm();
    Vector3f rend = ray.getPoint(dir * 2);

    osg::Vec3d orig(rstart[0], rstart[1], rstart[2]);
    osg::Vec3d end(rend[0], rend[1], rend[2]);

    if(getOwner()->isFlagSet(SceneNodeHitPointsFlag))
    {
        Ref<PointIntersector> intersector = new PointIntersector( orig, end );
        osgUtil::IntersectionVisitor iv( intersector.get() );
        myTransform->accept(iv);

        if ( intersector->containsIntersections() )
        {
            PointIntersector::Intersection result = *(intersector->getIntersections().begin());
            osg::Vec3d intersect = result.getWorldIntersectPoint();

            hitPoint->x() = intersect[0];
            hitPoint->y() = intersect[1];
            hitPoint->z() = intersect[2];
            return true;
        }
        else
            return false;
    }
    else
    {
        // Do standard face / line intersection
        Ref<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(orig, end);
        osgUtil::IntersectionVisitor iv(lsi);

        myTransform->accept(iv);

        if(!lsi->containsIntersections()) return false;

        osgUtil::LineSegmentIntersector::Intersection i = lsi->getFirstIntersection();

        osg::Vec3d intersect = i.getWorldIntersectPoint();

        hitPoint->x() = intersect[0];
        hitPoint->y() = intersect[1];
        hitPoint->z() = intersect[2];
        return true;
    }

    // We should never get here.
    return false;
}
