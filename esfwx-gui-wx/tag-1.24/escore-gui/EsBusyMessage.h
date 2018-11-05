#ifndef EsBusyMessageH
#define EsBusyMessageH
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

/// Busy Message container type
///
enum class EsBusyMessageType : ulong
{
  Form,
  Layout
};

/// Simple busy message frame class
///
class ESCORE_GUI_CLASS EsBusyMessage : public TComponent
{
public:		// User declarations
	__fastcall EsBusyMessage(TComponent* Owner, EsBusyMessageType type = EsBusyMessageType::Layout);
  virtual __fastcall ~EsBusyMessage();

  /// 'Message is shown' flag
  bool isShown() const;

  /// 'Cancel' is shown
  bool isCancelShown() const;

  /// Return busy message type
  EsBusyMessageType typeGet() const;

	/// Show simple busy prompt
	void show(const EsString& message, bool waitable = false, float w = 0, float h = 0);

	/// Show waitable|cancellable busy prompt.
	void showWaitable(const EsString& message, float w = 0, float h = 0);

	/// Show simple busy prompt
	void showNonWaitable(const EsString& message, float w = 0, float h = 0);

	/// Hide busy prompt
	void hide();

  /// Show|hide cancel panel
  void cancelShow(bool show);

  /// Popup form access
  TFmxObject* popupGet();

  /// Assign I18N manager
  void i18nManagerSet(EsI18nManager* mgr);

  __property TStyleBook* styleBook = {read=styleBookGet, write=styleBookSet};

  /// Pluggable event handlers
  ///

  /// Timed probe event
	__property TNotifyEvent onProbe = {read=m_onProbe, write=onProbeSet};

  /// Cancel click event
  __property TNotifyEvent onCancel = {read=m_onCancel, write=onCancelSet};

  /// Parent object access
  __property TFmxObject* Parent = {read=m_parent, write=parentSet};

  /// Parent alignment
  __property TAlignLayout Align = {read=m_align, write=alignSet};

  /// Busy message background opacity
  __property float Opacity = {read=opacityGet, write=opacitySet};

  /// Extra busy message images
  __property TCustomImageList* Images = {read=imagesGet, write=imagesSet};

  /// Extra busy message image index
  __property int ImageIndex = {read=imageIdxGet, write=imageIdxSet};

  /// Extra progress bar control. If set to <= 0, no progress is shown
  __property int ProgressRange = {read=m_progressRange, write=progressRangeSet};

  /// Progress bar current value
  __property int ProgressValue = {read=m_progressValue, write=progressValueSet};

protected:
  void heightRestrict(float& h, bool useCancel);
  void popupResize(float w, float h, bool useCancel);
  void popupPrepare(const EsString& message, float w, float h, bool useCancel);
  void __fastcall parentSet(TFmxObject* parent);
  void __fastcall onProbeSet(TNotifyEvent handler);
  void __fastcall onCancelSet(TNotifyEvent handler);
	void __fastcall btnCancel_Click(TObject *Sender);
  void __fastcall onClosePopup(TObject* Sender, TCloseAction &Action);

  TStyleBook* __fastcall styleBookGet();
  void __fastcall styleBookSet(TStyleBook* sb);

  void __fastcall alignSet(TAlignLayout align);

  float __fastcall opacityGet() const;
  void __fastcall opacitySet(float opacity);

  TCustomImageList* __fastcall imagesGet() const;
  void __fastcall imagesSet(TCustomImageList* imgs);

  int __fastcall imageIdxGet() const;
  void __fastcall imageIdxSet(int imgIdx);

  void __fastcall progressRangeSet(int range);
  void __fastcall progressValueSet(int value);

  void __fastcall onGuiStringsUpdate(TObject* sender);

protected:
  EsI18nManagerScope m_i18nScope;
  __weak TFmxObject* m_parent;

  EsBusyMessageType m_type;
  TAlignLayout m_align;
  TFmxObject* m_popup;
  TRectangle* m_rect;

	TAniIndicator* m_ani;
	TLabel* m_msg;
	TShadowEffect* m_shadow;
	TPanel* m_pnl;
	TLayout* m_layCancel;
	TButton* m_btnCancel;
	TTimer* m_tmrProbe;

  TGlyph* m_icon;

  TProgressBar* m_progress;
  int m_progressRange;
  int m_progressValue;

  float m_requestedW;
  float m_requestedH;

	TNotifyEvent m_onProbe;
  TNotifyEvent m_onCancel;
};
//---------------------------------------------------------------------------

/// Locally scoped busy message frame handler
///
class ESCORE_GUI_CLASS EsBusyMessageScope
{
public:
	EsBusyMessageScope(EsBusyMessage* bm, const EsString& message, float w = 0, float h = 0);
	~EsBusyMessageScope();

private:
  __weak EsBusyMessage* m_bm;

private:
	EsBusyMessageScope();
	EsBusyMessageScope(const EsBusyMessageScope&);
	EsBusyMessageScope& operator=(const EsBusyMessageScope&);
};
//---------------------------------------------------------------------------

/// Locally scoped busy message temporary hide|re-show
///
class ESCORE_GUI_CLASS EsBusyMessageTmpHideScope
{
public:
	EsBusyMessageTmpHideScope(EsBusyMessage* bm);
	~EsBusyMessageTmpHideScope();

private:
  __weak EsBusyMessage* m_bm;
  __weak TFmxObject* m_bmsg;
  __weak TFmxObject* m_parent;

private:
	EsBusyMessageTmpHideScope();
	EsBusyMessageTmpHideScope(const EsBusyMessageTmpHideScope&);
	EsBusyMessageTmpHideScope& operator=(const EsBusyMessageTmpHideScope&);
};
//---------------------------------------------------------------------------
#endif
