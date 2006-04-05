#ifndef otbPrincipalImageView_txx
#define otbPrincipalImageView_txx

#include "otbPrincipalImageView.h"


namespace otb
{

template <class TPixel, class OverlayPixelType>
PrincipalImageView<TPixel, OverlayPixelType>::
//GLImageView(int x, int y, int w, int h, const char *l):
//VectorImageView<TPixel>(x, y, w, h, l), Fl_Gl_Window(x, y, w, h, l)
PrincipalImageView() : GLVectorImageViewBase<TPixel, OverlayPixelType>()
  {
  }

template <class TPixel, class OverlayPixelType>
PrincipalImageView<TPixel, OverlayPixelType>::
~PrincipalImageView()
{

}

template <class TPixel, class OverlayPixelType>
int
PrincipalImageView<TPixel, OverlayPixelType>::handle(int event)
  {

  int key;

  switch(event)
    {
    case FL_PUSH:
    case FL_DRAG:
      {
        int x = Fl::event_x();
        int y = Fl::event_y();
        IndexType lCenter;
        IndexType lCenterPointImage;
        lCenter[0] = x;
        lCenter[1] = y;
        //Position du centre de l'image en coordonnees image
        lCenterPointImage = this->WindowCoord2ImageCoord(lCenter);
        // Mise a jour de la fenetre Zoom
        this->GetViewer()->PrincipalAction(lCenterPointImage);
      }
      break;
    case FL_FOCUS :
                this->update();
                this->GetViewer()->PrincipalResize();
                return 1;
                break;
    case FL_HIDE :
                this->GetViewer()->Hide();
                return 0;
                break;
    case FL_RELEASE:
      return 0;
      break;

    case FL_KEYBOARD:
    case FL_SHORTCUT:
    key = Fl::event_text()[0];
      switch(key)
        {
        case 'u':
        case 'U':
              this->update();
              return 1;
              break;
            default:
              break;
      }
    default:
      break;
  }
  
  return 0;
  
}

 
}; //namespace
#endif

