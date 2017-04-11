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

#ifndef otbWarpImageFilter_h
#define otbWarpImageFilter_h
#include "itkImageBase.h"
#include "itkImageToImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkPoint.h"

namespace otb
{

/** \class WarpImageFilter
 * \brief Warps an image using an input deformation field.
 *
 * WarpImageFilter warps an existing image with respect to
 * a given deformation field.
 *
 * A deformation field is represented as a image whose pixel type is some
 * vector type with at least N elements, where N is the dimension of
 * the input image. The vector type must support element access via operator
 * [].
 *
 * The output image is produced by inverse mapping: the output pixels
 * are mapped back onto the input image. This scheme avoids the creation of
 * any holes and overlaps in the output image.
 *
 * Each vector in the deformation field represent the distance between
 * a geometric point in the input space and a point in the output space such
 * that:
 *
 * \f[ p_{in} = p_{out} + d \f]
 *
 * Typically the mapped position does not correspond to an integer pixel
 * position in the input image. Interpolation via an image function
 * is used to compute values at non-integer positions. The default
 * interpolation typed used is the LinearInterpolateImageFunction.
 * The user can specify a particular interpolation function via
 * SetInterpolator(). Note that the input interpolator must derive
 * from base class InterpolateImageFunction.
 *
 * Position mapped to outside of the input image buffer are assigned
 * a edge padding value.
 *
 * The LargetPossibleRegion for the output is inherited
 * from the input deformation field. The output image
 * spacing, origin and orientation may be set via
 * SetOutputSpacing, SetOutputOrigin and
 * SetOutputDirection. The default are respectively a
 * vector of 1's, a vector of 0's and an identity matrix.
 *
 * This class is templated over the type of the input image, the
 * type of the output image and the type of the deformation field.
 *
 * The input image is set via SetInput. The input deformation field
 * is set via SetDisplacementField.
 *
 * This filter is implemented as a multithreaded filter.
 *
 * \warning This filter assumes that the input type, output type
 * and deformation field type all have the same number of dimensions.
 *
 * \ingroup GeometricTransforms MultiThreaded Streamed
 *
 * \ingroup OTBITK
 */
template <
  class TInputImage,
  class TOutputImage,
  class TDisplacementField
  >
class ITK_EXPORT WarpImageFilter :
    public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef WarpImageFilter                              Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef itk::SmartPointer<Self>                           Pointer;
  typedef itk::SmartPointer<const Self>                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( WarpImageFilter, ImageToImageFilter );

  /** Typedef to describe the output image region type. */
  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Inherit some types from the superclass. */
  typedef typename Superclass::InputImageType         InputImageType;
  typedef typename Superclass::InputImagePointer      InputImagePointer;
  typedef typename Superclass::OutputImageType        OutputImageType;
  typedef typename Superclass::OutputImagePointer     OutputImagePointer;
  typedef typename Superclass::InputImageConstPointer InputImageConstPointer;
  typedef typename OutputImageType::IndexType         IndexType;
  typedef typename OutputImageType::IndexValueType    IndexValueType;
  typedef typename OutputImageType::SizeType          SizeType;
  typedef typename OutputImageType::PixelType         PixelType;
  typedef typename OutputImageType::SpacingType       SpacingType;

  /** Determine the image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension );
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  itkStaticConstMacro(DisplacementFieldDimension, unsigned int,
                      TDisplacementField::ImageDimension );
  /** typedef for base image type at the current ImageDimension */
  typedef itk::ImageBase<itkGetStaticConstMacro(ImageDimension)> ImageBaseType;

  /** Displacement field typedef support. */
  typedef TDisplacementField                        DisplacementFieldType;
  typedef typename DisplacementFieldType::Pointer   DisplacementFieldPointer;
  typedef typename DisplacementFieldType::PixelType DisplacementType;

  /** Interpolator typedef support. */
  typedef double                                                CoordRepType;
  typedef itk::InterpolateImageFunction<InputImageType,CoordRepType> InterpolatorType;
  typedef typename InterpolatorType::Pointer                    InterpolatorPointer;
  typedef itk::LinearInterpolateImageFunction<InputImageType,CoordRepType>
                                                                DefaultInterpolatorType;

  /** Point type */
  typedef itk::Point<CoordRepType,itkGetStaticConstMacro(ImageDimension)> PointType;

  /** Type for representing the direction of the output image */
  typedef typename TOutputImage::DirectionType     DirectionType;

  /** Set the deformation field. */
  void SetDisplacementField( const DisplacementFieldType * field );

  /** Get a pointer the deformation field. */
  DisplacementFieldType * GetDisplacementField(void);

