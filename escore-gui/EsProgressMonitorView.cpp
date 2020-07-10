#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsProgressMonitorView.h"
//--------------------------------------------------------------------------------

EsProgressMonitorViewItem::EsProgressMonitorViewItem(wxWindow* parent, const EsString& path) :
wxPanel(
  parent,
  wxID_ANY,
  wxDefaultPosition,
  wxDefaultSize,
  0,
  path.c_str()
),
m_tmrPulser(
  this
),
m_txt(nullptr),
m_txtProgress(nullptr),
m_progress(nullptr),
m_contents(nullptr),
m_pulsing(false)
{
  ES_ASSERT(parent);

  m_txt = new wxStaticText(
    this, 
    wxID_ANY, 
    wxEmptyString,
    wxDefaultPosition,
    wxDefaultSize,
    wxALIGN_CENTRE_HORIZONTAL
  );
  ES_ASSERT(m_txt);

  m_txtProgress = new wxStaticText(
    this, 
    wxID_ANY, 
    wxEmptyString,
    wxDefaultPosition,
    wxDefaultSize,
    wxALIGN_CENTRE_HORIZONTAL
  );
  ES_ASSERT(m_txtProgress);

  m_progress = new wxGauge(
    this, 
    wxID_ANY, 
    100, 
    wxDefaultPosition, 
    wxSize(-1, 8), 
    wxGA_HORIZONTAL|wxGA_SMOOTH
  );
  ES_ASSERT(m_progress);

  m_contents = new wxBoxSizer(wxVERTICAL);
  ES_ASSERT(m_contents);

  m_contents->Add(m_txt, wxSizerFlags().FixedMinSize().Expand());
  m_contents->Add(m_txtProgress, wxSizerFlags().FixedMinSize().Expand());
  m_contents->Add(m_progress, wxSizerFlags().FixedMinSize().Expand());

  SetSizerAndFit(m_contents);

  Bind(wxEVT_TIMER, &EsProgressMonitorViewItem::onPulsing, this);
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::pulsingSet(bool pulsing)
{
  if( m_pulsing != pulsing )
  {
    m_pulsing = pulsing;
    
    if( m_pulsing )
    {
      m_progress->Pulse();
      m_tmrPulser.Start(500);
      progressTextSet(EsString::null());
    }
    else
    {
      m_tmrPulser.Stop();
    }
  }
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::onPulsing(wxTimerEvent& ES_UNUSED(evt))
{
  m_progress->Pulse();
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::reLayout()
{
  SetMinClientSize(
    wxSize(
      -1,
      -1
    )
  );

  m_contents->Layout();
  Fit();

  SetMinClientSize(
    wxSize(
      -1,
      GetSize().GetHeight()
    )
  );

  EsProgressMonitorViewItem* parentItem = dynamic_cast<EsProgressMonitorViewItem*>(GetParent());
  if(parentItem)
    parentItem->reLayout();
  else
  {
    EsProgressMonitorView* parent = dynamic_cast<EsProgressMonitorView*>(GetParent());
    if(parent)
      parent->reLayout();
  }
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::textSet(const EsString& txt)
{
  m_txt->SetLabelText(
    txt.c_str()
  );

  wxSizerItem* item = m_contents->GetItem(m_txt);
  ES_ASSERT(item);

  bool doLayout = false;
  if(txt.empty() && item->IsShown())
  {
    item->Show(false);
    doLayout = true;
  }
  else if(!txt.empty() && !item->IsShown())
  {
    item->Show(true);
    doLayout = true;
  }

  if(doLayout)
    reLayout();
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::progressTextSet(const EsString& txt)
{
  m_txtProgress->SetLabelText(
    txt.c_str()
  );

  wxSizerItem* item = m_contents->GetItem(m_txtProgress);
  ES_ASSERT(item);

  bool doLayout = false;
  if(txt.empty() && item->IsShown())
  {
    item->Show(false);
    doLayout = true;
  }
  else if(!txt.empty() && !item->IsShown())
  {
    item->Show(true);
    doLayout = true;
  }

  if(doLayout)
    reLayout();
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::progressSet(const EsVariant& val)
{
  if( val.isEmpty() )
    pulsingSet(true);
  else
  {
    pulsingSet(false);
    
    double dval = val.asDouble() * static_cast<double>(m_progress->GetRange());
    int pos = static_cast<int>(dval);
    if( pos > m_progress->GetRange() )
      pos = m_progress->GetRange();
    else if( pos < 0 )
      pos = 0;

    if( pos != m_progress->GetValue() )
      m_progress->SetValue(
        pos
      );
  }
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::nestedItemAdd(EsProgressMonitorViewItem* item)
{
  ES_ASSERT(item);
  ES_ASSERT(item != this);

  int level = 0;
  EsProgressMonitorViewItem* parentItem = dynamic_cast<EsProgressMonitorViewItem*>(item->GetParent());
  while( parentItem )
  {
    parentItem = dynamic_cast<EsProgressMonitorViewItem*>(parentItem->GetParent());
    ++level;
  }

  ES_DEBUG_TRACE(
    esT("Nested Item size: [%d, %d]"),
    item->GetSize().GetWidth(),
    item->GetSize().GetHeight()
  );

  m_contents->Add(
    item,
    wxSizerFlags().Border(
      wxLEFT,
      wxSystemSettings::GetMetric(wxSYS_FRAMESIZE_X) * level
    ).FixedMinSize().Expand()
  );

  reLayout();

  ES_DEBUG_TRACE(
    esT("Nested progress view item %s added to %s at level %d"),
    EsVariant(
      item->GetName().t_str(),
      EsVariant::ACCEPT_STRING
    ),
    EsVariant(
      GetName().t_str(),
      EsVariant::ACCEPT_STRING
    ),
    level
  );

  ES_DEBUG_TRACE(
    esT("Item size after: [%d, %d]"),
    GetSize().GetWidth(),
    GetSize().GetHeight()
  );
}
//--------------------------------------------------------------------------------

void EsProgressMonitorViewItem::nestedItemRemove(EsProgressMonitorViewItem* item)
{
  ES_ASSERT(item);
  ES_ASSERT(item != this);
  ES_ASSERT(this == item->GetParent());

  ES_DEBUG_TRACE(
    esT("Nested progress view item %s removed from %s"),
    EsVariant(
      item->GetName().t_str(),
      EsVariant::ACCEPT_STRING
    ),
    EsVariant(
      GetName().t_str(),
      EsVariant::ACCEPT_STRING
    )
  );

  m_contents->Show(
    item, 
    false
  );
  m_contents->Detach(
    item
  );
  item->Destroy();

  ES_DEBUG_TRACE(
    esT("Item size after: [%d, %d]"),
    GetSize().GetWidth(),
    GetSize().GetHeight()
  );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// EsEvent handler implementation for progress view
//
EsProgressMonitorView::EvtHandler::EvtHandler(EsProgressMonitorView& owner) :
EsEventSubscriber(EsString::null()), //< Handle all by default, filter what's needed in 'notify' handler
m_owner(owner)
{
  activeSet(false);
}
//--------------------------------------------------------------------------------

void EsProgressMonitorView::EvtHandler::notify(const EsEventIntf::Ptr& evt)
{
  ES_ASSERT(evt);
  const EsString& cat = evt->categoryGet();
  if(0 == cat.find(EVTC_PROGRESS_MONITOR_PFX))
  {
    ulong id = evt->idGet();
    const EsVariant& payload = evt->payloadGet();
    switch(id)
    {
    case evtTaskTextChanged:
      m_owner.onTaskTextChanged(payload);
      break;
    case evtTaskStateChanged:
      m_owner.onTaskStateChanged(payload);
      break;
    case evtTaskProgressChanged:
      m_owner.onTaskProgressChanged(payload);
      break;
    case evtTaskAdded:
      m_owner.onTaskAdded(payload);
      break;
    case evtTaskRemoved:
      m_owner.onTaskRemoved(payload);
      break;
    case evtNoMoreTasks:
      m_owner.viewClear();
      break;
    }
  }
}

//---------------------------------------------------------------------------
// progress view implementation
//
EsProgressMonitorView::EsProgressMonitorView(wxWindow* parent) :
wxPanel(
  parent,
  wxID_ANY,
  wxDefaultPosition,
  wxDefaultSize,
  0
),
m_handler(*this),
m_items(
  EsStringIndexedMap::ContainerUsesInterlock, 
  esT("EsProgressMonitorView")
),
m_contents(nullptr)
{
  m_contents = new wxBoxSizer(wxVERTICAL);
  ES_ASSERT(m_contents);

  SetSizerAndFit(m_contents);
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::progressMonitorSet(const EsProgressMonitorIntf::Ptr& mon)
{
  m_mon = mon;
  if(m_mon)
  {
    m_handler.categoriesSet(m_mon->eventCategoryGet());
    m_handler.activeSet(true);
    // try to build view of existing progress monitor contents
    viewRebuild();
  }
  else
  {
    // deactivate handler
    m_handler.activeSet(false);
    // clear view contents
    viewClear();
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::viewClear()
{
  m_contents->Clear(true);
  m_items.clear();

  m_contents->Layout();

  if(m_onReady)
    m_onReady(*this);
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::viewRebuild()
{
  viewClear();
  ES_ASSERT(m_mon);

  const EsStringArray& taskPaths = m_mon->taskPathsGet();
  if( taskPaths.empty() )
  {
    Fit();

    if( m_onReady )
      m_onReady(*this);

    return;
  }

  for( size_t idx = 0; idx < taskPaths.size(); ++idx )
  {
    const EsString& path = taskPaths[idx];
    EsProgressMonitorTaskIntf::Ptr task = m_mon->taskGetByPath(path);
    ES_ASSERT(task);

    taskViewCreate(task);
  }
}
//--------------------------------------------------------------------------------

void EsProgressMonitorView::reLayout()
{
  SetMinClientSize(
    wxSize(
      -1,
      -1
    )
  );

  m_contents->Layout();
  Fit();

  SetMinClientSize(
    wxSize(
      -1,
      GetSize().GetHeight()
    )
  );
}
//---------------------------------------------------------------------------

EsProgressMonitorViewItem* EsProgressMonitorView::taskItemFind(const EsString& path, size_t& idx) const
{
  idx = m_items.itemFind(path);
  if( EsStringIndexedMap::npos == idx )
    return nullptr;

  return reinterpret_cast<EsProgressMonitorViewItem*>(
    m_items.valueGet(idx).asPointer()
  );
}
//---------------------------------------------------------------------------

EsProgressMonitorViewItem* EsProgressMonitorView::parentTaskItemFind(const EsString& path) const
{
  EsProgressMonitorViewItem* item = nullptr;
  EsPath p = EsPath::createFromPath(path);

  if(!p.isOk())
    EsException::Throw(
      esT("Could not parse task path: '%s'"), 
      path
    );

  const EsStringArray& dirs = p.dirsGet();
  if(1 < dirs.size()) // is not top task path
  {
    p.dirLastRemove(); // contruct parent path
    const EsString& parentPath = p.pathGet(static_cast<ulong>(EsPathFlag::AsIs));
    size_t idx;
    item = taskItemFind(parentPath, idx);

    if(EsStringIndexedMap::npos == idx)
      EsException::Throw(
        esT("Could not find parent task with path '%s'"), 
        parentPath
      );
  }

  return item;
}
//---------------------------------------------------------------------------

EsProgressMonitorViewItem* EsProgressMonitorView::taskViewCreate(const EsProgressMonitorTaskIntf::Ptr& task)
{
  ES_ASSERT(task);

  return taskViewCreate(
    task->pathGet(), 
    task->textGet(), 
    task->progressTextGet(),
    task->completionGet()
  );
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::newItemInit(
  EsProgressMonitorViewItem* newItem,
  const EsString& path,
  const EsString& text,
  const EsString& progressText,
  const EsVariant& completion
)
{
  ES_ASSERT(newItem);
  m_items.itemAdd(
    path,
    EsVariant(
      newItem,
      EsVariant::ACCEPT_POINTER
    )
  );

  newItem->textSet(text);
  newItem->progressTextSet(progressText);
  newItem->progressSet(completion);
}
//--------------------------------------------------------------------------------

EsProgressMonitorViewItem* EsProgressMonitorView::taskViewCreate(
  const EsString& path,
  const EsString& text, 
  const EsString& progressText, 
  const EsVariant& completion
)
{
  EsProgressMonitorViewItem* newItem = nullptr;
  size_t idx;

  taskItemFind(
    path, 
    idx
  );

  if(EsStringIndexedMap::npos != idx)
    EsException::Throw(
      esT("Task view item with path '%s' already exists in '%s'"),
      path, 
      m_items.nameGet()
    );

  EsProgressMonitorViewItem* parentItem = parentTaskItemFind(path);

  if( parentItem )
  {
    ES_DEBUG_TRACE(esT("EsProgressMonitorView::taskViewCreate : nested"));

    newItem = new EsProgressMonitorViewItem(
      parentItem,
      path
    );

    newItemInit(
      newItem,
      path,
      text,
      progressText,
      completion
    );

    parentItem->nestedItemAdd(
      newItem
    );
  }
  else // this is top-level task item
  {
    newItem = new EsProgressMonitorViewItem(
      this,
      path
    );

    newItemInit(
      newItem,
      path,
      text,
      progressText,
      completion
    );

    m_contents->Add(
      newItem,
      wxSizerFlags().FixedMinSize().Expand()
    );

    ES_DEBUG_TRACE(esT("EsProgressMonitorView::taskViewCreate : top level"));
  }

  reLayout();

  return newItem;
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::taskViewDestroy(const EsString& path)
{
  size_t idx;
  EsProgressMonitorViewItem* item = taskItemFind(
    path, 
    idx
  );
  
  if(EsStringIndexedMap::npos != idx)
  {
    ES_ASSERT(item);

    wxWindowUpdateLocker lock(this);
    EsProgressMonitorViewItem* parentItem = dynamic_cast<EsProgressMonitorViewItem*>(item->GetParent());
    if(parentItem) //< Nested item
    {
      parentItem->nestedItemRemove(item);
      parentItem->reLayout();
    }
    else //< Top item
    {
      m_contents->Show(item, false);
      m_contents->Detach(item);
      item->Destroy();
      reLayout();
    }

    m_items.itemDelete(idx);
  }
}
//---------------------------------------------------------------------------

// EsEvent handlers
void EsProgressMonitorView::onTaskTextChanged(const EsVariant& payload)
{
  const EsString& path = payload[0].asString();
  const EsString& newText = payload[2].asString();
  size_t idx;
  EsProgressMonitorViewItem* item = taskItemFind(
    path, 
    idx
  );
  ES_ASSERT(item);

  item->textSet(newText);

  if(m_onActivity)
    m_onActivity(*this);
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::onTaskStateChanged(const EsVariant& payload)
{
  const EsString& path = payload[0].asString();
  const EsString& progressText = payload[2].asString();
  const EsVariant& completion = payload[3];

  size_t idx;
  EsProgressMonitorViewItem* item = taskItemFind(
    path, 
    idx
  );
  ES_ASSERT(item);

  item->progressTextSet(progressText);
  item->progressSet(completion);
  reLayout();

  if(m_onActivity)
    m_onActivity(*this);
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::onTaskProgressChanged(const EsVariant& payload)
{
  const EsString& path = payload[0].asString();
  const EsString& progressText = payload[1].asString();
  const EsVariant& completion = payload[2];

  size_t idx;
  EsProgressMonitorViewItem* item = taskItemFind(
    path, 
    idx
  );
  ES_ASSERT(item);

  item->progressTextSet(progressText);
  item->progressSet(completion);

  if(m_onActivity)
    m_onActivity(*this);
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::onTaskAdded(const EsVariant& payload)
{
  const EsString& path = payload[0].asString();
  const EsString& text = payload[1].asString();
  const EsString& progressText = payload[3].asString();
  const EsVariant& completion = payload[4];

  ES_DEBUG_TRACE(
    esT("EsProgressMonitorView::onTaskAdded, size before: [%d, %d]"),
    GetSize().GetWidth(),
    GetSize().GetHeight()
  );

  taskViewCreate(
    path, 
    text, 
    progressText, 
    completion
  );

  ES_DEBUG_TRACE(
    esT("EsProgressMonitorView::onTaskAdded, size after: [%d, %d]"),
    GetSize().GetWidth(),
    GetSize().GetHeight()
  );

  if(m_onActivity)
    m_onActivity(*this);
}
//---------------------------------------------------------------------------

void EsProgressMonitorView::onTaskRemoved(const EsVariant& payload)
{
  ES_DEBUG_TRACE(
    esT("EsProgressMonitorView::onTaskRemoved, size before: [%d, %d]"),
    GetSize().GetWidth(),
    GetSize().GetHeight()
  );

  taskViewDestroy(
    payload.asString()
  );

  ES_DEBUG_TRACE(
    esT("EsProgressMonitorView::onTaskRemoved, size after: [%d, %d]"),
    GetSize().GetWidth(),
    GetSize().GetHeight()
  );

  if(m_onActivity)
    m_onActivity(*this);
}
//---------------------------------------------------------------------------
