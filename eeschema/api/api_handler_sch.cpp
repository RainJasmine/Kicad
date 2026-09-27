/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2024 Jon Evans <jon@craftyjon.com>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <api/api_handler_sch.h>
#include <api/api_sch_utils.h>
#include <api/api_enums.h>
#include <api/api_utils.h>
#include <gal/graphics_abstraction_layer.h>
#include <libraries/symbol_library_adapter.h>
#include <lib_symbol.h>
#include <magic_enum.hpp>
#include <project_sch.h>
#include <refdes_utils.h>
#include <sch_commit.h>
#include <sch_edit_frame.h>
#include <sch_field.h>
#include <sch_junction.h>
#include <sch_line.h>
#include <sch_pin.h>
#include <sch_screen.h>
#include <sch_sheet_path.h>
#include <sch_symbol.h>
#include <schematic.h>
#include <symbol.h>
#include <string_utils.h>
#include <view/view.h>
#include <view/view_controls.h>

#include <algorithm>

#include <api/common/types/base_types.pb.h>
#include <api/schematic/schematic_types.pb.h>

using namespace kiapi::common::commands;
using kiapi::common::types::CommandStatus;
using kiapi::common::types::DocumentType;
using kiapi::common::types::ItemRequestStatus;
using namespace kiapi::schematic::commands;


// 返回创建后的实际实例数据，尤其是经过旋转、镜像后的引脚坐标。
static void packCreatedSymbol( google::protobuf::Any& aOutput, const SCH_SYMBOL& aSymbol,
                               const SCH_SHEET_PATH& aSheet )
{
    kiapi::schematic::types::Symbol result;
    result.mutable_id()->set_value( aSymbol.m_Uuid.AsStdString() );
    kiapi::common::PackVector2( *result.mutable_position(), aSymbol.GetPosition() );
    result.mutable_library_id()->CopyFrom( kiapi::common::LibIdToProto( aSymbol.GetLibId() ) );
    result.set_schematic_library_name( TO_UTF8( aSymbol.GetSchSymbolLibraryName() ) );
    result.set_unit( aSymbol.GetUnit() );
    result.set_body_style( aSymbol.GetBodyStyle() );
    result.set_orientation( aSymbol.GetOrientation() );
    result.set_reference_prefix( TO_UTF8( aSymbol.GetPrefix() ) );
    kiapi::common::PackBox2( *result.mutable_bounding_box(), aSymbol.GetBoundingBox() );

    std::vector<SCH_FIELD*> fields;
    aSymbol.GetFields( fields, false );

    for( const SCH_FIELD* field : fields )
    {
        kiapi::schematic::types::SymbolField* output = result.add_fields();
        output->set_name( TO_UTF8( field->GetCanonicalName() ) );
        output->set_field_id( static_cast<int>( field->GetId() ) );
        output->set_value( TO_UTF8( field->GetId() == FIELD_T::REFERENCE
                                             ? aSymbol.GetRef( &aSheet ) : field->GetText() ) );
        output->set_visible( field->IsVisible() );
        kiapi::common::PackVector2( *output->mutable_position(), field->GetTextPos() );
    }

    for( const SCH_PIN* pin : aSymbol.GetPins( &aSheet ) )
    {
        kiapi::schematic::types::SymbolPin* output = result.add_pins();
        output->set_number( TO_UTF8( pin->GetNumber() ) );
        output->set_name( TO_UTF8( pin->GetName() ) );
        // 实例引脚的 GetPosition() 已经是图纸坐标；再次变换会使插件导线偏离引脚。
        kiapi::common::PackVector2( *output->mutable_position(), pin->GetPosition() );
        output->set_unit( pin->GetUnit() );
        output->set_body_style( pin->GetBodyStyle() );
    }

    aOutput.PackFrom( result );
}


