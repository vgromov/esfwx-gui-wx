#ifndef TDlgUserPromptH
#define TDlgUserPromptH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Objects.hpp>
#include <FMX.ImgList.hpp>
#include "TDlgBase.h"
//---------------------------------------------------------------------------

class ESCORE_GUI_PKG TDlgUserPrompt : public TDlgBase
{
__published:	// IDE-managed Components
  TButton *btnMed_;
  TGlyph *img_;
  TLabel *txt_;
  void __fastcall img_Changed(TObject *Sender);
  void __fastcall txt_ApplyStyleLookup(TObject *Sender);

public:		// User declarations
  __fastcall TDlgUserPrompt(TComponent* Owner);
  __fastcall TDlgUserPrompt(TComponent* Owner, const UnicodeString& caption, TMsgDlgButtons btns = mbOKCancel);
  virtual __fastcall ~TDlgUserPrompt();

#ifdef ES_MODERN_CPP
  static void execute(
    TComponent* owner,
    TFormPosition pos,
    const UnicodeString& name,
    const UnicodeString& caption,
    const UnicodeString& msg,
    TMsgDlgType type,
    TMsgDlgButtons buttons,
    const EsVariant& payload,
    std::function<void (TDlgBase* Sender, TModalResult mr, const EsVariant& payload)> onClose = nullptr,
    TCustomImageList* images = nullptr,
    int imageIndex = -1
  );
#endif

  static void execute(
    TComponent* owner,
    TFormPosition pos,
    const UnicodeString& name,
    const UnicodeString& caption,
    const UnicodeString& msg,
    TMsgDlgType type,
    TMsgDlgButtons buttons,
    const EsVariant& payload,
    EsDlgOnClose onClose = nullptr,
    TCustomImageList* images = nullptr,
    int imageIndex = -1
  );

  __property TMsgDlgType type = {read=m_type, write=typeSet};
  __property TMsgDlgButtons buttons = {write=btnsSet};
  __property TCustomImageList* images = {read=imagesGet, write=imagesSet};
  __property int imageIndex = {read=imgIndexGet, write=imgIndexSet};
  __property UnicodeString message = {read=messageGet, write=messageSet};

protected:
  virtual void onDlgShow();

  void buttonsInit(TMsgDlgButtons btns);
  bool txtMsgHeightUpdate();

  void __fastcall typeSet(TMsgDlgType type);

  void __fastcall btnsSet(TMsgDlgButtons btns);

  TCustomImageList* __fastcall imagesGet() const;
  void __fastcall imagesSet(TCustomImageList* imgs);
  void imagesUpdate();

  int __fastcall imgIndexGet() const;
  void __fastcall imgIndexSet(int idx);

  UnicodeString __fastcall messageGet() const;
  void __fastcall messageSet(const UnicodeString& msg);

  void imageIndexResetToDefault();

protected:
  float m_msgFontSize;
  TMsgDlgType m_type;
  bool m_defaultImgIdx;
};
//---------------------------------------------------------------------------
#endif
