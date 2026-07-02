#ifndef TINYFF_RESULT_H
#define TINYFF_RESULT_H

#include <stdbool.h>

/**
 * @brief Flags used to represent binary choices 
 */
typedef enum {
    FF_TRUE = true,   ///< A true output/action
    FF_FALSE = false  ///< A false output/action
} FF_FLAG;

/**
 * @brief Master status codes for tinyff function operations. Returned by every (exported) function.
 *
 * This enumeration provides status codes to be returned from every single tinyff function
 * to show what output the operation ended up doing. Operations will return FF_RESULT_OK
 * when the function has done the explicit task it was meant to do.
 *
 * Warnings do not instantly mean that the function failed, but rather indicate that the 
 * operation completed under sub-optimal conditions, fallback behaviors, or without non-essential 
 * features enabled. In contrast, Error codes explicitly state that the operation has halted 
 * prematurely and failed to produce the intended output.
 */
typedef enum {

    /* ========================================================================= */
    /** @name Success Codes 
     *  Operational states indicating successful completion.
     */
    ///@{
    FF_RESULT_OK = 0, ///< Operation completed successfully and did not re/produce any errors.
    ///@}

    /* ========================================================================= */
    /** @name Warning Codes 
     *  Non-fatal notifications about limited functionality or disabled configurations.
     */
    ///@{
    FF_RESULT_WARN_DEBUG_DISABLED, ///< Operation succeeded, but internal debug logging/tracing features are disabled.
    FF_RESULT_WARN_NO_IMPL,         ///< The requested optional feature or extension has no implementation in this build.
    ///@}

    /* ========================================================================= */
    /** @name File System Errors 
     *  Failures encountered during high-level file operations.
     */
    ///@{
    FF_RESULT_ERROR_INVALID_FILE,      ///< The specified file handle is invalid, corrupted, or cannot be accessed.
    FF_RESULT_ERROR_READ_FILE_FAILURE, ///< A physical or software read error occurred while streaming data from the file.
    ///@}

    /* ========================================================================= */
    /** @name Generic Runtime Errors 
     *  Common execution safety violations.
     */
    ///@{
    FF_RESULT_ERROR_OUT_OF_BOUNDS, ///< An index or data pointer exceeded the legal boundaries of the allocated buffer.
    FF_RESULT_ERROR_NULL_PTR,      ///< A critical argument or internal pointer was found to be NULL where data was expected.
    ///@}

    /* ========================================================================= */
    /** @name PNG Media Errors 
     *  Failures specific to Portable Network Graphics files.
     */
    ///@{
    FF_RESULT_ERROR_INVALID_PNG_SIGNATURE, ///< The target data stream does not begin with the standard 8-byte PNG magic signature code.
    ///@}

    /* ========================================================================= */
    /** @name BMP Media Errors 
     *  Failures specific to Bitmap image files.
     */
    ///@{
    FF_RESULT_ERROR_INVALID_BMP_SIGNATURE, ///< The target data stream lacks the standard 'BM' magic signature bytes.
    FF_RESULT_ERROR_INVALID_BMP_HEADER,    ///< The internal BITMAPINFOHEADER or BITMAPFILEHEADER structures contain invalid, corrupted, or unsupported metadata dimensions.
    ///@}

    /* ========================================================================= */
    /** @name Archive Errors 
     *  Failures regarding compressed or multi-file archives.
     */
    ///@{
    FF_RESULT_ERROR_DECOMPRESSION_FAILURE, ///< The compression stream is corrupted or failed to inflate correctly during processing.
    ///@}

    /* ========================================================================= */
    /** @name Memory Allocation Errors 
     *  Failures regarding system resources.
     */
    ///@{
    FF_RESULT_ERROR_MEMORY_ALLOCATION, ///< The system heap is exhausted, and the application failed to allocate the requested working memory block.
    ///@}

} ff_result;

#endif
