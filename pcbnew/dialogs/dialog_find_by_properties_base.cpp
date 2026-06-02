///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_find_by_properties_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_FIND_BY_PROPERTIES_BASE::DIALOG_FIND_BY_PROPERTIES_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 350,450 ), wxDefaultSize );

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_selectionPanel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSelSizer;
	bSelSizer = new wxBoxSizer( wxVERTICAL );

	m_statusLabel = new wxStaticText( m_selectionPanel, wxID_ANY, _("No items selected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_statusLabel->Wrap( -1 );
	bSelSizer->Add( m_statusLabel, 0, wxALL, 5 );

	m_propertyGrid = new wxGrid( m_selectionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_propertyGrid->CreateGrid( 0, 3 );
	m_propertyGrid->EnableEditing( true );
	m_propertyGrid->EnableGridLines( true );
	m_propertyGrid->EnableDragGridSize( false );
	m_propertyGrid->SetMargins( 0, 0 );

	// Columns
	m_propertyGrid->EnableDragColMove( false );
	m_propertyGrid->EnableDragColSize( true );
	m_propertyGrid->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_propertyGrid->EnableDragRowSize( true );
	m_propertyGrid->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_propertyGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_propertyGrid->SetMinSize( wxSize( -1,200 ) );

	bSelSizer->Add( m_propertyGrid, 1, wxALL|wxEXPAND, 5 );


	m_selectionPanel->SetSizer( bSelSizer );
	m_selectionPanel->Layout();
	bSelSizer->Fit( m_selectionPanel );
	m_notebook->AddPage( m_selectionPanel, _("From Selection"), false );
	m_queryPanel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bQuerySizer;
	bQuerySizer = new wxBoxSizer( wxVERTICAL );

	m_staticText4 = new wxStaticText( m_queryPanel, wxID_ANY, _("Recently Used"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bQuerySizer->Add( m_staticText4, 0, wxLEFT|wxTOP, 5 );

	m_recentQueries = new wxComboBox( m_queryPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY );
	bQuerySizer->Add( m_recentQueries, 0, wxALL|wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( m_queryPanel, wxID_ANY, _("Expression"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bQuerySizer->Add( m_staticText5, 0, wxLEFT|wxTOP, 5 );

	m_queryEditor = new wxStyledTextCtrl( m_queryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	m_queryEditor->SetUseTabs( true );
	m_queryEditor->SetTabWidth( 4 );
	m_queryEditor->SetIndent( 4 );
	m_queryEditor->SetTabIndents( true );
	m_queryEditor->SetBackSpaceUnIndents( true );
	m_queryEditor->SetViewEOL( false );
	m_queryEditor->SetViewWhiteSpace( false );
	m_queryEditor->SetMarginWidth( 2, 0 );
	m_queryEditor->SetIndentationGuides( true );
	m_queryEditor->SetReadOnly( false );
	m_queryEditor->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	m_queryEditor->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	m_queryEditor->SetMarginWidth( 1, 16);
	m_queryEditor->SetMarginSensitive( 1, true );
	m_queryEditor->SetProperty( wxT("fold"), wxT("1") );
	m_queryEditor->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	m_queryEditor->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	m_queryEditor->SetMarginWidth( 0, m_queryEditor->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	m_queryEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	m_queryEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	m_queryEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	m_queryEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	m_queryEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	m_queryEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	m_queryEditor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	m_queryEditor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	m_queryEditor->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	m_queryEditor->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	m_queryEditor->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	m_queryEditor->SetMinSize( wxSize( -1,150 ) );

	bQuerySizer->Add( m_queryEditor, 1, wxEXPAND | wxALL, 5 );

	m_queryStatusLabel = new wxStaticText( m_queryPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_queryStatusLabel->Wrap( -1 );
	bQuerySizer->Add( m_queryStatusLabel, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bQueryBtnSizer;
	bQueryBtnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_checkSyntaxBtn = new wxButton( m_queryPanel, wxID_ANY, _("Check Syntax"), wxDefaultPosition, wxDefaultSize, 0 );
	bQueryBtnSizer->Add( m_checkSyntaxBtn, 0, wxALL, 5 );

	m_createQueryBtn = new wxButton( m_queryPanel, wxID_ANY, _("Create from Selection Tab"), wxDefaultPosition, wxDefaultSize, 0 );
	m_createQueryBtn->Enable( false );

	bQueryBtnSizer->Add( m_createQueryBtn, 0, wxALL, 5 );


	bQuerySizer->Add( bQueryBtnSizer, 0, wxBOTTOM|wxLEFT, 5 );


	m_queryPanel->SetSizer( bQuerySizer );
	m_queryPanel->Layout();
	bQuerySizer->Fit( m_queryPanel );
	m_notebook->AddPage( m_queryPanel, _("Query"), false );

	bMainSizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bOptionsSizer;
	bOptionsSizer = new wxBoxSizer( wxVERTICAL );

	m_deselectNonMatching = new wxCheckBox( this, wxID_ANY, _("Deselect non-matching items"), wxDefaultPosition, wxDefaultSize, 0 );
	m_deselectNonMatching->SetValue(true);
	bOptionsSizer->Add( m_deselectNonMatching, 0, wxALL, 3 );

	m_zoomToFit = new wxCheckBox( this, wxID_ANY, _("Zoom to fit selected items"), wxDefaultPosition, wxDefaultSize, 0 );
	bOptionsSizer->Add( m_zoomToFit, 0, wxALL, 3 );


	bMainSizer->Add( bOptionsSizer, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 10 );

	wxBoxSizer* bButtonSizer;
	bButtonSizer = new wxBoxSizer( wxHORIZONTAL );


	bButtonSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_closeButton = new wxButton( this, wxID_CANCEL, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonSizer->Add( m_closeButton, 0, wxALL, 5 );

	m_selectMatchingBtn = new wxButton( this, wxID_ANY, _("Select Matching"), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonSizer->Add( m_selectMatchingBtn, 0, wxALL, 5 );


	bMainSizer->Add( bButtonSizer, 0, wxALL|wxEXPAND, 5 );


	this->SetSizer( bMainSizer );
	this->Layout();
	bMainSizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_notebook->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onNotebookPageChanged ), NULL, this );
	m_recentQueries->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onRecentQuerySelected ), NULL, this );
	m_checkSyntaxBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onCheckSyntaxClick ), NULL, this );
	m_createQueryBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onCreateQueryClick ), NULL, this );
	m_closeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::OnCloseButtonClick ), NULL, this );
	m_selectMatchingBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onSelectMatchingClick ), NULL, this );
}

DIALOG_FIND_BY_PROPERTIES_BASE::~DIALOG_FIND_BY_PROPERTIES_BASE()
{
	// Disconnect Events
	m_notebook->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onNotebookPageChanged ), NULL, this );
	m_recentQueries->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onRecentQuerySelected ), NULL, this );
	m_checkSyntaxBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onCheckSyntaxClick ), NULL, this );
	m_createQueryBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onCreateQueryClick ), NULL, this );
	m_closeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::OnCloseButtonClick ), NULL, this );
	m_selectMatchingBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_FIND_BY_PROPERTIES_BASE::onSelectMatchingClick ), NULL, this );

}
