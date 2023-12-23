#pragma once
#ifndef __GX_CAIRO_H__
#define __GX_CAIRO_H__

//***********************************************
// slee's macro for lib
#ifdef CAIRO_WIN32_STATIC_BUILD
	#pragma comment( lib, "cairos.lib" )	// static lib
#else
	#pragma comment( lib, "cairo.lib" )		// with cairo.dll
#endif

//***********************************************
// cairo-version.h
#ifndef CAIRO_VERSION_H
#define CAIRO_VERSION_H
#define CAIRO_VERSION_MAJOR 1
#define CAIRO_VERSION_MINOR 14
#define CAIRO_VERSION_MICRO 10
#endif

//***********************************************
// cairo-feature.h
#ifndef CAIRO_FEATURES_H
#define CAIRO_FEATURES_H
#define CAIRO_HAS_IMAGE_SURFACE 1
#define CAIRO_HAS_USER_FONT 1
#define CAIRO_HAS_SVG_SURFACE 1
#define CAIRO_HAS_PDF_SURFACE 1
#define CAIRO_HAS_PS_SURFACE 1
#endif

//***********************************************
// cairo.h
#ifdef __has_include
	#if __has_include("cairo.h")
		#include "cairo.h"
	#elif __has_include("../cairo/cairo.h")
		#include "../cairo/cairo.h"
	#elif __has_include("../../cairo/cairo.h")
		#include "../../cairo/cairo.h"
	#elif __has_include(<cairo/cairo.h>)
		#include <cairo/cairo.h>
	#elif __has_include(<cairo.h>)
		#include <cairo.h>
	#endif
#else
	#include <cairo.h>
#endif

//***********************************************
// cairo-pdf.h
#ifndef CAIRO_PDF_H
#define CAIRO_PDF_H
extern "C" {
typedef enum _cairo_pdf_version { CAIRO_PDF_VERSION_1_4, CAIRO_PDF_VERSION_1_5 } cairo_pdf_version_t;
cairo_public cairo_surface_t* cairo_pdf_surface_create(const char *filename, double width_in_points, double height_in_points);
cairo_public cairo_surface_t* cairo_pdf_surface_create_for_stream(cairo_write_func_t write_func, void *closure, double width_in_points, double height_in_points);
cairo_public void cairo_pdf_surface_restrict_to_version(cairo_surface_t *surface, cairo_pdf_version_t version);
cairo_public void cairo_pdf_get_versions(cairo_pdf_version_t const **versions, int *num_versions); 
cairo_public const char* cairo_pdf_version_to_string(cairo_pdf_version_t version);
cairo_public void cairo_pdf_surface_set_size(cairo_surface_t *surface, double width_in_points, double height_in_points);
}
#endif /* CAIRO_PDF_H */

//***********************************************
#endif // __GX_CAIRO_H__

