#include "eschart_pch.h"
#pragma hdrstop

#include <plstream.h>
#include "EsChPlotStreamAdapter.h"
//--------------------------------------------------------------------------------

EsChPlotStreamAdapter::EsChPlotStreamAdapter(EsChPlot& plot) :
  m_plot(plot),
  plstream()
{}
//--------------------------------------------------------------------------------

// Necessary part for PLD_wxwidgets driver
// taken from PLplot driver/wxwidgets.cpp
// Use C linkage
// 
// wxwidgets headers
#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/except.h>

#include "plDevs.h"

// plplot headers
#include <plplotP.h>
#include <drivers.h>
#include <disptab.h>

// Private copy of PLplot wxdriver declaration
#include "PLwxwidgets.h"

//--------------------------------------------------------------------------
//  plD_init_wxwidgets( PLStream* pls )
//
//  Initialize wxWidgets device.
//--------------------------------------------------------------------------
void plD_init_wxwidgets(PLStream* pls)
{
  ES_DEBUG_TRACE(esT("plD_init_wxwidgets(): enter"));

  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(device)
      EsException::Throw(esT("plD_init_wxwidgets called when a initialization has already occurred."));

    // default options
    static PLINT text = 1;
    static PLINT hrshsym = 0;
    static char* mfo = NULL;

    DrvOpt       wx_options[] = {
        { "hrshsym", DRV_INT, &hrshsym, "Use Hershey symbol set (hrshsym=0|1)" },
        { "text",    DRV_INT, &text,    "Use own text routines (text=0|1)"     },
        { "mfo",     DRV_STR, &mfo,     "output metafile"                      },
        { NULL,      DRV_INT, NULL,     NULL                                   }
    };

    // Check for and set up driver options
    plParseDrvOpts(wx_options);

    // by default the own text routines are used for wxDC
    if(text == -1)
      text = 0;

  // create the new device
    device = new wxPLDevice(pls, mfo, text, hrshsym);

    // If portrait mode, apply a rotation and set freeaspect
    if(pls->portrait)
    {
      plsdiori((PLFLT)(4 - ORIENTATION));
      pls->freeaspect = 1;
    }

    //indicate that we support getting string length
    pls->has_string_length = 1;

    //indicate that we support xor rendering
    pls->dev_xor = 1;
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_init_wxwidgets.");
    pls->dev = NULL;
  }
  catch(...)
  {
    plabort("unknown error in plD_init_wxwidgets.");
  }
  ES_DEBUG_TRACE(esT("plD_init_wxwidgets(): leave"));
}

