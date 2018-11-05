#include "escore_gui_pch.h"
#pragma hdrstop

#include <wx/wupdlock.h>
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>

#include "EsCoreGuiUtilities.h"
//--------------------------------------------------------------------------------

const wxString& EsUtilities::wxSpaceString()
{
  static const wxString sc_space(" ");
  return sc_space;
}
//--------------------------------------------------------------------------------

void EsUtilities::wxArrayStringToEsStringArray(EsString::Array& dest, const wxArrayString& src)
{
  dest.clear();
  dest.reserve(src.size());

  for( auto const& item : src )
    dest.push_back(
      item.t_str()
    );
}
//--------------------------------------------------------------------------------

void EsUtilities::esStringArrayToWxArrayString(wxArrayString& dest, const EsString::Array& src)
{
  dest.clear();
  dest.reserve(src.size());
  for( auto const& item : src )
    dest.push_back(
      item.c_str()
    );
}
//--------------------------------------------------------------------------------

void EsUtilities::mruToEsStringArray(EsString::Array& dest, const wxFileHistory& src)
{
  dest.clear();
  dest.reserve(src.GetCount());

  for( size_t idx = 0; idx < src.GetCount(); ++idx )
    dest.push_back(
      src.GetHistoryFile(idx).t_str()
    );
}
//--------------------------------------------------------------------------------