API_HANDLER_SCH::API_HANDLER_SCH( SCH_EDIT_FRAME* aFrame ) :
        // 基类处理 BeginCommit 等通用请求时需要编辑器窗口来判断是否忙碌。
        API_HANDLER_EDITOR( aFrame ),
        m_frame( aFrame )
{
    registerHandler<GetOpenDocuments, GetOpenDocumentsResponse>(
            &API_HANDLER_SCH::handleGetOpenDocuments );
    registerHandler<SearchSymbolLibrary, SearchSymbolLibraryResponse>(
            &API_HANDLER_SCH::handleSearchSymbolLibrary );
    registerHandler<GetSymbolLibraryDefinition, SymbolLibraryDefinition>(
            &API_HANDLER_SCH::handleGetSymbolLibraryDefinition );
    registerHandler<GetActiveSchematicContext, ActiveSchematicContext>(
            &API_HANDLER_SCH::handleGetActiveSchematicContext );
}


std::unique_ptr<COMMIT> API_HANDLER_SCH::createCommit()
{
    return std::make_unique<SCH_COMMIT>( m_frame );
}


bool API_HANDLER_SCH::validateDocumentInternal( const DocumentSpecifier& aDocument ) const
{
    return resolveSheetPath( aDocument ).has_value();
}


std::optional<SCH_SHEET_PATH> API_HANDLER_SCH::resolveSheetPath(
        const DocumentSpecifier& aDocument ) const
{
    if( !m_frame || aDocument.type() != DocumentType::DOCTYPE_SCHEMATIC
        || m_frame->GetCurrentSheet().empty() )
    {
        return std::nullopt;
    }

    if( !aDocument.has_sheet_path()
        || ( aDocument.sheet_path().path().empty()
             && aDocument.sheet_path().path_human_readable().empty() ) )
    {
        return m_frame->GetCurrentSheet();
    }

    const auto& requested = aDocument.sheet_path();
    SCH_SHEET_LIST hierarchy = m_frame->Schematic().Hierarchy();

    if( !requested.path().empty() )
    {
        KIID_PATH path;

        for( const kiapi::common::types::KIID& id : requested.path() )
        {
            if( id.value().empty() )
                return std::nullopt;

            path.push_back( KIID( id.value() ) );
        }

        return hierarchy.GetSheetPathByKIIDPath( path, true );
    }

    const wxString human = wxString::FromUTF8( requested.path_human_readable().c_str() );
    std::optional<SCH_SHEET_PATH> match;

    for( const SCH_SHEET_PATH& candidate : hierarchy )
    {
        if( candidate.PathHumanReadable() == human
            || candidate.PathHumanReadable( false ) == human )
        {
            // 同名图纸路径可能有歧义；此时要求插件传入稳定的 KIID 路径。
            if( match )
                return std::nullopt;

            match = candidate;
        }
    }

    return match;
}


SCH_SCREEN* API_HANDLER_SCH::resolveScreen( const DocumentSpecifier& aDocument ) const
{
    std::optional<SCH_SHEET_PATH> path = resolveSheetPath( aDocument );
    return path ? path->LastScreen() : nullptr;
}


HANDLER_RESULT<GetOpenDocumentsResponse> API_HANDLER_SCH::handleGetOpenDocuments(
        const HANDLER_CONTEXT<GetOpenDocuments>& aCtx )
{
    if( aCtx.Request.type() != DocumentType::DOCTYPE_SCHEMATIC )
    {
        ApiResponseStatus e;

        // No message needed for AS_UNHANDLED; this is an internal flag for the API server
        e.set_status( ApiStatusCode::AS_UNHANDLED );
        return tl::unexpected( e );
    }

    GetOpenDocumentsResponse response;

    if( !m_frame || m_frame->GetCurrentSheet().empty() )
        return response;

    // 枚举层级图纸路径；同一个 SCH_SCREEN 可以被多个图纸实例复用。
    for( const SCH_SHEET_PATH& path : m_frame->Schematic().Hierarchy() )
    {
        DocumentSpecifier* doc = response.add_documents();
        doc->set_type( DocumentType::DOCTYPE_SCHEMATIC );
        doc->mutable_project()->set_name( TO_UTF8( m_frame->Prj().GetProjectName() ) );
        doc->mutable_project()->set_path( TO_UTF8( m_frame->Prj().GetProjectDirectory() ) );
        doc->mutable_sheet_path()->set_path_human_readable(
                TO_UTF8( path.PathHumanReadable() ) );

        for( const KIID& id : path.Path() )
            doc->mutable_sheet_path()->add_path()->set_value( id.AsStdString() );
    }
    return response;
}


