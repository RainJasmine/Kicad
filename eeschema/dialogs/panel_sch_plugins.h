/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PANEL_SCH_PLUGINS_H
#define PANEL_SCH_PLUGINS_H

#include <vector>
#include <wx/panel.h>
#include <wx/string.h>

class wxButton;
class wxCheckListBox;
class wxStaticText;


class PANEL_SCHEMATIC_API_PLUGINS : public wxPanel
{
public:
    explicit PANEL_SCHEMATIC_API_PLUGINS( wxWindow* aParent );
    ~PANEL_SCHEMATIC_API_PLUGINS() override;

    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

private:
    void onPluginAvailabilityChanged( wxCommandEvent& aEvent );
    void onSelectionChanged( wxCommandEvent& aEvent );
    void onReload( wxCommandEvent& aEvent );
    void onOpenDirectory( wxCommandEvent& aEvent );

    wxCheckListBox*       m_actions;
    wxStaticText*         m_details;
    wxStaticText*         m_status;
    wxButton*             m_reloadButton;
    wxButton*             m_openDirectoryButton;
    std::vector<wxString> m_identifiers;
};

#endif
