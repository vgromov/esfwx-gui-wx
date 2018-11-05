#ifndef EsReflectedClassConfigPaneH
#define EsReflectedClassConfigPaneH
//---------------------------------------------------------------------------

#include <wx/panel.h>

/// Configuration pane events category
///
#define ES_EVTC_CLASSCFG_PANE_NOTIFICATION esT("EsReflectedClassConfigPane")

enum class EsReflectedClassConfigPaneEvent : ulong
{
  SizeChanged = 1,  ///< payload[0] contains pointer to pane instance, which size was changed,
                    ///  payload[1] = x (double), payload[2] = y (double)
  PaneCreated = 2,  ///< payload[0] contains pointer to event source instance (sender),
                    ///  payload[1] = pinter to created pane instance
};

/// Base class for reflected objects configuration panes
///
class ESCORE_GUI_CLASS EsReflectedClassConfigPane : public wxPanel
{
__published:	// IDE-managed Components
  TGridPanelLayout *layContents_;

public:		// User declarations
	__fastcall EsReflectedClassConfigPane(TComponent* Owner, const EsString& className,
								const EsMetaclassIntf::Ptr& meta = EsMetaclassIntf::Ptr());

  virtual __fastcall ~EsReflectedClassConfigPane();

	EsString supportedTypeNameGet() const;
	virtual void applyFromObject(const EsReflectedClassIntf::Ptr& obj);
	virtual void applyToObject(const EsReflectedClassIntf::Ptr& obj);

  EsReflectedClassDataSource& sourceGet();

	// Return total height of frame contents
	virtual float totalHeightGet() const = 0;

  void i18nMgrSet(PlainEsI18nManagerPtrT mgr);

protected:
  void sizeChangedPost(double x, double y);

protected:
	EsReflectedClassDataSource* m_src;

private:
  EsReflectedClassConfigPane(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
  EsReflectedClassConfigPane& operator=(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------
#endif
