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




#include "otbGDALImageIO.h"
#include "itkMacro.h"
#include <iostream>

int otbGDALImageIOTestCanRead(int itkNotUsed(argc), char* argv[])
{
  otb::GDALImageIO::Pointer lGDALImageIO = otb::GDALImageIO::New();
  bool                      lCanRead = lGDALImageIO->CanReadFile(argv[1]);
  if (lCanRead == false)
    {
    std::cerr << "Error otb::GDALImageIO : impossible to open the dataset: " << argv[1] << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
