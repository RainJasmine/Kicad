///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-115-g11c2dec8-dirty)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_router_save_test_case_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_ROUTER_SAVE_TEST_CASE_BASE::DIALOG_ROUTER_SAVE_TEST_CASE_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 500,-1 ), wxDefaultSize );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

	dirLabel = new wxStaticText( this, wxID_ANY, _("Test cases directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	dirLabel->Wrap( -1 );
	fgSizer2->Add( dirLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_testCaseDirCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer2->Add( m_testCaseDirCtrl, 1, wxALL|wxEXPAND, 5 );

	nameLabel = new wxStaticText( this, wxID_ANY, _("Test case name:"), wxDefaultPosition, wxDefaultSize, 0 );
	nameLabel->Wrap( -1 );
	fgSizer2->Add( nameLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_testCaseNameCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_testCaseNameCtrl, 1, wxALL|wxEXPAND, 5 );


	bSizer10->Add( fgSizer2, 1, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );

	wxString m_rbTypeChoices[] = { _("Testcase (strict geometry)"), _("Testcase (connectivity only)"), _("Testcase (expected fail)"), _("Known bug") };
	int m_rbTypeNChoices = sizeof( m_rbTypeChoices ) / sizeof( wxString );
	m_rbType = new wxRadioBox( this, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, m_rbTypeNChoices, m_rbTypeChoices, 1, wxRA_SPECIFY_COLS );
	m_rbType->SetSelection( 0 );
	bSizer81->Add( m_rbType, 0, wxALL, 5 );


	bSizer10->Add( bSizer81, 0, 0, 5 );

	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();

	bSizer10->Add( m_sdbSizer2, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer10 );
	this->Layout();
	bSizer10->Fit( this );

	this->Centre( wxBOTH );
}

DIALOG_ROUTER_SAVE_TEST_CASE_BASE::~DIALOG_ROUTER_SAVE_TEST_CASE_BASE()
{
}
