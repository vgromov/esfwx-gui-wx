#include "StdAfx.h"
#pragma hdrstop

#include "EsCommConfigDlg.h"

EsCommConfigDlg::EsCommConfigDlg(wxWindow* parent, bool showProtocolPane /*= false*/,
	wxConfigBase* cfg /*= 0*/, const EsString& path /*= EsString::s_null*/) :
wxDialog(parent, wxID_ANY, _("Communication configuration"), wxDefaultPosition),
m_btnOK( new wxButton( this, wxID_OK ) ),
m_btnCancel( new wxButton( this, wxID_CANCEL ) ),
m_cfgPane(0)
{
	SetClientSize( wxSize(380, 300) );
	wxBoxSizer* contents = new wxBoxSizer( wxVERTICAL );
	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	
	buttons->AddButton( m_btnOK );
	buttons->AddButton( m_btnCancel );
	buttons->Realize();
	
	if( showProtocolPane )
	{
		wxNotebook* nb = new wxNotebook(this, wxID_ANY);
		m_cfgPane = new EsCommConfigPane(nb, cfg, path);
		wxASSERT(m_cfgPane);
		nb->AddPage(m_cfgPane, _("Channel"), true);
		EsVariant wxObj(nb);
		m_protocolPane = EsClassInfo::getClassInfo(wxT("EsProtocolConfigPane"))->
			classCall(c_reflectedCtrName, wxObj).asExistingObject();
		wxASSERT(m_protocolPane);
		nb->AddPage(m_protocolPane->getWindow(), _("Protocol"));
		contents->Add(nb, wxSizerFlags().Expand());
	}
	else
	{
		m_cfgPane = new EsCommConfigPane(this, cfg, path);
		wxASSERT(m_cfgPane);
		contents->Add(m_cfgPane, wxSizerFlags().Expand());		
	}	

	contents->Add( buttons, wxSizerFlags().Expand().Border() );
			
	SetSizer( contents );
	if( parent )
		EsUtilities::ctlsFontApply(this, parent->GetFont());
	Layout();
	Fit();

	CenterOnScreen();
}
