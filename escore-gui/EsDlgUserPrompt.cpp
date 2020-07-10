#include "escore_gui_pch.h"
#pragma hdrstop

#include "TDlgUserPrompt.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "TDlgBase"
#pragma resource "*.fmx"
//---------------------------------------------------------------------------

__fastcall TDlgUserPrompt::TDlgUserPrompt(TComponent* Owner) :
TDlgBase(Owner),
m_msgFontSize(12),
m_defaultImgIdx(true)
{
  BorderStyle = TFmxFormBorderStyle::Single;
  buttonsInit(mbOKCancel);
}
//---------------------------------------------------------------------------

__fastcall TDlgUserPrompt::TDlgUserPrompt(TComponent* Owner, const UnicodeString& caption, TMsgDlgButtons btns /*= mbOKCancel*/) :
TDlgBase(Owner),
m_msgFontSize(12),
m_type(TMsgDlgType::mtCustom),
m_defaultImgIdx(true)
{
  BorderStyle = TFmxFormBorderStyle::Single;

  buttonsInit(btns);

  Caption = caption;
}
//---------------------------------------------------------------------------

__fastcall TDlgUserPrompt::~TDlgUserPrompt()
{}
//---------------------------------------------------------------------------

#ifdef ES_MODERN_CPP
void TDlgUserPrompt::execute(
  TComponent* owner,
  TFormPosition pos,
  const UnicodeString& name,
  const UnicodeString& caption,
  const UnicodeString& msg,
  TMsgDlgType type,
  TMsgDlgButtons buttons,
  const EsVariant& payload,
  std::function<void (TDlgBase* Sender, TModalResult mr, const EsVariant& payload)> onClose /*= nullptr*/,
  TCustomImageList* images /*= nullptr*/,
  int imageIndex /*= -1*/
)
{
  ES_ASSERT( owner );

  TDlgUserPrompt* dlg = EsUtilities::componentCreateOrFindExisting<TDlgUserPrompt>(
    owner,
    name
  );

  ES_ASSERT(dlg);

  dlg->Position = pos;
  dlg->images = images;
  dlg->type = type;
  dlg->imageIndex = imageIndex;
  dlg->message = msg.c_str();

  if( !caption.IsEmpty() )
    dlg->Caption = caption;

  if( !buttons.Empty() )
    dlg->buttons = buttons;

  dlg->payloadSet(payload);

  dlg->showModal(onClose);
}
//---------------------------------------------------------------------------
#endif

