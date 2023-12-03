#include "img.hpp"

#include <fmt/core.h>

#include <system_error>

namespace img {

namespace fs = std::filesystem;

class LibRawErrorCategory: public std::error_category {
    public:
        constexpr LibRawErrorCategory() noexcept = default;
        const char* name() const noexcept override {
            return "LibRawError";
        }
        std::error_condition default_error_condition(int val) const noexcept override {
            return std::error_condition(val, *this);
        }
        std::string message(int condition) const override {
            switch (condition) {
                case LIBRAW_SUCCESS: return "No error; function terminated successfully.";
                //Regular errors
                case LIBRAW_FILE_UNSUPPORTED: return "Unsupported file format.";
                case LIBRAW_REQUEST_FOR_NONEXISTENT_IMAGE: return "Attempt to retrieve a RAW image with a number absent in the data file.";
                case LIBRAW_OUT_OF_ORDER_CALL: return "API functions have been called in wrong order";
                case LIBRAW_NO_THUMBNAIL: return "Attempt to retrieve a thumbnail from a file containing no preview.";
                case LIBRAW_UNSUPPORTED_THUMBNAIL: return "RAW file contains a preview of unsupported format.";
                case LIBRAW_INPUT_CLOSED: return "Input stream is not available for reading.";
                case LIBRAW_NOT_IMPLEMENTED: return "RAW storage/compression format is not implemented.";
                case LIBRAW_REQUEST_FOR_NONEXISTENT_THUMBNAIL: return "Attempt to retrieve a non-existent thumbnail by (invalid) index.";
                //Fatal error
                case LIBRAW_UNSUFFICIENT_MEMORY: return "Attempt to get memory from the system has failed.";
                case LIBRAW_DATA_ERROR: return "A fatal error emerged during data unpacking.";
                case LIBRAW_IO_ERROR: return "A fatal error emerged during file reading (premature end-of-file encountered or file is corrupt).";
                case LIBRAW_CANCELLED_BY_CALLBACK: return "Processing cancelled due to calling application demand.";
                case LIBRAW_BAD_CROP: return "The incorrect cropping coordinates are set via params.cropbox[]: the left-top corner of cropping rectangle is outside the image.";
                //Unknown
                case LIBRAW_UNSPECIFIED_ERROR:
                default: return "An unknown error has been encountered. This code should never be generated.";
            }
        }
};

#define PROCESS_LIBRAW_ERROR(res) \
    if ((res) != LIBRAW_SUCCESS) {                                        \
        if ((res) > LIBRAW_SUCCESS) {                                     \
            error_code = std::error_code(res, std::system_category());  \
            proc.recycle(); \
            return; \
        } \
        error_code = std::error_code(res, LibRawErrorCategory()); \
        proc.recycle(); \
        return; \
    }


void Proc::process_file(const fs::path& path, std::error_code& error_code) noexcept(true) {
    #if defined(_WIN32) || defined(WIN32)
    auto res = proc.open_file(path.native().c_str());
    #else
    auto res = proc.open_file(path.string().c_str());
    #endif

    PROCESS_LIBRAW_ERROR(res);

    fmt::println(">{}", path.string());
    if (proc.imgdata.idata.raw_count == 0) {
        fmt::println("!!No image data found");
        return;
    }

    fmt::println(
        "Dimensions: {}x{}\n"
        "Camera model: {}\n"
        "Color format: {}\n"
        "Description: {}\n"
        "Author: {}\n"
        "GPS (DMS): {}°{}'{:.1f}\"{} {}°{}'{:.1f}\"{}\n",
        proc.imgdata.sizes.width, proc.imgdata.sizes.height,
        proc.imgdata.idata.model,
        proc.imgdata.idata.cdesc,
        proc.imgdata.other.desc,
        proc.imgdata.other.artist,
        //latitude
        proc.imgdata.other.parsed_gps.latitude[0],
        proc.imgdata.other.parsed_gps.latitude[1],
        proc.imgdata.other.parsed_gps.latitude[2],
        proc.imgdata.other.parsed_gps.latref,
        //longitude
        proc.imgdata.other.parsed_gps.longitude[0],
        proc.imgdata.other.parsed_gps.longitude[1],
        proc.imgdata.other.parsed_gps.longitude[2],
        proc.imgdata.other.parsed_gps.longref
    );

    res = proc.unpack();
    PROCESS_LIBRAW_ERROR(res);

    res = proc.raw2image();
    //TODO
    //Consider to use https://github.com/CarVac/librtprocess for RAW conversion to RGB

    proc.recycle();
}

} //img
