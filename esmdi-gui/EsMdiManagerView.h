#ifndef _es_mdi_manager_view_h_
#define _es_mdi_manager_view_h_

class ESMDIGUI_CLASS EsMdiManagerView : public wxAuiNotebook
{
public:
  // Local Event IDs
  enum {
    esmdiID_DOC_CLOSE = escoreguiID_EXTBASE,
    esmdiID_DOC_CLOSE_ALL
  };

private:
  // ES events handler
  //
	class Handler : public EsEventSubscriber
	{
	public:
		Handler(EsMdiManagerView& This, ulong precedence);
		ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	protected:
		EsMdiManagerView& m_this;
	};
	friend class Handler;
  
public:
  typedef wxWeakRef<wxWindow> DocViewPtr;

  // Modified document save preparation flags
  enum PrepareSaveFlag {
    SaveFlagNone          = 0,
    PromptSaveModified    = 0x0001,
    SaveAs                = 0x0002,
    SaveAsCopy            = 0x0004
  };

public:
  EsMdiManagerView(wxWindow* parent, wxWindowID id, const EsDocumentManagerIntf::Ptr& docmgr, ulong eventPrecedence = 100); //< By default, handle MDI events with higher precendence, than an ordinary one (1000)

  virtual ~EsMdiManagerView() ES_OVERRIDE;

  /// Connected document manager engine access.
  ///
  EsDocumentManagerIntf::Ptr documentManagerGet() ES_NOTHROW { return m_docmgr; }

  /// Return document view by document's key.
  /// Return nullptr, if not found.
  /// 
  DocViewPtr documentViewByKeyGet(const EsVariant& key) const ES_NOTHROW;

  /// Return key associated with document view (same as document, being viewed), or null, if not found
  EsVariant keyByDocumentViewGet(const DocViewPtr& view) const ES_NOTHROW;

  /// Interactive helpers for document saving and closing managements
  ///
  
  /// Interactively open new document(s)
  /// If docTypesFilter is not empty - use only types from that filter to initialize 
  /// OpenFile dialog filter string
  /// 
  void documentsOpen(const EsString::Array& docTypesFilter = EsString::nullArray());
  
  /// Interactively select path for the new document file.
  /// Returned are result values from wxOK...etc. range.
  /// 
  /// @param  doc       [in]                      - Document object, for which new path should be selected interactively
  /// @param  newPath   [in, out]                 - Returned is the value of new file path selected for document object
  ///                                               of that type
  /// @return                                     - Modal result of UI interactive path selection, like wxOK/wxYES/wxNO/wxCANCEL, etc.
  /// 
  int newFilePathSelect(const EsDocumentIntf::Ptr& doc, EsString& newPath);

  /// Document object save, with optional interactive prompt. If document is modified, and
  /// promptToSaveModified parameter is false, document's contents get saved without an interactive 
  /// user prompt.
  ///
  /// @param  doc       [in]                      - Document object to be saved
  /// @param  promptToSaveModified [in]           - Do we have to use UI interactive prompt to save the file?
  /// 
  /// @return                                     - true, if document was saved, false otherwise, either sue to prompt was declined,
  ///                                               or whatever else happened (non-exception related)
  ///                                               
  bool documentSave(const EsDocumentIntf::Ptr& doc, bool promptToSaveModified = true);

  /// Document object save-as prompt, optionally saving it as copy. If document is being saved as copy, it 
  /// it would not cause it to change its key among other opened documents in MDI manager's engine.
  /// Otherwise, document object's key upon successful saving-as is effectively replaced with the new one,
  /// after corresponding open document's keys uniqueness check is passed.
  ///
  /// @param  doc       [in]                      - Document object to be saved
  /// @param  asCopy    [in]                      - Optional flag to change save-as behaviour from the standard one (false) to
  ///                                               save-as-copy pattern (true)
  /// @return                                     - true, if operation completes successfully, false otherwise
  /// 
  bool documentSaveAs(const EsDocumentIntf::Ptr& doc, bool asCopy = false);

  /// Post close documents request to the MDI manager engine.
  /// 
  /// @param  keys      [in]                      - Single key, collection of keys, or an empty, if all documents implied.
  /// 
  void documentsCloseRequest(const EsVariant& keys = EsVariant::null()) const;

  /// Immediate documents closing handling. Returns true, if all went well
  ///
  bool documentsClose(EsVariant keys = EsVariant::null());
  
protected:
  /// Internal helpers functionality
  ///
  
  /// Tabs context Menu items initialization
  void ctxmnuI18nUpdate();
  wxMenuSharedPtr ctxmenuTabGet();

  /// Add document view to a MDI notebook 
  void documentViewAddToNotebook(const DocViewPtr& view, const EsDocumentIntf::Ptr& doc, bool activate = true);

  /// Update MDI document tab from related document object
  void documentTabUpdate(const DocViewPtr& view, const EsDocumentIntf::Ptr& doc);

  /// Check if we got to change internal pointer to currently active view, and issue necessary callbacks
  void currentDocViewChangeCheck();

  /// Handle document-accompained parameters in view 
  void documentViewParamsHandle(const EsVariant& key, const EsAssocContainerIntf::Ptr& params);