void TDlgUserPrompt::execute(
  TComponent* owner,
  TFormPosition pos,
  const UnicodeString& name,
  const UnicodeString& caption,
  const UnicodeString& msg,
  TMsgDlgType type,
  TMsgDlgButtons buttons,
  const EsVariant& payload,
  EsDlgOnClose onClose /*= nullptr*/,
  TCustomImageList* images /*= nullptr*/,
  int imageIndex /*= -1*/
)
{
  ES_ASSERT( owner );

  TDlgUserPrompt* dlg = EsUtilities::componentCreateOrFindExisting<TDlgUserPrompt>(
    owner,
    name
  );

  ES_ASSERT(dlg);

  dlg->Position = pos;
  dlg->images = images;
  dlg->type = type;
  dlg->imageIndex = imageIndex;
  dlg->message = msg.c_str();

  if( !caption.IsEmpty() )
    dlg->Caption = caption;

  if( !buttons.Empty() )
    dlg->buttons = buttons;

  dlg->payloadSet(payload);

  dlg->showModal(onClose);
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::typeSet(TMsgDlgType type)
{
  if( m_type != type )
  {
    m_type = type;
    switch( m_type )
    {
    case TMsgDlgType::mtWarning:
      Caption = _("Warning");
      break;
    case TMsgDlgType::mtError:
      Caption = _("Error");
      break;
    case TMsgDlgType::mtInformation:
      Caption = _("Information");
      break;
    case TMsgDlgType::mtConfirmation:
      Caption = _("Confirmation");
      break;
    }

    imagesUpdate();
  }
}
//---------------------------------------------------------------------------

void TDlgUserPrompt::buttonsInit(TMsgDlgButtons btns)
{
  EsUpdateLocker<TLayout> lock(layBtns_);

  // Customize button titles
  btnCancel_->Visible = true;
  btnMed_->Visible = true;
  btnOk_->Visible = true;

  // OK button
  if( btns.Contains(TMsgDlgBtn::mbOK) )
  {
    btnOk_->Text = _("OK");
    btnOk_->ModalResult = mrOk;
  }
  else if( btns.Contains(TMsgDlgBtn::mbYes) )
  {
    btnOk_->Text = _("Yes");
    btnOk_->ModalResult = mrYes;
  }
  else if( btns.Contains(TMsgDlgBtn::mbYesToAll) )
  {
    btnOk_->Text = _("Yes to all");
    btnOk_->ModalResult = mrYesToAll;
  }
  else if( btns.Contains(TMsgDlgBtn::mbAll) )
  {
    btnOk_->Text = _("All");
    btnOk_->ModalResult = mrAll;
  }
  else if( btns.Contains(TMsgDlgBtn::mbRetry) )
  {
    btnOk_->Text = _("Retry");
    btnOk_->ModalResult = mrRetry;
  }
  else
    btnOk_->Visible = false;

  // Meduium button
  if( btns.Contains(TMsgDlgBtn::mbNo) )
  {
    btnMed_->Text = _("No");
    btnMed_->ModalResult = mrNo;
  }
  else if( btns.Contains(TMsgDlgBtn::mbIgnore) )
  {
    btnMed_->Text = _("Ignore");
    btnMed_->ModalResult = mrIgnore;
  }
  else
    btnMed_->Visible = false;

  // Cancel
  if( btns.Contains(TMsgDlgBtn::mbCancel) )
  {
    btnCancel_->Text = _("Cancel");
    btnCancel_->ModalResult = mrCancel;
  }
  else if( btns.Contains(TMsgDlgBtn::mbClose) )
  {
    btnCancel_->Text = _("Close");
    btnCancel_->ModalResult = mrClose;
  }
  else if( btns.Contains(TMsgDlgBtn::mbAbort) )
  {
    btnCancel_->Text = _("Abort");
    btnCancel_->ModalResult = mrAbort;
  }
  else if( btns.Contains(TMsgDlgBtn::mbNoToAll) )
  {
    btnCancel_->Text = _("No to all");
    btnCancel_->ModalResult = mrNoToAll;
  }
  else
    btnCancel_->Visible = false;
}
//---------------------------------------------------------------------------

void TDlgUserPrompt::onDlgShow()
{
  txtMsgHeightUpdate();
  TDlgBase::onDlgShow();
}
//---------------------------------------------------------------------------

bool TDlgUserPrompt::txtMsgHeightUpdate()
{
  TRectF initialRect = txt_->BoundsRect;
  TRectF result = EsUtilities::textRectGet(txt_, txt_->Text.c_str(), true, initialRect);
  txt_->Font->Size = m_msgFontSize;

  if( result.Height() != initialRect.Height() )
  {
    float delta = result.Height() - initialRect.Height();
    if( ClientHeight + delta < 120 )
      ClientHeight = 120;
    else
      ClientHeight = ClientHeight + delta;

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void TDlgUserPrompt::imagesUpdate()
{
  if( img_->Images && m_defaultImgIdx )
  {
    switch( m_type )
    {
    case TMsgDlgType::mtWarning:
      img_->ImageIndex = EsUtilities::imageIndexFindByName(images, esT("promptWarning"));
      break;
    case TMsgDlgType::mtError:
      img_->ImageIndex = EsUtilities::imageIndexFindByName(images, esT("promptError"));
      break;
    case TMsgDlgType::mtInformation:
      img_->ImageIndex = EsUtilities::imageIndexFindByName(images, esT("promptInformation"));
      break;
    case TMsgDlgType::mtConfirmation:
      img_->ImageIndex = EsUtilities::imageIndexFindByName(images, esT("promptConfirmation"));
      break;
    }
  }
}
//---------------------------------------------------------------------------

void TDlgUserPrompt::imageIndexResetToDefault()
{
  m_defaultImgIdx = true;
  imagesUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::btnsSet(TMsgDlgButtons btns)
{
  buttonsInit(btns);
}
//---------------------------------------------------------------------------

TCustomImageList* __fastcall TDlgUserPrompt::imagesGet() const
{
  return img_->Images;
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::imagesSet(TCustomImageList* imgs)
{
  img_->Images = imgs;
  imagesUpdate();
}
//---------------------------------------------------------------------------

int __fastcall TDlgUserPrompt::imgIndexGet() const
{
  return img_->ImageIndex;
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::imgIndexSet(int idx)
{
  if( -1 < idx )
  {
    img_->ImageIndex = idx;
    m_defaultImgIdx = false;
  }
  else
    imageIndexResetToDefault();
}
//---------------------------------------------------------------------------

UnicodeString __fastcall TDlgUserPrompt::messageGet() const
{
  return txt_->Text;
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::messageSet(const UnicodeString& msg)
{
  txt_->Text = msg;
  if( txtMsgHeightUpdate() )
    formReposition();
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::img_Changed(TObject *Sender)
{
  if( txtMsgHeightUpdate() )
    formReposition();
}
//---------------------------------------------------------------------------

void __fastcall TDlgUserPrompt::txt_ApplyStyleLookup(TObject *Sender)
{
  TText* to = dynamic_cast<TText*>( txt_->FindStyleResource(esT("text")) );
  if( to )
    m_msgFontSize = to->Font->Size;
}
//---------------------------------------------------------------------------

