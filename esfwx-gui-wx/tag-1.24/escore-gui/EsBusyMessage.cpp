#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsBusyMessage.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------

static const float sc_minWidth = 200;
static const float sc_minHeight = 100;
//---------------------------------------------------------------------------

__fastcall EsBusyMessage::EsBusyMessage(TComponent* Owner, EsBusyMessageType type) :
TComponent(Owner),
m_i18nScope(this),
m_onProbe(nullptr),
m_parent(nullptr),
m_type(type),
m_align(TAlignLayout::Client),
m_popup(nullptr),
m_rect(nullptr),
m_ani(nullptr),
m_msg(nullptr),
m_shadow(nullptr),
m_pnl(nullptr),
m_layCancel(nullptr),
m_btnCancel(nullptr),
m_tmrProbe(nullptr),
m_icon(nullptr),
m_progress(nullptr),
m_progressRange(0),
m_progressValue(0),
m_requestedW(0),
m_requestedH(0)
{
  if( EsBusyMessageType::Form == m_type )
    // The second constructor form is used to create non-resourced forms
    m_popup = new TForm(this, static_cast<NativeInt>(-1));
  else
  {
    m_popup = new TLayout(this);

    m_rect = new TRectangle(m_popup);
    ES_ASSERT(m_rect);

    m_rect->Parent = m_popup;

    m_rect->Stroke->Kind = TBrushKind::None;
    m_rect->Fill->Color = TAlphaColorRec::Black;
    m_rect->Align = TAlignLayout::Contents;
  }
  ES_ASSERT(m_popup);

  if( EsBusyMessageType::Form == m_type )
  {
    TForm* popup = static_cast<TForm*>(m_popup);

    popup->FormFactor->Devices = TDeviceKinds() <<
      TDeviceKind::Desktop <<
      TDeviceKind::iPhone <<
      TDeviceKind::iPad;

    popup->FormFactor->Orientations = TFormOrientations() <<
      TFormOrientation::Portrait <<
      TFormOrientation::Landscape <<
      TFormOrientation::InvertedPortrait <<
      TFormOrientation::InvertedLandscape;

    popup->BorderStyle = TFmxFormBorderStyle::None;
    popup->FormStyle = TFormStyle::StayOnTop;
    popup->OnClose = onClosePopup;
    popup->Position = TFormPosition::MainFormCenter;

    // To make panel shadow visible
    popup->Transparency = true;
    popup->Padding->Left = 8;
    popup->Padding->Top = 8;
    popup->Padding->Right = 8;
    popup->Padding->Bottom = 8;

    popup->Visible = false;
  }
  else
    static_cast<TRectangle*>(m_popup)->Visible = false;

 	m_pnl = new TPanel(m_popup);
  ES_ASSERT(m_pnl);

  if( EsBusyMessageType::Form == m_type )
    m_pnl->Align = TAlignLayout::Client;
  else
    m_pnl->Align = TAlignLayout::Center;

  m_pnl->Parent = m_popup;

	m_shadow = new TShadowEffect(m_popup);
  ES_ASSERT(m_shadow);
  m_shadow->Parent = m_pnl;
  m_shadow->Enabled = true;

	m_ani = new TAniIndicator(m_popup);
  ES_ASSERT(m_ani);
  m_ani->Enabled = false;
  m_ani->Align = TAlignLayout::Bottom;
  m_ani->Parent = m_pnl;

	m_msg = new TLabel(m_popup);
  ES_ASSERT(m_msg);
  m_msg->TextAlign = TTextAlign::Center;
  m_msg->WordWrap = true;
  m_msg->Align = TAlignLayout::Client;
  m_msg->Parent = m_pnl;

  m_icon = new TGlyph(m_popup);
  ES_ASSERT(m_icon);
  m_icon->Width = 64;
  m_icon->HitTest = false;
  m_icon->AutoHide = true;
  m_icon->Align = TAlignLayout::Left;
  m_icon->Parent = m_pnl;

  m_progress = new TProgressBar(m_popup);
  ES_ASSERT(m_progress);
  m_progress->Margins->Bottom = 4;
  m_progress->Min = 0;
  m_progress->Height = 8;
  m_progress->Visible = false;
  m_progress->Align = TAlignLayout::Bottom;
  m_progress->Parent = m_pnl;

	m_layCancel = new TLayout(m_popup);
  ES_ASSERT(m_layCancel);
  m_layCancel->Height = 32;
  m_layCancel->Visible = false;
  m_layCancel->Align = TAlignLayout::Bottom;
  m_layCancel->Parent = m_pnl;

  m_btnCancel = new TButton(m_popup);
  ES_ASSERT(m_btnCancel);
  m_btnCancel->Text = _("Cancel");
  m_btnCancel->ModalResult = TModalResult(mrCancel);
  m_btnCancel->CanFocus = false;
  m_btnCancel->Cancel = true;
  m_btnCancel->Align = TAlignLayout::Center;
  m_btnCancel->Height = 24;
  m_btnCancel->Width = 140;
  m_btnCancel->Parent = m_layCancel;
  m_btnCancel->OnClick = btnCancel_Click;

  m_tmrProbe = new TTimer(m_popup);
  ES_ASSERT(m_tmrProbe);

  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
}
//---------------------------------------------------------------------------

