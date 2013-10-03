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
 *	An OpenSceneGraph reader that uses the embedded omegalib image reader to
 *	load images.
 ******************************************************************************/
#include <omega.h>
#include <osg/Image>
#include <osg/Notify>

//#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <string.h>

//#include <osgDB/fstream>

#include "omegaOsg/ReaderFreeImage.h"
#include "omegaOsg/OsgModule.h"

using namespace omega;

///////////////////////////////////////////////////////////////////////////////
ReaderFreeImage::ReaderFreeImage()
{
    supportsExtension("tga","Tga Image format");
    supportsExtension("png","Png Image format");
    supportsExtension("jpg","Jpg Image format");
    supportsExtension("JPG","Jpg Image format");
    supportsExtension("GIF","Jpg Image format");
    supportsExtension("PNG","Jpg Image format");
}
        
///////////////////////////////////////////////////////////////////////////////
ReaderWriter::ReadResult ReaderFreeImage::readObject(std::istream& fin,const osgDB::ReaderWriter::Options* options) const
{
	return ReadResult::FILE_NOT_HANDLED;
}

///////////////////////////////////////////////////////////////////////////////
ReaderWriter::ReadResult ReaderFreeImage::readObject(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{
    return readImage(file, options);
}

///////////////////////////////////////////////////////////////////////////////
ReaderWriter::ReadResult ReaderFreeImage::readImage(std::istream& fin,const Options* options) const
{
	omega::Ref<omega::PixelData> img = omega::ImageUtils::loadImageFromStream(fin, "STREAM");
	if(img == NULL) return ReadResult::FILE_NOT_FOUND;
	osg::Image* pOsgImage = omegaOsg::OsgModule::pixelDataToOsg(img, true);

    ReadResult rr(pOsgImage);
    if(rr.validImage()) rr.getImage()->setFileName("STREAM");
    return rr;
}

///////////////////////////////////////////////////////////////////////////////
ReaderWriter::ReadResult ReaderFreeImage::readImage(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{
	String filePath = file;

	std::string ext = osgDB::getLowerCaseFileExtension(filePath);
    if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

    //std::string fileName = osgDB::findDataFile( file, options );
    //if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

	bool leaveMemoryAlone = false;

	omega::Ref<omega::PixelData> img = omega::ImageUtils::loadImage(filePath, false);
	if(img == NULL) return ReadResult::FILE_NOT_FOUND;

	// Create an osg Image from the pixel data. the osg image gets ownership 
	// of the pixel buffer, since the PixelData object will be destroyed at 
	// the end of this function.
	osg::Image* pOsgImage = omegaOsg::OsgModule::pixelDataToOsg(img, true);
	
	
    ReadResult rr(pOsgImage);
    if(rr.validImage()) rr.getImage()->setFileName(filePath);
    return rr;
}
       
///////////////////////////////////////////////////////////////////////////////
ReaderWriter::WriteResult ReaderFreeImage::writeImage(const osg::Image& image, std::ostream& fout, const Options*) const
{
	return WriteResult::NOT_IMPLEMENTED;
}
        
///////////////////////////////////////////////////////////////////////////////
ReaderWriter::WriteResult ReaderFreeImage::writeImage(const osg::Image& image, const std::string& fileName, const Options* options) const
{
	return WriteResult::NOT_IMPLEMENTED;
}
