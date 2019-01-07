#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoConfigPane.h"
#include "EsIoConfigPaneFactory.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoConfigPane::EsChannelIoConfigPane(wxWindow* parent, wxConfigBase* cfg/* = nullptr*/, const EsString& path /*= EsString::null()*/) :
wxPanel(parent, wxID_ANY),
m_path(path),
m_cfg(cfg),
m_contents(nullptr),
m_channels(nullptr)
{
	m_channelNames = EsChannelIoFactory::channelNamesGet();
	
  m_contents = new wxBoxSizer(wxVERTICAL);
  ES_ASSERT(m_contents);

	wxBoxSizer* channel = new wxBoxSizer( wxHORIZONTAL );
	ES_ASSERT(channel);

	wxStaticText* lblChannel = new wxStaticText( 
    this, 
    wxID_ANY, 
    _("Select channel:") 
  );
	ES_ASSERT(lblChannel);
	
  channel->Add( 
    lblChannel, 
    wxSizerFlags().Border().Expand()
  );
	
	m_channels = new wxComboBox( 
    this, 
    wxID_ANY, 
    wxEmptyString, 
    wxDefaultPosition, 
    wxDefaultSize, 
    0, 
    NULL,
    wxCB_READONLY 
  ); 
	ES_ASSERT(m_channels);
	
  channel->Add( 
    m_channels, 
    wxSizerFlags(1).Border().Expand()
  );

	m_contents->Add( 
    channel, 
    wxSizerFlags().Border().Expand() 
  );
	
	SetSizer( m_contents );
	Layout();
	
	// populate channel selector
	m_channels->Append( 
    EsUtilities::esStringArrayToWxArrayString(
      EsChannelIoFactory::channelDescriptionsGet()
    ) 
  );

	if( !m_channelNames.empty() )
	{
		channelSelect(
      m_channelNames[0]
    );
		m_channels->SetSelection(0);
	}

	// connect Events
	m_channels->Bind( 
    wxEVT_COMMAND_COMBOBOX_SELECTED, 
    &EsChannelIoConfigPane::onChannelSelected, 
    this 
  );
}
//--------------------------------------------------------------------------------

EsChannelIoConfigPane::~EsChannelIoConfigPane()
{
	if( m_cfg && m_paneChannel )
	{
    EsReflectedClassContainerDataSource& dsrc = dynamic_cast<EsReflectedClassContainerDataSource&>( 
      m_paneChannel->dataSourceAccess() 
    );
		//EsStreamIntf::Ptr stream = EsStreamConfig::create(*m_cfg, m_path, EsStream::flagWrite);
		//stream->objectWrite(m_channel);
	}

	// disconnect Events
	m_channels->Unbind( 
    wxEVT_COMMAND_COMBOBOX_SELECTED, 
    &EsChannelIoConfigPane::onChannelSelected, 
    this 
  );
}
//--------------------------------------------------------------------------------

void EsChannelIoConfigPane::channelSet(const EsReflectedClassIntf::Ptr& chnl)
{
	ES_ASSERT(chnl);

	const EsString& typeName = chnl->typeNameGet();
	channelSelect(typeName);
}
//--------------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsChannelIoConfigPane::channelGet() const
{
  EsReflectedClassContainerDataSource& dsrc = dynamic_cast<EsReflectedClassContainerDataSource&>( 
    m_paneChannel->dataSourceAccess() 
  );

	return  dsrc.objectGet();
}
//--------------------------------------------------------------------------------

void EsChannelIoConfigPane::currentPaneReset(const EsString& chnlTypeName)
{
	if( 
    m_paneChannel && 
		m_paneChannel->classNameGet() != chnlTypeName 
  )
	{
		// save changes into m_channel
    EsReflectedClassIntf::Ptr chnl;
    EsReflectedClassContainerDataSource& dsrc = dynamic_cast<EsReflectedClassContainerDataSource&>(
      m_paneChannel->dataSourceAccess()
    );
		if( m_cfg )
      chnl = dsrc.objectGet();

		// destroy active pane
		wxWindow* wnd = m_paneChannel->paneGet();
		if(wnd)
			m_contents->Detach(wnd);
		
		m_paneChannel.reset();
    
    // Save settings of existing channel to the configuration stream
		if( 
      m_cfg && 
      chnl
    )
		{
			//EsStreamIntf::Ptr stream = EsStreamConfig::create(*m_cfg, m_path, EsStream::flagWrite);
			//stream->objectWrite(m_channel);
		}
	}
}
//--------------------------------------------------------------------------------

void EsChannelIoConfigPane::currentPaneCreate(const EsString& chnlTypeName)
{
	m_paneChannel = EsIoConfigPaneFactory::create(
    chnlTypeName, 
    this
  );
	ES_ASSERT(m_paneChannel);

  EsReflectedClassContainerDataSource& dsrc = dynamic_cast<EsReflectedClassContainerDataSource&>( 
    m_paneChannel->dataSourceAccess() 
  );
    
  dsrc.objectSet( 
    EsChannelIoFactory::channelCreate(
      chnlTypeName
    ) 
  );

	wxWindow* wnd = m_paneChannel->paneGet();
	if( wnd )
	{
		m_contents->Add(
      wnd, 
      wxSizerFlags().Proportion(1).Expand().Border()
    );

		EsUtilities::ctlsFontApply(wnd, GetFont());
		Layout();
	}
}
//--------------------------------------------------------------------------------

void EsChannelIoConfigPane::topLevelFit()
{
	wxWindowRef wnd = EsUtilities::topLevelWindowGetFor(this);
	
  if( wnd )
		wnd->Fit();
}
//--------------------------------------------------------------------------------

void EsChannelIoConfigPane::channelSelect(const EsString& chnlTypeName)
{
	currentPaneReset(chnlTypeName);

	// create new pane && update channel instance
	if( !m_paneChannel )
	{
		currentPaneCreate(chnlTypeName);

		// optionally update channel settings from persistent storage
		if( m_cfg )
		{
      EsReflectedClassContainerDataSource& dsrc = dynamic_cast<EsReflectedClassContainerDataSource&>( 
        m_paneChannel->dataSourceAccess() 
      );
			
      //EsStreamIntf::Ptr stream = EsStreamConfig::create(*m_cfg, m_path, EsStream::flagRead);
			//if( stream->rootObjectTypeEntryLocate(chnlTypeName, false) )
			//{
			//	stream->objectRead(m_channel);
			//	m_paneChannel->updateControls(m_channel);
			//}
		}

		int sel = m_channels->GetSelection();
		for(size_t idx = 0; idx < m_channelNames.size(); ++idx)
		{
			const EsString& name = m_channelNames[idx];
			if( 
        name == chnlTypeName &&
				sel != static_cast<int>(idx) 
      )
			{
				m_channels->Unbind( wxEVT_COMMAND_COMBOBOX_SELECTED, &EsChannelIoConfigPane::onChannelSelected, this );
				m_channels->SetSelection(idx);

				m_channels->Bind( wxEVT_COMMAND_COMBOBOX_SELECTED, &EsChannelIoConfigPane::onChannelSelected, this );
				break;
			}
		}

		// call fit on top level, as well
		topLevelFit();
	}
}
//--------------------------------------------------------------------------------

void EsChannelIoConfigPane::onChannelSelected( wxCommandEvent& evt )
{
	int sel = m_channels->GetSelection();
	if( wxNOT_FOUND != sel )
	{
		ES_ASSERT(sel < static_cast<int>(m_channelNames.size()));
		
    channelSelect(m_channelNames[sel]);
	}
	else
		currentPaneReset(EsString::null());
}
//--------------------------------------------------------------------------------
