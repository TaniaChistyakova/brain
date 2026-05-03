cmake_minimum_required(VERSION 3.28)

project(brain_doc)

#Download UseLATEX
include(FetchContent)
FetchContent_Populate(
    UseLATEX
    GIT_REPOSITORY https://gitlab.kitware.com/kmorel/UseLATEX.git
    GIT_TAG        Version2.8.3
    SOURCE_DIR     UseLATEX
)

include("${CMAKE_CURRENT_BINARY_DIR}/UseLATEX/UseLATEX.cmake")

add_latex_document("brain.tex" BIBFILES "brain.bib" FORCE_PDF)
