/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   arial_narrow_7_ttf;
    const int            arial_narrow_7_ttfSize = 73996;

    extern const char*   baselayout_png;
    const int            baselayout_pngSize = 1577993;

    extern const char*   chorusoff_png;
    const int            chorusoff_pngSize = 7552;

    extern const char*   choruson_png;
    const int            choruson_pngSize = 8126;

    extern const char*   game_over_ttf;
    const int            game_over_ttfSize = 19164;

    extern const char*   HackRegular_ttf;
    const int            HackRegular_ttfSize = 383104;

    extern const char*   linkoff_png;
    const int            linkoff_pngSize = 6747;

    extern const char*   linkon_png;
    const int            linkon_pngSize = 6794;

    extern const char*   screens_png;
    const int            screens_pngSize = 91704;

    extern const char*   sliderbtn_png;
    const int            sliderbtn_pngSize = 4448;

    extern const char*   switch_png;
    const int            switch_pngSize = 5963;

    extern const char*   timeselect_png;
    const int            timeselect_pngSize = 10462;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 12;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