void API_HANDLER_SCH::fillSymbolSummary( SymbolLibrarySummary& aOutput,
                                         const LIB_SYMBOL& aSymbol ) const
{
    aOutput.mutable_library_id()->CopyFrom( kiapi::common::LibIdToProto( aSymbol.GetLibId() ) );
    aOutput.set_display_name( TO_UTF8( aSymbol.GetName() ) );
    aOutput.set_description( TO_UTF8( aSymbol.GetDescription() ) );
    aOutput.set_keywords( TO_UTF8( aSymbol.GetKeyWords() ) );
    aOutput.set_reference_prefix( TO_UTF8( aSymbol.GetReferenceField().GetText() ) );
    aOutput.set_unit_count( std::max( 1, aSymbol.GetUnitCount() ) );
    aOutput.set_body_style_count( std::max( 1, aSymbol.GetBodyStyleCount() ) );
}


HANDLER_RESULT<SearchSymbolLibraryResponse> API_HANDLER_SCH::handleSearchSymbolLibrary(
        const HANDLER_CONTEXT<SearchSymbolLibrary>& aCtx )
{
    if( !m_frame || m_frame->GetCurrentSheet().empty() )
    {
        ApiResponseStatus error;
        error.set_status( ApiStatusCode::AS_BAD_REQUEST );
        error.set_error_message( "no schematic is open" );
        return tl::unexpected( error );
    }

    SYMBOL_LIBRARY_ADAPTER* adapter = PROJECT_SCH::SymbolLibAdapter( &m_frame->Prj() );

    if( !adapter )
    {
        ApiResponseStatus error;
        error.set_status( ApiStatusCode::AS_UNHANDLED );
        error.set_error_message( "the project symbol library adapter is unavailable" );
        return tl::unexpected( error );
    }

    const wxString query = wxString::FromUTF8( aCtx.Request.query().c_str() );
    const uint32_t limit = std::min<uint32_t>(
            aCtx.Request.max_results() == 0 ? 100 : aCtx.Request.max_results(), 1000 );
    std::vector<wxString> libraries;

    if( aCtx.Request.has_library_nickname() )
    {
        wxString nickname = wxString::FromUTF8( aCtx.Request.library_nickname().c_str() );

        if( !adapter->HasLibrary( nickname ) )
        {
            ApiResponseStatus error;
            error.set_status( ApiStatusCode::AS_BAD_REQUEST );
            error.set_error_message( "the requested symbol library is not in the project library table" );
            return tl::unexpected( error );
        }

        libraries.push_back( nickname );
    }
    else
    {
        libraries = adapter->GetLibraryNames();
    }

    SearchSymbolLibraryResponse response;

    for( const wxString& nickname : libraries )
    {
        std::vector<wxString> names;

        try
        {
            names = adapter->GetSymbolNames( nickname );
        }
        catch( const IO_ERROR& )
        {
            continue;
        }

        for( const wxString& name : names )
        {
            bool matches = false;

            switch( aCtx.Request.match_mode() )
            {
            case SLMM_EXACT:  matches = name.CmpNoCase( query ) == 0; break;
            case SLMM_PREFIX: matches = name.Lower().StartsWith( query.Lower() ); break;
            case SLMM_SUBSTRING:
            default:          matches = name.Lower().Contains( query.Lower() ); break;
            }

            // 先按枚举名称过滤，再加载候选符号，避免无关库对象触发插件读取。
            if( !matches )
                continue;

            LIB_SYMBOL* symbol = nullptr;

            try
            {
                symbol = adapter->LoadSymbol( nickname, name );
            }
            catch( const IO_ERROR& )
            {
                continue;
            }

            // LoadSymbol 的指针由库管理器持有；这里仅借用，不释放。
            if( !symbol )
                continue;

            fillSymbolSummary( *response.add_symbols(), *symbol );

            if( static_cast<uint32_t>( response.symbols_size() ) >= limit )
                return response;
        }
    }

    return response;
}


