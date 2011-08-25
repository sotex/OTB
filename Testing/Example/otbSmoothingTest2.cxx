/*=========================================================================

 Program:   ORFEO Toolbox
 Language:  C++
 Date:      $Date$
 Version:   $Revision$


 Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
 See OTBCopyright.txt for details.


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/
#include "otbWrapperApplication.h"
#include "otbWrapperApplicationRegistry.h"

using otb::Wrapper::Application;
using otb::Wrapper::ApplicationRegistry;

int otbSmoothingTest2(int argc, char* argv[])
{
  const char* in = argv[1];
  const char* out = argv[2];
  ApplicationRegistry::SetApplicationPath(argv[3]);

  Application::Pointer app = ApplicationRegistry::CreateApplication("Smoothing");

  app->SetParameterString("in", in );
  app->SetParameterString("out", out );
  app->SetParameterString("type", "mean");
  app->ExecuteAndWriteOutput();
  return EXIT_SUCCESS;
}
