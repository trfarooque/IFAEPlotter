atlas_subdir ( IFAEPlotter )


atlas_depends_on_subdirs( PUBLIC
			  IFAETopFramework
)

find_package( ROOT REQUIRED COMPONENTS RIO Core Hist Gpad Graf Tree)

atlas_add_root_dictionary( IFAEPlotter _cintDictSource
   ROOT_HEADERS IFAEPlotter/*.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( IFAEPlotter Root/*.cxx Root/*.h 
		   IFAEPlotter/*h ${_cintDictSource}
		   PUBLIC_HEADERS IFAEPlotter
		   LINK_LIBRARIES IFAETopFramework
				  ${ROOT_LIBRARIES}
		   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

atlas_install_data( share/* )
atlas_install_scripts( scripts/*py )
# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

#atlas_add_executable( makeConfigurationFiles_SUSY
#		      util/makeConfigurationFiles_SUSY.cxx
#		      LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable(  makeConfigurationFiles_TTH
		       util/makeConfigurationFiles_TTH.cxx
		       LINK_LIBRARIES IFAETopFramework
		       		      ${ROOT_LIBRARIES} )

atlas_add_executable(  makeNameTransformation
                       util/makeNameTransformation.cxx
                       LINK_LIBRARIES IFAETopFramework
                                      ${ROOT_LIBRARIES} )

atlas_add_executable(  makeSystematicsPlots
                       util/makeSystematicsPlots.cxx
                       LINK_LIBRARIES IFAETopFramework
                                      ${ROOT_LIBRARIES} )

atlas_add_executable( plot
                      util/plot.cxx
                      LINK_LIBRARIES ${ROOT_LIBRARIES} 
		      		     IFAEPlotter)