void EsUtilities::esStringArrayToMru(wxFileHistory& dest, const EsString::Array& src)
{
  while(dest.GetCount())
    dest.RemoveFileFromHistory(0);
  
  for( size_t idx = 0; idx < src.size(); ++idx )
    dest.AddFileToHistory(
      src[idx].c_str()
    );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosSave(const EsStreamIntf::Ptr& out, const wxPoint& pos)
{
  ES_ASSERT(out);

  out->valueWrite(
    esT("LEFT"),
    pos.x
  );

  out->valueWrite(
    esT("TOP"),
    pos.y
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosSave(const EsStreamIntf::Ptr& out, const wxWindow* wnd)
{
  ES_ASSERT(out);
  ES_ASSERT(wnd);

  const wxRect& rect = wnd->GetRect();

  windowPosSave(
    out,
    rect.GetTopLeft()
  );

  out->valueWrite(
    esT("WIDTH"),
    rect.GetWidth()
  );

  out->valueWrite(
    esT("HEIGHT"),
    rect.GetHeight()
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosLoad(const EsStreamIntf::Ptr& in, wxPoint& pos, const wxPoint& defaultPos)
{
  ES_ASSERT(in);

  // form pos & size
  if(in->valueExists(esT("LEFT")))
  {
    pos.x = in->valueRead(
      esT("LEFT"),
      defaultPos.x
    ).asInt();

    if(in->valueExists(esT("TOP")))
    {
      pos.y = in->valueRead(
        esT("TOP"),
        defaultPos.y
      ).asInt();
    }
  }
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosLoad(const EsStreamIntf::Ptr& in, wxWindow* wnd, const wxSize& minSize)
{
  ES_ASSERT(in);
  ES_ASSERT(wnd);

  // form pos & size
  wxPoint pos = wnd->GetPosition();
  windowPosLoad(
    in, 
    pos, 
    wxPoint(0, 0)
  );

  wxSize sze = wnd->GetSize();
  if(in->valueExists(esT("WIDTH")))
  {
    sze.x = in->valueRead(
      esT("WIDTH"),
      minSize.x
    ).asInt();

    if(in->valueExists(esT("HEIGHT")))
    {
      sze.y = in->valueRead(
        esT("HEIGHT"),
        minSize.y
      ).asInt();
    }
  }

  pos.x = wxMax(0, pos.x);
  pos.y = wxMax(0, pos.y);
  sze.x = wxMax(minSize.x, sze.x);
  sze.y = wxMax(minSize.y, sze.y);

  wnd->SetPosition(pos);
  wnd->SetSize(sze);
}
//--------------------------------------------------------------------------------

void EsUtilities::windowStateSave(const EsStreamIntf::Ptr& out, const wxFrame* wnd)
{
  ES_ASSERT(out);
  ES_ASSERT(wnd);

  EsString state = esT("normal");
  if( wnd->IsMaximized() )
    state = esT("maximized");
  else if( wnd->IsIconized() )
    state = esT("iconized");
  else if( wnd->IsFullScreen() )
    state = esT("fullScreen");

  out->valueWrite(
    esT("STATE"),
    state
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowStateLoad(const EsStreamIntf::Ptr& in, wxFrame* wnd, long fullScreenStyle /*= wxFULLSCREEN_ALL*/)
{
  ES_ASSERT(in);
  ES_ASSERT(wnd);

  const EsString& state = in->valueRead(
      esT("STATE"),
      esVT("normal")
    ).asString();

  if( esT("maximized") == state )
    wnd->Maximize();
  else if( esT("iconized") == state )
    wnd->Iconize();
  else if( esT("fullScreen") == state )
    wnd->ShowFullScreen(
      true,
      fullScreenStyle
    );
  else
    wnd->Maximize(false);
}
//--------------------------------------------------------------------------------

// Standard menu bitmaps
//
#include "res/menu-check-checked.xpm"
#include "res/menu-check-unchecked.xpm"
#include "res/menu-radio-checked.xpm"
#include "res/menu-radio-unchecked.xpm"

const wxBitmap& EsUtilities::stdBitmapGet(EsStdBitmapId id)
{
  static std::map<EsStdBitmapId, wxBitmap> s_stock;

  auto cit = s_stock.find(id);
  if(s_stock.end() != cit)
    return (*cit).second;

  if(EsStdBitmapId::Null != id)
  {
    switch(id)
    {
    case EsStdBitmapId::CheckChecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::CheckChecked,
          wxBitmap(menu_check_checked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    case EsStdBitmapId::CheckUnchecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::CheckUnchecked,
          wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    case EsStdBitmapId::RadioChecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::RadioChecked,
          wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    case EsStdBitmapId::RadioUnchecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::RadioUnchecked,
          wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    }
  }

  return wxNullBitmap;
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, wxItemKind kind, const wxBitmap& normalChecked,
  const wxBitmap& unchecked, wxMenu* submenu /*= 0*/)
{
  wxMenuItem* result = new wxMenuItem(parent, id, label, help, kind, submenu);
  ES_ASSERT(result);

  result->SetBitmaps(normalChecked, unchecked);
  return result;
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuNormalItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, const wxBitmap& normal/* = wxNullBitmap*/, wxMenu* submenu /*= 0*/)
{
  return EsUtilities::menuItemCreate(parent, id, label, help, wxITEM_NORMAL, normal, normal, submenu);
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuCheckItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, const wxBitmap& checked/* = wxNullBitmap*/, const wxBitmap& unchecked/* = wxNullBitmap*/)
{
  return EsUtilities::menuItemCreate(
    parent, 
    id, 
    label, 
    help, 
    wxITEM_CHECK,
    checked.IsNull() ? 
      stdBitmapGet(EsStdBitmapId::CheckChecked) : 
      checked,
    unchecked.IsNull() ? 
      stdBitmapGet(EsStdBitmapId::CheckUnchecked) : 
      unchecked, 
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuRadioItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, const wxBitmap& checked/* = wxNullBitmap*/, const wxBitmap& unchecked/* = wxNullBitmap*/)
{
  return EsUtilities::menuItemCreate(
    parent, 
    id, 
    label, 
    help, 
    wxITEM_CHECK/*wxITEM_RADIO*/,
    checked.IsNull() ? 
      stdBitmapGet(EsStdBitmapId::RadioChecked) : 
      checked,
    unchecked.IsNull() ? 
      stdBitmapGet(EsStdBitmapId::RadioUnchecked) : 
      unchecked, 
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxMenu* EsUtilities::menuLanguageSelectionCreate(const EsString::Array& langs, int startId)
{
  wxMenu* result = new wxMenu;
  ES_ASSERT(result);

  // create language selection menu
  for(size_t idx = 0; idx < langs.size(); ++idx)
  {
    const EsString& nativeLang = EsStringI18n::nativeLanguageNameGet(
      langs[idx]
    );
    ES_ASSERT(!nativeLang.empty());

    result->AppendCheckItem(
      startId + idx,
      nativeLang.c_str(), 
      wxString::Format(
        _("Select '%s' user interface language"), 
        nativeLang.c_str()
      )
    );
  }

  return result;
}
//--------------------------------------------------------------------------------

void EsUtilities::menuItemBitmapSet(wxMenu* mnu, int id, const wxSize& bmSize, const wxArtClient& client, const wxArtID& bm, const wxArtID& bmDisabled /*= wxEmptyString*/)
{
  ES_ASSERT(mnu);

  wxMenuItem* item = mnu->FindItem(id);
  if( !item )
    return;

  wxBitmap img;
  if( !bm.IsEmpty() )
    img = wxArtProvider::GetBitmap(
      bm,
      client,
      bmSize
    );
  if( img.IsOk() )
    item->SetBitmap(img);

  wxBitmap imgDisabled;
  if( !bmDisabled.IsEmpty() )
    imgDisabled = wxArtProvider::GetBitmap(
      bmDisabled,
      client,
      bmSize
    );
  if( imgDisabled.IsOk() )
    item->SetDisabledBitmap(imgDisabled);
}
//--------------------------------------------------------------------------------

void EsUtilities::menuClone(wxMenu* dest, const wxMenu* src)
{
  if(!src)
    return;

  ES_ASSERT(dest);

  const wxMenuItemList& items = src->GetMenuItems();
  for(auto const& item : items)
  {
    wxMenuItem* newItem = nullptr;

    if(item->IsSubMenu())
    {
      wxMenu* submenu = menuCloneCreate(
        item->GetSubMenu()
      );
      ES_ASSERT(submenu);

      newItem = dest->AppendSubMenu(
        submenu,
        item->GetItemLabel(),
        item->GetHelp()
      );
      ES_ASSERT(newItem);

      newItem->SetId(
        item->GetId()
      );
    }
    else
    {
      newItem = new wxMenuItem(
        dest,
        item->GetId(),
        item->GetItemLabel(),
        item->GetHelp(),
        item->GetKind()
      );
      ES_ASSERT(newItem);

      dest->Append(newItem);
    }

    newItem->SetBitmaps(
      item->GetBitmap(),
      item->GetBitmap(false)
    );
    newItem->SetDisabledBitmap(
      item->GetDisabledBitmap()
    );
  }
}
//--------------------------------------------------------------------------------

wxMenu* EsUtilities::menuCloneCreate(const wxMenu* src)
{
  if(!src)
    return nullptr;

  std::unique_ptr<wxMenu> dest = std::make_unique<wxMenu>(
    src->GetTitle(),
    src->GetStyle()
  );

  menuClone(
    dest.get(),
    src
  );

  return dest.release();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// return top level window given the current one
wxWindow* EsUtilities::topLevelWindowGetFor(wxWindow* wnd)
{
  while(wnd)
  {
    if(wnd->IsTopLevel())
      return wnd;

    wnd = wnd->GetParent();
  }

  return nullptr;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// GUI adjustments
//
static void menuFontSet(wxMenu* menu, const wxFont& fnt)
{
  ES_ASSERT(menu);
  wxMenuItemList& items = menu->GetMenuItems();
  for(size_t idx = 0; idx < items.size(); ++idx)
  {
    wxMenuItem* item = items[idx];
    ES_ASSERT(item);
    if(!item->IsSeparator())
    {
      menu->Remove(item);
      item->SetFont(fnt);
      menu->Insert(idx, item);
    }
    if(item->IsSubMenu())
      menuFontSet(item->GetSubMenu(), fnt);
  }
}
//--------------------------------------------------------------------------------

void EsUtilities::menuFontApply(wxMenuBar* mnu, const wxFont& fnt)
{
  if(mnu)
  {
    wxWindowUpdateLocker lock(mnu);

    for(size_t idx = 0; idx < mnu->GetMenuCount(); ++idx)
    {
      wxMenu* menu = mnu->GetMenu(idx);
      menuFontSet(menu, fnt);
    }
  }
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static void ctlsFontSet(wxWindow* wnd, const wxFont& fnt)
{
  ES_ASSERT(wnd);
  wxWindowList& children = wnd->GetChildren();
  wxSize sze;
  int height = fnt.GetPixelSize().y;
  for(size_t idx = 0; idx < children.size(); ++idx)
  {
    wxWindow* child = children[idx];
    ES_ASSERT(child);
    ctlsFontSet(child, fnt);
    child->SetFont(fnt);
    sze = child->GetClientSize();
    sze.y = wxMax(sze.y, height);
    child->SetClientSize(sze);
  }
  wnd->SetFont(fnt);
  sze = wnd->GetClientSize();
  sze.y = wxMax(sze.y, height);
  wnd->SetClientSize(sze);
}

void EsUtilities::ctlsFontApply(wxWindow* wnd, const wxFont& fnt)
{
  if(wnd)
  {
    wxWindowUpdateLocker lock(wnd);

    ctlsFontSet(wnd, fnt);
    wnd->Layout();
    wnd->Refresh();
  }
}

// our own message dialog implementation (use centralized font settings)
int EsUtilities::messageShow(const EsString& msg, const EsString& caption, long lStyle, const wxPoint& pos)
{
  wxMessageDialog dlg(wxTheApp->GetTopWindow(), msg.c_str(), caption.c_str(), lStyle, pos);
  return dlg.ShowModal();
}
//--------------------------------------------------------------------------------

// show modal error message box
void EsUtilities::errorShow(const EsString& msg, const EsString& caption/* = _("Error")*/)
{
  EsUtilities::messageShow(msg.c_str(), caption.c_str(), wxCENTRE|wxOK|wxICON_ERROR);
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8,
      arg9
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8,
      arg9,
      arg10
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8,
      arg9,
      arg10,
      arg11
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

// get longest string width in pixels, using wnd visual settings
int EsUtilities::longestStringWidthGet(const EsString::Array& strings, const wxWindow& wnd, const wxFont& fnt)
{
  int result = 0;

  for(size_t idx = 0; idx < strings.size(); ++idx)
  {
    int w, h;
    if(!fnt.IsNull())
      wnd.GetTextExtent(strings[idx].c_str(), &w, &h, 0, 0, &fnt);
    else
      wnd.GetTextExtent(strings[idx].c_str(), &w, &h);
    result = wxMax(result, w);
  }

  return result;
}
//--------------------------------------------------------------------------------

wxAuiToolBarItem* EsUtilities::toolbarToolAddFromMenuItem(wxAuiToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli /*= wxEmptyString*/, const wxArtID& bm /*= wxEmptyString*/, const wxArtID& bmDisabled /*= wxEmptyString*/)
{
  ES_ASSERT(tb);
  ES_ASSERT(mnu);

  wxMenuItem* item = mnu->FindItem(id);
  if( !item )
    return nullptr;

  wxBitmap img;
  if( !bm.IsEmpty() )
    img = wxArtProvider::GetBitmap(
      bm, 
      cli, 
      tb->GetToolBitmapSize()
    );
  if( !img.IsOk() )
    img = item->GetBitmap(false);

  wxBitmap imgDisabled;
  if( !bmDisabled.IsEmpty() )
    imgDisabled = wxArtProvider::GetBitmap(
      bmDisabled,
      cli,
      tb->GetToolBitmapSize()
    );
  if( !imgDisabled.IsOk() )
    imgDisabled = item->GetDisabledBitmap();

  return tb->AddTool(
    id, 
    item->GetItemLabelText(),
    img,
    imgDisabled,
    item->GetKind(),
    item->GetHelp(),
    wxEmptyString,
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxSize EsUtilities::appSizePropertyGet(const wxString& prop)
{
  int wh = wxSystemOptions::GetOptionInt(prop);
  ES_ASSERT(-1 != wh);
  return wxSize(
    wh,
    wh
  );
}
//--------------------------------------------------------------------------------

wxSize EsUtilities::appMainFrameMinSizeGet()
{
  int w = wxSystemOptions::GetOptionInt(ES_APP_OPT_MAINFRM_MIN_WIDTH);
  ES_ASSERT(-1 != w);

  int h = wxSystemOptions::GetOptionInt(ES_APP_OPT_MAINFRM_MIN_HEIGHT);
  ES_ASSERT(-1 != h);

  return wxSize(
    w,
    h
  );
}
//--------------------------------------------------------------------------------

wxToolBarToolBase* EsUtilities::toolbarToolAddFromMenuItem(wxToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli /*= wxEmptyString*/, const wxArtID& bm /*= wxEmptyString*/, const wxArtID& bmDisabled /*= wxEmptyString*/)
{
  ES_ASSERT(tb);
  ES_ASSERT(mnu);

  wxMenuItem* item = mnu->FindItem(id);
  if(!item)
    return nullptr;

  wxBitmap img;
  if(!bm.IsEmpty())
    img = wxArtProvider::GetBitmap(
      bm,
      cli,
      tb->GetToolBitmapSize()
    );
  if(!img.IsOk())
    img = item->GetBitmap(false);

  wxBitmap imgDisabled;
  if(!bmDisabled.IsEmpty())
    imgDisabled = wxArtProvider::GetBitmap(
      bmDisabled,
      cli,
      tb->GetToolBitmapSize()
    );
  if(!imgDisabled.IsOk())
    imgDisabled = item->GetDisabledBitmap();

  return tb->AddTool(
    id,
    item->GetItemLabelText(),
    img,
    imgDisabled,
    item->GetKind(),
    item->GetHelp(),
    wxEmptyString,
    nullptr
  );
}
//--------------------------------------------------------------------------------
