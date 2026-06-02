///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-115-g11c2dec8-dirty)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_ROUTER_SAVE_TEST_CASE_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_ROUTER_SAVE_TEST_CASE_BASE : public DIALOG_SHIM
{
	private:

	protected:
		wxStaticText* dirLabel;
		wxTextCtrl* m_testCaseDirCtrl;
		wxStaticText* nameLabel;
		wxTextCtrl* m_testCaseNameCtrl;
		wxRadioBox* m_rbType;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		wxButton* m_sdbSizer2Cancel;

	public:

		DIALOG_ROUTER_SAVE_TEST_CASE_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Save Router Testcase"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~DIALOG_ROUTER_SAVE_TEST_CASE_BASE();

};

