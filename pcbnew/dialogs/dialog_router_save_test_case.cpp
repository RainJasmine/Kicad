/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <dialogs/dialog_router_save_test_case.h>

#include "router/pns_logger.h"

#include <pcb_base_edit_frame.h>

DIALOG_ROUTER_SAVE_TEST_CASE::DIALOG_ROUTER_SAVE_TEST_CASE( PCB_BASE_EDIT_FRAME* aParent, const wxString& aTestCaseDir ) :
        DIALOG_ROUTER_SAVE_TEST_CASE_BASE( aParent ),
        m_testCaseDir( aTestCaseDir )
{
}

DIALOG_ROUTER_SAVE_TEST_CASE::~DIALOG_ROUTER_SAVE_TEST_CASE()
{
}

bool DIALOG_ROUTER_SAVE_TEST_CASE::TransferDataToWindow()
{
    m_testCaseDirCtrl->SetValue( m_testCaseDir );
    return true;
}


bool DIALOG_ROUTER_SAVE_TEST_CASE::TransferDataFromWindow()
{
    switch( m_rbType->GetSelection() )
    {
        case 0: m_testCaseType = PNS::LOGGER::TCT_STRICT_GEOMETRY; break;
        case 1: m_testCaseType = PNS::LOGGER::TCT_CONNECTIVITY_ONLY; break;
        case 2: m_testCaseType = PNS::LOGGER::TCT_EXPECTED_FAIL; break;
        case 3: m_testCaseType = PNS::LOGGER::TCT_KNOWN_BUG; break;
        default: return false;
    }

    m_testCaseName = m_testCaseNameCtrl->GetValue();
    return true;
}