HANDLER_RESULT<SymbolLibraryDefinition> API_HANDLER_SCH::handleGetSymbolLibraryDefinition(
        const HANDLER_CONTEXT<GetSymbolLibraryDefinition>& aCtx )
{
    const auto& protoId = aCtx.Request.library_id();

    if( !m_frame || m_frame->GetCurrentSheet().empty()
        || protoId.library_nickname().empty() || protoId.entry_name().empty() )
    {
        ApiResponseStatus error;
        error.set_status( ApiStatusCode::AS_BAD_REQUEST );
        error.set_error_message( "an open schematic and exact library nickname and entry name are required" );
        return tl::unexpected( error );
    }

    SYMBOL_LIBRARY_ADAPTER* adapter = PROJECT_SCH::SymbolLibAdapter( &m_frame->Prj() );
    LIB_SYMBOL* symbol = nullptr;

    if( adapter )
    {
        try
        {
            symbol = adapter->LoadSymbol( kiapi::common::LibIdFromProto( protoId ) );
        }
        catch( const IO_ERROR& )
        {
        }
    }

    if( !symbol )
    {
        ApiResponseStatus error;
        error.set_status( ApiStatusCode::AS_BAD_REQUEST );
        error.set_error_message( "the requested symbol was not found in the project libraries" );
        return tl::unexpected( error );
    }

    SymbolLibraryDefinition response;
    fillSymbolSummary( *response.mutable_summary(), *symbol );
    kiapi::common::PackBox2( *response.mutable_bounding_box(), symbol->GetBoundingBox() );

    std::vector<SCH_FIELD*> fields;
    symbol->GetFields( fields );

    for( const SCH_FIELD* field : fields )
    {
        SymbolLibraryField* output = response.add_default_fields();
        output->set_name( TO_UTF8( field->GetName( false ) ) );
        output->set_value( TO_UTF8( field->GetText() ) );
        kiapi::common::PackVector2( *output->mutable_position(), field->GetPosition() );
        output->set_visible( field->IsVisible() );
    }

    const int unitCount = std::max( 1, symbol->GetUnitCount() );
    const int bodyStyleCount = std::max( 1, symbol->GetBodyStyleCount() );
    const std::vector<SCH_PIN*> pins = symbol->GetPins();

    for( int unit = 1; unit <= unitCount; ++unit )
    {
        for( int bodyStyle = 1; bodyStyle <= bodyStyleCount; ++bodyStyle )
        {
            SymbolLibraryVariant* variant = response.add_variants();
            variant->set_unit( unit );
            variant->set_body_style( bodyStyle );
            kiapi::common::PackBox2( *variant->mutable_bounding_box(),
                                     symbol->GetUnitBoundingBox( unit, bodyStyle ) );

            for( const SCH_PIN* pin : pins )
            {
                if( ( pin->GetUnit() != 0 && pin->GetUnit() != unit )
                    || ( pin->GetBodyStyle() != 0 && pin->GetBodyStyle() != bodyStyle ) )
                {
                    continue;
                }

                SymbolLibraryPin* output = variant->add_pins();
                output->set_number( TO_UTF8( pin->GetNumber() ) );
                output->set_name( TO_UTF8( pin->GetName() ) );
                kiapi::common::PackVector2( *output->mutable_position(), pin->GetPosition() );
                output->set_unit( pin->GetUnit() );
                output->set_body_style( pin->GetBodyStyle() );
            }
        }
    }

    return response;
}


