/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   amp_png;
    const int            amp_pngSize = 15568;

    extern const char*   amp_clicked_png;
    const int            amp_clicked_pngSize = 17687;

    extern const char*   amp_hover_png;
    const int            amp_hover_pngSize = 19871;

    extern const char*   filt_png;
    const int            filt_pngSize = 13836;

    extern const char*   filt_clicked_png;
    const int            filt_clicked_pngSize = 16336;

    extern const char*   filt_hover_png;
    const int            filt_hover_pngSize = 18406;

    extern const char*   filter_png;
    const int            filter_pngSize = 4569;

    extern const char*   osc_png;
    const int            osc_pngSize = 15794;

    extern const char*   osc_clicked_png;
    const int            osc_clicked_pngSize = 18456;

    extern const char*   osc_hover_png;
    const int            osc_hover_pngSize = 20829;

    extern const char*   speaker_jpeg;
    const int            speaker_jpegSize = 11045;

    extern const char*   wave_jpeg;
    const int            wave_jpegSize = 3999;

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