  /** Set the interpolator function. */
  itkSetObjectMacro( Interpolator, InterpolatorType );

  /** Get a pointer to the interpolator function. */
  itkGetObjectMacro( Interpolator, InterpolatorType );

  /** Set the output image spacing. */
  itkSetMacro(OutputSpacing, SpacingType);
  virtual void SetOutputSpacing( const double* values);

  /** Get the output image spacing. */
  itkGetConstReferenceMacro(OutputSpacing, SpacingType);

  /** Set the output image origin. */
  itkSetMacro(OutputOrigin, PointType);
  virtual void SetOutputOrigin( const double* values);

  /** Get the output image origin. */
  itkGetConstReferenceMacro(OutputOrigin, PointType);

  /** Set/Get the direction (orientation) of the output image */
  itkSetMacro(OutputDirection, DirectionType );
  itkGetConstReferenceMacro(OutputDirection, DirectionType );

  /** Helper method to set the output parameters based on this image */
  void SetOutputParametersFromImage ( const ImageBaseType *image );

  /** Set the start index of the output largest possible region.
   * The default is an index of all zeros. */
  itkSetMacro( OutputStartIndex, IndexType );

  /** Get the start index of the output largest possible region. */
  itkGetConstReferenceMacro( OutputStartIndex, IndexType );

  /** Set the size of the output image. */
  itkSetMacro( OutputSize, SizeType );

  /** Get the size of the output image. */
  itkGetConstReferenceMacro( OutputSize, SizeType );

  /** Set the edge padding value */
  itkSetMacro( EdgePaddingValue, PixelType );

  /** Get the edge padding value */
  itkGetConstMacro( EdgePaddingValue, PixelType );

  /** WarpImageFilter produces an image which is a different
   * size than its input image. As such, it needs to provide an
   * implementation for GenerateOutputInformation() which set
   * the output information according the OutputSpacing, OutputOrigin
   * and the deformation field's LargestPossibleRegion. */
  void GenerateOutputInformation() ITK_OVERRIDE;

  /** It is difficult to compute in advance the input image region
   * required to compute the requested output region. Thus the safest
   * thing to do is to request for the whole input image.
   *
   * For the deformation field, the input requested region
   * set to be the same as that of the output requested region. */
  void GenerateInputRequestedRegion() ITK_OVERRIDE;

  /** This method is used to set the state of the filter before
   * multi-threading. */
  void BeforeThreadedGenerateData() ITK_OVERRIDE;

  /** This method is used to set the state of the filter after
   * multi-threading. */
  void AfterThreadedGenerateData() ITK_OVERRIDE;

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimensionCheck1,
    (itk::Concept::SameDimension<ImageDimension, InputImageDimension>));
  itkConceptMacro(SameDimensionCheck2,
    (itk::Concept::SameDimension<ImageDimension, DisplacementFieldDimension>));
  /** itkConceptMacro(InputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<typename TInputImage::PixelType>)); */
  itkConceptMacro(DisplacementFieldHasNumericTraitsCheck,
                  (itk::Concept::HasNumericTraits<typename TDisplacementField::PixelType::ValueType>));
  /** End concept checking */
#endif

protected:
  WarpImageFilter();
  ~WarpImageFilter() ITK_OVERRIDE {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const ITK_OVERRIDE;

  /** WarpImageFilter is implemented as a multi-threaded filter.
   * As such, it needs to provide and implementation for
   * ThreadedGenerateData(). */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            itk::ThreadIdType threadId ) ITK_OVERRIDE;

  /** Override VerifyInputInformation() since this filter's inputs do
    * not need to occupy the same physical space.
    *
    * \sa ProcessObject::VerifyInputInformation
    */
   void VerifyInputInformation() ITK_OVERRIDE {}

private:

  WarpImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** This function should be in an interpolator but none of the ITK
   * interpolators at this point handle edge conditions properly
   */
  DisplacementType EvaluateDisplacementAtPhysicalPoint(const PointType &p, const DisplacementFieldType *fieldPtr);

  PixelType                  m_EdgePaddingValue;
  SpacingType                m_OutputSpacing;
  PointType                  m_OutputOrigin;
  DirectionType              m_OutputDirection;

  InterpolatorPointer        m_Interpolator;
  SizeType                   m_OutputSize;        // Size of the output image
  IndexType                  m_OutputStartIndex;  // output image start index
  bool                       m_DefFieldSizeSame;
  // variables for deffield interpoator
  IndexType m_StartIndex,m_EndIndex;


};

} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbWarpImageFilter.txx"
#endif

#endif
