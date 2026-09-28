/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "panel_sch_plugins.h"

#ifdef KICAD_IPC_API

#include <algorithm>
#include <api/api_plugin_manager.h>
#include <eda_draw_frame.h>
#include <eeschema_settings.h>
#include <launch_ext.h>
#include <pgm_base.h>
#include <settings/settings_manager.h>

#include <wx/app.h>
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>


PANEL_SCHEMATIC_API_PLUGINS::PANEL_SCHEMATIC_API_PLUGINS( wxWindow* aParent ) :
        wxPanel( aParent ),
        m_actions( nullptr ),
        m_details( nullptr ),
        m_status( nullptr ),
        m_reloadButton( nullptr ),
        m_openDirectoryButton( nullptr )
{
    wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );

    wxStaticText* description = new wxStaticText( this, wxID_ANY,
            _( "Schematic plugin actions. Check an action to show its toolbar button." ) );
    sizer->Add( description, 0, wxALL | wxEXPAND, FromDIP( 10 ) );

    m_actions = new wxCheckListBox( this, wxID_ANY );
    sizer->Add( m_actions, 1, wxLEFT | wxRIGHT | wxEXPAND, FromDIP( 10 ) );

    m_details = new wxStaticText( this, wxID_ANY, wxEmptyString );
    m_details->Wrap( FromDIP( 500 ) );
    sizer->Add( m_details, 0, wxALL | wxEXPAND, FromDIP( 10 ) );

    m_status = new wxStaticText( this, wxID_ANY, wxEmptyString );
    sizer->Add( m_status, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, FromDIP( 10 ) );

    wxBoxSizer* buttons = new wxBoxSizer( wxHORIZONTAL );
    m_openDirectoryButton = new wxButton( this, wxID_ANY, _( "Open Installed Plugin Directory" ) );
    m_reloadButton = new wxButton( this, wxID_ANY, _( "Reload Plugins" ) );
    buttons->Add( m_openDirectoryButton, 0, wxRIGHT, FromDIP( 8 ) );
    buttons->Add( m_reloadButton );
    sizer->Add( buttons, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP( 10 ) );
    SetSizer( sizer );

    m_actions->Bind( wxEVT_LISTBOX, &PANEL_SCHEMATIC_API_PLUGINS::onSelectionChanged, this );
    m_reloadButton->Bind( wxEVT_BUTTON, &PANEL_SCHEMATIC_API_PLUGINS::onReload, this );
    m_openDirectoryButton->Bind( wxEVT_BUTTON, &PANEL_SCHEMATIC_API_PLUGINS::onOpenDirectory, this );
    wxTheApp->Bind( EDA_EVT_PLUGIN_AVAILABILITY_CHANGED,
                    &PANEL_SCHEMATIC_API_PLUGINS::onPluginAvailabilityChanged, this );
}


PANEL_SCHEMATIC_API_PLUGINS::~PANEL_SCHEMATIC_API_PLUGINS()
{
    wxTheApp->Unbind( EDA_EVT_PLUGIN_AVAILABILITY_CHANGED,
                      &PANEL_SCHEMATIC_API_PLUGINS::onPluginAvailabilityChanged, this );
}


