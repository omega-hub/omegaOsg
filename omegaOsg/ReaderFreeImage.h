#include <osg/Image>
#include <osg/Notify>
/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory, University of Illinois at Chicago
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
#ifndef __READER_FREEIMAGE_H__
#define __READER_FREEIMAGE_H__

#include "oosgbase.h"
#include <osgDB/fstream>
#include <osg/Image>
#include <osgDB/ReaderWriter>
#include <osgDB/Registry>

using namespace osgDB;

///////////////////////////////////////////////////////////////////////////////////////////////////
class OOSG_API ReaderFreeImage : public osgDB::ReaderWriter
{
public:
        ReaderFreeImage();
        virtual const char* className() const { return "FreeImage Image Reader"; }
        ReadResult readStream(std::istream& fin) const;
        virtual ReadResult readObject(std::istream& fin,const osgDB::ReaderWriter::Options* options =NULL) const;
        virtual ReadResult readObject(const std::string& file, const osgDB::ReaderWriter::Options* options =NULL) const;
        virtual ReadResult readImage(std::istream& fin,const Options* =NULL) const;
        virtual ReadResult readImage(const std::string& file, const osgDB::ReaderWriter::Options* options) const;
        bool saveStream(const osg::Image& image, std::ostream& fout) const;
        virtual WriteResult writeImage(const osg::Image& image, std::ostream& fout, const Options*) const;
        virtual WriteResult writeImage(const osg::Image& image, const std::string& fileName, const Options* options) const;
};

#endif