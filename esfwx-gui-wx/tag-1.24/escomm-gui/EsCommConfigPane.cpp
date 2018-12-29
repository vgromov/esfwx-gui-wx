#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsCommConfigPane.h"
#include "EsChannelIoConfigPaneFactory.h"

EsCommConfigPane::EsCommConfigPane(wxWindow* parent, 
	wxConfigBase* cfg/* = 0*/, const EsString& path /*= EsString::s_null*/) :
wxPanel(parent, wxID_ANY),
m_cfg(cfg),
m_path(path),
m_contents(new wxBoxSizer(wxVERTICAL)),
m_channels(0)
{
	m_channelNames = EsChannelIoFactory::channelNamesGet();

	wxASSERT(m_contents);
	wxBoxSizer* channel = new wxBoxSizer( wxHORIZONTAL );
	wxASSERT(channel);
	wxStaticText* lblChannel = new wxStaticText( this, wxID_ANY, _("Select channel:") );
	wxASSERT(lblChannel);
	channel->Add( lblChannel, 0, wxALL|wxEXPAND, 5 );
	
	m_channels = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	wxASSERT(m_channels);
	channel->Add( m_channels, 1, wxALL|wxEXPAND, 5 );
	m_contents->Add( channel, 0, wxEXPAND|wxALL, 5 );
	
	SetSizer( m_contents );
	Layout();
	
	// populate channel selector
	m_channels->Append( EsUtilities::toArrayString(EsChannelIoFactory::channelDescriptionsGet()) );

	if( !m_channelNames.empty() )
	{
		channelSelect(m_channelNames[0]);
		m_channels->SetSelection(0);
	}

	// connect Events
	m_channels->Bind( wxEVT_COMMAND_COMBOBOX_SELECTED, &EsCommConfigPane::onChannelSelected, this );
}

EsCommConfigPane::~EsCommConfigPane()
{
	if( m_cfg && m_curChannel &&
			!m_cfg->HasGroup(m_curChannel->typeNameGet()) )
	{
		EsStreamIntf::Ptr stream = EsStreamConfig::create(*m_cfg, m_path, EsStream::flagWrite);
		stream->objectWrite(m_curChannel);
	}

	// disconnect Events
	m_channels->Unbind( wxEVT_COMMAND_COMBOBOX_SELECTED, &EsCommConfigPane::onChannelSelected, this );
}

void EsCommConfigPane::channelSet(const EsReflectedClassIntf::Ptr& chnl)
{
	if(chnl)
	{
		EsString typeName = chnl->typeNameGet();
		channelSelect(typeName);
		wxASSERT(m_curChannel);
		m_curChannel->copyFrom(chnl);
		m_pane->updateControls(m_curChannel);
	}
}

EsReflectedClassIntf::Ptr EsCommConfigPane::channelGet() const
{
	if( m_pane && m_curChannel )
		m_pane->updateObject(m_curChannel);

	return m_curChannel;
}

void EsCommConfigPane::currentPaneReset(const EsString& chnlTypeName)
{
	if( m_pane && 
		m_pane->getObjectName() != chnlTypeName )
	{
		// save changes into m_curChannel
		if( m_cfg && m_curChannel )
			m_pane->updateObject(m_curChannel);
		
		// destroy active pane
		wxWindow* wnd = m_pane->getWindow();
		if(wnd)
			m_contents->Detach(wnd);
		m_pane.reset();
		
		// save settings of existing channel to the configuration stream
		if( m_cfg && m_curChannel )
		{
			EsStreamIntf::Ptr stream = EsStreamConfig::create(*m_cfg, m_path, EsStream::flagWrite);
			stream->objectWrite(m_curChannel);
			m_curChannel.reset();
		}
	}
}

void EsCommConfigPane::currentPaneCreate(const EsString& chnlTypeName)
{
	m_pane = EsChannelIoConfigPaneFactory::instanceGet().create(chnlTypeName, this);
	wxASSERT(m_pane);
	wxWindow* wnd = m_pane->getWindow();
	if( wnd )
	{
		m_contents->Add(wnd, wxSizerFlags().Proportion(1).Expand().Border());
		EsUtilities::ctlsFontApply(wnd, GetFont());
		Layout();
	}
}

void EsCommConfigPane::topLevelFit()
{
	wxWindow* wnd = EsUtilities::getTopLevelFor(this);
	if( wnd )
		wnd->Fit();
}

// try to select channel by name.
void EsCommConfigPane::channelSelect(const EsString& chnlTypeName)
{
	currentPaneReset(chnlTypeName);

	// create new pane && update channel instance
	if( !m_pane )
	{
		m_curChannel = EsChannelIoFactory::channelCreate(chnlTypeName);
		wxASSERT(m_curChannel);
		currentPaneCreate(chnlTypeName);

		// optionally update channel settings from persistent storage
		if( m_cfg )
		{
			EsStreamIntf::Ptr stream = EsStreamConfig::create(*m_cfg, m_path, EsStream::flagRead);
			if( stream->rootObjectTypeEntryLocate(chnlTypeName, false) )
			{
				stream->objectRead(m_curChannel);
				m_pane->updateControls(m_curChannel);
			}
		}
		
		for(size_t idx = 0; idx < m_channelNames.size(); ++idx)
		{
			const EsString& name = m_channelNames[idx];
			if( name == chnlTypeName &&
					m_channels->GetSelection() != static_cast<int>(idx) )
			{
				m_channels->Unbind( wxEVT_COMMAND_COMBOBOX_SELECTED, &EsCommConfigPane::onChannelSelected, this );
				m_channels->SetSelection(idx);
				m_channels->Bind( wxEVT_COMMAND_COMBOBOX_SELECTED, &EsCommConfigPane::onChannelSelected, this );
				break;
			}
		}

		// call fit on top level, as well
		topLevelFit();
	}
}

void EsCommConfigPane::onChannelSelected( wxCommandEvent& evt )
{
	int sel = m_channels->GetSelection();
	if( -1 != sel )
	{
		wxASSERT(sel < static_cast<int>(m_channelNames.size()));
		channelSelect(m_channelNames[sel]);
	}
	else
		currentPaneReset(EsString::s_null);
}
