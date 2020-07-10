#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsCommConfigDlg.h"
//--------------------------------------------------------------------------------

EsCommConfigDlg::EsCommConfigDlg(
  wxWindow* parent, 
  bool showProtocolPane /*= false*/,
	wxConfigBase* cfg /*= nullptr*/, 
  const EsString& path /*= EsString::null()*/
) :
wxDialog(
  parent, 
  wxID_ANY, 
  _("Communication configuration").c_str(),
  wxDefaultPosition
),
m_paneChannel(nullptr),
m_btnOK( nullptr ),
m_btnCancel( nullptr )
{
	SetClientSize( wxSize(380, 300) );

	wxBoxSizer* contents = new wxBoxSizer( wxVERTICAL );
  ES_ASSERT(contents);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	ES_ASSERT(buttons);

  m_btnOK = new wxButton( this, wxID_OK );
  ES_ASSERT(m_btnOK);

  m_btnCancel = new wxButton( this, wxID_CANCEL );
  ES_ASSERT(m_btnCancel);

	buttons->AddButton( m_btnOK );
	buttons->AddButton( m_btnCancel );
	buttons->Realize();
	
	if( showProtocolPane )
	{
		wxNotebook* nb = new wxNotebook(this, wxID_ANY);
    ES_ASSERT(nb);

		m_paneChannel = new EsChannelIoConfigPane(
      nb, 
      cfg, 
      path
    );
    ES_ASSERT(m_paneChannel);

		nb->AddPage(
      m_paneChannel, 
      _("Channel").c_str(),
      true
    );
		
    m_paneProtocol = EsIoConfigPaneFactory::create(
      EsRpcMaster::classNameGetStatic(),
      nb
    );
		nb->AddPage(
      m_paneProtocol->paneGet(), 
      _("Protocol").c_str()
    );
		
    contents->Add(
      nb, 
      wxSizerFlags().Expand()
    );
	}
	else
	{
		m_paneChannel = new EsChannelIoConfigPane(
      this, 
      cfg, 
      path
    );
		ES_ASSERT(m_paneChannel);

		contents->Add(
      m_paneChannel, 
      wxSizerFlags().Expand()
    );		
	}	

	contents->Add( 
    buttons, 
    wxSizerFlags().Expand().Border() 
  );
			
	SetSizer( contents );
	
  if( parent )
		EsUtilities::ctlsFontApply(
      this, 
      parent->GetFont()
    );
	
  Layout();
	Fit();

	CenterOnScreen();
}
//--------------------------------------------------------------------------------