  /// Close document view
  void documentViewClose(const DocViewPtr& view);
  
  /// Find document by its key, mainly proxy to an existing document manager functionality
  EsDocumentIntf::Ptr documentFind(const EsVariant& key) const;

  /// Open new document file, or activate an existing one
  void documentOpenOrActivate(const EsVariant& key, const EsVariant& params, bool preferActivate = false);

  /// Functionality to be implemented in derived classes
  ///
  
  /// Get Document Tab text from document
  virtual wxString documentTabTextGet( const EsDocumentIntf::Ptr& doc ) = 0;

  /// Get Document Tab tooltip text from document
  virtual wxString documentTabTooltipGet( const EsDocumentIntf::Ptr& doc ) { return wxEmptyString; }

  /// Get Document Icon for notebook tab
  virtual wxBitmap documentTabIconGet( const EsDocumentIntf::Ptr& doc ) { return wxNullBitmap; }

  /// Create dicument view for specific document object, and return its pointer
  virtual wxWindow* documentViewCreate( const EsDocumentIntf::Ptr& doc ) = 0;

  /// Do something upon document view creation
  virtual void onDocumentViewCreated(const DocViewPtr& ES_UNUSED(view), const EsDocumentIntf::Ptr& ES_UNUSED(doc)) {}

  /// Do something when document tab is updated
  virtual void onDocumentTabUpdated(const DocViewPtr& ES_UNUSED(view), const EsDocumentIntf::Ptr& ES_UNUSED(doc)) {}

  /// Do something, if we're updated selected document tab
  virtual void onSelectedTabUpdated() {}

  /// Currently active view is about to change
  virtual void onActiveViewChanging() {}

  /// Currently active view has been changed
  virtual void onActiveViewChanged() {}

  /// Currently active view is about to close
  virtual void onActiveViewClosing() {}

  /// The last document view is closed
  virtual void onLastViewClosed() {}

  /// Handle document parameters in view
  virtual void doHandleDocumentViewParameters(const DocViewPtr& view, const EsAssocContainerIntf::Ptr& params) {}

  /// Overridden implementations of this method may return some additional document-related paramters, based on its object
  /// and optional, implementation-defined extra parameter. 
  /// Default implementation does nothing, returning an empty container pointer.
  /// 
  virtual EsAssocContainerIntf::Ptr documentAdditionalParamsGet(const EsDocumentIntf::Ptr& ES_UNUSED(doc), const EsVariant& ES_UNUSED(extra)) { return nullptr; }

  /// Handle interactive document close request. If request is handled, return true. If not, - return false
  /// An additional continueAnyway parameter is used in mass-close handling, and multi-close handler will continue
  /// calling requests for other keys in collection, even if particular request for specific key returns false.
  /// If document close request was handled successfully, document was saved, and document key was 
  /// changed during save process (for instance, when saving new document).
  /// 
  virtual bool doHandleDocumentCloseRequest(const EsVariant& key, bool& continueAnyway, EsVariant& keyAfterSave) = 0;

  /// Internal document save helper. Either saves document, changing its key to target, or clone it as copy,
  /// retaining current document state and key
  /// 
  void documentSaveInternal(const EsDocumentIntf::Ptr& doc, const EsVariant& target, bool asCopy = false);

  /// Helper method to handle document save interactive request pre-process.
  /// Returned are result values from wxOK...etc. range.
  /// flags         - UI interactive document save preparation flags from PrepareSaveFlag
  /// newPath       - optionally, returned is value of new, or saved-as file path
  /// 
  int documentPrepareForSave(const EsDocumentIntf::Ptr& doc, ulong flags, EsString& newPath);
  
  /// WX Event handlers
  ///
  void onDocumentTabClose(wxAuiNotebookEvent& evt);
  void onDocumentTabChanged(wxAuiNotebookEvent& evt);
  void onDocumentTabRclicked(wxAuiNotebookEvent& evt);
  void onCloseRequest(wxCommandEvent& evt);
  void onCloseAllRequest(wxCommandEvent& evt);

  /// ES event handlers
  ///
  void onI18nLocaleChanged(const EsString& lang);
  void onDocumentCreated(const EsVariant& key, const EsVariant& params);
  void onDocumentOpened(const EsVariant& key, const EsVariant& params);
  void onDocumentClosed(const EsVariant& key, const EsString& type);
  void onDocumentChanged(const EsVariant& key, const EsString& subj, void* sender);
  void onDocumentStateChanged(const EsVariant& key);
  void onDocumentKeyChanged(const EsVariant& keyOld, const EsVariant& keyNew);
  virtual void onAllDocumentsClosed() {}
  void onDocumentActivating(const EsVariant& key);
  void onDocumentsCloseRequest(const EsVariant& keys);
  
protected:
  Handler m_evtHandler;
  EsDocumentManagerIntf::Ptr m_docmgr;
  std::map<EsVariant, DocViewPtr> m_views;
  DocViewPtr m_viewActive;
  EsVariant m_keyActive;
  wxMenuSharedPtr m_pmnuTab;
  int m_rclickedIdx;

  ES_NO_DEFAULT_CTR(EsMdiManagerView);
  ES_NON_COPYABLE(EsMdiManagerView);
};

#endif //< _es_mdi_manager_view_h_