__fastcall EsBusyMessage::~EsBusyMessage()
{
  Parent = 0;

  styleBook = 0;

  m_i18nScope.managerSet(0);
}
//---------------------------------------------------------------------------

void EsBusyMessage::i18nManagerSet(EsI18nManager* mgr)
{
  m_i18nScope.managerSet(mgr);
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::onGuiStringsUpdate(TObject* ES_UNUSED(sender))
{
  m_msg->Text = EsStringI18n::translate( m_msg->Text.c_str() ).c_str();
  m_btnCancel->Text = _("Cancel");
}
//---------------------------------------------------------------------------

EsBusyMessageType EsBusyMessage::typeGet() const
{
  return m_type;
}
//---------------------------------------------------------------------------

TFmxObject* EsBusyMessage::popupGet()
{
  return m_popup;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::parentSet(TFmxObject* parent)
{
  if( isShown() )
    m_popup->Parent = parent;

  m_parent = parent;
}
//---------------------------------------------------------------------------

TStyleBook* __fastcall EsBusyMessage::styleBookGet()
{
  if( EsBusyMessageType::Form == m_type )
    return static_cast<TForm*>(m_popup)->StyleBook;

  return 0;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::styleBookSet(TStyleBook* sb)
{
  if( EsBusyMessageType::Form == m_type )
    static_cast<TForm*>(m_popup)->StyleBook = sb;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::alignSet(TAlignLayout align)
{
  if( m_align != align )
  {
    m_align = align;

    if( EsBusyMessageType::Layout == m_type )
    {
      TRectangle* lay = static_cast<TRectangle*>(m_popup);
      if( lay->Visible )
        lay->Align = m_align;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::opacitySet(float opacity)
{
  if( EsBusyMessageType::Layout == m_type && m_rect )
    m_rect->Opacity = opacity;
}
//---------------------------------------------------------------------------

float __fastcall EsBusyMessage::opacityGet() const
{
  if( EsBusyMessageType::Layout == m_type && m_rect )
    return m_rect->Opacity;

  return 1;
}
//---------------------------------------------------------------------------

TCustomImageList* __fastcall EsBusyMessage::imagesGet() const
{
  ES_ASSERT(m_icon);
  return m_icon->Images;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::imagesSet(TCustomImageList* imgs)
{
  ES_ASSERT(m_icon);
  m_icon->Images = imgs;
}
//---------------------------------------------------------------------------

int __fastcall EsBusyMessage::imageIdxGet() const
{
  ES_ASSERT(m_icon);
  return m_icon->ImageIndex;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::imageIdxSet(int imgIdx)
{
  ES_ASSERT(m_icon);
  m_icon->ImageIndex = imgIdx;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::progressRangeSet(int range)
{
  if( m_progressRange != range )
  {
    m_progressRange = range;
    m_progressValue = 0;

    if( m_progressRange <= 0 )
      m_progress->Visible = false;
    else
    {
      m_progress->Min = 0;
      m_progress->Max = range;
      m_progress->Value = m_progressValue;
      m_progress->Position->Y = m_ani->Position->Y-1;
      m_progress->Visible = true;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::progressValueSet(int value)
{
  if( value < -1 )
    value = 0;

  if( value > m_progressRange )
    value = m_progressRange;

  if( m_progressValue != value )
  {
    m_progressValue = value;
    m_progress->Value = value;
  }
}
//---------------------------------------------------------------------------

bool EsBusyMessage::isShown() const
{
  if( EsBusyMessageType::Layout == m_type )
    return static_cast<TRectangle*>(m_popup)->Visible;
  else
    return static_cast<TForm*>(m_popup)->Visible;
}
//---------------------------------------------------------------------------

bool EsBusyMessage::isCancelShown() const
{
  return m_layCancel->Visible;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::onProbeSet(TNotifyEvent handler)
{
  if( m_onProbe != handler )
  {
    m_tmrProbe->Enabled = false;

    m_onProbe = handler;
    m_tmrProbe->OnTimer = m_onProbe;

    if( isShown() && m_onProbe )
      m_tmrProbe->Enabled = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::onCancelSet(TNotifyEvent handler)
{
  if( m_onCancel != handler )
    m_onCancel = handler;
}
//---------------------------------------------------------------------------

void EsBusyMessage::heightRestrict(float& h, bool useCancel)
{
  if( useCancel )
  {
    if( h < sc_minHeight + m_layCancel->Height )
      h = sc_minHeight + m_layCancel->Height;
  }
  else
  {
    if( h < sc_minHeight )
      h = sc_minHeight;
  }
}
//---------------------------------------------------------------------------

static void widthRestrict(float& w, bool useCancel)
{
  if( w < sc_minWidth )
    w = sc_minWidth;
}
//---------------------------------------------------------------------------

void EsBusyMessage::popupResize(float w, float h, bool useCancel)
{
  if( EsBusyMessageType::Form == m_type )
  {
    TForm* popup = static_cast<TForm*>(m_popup);

    popup->ClientWidth =
      w + popup->Padding->Left + popup->Padding->Right;

    popup->ClientHeight =
      h + popup->Padding->Top + popup->Padding->Bottom;

    EsUtilities::formPositionFix(popup);
  }
  else
  {
    m_pnl->Width = w + m_pnl->Padding->Left + m_pnl->Padding->Right;
    m_pnl->Height = h + m_pnl->Padding->Top + m_pnl->Padding->Bottom;
  }

  if( useCancel )
  {
    m_layCancel->Position->Y = m_ani->Position->Y+m_ani->Height+1;
	  m_layCancel->Visible = true;
  }
  else
    m_layCancel->Visible = false;
}
//---------------------------------------------------------------------------

void EsBusyMessage::popupPrepare(const EsString& message, float w, float h, bool useCancel)
{
  popupResize(w, h, useCancel);

	m_msg->Text = message.c_str();
}
//---------------------------------------------------------------------------

// simple access API
void EsBusyMessage::show(const EsString& message, bool waitable /*= false*/, float w /*= 0*/, float h /*= 0*/)
{
  m_requestedW = w;
  widthRestrict(w, waitable);

  m_requestedH = h;
  heightRestrict(h, waitable);

	if( isShown() )
		hide();

  popupPrepare(message, w, h, waitable);

  if( EsBusyMessageType::Layout == m_type )
    static_cast<TRectangle*>(m_popup)->Align = m_align;

  if( !m_parent )
  {
    TCommonCustomForm* form = Application->MainForm;
    if( form )
    {
      if( EsBusyMessageType::Form == m_type )
        static_cast<TForm*>(m_popup)->FormStyle = TFormStyle::StayOnTop;

      Parent = form;
    }
  }

  m_popup->Parent = m_parent;

  if( EsBusyMessageType::Form == m_type )
  {
    static_cast<TForm*>(m_popup)->FormStyle = TFormStyle::StayOnTop;
    static_cast<TForm*>(m_popup)->Show();
  }
  else
  {
    m_popup->BringToFront();
    static_cast<TRectangle*>(m_popup)->Visible = true;
  }

	m_ani->Enabled = true;

  if( m_onProbe )
	  m_tmrProbe->Enabled = true;
}
//---------------------------------------------------------------------------

void EsBusyMessage::cancelShow(bool show)
{
  if( isShown() && isCancelShown() != show )
  {
    float w = m_requestedW;
    widthRestrict(w, show);

    float h = m_requestedH;
    heightRestrict(h, show);

    if( EsBusyMessageType::Layout == m_type )
    {
      EsUpdateLocker<TRectangle> lock( static_cast<TRectangle*>(m_popup) );
      popupResize(w, h, show);
    }
    else
    {
      EsUpdateLocker<TForm> lock( static_cast<TForm*>(m_popup) );
      popupResize(w, h, show);
    }
  }
}
//---------------------------------------------------------------------------

void EsBusyMessage::showWaitable(const EsString& message, float w, float h)
{
  show(message, true, w, h);
}
//---------------------------------------------------------------------------

void EsBusyMessage::showNonWaitable(const EsString& message, float w, float h)
{
  show(message, false, w, h);
}
//---------------------------------------------------------------------------

void EsBusyMessage::hide()
{
	m_tmrProbe->Enabled = false;
	m_ani->Enabled = false;

  if( EsBusyMessageType::Layout == m_type )
  {
    static_cast<TRectangle*>(m_popup)->Visible = false;
    m_popup->Parent = nullptr;
  }
  else
  	static_cast<TForm*>(m_popup)->Close();

	m_layCancel->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::onClosePopup(TObject* Sender, TCloseAction &Action)
{
  Action = TCloseAction::caHide;
}
//---------------------------------------------------------------------------

void __fastcall EsBusyMessage::btnCancel_Click(TObject *Sender)
{
	hide();
  if( m_onCancel )
    m_onCancel( this );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
EsBusyMessageScope::EsBusyMessageScope(EsBusyMessage* bm, const EsString& message, float w /*= 0*/, float h /*= 0*/) :
m_bm(bm)
{
  ES_ASSERT(bm);
	m_bm->show(message, w, h);
}
//---------------------------------------------------------------------------

EsBusyMessageScope::~EsBusyMessageScope()
{
  ES_ASSERT(m_bm);
	m_bm->hide();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsBusyMessageTmpHideScope::EsBusyMessageTmpHideScope(EsBusyMessage* bm) :
m_bm(bm),
m_bmsg(0),
m_parent(0)
{
  if( m_bm->isShown() )
  {
    m_bmsg = m_bm->popupGet();
    m_parent = m_bmsg->Parent;

    if( EsBusyMessageType::Form == m_bm->typeGet() )
      static_cast<TForm*>(m_bmsg)->Hide();
    else
      static_cast<TRectangle*>(m_bmsg)->Visible = false;

    m_bmsg->Parent = 0;
  }
}
//---------------------------------------------------------------------------

EsBusyMessageTmpHideScope::~EsBusyMessageTmpHideScope()
{
  if( m_bmsg )
  {
    m_bmsg->Parent = m_parent;

    if( EsBusyMessageType::Form == m_bm->typeGet() )
    {
      TForm* popup = static_cast<TForm*>(m_bmsg);
      EsUtilities::formPositionFix(popup);
      popup->FormStyle = TFormStyle::StayOnTop;
      popup->Show();
    }
    else
    {
      TRectangle* popup = static_cast<TRectangle*>(m_bmsg);
      popup->Visible = true;
      popup->Parent = m_parent;
      popup->BringToFront();
    }
  }
}
//---------------------------------------------------------------------------