//--------------------------------------------------------------------------
//  void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a,
//													 short x2a, short y2a )
//
//  Draws a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void plD_line_wxwidgets(PLStream* pls, short x1a, short y1a, short x2a, short y2a)
{
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      EsException::Throw(esT("plD_line_wxwidgets called before initialization."));
    device->DrawLine(x1a, y1a, x2a, y2a);
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_line_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_line_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya,
//															 PLINT npts )
//
//  Draw a poly line - points are in xa and ya arrays.
//--------------------------------------------------------------------------
void plD_polyline_wxwidgets(PLStream* pls, short* xa, short* ya, PLINT npts)
{
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      EsException::Throw(esT("plD_polyline_wxwidgets called before initialization."));
    device->DrawPolyline(xa, ya, npts);
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_polyline_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_polyline_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_eop_wxwidgets( PLStream *pls )
//
//  End of Page. This function is called if a "end of page" is send by the
//  user. This command is ignored if we have the plot embedded in a
//  wxWidgets application, otherwise the application created by the device
//  takes over.
//--------------------------------------------------------------------------
void plD_eop_wxwidgets(PLStream* pls)
{
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      EsException::Throw(esT("plD_eop_wxwidgets called before initialization."));
    if(pls->nopause)
      device->EndPage(pls);
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_eop_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_eop_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_wait_wxwidgets( PLStream *pls )
//
//  Wait for user input. This command is ignored if we have the plot embedded in a
//  wxWidgets application, otherwise the application created by the device
//  takes over.
//--------------------------------------------------------------------------
void plD_wait_wxwidgets(PLStream* pls)
{
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      EsException::Throw(esT("plD_wait_wxwidgets called before initialization."));

    device->EndPage(pls);
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_wait_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_wait_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_bop_wxwidgets( PLStream *pls )
//
//  Begin of page. Before any plot command, this function is called, If we
//  have already a dc the background is cleared in background color and some
//  state calls are resent - this is because at the first call of this
//  function, a dc does most likely not exist, but state calls are recorded
//  and when a new dc is created this function is called again.
//--------------------------------------------------------------------------
void plD_bop_wxwidgets(PLStream* pls)
{
    // PLPLOT_wxLogDebug( "plD_bop_wxwidgets()" );
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      EsException::Throw(esT("plD_bop_wxwidgets called before initialization."));
    device->BeginPage(pls);
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_bop_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_bop_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_tidy_wxwidgets( PLStream *pls )
//
//  This function is called, if all plots are done.
//--------------------------------------------------------------------------
void plD_tidy_wxwidgets(PLStream* pls)
{
  if(!pls->dev)
    return;
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(device)
    {
      device->PreDestructorTidy(pls);
      delete device;
    }
    pls->dev = NULL;         //so it doesn't get freed elsewhere
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_tidy_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_tidy_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_state_wxwidgets( PLStream *pls, PLINT op )
//
//  Handler for several state codes. Here we take care of setting the width
//  and color of the pen.
//--------------------------------------------------------------------------
void plD_state_wxwidgets(PLStream* pls, PLINT op)
{
  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      return;             //we can call state functions before initialization so just return
    switch(op)
    {
    case PLSTATE_WIDTH:         // 1
      device->SetWidth(pls);
      break;

    case PLSTATE_COLOR0:         // 2
      device->SetColor(pls);
      break;

    case PLSTATE_COLOR1:         // 3
      device->SetColor(pls);
      break;
    }
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_state_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_state_wxwidgets.");
  }
}

//--------------------------------------------------------------------------
//  void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
//
//  Handler for several escape codes. Here we take care of filled polygons,
//  XOR or copy mode, initialize device (install dc from outside), and if
//  there is freetype support, re-rendering of text.
//--------------------------------------------------------------------------
void plD_esc_wxwidgets(PLStream* pls, PLINT op, void* ptr)
{
  if(!pls->dev)
    return;

  try
  {
    wxPLDevice* device = (wxPLDevice*)pls->dev;
    if(!device)
      EsException::Throw(esT("plD_esc_wxwidgets called before initialization."));
    switch(op)
    {
    case PLESC_FILL:
      device->FillPolygon(pls);
      break;

    case PLESC_XORMOD:
      device->SetXorMode(*((PLINT*)ptr) == 0 ? false : true);
      break;

    case PLESC_DEVINIT:
      device->SetDC(pls, (wxDC*)ptr);
      break;

    case PLESC_HAS_TEXT:
      device->drawText(pls, (EscText*)ptr);
      break;

    case PLESC_RESIZE:
    {
      wxSize* size = (wxSize*)ptr;
      device->SetSize(pls, size->GetWidth(), size->GetHeight());
    }
    break;

    case PLESC_CLEAR:
      device->ClearBackground(pls, pls->sppxmi, pls->sppymi,
          pls->sppxma, pls->sppyma);
      break;

    case PLESC_FLUSH:                // forced update of the window
      device->Flush(pls);
      break;

    case PLESC_GETC:
      device->Locate(pls, (PLGraphicsIn*)ptr);
      break;

    case PLESC_FIXASPECT:
      device->FixAspectRatio(*((bool*)ptr));
      break;

    default:
      break;
    }
  }
  catch(const char* message)
  {
    plabort(message);
    plabort("error in plD_tidy_wxwidgets.");
  }
  catch(...)
  {
    plabort("unknown error in plD_tidy_wxwidgets.");
  }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------
//  void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
//
//  Make wxwidgets driver functions known to plplot.
//--------------------------------------------------------------------------
void plD_dispatch_init_wxwidgets(PLDispatchTable* pdt)
{
  ES_ASSERT(pdt);

  pdt->pl_MenuStr = "wxWidgets DC";
  pdt->pl_DevName = "wxwidgets";
  pdt->pl_type = plDevType_Interactive;
  pdt->pl_seq = 51;
  pdt->pl_init = (plD_init_fp)plD_init_wxwidgets;
  pdt->pl_line = (plD_line_fp)plD_line_wxwidgets;
  pdt->pl_polyline = (plD_polyline_fp)plD_polyline_wxwidgets;
  pdt->pl_eop = (plD_eop_fp)plD_eop_wxwidgets;
  pdt->pl_wait = (plD_wait_fp)plD_wait_wxwidgets;
  pdt->pl_bop = (plD_bop_fp)plD_bop_wxwidgets;
  pdt->pl_tidy = (plD_tidy_fp)plD_tidy_wxwidgets;
  pdt->pl_state = (plD_state_fp)plD_state_wxwidgets;
  pdt->pl_esc = (plD_esc_fp)plD_esc_wxwidgets;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// wxwidgets headers
#include <wx/dir.h>
#include <wx/ustring.h>

//--------------------------------------------------------------------------
// PlDevice::PlDevice()
//
// Constructor for wxPLDevice
//--------------------------------------------------------------------------
PlDevice::PlDevice()
{
  m_prevSymbol = 0;
  m_prevBaseFontSize = 0.;
  m_prevLevel = 0;
  m_prevFci = 0;
  m_prevSymbolWidth = 0;
  m_prevSymbolHeight = 0;
}

//--------------------------------------------------------------------------
//  void wxPLDevice::DrawText( PLStream* pls, EscText* args )
//
//  This is the main function which processes the unicode text strings.
//  Font size, rotation and color are set, width and height of the
//  text string is determined and then the string is drawn to the canvas.
//--------------------------------------------------------------------------
void PlDevice::drawText(PLStream* pls, EscText* args)
{
    // Split the text into lines separated by forced linebreak '\n' characters
    // inserted by the user.
  typedef std::pair< PLUNICODE*, PLUNICODE*>   uniIterPair;
  PLUNICODE* textEnd = args->unicode_array + args->unicode_array_len;
  PLUNICODE lf = PLUNICODE('\n');
  std::vector< uniIterPair > lines(1, uniIterPair(args->unicode_array, textEnd));
  for(PLUNICODE* uni = args->unicode_array; uni != textEnd; ++uni)
  {
    if(*uni == lf)
    {
      lines.back().second = uni;
      lines.push_back(uniIterPair(uni + 1, textEnd));
    }
  }

  // Check that we got unicode, warning message and return if not
  if(args->unicode_array_len == 0)
  {
    printf("Non unicode string passed to the wxWidgets driver, ignoring\n");
    return;
  }

  // Check that unicode string isn't longer then the max we allow
  if(args->unicode_array_len >= 500)
  {
    printf("Sorry, the wxWidgets drivers only handles strings of length < %d\n", 500);
    return;
  }

  // Calculate the font size (in pt)
  // PLplot saves it in mm (bizarre units!)
  PLFLT baseFontSize = pls->chrht * PLPLOT_POINTS_PER_INCH / PLPLOT_MM_PER_INCH;

  //initialize the text state
  PLUNICODE currentFci;
  plgfci(&currentFci);
  bool      currentUnderlined = false;

  //Get the size of each line. Even for left aligned text
  //we still need the text height to vertically align text
  std::vector<wxCoord> lineWidths(lines.size());
  std::vector<wxCoord> lineHeights(lines.size());
  std::vector<wxCoord> lineDepths(lines.size());
  {
      // Get the text length without drawing it.  Also, determine
      // lineWidths, lineHeights, and lineDepths arrays that are required
      // for the actual draw.
    wxCoord   paraWidth = 0;
    wxCoord   paraHeight = 0;
    PLUNICODE testFci = currentFci;
    bool      testUnderlined = currentUnderlined = false;
    PLFLT     identityMatrix[6];
    plP_affine_identity(identityMatrix);
    for(size_t i = 0; i < lines.size(); ++i)
    {
      DrawTextLine(lines[i].first, lines[i].second - lines[i].first, 0, 0, 0, 0, identityMatrix, baseFontSize, false,
          testUnderlined, testFci,
          0, 0, 0, 0.0, lineWidths[i], lineHeights[i], lineDepths[i]);
      paraWidth = MAX(paraWidth, lineWidths[i]);
      paraHeight += lineHeights[i] + lineDepths[i];
    }
    pls->string_length = paraWidth / pls->xpmm;
  }

  if(!pls->get_string_length)
  {
      // Draw the text string if requested by PLplot.  The needed lineWidths,
      // lineHeights, and lineDepths arrays are determined above.
    wxCoord cumSumHeight = 0;
    // Plplot doesn't include plot orientation in args->xform, so we must
    // rotate the text if needed;
    PLFLT textTransform[6];
    PLFLT diorot = pls->diorot - 4.0 * floor(pls->diorot / 4.0);       //put diorot in range 0-4
    textTransform[0] = args->xform[0];
    textTransform[2] = args->xform[1];
    textTransform[1] = args->xform[2];
    textTransform[3] = args->xform[3];
    textTransform[4] = 0.0;
    textTransform[5] = 0.0;
    PLFLT diorotTransform[6];
    if(diorot == 0.0)
    {
      diorotTransform[0] = 1;
      diorotTransform[1] = 0;
      diorotTransform[2] = 0;
      diorotTransform[3] = 1;
    }
    else if(diorot == 1.0)
    {
      diorotTransform[0] = 0;
      diorotTransform[1] = -1;
      diorotTransform[2] = 1;
      diorotTransform[3] = 0;
    }
    else if(diorot == 2.0)
    {
      diorotTransform[0] = -1;
      diorotTransform[1] = 0;
      diorotTransform[2] = 0;
      diorotTransform[3] = -1;
    }
    else if(diorot == 3.0)
    {
      diorotTransform[0] = 0;
      diorotTransform[1] = 1;
      diorotTransform[2] = -1;
      diorotTransform[3] = 0;
    }
    else
    {
      PLFLT angle = diorot * M_PI / 2.0;
      PLFLT cosAngle = cos(angle);
      PLFLT sinAngle = sin(angle);
      diorotTransform[0] = cosAngle;
      diorotTransform[1] = -sinAngle;
      diorotTransform[2] = sinAngle;
      diorotTransform[3] = cosAngle;
    }
    diorotTransform[4] = 0;
    diorotTransform[5] = 0;

    PLFLT finalTransform[6];
    memcpy(finalTransform, textTransform, sizeof(PLFLT) * 6);
    plP_affine_multiply(finalTransform, textTransform, diorotTransform);

    std::vector<wxCoord> lineWidths_ignored(lines.size());
    std::vector<wxCoord> lineHeights_ignored(lines.size());
    std::vector<wxCoord> lineDepths_ignored(lines.size());
    for(size_t i = 0; i < lines.size(); ++i)
    {
      DrawTextLine(lines[i].first, lines[i].second - lines[i].first,
          args->x,
          args->y,
          -lineWidths[i] * args->just, 0.5 * (lineHeights[i]) - cumSumHeight,
          finalTransform, baseFontSize, true,
          currentUnderlined,
          currentFci, pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a, lineWidths_ignored[i],
          lineHeights_ignored[i], lineDepths_ignored[i]);

      // Ignore the ignored versions even though gdb tells me
      // (AWI) they are the same as the unignored versions
      // determined above for the DrawText false case (as
      // expected from inspection of the DrawTextLine code).
      cumSumHeight += lineHeights[i] + lineDepths[i];
    }
  }
}

//  This function will draw a line of text given by ucs4 with ucs4Len
//  characters. The ucs4 argument must not contain any newline characters.
//  basefontSize is the size of a full size character in points. Pass
//  in underlined flag and fci for the beginning of the line. On
//  return they will be filled with the values at the end of the line.
//  On return textWidth, textHeigth and textDepth will be filled with
//  the width, ascender height and descender depth of the text string.
//  If drawText is true the text will actually be drawn. If it is
//  false the size will be calculated but the text will not be
//  rendered.

void PlDevice::DrawTextLine(PLUNICODE* ucs4, int ucs4Len, wxCoord xOrigin, wxCoord yOrigin, wxCoord x, wxCoord y, PLFLT* transform, PLFLT baseFontSize, bool drawText, bool& underlined, PLUNICODE& fci, unsigned char red, unsigned char green, unsigned char blue, PLFLT alpha, wxCoord& textWidth, wxCoord& textHeight, wxCoord& textDepth)
{
  PLINT level = 0;
  PLFLT oldScale;
  PLFLT Scale = 1.;
  PLFLT scaledFontSize = baseFontSize;
  PLFLT oldOffset;
  PLFLT Offset = 0.;
  PLFLT yScale;
  PLFLT scaledOffset = 0.;

  // Factor of 1.2 is an empirical correction to work around a bug
  // where the calculated symmetrical subscript and superscript
  // offset arguments of DrawTextSection are rendered in that
  // routine in an asymmetical way (with subscript levels having a
  // differently rendered offset than the corresponding superscript
  // level).  Of course, fixing this DrawTextSection bug is far
  // preferable to this workaround, but I have been unable to find
  // that bug in DrawTextSection so I am leaving this ultimate fix
  // until later.

  PLFLT   empiricalSymmetricFactor = 1.2;

  wxCoord sectionWidth;
  wxCoord sectionHeight;
  wxCoord sectionDepth;

  // check if we have the same symbol as last time - only do this for single characters
  // (e.g., typical plstring use).
  if(!drawText
       && ucs4Len == 1
       && ucs4[0] == m_prevSymbol
       && baseFontSize == m_prevBaseFontSize
       && level == m_prevLevel
       && fci == m_prevFci)
  {
    textWidth = m_prevSymbolWidth;
    textHeight = m_prevSymbolHeight;
    textDepth = m_prevSymbolDepth;
    return;
  }

  wxString section;

  PLFLT    sectionTransform[6];
  memcpy(sectionTransform, transform, sizeof(sectionTransform));

  // Get PLplot escape character
  char plplotEsc;
  plgesc(&plplotEsc);

  // Reset the size metrics
  textWidth = 0;
  textHeight = 0;
  textDepth = 0;

  int i = 0;
  while(i < ucs4Len)
  {
    if(ucs4[i] == (PLUNICODE)plplotEsc)
    {
        // We found an escape character. Move to the next character to see what we need to do next
      ++i;
      if(ucs4[i] == (PLUNICODE)plplotEsc)
      {
          // Add the actual escape character to the string
        section += wxUString((wxChar32)ucs4[i]);
      }
      else
      {
          // We have a change of state. Output the string so far
        DrawTextSection(section, xOrigin, yOrigin, x + textWidth, y + scaledOffset, transform,
            scaledFontSize, drawText, underlined, fci, red, green, blue, alpha, yScale, sectionWidth, sectionHeight, sectionDepth);
        textWidth += sectionWidth;
        textHeight = MAX(textHeight, sectionHeight + scaledOffset);
        textDepth = MAX(textDepth, sectionDepth - scaledOffset);
        section = wxEmptyString;

        // Act on the escape character
        if(ucs4[i] == (PLUNICODE)'u')
        {
            // Superscript escape

            // y, textHeight, and textDepth are all scaled
            // quantities so any offset-related variable that
            // is linearly combined with them such as
            // scaledOffset must be scaled as well.  Offset is
            // always positive so the last factor is to give
            // scaledOffset the correct sign depending on
            // level.
          plP_script_scale(TRUE, &level, &oldScale, &Scale, &oldOffset, &Offset);
          scaledFontSize = baseFontSize * Scale;
          scaledOffset = yScale * Offset * baseFontSize * (level > 0 ? 1.0 / empiricalSymmetricFactor : -1.0 * empiricalSymmetricFactor);
        }
        else if(ucs4[i] == (PLUNICODE)'d')
        {
            // Subscript escape

            // y, textHeight, and textDepth are all scaled
            // quantities so any offset-related variable that
            // is linearly combined with them such as
            // scaledOffset must be scaled as well.  Offset is
            // always positive so the last factor is to give
            // scaledOffset the correct sign depending on
            // level.
          plP_script_scale(FALSE, &level, &oldScale, &Scale, &oldOffset, &Offset);
          scaledFontSize = baseFontSize * Scale;
          scaledOffset = yScale * Offset * baseFontSize * (level > 0 ? 1.0 / empiricalSymmetricFactor : -1.0 * empiricalSymmetricFactor);
        }
        else if(ucs4[i] == (PLUNICODE)'-')          // underline
          underlined = !underlined;
        else if(ucs4[i] == (PLUNICODE)'+')          // overline
        {                                               // not implemented yet
        }
      }
    }
    else if(ucs4[i] >= PL_FCI_MARK)
    {
        // A font change
        // draw string so far
      DrawTextSection(section, xOrigin, yOrigin, x + textWidth, y + scaledOffset, transform,
          scaledFontSize, drawText, underlined, fci, red, green, blue, alpha, yScale, sectionWidth, sectionHeight, sectionDepth);
      textWidth += sectionWidth;
      textHeight = MAX(textHeight, sectionHeight + scaledOffset);
      textDepth = MAX(textDepth, sectionDepth - scaledOffset);
      section = wxEmptyString;

      // Get new fci
      fci = ucs4[i];
    }
    else
    {
        // Just a regular character - add it to the string
      section += wxUString((wxChar32)ucs4[i]);
    }

    ++i;
  }

  // We have reached the end of the string. Draw the last section.
  DrawTextSection(section, xOrigin, yOrigin, x + textWidth, y + scaledOffset, transform,
      scaledFontSize, drawText, underlined, fci, red, green, blue, alpha, yScale, sectionWidth, sectionHeight, sectionDepth);
  textWidth += sectionWidth;
  textHeight = MAX(textHeight, sectionHeight + scaledOffset);
  textDepth = MAX(textDepth, sectionDepth - scaledOffset);

  // If this was a single character remember its size as it is
  // likely to be requested repeatedly (e.g., typical plstring use).
  if(ucs4Len == 1)
  {
    m_prevSymbol = ucs4[0];
    m_prevBaseFontSize = baseFontSize;
    m_prevLevel = level;
    m_prevFci = fci;
    m_prevSymbolWidth = textWidth;
    m_prevSymbolHeight = textHeight;
    m_prevSymbolDepth = textDepth;
  }
}

//--------------------------------------------------------------------------
// Scaler class
// This class changes the logical scale of a dc on construction and resets
// it to its original value on destruction. It is ideal for making temporary
// changes to the scale and guaranteeing that the scale gets set back.
//--------------------------------------------------------------------------
class Scaler
{
public:
  Scaler(wxDC* dc, double xScale, double yScale)
  {
    m_dc = dc;
    if(m_dc)
    {
      dc->GetUserScale(&m_xScaleOld, &m_yScaleOld);
      dc->SetUserScale(xScale, yScale);
    }
  }
  ~Scaler()
  {
    if(m_dc)
      m_dc->SetUserScale(m_xScaleOld, m_yScaleOld);
  }
private:
  wxDC* m_dc;
  double m_xScaleOld;
  double m_yScaleOld;
  Scaler& operator=(const Scaler&);
  Scaler(const Scaler&);
};

//--------------------------------------------------------------------------
// OriginChanger class
// This class changes the logical origin of a dc on construction and resets
// it to its original value on destruction. It is ideal for making temporary
// changes to the origin and guaranteeing that the scale gets set back.
//--------------------------------------------------------------------------
class OriginChanger
{
public:
  OriginChanger(wxDC* dc, wxCoord xOrigin, wxCoord yOrigin)
  {
    m_dc = dc;
    if(m_dc)
    {
      dc->GetLogicalOrigin(&m_xOriginOld, &m_yOriginOld);
      dc->SetLogicalOrigin(xOrigin, yOrigin);
    }
  }
  ~OriginChanger()
  {
    if(m_dc)
      m_dc->SetLogicalOrigin(m_xOriginOld, m_yOriginOld);
  }
private:
  wxDC* m_dc;
  wxCoord m_xOriginOld;
  wxCoord m_yOriginOld;
  OriginChanger& operator=(const OriginChanger&);
  OriginChanger(const OriginChanger&);
};

//--------------------------------------------------------------------------
// DrawingObjectsChanger class
// This class changes the pen and brush of a dc on construction and resets
// them to their original values on destruction. It is ideal for making temporary
// changes to the pen and brush and guaranteeing that they get set back.
//--------------------------------------------------------------------------
class DrawingObjectsChanger
{
public:
  DrawingObjectsChanger(wxDC* dc, const wxPen& pen, const wxBrush& brush)
  {
    m_dc = dc;
    if(m_dc)
    {
      m_pen = dc->GetPen();
      m_brush = dc->GetBrush();
      dc->SetPen(pen);
      dc->SetBrush(brush);
    }
  }
  ~DrawingObjectsChanger()
  {
    if(m_dc)
    {
      m_dc->SetPen(m_pen);
      m_dc->SetBrush(m_brush);
    }
  }
private:
  wxDC* m_dc;
  wxPen   m_pen;
  wxBrush m_brush;
  DrawingObjectsChanger& operator=(const DrawingObjectsChanger&);
  DrawingObjectsChanger(const DrawingObjectsChanger&);
};

//--------------------------------------------------------------------------
//TextObjectsSaver class
//This class saves the text rendering details of a dc on construction and
//resets them to their original values on destruction. It can be used to
//ensure the restoration of state when a scope is exited
//--------------------------------------------------------------------------
class TextObjectsSaver
{
public:
  TextObjectsSaver(wxDC* dc)
  {
    m_dc = dc;
    if(m_dc)
    {
      m_font = dc->GetFont();
      m_textForeground = dc->GetTextForeground();
      m_textBackground = dc->GetTextBackground();
    }
  }
  ~TextObjectsSaver()
  {
    if(m_dc)
    {
      m_dc->SetTextForeground(m_textForeground);
      m_dc->SetTextBackground(m_textBackground);
      m_dc->SetFont(m_font);
    }
  }
private:
  wxDC* m_dc;
  wxFont   m_font;
  wxColour m_textForeground;
  wxColour m_textBackground;
  TextObjectsSaver& operator=(const TextObjectsSaver&);
  TextObjectsSaver(const TextObjectsSaver&);
};

//--------------------------------------------------------------------------
// TextObjectsChanger class
// This class changes the font and text colours of a dc on construction and resets
// them to their original values on destruction. It is ideal for making temporary
// changes to the text and guaranteeing that they get set back.
//--------------------------------------------------------------------------
class TextObjectsChanger
{
public:
  TextObjectsChanger(wxDC* dc, const wxFont& font, const wxColour& textForeground, const wxColour& textBackground)
    : m_saver(dc)
  {
    if(dc)
    {
      dc->SetTextForeground(textForeground);
      dc->SetTextBackground(textBackground);
      dc->SetFont(font);
    }
  }
  TextObjectsChanger(wxDC* dc, const wxFont& font)
    : m_saver(dc)
  {
    if(dc)
      dc->SetFont(font);
  }
  TextObjectsChanger(wxDC* dc, FontGrabber& fontGrabber, PLUNICODE fci, PLFLT size, bool underlined, const wxColour& textForeground, const wxColour& textBackground)
    : m_saver(dc)
  {
    if(dc)
    {
      wxFont font = fontGrabber.GetFont(fci, size, underlined).getWxFont();
      dc->SetTextForeground(textForeground);
      dc->SetTextBackground(textBackground);
      dc->SetFont(font);
    }
  }
private:
  TextObjectsSaver m_saver;
  TextObjectsChanger& operator=(const TextObjectsChanger&);
  TextObjectsChanger(const TextObjectsChanger&);
};

//--------------------------------------------------------------------------
// Clipper class
// This class changes the clipping region of a dc on construction and restores
// it to its previous region on destruction. It is ideal for making temporary
// changes to the clip region and guaranteeing that the scale gets set back.
//
// It turns out that clipping is mostly broken for wxGCDC - see
// http://trac.wxwidgets.org/ticket/17013. So there are a lot of things in
// this class to work around those bugs. In particular you should check
// isEveryThingClipped before drawing as I'm not sure if non-overlapping
//clip regions behave properly.
//--------------------------------------------------------------------------
class Clipper
{
public:
  Clipper(wxDC* dc, const wxRect& rect)
  {
    m_dc = dc;
    m_clipEverything = true;
    if(m_dc)
    {
      dc->GetClippingBox(m_boxOld);
      wxRect newRect = rect;
      m_clipEverything = !(newRect.Intersects(m_boxOld)
                            || (m_boxOld.width == 0 && m_boxOld.height == 0));
      if(m_clipEverything)
        dc->SetClippingRegion(wxRect(-1, -1, 1, 1));                 //not sure if this works
      else
        dc->SetClippingRegion(rect);
    }
  }
  ~Clipper()
  {
    if(m_dc)
    {
      m_dc->DestroyClippingRegion();
      m_dc->SetClippingRegion(wxRect(0, 0, 0, 0));
      m_dc->DestroyClippingRegion();
      if(m_boxOld.width != 0 && m_boxOld.height != 0)
        m_dc->SetClippingRegion(m_boxOld);
    }
  }
  bool isEverythingClipped()
  {
    return m_clipEverything;
  }
private:
  wxDC* m_dc;
  wxRect m_boxOld;
  bool   m_clipEverything;

  ES_NON_COPYABLE(Clipper);
};

//--------------------------------------------------------------------------
// class Rand
// This is a simple random number generator class, created solely so that
// random numbers can be generated in this file without "contaminating" the
// global series of random numbers with a new seed.
// It uses an algorithm that apparently used to be used in gcc rand()
// provided under GNU LGPL v2.1.
//--------------------------------------------------------------------------
class Rand
{
public:
  Rand()
  {
    std::srand(m_seed);
  }

  Rand(unsigned int seed)
  {
    m_seed = seed;
  }

  unsigned int operator()()
  {
    unsigned int next = m_seed;
    int          result;

    next *= 1103515245;
    next += 12345;
    result = (unsigned int)(next / max) % 2048;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int)(next / max) % 1024;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int)(next / max) % 1024;

    m_seed = next;

    return result;
  }
  static const unsigned int max = 65536;
private:
  unsigned int m_seed;
};

void plFontToWxFontParameters(PLUNICODE fci, PLFLT scaledFontSize, wxFontFamily& family, int& style, int& weight, int& pt)
{
  unsigned char plFontFamily, plFontStyle, plFontWeight;

  plP_fci2hex(fci, &plFontFamily, PL_FCI_FAMILY);
  plP_fci2hex(fci, &plFontStyle, PL_FCI_STYLE);
  plP_fci2hex(fci, &plFontWeight, PL_FCI_WEIGHT);

  family = fontFamilyLookup[plFontFamily];
  style = fontStyleLookup[plFontStyle];
  weight = fontWeightLookup[plFontWeight];
  pt = ROUND(scaledFontSize);
}

Font::Font()
{
  m_fci = 0;
  m_size = std::numeric_limits<PLFLT>::quiet_NaN();
  m_underlined = false;
  m_hasFont = false;
}

Font::Font(PLUNICODE fci, PLFLT size, bool underlined, bool createFontOnConstruction)
{
  m_fci = fci;
  m_size = size;
  m_underlined = underlined;
  m_hasFont = false;
  if(createFontOnConstruction)
    createFont();
}

void Font::createFont()
{
  wxFontFamily family;
  int          style;
  int          weight;
  int          pt;
  plFontToWxFontParameters(m_fci, m_size, family, style, weight, pt);

  m_font = wxFont(pt, family, style, weight, m_underlined, wxEmptyString, wxFONTENCODING_DEFAULT);
  //wxWidgets has a feature where wxDEFAULT can be passed in as the size in the constructor
  //which gives the default size for the system. Annoyingly wxDEFAULT is 70 which can get used
  //as an actual size. The workaround as per http://trac.wxwidgets.org/ticket/12315 is to call
  //wxFont::SetPointSize after construction.
  if(pt == wxDEFAULT)
    m_font.SetPointSize(pt);
  m_hasFont = true;
}

wxFont Font::getWxFont()
{
  if(!m_hasFont)
    createFont();
  return m_font;
}

bool operator ==(const Font& lhs, const Font& rhs)
{
  return lhs.getFci() == rhs.getFci()
    && lhs.getSize() == rhs.getSize()
    && lhs.getUnderlined() == rhs.getUnderlined();
}

//--------------------------------------------------------------------------
//  FontGrabber::FontGrabber( )
//
//  Default constructor
//--------------------------------------------------------------------------
FontGrabber::FontGrabber()
{
  m_lastWasCached = false;
}

//--------------------------------------------------------------------------
//  Font FontGrabber::GetFont( PLUNICODE fci )
//
//  Get the requested font either fresh or from the cache.
//--------------------------------------------------------------------------
Font FontGrabber::GetFont(PLUNICODE fci, PLFLT scaledFontSize, bool underlined)
{
  Font newFont(fci, scaledFontSize, underlined);
  if(m_prevFont == newFont)
  {
    m_lastWasCached = true;
    return m_prevFont;
  }

  m_lastWasCached = false;

  return m_prevFont = newFont;
}

//--------------------------------------------------------------------------
//  wxPLDevice::wxPLDevice( void )
//
//  Constructor of the standard wxWidgets device based on the wxPLDevBase
//  class. Only some initialisations are done.
//--------------------------------------------------------------------------
wxPLDevice::wxPLDevice(PLStream* pls, char* mfo, PLINT text, PLINT hrshsym)
  : m_plplotEdgeLength(PLFLT(SHRT_MAX)), m_interactiveTextImage(1, 1)
{
  PLPLOT_wxLogDebug("wxPLDevice(): enter");
  m_fixedAspect = false;

  m_lineSpacing = 1.0;

  m_dc = NULL;

  wxGraphicsContext* gc = wxGraphicsContext::Create(m_interactiveTextImage);
  PLPLOT_wxLogDebug("wxPLDevice(): gc done");
  try
  {
    m_interactiveTextGcdc = new wxGCDC(gc);
  }
  catch(...)
  {
    delete gc;
    throw("wxPLDevice::wxPLDevice: unknown failure in new wxGCDC( gc )");
  }
  PLPLOT_wxLogDebug("wxPLDevice(): m_interactiveTextGcdc done");

  if(mfo)
    strcpy(m_mfo, mfo);
  else
  //assume we will be outputting to the default
  //memory map until we are given a dc to draw to
#ifdef WXPLVIEWER_DEBUG
    strcpy(m_mfo, "plplotMemoryMap");
#else
    strcpy(m_mfo, "plplotMemoryMap??????????");
#endif

    // be verbose and write out debug messages
#ifdef _DEBUG
  pls->verbose = 1;
  pls->debug = 1;
#endif

  pls->color = 1;                               // Is a color device
  pls->dev_flush = 1;                               // Handles flushes
  pls->dev_fill0 = 1;                               // Can handle solid fills
  pls->dev_fill1 = 0;                               // Can't handle pattern fills
  pls->dev_dash = 0;
  pls->dev_clear = 1;                               // driver supports clear
  pls->plbuf_write = 1;                               // use the plot buffer!
  pls->termin = (strlen(m_mfo)) > 0 ? 0 : 1; // interactive device unless we are writing to memory - pretty sure this is an unused option though
  pls->graphx = GRAPHICS_MODE;                   //  This indicates this is a graphics driver. PLplot will therefore call pltext() before outputting text, however we currently have not implemented catching that text.

  if(text)
  {
    pls->dev_text = 1; // want to draw text
    pls->dev_unicode = 1; // want unicode
    if(hrshsym)
      pls->dev_hrshsym = 1;
  }


  // Set up physical limits of plotting device in plplot internal units
  // we just tell plplot we are the maximum plint in both dimensions
  //which gives us the best resolution
  plP_setphy((PLINT)0, (PLINT)SHRT_MAX,
      (PLINT)0, (PLINT)SHRT_MAX);

  // set dpi and page size defaults if the user has not already set
  // these with -dpi or -geometry command line options or with
  // plspage.

  if(pls->xdpi <= 0. || pls->ydpi <= 0.)
  {
      // Use recommended default pixels per inch.
    plspage(PLPLOT_DEFAULT_PIXELS_PER_INCH, PLPLOT_DEFAULT_PIXELS_PER_INCH, 0, 0, 0, 0);
  }

  if(pls->xlength == 0 || pls->ylength == 0)
  {
      // Use recommended default pixel width and height.
    plspage(0., 0., PLPLOT_DEFAULT_WIDTH_PIXELS, PLPLOT_DEFAULT_HEIGHT_PIXELS, 0, 0);
  }
  m_localBufferPosition = 0;

  SetSize(pls, plsc->xlength, plsc->ylength);

  ES_ASSERT(pls->dev_data);
  SetDC(pls, (wxDC*)pls->dev_data);
  PLPLOT_wxLogDebug("wxPLDevice(): SetDC done");

  PLPLOT_wxLogDebug("wxPLDevice(): leave");
  //this must be the absolute last thing that is done
  //so that if an exception is thrown pls->dev remains as NULL
  pls->dev = (void*)this;
}

//--------------------------------------------------------------------------
//  wxPLDevice::~wxPLDevice( void )
//
//  The destructor frees memory allocated by the device.
//--------------------------------------------------------------------------
wxPLDevice::~wxPLDevice()
{
  wxDELETE(m_interactiveTextGcdc);
}

//--------------------------------------------------------------------------
//  void wxPLDevice::PreDestructorTidy( PLStream *pls )
//
//  This function performs any tidying up that requires a PLStream. should
//  be called before the destructor obviously
//--------------------------------------------------------------------------
void wxPLDevice::PreDestructorTidy(PLStream* pls)
{
  if(!m_dc && pls->nopause)
    TransmitBuffer(pls, transmissionClose);
}

//--------------------------------------------------------------------------
//  void wxPLDevice::DrawLine( short x1a, short y1a, short x2a, short y2a )
//
//  Draw a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void wxPLDevice::DrawLine(short x1a, short y1a, short x2a, short y2a)
{
  if(!m_dc)
    return;

  Clipper clipper(m_dc, GetClipRegion().GetBox());
  Scaler  scaler(m_dc, 1.0 / m_scale, 1.0 / m_scale);
  DrawingObjectsChanger drawingObjectsChanger(m_dc, m_pen, m_brush);
  m_dc->DrawLine((wxCoord)(m_xAspect * x1a), (wxCoord)(m_yAspect * (m_plplotEdgeLength - y1a)),
      (wxCoord)(m_xAspect * x2a), (wxCoord)(m_yAspect * (m_plplotEdgeLength - y2a)));
}


//--------------------------------------------------------------------------
//  void wxPLDevice::DrawPolyline( short *xa, short *ya, PLINT npts )
//
//  Draw a poly line - coordinates are in the xa and ya arrays.
//--------------------------------------------------------------------------
void wxPLDevice::DrawPolyline(short* xa, short* ya, PLINT npts)
{
  if(!m_dc)
    return;
  Clipper clipper(m_dc, GetClipRegion().GetBox());
  Scaler  scaler(m_dc, 1.0 / m_scale, 1.0 / m_scale);
  DrawingObjectsChanger drawingObjectsChanger(m_dc, m_pen, m_brush);
  for(PLINT i = 1; i < npts; i++)
    m_dc->DrawLine(m_xAspect * xa[i - 1], m_yAspect * (m_plplotEdgeLength - ya[i - 1]),
        m_xAspect * xa[i], m_yAspect * (m_plplotEdgeLength - ya[i]));
}


//--------------------------------------------------------------------------
//  void wxPLDevice::ClearBackground( PLStream* pls, PLINT bgr, PLINT bgg, PLINT bgb,
//                                   PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
//
//  Clear parts ((x1,y1) to (x2,y2)) of the background in color (bgr,bgg,bgb).
//--------------------------------------------------------------------------
void wxPLDevice::ClearBackground(PLStream* pls, PLINT x1, PLINT y1, PLINT x2, PLINT y2)
{
  if(!m_dc)
    return;

  x1 = x1 < 0 ? 0 : x1;
  x2 = x2 < 0 ? m_plplotEdgeLength : x2;
  y1 = y1 < 0 ? 0 : y1;
  y2 = y2 < 0 ? m_plplotEdgeLength : y2;

  PLINT x = MIN(x1, x2) * m_xAspect;
  PLINT y = (m_plplotEdgeLength - MAX(y1, y2)) * m_yAspect;
  PLINT width = abs(x1 - x2) * m_xAspect;
  PLINT height = abs(y1 - y2) * m_yAspect;

  if(width > 0 && height > 0)
  {
    PLINT    r, g, b;
    PLFLT    a;
    plgcolbga(&r, &g, &b, &a);
    wxColour bgColour(r, g, b, a * 255);
    DrawingObjectsChanger changer(m_dc, wxPen(bgColour, 0), wxBrush(bgColour));
    Scaler   scaler(m_dc, 1.0 / m_scale, 1.0 / m_scale);
    m_dc->DrawRectangle(x, y, width, height);
  }
}


//--------------------------------------------------------------------------
//  void wxPLDevice::FillPolygon( PLStream *pls )
//
//  Draw a filled polygon.
//--------------------------------------------------------------------------
void wxPLDevice::FillPolygon(PLStream* pls)
{
  if(!m_dc)
    return;

//edge the polygon with a 0.5 pixel line to avoid seams. This is a
//bit of a bodge really but this is a difficult problem
  wxPen edgePen(m_brush.GetColour(), m_scale, wxSOLID);
  DrawingObjectsChanger changer(m_dc, edgePen, m_brush);
  //DrawingObjectsChanger changer(m_dc, wxNullPen, m_brush );
  Clipper clipper(m_dc, GetClipRegion().GetBox());
  Scaler  scaler(m_dc, 1.0 / m_scale, 1.0 / m_scale);
  wxPoint* points = new wxPoint[pls->dev_npts];
  wxCoord xoffset = 0;
  wxCoord yoffset = 0;

  for(int i = 0; i < pls->dev_npts; i++)
  {
    points[i].x = (int)(m_xAspect * pls->dev_x[i]);
    points[i].y = (int)(m_yAspect * (m_plplotEdgeLength - pls->dev_y[i]));
  }

  if(pls->dev_eofill)
  {
    m_dc->DrawPolygon(pls->dev_npts, points, xoffset, yoffset, wxODDEVEN_RULE);
  }
  else
  {
    m_dc->DrawPolygon(pls->dev_npts, points, xoffset, yoffset, wxWINDING_RULE);
  }
  delete[] points;
}


//--------------------------------------------------------------------------
//  void wxPLDevice::SetWidth( PLStream *pls )
//
//  Set the width of the drawing pen.
//--------------------------------------------------------------------------
void wxPLDevice::SetWidth(PLStream* pls)
{
  PLFLT width = (pls->width > 0.0 ? pls->width : 1.0) * m_scale;
  m_pen = wxPen(wxColour(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b,
    pls->curcolor.a * 255), width, wxSOLID);
}


//--------------------------------------------------------------------------
//  void wxPLDevice::SetColor( PLStream *pls )
//
//  Set color from PLStream.
//--------------------------------------------------------------------------
void wxPLDevice::SetColor(PLStream* pls)
{
  PLFLT width = (pls->width > 0.0 ? pls->width : 1.0) * m_scale;
  m_pen = wxPen(wxColour(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b,
    pls->curcolor.a * 255), width, wxSOLID);
  m_brush = wxBrush(wxColour(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b,
    pls->curcolor.a * 255));
}

//--------------------------------------------------------------------------
//  void wxPLDevice::SetXorMode( bool on )
//
//  Set whether we want XOR mode on or off.
//--------------------------------------------------------------------------
void wxPLDevice::SetXorMode(bool on)
{
  if(m_dc)
    m_dc->SetLogicalFunction(on ? wxXOR : wxCOPY);
}

//--------------------------------------------------------------------------
//  void wxPLDevice::SetDC( PLStream *pls, void* dc )
//
//  Adds a dc to the device. In that case, the drivers doesn't provide
//  a GUI.
//--------------------------------------------------------------------------
void wxPLDevice::SetDC(PLStream* pls, wxDC* dc)
{
  m_dc = dc;                 // Add the dc to the device
  m_useDcTextTransform = false;
  m_gc = NULL;
  if(m_dc)
  {
#if wxVERSION_NUMBER >= 2902
    m_useDcTextTransform = m_dc->CanUseTransformMatrix();
#endif
        //Prior to some point in wxWidgets 3.1 development wxGCDC didn't
        //support transformation matrices, but the underlying
        //wxGraphicsContext had its own transformation matrix ability.
        //So check if we are using a wxGCDC  using RTTI and if so we can
        //use this.
    wxGCDC* gcdc = NULL;
    try
    {
        //put this in a try block as I'm not sure if it will throw if
        //RTTI is switched off
      gcdc = dynamic_cast<wxGCDC*>(m_dc);
    }
    catch(...)
    {
      throw("unknown failure in dynamic_cast< wxGCDC* >( m_dc )");
    }
    if(gcdc)
      m_gc = gcdc->GetGraphicsContext();

    strcpy(m_mfo, "");
    SetSize(pls, m_width, m_height);  //call with our current size to set the scaling
    pls->has_string_length = 1;         // Driver supports string length calculations, if we have a dc to draw on
  }
  else
  {
    pls->has_string_length = 0;         //if we have no device to draw on we cannot check string size
  }
}

//  This function will draw a section of text given by section at location
//  x, y relative to the origin xOrigin, yOrigin. The text must not
//  contain any newline characters or PLplot escapes.
//  transform is a transformation to be applied to the device context AFTER
//  the origin of the device context has been moved to xOrigin, yOrigin. The
//  purpose of having this origin is to allow the units used by wxWidgets to
//  be different to those passed in and for the scaling factors to be
//  different in each direction.
//  scaledFontSize is the font size in pt after any scaling for super/sub script.
//  The underlined flag, overlined flag, fci and text colours are inputs defining
//  the text style.
//  On return yScale, textWidth, textHeigth and textDepth will be
//  filled with the scaling value used for the Y dimension and the
//  (positive) width, (positive) ascender height and (positive)
//  descender depth of the text string.
//  If drawText is true the text will actually be rendered. If it is false the size
//  will be calculated but the text will not be rendered.
//--------------------------------------------------------------------------
void wxPLDevice::DrawTextSection(wxString section, wxCoord xOrigin, wxCoord yOrigin, wxCoord x, wxCoord y, PLFLT* transform, PLFLT scaledFontSize, bool drawText, bool underlined, PLUNICODE fci, unsigned char red, unsigned char green, unsigned char blue, PLFLT alpha, PLFLT& yScale, wxCoord& sectionWidth, wxCoord& sectionHeight, wxCoord& sectionDepth)
{
    //for text, work in native coordinates, because it is significantly easier when
    //dealing with superscripts and text chunks.
    //The scaler object sets the scale to the new value until it is destroyed
    //when this function exits.
  Scaler           scaler(m_dc, 1.0, 1.0);
  //Also move the origin back to the top left, rather than the bottom
  OriginChanger    originChanger(m_dc, 0, wxCoord(m_height - m_plplotEdgeLength / m_yScale));
  //save the text state for automatic restoration on scope exit
  TextObjectsSaver textObjectsSaver(m_dc);

  wxCoord          leading;
  Font             font = m_fontGrabber.GetFont(fci, scaledFontSize, underlined);

  // Adjusted so that utf8 "heavy multiplication x", "number sign",
  // "four teardrop-spoked asterisk", and "8-spoked asterisk" are
  // aligned properly when running
  //
  // python (or python3 as appropriate) examples/python/test_circle.py -dev wxwidgets
  //
  PLFLT   empiricalYOffset = -0.020 * scaledFontSize * m_yScale;
  wxCoord empiricalY = y + empiricalYOffset;
  // Return via the yScale argument the value used for scaling in the Y direction.
  yScale = m_yScale;
  if(m_dc)
  {
    wxFont theFont = font.getWxFont();

    // gdb sessions typically show something like the following:
    // 4: sectionWidth = (wxCoord &) @0x7fffffffd6ec: 7
    // 3: sectionHeight = (wxCoord &) @0x7fffffffd6e8: 13
    // 2: sectionDepth = (wxCoord &) @0x7fffffffd6e4: 3
    // 1: leading = 0
    // That is, sectionWidth, sectionHeight, and sectionDepth
    // returned by GetTextExtent are all normally small positive integers while leading
    // returned by the same call is typically zero.
    m_dc->GetTextExtent(section, &sectionWidth, &sectionHeight,
        &sectionDepth, &leading, &theFont);
  }
  else
  {
    wxFont theFont = font.getWxFont();
    // See comments above concerning interpretation of GetTextExtent results.
    m_interactiveTextGcdc->GetTextExtent(section, &sectionWidth, &sectionHeight,
        &sectionDepth, &leading, &theFont);
  }

  // The leading value that is returned is a vertical offset used by
  // some font designers, but I (AWI) can find no documentation
  // concerning the sign convention for leading so I simply follow
  // here the convention previously used by this code.
  // Note, that all fonts I have tried have zero leading so (a) this
  // adopted sign convention does not matter for them, and (b) if it
  // is wrong here, there is no chance to debug it by looking at the
  // vertical offset of rendered strings.
  sectionHeight -= leading;
  sectionDepth += leading;
  sectionWidth *= m_xScale;
  sectionHeight *= m_yScale;
  sectionDepth *= m_yScale;

  // Draw the text if requested
  if(drawText && m_dc)
  {
      //set the text colour
    m_dc->SetTextBackground(wxColour(red, green, blue, alpha * 255));
    m_dc->SetTextForeground(wxColour(red, green, blue, alpha * 255));

    // Set the clipping limits
    PLINT   rcx[4], rcy[4];
    difilt_clip(rcx, rcy);
    wxPoint cpoints[4];
    for(int i = 0; i < 4; i++)
    {
      cpoints[i].x = rcx[i] / m_xScale;
      cpoints[i].y = m_height - rcy[i] / m_yScale;
    }
    Clipper clipper(m_dc, wxRegion(4, cpoints).GetBox());

    Font    font = m_fontGrabber.GetFont(fci, scaledFontSize, underlined);
    m_dc->SetFont(font.getWxFont());

    if(m_useDcTextTransform)
    {
#if wxVERSION_NUMBER >= 2902
      wxAffineMatrix2D originalDcMatrix = m_dc->GetTransformMatrix();

      wxAffineMatrix2D newMatrix = originalDcMatrix;
      newMatrix.Translate(xOrigin / m_xScale, m_height - yOrigin / m_yScale);
      wxAffineMatrix2D textMatrix;
      // For some reason we don't do the mirroring like in the
      // wxGraphicsContext when we use a wxDC.
      PLFLT xTransform = transform[4] / m_xScale;
      PLFLT yTransform = transform[5] / m_yScale;
      textMatrix.Set(
          wxMatrix2D(
            transform[0], transform[2],
            transform[1], transform[3]),
          wxPoint2DDouble(
            xTransform, yTransform));
      newMatrix.Concat(textMatrix);
      m_dc->SetTransformMatrix(newMatrix);

      m_dc->DrawText(section, x / m_xScale, -empiricalY / m_yScale);

      m_dc->SetTransformMatrix(originalDcMatrix);
#endif
    }
    else if(m_gc)
    {
      wxGraphicsMatrix originalGcMatrix = m_gc->GetTransform();

      m_gc->Translate(xOrigin / m_xScale, m_height - yOrigin / m_yScale);              //move to text starting position
      //Create a wxGraphicsMatrix from our plplot transformation matrix.
      //Note the different conventions
      //1) plplot transforms use notation x' = Mx, where x and x' are column vectors,
      //   wxGraphicsContext uses xM = x' where x and x' are row vectors. This means
      //   we must transpose the matrix.
      //2) plplot Affine matrices a represented by 6 values which start at the top left
      //   and work down each column. The 3rd row is implied as 0 0 1. wxWidget matrices
      //   are represented by 6 values which start at the top left and work along each
      //   row. The 3rd column is implied as 0 0 1. This means we must transpose the
      //   matrix.
      //3) Items 1 and 2 cancel out so we don't actually need to do anything about them.
      //4) The wxGraphicsContext has positive y in the downward direction, but plplot
      //   has positive y in the upwards direction. This means we must do a reflection
      //   in the y direction before and after applying the transform. Also we must scale
      //   the translation parts to match the pixel scale.
      //The overall transform is
      //
      //   |1  0 0| |transform[0] transform[2] transform[4]/m_xScale| |1  0 0|
      //   |0 -1 0| |transform[1] transform[3] transform[5]/m_yScale| |0 -1 0|
      //   |0  0 1| |     0            0                1            | |0  0 1|
      //
      //which results in
      //
      //   | transform[0]          -transform[2]          0|
      //   |-transform[1]           transform[3]          0|
      //   | transform[4]/m_xScale -transform[5]/m_yScale 1|
      //
      PLFLT            xTransform = transform[4] / m_xScale;
      PLFLT            yTransform = transform[5] / m_yScale;
      wxGraphicsMatrix matrix = m_gc->CreateMatrix(
          transform[0], -transform[1],
          -transform[2], transform[3],
          xTransform, -yTransform);
      wxGraphicsMatrix reflectMatrix = m_gc->CreateMatrix();
      m_gc->ConcatTransform(matrix);
      m_gc->DrawText(section, x / m_xScale, -empiricalY / m_yScale);
      m_gc->SetTransform(originalGcMatrix);
    }
    else
    {
        // If we are stuck with a wxDC that has no transformation
        // abilities then all we can really do is rotate the text
        // - this is a bit of a poor state really, but to be
        // honest it is better than defaulting to Hershey for all
        // text
      PLFLT xTransformed = x / m_xScale * transform[0] + empiricalY / m_yScale * transform[2] + transform[4] / m_xScale;
      PLFLT yTransformed = x / m_xScale * transform[1] + empiricalY / m_yScale * transform[3] + transform[4] / m_xScale;
      // This angle calculation comes from transforming the
      // point (0,0) and any other point on the empiricalY = 0 line and
      // getting the angle from the horizontal of that line.
      PLFLT angle = atan2(transform[1], transform[0]) * 180.0 / M_PI;
      m_dc->DrawRotatedText(section, xOrigin / m_xScale + xTransformed, m_height - yOrigin / m_yScale - yTransformed, angle);
    }
  }
}

//--------------------------------------------------------------------------
//  void wxPLDevice::EndPage( PLStream* pls )
//  End the page. This is the point that we write the buffer to the memory
//  mapped file if needed
//--------------------------------------------------------------------------
void wxPLDevice::EndPage(PLStream* pls)
{
  if(!m_dc)
  {
    if(pls->nopause)
      TransmitBuffer(pls, transmissionEndOfPageNoPause);
    else
      TransmitBuffer(pls, transmissionEndOfPage);
    return;
  }
}

//--------------------------------------------------------------------------
//  void wxPLDevice::BeginPage( PLStream* pls )
//  Sets up for transfer in case it is needed and sets the current state
//--------------------------------------------------------------------------
void wxPLDevice::BeginPage(PLStream* pls)
{
  if(!m_dc)
  {
    m_localBufferPosition = 0;
    TransmitBuffer(NULL, transmissionBeginPage);
  }

  // Get the starting colour, width and font from the stream
  SetWidth(pls);
  SetColor(pls);

  //clear the page
  ClearBackground(pls);
}

//--------------------------------------------------------------------------
//  void wxPLDevice::SetSize( PLStream* pls )
//  Set the size of the page, scale parameters and the dpi
//--------------------------------------------------------------------------
void wxPLDevice::SetSize(PLStream* pls, int width, int height)
{
    //we call BeginPage, before we fiddle with fixed aspect so that the
    //whole background gets filled
    // get boundary coordinates in plplot units
  PLINT xmin;
  PLINT xmax;
  PLINT ymin;
  PLINT ymax;
  plP_gphy(&xmin, &xmax, &ymin, &ymax);
  //split the scaling into an overall scale, the same in both dimensions
  //and an aspect part which differs in both directions.
  //We will apply the aspect ratio part, and let the DC do the overall
  //scaling. This gives us subpixel accuracy, but ensures line thickness
  //remains consistent in both directions
  m_xScale = width > 0 ? (PLFLT)(xmax - xmin) / (PLFLT)width : 0.0;
  m_yScale = height > 0 ? (PLFLT)(ymax - ymin) / (PLFLT)height : 0.0;
  m_scale = MIN(m_xScale, m_yScale);

  if(!m_fixedAspect)
  {
    m_xAspect = m_scale / m_xScale;
    m_yAspect = m_scale / m_yScale;
  }
  else
  {
      //now sort out the fixed aspect and reset the logical scale if needed
    if(PLFLT(height) / PLFLT(width) > m_yAspect / m_xAspect)
    {
      m_scale = m_xScale * m_xAspect;
      m_yScale = m_scale / m_yAspect;
    }
    else
    {
      m_scale = m_yScale * m_yAspect;
      m_xScale = m_scale / m_xAspect;
    }
  }

  if(m_dc)
  {
    const wxSize& ppi = m_dc->GetPPI();
    
    pls->xdpi = static_cast<PLFLT>(ppi.x);// / m_xScale;
    pls->ydpi = static_cast<PLFLT>(ppi.y);// / m_yScale;
  }

  m_width = (xmax - xmin) / m_xScale;
  pls->xlength = PLINT(m_width + 0.5);
  m_height = (ymax - ymin) / m_yScale;
  pls->ylength = PLINT(m_height + 0.5);

  // Set the number of plplot pixels per mm
  PLFLT mmw = m_width * PLPLOT_MM_PER_INCH / pls->xdpi;
  PLFLT mmh = m_height * PLPLOT_MM_PER_INCH / pls->ydpi;
  PLFLT pxmmx = m_plplotEdgeLength / mmw;
  PLFLT pxmmy = m_plplotEdgeLength / mmh;
  plP_setpxl(
    pxmmx, 
    pxmmy
  );
  //
  //The line above is technically correct, however, 3d text only looks at device dimensions (32767x32767 square)
  //but 2d rotated text uses the mm size derived above. The only way to consistently deal with them is
  //by having an equal device units per mm in both directions and do a correction in DrawText().
  //Usefully this also allows us to change text rotation as aspect ratios change
  //PLFLT size = m_xAspect > m_yAspect ? m_width : m_height;
  //plP_setpxl( m_plplotEdgeLength / size * pls->xdpi / PLPLOT_MM_PER_INCH, m_plplotEdgeLength / size * pls->ydpi / PLPLOT_MM_PER_INCH );


  // redraw the plot
  if(m_dc && pls->plbuf_buffer)
    plreplot();
}


void wxPLDevice::FixAspectRatio(bool fix)
{
  m_fixedAspect = fix;
}

void wxPLDevice::Flush(PLStream* pls)
{}

// This function transmits data to the gui program via a memory map.
// This function can be called with pls set to NULL for transmission
// of just a flag for e.g. page end or begin.
void wxPLDevice::TransmitBuffer(PLStream* pls, unsigned char transmissionType)
{}

void wxPLDevice::SetupMemoryMap()
{}

void wxPLDevice::Locate(PLStream* pls, PLGraphicsIn* graphicsIn)
{}

//--------------------------------------------------------------------------
// wxRegion wxPLDevice::GetClipRegion()
// Gets the current clip region from plplot as a wxRegion
//--------------------------------------------------------------------------

wxRegion wxPLDevice::GetClipRegion()
{
  PLINT rcx[4], rcy[4];
  difilt_clip(rcx, rcy);

  wxPoint cpoints[4];
  for(int i = 0; i < 4; i++)
  {
    cpoints[i].x = rcx[i] / m_xScale;
    cpoints[i].y = m_height - rcy[i] / m_yScale;
  }
  return wxRegion(4, cpoints);
}
//--------------------------------------------------------------------------------