HANDLER_RESULT<ActiveSchematicContext> API_HANDLER_SCH::handleGetActiveSchematicContext(
        const HANDLER_CONTEXT<GetActiveSchematicContext>& )
{
    if( !m_frame || m_frame->GetCurrentSheet().empty() )
    {
        ApiResponseStatus error;
        error.set_status( ApiStatusCode::AS_BAD_REQUEST );
        error.set_error_message( "no schematic is open" );
        return tl::unexpected( error );
    }

    ActiveSchematicContext response;
    const SCH_SHEET_PATH& path = m_frame->GetCurrentSheet();
    DocumentSpecifier* document = response.mutable_document();
    document->set_type( DocumentType::DOCTYPE_SCHEMATIC );
    document->mutable_project()->set_name( TO_UTF8( m_frame->Prj().GetProjectName() ) );
    document->mutable_project()->set_path( TO_UTF8( m_frame->Prj().GetProjectDirectory() ) );
    document->mutable_sheet_path()->set_path_human_readable(
            TO_UTF8( path.PathHumanReadable() ) );

    for( const KIID& id : path.Path() )
        document->mutable_sheet_path()->add_path()->set_value( id.AsStdString() );

    if( SCH_SCREEN* screen = path.LastScreen() )
        response.set_screen_filename( TO_UTF8( screen->GetFileName() ) );

    if( m_frame->GetCanvas() && m_frame->GetCanvas()->GetView() )
    {
        KIGFX::VIEW* view = m_frame->GetCanvas()->GetView();
        const BOX2D viewport = view->GetViewport();
        response.mutable_visible_canvas()->mutable_position()->set_x_nm(
                static_cast<int64_t>( viewport.GetOrigin().x ) );
        response.mutable_visible_canvas()->mutable_position()->set_y_nm(
                static_cast<int64_t>( viewport.GetOrigin().y ) );
        response.mutable_visible_canvas()->mutable_size()->set_x_nm(
                static_cast<int64_t>( viewport.GetSize().x ) );
        response.mutable_visible_canvas()->mutable_size()->set_y_nm(
                static_cast<int64_t>( viewport.GetSize().y ) );

        if( KIGFX::GAL* gal = view->GetGAL() )
        {
            const VECTOR2D& grid = gal->GetGridSize();
            response.mutable_grid()->set_x_nm( static_cast<int64_t>( grid.x ) );
            response.mutable_grid()->set_y_nm( static_cast<int64_t>( grid.y ) );
        }

        if( KIGFX::VIEW_CONTROLS* controls = m_frame->GetCanvas()->GetViewControls() )
        {
            const VECTOR2D cursor = controls->GetCursorPosition();
            response.mutable_cursor()->set_x_nm( static_cast<int64_t>( cursor.x ) );
            response.mutable_cursor()->set_y_nm( static_cast<int64_t>( cursor.y ) );
        }
    }

    response.set_coordinate_unit( "schematic_internal_unit" );
    return response;
}


HANDLER_RESULT<std::unique_ptr<EDA_ITEM>> API_HANDLER_SCH::createItemForType( KICAD_T aType,
        EDA_ITEM* aContainer )
{
    if( !aContainer && aType != SCH_LINE_T && aType != SCH_JUNCTION_T )
    {
        ApiResponseStatus e;
        e.set_status( ApiStatusCode::AS_BAD_REQUEST );
        e.set_error_message( "Tried to create an item in a null container" );
        return tl::unexpected( e );
    }

    if( aType == SCH_PIN_T && !dynamic_cast<SCH_SYMBOL*>( aContainer ) )
    {
        ApiResponseStatus e;
        e.set_status( ApiStatusCode::AS_BAD_REQUEST );
        e.set_error_message( fmt::format( "Tried to create a pin in {}, which is not a symbol",
                                          aContainer->GetFriendlyName().ToStdString() ) );
        return tl::unexpected( e );
    }

    std::unique_ptr<EDA_ITEM> created = CreateItemForType( aType, aContainer );

    if( !created )
    {
        ApiResponseStatus e;
        e.set_status( ApiStatusCode::AS_BAD_REQUEST );
        e.set_error_message( fmt::format( "Tried to create an item of type {}, which is unhandled",
                                          magic_enum::enum_name( aType ) ) );
        return tl::unexpected( e );
    }

    return created;
}


