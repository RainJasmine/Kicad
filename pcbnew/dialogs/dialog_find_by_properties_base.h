///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
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
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/combobox.h>
#include <wx/stc/stc.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_FIND_BY_PROPERTIES_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_FIND_BY_PROPERTIES_BASE : public DIALOG_SHIM
{
	private:

	protected:
		wxNotebook* m_notebook;
		wxPanel* m_selectionPanel;
		wxStaticText* m_statusLabel;
		wxGrid* m_propertyGrid;
		wxPanel* m_queryPanel;
		wxStaticText* m_staticText4;
		wxComboBox* m_recentQueries;
		wxStaticText* m_staticText5;
		wxStyledTextCtrl* m_queryEditor;
		wxStaticText* m_queryStatusLabel;
		wxButton* m_checkSyntaxBtn;
		wxButton* m_createQueryBtn;
		wxCheckBox* m_deselectNonMatching;
		wxCheckBox* m_zoomToFit;
		wxButton* m_closeButton;
		wxButton* m_selectMatchingBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onNotebookPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void onRecentQuerySelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCheckSyntaxClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCreateQueryClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCloseButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectMatchingClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		DIALOG_FIND_BY_PROPERTIES_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Find by Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~DIALOG_FIND_BY_PROPERTIES_BASE();

};

