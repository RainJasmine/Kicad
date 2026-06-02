
function( sign_kicad_bundle target signing_id use_secure_timestamp use_hardened_runtime entitlements_file use_sentry)

    # If the signing ID wasn't passed in, use - which means adhoc signing
    if ( NOT signing_id )
        set( signing_id "-")
    endif()

    MESSAGE( STATUS "Signing ${target} with ${signing_id}, hardened runtime: ${use_hardened_runtime}, secure timestamp: ${use_secure_timestamp}, entitlements file: ${entitlements_file}, sentry: ${use_sentry}" )

    # --deep doesn't really work and is officially deprecated as of macos 13
    # https://developer.apple.com/library/archive/technotes/tn2206/_index.html#//apple_ref/doc/uid/DTS40007919-CH1-TNTAG201

    # collect a list of things to sign, in order
    set( sign_list "${target}/Contents/Applications/eeschema.app/Contents/MacOS/eeschema"
            "${target}/Contents/Applications/eeschema.app"
            "${target}/Contents/Applications/gerbview.app/Contents/MacOS/gerbview"
            "${target}/Contents/Applications/gerbview.app"  "${target}/Contents/Applications/pcbnew.app/Contents/MacOS/pcbnew" "${target}/Contents/Applications/pcbnew.app" "${target}/Contents/Applications/bitmap2component.app/Contents/MacOS/bitmap2component" "${target}/Contents/Applications/bitmap2component.app" "${target}/Contents/Applications/pcb_calculator.app/Contents/MacOS/pcb_calculator" "${target}/Contents/Applications/pcb_calculator.app" "${target}/Contents/Applications/pl_editor.app/Contents/MacOS/pl_editor" "${target}/Contents/Applications/pl_editor.app")

    # add all the dylibs from contents/frameworks
    file( GLOB framework_dylibs "${target}/Contents/Frameworks/*.dylib" )

    # add all the files in Contents/PlugIns
    file( GLOB_RECURSE plugins "${target}/Contents/PlugIns/*" )

    file( GLOB_RECURSE translations "${target}/Contents/SharedSupport/internat/*.mo" )

    # add all the files in Contents/MacOS/
    # But we've gotta sign kicad-cli before signing kicad, at least on x86_64
    set( kicad_bins "${target}/Contents/MacOS/dxf2idf"
            "${target}/Contents/MacOS/idf2vrml"
            "${target}/Contents/MacOS/idfcyl"
            "${target}/Contents/MacOS/idfrect"
            "${target}/Contents/MacOS/kicad-cli"
            "${target}/Contents/MacOS/kicad")

    if( use_sentry )
        set( sign_list ${sign_list} "${target}/Contents/MacOS/crashpad_handler" )
    endif()

    set( sign_list ${sign_list} ${framework_dylibs} ${plugins} ${translations} ${kicad_bins} ) # do i need to quote this differently?

    # add kicad.app!
    set( sign_list ${sign_list} "${target}" )

    # build the command used for signing
    set( command codesign --force --sign "${signing_id}" )

    if( use_secure_timestamp )
        set( command ${command} --timestamp )
    endif( )

    if( use_hardened_runtime )
        if ( signing_id STREQUAL "-" )
            message( FATAL_ERROR "Hardened runtime requires a (non-ad-hoc) signing identity." )
        endif( )

        set( command ${command} --options runtime )
    endif( )

    if( entitlements_file )
        set( command ${command} --entitlements "${entitlements_file}" )
    endif( )

    foreach( item ${sign_list} )
        set( cmd ${command} "${item}" )

        # MESSAGE( STATUS "Running ${cmd}")
        execute_process( COMMAND ${cmd}
                RESULT_VARIABLE codesign_result)

        if( NOT codesign_result EQUAL 0 )
            message( WARNING "macOS signing failed; ${cmd} returned ${codesign_result}" )
        endif( )
    endforeach( )
endfunction()


function( verify_signing target )
    set( cmd codesign --verify --deep --strict --verbose=3 "${target}" )

    execute_process( COMMAND ${cmd} RESULT_VARIABLE verify_result )
    if( NOT verify_result EQUAL 0 )
        message( FATAL_ERROR "macOS signing verification failed; ran ${cmd}" )
    endif( )
endfunction( )