HANDLER_RESULT<ItemRequestStatus> API_HANDLER_SCH::handleCreateUpdateItemsInternal( bool aCreate,
        const std::string& aClientName,
        const types::ItemHeader &aHeader,
        const google::protobuf::RepeatedPtrField<google::protobuf::Any>& aItems,
        std::function<void( ItemStatus, google::protobuf::Any )> aItemHandler )
{
    ApiResponseStatus e;

    auto containerResult = validateItemHeaderDocument( aHeader );

    if( !containerResult && containerResult.error().status() == ApiStatusCode::AS_UNHANDLED )
    {
        // No message needed for AS_UNHANDLED; this is an internal flag for the API server
        e.set_status( ApiStatusCode::AS_UNHANDLED );
        return tl::unexpected( e );
    }
    else if( !containerResult )
    {
        e.CopyFrom( containerResult.error() );
        return tl::unexpected( e );
    }

    // 使用请求中的层级图纸路径，不能把其他图纸的请求落到当前画布。
    SCH_SCREEN* screen = resolveScreen( aHeader.document() );

    if( !screen )
    {
        e.set_status( ApiStatusCode::AS_BAD_REQUEST );
        e.set_error_message( "the requested schematic sheet is not open" );
        return tl::unexpected( e );
    }

    EE_RTREE& screenItems = screen->Items();

    std::map<KIID, EDA_ITEM*> itemUuidMap;

    std::for_each( screenItems.begin(), screenItems.end(),
                   [&]( EDA_ITEM* aItem )
                   {
                       itemUuidMap[aItem->m_Uuid] = aItem;
                   } );

    EDA_ITEM* container = nullptr;

    if( containerResult->has_value() )
    {
        const KIID& containerId = **containerResult;

        if( itemUuidMap.count( containerId ) )
        {
            container = itemUuidMap.at( containerId );

            if( !container )
            {
                e.set_status( ApiStatusCode::AS_BAD_REQUEST );
                e.set_error_message( fmt::format(
                        "The requested container {} is not a valid schematic item container",
                        containerId.AsStdString() ) );
                return tl::unexpected( e );
            }
        }
        else
        {
            e.set_status( ApiStatusCode::AS_BAD_REQUEST );
            e.set_error_message( fmt::format(
                    "The requested container {} does not exist in this document",
                    containerId.AsStdString() ) );
            return tl::unexpected( e );
        }
    }

    COMMIT* commit = getCurrentCommit( aClientName );
    const std::optional<SCH_SHEET_PATH> sheetPath = resolveSheetPath( aHeader.document() );

    for( const google::protobuf::Any& anyItem : aItems )
    {
        ItemStatus status;
        std::optional<KICAD_T> type = TypeNameFromAny( anyItem );

        if( !type )
        {
            status.set_code( ItemStatusCode::ISC_INVALID_TYPE );
            status.set_error_message( fmt::format( "Could not decode a valid type from {}",
                                                   anyItem.type_url() ) );
            aItemHandler( status, anyItem );
            continue;
        }

        std::unique_ptr<EDA_ITEM> item;

        if( *type == SCH_SYMBOL_T && aCreate && !container && sheetPath )
        {
            kiapi::schematic::types::Symbol request;

            if( !anyItem.UnpackTo( &request ) || !request.has_position()
                || request.library_id().library_nickname().empty()
                || request.library_id().entry_name().empty() )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "symbol requires a position and exact library ID" );
                aItemHandler( status, anyItem );
                continue;
            }

            const int unit = request.unit() == 0 ? 1 : request.unit();
            const int bodyStyle = request.body_style() == 0 ? 1 : request.body_style();
            const int orientation = request.orientation() == 0 ? SYM_ORIENT_0
                                                             : request.orientation();
            const int rotation = orientation & 0xff;
            const int mirror = orientation & ~0xff;

            if( rotation < SYM_ORIENT_0 || rotation > SYM_ORIENT_270
                || ( mirror != 0 && mirror != SYM_MIRROR_X && mirror != SYM_MIRROR_Y
                     && mirror != ( SYM_MIRROR_X | SYM_MIRROR_Y ) ) )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "invalid symbol orientation" );
                aItemHandler( status, anyItem );
                continue;
            }

            LIB_ID libId = kiapi::common::LibIdFromProto( request.library_id() );
            SYMBOL_LIBRARY_ADAPTER* adapter = PROJECT_SCH::SymbolLibAdapter( &m_frame->Prj() );
            LIB_SYMBOL* librarySymbol = nullptr;

            if( adapter )
            {
                try
                {
                    // LoadSymbol 返回工程库持有的对象，实例构造函数会复制所需的符号定义。
                    librarySymbol = adapter->LoadSymbol( libId );
                }
                catch( const IO_ERROR& )
                {
                }
            }

            if( !librarySymbol || unit < 1 || unit > std::max( 1, librarySymbol->GetUnitCount() )
                || bodyStyle < 1 || bodyStyle > std::max( 1, librarySymbol->GetBodyStyleCount() ) )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "symbol is unavailable or its unit/body style is invalid" );
                aItemHandler( status, anyItem );
                continue;
            }

            auto symbol = std::make_unique<SCH_SYMBOL>( *librarySymbol, libId, &*sheetPath,
                    unit, bodyStyle, kiapi::common::UnpackVector2( request.position() ) );
            symbol->SetOrientation( orientation );

            if( !request.id().value().empty() )
                const_cast<KIID&>( symbol->m_Uuid ) = KIID( request.id().value() );

            if( !request.reference_prefix().empty() )
            {
                wxString reference = UTIL::GetRefDesUnannotated(
                        wxString::FromUTF8( request.reference_prefix() ) );
                symbol->SetRef( &*sheetPath, reference );
                symbol->GetField( FIELD_T::REFERENCE )->SetText( reference );
            }

            for( const kiapi::schematic::types::SymbolField& override : request.fields() )
            {
                SCH_FIELD* field = nullptr;

                if( override.field_id() >= static_cast<int>( FIELD_T::REFERENCE )
                    && override.field_id() <= static_cast<int>( FIELD_T::DESCRIPTION ) )
                {
                    field = symbol->GetField( static_cast<FIELD_T>( override.field_id() ) );
                }
                else if( !override.name().empty() )
                {
                    wxString name = wxString::FromUTF8( override.name() );
                    field = symbol->GetField( name );

                    if( !field )
                    {
                        SCH_FIELD custom( symbol.get(), FIELD_T::USER, name );
                        custom.SetOrdinal( symbol->GetNextFieldOrdinal() );
                        custom.SetTextPos( symbol->GetPosition() );
                        field = symbol->AddField( custom );
                    }
                }

                if( !field )
                    continue;

                if( override.has_value() )
                {
                    wxString value = wxString::FromUTF8( override.value() );

                    if( field->GetId() == FIELD_T::REFERENCE )
                        symbol->SetRef( &*sheetPath, value );

                    field->SetText( value );
                }

                if( override.has_visible() )
                    field->SetVisible( override.visible() );

                if( override.has_position() )
                    field->SetTextPos( kiapi::common::UnpackVector2( override.position() ) );
            }

            item = std::move( symbol );
        }
        else if( *type == SCH_LINE_T && aCreate && !container )
        {
            kiapi::schematic::types::Line request;

            if( !anyItem.UnpackTo( &request ) || !request.has_start()
                || !request.has_end() )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "line requires start and end points" );
                aItemHandler( status, anyItem );
                continue;
            }

            if( request.layer() != kiapi::schematic::types::SL_WIRE
                && request.layer() != kiapi::schematic::types::SL_BUS
                && request.layer() != kiapi::schematic::types::SL_NOTES )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "line layer must be wire, bus, or notes" );
                aItemHandler( status, anyItem );
                continue;
            }

            // 复用原理图线段序列化所用的全局枚举转换，避免把导线层误当成 protobuf 数值。
            SCH_LAYER_ID layer = FromProtoEnum<SCH_LAYER_ID,
                    kiapi::schematic::types::SchematicLayer>( request.layer() );

            item = std::make_unique<SCH_LINE>(
                    kiapi::common::UnpackVector2( request.start() ), layer );

            if( !item->Deserialize( anyItem ) )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "line endpoints must be different" );
                aItemHandler( status, anyItem );
                continue;
            }
        }
        else if( *type == SCH_JUNCTION_T && aCreate && !container )
        {
            kiapi::schematic::types::Junction request;

            if( !anyItem.UnpackTo( &request ) || !request.has_position() )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "junction requires a position" );
                aItemHandler( status, anyItem );
                continue;
            }

            auto junction = std::make_unique<SCH_JUNCTION>(
                    kiapi::common::UnpackVector2( request.position() ) );

            if( !request.id().value().empty() )
                const_cast<KIID&>( junction->m_Uuid ) = KIID( request.id().value() );

            item = std::move( junction );
        }
        else
        {
            HANDLER_RESULT<std::unique_ptr<EDA_ITEM>> creationResult =
                    createItemForType( *type, container );

            if( !creationResult )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_TYPE );
                status.set_error_message( creationResult.error().error_message() );
                aItemHandler( status, anyItem );
                continue;
            }

            item = std::move( *creationResult );

            if( !item->Deserialize( anyItem ) )
            {
                status.set_code( ItemStatusCode::ISC_INVALID_DATA );
                status.set_error_message( "could not deserialize schematic item" );
                aItemHandler( status, anyItem );
                continue;
            }
        }

        if( !item )
        {
            status.set_code( ItemStatusCode::ISC_INVALID_DATA );
            aItemHandler( status, anyItem );
            continue;
        }

        if( aCreate && itemUuidMap.count( item->m_Uuid ) )
        {
            status.set_code( ItemStatusCode::ISC_EXISTING );
            status.set_error_message( fmt::format( "an item with UUID {} already exists",
                                                   item->m_Uuid.AsStdString() ) );
            aItemHandler( status, anyItem );
            continue;
        }
        else if( !aCreate && !itemUuidMap.count( item->m_Uuid ) )
        {
            status.set_code( ItemStatusCode::ISC_NONEXISTENT );
            status.set_error_message( fmt::format( "an item with UUID {} does not exist",
                                                   item->m_Uuid.AsStdString() ) );
            aItemHandler( status, anyItem );
            continue;
        }

        status.set_code( ItemStatusCode::ISC_OK );
        google::protobuf::Any newItem;

        if( aCreate )
        {
            if( *type == SCH_SYMBOL_T && sheetPath )
                packCreatedSymbol( newItem, *static_cast<SCH_SYMBOL*>( item.get() ), *sheetPath );
            else if( *type == SCH_JUNCTION_T )
            {
                kiapi::schematic::types::Junction output;
                output.mutable_id()->set_value( item->m_Uuid.AsStdString() );
                kiapi::common::PackVector2( *output.mutable_position(),
                                             item->GetPosition() );
                newItem.PackFrom( output );
            }
            else
                item->Serialize( newItem );

            itemUuidMap[item->m_Uuid] = item.get();
            commit->Add( item.release(), screen );
        }
        else
        {
            EDA_ITEM* edaItem = itemUuidMap[item->m_Uuid];

            if( SCH_ITEM* schItem = dynamic_cast<SCH_ITEM*>( edaItem ) )
            {
                commit->Modify( schItem, screen );
                schItem->SwapItemData( static_cast<SCH_ITEM*>( item.get() ) );
                schItem->Serialize( newItem );
            }
            else
            {
                wxASSERT( false );
            }

        }

        aItemHandler( status, newItem );
    }

    // 单次请求只提交一次；显式事务则留给 EndCommit 统一提交符号和导线。
    if( !m_activeClients.count( aClientName ) )
        pushCurrentCommit( aClientName, aCreate ? _( "Added items via API" )
                                               : _( "Updated items via API" ) );


    return ItemRequestStatus::IRS_OK;
}


void API_HANDLER_SCH::deleteItemsInternal( std::map<KIID, ItemDeletionStatus>& aItemsToDelete,
                                           const std::string& aClientName )
{
    // TODO
}


std::optional<EDA_ITEM*> API_HANDLER_SCH::getItemFromDocument( const DocumentSpecifier& aDocument,
                                                               const KIID& aId )
{
    SCH_SCREEN* screen = resolveScreen( aDocument );

    if( !screen )
        return std::nullopt;

    for( EDA_ITEM* item : screen->Items() )
    {
        if( item->m_Uuid == aId )
            return item;
    }

    return std::nullopt;
}