bool PANEL_SCHEMATIC_API_PLUGINS::TransferDataToWindow()
{
    EESCHEMA_SETTINGS* settings = GetAppSettings<EESCHEMA_SETTINGS>( "eeschema" );
    wxCHECK_MSG( settings, false, wxS( "Missing schematic settings" ) );
    API_PLUGIN_MANAGER& manager = Pgm().GetPluginManager();

    m_actions->Clear();
    m_identifiers.clear();
    size_t readyCount = 0;

    // 已发现但未就绪的动作也要列出，否则无法区分扫描失败与 Python 依赖失败。
    for( const PLUGIN_ACTION* action : manager.GetActionsForScope( PLUGIN_ACTION_SCOPE::SCHEMATIC,
                                                                     true ) )
    {
        bool ready = manager.IsPluginReady( action->plugin.Identifier() );
        wxString label = action->name;

        if( ready )
            ++readyCount;
        else if( manager.IsPluginBusy( action->plugin.Identifier() ) )
            label += _( " (installing dependencies)" );
        else
            label += _( " (not ready)" );

        unsigned int index = m_actions->Append( label );
        m_actions->Check( index, EDA_DRAW_FRAME::IsPluginActionButtonVisible( *action, settings ) );
        m_identifiers.push_back( action->identifier );
    }

    m_details->SetLabel( wxEmptyString );

    if( !manager.PluginErrors().empty() )
    {
        const auto& [plugin, error] = *manager.PluginErrors().begin();
        m_status->SetLabel( wxString::Format( _( "Plugin '%s' failed to load: %s" ), plugin,
                                              error.Left( 500 ) ) );
    }
    else if( m_identifiers.empty() )
    {
        m_status->SetLabel( _( "No schematic plugin actions found. Check the KiCad API setting and reload plugins." ) );
    }
    else
    {
        m_status->SetLabel( wxString::Format( _( "%zu of %zu schematic plugin actions ready." ),
                                              readyCount, m_identifiers.size() ) );
    }

    m_status->Wrap( FromDIP( 500 ) );
    m_reloadButton->Enable();
    Layout();
    return true;
}


bool PANEL_SCHEMATIC_API_PLUGINS::TransferDataFromWindow()
{
    EESCHEMA_SETTINGS* settings = GetAppSettings<EESCHEMA_SETTINGS>( "eeschema" );
    wxCHECK_MSG( settings, false, wxS( "Missing schematic settings" ) );

    // 保存当前列出的动作，同时保留尚未就绪或暂时未安装插件的既有设置。
    for( size_t i = 0; i < m_identifiers.size(); ++i )
    {
        auto it = std::find_if( settings->m_Plugins.actions.begin(), settings->m_Plugins.actions.end(),
                [&]( const auto& entry ) { return entry.first == m_identifiers[i]; } );

        if( it != settings->m_Plugins.actions.end() )
            it->second = m_actions->IsChecked( i );
        else
            settings->m_Plugins.actions.emplace_back( m_identifiers[i], m_actions->IsChecked( i ) );
    }

    wxQueueEvent( wxTheApp, new wxCommandEvent( EDA_EVT_PLUGIN_AVAILABILITY_CHANGED ) );
    return true;
}


void PANEL_SCHEMATIC_API_PLUGINS::onPluginAvailabilityChanged( wxCommandEvent& aEvent )
{
    TransferDataToWindow();
    aEvent.Skip();
}


void PANEL_SCHEMATIC_API_PLUGINS::onSelectionChanged( wxCommandEvent& aEvent )
{
    int selection = m_actions->GetSelection();

    if( selection != wxNOT_FOUND && static_cast<size_t>( selection ) < m_identifiers.size() )
    {
        auto action = Pgm().GetPluginManager().GetAction( m_identifiers[selection] );

        if( action )
        {
            const wxString& plugin = ( *action )->plugin.Identifier();
            wxString details = ( *action )->plugin.Name() + wxS( ": " ) + ( *action )->description;

            if( auto error = Pgm().GetPluginManager().PluginErrors().find( plugin );
                error != Pgm().GetPluginManager().PluginErrors().end() )
            {
                details += wxS( "\n" ) + error->second;
            }

            m_details->SetLabel( details );
            m_details->Wrap( FromDIP( 500 ) );
        }
    }

    aEvent.Skip();
}


void PANEL_SCHEMATIC_API_PLUGINS::onReload( wxCommandEvent& aEvent )
{
    m_reloadButton->Disable();
    m_status->SetLabel( _( "Reloading plugins..." ) );
    Pgm().GetPluginManager().ReloadPlugins();
}


void PANEL_SCHEMATIC_API_PLUGINS::onOpenDirectory( wxCommandEvent& aEvent )
{
    wxFileName directory = wxFileName::DirName( API_PLUGIN_MANAGER::GetThirdPartyPath() );
    directory.AppendDir( wxS( "plugins" ) );
    const wxString path = directory.GetPath();

    if( !wxFileName::DirExists( path ) || !LaunchExternal( path ) )
        wxMessageBox( wxString::Format( _( "Unable to open plugin directory '%s'." ), path ),
                      _( "Plugin Directory" ), wxOK | wxICON_ERROR, this );
}

#endif
