/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "otbGlView.h"


#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>

#include <algorithm>
#include <cstring>
#include <cassert>

#include "itkRGBAPixel.h"

#include "otbGeoInterface.h"
#include "otbImage.h"
#include "otbImageFileWriter.h"
#include "otbImportImageFilter.h"


namespace otb
{

GlView::GlView()
  : m_Settings(ViewSettings::New()),
    m_Actors(),
    m_RenderingOrder()
{}

GlView::~GlView()
{}

void GlView::Initialize(unsigned int sx, unsigned int sy)
{
  m_Settings = ViewSettings::New();
 
  this->ClearActors();

  this->Resize(sx,sy);
}

void GlView::Resize(unsigned int sx, unsigned int sy)
{
 ViewSettings::SizeType size;
  size[0] = sx;
  size[1] = sy;
  m_Settings->SetViewportSize(size);
}

void GlView::BeforeRendering()
{
  // First, get all actors informed of new settings
  for(ActorMapType::iterator it = m_Actors.begin();
      it!=m_Actors.end();++it)
    {
    if(it->second->GetVisible())
      {
      it->second->ProcessViewSettings();
      }
    }

  // Second, get opengl ready

  // Reset viewport
  glViewport(0, 0, (GLint)m_Settings->GetViewportSize()[0], (GLint)m_Settings->GetViewportSize()[1]);

  // Clear back-buffer(s) before rendering
  glClear( GL_COLOR_BUFFER_BIT );

  // Setup projection according to view settings
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  double ulx = -1.0;
  double uly = -1.0;
  double lrx = 1.0;
  double lry = 1.0;
  
  m_Settings->GetViewportExtent(ulx,uly,lrx,lry);

  glOrtho(ulx, lrx, lry, uly, -1, 1);

  // std::cout
  //   << "glOrtho( "
  //   << ulx << ", " << lrx << ", "
  //   << lry << ", " << uly
  //   << ", -1, 1 )"
  //   << std::endl;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glPushMatrix();
  
  glTranslatef(m_Settings->GetRotationCenter()[0],m_Settings->GetRotationCenter()[1],0);
  glRotatef(-m_Settings->GetRotationAngle()*180/M_PI,0,0,1);
  glTranslatef(-m_Settings->GetRotationCenter()[0],-m_Settings->GetRotationCenter()[1],0);

  // glScalef(m_Settings->GetSpacing()[0],m_Settings->GetSpacing()[1],1);
}

void GlView::AfterRendering()
{
  // std::cout << "geometry-changed: FALSE" << std::endl;

  m_Settings->SetGeometryChanged(false);
  glPopMatrix();
}

std::string GlView::AddActor(ActorType * actor, const std::string& key)
{
  std::string newKey = key;

  if(newKey == "")
    {
    std::ostringstream oss;
    oss<<"Actor"<<m_Actors.size()+1<<std::endl;
    newKey = oss.str();
    }

  if(m_Actors.count(newKey)!=0)
    {
    itkExceptionMacro(<<"An actor with key "<<newKey<<" already exists, please choose another key");
    }

  actor->SetSettings(m_Settings);
  m_Actors[newKey] = actor;
  m_RenderingOrder.push_back(newKey);

  return newKey;
}

bool GlView::RemoveActor(const std::string & key)
{
  if(m_Actors.count(key) == 0)
    {
    return false;
    }

  m_Actors.erase(key);

  StringVectorType::iterator it = std::find(m_RenderingOrder.begin(),m_RenderingOrder.end(),key);

  if(it!=m_RenderingOrder.end())
    {
    m_RenderingOrder.erase(it);
    }
  return true;
}

void GlView::ClearActors()
{
  m_Actors.clear();
  m_RenderingOrder.clear();
}

GlView::ActorType::Pointer GlView::GetActor(const std::string & key) const
{
  // if(m_Actors.count(key)==0)
  //   {
  //   itkExceptionMacro(<<"No actor found with key"<<key);
  //   }


  ActorMapType::const_iterator it( m_Actors.find( key ) );

  if( it==m_Actors.end() )
    return ActorType::Pointer();


  assert( it->first==key );

  return it->second;
}


bool
GlView
::ContainsActor( const KeyType & key ) const
{
  return ActorMapType::const_iterator( m_Actors.find( key ) )!=m_Actors.end();
}


std::vector<std::string> GlView::GetActorsKeys() const
{
  std::vector<std::string> resp;

  for(ActorMapType::const_iterator it = m_Actors.begin();
      it!=m_Actors.end();++it)
    {
    resp.push_back(it->first);
    }

  return resp;
}


void GlView::LightRender()
{
  // std::cout
  //   << "otb::GlView@" << std::hex << this << std::dec
  //   << "::LightRender()" << std::endl;

  // std::cout << "{" << std::endl;

  // {
  // double ulx = 0.0;
  // double uly = 0.0;
  // double lrx = 0.0;
  // double lry = 0.0;

  // m_Settings->GetViewportExtent( ulx, uly, lrx, lry );

  // std::cout << "v-origin: " << ulx << ", " << uly << std::endl;
  // std::cout << "v-extent: " << lrx << ", " << lry << std::endl;
  // }

  // std::cout
  //   << "v-spacing: "
  //   << m_Settings->GetSpacing()[ 0 ] << ", "
  //   << m_Settings->GetSpacing()[ 1 ] << std::endl;

  // std::cout
  //   << "v-size: "
  //   << m_Settings->GetViewportSize()[ 0 ] << ", "
  //   << m_Settings->GetViewportSize()[ 1 ] << std::endl;

  for(StringVectorType::reverse_iterator it = m_RenderingOrder.rbegin();
      it!=m_RenderingOrder.rend();++it)
    {
    ActorMapType::iterator actIt = m_Actors.find(*it);

    if(actIt != m_Actors.end() && actIt->second->GetVisible())
      {
      actIt->second->Render();

      actIt->second->GeometryChangedOff();
      }
    }

  // std::cout << "}" << std::endl;
}

void GlView::HeavyRender()
{
  // TODO: Light render/ heavy render could be optimized
  for(StringVectorType::reverse_iterator it = m_RenderingOrder.rbegin();
      it!=m_RenderingOrder.rend();++it)
    {
    ActorMapType::iterator actIt = m_Actors.find(*it);

    if(actIt != m_Actors.end() && actIt->second->GetVisible())
      {
      actIt->second->UpdateData();
      }
    }

  this->LightRender();
}


void GlView::RotateRenderingOrder(bool down)
{
  if(m_RenderingOrder.size()>1)
    {
    if(down)
      {
      std::rotate(m_RenderingOrder.rbegin(),m_RenderingOrder.rbegin()+1,m_RenderingOrder.rend());
      }
    else
      {
      std::rotate(m_RenderingOrder.begin(),m_RenderingOrder.begin()+1,m_RenderingOrder.end());
      }
    }


}

// Move actor in rendering order
void GlView::MoveActorInRenderingOrder(std::string key, bool down)
{
  StringVectorType::iterator it = std::find(m_RenderingOrder.begin(),m_RenderingOrder.end(),key);

  if(it!=m_RenderingOrder.end())
    {
    if(down && it+1 != m_RenderingOrder.end())
      {
      std::swap(*it,*(it+1));
      }
    else if(!down && it != m_RenderingOrder.begin())
      {
      std::swap((*it),*(it-1));
      }
    }
}

// Move actor to the end of rendering order (either front if front
// is set to true or back if front is set to false)
void GlView::MoveActorToEndOfRenderingOrder(std::string key, bool front)
{
  StringVectorType::iterator it = std::find(m_RenderingOrder.begin(),m_RenderingOrder.end(),key);

  if(it!=m_RenderingOrder.end())
    {
    if(front)
      {
      m_RenderingOrder.erase(it);
      m_RenderingOrder.insert(m_RenderingOrder.begin(),key);
      }
    else
      {
      m_RenderingOrder.erase(it);
      m_RenderingOrder.push_back(key);
      }
    }
}

void
GlView
::SetRenderingOrder( const StringVectorType & keys, bool front )
{
  StringVectorType thisKeys( m_RenderingOrder );

  m_RenderingOrder.clear();

  for( StringVectorType::const_iterator it( keys.begin() );
       it!=keys.end();
       ++it )
    if( ContainsActor( *it ) )
      {
      m_RenderingOrder.push_back( *it );

      StringVectorType::iterator pos = find(thisKeys.begin(),thisKeys.end(),*it);
      if(pos!=thisKeys.end())
        {
        thisKeys.erase(pos);
        }
      }

  m_RenderingOrder.insert(
    front
    ? m_RenderingOrder.end()
    : m_RenderingOrder.begin(),
    thisKeys.begin(), 
    thisKeys.end()
  );
}


void
GlView
::SaveScreenshot( const std::string & filename ) const
{
  //
  // Check input(s).
  assert( !filename.empty() );

  //
  // Get size of viewport.
  assert( !m_Settings.IsNull() );

  ViewSettings::SizeType size( m_Settings->GetViewportSize() );

  assert( size[ 0 ]>0 && size[ 1 ]>0 );

  //
  // Define types.
  typedef itk::RGBPixel< GLubyte > RgbPixel;
  typedef otb::Image< RgbPixel > RgbImage;
  typedef otb::ImportImageFilter< RgbImage > ImportRgbImageFilter;
  typedef otb::ImageFileWriter< RgbImage > RgbImageFileWriter;

  //
  // Setup OpenGL pixel storage.
  glPixelStorei( GL_PACK_ALIGNMENT, 1 );

  //
  // Check against OpenGL error and throw exception if any.
  assert( glGetError()==GL_NO_ERROR );

  if( glGetError()!=GL_NO_ERROR )
    throw std::runtime_error(
      std::string(
	reinterpret_cast< const char * >(
	  gluErrorString(
	    glGetError()
	  )
	)
      )
    );

  //
  // Read OpenGL pixel buffer.
  unsigned long count = size[ 0 ] * size[ 1 ];

  assert( RgbPixel::Length==3 );
  assert( sizeof( GLubyte )==1 );

  RgbPixel::ValueType * glBuffer = new RgbPixel::ValueType[ RgbPixel::Length  * count ];
  assert( glBuffer!=NULL );

  glReadPixels( 
    0, 0,
    size[ 0 ], size[ 1 ],
    GL_RGB, GL_UNSIGNED_BYTE,
    glBuffer
  );

  //
  // Check against OpenGL error and throw exception if any.
  assert( glGetError()==GL_NO_ERROR );

  if( glGetError()!=GL_NO_ERROR )
    throw std::runtime_error(
      std::string(
	reinterpret_cast< const char * >(
	  gluErrorString(
	    glGetError()
	  )
	)
      )
    );

  //
  // Copy & flip OpenGL pixel buffer into itk::RGBAPixel<> buffer.
  RgbPixel * itkBuffer = new RgbPixel[ count ];
  assert( itkBuffer );

  for( unsigned long j=0; j<size[ 1 ]; ++j )
    for( unsigned long i=0; i<size[ 0 ]; ++i )
      {
      unsigned long glOffset = RgbPixel::Length * ( size[ 0 ] * j + i );
      assert( glOffset < RgbPixel::Length * count );

      unsigned long itkOffset = size[ 0 ] * ( size[ 1 ] - 1 - j ) + i;
      assert( itkOffset<count );

      itkBuffer[ itkOffset ][ 0 ] = glBuffer[ glOffset + 0 ];
      itkBuffer[ itkOffset ][ 1 ] = glBuffer[ glOffset + 1 ];
      itkBuffer[ itkOffset ][ 2 ] = glBuffer[ glOffset + 2 ];
      }

  delete[] glBuffer;
  glBuffer = ITK_NULLPTR;

  //
  // Setup import RGBA-image filter.
  ImportRgbImageFilter::Pointer filter( ImportRgbImageFilter::New() );
  assert( !filter.IsNull() );

  ImportRgbImageFilter::RegionType::IndexType index;
  index.Fill( 0 );

  filter->SetRegion( ImportRgbImageFilter::RegionType( index, size ) );

  // Pass ownership of allocated buffer to ImportRgbImageFilter. So,
  // it will be deleted automatically in case an exception is thrown.
  filter->SetImportPointer(
    itkBuffer,
    count,
    true
  );

  //
  // Setup RGBA-image file-writer.
  RgbImageFileWriter::Pointer writer( RgbImageFileWriter::New() );
  assert( !writer.IsNull() );

  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( filename );

  //
  // Run.
  writer->Update();

  //
  // Allocated buffer will be freed by RgbImportImageFilter.
  // delete[] itkBuffer;
  // itkBuffer = NULL;
}

}
