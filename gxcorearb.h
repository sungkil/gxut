#ifndef __gxcorearb_h_
#define __gxcorearb_h_ 1

#if defined(__gl_h_) || defined(__GL_H__)
	#error do not include <gl.h> together with <gxcorearb.h>
#else
	#define __gl_h_
	#define __GL_H__
#endif

#if defined(__glu_h__) || defined(__GLU_H__)
	#error do not include <glu.h> for core profile
#else
	#define __glu_h__
	#define __GLU_H__
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

#ifndef GL_VERSION_1_0
#define GL_VERSION_1_0 1
typedef void GLvoid;
typedef unsigned int GLenum;

/* #include <KHR/khrplatform.h> */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__GNUC__) || defined(__SCO__) || defined(__USLC__)
#include <stdint.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#if defined(__SIZEOF_LONG__) && defined(__SIZEOF_POINTER__)
#if __SIZEOF_POINTER__ > __SIZEOF_LONG__
#define KHRONOS_USE_INTPTR_T
#endif
#endif
#elif defined(_WIN32) && !defined(__SCITECH_SNAP__)
typedef __int32					khronos_int32_t;
typedef unsigned __int32		khronos_uint32_t;
typedef __int64					khronos_int64_t;
typedef unsigned __int64		khronos_uint64_t;
#endif
typedef signed   char			khronos_int8_t;
typedef unsigned char			khronos_uint8_t;
typedef signed   short int		khronos_int16_t;
typedef unsigned short int		khronos_uint16_t;
typedef          float			khronos_float_t;
typedef khronos_uint64_t		khronos_utime_nanoseconds_t;
typedef khronos_int64_t			khronos_stime_nanoseconds_t;
#ifdef _M_X64
typedef signed   long long int	khronos_intptr_t;
typedef unsigned long long int	khronos_uintptr_t;
typedef signed   long long int	khronos_ssize_t;
typedef unsigned long long int	khronos_usize_t;
#else
typedef signed   long int		khronos_intptr_t;
typedef unsigned long int		khronos_uintptr_t;
typedef signed   long int		khronos_ssize_t;
typedef unsigned long int		khronos_usize_t;
#endif
#ifndef KHRONOS_MAX_ENUM
#define KHRONOS_MAX_ENUM 0x7FFFFFFF
#endif
typedef enum {
	KHRONOS_FALSE = 0,
	KHRONOS_TRUE  = 1,
	KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = KHRONOS_MAX_ENUM
} khronos_boolean_enum_t;
/* end #include <KHR/khrplatform.h> */

typedef khronos_float_t GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef double GLdouble;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef khronos_uint8_t GLubyte;
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_OUT_OF_MEMORY                  0x0505
#define GL_CW                             0x0900
#define GL_CCW                            0x0901
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_VIEWPORT                       0x0BA2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F
#define GL_TEXTURE                        0x1702
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
typedef void (APIENTRYP PFNGLCULLFACEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLFRONTFACEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLHINTPROC) (GLenum target, GLenum mode);
typedef void (APIENTRYP PFNGLLINEWIDTHPROC) (GLfloat width);
typedef void (APIENTRYP PFNGLPOINTSIZEPROC) (GLfloat size);
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC) (GLenum face, GLenum mode);
typedef void (APIENTRYP PFNGLSCISSORPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC) (GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLDRAWBUFFERPROC) (GLenum buf);
typedef void (APIENTRYP PFNGLCLEARPROC) (GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC) (GLint s);
typedef void (APIENTRYP PFNGLCLEARDEPTHPROC) (GLdouble depth);
typedef void (APIENTRYP PFNGLSTENCILMASKPROC) (GLuint mask);
typedef void (APIENTRYP PFNGLCOLORMASKPROC) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC) (GLboolean flag);
typedef void (APIENTRYP PFNGLDISABLEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLENABLEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLFINISHPROC) (void);
typedef void (APIENTRYP PFNGLFLUSHPROC) (void);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC) (GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLLOGICOPPROC) (GLenum opcode);
typedef void (APIENTRYP PFNGLSTENCILFUNCPROC) (GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLSTENCILOPPROC) (GLenum fail, GLenum zfail, GLenum zpass);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC) (GLenum func);
typedef void (APIENTRYP PFNGLPIXELSTOREFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLREADBUFFERPROC) (GLenum src);
typedef void (APIENTRYP PFNGLREADPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLGETBOOLEANVPROC) (GLenum pname, GLboolean *data);
typedef void (APIENTRYP PFNGLGETDOUBLEVPROC) (GLenum pname, GLdouble *data);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC) (void);
typedef void (APIENTRYP PFNGLGETFLOATVPROC) (GLenum pname, GLfloat *data);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC) (GLenum pname, GLint *data);
typedef const GLubyte *(APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC) (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC) (GLenum target, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC) (GLenum target, GLint level, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLDEPTHRANGEPROC) (GLdouble n, GLdouble f);
typedef void (APIENTRYP PFNGLVIEWPORTPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
#endif /* GL_VERSION_1_0 */

#ifndef GL_VERSION_1_1
#define GL_VERSION_1_1 1
typedef khronos_float_t GLclampf;
typedef double GLclampd;
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_DOUBLE                         0x140A
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_VERTEX_ARRAY                   0x8074
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLGETPOINTERVPROC) (GLenum pname, void **params);
typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC) (GLfloat factor, GLfloat units);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC) (GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC) (GLsizei n, const GLuint *textures);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC) (GLsizei n, GLuint *textures);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC) (GLuint texture);
#endif /* GL_VERSION_1_1 */

#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_TEXTURE_BINDING_3D             0x806A
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY  0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY  0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
#endif /* GL_VERSION_1_2 */

#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3 1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
#define GL_COMPRESSED_RGB                 0x84ED
#define GL_COMPRESSED_RGBA                0x84EE
#define GL_TEXTURE_COMPRESSION_HINT       0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
#define GL_CLAMP_TO_BORDER                0x812D
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC) (GLfloat value, GLboolean invert);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint level, void *img);
#endif /* GL_VERSION_1_3 */

#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE      0x8128
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_MIRRORED_REPEAT                0x8370
#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_INCR_WRAP                      0x8507
#define GL_DECR_WRAP                      0x8508
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_BLEND_COLOR                    0x8005
#define GL_BLEND_EQUATION                 0x8009
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC) (GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC) (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLBLENDCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC) (GLenum mode);
#endif /* GL_VERSION_1_4 */

#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5 1
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
#define GL_BUFFER_SIZE                    0x8764
#define GL_BUFFER_USAGE                   0x8765
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_SAMPLES_PASSED                 0x8914
#define GL_SRC1_ALPHA                     0x8589
typedef void (APIENTRYP PFNGLGENQUERIESPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRYP PFNGLISQUERYPROC) (GLuint id);
typedef void (APIENTRYP PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLENDQUERYPROC) (GLenum target);
typedef void (APIENTRYP PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC) (GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef GLboolean (APIENTRYP PFNGLISBUFFERPROC) (GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, void *data);
typedef void *(APIENTRYP PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC) (GLenum target);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVPROC) (GLenum target, GLenum pname, void **params);
#endif /* GL_VERSION_1_5 */

#ifndef GL_VERSION_2_0
#define GL_VERSION_2_0 1
typedef char GLchar;
typedef khronos_int16_t GLshort;
typedef khronos_int8_t GLbyte;
typedef khronos_uint16_t GLushort;
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED    0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE       0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE     0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE       0x8625
#define GL_CURRENT_VERTEX_ATTRIB          0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER    0x8645
#define GL_STENCIL_BACK_FUNC              0x8800
#define GL_STENCIL_BACK_FAIL              0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL   0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS   0x8803
#define GL_MAX_DRAW_BUFFERS               0x8824
#define GL_DRAW_BUFFER0                   0x8825
#define GL_DRAW_BUFFER1                   0x8826
#define GL_DRAW_BUFFER2                   0x8827
#define GL_DRAW_BUFFER3                   0x8828
#define GL_DRAW_BUFFER4                   0x8829
#define GL_DRAW_BUFFER5                   0x882A
#define GL_DRAW_BUFFER6                   0x882B
#define GL_DRAW_BUFFER7                   0x882C
#define GL_DRAW_BUFFER8                   0x882D
#define GL_DRAW_BUFFER9                   0x882E
#define GL_DRAW_BUFFER10                  0x882F
#define GL_DRAW_BUFFER11                  0x8830
#define GL_DRAW_BUFFER12                  0x8831
#define GL_DRAW_BUFFER13                  0x8832
#define GL_DRAW_BUFFER14                  0x8833
#define GL_DRAW_BUFFER15                  0x8834
#define GL_BLEND_EQUATION_ALPHA           0x883D
#define GL_MAX_VERTEX_ATTRIBS             0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS        0x8872
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS  0x8B4A
#define GL_MAX_VARYING_FLOATS             0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE                    0x8B4F
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_BOOL                           0x8B56
#define GL_BOOL_VEC2                      0x8B57
#define GL_BOOL_VEC3                      0x8B58
#define GL_BOOL_VEC4                      0x8B59
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_SAMPLER_1D                     0x8B5D
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60
#define GL_SAMPLER_1D_SHADOW              0x8B61
#define GL_SAMPLER_2D_SHADOW              0x8B62
#define GL_DELETE_STATUS                  0x8B80
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ATTACHED_SHADERS               0x8B85
#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87
#define GL_SHADER_SOURCE_LENGTH           0x8B88
#define GL_ACTIVE_ATTRIBUTES              0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN      0x8CA0
#define GL_LOWER_LEFT                     0x8CA1
#define GL_UPPER_LEFT                     0x8CA2
#define GL_STENCIL_BACK_REF               0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK        0x8CA4
#define GL_STENCIL_BACK_WRITEMASK         0x8CA5
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC) (GLenum face, GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC) (GLenum face, GLuint mask);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat *params);
typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC) (GLuint program, GLint location, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC) (GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC) (GLuint index, GLenum pname, void **pointer);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPROC) (GLuint program);
typedef GLboolean (APIENTRYP PFNGLISSHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLUNIFORM3IPROC) (GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLUNIFORM4IPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DPROC) (GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC) (GLuint index, GLfloat x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SPROC) (GLuint index, GLshort x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SPROC) (GLuint index, GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
#endif /* GL_VERSION_2_0 */

#ifndef GL_VERSION_2_1
#define GL_VERSION_2_1 1
#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_SRGB                           0x8C40
#define GL_SRGB8                          0x8C41
#define GL_SRGB_ALPHA                     0x8C42
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_COMPRESSED_SRGB                0x8C48
#define GL_COMPRESSED_SRGB_ALPHA          0x8C49
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
#endif /* GL_VERSION_2_1 */

#ifndef GL_VERSION_3_0
#define GL_VERSION_3_0 1
typedef khronos_uint16_t GLhalf;
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_CLIP_DISTANCE0                 0x3000
#define GL_CLIP_DISTANCE1                 0x3001
#define GL_CLIP_DISTANCE2                 0x3002
#define GL_CLIP_DISTANCE3                 0x3003
#define GL_CLIP_DISTANCE4                 0x3004
#define GL_CLIP_DISTANCE5                 0x3005
#define GL_CLIP_DISTANCE6                 0x3006
#define GL_CLIP_DISTANCE7                 0x3007
#define GL_MAX_CLIP_DISTANCES             0x0D32
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_NUM_EXTENSIONS                 0x821D
#define GL_CONTEXT_FLAGS                  0x821E
#define GL_COMPRESSED_RED                 0x8225
#define GL_COMPRESSED_RG                  0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER    0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS       0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET       0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET       0x8905
#define GL_CLAMP_READ_COLOR               0x891C
#define GL_FIXED_ONLY                     0x891D
#define GL_MAX_VARYING_COMPONENTS         0x8B4B
#define GL_TEXTURE_1D_ARRAY               0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY         0x8C19
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY         0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY       0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY       0x8C1D
#define GL_R11F_G11F_B10F                 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV   0x8C3B
#define GL_RGB9_E5                        0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV       0x8C3E
#define GL_TEXTURE_SHARED_SIZE            0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS    0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED           0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD             0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS            0x8C8C
#define GL_SEPARATE_ATTRIBS               0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER      0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI                       0x8D70
#define GL_RGB32UI                        0x8D71
#define GL_RGBA16UI                       0x8D76
#define GL_RGB16UI                        0x8D77
#define GL_RGBA8UI                        0x8D7C
#define GL_RGB8UI                         0x8D7D
#define GL_RGBA32I                        0x8D82
#define GL_RGB32I                         0x8D83
#define GL_RGBA16I                        0x8D88
#define GL_RGB16I                         0x8D89
#define GL_RGBA8I                         0x8D8E
#define GL_RGB8I                          0x8D8F
#define GL_RED_INTEGER                    0x8D94
#define GL_GREEN_INTEGER                  0x8D95
#define GL_BLUE_INTEGER                   0x8D96
#define GL_RGB_INTEGER                    0x8D98
#define GL_RGBA_INTEGER                   0x8D99
#define GL_BGR_INTEGER                    0x8D9A
#define GL_BGRA_INTEGER                   0x8D9B
#define GL_SAMPLER_1D_ARRAY               0x8DC0
#define GL_SAMPLER_2D_ARRAY               0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW        0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW        0x8DC4
#define GL_SAMPLER_CUBE_SHADOW            0x8DC5
#define GL_UNSIGNED_INT_VEC2              0x8DC6
#define GL_UNSIGNED_INT_VEC3              0x8DC7
#define GL_UNSIGNED_INT_VEC4              0x8DC8
#define GL_INT_SAMPLER_1D                 0x8DC9
#define GL_INT_SAMPLER_2D                 0x8DCA
#define GL_INT_SAMPLER_3D                 0x8DCB
#define GL_INT_SAMPLER_CUBE               0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY           0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY           0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D        0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D        0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D        0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE      0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY  0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY  0x8DD7
#define GL_QUERY_WAIT                     0x8E13
#define GL_QUERY_NO_WAIT                  0x8E14
#define GL_QUERY_BY_REGION_WAIT           0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT        0x8E16
#define GL_BUFFER_ACCESS_FLAGS            0x911F
#define GL_BUFFER_MAP_LENGTH              0x9120
#define GL_BUFFER_MAP_OFFSET              0x9121
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_DEPTH32F_STENCIL8              0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT            0x8218
#define GL_FRAMEBUFFER_UNDEFINED          0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_MAX_RENDERBUFFER_SIZE          0x84E8
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_TEXTURE_STENCIL_SIZE           0x88F1
#define GL_TEXTURE_RED_TYPE               0x8C10
#define GL_TEXTURE_GREEN_TYPE             0x8C11
#define GL_TEXTURE_BLUE_TYPE              0x8C12
#define GL_TEXTURE_ALPHA_TYPE             0x8C13
#define GL_TEXTURE_DEPTH_TYPE             0x8C16
#define GL_UNSIGNED_NORMALIZED            0x8C17
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING       0x8CA6
#define GL_RENDERBUFFER_BINDING           0x8CA7
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING       0x8CAA
#define GL_RENDERBUFFER_SAMPLES           0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_RENDERBUFFER_WIDTH             0x8D42
#define GL_RENDERBUFFER_HEIGHT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT   0x8D44
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49
#define GL_RENDERBUFFER_RED_SIZE          0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE        0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE         0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE        0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE        0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE      0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES                    0x8D57
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_HALF_FLOAT                     0x140B
#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020
#define GL_COMPRESSED_RED_RGTC1           0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1    0x8DBC
#define GL_COMPRESSED_RG_RGTC2            0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2     0x8DBE
#define GL_RG                             0x8227
#define GL_RG_INTEGER                     0x8228
#define GL_R8                             0x8229
#define GL_R16                            0x822A
#define GL_RG8                            0x822B
#define GL_RG16                           0x822C
#define GL_R16F                           0x822D
#define GL_R32F                           0x822E
#define GL_RG16F                          0x822F
#define GL_RG32F                          0x8230
#define GL_R8I                            0x8231
#define GL_R8UI                           0x8232
#define GL_R16I                           0x8233
#define GL_R16UI                          0x8234
#define GL_R32I                           0x8235
#define GL_R32UI                          0x8236
#define GL_RG8I                           0x8237
#define GL_RG8UI                          0x8238
#define GL_RG16I                          0x8239
#define GL_RG16UI                         0x823A
#define GL_RG32I                          0x823B
#define GL_RG32UI                         0x823C
#define GL_VERTEX_ARRAY_BINDING           0x85B5
typedef void (APIENTRYP PFNGLCOLORMASKIPROC) (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC) (GLenum target, GLuint index, GLboolean *data);
typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC) (GLenum target, GLuint index, GLint *data);
typedef void (APIENTRYP PFNGLENABLEIPROC) (GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLDISABLEIPROC) (GLenum target, GLuint index);
typedef GLboolean (APIENTRYP PFNGLISENABLEDIPROC) (GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC) (GLenum primitiveMode);
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC) (void);
typedef void (APIENTRYP PFNGLBINDBUFFERRANGEPROC) (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC) (GLenum target, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC) (GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLCLAMPCOLORPROC) (GLenum target, GLenum clamp);
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERPROC) (GLuint id, GLenum mode);
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERPROC) (void);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC) (GLuint index, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IPROC) (GLuint index, GLint x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IPROC) (GLuint index, GLint x, GLint y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IPROC) (GLuint index, GLint x, GLint y, GLint z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IPROC) (GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIPROC) (GLuint index, GLuint x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIPROC) (GLuint index, GLuint x, GLuint y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLGETUNIFORMUIVPROC) (GLuint program, GLint location, GLuint *params);
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONPROC) (GLuint program, GLuint color, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETFRAGDATALOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORM1UIPROC) (GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLUNIFORM2UIPROC) (GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLUNIFORM3UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLUNIFORM4UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLUNIFORM1UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORM2UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORM3UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORM4UIVPROC) (GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLCLEARBUFFERIVPROC) (GLenum buffer, GLint drawbuffer, const GLint *value);
typedef void (APIENTRYP PFNGLCLEARBUFFERUIVPROC) (GLenum buffer, GLint drawbuffer, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLCLEARBUFFERFIPROC) (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef const GLubyte *(APIENTRYP PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFERPROC) (GLuint renderbuffer);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC) (GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFERPROC) (GLuint framebuffer);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC) (GLenum target);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC) (GLenum target);
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void *(APIENTRYP PFNGLMAPBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYPROC) (GLuint array);
#endif /* GL_VERSION_3_0 */

#ifndef GL_VERSION_3_1
#define GL_VERSION_3_1 1
#define GL_SAMPLER_2D_RECT                0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW         0x8B64
#define GL_SAMPLER_BUFFER                 0x8DC2
#define GL_INT_SAMPLER_2D_RECT            0x8DCD
#define GL_INT_SAMPLER_BUFFER             0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT   0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER    0x8DD8
#define GL_TEXTURE_BUFFER                 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE        0x8C2B
#define GL_TEXTURE_BINDING_BUFFER         0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE              0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE      0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE        0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE     0x84F8
#define GL_R8_SNORM                       0x8F94
#define GL_RG8_SNORM                      0x8F95
#define GL_RGB8_SNORM                     0x8F96
#define GL_RGBA8_SNORM                    0x8F97
#define GL_R16_SNORM                      0x8F98
#define GL_RG16_SNORM                     0x8F99
#define GL_RGB16_SNORM                    0x8F9A
#define GL_RGBA16_SNORM                   0x8F9B
#define GL_SIGNED_NORMALIZED              0x8F9C
#define GL_PRIMITIVE_RESTART              0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX        0x8F9E
#define GL_COPY_READ_BUFFER               0x8F36
#define GL_COPY_WRITE_BUFFER              0x8F37
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_UNIFORM_BUFFER_BINDING         0x8A28
#define GL_UNIFORM_BUFFER_START           0x8A29
#define GL_UNIFORM_BUFFER_SIZE            0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS      0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS    0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS    0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS    0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS    0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE         0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS          0x8A36
#define GL_UNIFORM_TYPE                   0x8A37
#define GL_UNIFORM_SIZE                   0x8A38
#define GL_UNIFORM_NAME_LENGTH            0x8A39
#define GL_UNIFORM_BLOCK_INDEX            0x8A3A
#define GL_UNIFORM_OFFSET                 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE           0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE          0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR           0x8A3E
#define GL_UNIFORM_BLOCK_BINDING          0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE        0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH      0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS  0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX                  0xFFFFFFFFu
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC) (GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
typedef void (APIENTRYP PFNGLTEXBUFFERPROC) (GLenum target, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXPROC) (GLuint index);
typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC) (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETUNIFORMINDICESPROC) (GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC) (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC) (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
typedef GLuint (APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC) (GLuint program, const GLchar *uniformBlockName);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC) (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC) (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
#endif /* GL_VERSION_3_1 */

#ifndef GL_VERSION_3_2
#define GL_VERSION_3_2 1
typedef struct __GLsync *GLsync;
typedef khronos_uint64_t GLuint64;
typedef khronos_int64_t GLint64;
#define GL_CONTEXT_CORE_PROFILE_BIT       0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY                0x000A
#define GL_LINE_STRIP_ADJACENCY           0x000B
#define GL_TRIANGLES_ADJACENCY            0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY       0x000D
#define GL_PROGRAM_POINT_SIZE             0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_GEOMETRY_VERTICES_OUT          0x8916
#define GL_GEOMETRY_INPUT_TYPE            0x8917
#define GL_GEOMETRY_OUTPUT_TYPE           0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES   0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS   0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS  0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS  0x9125
#define GL_CONTEXT_PROFILE_MASK           0x9126
#define GL_DEPTH_CLAMP                    0x864F
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION        0x8E4D
#define GL_LAST_VERTEX_CONVENTION         0x8E4E
#define GL_PROVOKING_VERTEX               0x8E4F
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F
#define GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
#define GL_OBJECT_TYPE                    0x9112
#define GL_SYNC_CONDITION                 0x9113
#define GL_SYNC_STATUS                    0x9114
#define GL_SYNC_FLAGS                     0x9115
#define GL_SYNC_FENCE                     0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE     0x9117
#define GL_UNSIGNALED                     0x9118
#define GL_SIGNALED                       0x9119
#define GL_ALREADY_SIGNALED               0x911A
#define GL_TIMEOUT_EXPIRED                0x911B
#define GL_CONDITION_SATISFIED            0x911C
#define GL_WAIT_FAILED                    0x911D
#define GL_TIMEOUT_IGNORED                0xFFFFFFFFFFFFFFFFull
#define GL_SYNC_FLUSH_COMMANDS_BIT        0x00000001
#define GL_SAMPLE_POSITION                0x8E50
#define GL_SAMPLE_MASK                    0x8E51
#define GL_SAMPLE_MASK_VALUE              0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS          0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE   0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY   0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES                0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE         0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE     0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY   0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
#define GL_MAX_INTEGER_SAMPLES            0x9110
typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC) (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
typedef void (APIENTRYP PFNGLPROVOKINGVERTEXPROC) (GLenum mode);
typedef GLsync (APIENTRYP PFNGLFENCESYNCPROC) (GLenum condition, GLbitfield flags);
typedef GLboolean (APIENTRYP PFNGLISSYNCPROC) (GLsync sync);
typedef void (APIENTRYP PFNGLDELETESYNCPROC) (GLsync sync);
typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLGETINTEGER64VPROC) (GLenum pname, GLint64 *data);
typedef void (APIENTRYP PFNGLGETSYNCIVPROC) (GLsync sync, GLenum pname, GLsizei count, GLsizei *length, GLint *values);
typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC) (GLenum target, GLuint index, GLint64 *data);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC) (GLenum target, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC) (GLenum pname, GLuint index, GLfloat *val);
typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC) (GLuint maskNumber, GLbitfield mask);
#endif /* GL_VERSION_3_2 */

#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3 1
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR    0x88FE
#define GL_SRC1_COLOR                     0x88F9
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC
#define GL_ANY_SAMPLES_PASSED             0x8C2F
#define GL_SAMPLER_BINDING                0x8919
#define GL_RGB10_A2UI                     0x906F
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#define GL_TIME_ELAPSED                   0x88BF
#define GL_TIMESTAMP                      0x8E28
#define GL_INT_2_10_10_10_REV             0x8D9F
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONINDEXEDPROC) (GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETFRAGDATAINDEXPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGENSAMPLERSPROC) (GLsizei count, GLuint *samplers);
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC) (GLsizei count, const GLuint *samplers);
typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC) (GLuint sampler);
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC) (GLuint unit, GLuint sampler);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC) (GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC) (GLuint sampler, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, const GLuint *param);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLQUERYCOUNTERPROC) (GLuint id, GLenum target);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VPROC) (GLuint id, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC) (GLuint id, GLenum pname, GLuint64 *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC) (GLuint index, GLuint divisor);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
#endif /* GL_VERSION_3_3 */

#ifndef GL_VERSION_4_0
#define GL_VERSION_4_0 1
#define GL_SAMPLE_SHADING                 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE       0x8C37
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY   0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY         0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW  0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY     0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING   0x8F43
#define GL_GEOMETRY_SHADER_INVOCATIONS    0x887F
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
#define GL_MAX_VERTEX_STREAMS             0x8E71
#define GL_DOUBLE_VEC2                    0x8FFC
#define GL_DOUBLE_VEC3                    0x8FFD
#define GL_DOUBLE_VEC4                    0x8FFE
#define GL_DOUBLE_MAT2                    0x8F46
#define GL_DOUBLE_MAT3                    0x8F47
#define GL_DOUBLE_MAT4                    0x8F48
#define GL_DOUBLE_MAT2x3                  0x8F49
#define GL_DOUBLE_MAT2x4                  0x8F4A
#define GL_DOUBLE_MAT3x2                  0x8F4B
#define GL_DOUBLE_MAT3x4                  0x8F4C
#define GL_DOUBLE_MAT4x2                  0x8F4D
#define GL_DOUBLE_MAT4x3                  0x8F4E
#define GL_ACTIVE_SUBROUTINES             0x8DE5
#define GL_ACTIVE_SUBROUTINE_UNIFORMS     0x8DE6
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH   0x8E48
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_MAX_SUBROUTINES                0x8DE7
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_NUM_COMPATIBLE_SUBROUTINES     0x8E4A
#define GL_COMPATIBLE_SUBROUTINES         0x8E4B
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_PATCH_DEFAULT_INNER_LEVEL      0x8E73
#define GL_PATCH_DEFAULT_OUTER_LEVEL      0x8E74
#define GL_TESS_CONTROL_OUTPUT_VERTICES   0x8E75
#define GL_TESS_GEN_MODE                  0x8E76
#define GL_TESS_GEN_SPACING               0x8E77
#define GL_TESS_GEN_VERTEX_ORDER          0x8E78
#define GL_TESS_GEN_POINT_MODE            0x8E79
#define GL_ISOLINES                       0x8E7A
#define GL_FRACTIONAL_ODD                 0x8E7B
#define GL_FRACTIONAL_EVEN                0x8E7C
#define GL_MAX_PATCH_VERTICES             0x8E7D
#define GL_MAX_TESS_GEN_LEVEL             0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS      0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED 0x8E23
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_BINDING     0x8E25
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
typedef void (APIENTRYP PFNGLMINSAMPLESHADINGPROC) (GLfloat value);
typedef void (APIENTRYP PFNGLBLENDEQUATIONIPROC) (GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC) (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLBLENDFUNCIPROC) (GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC) (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (APIENTRYP PFNGLDRAWARRAYSINDIRECTPROC) (GLenum mode, const void *indirect);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINDIRECTPROC) (GLenum mode, GLenum type, const void *indirect);
typedef void (APIENTRYP PFNGLUNIFORM1DPROC) (GLint location, GLdouble x);
typedef void (APIENTRYP PFNGLUNIFORM2DPROC) (GLint location, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLUNIFORM3DPROC) (GLint location, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLUNIFORM4DPROC) (GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLUNIFORM1DVPROC) (GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORM2DVPROC) (GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORM3DVPROC) (GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORM4DVPROC) (GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3DVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETUNIFORMDVPROC) (GLuint program, GLint location, GLdouble *params);
typedef GLint (APIENTRYP PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC) (GLuint program, GLenum shadertype, const GLchar *name);
typedef GLuint (APIENTRYP PFNGLGETSUBROUTINEINDEXPROC) (GLuint program, GLenum shadertype, const GLchar *name);
typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC) (GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC) (GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINENAMEPROC) (GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORMSUBROUTINESUIVPROC) (GLenum shadertype, GLsizei count, const GLuint *indices);
typedef void (APIENTRYP PFNGLGETUNIFORMSUBROUTINEUIVPROC) (GLenum shadertype, GLint location, GLuint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMSTAGEIVPROC) (GLuint program, GLenum shadertype, GLenum pname, GLint *values);
typedef void (APIENTRYP PFNGLPATCHPARAMETERIPROC) (GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLPATCHPARAMETERFVPROC) (GLenum pname, const GLfloat *values);
typedef void (APIENTRYP PFNGLBINDTRANSFORMFEEDBACKPROC) (GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSPROC) (GLsizei n, const GLuint *ids);
typedef void (APIENTRYP PFNGLGENTRANSFORMFEEDBACKSPROC) (GLsizei n, GLuint *ids);
typedef GLboolean (APIENTRYP PFNGLISTRANSFORMFEEDBACKPROC) (GLuint id);
typedef void (APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKPROC) (void);
typedef void (APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKPROC) (void);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKPROC) (GLenum mode, GLuint id);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC) (GLenum mode, GLuint id, GLuint stream);
typedef void (APIENTRYP PFNGLBEGINQUERYINDEXEDPROC) (GLenum target, GLuint index, GLuint id);
typedef void (APIENTRYP PFNGLENDQUERYINDEXEDPROC) (GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLGETQUERYINDEXEDIVPROC) (GLenum target, GLuint index, GLenum pname, GLint *params);
#endif /* GL_VERSION_4_0 */

#ifndef GL_VERSION_4_1
#define GL_VERSION_4_1 1
#define GL_FIXED                          0x140C
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_LOW_FLOAT                      0x8DF0
#define GL_MEDIUM_FLOAT                   0x8DF1
#define GL_HIGH_FLOAT                     0x8DF2
#define GL_LOW_INT                        0x8DF3
#define GL_MEDIUM_INT                     0x8DF4
#define GL_HIGH_INT                       0x8DF5
#define GL_SHADER_COMPILER                0x8DFA
#define GL_SHADER_BINARY_FORMATS          0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS      0x8DF9
#define GL_MAX_VERTEX_UNIFORM_VECTORS     0x8DFB
#define GL_MAX_VARYING_VECTORS            0x8DFC
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS   0x8DFD
#define GL_RGB565                         0x8D62
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_PROGRAM_BINARY_LENGTH          0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS     0x87FE
#define GL_PROGRAM_BINARY_FORMATS         0x87FF
#define GL_VERTEX_SHADER_BIT              0x00000001
#define GL_FRAGMENT_SHADER_BIT            0x00000002
#define GL_GEOMETRY_SHADER_BIT            0x00000004
#define GL_TESS_CONTROL_SHADER_BIT        0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT     0x00000010
#define GL_ALL_SHADER_BITS                0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE              0x8258
#define GL_ACTIVE_PROGRAM                 0x8259
#define GL_PROGRAM_PIPELINE_BINDING       0x825A
#define GL_MAX_VIEWPORTS                  0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS         0x825C
#define GL_VIEWPORT_BOUNDS_RANGE          0x825D
#define GL_LAYER_PROVOKING_VERTEX         0x825E
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX 0x825F
#define GL_UNDEFINED_VERTEX               0x8260
typedef void (APIENTRYP PFNGLRELEASESHADERCOMPILERPROC) (void);
typedef void (APIENTRYP PFNGLSHADERBINARYPROC) (GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void *binary, GLsizei length);
typedef void (APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC) (GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC) (GLfloat n, GLfloat f);
typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC) (GLfloat d);
typedef void (APIENTRYP PFNGLGETPROGRAMBINARYPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
typedef void (APIENTRYP PFNGLPROGRAMBINARYPROC) (GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC) (GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLUSEPROGRAMSTAGESPROC) (GLuint pipeline, GLbitfield stages, GLuint program);
typedef void (APIENTRYP PFNGLACTIVESHADERPROGRAMPROC) (GLuint pipeline, GLuint program);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROGRAMVPROC) (GLenum type, GLsizei count, const GLchar *const*strings);
typedef void (APIENTRYP PFNGLBINDPROGRAMPIPELINEPROC) (GLuint pipeline);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPIPELINESPROC) (GLsizei n, const GLuint *pipelines);
typedef void (APIENTRYP PFNGLGENPROGRAMPIPELINESPROC) (GLsizei n, GLuint *pipelines);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPIPELINEPROC) (GLuint pipeline);
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEIVPROC) (GLuint pipeline, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IPROC) (GLuint program, GLint location, GLint v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FPROC) (GLuint program, GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DPROC) (GLuint program, GLint location, GLdouble v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIPROC) (GLuint program, GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IPROC) (GLuint program, GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FPROC) (GLuint program, GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DPROC) (GLuint program, GLint location, GLdouble v0, GLdouble v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIPROC) (GLuint program, GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IPROC) (GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FPROC) (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DPROC) (GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIPROC) (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IPROC) (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVPROC) (GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FPROC) (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVPROC) (GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DPROC) (GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DVPROC) (GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIPROC) (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVPROC) (GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC) (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEPROC) (GLuint pipeline);
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGPROC) (GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DPROC) (GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLDVPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLVIEWPORTARRAYVPROC) (GLuint first, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLSCISSORARRAYVPROC) (GLuint first, GLsizei count, const GLint *v);
typedef void (APIENTRYP PFNGLSCISSORINDEXEDPROC) (GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLSCISSORINDEXEDVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYVPROC) (GLuint first, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDPROC) (GLuint index, GLdouble n, GLdouble f);
typedef void (APIENTRYP PFNGLGETFLOATI_VPROC) (GLenum target, GLuint index, GLfloat *data);
typedef void (APIENTRYP PFNGLGETDOUBLEI_VPROC) (GLenum target, GLuint index, GLdouble *data);
#endif /* GL_VERSION_4_1 */

#ifndef GL_VERSION_4_2
#define GL_VERSION_4_2 1
#define GL_COPY_READ_BUFFER_BINDING       0x8F36
#define GL_COPY_WRITE_BUFFER_BINDING      0x8F37
#define GL_TRANSFORM_FEEDBACK_ACTIVE      0x8E24
#define GL_TRANSFORM_FEEDBACK_PAUSED      0x8E23
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH  0x9127
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH  0x9129
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE   0x912A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH    0x912B
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT   0x912C
#define GL_PACK_COMPRESSED_BLOCK_DEPTH    0x912D
#define GL_PACK_COMPRESSED_BLOCK_SIZE     0x912E
#define GL_NUM_SAMPLE_COUNTS              0x9380
#define GL_MIN_MAP_BUFFER_ALIGNMENT       0x90BC
#define GL_ATOMIC_COUNTER_BUFFER          0x92C0
#define GL_ATOMIC_COUNTER_BUFFER_BINDING  0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START    0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE     0x92C3
#define GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE 0x92C4
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS 0x92C5
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES 0x92C6
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER 0x92C7
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER 0x92C8
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER 0x92C9
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER 0x92CA
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER 0x92CB
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS 0x92CE
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS 0x92CF
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS     0x92D2
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS 0x92D4
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS   0x92D5
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS   0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS   0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS  0x92D9
#define GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX 0x92DA
#define GL_UNSIGNED_INT_ATOMIC_COUNTER    0x92DB
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT      0x00000002
#define GL_UNIFORM_BARRIER_BIT            0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT      0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT            0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT       0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT     0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT      0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT        0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT     0x00001000
#define GL_ALL_BARRIER_BITS               0xFFFFFFFF
#define GL_MAX_IMAGE_UNITS                0x8F38
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS 0x8F39
#define GL_IMAGE_BINDING_NAME             0x8F3A
#define GL_IMAGE_BINDING_LEVEL            0x8F3B
#define GL_IMAGE_BINDING_LAYERED          0x8F3C
#define GL_IMAGE_BINDING_LAYER            0x8F3D
#define GL_IMAGE_BINDING_ACCESS           0x8F3E
#define GL_IMAGE_1D                       0x904C
#define GL_IMAGE_2D                       0x904D
#define GL_IMAGE_3D                       0x904E
#define GL_IMAGE_2D_RECT                  0x904F
#define GL_IMAGE_CUBE                     0x9050
#define GL_IMAGE_BUFFER                   0x9051
#define GL_IMAGE_1D_ARRAY                 0x9052
#define GL_IMAGE_2D_ARRAY                 0x9053
#define GL_IMAGE_CUBE_MAP_ARRAY           0x9054
#define GL_IMAGE_2D_MULTISAMPLE           0x9055
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY     0x9056
#define GL_INT_IMAGE_1D                   0x9057
#define GL_INT_IMAGE_2D                   0x9058
#define GL_INT_IMAGE_3D                   0x9059
#define GL_INT_IMAGE_2D_RECT              0x905A
#define GL_INT_IMAGE_CUBE                 0x905B
#define GL_INT_IMAGE_BUFFER               0x905C
#define GL_INT_IMAGE_1D_ARRAY             0x905D
#define GL_INT_IMAGE_2D_ARRAY             0x905E
#define GL_INT_IMAGE_CUBE_MAP_ARRAY       0x905F
#define GL_INT_IMAGE_2D_MULTISAMPLE       0x9060
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x9061
#define GL_UNSIGNED_INT_IMAGE_1D          0x9062
#define GL_UNSIGNED_INT_IMAGE_2D          0x9063
#define GL_UNSIGNED_INT_IMAGE_3D          0x9064
#define GL_UNSIGNED_INT_IMAGE_2D_RECT     0x9065
#define GL_UNSIGNED_INT_IMAGE_CUBE        0x9066
#define GL_UNSIGNED_INT_IMAGE_BUFFER      0x9067
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY    0x9068
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY    0x9069
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY 0x906A
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 0x906B
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x906C
#define GL_MAX_IMAGE_SAMPLES              0x906D
#define GL_IMAGE_BINDING_FORMAT           0x906E
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_MAX_VERTEX_IMAGE_UNIFORMS      0x90CA
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS 0x90CC
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS    0x90CD
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS    0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS    0x90CF
#define GL_COMPRESSED_RGBA_BPTC_UNORM     0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F
#define GL_TEXTURE_IMMUTABLE_FORMAT       0x912F
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) (GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATIVPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) (GLuint program, GLuint bufferIndex, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTUREPROC) (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void (APIENTRYP PFNGLMEMORYBARRIERPROC) (GLbitfield barriers);
typedef void (APIENTRYP PFNGLTEXSTORAGE1DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) (GLenum mode, GLuint id, GLsizei instancecount);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) (GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
#endif /* GL_VERSION_4_2 */

#ifndef GL_VERSION_4_3
#define GL_VERSION_4_3 1
typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#define GL_NUM_SHADING_LANGUAGE_VERSIONS  0x82E9
#define GL_VERTEX_ATTRIB_ARRAY_LONG       0x874E
#define GL_COMPRESSED_RGB8_ETC2           0x9274
#define GL_COMPRESSED_SRGB8_ETC2          0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_COMPRESSED_R11_EAC             0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC      0x9271
#define GL_COMPRESSED_RG11_EAC            0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC     0x9273
#define GL_PRIMITIVE_RESTART_FIXED_INDEX  0x8D69
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_MAX_ELEMENT_INDEX              0x8D6B
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS     0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS     0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS    0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT   0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE    0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE        0x8267
#define GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER 0x90EC
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER 0x90ED
#define GL_DISPATCH_INDIRECT_BUFFER       0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT             0x00000020
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
#define GL_MAX_UNIFORM_LOCATIONS          0x826E
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS     0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH          0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
#define GL_MAX_FRAMEBUFFER_LAYERS         0x9317
#define GL_MAX_FRAMEBUFFER_SAMPLES        0x9318
#define GL_INTERNALFORMAT_SUPPORTED       0x826F
#define GL_INTERNALFORMAT_PREFERRED       0x8270
#define GL_INTERNALFORMAT_RED_SIZE        0x8271
#define GL_INTERNALFORMAT_GREEN_SIZE      0x8272
#define GL_INTERNALFORMAT_BLUE_SIZE       0x8273
#define GL_INTERNALFORMAT_ALPHA_SIZE      0x8274
#define GL_INTERNALFORMAT_DEPTH_SIZE      0x8275
#define GL_INTERNALFORMAT_STENCIL_SIZE    0x8276
#define GL_INTERNALFORMAT_SHARED_SIZE     0x8277
#define GL_INTERNALFORMAT_RED_TYPE        0x8278
#define GL_INTERNALFORMAT_GREEN_TYPE      0x8279
#define GL_INTERNALFORMAT_BLUE_TYPE       0x827A
#define GL_INTERNALFORMAT_ALPHA_TYPE      0x827B
#define GL_INTERNALFORMAT_DEPTH_TYPE      0x827C
#define GL_INTERNALFORMAT_STENCIL_TYPE    0x827D
#define GL_MAX_WIDTH                      0x827E
#define GL_MAX_HEIGHT                     0x827F
#define GL_MAX_DEPTH                      0x8280
#define GL_MAX_LAYERS                     0x8281
#define GL_MAX_COMBINED_DIMENSIONS        0x8282
#define GL_COLOR_COMPONENTS               0x8283
#define GL_DEPTH_COMPONENTS               0x8284
#define GL_STENCIL_COMPONENTS             0x8285
#define GL_COLOR_RENDERABLE               0x8286
#define GL_DEPTH_RENDERABLE               0x8287
#define GL_STENCIL_RENDERABLE             0x8288
#define GL_FRAMEBUFFER_RENDERABLE         0x8289
#define GL_FRAMEBUFFER_RENDERABLE_LAYERED 0x828A
#define GL_FRAMEBUFFER_BLEND              0x828B
#define GL_READ_PIXELS                    0x828C
#define GL_READ_PIXELS_FORMAT             0x828D
#define GL_READ_PIXELS_TYPE               0x828E
#define GL_TEXTURE_IMAGE_FORMAT           0x828F
#define GL_TEXTURE_IMAGE_TYPE             0x8290
#define GL_GET_TEXTURE_IMAGE_FORMAT       0x8291
#define GL_GET_TEXTURE_IMAGE_TYPE         0x8292
#define GL_MIPMAP                         0x8293
#define GL_MANUAL_GENERATE_MIPMAP         0x8294
#define GL_AUTO_GENERATE_MIPMAP           0x8295
#define GL_COLOR_ENCODING                 0x8296
#define GL_SRGB_READ                      0x8297
#define GL_SRGB_WRITE                     0x8298
#define GL_FILTER                         0x829A
#define GL_VERTEX_TEXTURE                 0x829B
#define GL_TESS_CONTROL_TEXTURE           0x829C
#define GL_TESS_EVALUATION_TEXTURE        0x829D
#define GL_GEOMETRY_TEXTURE               0x829E
#define GL_FRAGMENT_TEXTURE               0x829F
#define GL_COMPUTE_TEXTURE                0x82A0
#define GL_TEXTURE_SHADOW                 0x82A1
#define GL_TEXTURE_GATHER                 0x82A2
#define GL_TEXTURE_GATHER_SHADOW          0x82A3
#define GL_SHADER_IMAGE_LOAD              0x82A4
#define GL_SHADER_IMAGE_STORE             0x82A5
#define GL_SHADER_IMAGE_ATOMIC            0x82A6
#define GL_IMAGE_TEXEL_SIZE               0x82A7
#define GL_IMAGE_COMPATIBILITY_CLASS      0x82A8
#define GL_IMAGE_PIXEL_FORMAT             0x82A9
#define GL_IMAGE_PIXEL_TYPE               0x82AA
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST 0x82AC
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST 0x82AD
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE 0x82AE
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE 0x82AF
#define GL_TEXTURE_COMPRESSED_BLOCK_WIDTH 0x82B1
#define GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT 0x82B2
#define GL_TEXTURE_COMPRESSED_BLOCK_SIZE  0x82B3
#define GL_CLEAR_BUFFER                   0x82B4
#define GL_TEXTURE_VIEW                   0x82B5
#define GL_VIEW_COMPATIBILITY_CLASS       0x82B6
#define GL_FULL_SUPPORT                   0x82B7
#define GL_CAVEAT_SUPPORT                 0x82B8
#define GL_IMAGE_CLASS_4_X_32             0x82B9
#define GL_IMAGE_CLASS_2_X_32             0x82BA
#define GL_IMAGE_CLASS_1_X_32             0x82BB
#define GL_IMAGE_CLASS_4_X_16             0x82BC
#define GL_IMAGE_CLASS_2_X_16             0x82BD
#define GL_IMAGE_CLASS_1_X_16             0x82BE
#define GL_IMAGE_CLASS_4_X_8              0x82BF
#define GL_IMAGE_CLASS_2_X_8              0x82C0
#define GL_IMAGE_CLASS_1_X_8              0x82C1
#define GL_IMAGE_CLASS_11_11_10           0x82C2
#define GL_IMAGE_CLASS_10_10_10_2         0x82C3
#define GL_VIEW_CLASS_128_BITS            0x82C4
#define GL_VIEW_CLASS_96_BITS             0x82C5
#define GL_VIEW_CLASS_64_BITS             0x82C6
#define GL_VIEW_CLASS_48_BITS             0x82C7
#define GL_VIEW_CLASS_32_BITS             0x82C8
#define GL_VIEW_CLASS_24_BITS             0x82C9
#define GL_VIEW_CLASS_16_BITS             0x82CA
#define GL_VIEW_CLASS_8_BITS              0x82CB
#define GL_VIEW_CLASS_S3TC_DXT1_RGB       0x82CC
#define GL_VIEW_CLASS_S3TC_DXT1_RGBA      0x82CD
#define GL_VIEW_CLASS_S3TC_DXT3_RGBA      0x82CE
#define GL_VIEW_CLASS_S3TC_DXT5_RGBA      0x82CF
#define GL_VIEW_CLASS_RGTC1_RED           0x82D0
#define GL_VIEW_CLASS_RGTC2_RG            0x82D1
#define GL_VIEW_CLASS_BPTC_UNORM          0x82D2
#define GL_VIEW_CLASS_BPTC_FLOAT          0x82D3
#define GL_UNIFORM                        0x92E1
#define GL_UNIFORM_BLOCK                  0x92E2
#define GL_PROGRAM_INPUT                  0x92E3
#define GL_PROGRAM_OUTPUT                 0x92E4
#define GL_BUFFER_VARIABLE                0x92E5
#define GL_SHADER_STORAGE_BLOCK           0x92E6
#define GL_VERTEX_SUBROUTINE              0x92E8
#define GL_TESS_CONTROL_SUBROUTINE        0x92E9
#define GL_TESS_EVALUATION_SUBROUTINE     0x92EA
#define GL_GEOMETRY_SUBROUTINE            0x92EB
#define GL_FRAGMENT_SUBROUTINE            0x92EC
#define GL_COMPUTE_SUBROUTINE             0x92ED
#define GL_VERTEX_SUBROUTINE_UNIFORM      0x92EE
#define GL_TESS_CONTROL_SUBROUTINE_UNIFORM 0x92EF
#define GL_TESS_EVALUATION_SUBROUTINE_UNIFORM 0x92F0
#define GL_GEOMETRY_SUBROUTINE_UNIFORM    0x92F1
#define GL_FRAGMENT_SUBROUTINE_UNIFORM    0x92F2
#define GL_COMPUTE_SUBROUTINE_UNIFORM     0x92F3
#define GL_TRANSFORM_FEEDBACK_VARYING     0x92F4
#define GL_ACTIVE_RESOURCES               0x92F5
#define GL_MAX_NAME_LENGTH                0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES       0x92F7
#define GL_MAX_NUM_COMPATIBLE_SUBROUTINES 0x92F8
#define GL_NAME_LENGTH                    0x92F9
#define GL_TYPE                           0x92FA
#define GL_ARRAY_SIZE                     0x92FB
#define GL_OFFSET                         0x92FC
#define GL_BLOCK_INDEX                    0x92FD
#define GL_ARRAY_STRIDE                   0x92FE
#define GL_MATRIX_STRIDE                  0x92FF
#define GL_IS_ROW_MAJOR                   0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX    0x9301
#define GL_BUFFER_BINDING                 0x9302
#define GL_BUFFER_DATA_SIZE               0x9303
#define GL_NUM_ACTIVE_VARIABLES           0x9304
#define GL_ACTIVE_VARIABLES               0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER    0x9306
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER 0x9308
#define GL_REFERENCED_BY_GEOMETRY_SHADER  0x9309
#define GL_REFERENCED_BY_FRAGMENT_SHADER  0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER   0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE           0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE         0x930D
#define GL_LOCATION                       0x930E
#define GL_LOCATION_INDEX                 0x930F
#define GL_IS_PER_PATCH                   0x92E7
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING  0x90D3
#define GL_SHADER_STORAGE_BUFFER_START    0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE     0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE  0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT     0x00002000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_DEPTH_STENCIL_TEXTURE_MODE     0x90EA
#define GL_TEXTURE_BUFFER_OFFSET          0x919D
#define GL_TEXTURE_BUFFER_SIZE            0x919E
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT 0x919F
#define GL_TEXTURE_VIEW_MIN_LEVEL         0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS        0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER         0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS        0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS       0x82DF
#define GL_VERTEX_ATTRIB_BINDING          0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET  0x82D5
#define GL_VERTEX_BINDING_DIVISOR         0x82D6
#define GL_VERTEX_BINDING_OFFSET          0x82D7
#define GL_VERTEX_BINDING_STRIDE          0x82D8
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS     0x82DA
#define GL_VERTEX_BINDING_BUFFER          0x8F4F
typedef void (APIENTRYP PFNGLCLEARBUFFERDATAPROC) (GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARBUFFERSUBDATAPROC) (GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEPROC) (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEINDIRECTPROC) (GLintptr indirect);
typedef void (APIENTRYP PFNGLCOPYIMAGESUBDATAPROC) (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
typedef void (APIENTRYP PFNGLFRAMEBUFFERPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATI64VPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint64 *params);
typedef void (APIENTRYP PFNGLINVALIDATETEXSUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLINVALIDATETEXIMAGEPROC) (GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLINVALIDATEBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLINVALIDATEBUFFERDATAPROC) (GLuint buffer);
typedef void (APIENTRYP PFNGLINVALIDATEFRAMEBUFFERPROC) (GLenum target, GLsizei numAttachments, const GLenum *attachments);
typedef void (APIENTRYP PFNGLINVALIDATESUBFRAMEBUFFERPROC) (GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTPROC) (GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTPROC) (GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLGETPROGRAMINTERFACEIVPROC) (GLuint program, GLenum programInterface, GLenum pname, GLint *params);
typedef GLuint (APIENTRYP PFNGLGETPROGRAMRESOURCEINDEXPROC) (GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCENAMEPROC) (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCEIVPROC) (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei count, GLsizei *length, GLint *params);
typedef GLint (APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONPROC) (GLuint program, GLenum programInterface, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC) (GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLSHADERSTORAGEBLOCKBINDINGPROC) (GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
typedef void (APIENTRYP PFNGLTEXBUFFERRANGEPROC) (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXTUREVIEWPROC) (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERPROC) (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXATTRIBFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXATTRIBBINDINGPROC) (GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLVERTEXBINDINGDIVISORPROC) (GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, const void *userParam);
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC) (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC) (GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC) (void);
typedef void (APIENTRYP PFNGLOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC) (const void *ptr, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC) (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
#endif /* GL_VERSION_4_3 */

#ifndef GL_VERSION_4_4
#define GL_VERSION_4_4 1
#define GL_MAX_VERTEX_ATTRIB_STRIDE       0x82E5
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_TEXTURE_BUFFER_BINDING         0x8C2A
#define GL_MAP_PERSISTENT_BIT             0x0040
#define GL_MAP_COHERENT_BIT               0x0080
#define GL_DYNAMIC_STORAGE_BIT            0x0100
#define GL_CLIENT_STORAGE_BIT             0x0200
#define GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT 0x00004000
#define GL_BUFFER_IMMUTABLE_STORAGE       0x821F
#define GL_BUFFER_STORAGE_FLAGS           0x8220
#define GL_CLEAR_TEXTURE                  0x9365
#define GL_LOCATION_COMPONENT             0x934A
#define GL_TRANSFORM_FEEDBACK_BUFFER_INDEX 0x934B
#define GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE 0x934C
#define GL_QUERY_BUFFER                   0x9192
#define GL_QUERY_BUFFER_BARRIER_BIT       0x00008000
#define GL_QUERY_BUFFER_BINDING           0x9193
#define GL_QUERY_RESULT_NO_WAIT           0x9194
#define GL_MIRROR_CLAMP_TO_EDGE           0x8743
typedef void (APIENTRYP PFNGLBUFFERSTORAGEPROC) (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLCLEARTEXIMAGEPROC) (GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARTEXSUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLBINDBUFFERSBASEPROC) (GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERSRANGEPROC) (GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
typedef void (APIENTRYP PFNGLBINDTEXTURESPROC) (GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLBINDSAMPLERSPROC) (GLuint first, GLsizei count, const GLuint *samplers);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTURESPROC) (GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERSPROC) (GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
#endif /* GL_VERSION_4_4 */

#ifndef GL_VERSION_4_5
#define GL_VERSION_4_5 1
#define GL_CONTEXT_LOST                   0x0507
#define GL_NEGATIVE_ONE_TO_ONE            0x935E
#define GL_ZERO_TO_ONE                    0x935F
#define GL_CLIP_ORIGIN                    0x935C
#define GL_CLIP_DEPTH_MODE                0x935D
#define GL_QUERY_WAIT_INVERTED            0x8E17
#define GL_QUERY_NO_WAIT_INVERTED         0x8E18
#define GL_QUERY_BY_REGION_WAIT_INVERTED  0x8E19
#define GL_QUERY_BY_REGION_NO_WAIT_INVERTED 0x8E1A
#define GL_MAX_CULL_DISTANCES             0x82F9
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES 0x82FA
#define GL_TEXTURE_TARGET                 0x1006
#define GL_QUERY_TARGET                   0x82EA
#define GL_GUILTY_CONTEXT_RESET           0x8253
#define GL_INNOCENT_CONTEXT_RESET         0x8254
#define GL_UNKNOWN_CONTEXT_RESET          0x8255
#define GL_RESET_NOTIFICATION_STRATEGY    0x8256
#define GL_LOSE_CONTEXT_ON_RESET          0x8252
#define GL_NO_RESET_NOTIFICATION          0x8261
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT 0x00000004
#define GL_CONTEXT_RELEASE_BEHAVIOR       0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
typedef void (APIENTRYP PFNGLCLIPCONTROLPROC) (GLenum origin, GLenum depth);
typedef void (APIENTRYP PFNGLCREATETRANSFORMFEEDBACKSPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC) (GLuint xfb, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC) (GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKIVPROC) (GLuint xfb, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint *param);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI64_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint64 *param);
typedef void (APIENTRYP PFNGLCREATEBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSTORAGEPROC) (GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLNAMEDBUFFERDATAPROC) (GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRYP PFNGLCOPYNAMEDBUFFERSUBDATAPROC) (GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERDATAPROC) (GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef void *(APIENTRYP PFNGLMAPNAMEDBUFFERPROC) (GLuint buffer, GLenum access);
typedef void *(APIENTRYP PFNGLMAPNAMEDBUFFERRANGEPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (APIENTRYP PFNGLUNMAPNAMEDBUFFERPROC) (GLuint buffer);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERIVPROC) (GLuint buffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERI64VPROC) (GLuint buffer, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPOINTERVPROC) (GLuint buffer, GLenum pname, void **params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
typedef void (APIENTRYP PFNGLCREATEFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC) (GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC) (GLuint framebuffer, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTUREPROC) (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC) (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC) (GLuint framebuffer, GLenum buf);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC) (GLuint framebuffer, GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC) (GLuint framebuffer, GLenum src);
typedef void (APIENTRYP PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC) (GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments);
typedef void (APIENTRYP PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC) (GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERIVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERFVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERFIPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef void (APIENTRYP PFNGLBLITNAMEDFRAMEBUFFERPROC) (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef GLenum (APIENTRYP PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC) (GLuint framebuffer, GLenum target);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC) (GLuint framebuffer, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCREATERENDERBUFFERSPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEPROC) (GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC) (GLuint renderbuffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCREATETEXTURESPROC) (GLenum target, GLsizei n, GLuint *textures);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERPROC) (GLuint texture, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERRANGEPROC) (GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE1DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC) (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC) (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFPROC) (GLuint texture, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFVPROC) (GLuint texture, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIPROC) (GLuint texture, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIIVPROC) (GLuint texture, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIUIVPROC) (GLuint texture, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIVPROC) (GLuint texture, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLGENERATETEXTUREMIPMAPPROC) (GLuint texture);
typedef void (APIENTRYP PFNGLBINDTEXTUREUNITPROC) (GLuint unit, GLuint texture);
typedef void (APIENTRYP PFNGLGETTEXTUREIMAGEPROC) (GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC) (GLuint texture, GLint level, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERFVPROC) (GLuint texture, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERIVPROC) (GLuint texture, GLint level, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERFVPROC) (GLuint texture, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIIVPROC) (GLuint texture, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIUIVPROC) (GLuint texture, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIVPROC) (GLuint texture, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCREATEVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLDISABLEVERTEXARRAYATTRIBPROC) (GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLENABLEVERTEXARRAYATTRIBPROC) (GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLVERTEXARRAYELEMENTBUFFERPROC) (GLuint vaobj, GLuint buffer);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBUFFERPROC) (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBUFFERSPROC) (GLuint vaobj, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBBINDINGPROC) (GLuint vaobj, GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBIFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBLFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLVERTEXARRAYBINDINGDIVISORPROC) (GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYIVPROC) (GLuint vaobj, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINDEXEDIVPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINDEXED64IVPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint64 *param);
typedef void (APIENTRYP PFNGLCREATESAMPLERSPROC) (GLsizei n, GLuint *samplers);
typedef void (APIENTRYP PFNGLCREATEPROGRAMPIPELINESPROC) (GLsizei n, GLuint *pipelines);
typedef void (APIENTRYP PFNGLCREATEQUERIESPROC) (GLenum target, GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTI64VPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTIVPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTUI64VPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTUIVPROC) (GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLMEMORYBARRIERBYREGIONPROC) (GLbitfield barriers);
typedef void (APIENTRYP PFNGLGETTEXTURESUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void *pixels);
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSPROC) (void);
typedef void (APIENTRYP PFNGLGETNCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint lod, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETNTEXIMAGEPROC) (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLGETNUNIFORMDVPROC) (GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMFVPROC) (GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMIVPROC) (GLuint program, GLint location, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVPROC) (GLuint program, GLint location, GLsizei bufSize, GLuint *params);
typedef void (APIENTRYP PFNGLREADNPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
typedef void (APIENTRYP PFNGLTEXTUREBARRIERPROC) (void);
#endif /* GL_VERSION_4_5 */

#ifndef GL_VERSION_4_6
#define GL_VERSION_4_6 1
#define GL_SHADER_BINARY_FORMAT_SPIR_V    0x9551
#define GL_SPIR_V_BINARY                  0x9552
#define GL_PARAMETER_BUFFER               0x80EE
#define GL_PARAMETER_BUFFER_BINDING       0x80EF
#define GL_CONTEXT_FLAG_NO_ERROR_BIT      0x00000008
#define GL_VERTICES_SUBMITTED             0x82EE
#define GL_PRIMITIVES_SUBMITTED           0x82EF
#define GL_VERTEX_SHADER_INVOCATIONS      0x82F0
#define GL_TESS_CONTROL_SHADER_PATCHES    0x82F1
#define GL_TESS_EVALUATION_SHADER_INVOCATIONS 0x82F2
#define GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED 0x82F3
#define GL_FRAGMENT_SHADER_INVOCATIONS    0x82F4
#define GL_COMPUTE_SHADER_INVOCATIONS     0x82F5
#define GL_CLIPPING_INPUT_PRIMITIVES      0x82F6
#define GL_CLIPPING_OUTPUT_PRIMITIVES     0x82F7
#define GL_POLYGON_OFFSET_CLAMP           0x8E1B
#define GL_SPIR_V_EXTENSIONS              0x9553
#define GL_NUM_SPIR_V_EXTENSIONS          0x9554
#define GL_TEXTURE_MAX_ANISOTROPY         0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY     0x84FF
#define GL_TRANSFORM_FEEDBACK_OVERFLOW    0x82EC
#define GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW 0x82ED
typedef void (APIENTRYP PFNGLSPECIALIZESHADERPROC) (GLuint shader, const GLchar *pEntryPoint, GLuint numSpecializationConstants, const GLuint *pConstantIndex, const GLuint *pConstantValue);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC) (GLenum mode, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC) (GLenum mode, GLenum type, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLPOLYGONOFFSETCLAMPPROC) (GLfloat factor, GLfloat units, GLfloat clamp);
#endif /* GL_VERSION_4_6 */

#ifndef GL_ARB_ES2_compatibility
#define GL_ARB_ES2_compatibility 1
#endif /* GL_ARB_ES2_compatibility */

#ifndef GL_ARB_ES3_1_compatibility
#define GL_ARB_ES3_1_compatibility 1
#endif /* GL_ARB_ES3_1_compatibility */

#ifndef GL_ARB_ES3_2_compatibility
#define GL_ARB_ES3_2_compatibility 1
#define GL_PRIMITIVE_BOUNDING_BOX_ARB     0x92BE
#define GL_MULTISAMPLE_LINE_WIDTH_RANGE_ARB 0x9381
#define GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY_ARB 0x9382
typedef void (APIENTRYP PFNGLPRIMITIVEBOUNDINGBOXARBPROC) (GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW);
#endif /* GL_ARB_ES3_2_compatibility */

#ifndef GL_ARB_ES3_compatibility
#define GL_ARB_ES3_compatibility 1
#endif /* GL_ARB_ES3_compatibility */

#ifndef GL_ARB_arrays_of_arrays
#define GL_ARB_arrays_of_arrays 1
#endif /* GL_ARB_arrays_of_arrays */

#ifndef GL_ARB_base_instance
#define GL_ARB_base_instance 1
#endif /* GL_ARB_base_instance */

#ifndef GL_ARB_bindless_texture
#define GL_ARB_bindless_texture 1
typedef khronos_uint64_t GLuint64EXT;
#define GL_UNSIGNED_INT64_ARB             0x140F
typedef GLuint64 (APIENTRYP PFNGLGETTEXTUREHANDLEARBPROC) (GLuint texture);
typedef GLuint64 (APIENTRYP PFNGLGETTEXTURESAMPLERHANDLEARBPROC) (GLuint texture, GLuint sampler);
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLERESIDENTARBPROC) (GLuint64 handle);
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC) (GLuint64 handle);
typedef GLuint64 (APIENTRYP PFNGLGETIMAGEHANDLEARBPROC) (GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLERESIDENTARBPROC) (GLuint64 handle, GLenum access);
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC) (GLuint64 handle);
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64ARBPROC) (GLint location, GLuint64 value);
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64VARBPROC) (GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC) (GLuint program, GLint location, GLuint64 value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLuint64 *values);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREHANDLERESIDENTARBPROC) (GLuint64 handle);
typedef GLboolean (APIENTRYP PFNGLISIMAGEHANDLERESIDENTARBPROC) (GLuint64 handle);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1UI64ARBPROC) (GLuint index, GLuint64EXT x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1UI64VARBPROC) (GLuint index, const GLuint64EXT *v);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLUI64VARBPROC) (GLuint index, GLenum pname, GLuint64EXT *params);
#endif /* GL_ARB_bindless_texture */

#ifndef GL_ARB_blend_func_extended
#define GL_ARB_blend_func_extended 1
#endif /* GL_ARB_blend_func_extended */

#ifndef GL_ARB_buffer_storage
#define GL_ARB_buffer_storage 1
#endif /* GL_ARB_buffer_storage */

#ifndef GL_ARB_cl_event
#define GL_ARB_cl_event 1
struct _cl_context;
struct _cl_event;
#define GL_SYNC_CL_EVENT_ARB              0x8240
#define GL_SYNC_CL_EVENT_COMPLETE_ARB     0x8241
typedef GLsync (APIENTRYP PFNGLCREATESYNCFROMCLEVENTARBPROC) (struct _cl_context *context, struct _cl_event *event, GLbitfield flags);
#endif /* GL_ARB_cl_event */

#ifndef GL_ARB_clear_buffer_object
#define GL_ARB_clear_buffer_object 1
#endif /* GL_ARB_clear_buffer_object */

#ifndef GL_ARB_clear_texture
#define GL_ARB_clear_texture 1
#endif /* GL_ARB_clear_texture */

#ifndef GL_ARB_clip_control
#define GL_ARB_clip_control 1
#endif /* GL_ARB_clip_control */

#ifndef GL_ARB_compressed_texture_pixel_storage
#define GL_ARB_compressed_texture_pixel_storage 1
#endif /* GL_ARB_compressed_texture_pixel_storage */

#ifndef GL_ARB_compute_shader
#define GL_ARB_compute_shader 1
#endif /* GL_ARB_compute_shader */

#ifndef GL_ARB_compute_variable_group_size
#define GL_ARB_compute_variable_group_size 1
#define GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB 0x9344
#define GL_MAX_COMPUTE_FIXED_GROUP_INVOCATIONS_ARB 0x90EB
#define GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB 0x9345
#define GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB 0x91BF
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC) (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z);
#endif /* GL_ARB_compute_variable_group_size */

#ifndef GL_ARB_conditional_render_inverted
#define GL_ARB_conditional_render_inverted 1
#endif /* GL_ARB_conditional_render_inverted */

#ifndef GL_ARB_conservative_depth
#define GL_ARB_conservative_depth 1
#endif /* GL_ARB_conservative_depth */

#ifndef GL_ARB_copy_buffer
#define GL_ARB_copy_buffer 1
#endif /* GL_ARB_copy_buffer */

#ifndef GL_ARB_copy_image
#define GL_ARB_copy_image 1
#endif /* GL_ARB_copy_image */

#ifndef GL_ARB_cull_distance
#define GL_ARB_cull_distance 1
#endif /* GL_ARB_cull_distance */

#ifndef GL_ARB_debug_output
#define GL_ARB_debug_output 1
typedef void (APIENTRY  *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB   0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION_ARB    0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_ARB  0x8245
#define GL_DEBUG_SOURCE_API_ARB           0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_ARB   0x8249
#define GL_DEBUG_SOURCE_APPLICATION_ARB   0x824A
#define GL_DEBUG_SOURCE_OTHER_ARB         0x824B
#define GL_DEBUG_TYPE_ERROR_ARB           0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB 0x824E
#define GL_DEBUG_TYPE_PORTABILITY_ARB     0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_ARB     0x8250
#define GL_DEBUG_TYPE_OTHER_ARB           0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB   0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB  0x9144
#define GL_DEBUG_LOGGED_MESSAGES_ARB      0x9145
#define GL_DEBUG_SEVERITY_HIGH_ARB        0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_ARB      0x9147
#define GL_DEBUG_SEVERITY_LOW_ARB         0x9148
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLARBPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTARBPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKARBPROC) (GLDEBUGPROCARB callback, const void *userParam);
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGARBPROC) (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
#endif /* GL_ARB_debug_output */

#ifndef GL_ARB_depth_buffer_float
#define GL_ARB_depth_buffer_float 1
#endif /* GL_ARB_depth_buffer_float */

#ifndef GL_ARB_depth_clamp
#define GL_ARB_depth_clamp 1
#endif /* GL_ARB_depth_clamp */

#ifndef GL_ARB_derivative_control
#define GL_ARB_derivative_control 1
#endif /* GL_ARB_derivative_control */

#ifndef GL_ARB_direct_state_access
#define GL_ARB_direct_state_access 1
#endif /* GL_ARB_direct_state_access */

#ifndef GL_ARB_draw_buffers_blend
#define GL_ARB_draw_buffers_blend 1
typedef void (APIENTRYP PFNGLBLENDEQUATIONIARBPROC) (GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIARBPROC) (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLBLENDFUNCIARBPROC) (GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIARBPROC) (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
#endif /* GL_ARB_draw_buffers_blend */

#ifndef GL_ARB_draw_elements_base_vertex
#define GL_ARB_draw_elements_base_vertex 1
#endif /* GL_ARB_draw_elements_base_vertex */

#ifndef GL_ARB_draw_indirect
#define GL_ARB_draw_indirect 1
#endif /* GL_ARB_draw_indirect */

#ifndef GL_ARB_draw_instanced
#define GL_ARB_draw_instanced 1
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDARBPROC) (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDARBPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount);
#endif /* GL_ARB_draw_instanced */

#ifndef GL_ARB_enhanced_layouts
#define GL_ARB_enhanced_layouts 1
#endif /* GL_ARB_enhanced_layouts */

#ifndef GL_ARB_explicit_attrib_location
#define GL_ARB_explicit_attrib_location 1
#endif /* GL_ARB_explicit_attrib_location */

#ifndef GL_ARB_explicit_uniform_location
#define GL_ARB_explicit_uniform_location 1
#endif /* GL_ARB_explicit_uniform_location */

#ifndef GL_ARB_fragment_coord_conventions
#define GL_ARB_fragment_coord_conventions 1
#endif /* GL_ARB_fragment_coord_conventions */

#ifndef GL_ARB_fragment_layer_viewport
#define GL_ARB_fragment_layer_viewport 1
#endif /* GL_ARB_fragment_layer_viewport */

#ifndef GL_ARB_fragment_shader_interlock
#define GL_ARB_fragment_shader_interlock 1
#endif /* GL_ARB_fragment_shader_interlock */

#ifndef GL_ARB_framebuffer_no_attachments
#define GL_ARB_framebuffer_no_attachments 1
#endif /* GL_ARB_framebuffer_no_attachments */

#ifndef GL_ARB_framebuffer_object
#define GL_ARB_framebuffer_object 1
#endif /* GL_ARB_framebuffer_object */

#ifndef GL_ARB_framebuffer_sRGB
#define GL_ARB_framebuffer_sRGB 1
#endif /* GL_ARB_framebuffer_sRGB */

#ifndef GL_ARB_geometry_shader4
#define GL_ARB_geometry_shader4 1
#define GL_LINES_ADJACENCY_ARB            0x000A
#define GL_LINE_STRIP_ADJACENCY_ARB       0x000B
#define GL_TRIANGLES_ADJACENCY_ARB        0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY_ARB   0x000D
#define GL_PROGRAM_POINT_SIZE_ARB         0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB 0x8DA8
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB 0x8DA9
#define GL_GEOMETRY_SHADER_ARB            0x8DD9
#define GL_GEOMETRY_VERTICES_OUT_ARB      0x8DDA
#define GL_GEOMETRY_INPUT_TYPE_ARB        0x8DDB
#define GL_GEOMETRY_OUTPUT_TYPE_ARB       0x8DDC
#define GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB 0x8DDD
#define GL_MAX_VERTEX_VARYING_COMPONENTS_ARB 0x8DDE
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB 0x8DE1
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIARBPROC) (GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREARBPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERARBPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREFACEARBPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face);
#endif /* GL_ARB_geometry_shader4 */

#ifndef GL_ARB_get_program_binary
#define GL_ARB_get_program_binary 1
#endif /* GL_ARB_get_program_binary */

#ifndef GL_ARB_get_texture_sub_image
#define GL_ARB_get_texture_sub_image 1
#endif /* GL_ARB_get_texture_sub_image */

#ifndef GL_ARB_gl_spirv
#define GL_ARB_gl_spirv 1
#define GL_SHADER_BINARY_FORMAT_SPIR_V_ARB 0x9551
#define GL_SPIR_V_BINARY_ARB              0x9552
typedef void (APIENTRYP PFNGLSPECIALIZESHADERARBPROC) (GLuint shader, const GLchar *pEntryPoint, GLuint numSpecializationConstants, const GLuint *pConstantIndex, const GLuint *pConstantValue);
#endif /* GL_ARB_gl_spirv */

#ifndef GL_ARB_gpu_shader5
#define GL_ARB_gpu_shader5 1
#endif /* GL_ARB_gpu_shader5 */

#ifndef GL_ARB_gpu_shader_fp64
#define GL_ARB_gpu_shader_fp64 1
#endif /* GL_ARB_gpu_shader_fp64 */

#ifndef GL_ARB_gpu_shader_int64
#define GL_ARB_gpu_shader_int64 1
#define GL_INT64_ARB                      0x140E
#define GL_INT64_VEC2_ARB                 0x8FE9
#define GL_INT64_VEC3_ARB                 0x8FEA
#define GL_INT64_VEC4_ARB                 0x8FEB
#define GL_UNSIGNED_INT64_VEC2_ARB        0x8FF5
#define GL_UNSIGNED_INT64_VEC3_ARB        0x8FF6
#define GL_UNSIGNED_INT64_VEC4_ARB        0x8FF7
typedef void (APIENTRYP PFNGLUNIFORM1I64ARBPROC) (GLint location, GLint64 x);
typedef void (APIENTRYP PFNGLUNIFORM2I64ARBPROC) (GLint location, GLint64 x, GLint64 y);
typedef void (APIENTRYP PFNGLUNIFORM3I64ARBPROC) (GLint location, GLint64 x, GLint64 y, GLint64 z);
typedef void (APIENTRYP PFNGLUNIFORM4I64ARBPROC) (GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w);
typedef void (APIENTRYP PFNGLUNIFORM1I64VARBPROC) (GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM2I64VARBPROC) (GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM3I64VARBPROC) (GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM4I64VARBPROC) (GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM1UI64ARBPROC) (GLint location, GLuint64 x);
typedef void (APIENTRYP PFNGLUNIFORM2UI64ARBPROC) (GLint location, GLuint64 x, GLuint64 y);
typedef void (APIENTRYP PFNGLUNIFORM3UI64ARBPROC) (GLint location, GLuint64 x, GLuint64 y, GLuint64 z);
typedef void (APIENTRYP PFNGLUNIFORM4UI64ARBPROC) (GLint location, GLuint64 x, GLuint64 y, GLuint64 z, GLuint64 w);
typedef void (APIENTRYP PFNGLUNIFORM1UI64VARBPROC) (GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM2UI64VARBPROC) (GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM3UI64VARBPROC) (GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLUNIFORM4UI64VARBPROC) (GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLGETUNIFORMI64VARBPROC) (GLuint program, GLint location, GLint64 *params);
typedef void (APIENTRYP PFNGLGETUNIFORMUI64VARBPROC) (GLuint program, GLint location, GLuint64 *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMI64VARBPROC) (GLuint program, GLint location, GLsizei bufSize, GLint64 *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMUI64VARBPROC) (GLuint program, GLint location, GLsizei bufSize, GLuint64 *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1I64ARBPROC) (GLuint program, GLint location, GLint64 x);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2I64ARBPROC) (GLuint program, GLint location, GLint64 x, GLint64 y);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3I64ARBPROC) (GLuint program, GLint location, GLint64 x, GLint64 y, GLint64 z);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4I64ARBPROC) (GLuint program, GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1I64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2I64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3I64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4I64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UI64ARBPROC) (GLuint program, GLint location, GLuint64 x);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UI64ARBPROC) (GLuint program, GLint location, GLuint64 x, GLuint64 y);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UI64ARBPROC) (GLuint program, GLint location, GLuint64 x, GLuint64 y, GLuint64 z);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UI64ARBPROC) (GLuint program, GLint location, GLuint64 x, GLuint64 y, GLuint64 z, GLuint64 w);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UI64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UI64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UI64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UI64VARBPROC) (GLuint program, GLint location, GLsizei count, const GLuint64 *value);
#endif /* GL_ARB_gpu_shader_int64 */

#ifndef GL_ARB_half_float_vertex
#define GL_ARB_half_float_vertex 1
#endif /* GL_ARB_half_float_vertex */

#ifndef GL_ARB_imaging
#define GL_ARB_imaging 1
#endif /* GL_ARB_imaging */

#ifndef GL_ARB_indirect_parameters
#define GL_ARB_indirect_parameters 1
#define GL_PARAMETER_BUFFER_ARB           0x80EE
#define GL_PARAMETER_BUFFER_BINDING_ARB   0x80EF
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTCOUNTARBPROC) (GLenum mode, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTARBPROC) (GLenum mode, GLenum type, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
#endif /* GL_ARB_indirect_parameters */

#ifndef GL_ARB_instanced_arrays
#define GL_ARB_instanced_arrays 1
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ARB 0x88FE
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORARBPROC) (GLuint index, GLuint divisor);
#endif /* GL_ARB_instanced_arrays */

#ifndef GL_ARB_internalformat_query
#define GL_ARB_internalformat_query 1
#endif /* GL_ARB_internalformat_query */

#ifndef GL_ARB_internalformat_query2
#define GL_ARB_internalformat_query2 1
#define GL_SRGB_DECODE_ARB                0x8299
#define GL_VIEW_CLASS_EAC_R11             0x9383
#define GL_VIEW_CLASS_EAC_RG11            0x9384
#define GL_VIEW_CLASS_ETC2_RGB            0x9385
#define GL_VIEW_CLASS_ETC2_RGBA           0x9386
#define GL_VIEW_CLASS_ETC2_EAC_RGBA       0x9387
#define GL_VIEW_CLASS_ASTC_4x4_RGBA       0x9388
#define GL_VIEW_CLASS_ASTC_5x4_RGBA       0x9389
#define GL_VIEW_CLASS_ASTC_5x5_RGBA       0x938A
#define GL_VIEW_CLASS_ASTC_6x5_RGBA       0x938B
#define GL_VIEW_CLASS_ASTC_6x6_RGBA       0x938C
#define GL_VIEW_CLASS_ASTC_8x5_RGBA       0x938D
#define GL_VIEW_CLASS_ASTC_8x6_RGBA       0x938E
#define GL_VIEW_CLASS_ASTC_8x8_RGBA       0x938F
#define GL_VIEW_CLASS_ASTC_10x5_RGBA      0x9390
#define GL_VIEW_CLASS_ASTC_10x6_RGBA      0x9391
#define GL_VIEW_CLASS_ASTC_10x8_RGBA      0x9392
#define GL_VIEW_CLASS_ASTC_10x10_RGBA     0x9393
#define GL_VIEW_CLASS_ASTC_12x10_RGBA     0x9394
#define GL_VIEW_CLASS_ASTC_12x12_RGBA     0x9395
#endif /* GL_ARB_internalformat_query2 */

#ifndef GL_ARB_invalidate_subdata
#define GL_ARB_invalidate_subdata 1
#endif /* GL_ARB_invalidate_subdata */

#ifndef GL_ARB_map_buffer_alignment
#define GL_ARB_map_buffer_alignment 1
#endif /* GL_ARB_map_buffer_alignment */

#ifndef GL_ARB_map_buffer_range
#define GL_ARB_map_buffer_range 1
#endif /* GL_ARB_map_buffer_range */

#ifndef GL_ARB_multi_bind
#define GL_ARB_multi_bind 1
#endif /* GL_ARB_multi_bind */

#ifndef GL_ARB_multi_draw_indirect
#define GL_ARB_multi_draw_indirect 1
#endif /* GL_ARB_multi_draw_indirect */

#ifndef GL_ARB_occlusion_query2
#define GL_ARB_occlusion_query2 1
#endif /* GL_ARB_occlusion_query2 */

#ifndef GL_ARB_parallel_shader_compile
#define GL_ARB_parallel_shader_compile 1
#define GL_MAX_SHADER_COMPILER_THREADS_ARB 0x91B0
#define GL_COMPLETION_STATUS_ARB          0x91B1
typedef void (APIENTRYP PFNGLMAXSHADERCOMPILERTHREADSARBPROC) (GLuint count);
#endif /* GL_ARB_parallel_shader_compile */

#ifndef GL_ARB_pipeline_statistics_query
#define GL_ARB_pipeline_statistics_query 1
#define GL_VERTICES_SUBMITTED_ARB         0x82EE
#define GL_PRIMITIVES_SUBMITTED_ARB       0x82EF
#define GL_VERTEX_SHADER_INVOCATIONS_ARB  0x82F0
#define GL_TESS_CONTROL_SHADER_PATCHES_ARB 0x82F1
#define GL_TESS_EVALUATION_SHADER_INVOCATIONS_ARB 0x82F2
#define GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED_ARB 0x82F3
#define GL_FRAGMENT_SHADER_INVOCATIONS_ARB 0x82F4
#define GL_COMPUTE_SHADER_INVOCATIONS_ARB 0x82F5
#define GL_CLIPPING_INPUT_PRIMITIVES_ARB  0x82F6
#define GL_CLIPPING_OUTPUT_PRIMITIVES_ARB 0x82F7
#endif /* GL_ARB_pipeline_statistics_query */

#ifndef GL_ARB_pixel_buffer_object
#define GL_ARB_pixel_buffer_object 1
#define GL_PIXEL_PACK_BUFFER_ARB          0x88EB
#define GL_PIXEL_UNPACK_BUFFER_ARB        0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING_ARB  0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING_ARB 0x88EF
#endif /* GL_ARB_pixel_buffer_object */

#ifndef GL_ARB_polygon_offset_clamp
#define GL_ARB_polygon_offset_clamp 1
#endif /* GL_ARB_polygon_offset_clamp */

#ifndef GL_ARB_post_depth_coverage
#define GL_ARB_post_depth_coverage 1
#endif /* GL_ARB_post_depth_coverage */

#ifndef GL_ARB_program_interface_query
#define GL_ARB_program_interface_query 1
#endif /* GL_ARB_program_interface_query */

#ifndef GL_ARB_provoking_vertex
#define GL_ARB_provoking_vertex 1
#endif /* GL_ARB_provoking_vertex */

#ifndef GL_ARB_query_buffer_object
#define GL_ARB_query_buffer_object 1
#endif /* GL_ARB_query_buffer_object */

#ifndef GL_ARB_robust_buffer_access_behavior
#define GL_ARB_robust_buffer_access_behavior 1
#endif /* GL_ARB_robust_buffer_access_behavior */

#ifndef GL_ARB_robustness
#define GL_ARB_robustness 1
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB 0x00000004
#define GL_LOSE_CONTEXT_ON_RESET_ARB      0x8252
#define GL_GUILTY_CONTEXT_RESET_ARB       0x8253
#define GL_INNOCENT_CONTEXT_RESET_ARB     0x8254
#define GL_UNKNOWN_CONTEXT_RESET_ARB      0x8255
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_NO_RESET_NOTIFICATION_ARB      0x8261
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSARBPROC) (void);
typedef void (APIENTRYP PFNGLGETNTEXIMAGEARBPROC) (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *img);
typedef void (APIENTRYP PFNGLREADNPIXELSARBPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
typedef void (APIENTRYP PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint lod, GLsizei bufSize, void *img);
typedef void (APIENTRYP PFNGLGETNUNIFORMFVARBPROC) (GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMIVARBPROC) (GLuint program, GLint location, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVARBPROC) (GLuint program, GLint location, GLsizei bufSize, GLuint *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMDVARBPROC) (GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
#endif /* GL_ARB_robustness */

#ifndef GL_ARB_robustness_isolation
#define GL_ARB_robustness_isolation 1
#endif /* GL_ARB_robustness_isolation */

#ifndef GL_ARB_sample_locations
#define GL_ARB_sample_locations 1
#define GL_SAMPLE_LOCATION_SUBPIXEL_BITS_ARB 0x933D
#define GL_SAMPLE_LOCATION_PIXEL_GRID_WIDTH_ARB 0x933E
#define GL_SAMPLE_LOCATION_PIXEL_GRID_HEIGHT_ARB 0x933F
#define GL_PROGRAMMABLE_SAMPLE_LOCATION_TABLE_SIZE_ARB 0x9340
#define GL_SAMPLE_LOCATION_ARB            0x8E50
#define GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB 0x9341
#define GL_FRAMEBUFFER_PROGRAMMABLE_SAMPLE_LOCATIONS_ARB 0x9342
#define GL_FRAMEBUFFER_SAMPLE_LOCATION_PIXEL_GRID_ARB 0x9343
typedef void (APIENTRYP PFNGLFRAMEBUFFERSAMPLELOCATIONSFVARBPROC) (GLenum target, GLuint start, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVARBPROC) (GLuint framebuffer, GLuint start, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLEVALUATEDEPTHVALUESARBPROC) (void);
#endif /* GL_ARB_sample_locations */

#ifndef GL_ARB_sample_shading
#define GL_ARB_sample_shading 1
#define GL_SAMPLE_SHADING_ARB             0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE_ARB   0x8C37
typedef void (APIENTRYP PFNGLMINSAMPLESHADINGARBPROC) (GLfloat value);
#endif /* GL_ARB_sample_shading */

#ifndef GL_ARB_sampler_objects
#define GL_ARB_sampler_objects 1
#endif /* GL_ARB_sampler_objects */

#ifndef GL_ARB_seamless_cube_map
#define GL_ARB_seamless_cube_map 1
#endif /* GL_ARB_seamless_cube_map */

#ifndef GL_ARB_seamless_cubemap_per_texture
#define GL_ARB_seamless_cubemap_per_texture 1
#endif /* GL_ARB_seamless_cubemap_per_texture */

#ifndef GL_ARB_separate_shader_objects
#define GL_ARB_separate_shader_objects 1
#endif /* GL_ARB_separate_shader_objects */

#ifndef GL_ARB_shader_atomic_counter_ops
#define GL_ARB_shader_atomic_counter_ops 1
#endif /* GL_ARB_shader_atomic_counter_ops */

#ifndef GL_ARB_shader_atomic_counters
#define GL_ARB_shader_atomic_counters 1
#endif /* GL_ARB_shader_atomic_counters */

#ifndef GL_ARB_shader_ballot
#define GL_ARB_shader_ballot 1
#endif /* GL_ARB_shader_ballot */

#ifndef GL_ARB_shader_bit_encoding
#define GL_ARB_shader_bit_encoding 1
#endif /* GL_ARB_shader_bit_encoding */

#ifndef GL_ARB_shader_clock
#define GL_ARB_shader_clock 1
#endif /* GL_ARB_shader_clock */

#ifndef GL_ARB_shader_draw_parameters
#define GL_ARB_shader_draw_parameters 1
#endif /* GL_ARB_shader_draw_parameters */

#ifndef GL_ARB_shader_group_vote
#define GL_ARB_shader_group_vote 1
#endif /* GL_ARB_shader_group_vote */

#ifndef GL_ARB_shader_image_load_store
#define GL_ARB_shader_image_load_store 1
#endif /* GL_ARB_shader_image_load_store */

#ifndef GL_ARB_shader_image_size
#define GL_ARB_shader_image_size 1
#endif /* GL_ARB_shader_image_size */

#ifndef GL_ARB_shader_precision
#define GL_ARB_shader_precision 1
#endif /* GL_ARB_shader_precision */

#ifndef GL_ARB_shader_stencil_export
#define GL_ARB_shader_stencil_export 1
#endif /* GL_ARB_shader_stencil_export */

#ifndef GL_ARB_shader_storage_buffer_object
#define GL_ARB_shader_storage_buffer_object 1
#endif /* GL_ARB_shader_storage_buffer_object */

#ifndef GL_ARB_shader_subroutine
#define GL_ARB_shader_subroutine 1
#endif /* GL_ARB_shader_subroutine */

#ifndef GL_ARB_shader_texture_image_samples
#define GL_ARB_shader_texture_image_samples 1
#endif /* GL_ARB_shader_texture_image_samples */

#ifndef GL_ARB_shader_viewport_layer_array
#define GL_ARB_shader_viewport_layer_array 1
#endif /* GL_ARB_shader_viewport_layer_array */

#ifndef GL_ARB_shading_language_420pack
#define GL_ARB_shading_language_420pack 1
#endif /* GL_ARB_shading_language_420pack */

#ifndef GL_ARB_shading_language_include
#define GL_ARB_shading_language_include 1
#define GL_SHADER_INCLUDE_ARB             0x8DAE
#define GL_NAMED_STRING_LENGTH_ARB        0x8DE9
#define GL_NAMED_STRING_TYPE_ARB          0x8DEA
typedef void (APIENTRYP PFNGLNAMEDSTRINGARBPROC) (GLenum type, GLint namelen, const GLchar *name, GLint stringlen, const GLchar *string);
typedef void (APIENTRYP PFNGLDELETENAMEDSTRINGARBPROC) (GLint namelen, const GLchar *name);
typedef void (APIENTRYP PFNGLCOMPILESHADERINCLUDEARBPROC) (GLuint shader, GLsizei count, const GLchar *const*path, const GLint *length);
typedef GLboolean (APIENTRYP PFNGLISNAMEDSTRINGARBPROC) (GLint namelen, const GLchar *name);
typedef void (APIENTRYP PFNGLGETNAMEDSTRINGARBPROC) (GLint namelen, const GLchar *name, GLsizei bufSize, GLint *stringlen, GLchar *string);
typedef void (APIENTRYP PFNGLGETNAMEDSTRINGIVARBPROC) (GLint namelen, const GLchar *name, GLenum pname, GLint *params);
#endif /* GL_ARB_shading_language_include */

#ifndef GL_ARB_shading_language_packing
#define GL_ARB_shading_language_packing 1
#endif /* GL_ARB_shading_language_packing */

#ifndef GL_ARB_sparse_buffer
#define GL_ARB_sparse_buffer 1
#define GL_SPARSE_STORAGE_BIT_ARB         0x0400
#define GL_SPARSE_BUFFER_PAGE_SIZE_ARB    0x82F8
typedef void (APIENTRYP PFNGLBUFFERPAGECOMMITMENTARBPROC) (GLenum target, GLintptr offset, GLsizeiptr size, GLboolean commit);
typedef void (APIENTRYP PFNGLNAMEDBUFFERPAGECOMMITMENTEXTPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, GLboolean commit);
typedef void (APIENTRYP PFNGLNAMEDBUFFERPAGECOMMITMENTARBPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, GLboolean commit);
#endif /* GL_ARB_sparse_buffer */

#ifndef GL_ARB_sparse_texture
#define GL_ARB_sparse_texture 1
#define GL_TEXTURE_SPARSE_ARB             0x91A6
#define GL_VIRTUAL_PAGE_SIZE_INDEX_ARB    0x91A7
#define GL_NUM_SPARSE_LEVELS_ARB          0x91AA
#define GL_NUM_VIRTUAL_PAGE_SIZES_ARB     0x91A8
#define GL_VIRTUAL_PAGE_SIZE_X_ARB        0x9195
#define GL_VIRTUAL_PAGE_SIZE_Y_ARB        0x9196
#define GL_VIRTUAL_PAGE_SIZE_Z_ARB        0x9197
#define GL_MAX_SPARSE_TEXTURE_SIZE_ARB    0x9198
#define GL_MAX_SPARSE_3D_TEXTURE_SIZE_ARB 0x9199
#define GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_ARB 0x919A
#define GL_SPARSE_TEXTURE_FULL_ARRAY_CUBE_MIPMAPS_ARB 0x91A9
typedef void (APIENTRYP PFNGLTEXPAGECOMMITMENTARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean commit);
#endif /* GL_ARB_sparse_texture */

#ifndef GL_ARB_sparse_texture2
#define GL_ARB_sparse_texture2 1
#endif /* GL_ARB_sparse_texture2 */

#ifndef GL_ARB_sparse_texture_clamp
#define GL_ARB_sparse_texture_clamp 1
#endif /* GL_ARB_sparse_texture_clamp */

#ifndef GL_ARB_spirv_extensions
#define GL_ARB_spirv_extensions 1
#endif /* GL_ARB_spirv_extensions */

#ifndef GL_ARB_stencil_texturing
#define GL_ARB_stencil_texturing 1
#endif /* GL_ARB_stencil_texturing */

#ifndef GL_ARB_sync
#define GL_ARB_sync 1
#endif /* GL_ARB_sync */

#ifndef GL_ARB_tessellation_shader
#define GL_ARB_tessellation_shader 1
#endif /* GL_ARB_tessellation_shader */

#ifndef GL_ARB_texture_barrier
#define GL_ARB_texture_barrier 1
#endif /* GL_ARB_texture_barrier */

#ifndef GL_ARB_texture_border_clamp
#define GL_ARB_texture_border_clamp 1
#define GL_CLAMP_TO_BORDER_ARB            0x812D
#endif /* GL_ARB_texture_border_clamp */

#ifndef GL_ARB_texture_buffer_object
#define GL_ARB_texture_buffer_object 1
#define GL_TEXTURE_BUFFER_ARB             0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE_ARB    0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_ARB     0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_ARB 0x8C2D
#define GL_TEXTURE_BUFFER_FORMAT_ARB      0x8C2E
typedef void (APIENTRYP PFNGLTEXBUFFERARBPROC) (GLenum target, GLenum internalformat, GLuint buffer);
#endif /* GL_ARB_texture_buffer_object */

#ifndef GL_ARB_texture_buffer_object_rgb32
#define GL_ARB_texture_buffer_object_rgb32 1
#endif /* GL_ARB_texture_buffer_object_rgb32 */

#ifndef GL_ARB_texture_buffer_range
#define GL_ARB_texture_buffer_range 1
#endif /* GL_ARB_texture_buffer_range */

#ifndef GL_ARB_texture_compression_bptc
#define GL_ARB_texture_compression_bptc 1
#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB 0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB 0x8E8F
#endif /* GL_ARB_texture_compression_bptc */

#ifndef GL_ARB_texture_compression_rgtc
#define GL_ARB_texture_compression_rgtc 1
#endif /* GL_ARB_texture_compression_rgtc */

#ifndef GL_ARB_texture_cube_map_array
#define GL_ARB_texture_cube_map_array 1
#define GL_TEXTURE_CUBE_MAP_ARRAY_ARB     0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_ARB 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB 0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY_ARB     0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_ARB 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_ARB 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_ARB 0x900F
#endif /* GL_ARB_texture_cube_map_array */

#ifndef GL_ARB_texture_filter_anisotropic
#define GL_ARB_texture_filter_anisotropic 1
#endif /* GL_ARB_texture_filter_anisotropic */

#ifndef GL_ARB_texture_filter_minmax
#define GL_ARB_texture_filter_minmax 1
#define GL_TEXTURE_REDUCTION_MODE_ARB     0x9366
#define GL_WEIGHTED_AVERAGE_ARB           0x9367
#endif /* GL_ARB_texture_filter_minmax */

#ifndef GL_ARB_texture_gather
#define GL_ARB_texture_gather 1
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET_ARB 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET_ARB 0x8E5F
#define GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS_ARB 0x8F9F
#endif /* GL_ARB_texture_gather */

#ifndef GL_ARB_texture_mirror_clamp_to_edge
#define GL_ARB_texture_mirror_clamp_to_edge 1
#endif /* GL_ARB_texture_mirror_clamp_to_edge */

#ifndef GL_ARB_texture_mirrored_repeat
#define GL_ARB_texture_mirrored_repeat 1
#define GL_MIRRORED_REPEAT_ARB            0x8370
#endif /* GL_ARB_texture_mirrored_repeat */

#ifndef GL_ARB_texture_multisample
#define GL_ARB_texture_multisample 1
#endif /* GL_ARB_texture_multisample */

#ifndef GL_ARB_texture_non_power_of_two
#define GL_ARB_texture_non_power_of_two 1
#endif /* GL_ARB_texture_non_power_of_two */

#ifndef GL_ARB_texture_query_levels
#define GL_ARB_texture_query_levels 1
#endif /* GL_ARB_texture_query_levels */

#ifndef GL_ARB_texture_query_lod
#define GL_ARB_texture_query_lod 1
#endif /* GL_ARB_texture_query_lod */

#ifndef GL_ARB_texture_rg
#define GL_ARB_texture_rg 1
#endif /* GL_ARB_texture_rg */

#ifndef GL_ARB_texture_rgb10_a2ui
#define GL_ARB_texture_rgb10_a2ui 1
#endif /* GL_ARB_texture_rgb10_a2ui */

#ifndef GL_ARB_texture_stencil8
#define GL_ARB_texture_stencil8 1
#endif /* GL_ARB_texture_stencil8 */

#ifndef GL_ARB_texture_storage
#define GL_ARB_texture_storage 1
#endif /* GL_ARB_texture_storage */

#ifndef GL_ARB_texture_storage_multisample
#define GL_ARB_texture_storage_multisample 1
#endif /* GL_ARB_texture_storage_multisample */

#ifndef GL_ARB_texture_swizzle
#define GL_ARB_texture_swizzle 1
#endif /* GL_ARB_texture_swizzle */

#ifndef GL_ARB_texture_view
#define GL_ARB_texture_view 1
#endif /* GL_ARB_texture_view */

#ifndef GL_ARB_timer_query
#define GL_ARB_timer_query 1
#endif /* GL_ARB_timer_query */

#ifndef GL_ARB_transform_feedback2
#define GL_ARB_transform_feedback2 1
#endif /* GL_ARB_transform_feedback2 */

#ifndef GL_ARB_transform_feedback3
#define GL_ARB_transform_feedback3 1
#endif /* GL_ARB_transform_feedback3 */

#ifndef GL_ARB_transform_feedback_instanced
#define GL_ARB_transform_feedback_instanced 1
#endif /* GL_ARB_transform_feedback_instanced */

#ifndef GL_ARB_transform_feedback_overflow_query
#define GL_ARB_transform_feedback_overflow_query 1
#define GL_TRANSFORM_FEEDBACK_OVERFLOW_ARB 0x82EC
#define GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW_ARB 0x82ED
#endif /* GL_ARB_transform_feedback_overflow_query */

#ifndef GL_ARB_uniform_buffer_object
#define GL_ARB_uniform_buffer_object 1
#endif /* GL_ARB_uniform_buffer_object */

#ifndef GL_ARB_vertex_array_bgra
#define GL_ARB_vertex_array_bgra 1
#endif /* GL_ARB_vertex_array_bgra */

#ifndef GL_ARB_vertex_array_object
#define GL_ARB_vertex_array_object 1
#endif /* GL_ARB_vertex_array_object */

#ifndef GL_ARB_vertex_attrib_64bit
#define GL_ARB_vertex_attrib_64bit 1
#endif /* GL_ARB_vertex_attrib_64bit */

#ifndef GL_ARB_vertex_attrib_binding
#define GL_ARB_vertex_attrib_binding 1
#endif /* GL_ARB_vertex_attrib_binding */

#ifndef GL_ARB_vertex_type_10f_11f_11f_rev
#define GL_ARB_vertex_type_10f_11f_11f_rev 1
#endif /* GL_ARB_vertex_type_10f_11f_11f_rev */

#ifndef GL_ARB_vertex_type_2_10_10_10_rev
#define GL_ARB_vertex_type_2_10_10_10_rev 1
#endif /* GL_ARB_vertex_type_2_10_10_10_rev */

#ifndef GL_ARB_viewport_array
#define GL_ARB_viewport_array 1
typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYDVNVPROC) (GLuint first, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDDNVPROC) (GLuint index, GLdouble n, GLdouble f);
#endif /* GL_ARB_viewport_array */

#ifndef GL_KHR_texture_compression_astc_hdr
#define GL_KHR_texture_compression_astc_hdr 1
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#endif /* GL_KHR_texture_compression_astc_hdr */

#ifndef GL_KHR_texture_compression_astc_ldr
#define GL_KHR_texture_compression_astc_ldr 1
#endif /* GL_KHR_texture_compression_astc_ldr */

#ifndef GL_KHR_texture_compression_astc_sliced_3d
#define GL_KHR_texture_compression_astc_sliced_3d 1
#endif /* GL_KHR_texture_compression_astc_sliced_3d */

#ifndef GL_MESA_framebuffer_flip_x
#define GL_MESA_framebuffer_flip_x 1
#define GL_FRAMEBUFFER_FLIP_X_MESA        0x8BBC
#endif /* GL_MESA_framebuffer_flip_x */

#ifndef GL_MESA_framebuffer_flip_y
#define GL_MESA_framebuffer_flip_y 1
#define GL_FRAMEBUFFER_FLIP_Y_MESA        0x8BBB
typedef void (APIENTRYP PFNGLFRAMEBUFFERPARAMETERIMESAPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVMESAPROC) (GLenum target, GLenum pname, GLint *params);
#endif /* GL_MESA_framebuffer_flip_y */

#ifndef GL_MESA_framebuffer_swap_xy
#define GL_MESA_framebuffer_swap_xy 1
#define GL_FRAMEBUFFER_SWAP_XY_MESA       0x8BBD
#endif /* GL_MESA_framebuffer_swap_xy */

#ifndef GL_NV_conservative_raster
#define GL_NV_conservative_raster 1
#define GL_CONSERVATIVE_RASTERIZATION_NV  0x9346
#define GL_SUBPIXEL_PRECISION_BIAS_X_BITS_NV 0x9347
#define GL_SUBPIXEL_PRECISION_BIAS_Y_BITS_NV 0x9348
#define GL_MAX_SUBPIXEL_PRECISION_BIAS_BITS_NV 0x9349
typedef void (APIENTRYP PFNGLSUBPIXELPRECISIONBIASNVPROC) (GLuint xbits, GLuint ybits);
#endif /* GL_NV_conservative_raster */

#ifndef GL_NV_conservative_raster_dilate
#define GL_NV_conservative_raster_dilate 1
#define GL_CONSERVATIVE_RASTER_DILATE_NV  0x9379
#define GL_CONSERVATIVE_RASTER_DILATE_RANGE_NV 0x937A
#define GL_CONSERVATIVE_RASTER_DILATE_GRANULARITY_NV 0x937B
typedef void (APIENTRYP PFNGLCONSERVATIVERASTERPARAMETERFNVPROC) (GLenum pname, GLfloat value);
#endif /* GL_NV_conservative_raster_dilate */

#ifndef GL_NV_conservative_raster_pre_snap
#define GL_NV_conservative_raster_pre_snap 1
#define GL_CONSERVATIVE_RASTER_MODE_PRE_SNAP_NV 0x9550
#endif /* GL_NV_conservative_raster_pre_snap */

#ifndef GL_NV_conservative_raster_pre_snap_triangles
#define GL_NV_conservative_raster_pre_snap_triangles 1
#define GL_CONSERVATIVE_RASTER_MODE_NV    0x954D
#define GL_CONSERVATIVE_RASTER_MODE_POST_SNAP_NV 0x954E
#define GL_CONSERVATIVE_RASTER_MODE_PRE_SNAP_TRIANGLES_NV 0x954F
typedef void (APIENTRYP PFNGLCONSERVATIVERASTERPARAMETERINVPROC) (GLenum pname, GLint param);
#endif /* GL_NV_conservative_raster_pre_snap_triangles */

#ifndef GL_NV_conservative_raster_underestimation
#define GL_NV_conservative_raster_underestimation 1
#endif /* GL_NV_conservative_raster_underestimation */

#ifdef __cplusplus
}
#endif


#if __cplusplus>=201703L||(defined(_MSVC_LANG)&&_MSVC_LANG>=201703L)
	#define e(t) inline bool GX_##t = false;
#endif

e(ARB_ES2_compatibility)
e(ARB_ES3_1_compatibility)
e(ARB_ES3_2_compatibility)
e(ARB_ES3_compatibility)
e(ARB_arrays_of_arrays)
e(ARB_base_instance)
e(ARB_bindless_texture)
e(ARB_blend_func_extended)
e(ARB_buffer_storage)
e(ARB_cl_event)
e(ARB_clear_buffer_object)
e(ARB_clear_texture)
e(ARB_clip_control)
e(ARB_compressed_texture_pixel_storage)
e(ARB_compute_shader)
e(ARB_compute_variable_group_size)
e(ARB_conditional_render_inverted)
e(ARB_conservative_depth)
e(ARB_copy_buffer)
e(ARB_copy_image)
e(ARB_cull_distance)
e(ARB_debug_output)
e(ARB_depth_buffer_float)
e(ARB_depth_clamp)
e(ARB_derivative_control)
e(ARB_direct_state_access)
e(ARB_draw_buffers_blend)
e(ARB_draw_elements_base_vertex)
e(ARB_draw_indirect)
e(ARB_draw_instanced)
e(ARB_enhanced_layouts)
e(ARB_explicit_attrib_location)
e(ARB_explicit_uniform_location)
e(ARB_fragment_coord_conventions)
e(ARB_fragment_layer_viewport)
e(ARB_fragment_shader_interlock)
e(ARB_framebuffer_no_attachments)
e(ARB_framebuffer_object)
e(ARB_framebuffer_sRGB)
e(ARB_geometry_shader4)
e(ARB_get_program_binary)
e(ARB_get_texture_sub_image)
e(ARB_gl_spirv)
e(ARB_gpu_shader5)
e(ARB_gpu_shader_fp64)
e(ARB_gpu_shader_int64)
e(ARB_half_float_vertex)
e(ARB_imaging)
e(ARB_indirect_parameters)
e(ARB_instanced_arrays)
e(ARB_internalformat_query)
e(ARB_internalformat_query2)
e(ARB_invalidate_subdata)
e(ARB_map_buffer_alignment)
e(ARB_map_buffer_range)
e(ARB_multi_bind)
e(ARB_multi_draw_indirect)
e(ARB_occlusion_query2)
e(ARB_parallel_shader_compile)
e(ARB_pipeline_statistics_query)
e(ARB_pixel_buffer_object)
e(ARB_polygon_offset_clamp)
e(ARB_post_depth_coverage)
e(ARB_program_interface_query)
e(ARB_provoking_vertex)
e(ARB_query_buffer_object)
e(ARB_robust_buffer_access_behavior)
e(ARB_robustness)
e(ARB_robustness_isolation)
e(ARB_sample_locations)
e(ARB_sample_shading)
e(ARB_sampler_objects)
e(ARB_seamless_cube_map)
e(ARB_seamless_cubemap_per_texture)
e(ARB_separate_shader_objects)
e(ARB_shader_atomic_counter_ops)
e(ARB_shader_atomic_counters)
e(ARB_shader_ballot)
e(ARB_shader_bit_encoding)
e(ARB_shader_clock)
e(ARB_shader_draw_parameters)
e(ARB_shader_group_vote)
e(ARB_shader_image_load_store)
e(ARB_shader_image_size)
e(ARB_shader_precision)
e(ARB_shader_stencil_export)
e(ARB_shader_storage_buffer_object)
e(ARB_shader_subroutine)
e(ARB_shader_texture_image_samples)
e(ARB_shader_viewport_layer_array)
e(ARB_shading_language_420pack)
e(ARB_shading_language_include)
e(ARB_shading_language_packing)
e(ARB_sparse_buffer)
e(ARB_sparse_texture)
e(ARB_sparse_texture2)
e(ARB_sparse_texture_clamp)
e(ARB_spirv_extensions)
e(ARB_stencil_texturing)
e(ARB_sync)
e(ARB_tessellation_shader)
e(ARB_texture_barrier)
e(ARB_texture_border_clamp)
e(ARB_texture_buffer_object)
e(ARB_texture_buffer_object_rgb32)
e(ARB_texture_buffer_range)
e(ARB_texture_compression_bptc)
e(ARB_texture_compression_rgtc)
e(ARB_texture_cube_map_array)
e(ARB_texture_filter_anisotropic)
e(ARB_texture_filter_minmax)
e(ARB_texture_gather)
e(ARB_texture_mirror_clamp_to_edge)
e(ARB_texture_mirrored_repeat)
e(ARB_texture_multisample)
e(ARB_texture_non_power_of_two)
e(ARB_texture_query_levels)
e(ARB_texture_query_lod)
e(ARB_texture_rg)
e(ARB_texture_rgb10_a2ui)
e(ARB_texture_stencil8)
e(ARB_texture_storage)
e(ARB_texture_storage_multisample)
e(ARB_texture_swizzle)
e(ARB_texture_view)
e(ARB_timer_query)
e(ARB_transform_feedback2)
e(ARB_transform_feedback3)
e(ARB_transform_feedback_instanced)
e(ARB_transform_feedback_overflow_query)
e(ARB_uniform_buffer_object)
e(ARB_vertex_array_bgra)
e(ARB_vertex_array_object)
e(ARB_vertex_attrib_64bit)
e(ARB_vertex_attrib_binding)
e(ARB_vertex_type_10f_11f_11f_rev)
e(ARB_vertex_type_2_10_10_10_rev)
e(ARB_viewport_array)
e(KHR_texture_compression_astc_hdr)
e(KHR_texture_compression_astc_ldr)
e(KHR_texture_compression_astc_sliced_3d)
e(MESA_framebuffer_flip_x)
e(MESA_framebuffer_flip_y)
e(MESA_framebuffer_swap_xy)
e(NV_conservative_raster)
e(NV_conservative_raster_dilate)
e(NV_conservative_raster_pre_snap)
e(NV_conservative_raster_pre_snap_triangles)
e(NV_conservative_raster_underestimation)
#undef e

#if __cplusplus>=201703L||(defined(_MSVC_LANG)&&_MSVC_LANG>=201703L)
	#define f(proc,func) inline PFNGL##proc##PROC gl##func = 0;
#endif

f(CULLFACE,CullFace)
f(FRONTFACE,FrontFace)
f(HINT,Hint)
f(LINEWIDTH,LineWidth)
f(POINTSIZE,PointSize)
f(POLYGONMODE,PolygonMode)
f(SCISSOR,Scissor)
f(TEXPARAMETERF,TexParameterf)
f(TEXPARAMETERFV,TexParameterfv)
f(TEXPARAMETERI,TexParameteri)
f(TEXPARAMETERIV,TexParameteriv)
f(TEXIMAGE1D,TexImage1D)
f(TEXIMAGE2D,TexImage2D)
f(DRAWBUFFER,DrawBuffer)
f(CLEAR,Clear)
f(CLEARCOLOR,ClearColor)
f(CLEARSTENCIL,ClearStencil)
f(CLEARDEPTH,ClearDepth)
f(STENCILMASK,StencilMask)
f(COLORMASK,ColorMask)
f(DEPTHMASK,DepthMask)
f(DISABLE,Disable)
f(ENABLE,Enable)
f(FINISH,Finish)
f(FLUSH,Flush)
f(BLENDFUNC,BlendFunc)
f(LOGICOP,LogicOp)
f(STENCILFUNC,StencilFunc)
f(STENCILOP,StencilOp)
f(DEPTHFUNC,DepthFunc)
f(PIXELSTOREF,PixelStoref)
f(PIXELSTOREI,PixelStorei)
f(READBUFFER,ReadBuffer)
f(READPIXELS,ReadPixels)
f(GETBOOLEANV,GetBooleanv)
f(GETDOUBLEV,GetDoublev)
f(GETERROR,GetError)
f(GETFLOATV,GetFloatv)
f(GETINTEGERV,GetIntegerv)
f(GETSTRING,GetString)
f(GETTEXIMAGE,GetTexImage)
f(GETTEXPARAMETERFV,GetTexParameterfv)
f(GETTEXPARAMETERIV,GetTexParameteriv)
f(GETTEXLEVELPARAMETERFV,GetTexLevelParameterfv)
f(GETTEXLEVELPARAMETERIV,GetTexLevelParameteriv)
f(ISENABLED,IsEnabled)
f(DEPTHRANGE,DepthRange)
f(VIEWPORT,Viewport)
f(DRAWARRAYS,DrawArrays)
f(DRAWELEMENTS,DrawElements)
f(GETPOINTERV,GetPointerv)
f(POLYGONOFFSET,PolygonOffset)
f(COPYTEXIMAGE1D,CopyTexImage1D)
f(COPYTEXIMAGE2D,CopyTexImage2D)
f(COPYTEXSUBIMAGE1D,CopyTexSubImage1D)
f(COPYTEXSUBIMAGE2D,CopyTexSubImage2D)
f(TEXSUBIMAGE1D,TexSubImage1D)
f(TEXSUBIMAGE2D,TexSubImage2D)
f(BINDTEXTURE,BindTexture)
f(DELETETEXTURES,DeleteTextures)
f(GENTEXTURES,GenTextures)
f(ISTEXTURE,IsTexture)
f(DRAWRANGEELEMENTS,DrawRangeElements)
f(TEXIMAGE3D,TexImage3D)
f(TEXSUBIMAGE3D,TexSubImage3D)
f(COPYTEXSUBIMAGE3D,CopyTexSubImage3D)
f(ACTIVETEXTURE,ActiveTexture)
f(SAMPLECOVERAGE,SampleCoverage)
f(COMPRESSEDTEXIMAGE3D,CompressedTexImage3D)
f(COMPRESSEDTEXIMAGE2D,CompressedTexImage2D)
f(COMPRESSEDTEXIMAGE1D,CompressedTexImage1D)
f(COMPRESSEDTEXSUBIMAGE3D,CompressedTexSubImage3D)
f(COMPRESSEDTEXSUBIMAGE2D,CompressedTexSubImage2D)
f(COMPRESSEDTEXSUBIMAGE1D,CompressedTexSubImage1D)
f(GETCOMPRESSEDTEXIMAGE,GetCompressedTexImage)
f(BLENDFUNCSEPARATE,BlendFuncSeparate)
f(MULTIDRAWARRAYS,MultiDrawArrays)
f(MULTIDRAWELEMENTS,MultiDrawElements)
f(POINTPARAMETERF,PointParameterf)
f(POINTPARAMETERFV,PointParameterfv)
f(POINTPARAMETERI,PointParameteri)
f(POINTPARAMETERIV,PointParameteriv)
f(BLENDCOLOR,BlendColor)
f(BLENDEQUATION,BlendEquation)
f(GENQUERIES,GenQueries)
f(DELETEQUERIES,DeleteQueries)
f(ISQUERY,IsQuery)
f(BEGINQUERY,BeginQuery)
f(ENDQUERY,EndQuery)
f(GETQUERYIV,GetQueryiv)
f(GETQUERYOBJECTIV,GetQueryObjectiv)
f(GETQUERYOBJECTUIV,GetQueryObjectuiv)
f(BINDBUFFER,BindBuffer)
f(DELETEBUFFERS,DeleteBuffers)
f(GENBUFFERS,GenBuffers)
f(ISBUFFER,IsBuffer)
f(BUFFERDATA,BufferData)
f(BUFFERSUBDATA,BufferSubData)
f(GETBUFFERSUBDATA,GetBufferSubData)
f(MAPBUFFER,MapBuffer)
f(UNMAPBUFFER,UnmapBuffer)
f(GETBUFFERPARAMETERIV,GetBufferParameteriv)
f(GETBUFFERPOINTERV,GetBufferPointerv)
f(BLENDEQUATIONSEPARATE,BlendEquationSeparate)
f(DRAWBUFFERS,DrawBuffers)
f(STENCILOPSEPARATE,StencilOpSeparate)
f(STENCILFUNCSEPARATE,StencilFuncSeparate)
f(STENCILMASKSEPARATE,StencilMaskSeparate)
f(ATTACHSHADER,AttachShader)
f(BINDATTRIBLOCATION,BindAttribLocation)
f(COMPILESHADER,CompileShader)
f(CREATEPROGRAM,CreateProgram)
f(CREATESHADER,CreateShader)
f(DELETEPROGRAM,DeleteProgram)
f(DELETESHADER,DeleteShader)
f(DETACHSHADER,DetachShader)
f(DISABLEVERTEXATTRIBARRAY,DisableVertexAttribArray)
f(ENABLEVERTEXATTRIBARRAY,EnableVertexAttribArray)
f(GETACTIVEATTRIB,GetActiveAttrib)
f(GETACTIVEUNIFORM,GetActiveUniform)
f(GETATTACHEDSHADERS,GetAttachedShaders)
f(GETATTRIBLOCATION,GetAttribLocation)
f(GETPROGRAMIV,GetProgramiv)
f(GETPROGRAMINFOLOG,GetProgramInfoLog)
f(GETSHADERIV,GetShaderiv)
f(GETSHADERINFOLOG,GetShaderInfoLog)
f(GETSHADERSOURCE,GetShaderSource)
f(GETUNIFORMLOCATION,GetUniformLocation)
f(GETUNIFORMFV,GetUniformfv)
f(GETUNIFORMIV,GetUniformiv)
f(GETVERTEXATTRIBDV,GetVertexAttribdv)
f(GETVERTEXATTRIBFV,GetVertexAttribfv)
f(GETVERTEXATTRIBIV,GetVertexAttribiv)
f(GETVERTEXATTRIBPOINTERV,GetVertexAttribPointerv)
f(ISPROGRAM,IsProgram)
f(ISSHADER,IsShader)
f(LINKPROGRAM,LinkProgram)
f(SHADERSOURCE,ShaderSource)
f(USEPROGRAM,UseProgram)
f(UNIFORM1F,Uniform1f)
f(UNIFORM2F,Uniform2f)
f(UNIFORM3F,Uniform3f)
f(UNIFORM4F,Uniform4f)
f(UNIFORM1I,Uniform1i)
f(UNIFORM2I,Uniform2i)
f(UNIFORM3I,Uniform3i)
f(UNIFORM4I,Uniform4i)
f(UNIFORM1FV,Uniform1fv)
f(UNIFORM2FV,Uniform2fv)
f(UNIFORM3FV,Uniform3fv)
f(UNIFORM4FV,Uniform4fv)
f(UNIFORM1IV,Uniform1iv)
f(UNIFORM2IV,Uniform2iv)
f(UNIFORM3IV,Uniform3iv)
f(UNIFORM4IV,Uniform4iv)
f(UNIFORMMATRIX2FV,UniformMatrix2fv)
f(UNIFORMMATRIX3FV,UniformMatrix3fv)
f(UNIFORMMATRIX4FV,UniformMatrix4fv)
f(VALIDATEPROGRAM,ValidateProgram)
f(VERTEXATTRIB1D,VertexAttrib1d)
f(VERTEXATTRIB1DV,VertexAttrib1dv)
f(VERTEXATTRIB1F,VertexAttrib1f)
f(VERTEXATTRIB1FV,VertexAttrib1fv)
f(VERTEXATTRIB1S,VertexAttrib1s)
f(VERTEXATTRIB1SV,VertexAttrib1sv)
f(VERTEXATTRIB2D,VertexAttrib2d)
f(VERTEXATTRIB2DV,VertexAttrib2dv)
f(VERTEXATTRIB2F,VertexAttrib2f)
f(VERTEXATTRIB2FV,VertexAttrib2fv)
f(VERTEXATTRIB2S,VertexAttrib2s)
f(VERTEXATTRIB2SV,VertexAttrib2sv)
f(VERTEXATTRIB3D,VertexAttrib3d)
f(VERTEXATTRIB3DV,VertexAttrib3dv)
f(VERTEXATTRIB3F,VertexAttrib3f)
f(VERTEXATTRIB3FV,VertexAttrib3fv)
f(VERTEXATTRIB3S,VertexAttrib3s)
f(VERTEXATTRIB3SV,VertexAttrib3sv)
f(VERTEXATTRIB4NBV,VertexAttrib4Nbv)
f(VERTEXATTRIB4NIV,VertexAttrib4Niv)
f(VERTEXATTRIB4NSV,VertexAttrib4Nsv)
f(VERTEXATTRIB4NUB,VertexAttrib4Nub)
f(VERTEXATTRIB4NUBV,VertexAttrib4Nubv)
f(VERTEXATTRIB4NUIV,VertexAttrib4Nuiv)
f(VERTEXATTRIB4NUSV,VertexAttrib4Nusv)
f(VERTEXATTRIB4BV,VertexAttrib4bv)
f(VERTEXATTRIB4D,VertexAttrib4d)
f(VERTEXATTRIB4DV,VertexAttrib4dv)
f(VERTEXATTRIB4F,VertexAttrib4f)
f(VERTEXATTRIB4FV,VertexAttrib4fv)
f(VERTEXATTRIB4IV,VertexAttrib4iv)
f(VERTEXATTRIB4S,VertexAttrib4s)
f(VERTEXATTRIB4SV,VertexAttrib4sv)
f(VERTEXATTRIB4UBV,VertexAttrib4ubv)
f(VERTEXATTRIB4UIV,VertexAttrib4uiv)
f(VERTEXATTRIB4USV,VertexAttrib4usv)
f(VERTEXATTRIBPOINTER,VertexAttribPointer)
f(UNIFORMMATRIX2X3FV,UniformMatrix2x3fv)
f(UNIFORMMATRIX3X2FV,UniformMatrix3x2fv)
f(UNIFORMMATRIX2X4FV,UniformMatrix2x4fv)
f(UNIFORMMATRIX4X2FV,UniformMatrix4x2fv)
f(UNIFORMMATRIX3X4FV,UniformMatrix3x4fv)
f(UNIFORMMATRIX4X3FV,UniformMatrix4x3fv)
f(COLORMASKI,ColorMaski)
f(GETBOOLEANI_V,GetBooleani_v)
f(GETINTEGERI_V,GetIntegeri_v)
f(ENABLEI,Enablei)
f(DISABLEI,Disablei)
f(ISENABLEDI,IsEnabledi)
f(BEGINTRANSFORMFEEDBACK,BeginTransformFeedback)
f(ENDTRANSFORMFEEDBACK,EndTransformFeedback)
f(BINDBUFFERRANGE,BindBufferRange)
f(BINDBUFFERBASE,BindBufferBase)
f(TRANSFORMFEEDBACKVARYINGS,TransformFeedbackVaryings)
f(GETTRANSFORMFEEDBACKVARYING,GetTransformFeedbackVarying)
f(CLAMPCOLOR,ClampColor)
f(BEGINCONDITIONALRENDER,BeginConditionalRender)
f(ENDCONDITIONALRENDER,EndConditionalRender)
f(VERTEXATTRIBIPOINTER,VertexAttribIPointer)
f(GETVERTEXATTRIBIIV,GetVertexAttribIiv)
f(GETVERTEXATTRIBIUIV,GetVertexAttribIuiv)
f(VERTEXATTRIBI1I,VertexAttribI1i)
f(VERTEXATTRIBI2I,VertexAttribI2i)
f(VERTEXATTRIBI3I,VertexAttribI3i)
f(VERTEXATTRIBI4I,VertexAttribI4i)
f(VERTEXATTRIBI1UI,VertexAttribI1ui)
f(VERTEXATTRIBI2UI,VertexAttribI2ui)
f(VERTEXATTRIBI3UI,VertexAttribI3ui)
f(VERTEXATTRIBI4UI,VertexAttribI4ui)
f(VERTEXATTRIBI1IV,VertexAttribI1iv)
f(VERTEXATTRIBI2IV,VertexAttribI2iv)
f(VERTEXATTRIBI3IV,VertexAttribI3iv)
f(VERTEXATTRIBI4IV,VertexAttribI4iv)
f(VERTEXATTRIBI1UIV,VertexAttribI1uiv)
f(VERTEXATTRIBI2UIV,VertexAttribI2uiv)
f(VERTEXATTRIBI3UIV,VertexAttribI3uiv)
f(VERTEXATTRIBI4UIV,VertexAttribI4uiv)
f(VERTEXATTRIBI4BV,VertexAttribI4bv)
f(VERTEXATTRIBI4SV,VertexAttribI4sv)
f(VERTEXATTRIBI4UBV,VertexAttribI4ubv)
f(VERTEXATTRIBI4USV,VertexAttribI4usv)
f(GETUNIFORMUIV,GetUniformuiv)
f(BINDFRAGDATALOCATION,BindFragDataLocation)
f(GETFRAGDATALOCATION,GetFragDataLocation)
f(UNIFORM1UI,Uniform1ui)
f(UNIFORM2UI,Uniform2ui)
f(UNIFORM3UI,Uniform3ui)
f(UNIFORM4UI,Uniform4ui)
f(UNIFORM1UIV,Uniform1uiv)
f(UNIFORM2UIV,Uniform2uiv)
f(UNIFORM3UIV,Uniform3uiv)
f(UNIFORM4UIV,Uniform4uiv)
f(TEXPARAMETERIIV,TexParameterIiv)
f(TEXPARAMETERIUIV,TexParameterIuiv)
f(GETTEXPARAMETERIIV,GetTexParameterIiv)
f(GETTEXPARAMETERIUIV,GetTexParameterIuiv)
f(CLEARBUFFERIV,ClearBufferiv)
f(CLEARBUFFERUIV,ClearBufferuiv)
f(CLEARBUFFERFV,ClearBufferfv)
f(CLEARBUFFERFI,ClearBufferfi)
f(GETSTRINGI,GetStringi)
f(ISRENDERBUFFER,IsRenderbuffer)
f(BINDRENDERBUFFER,BindRenderbuffer)
f(DELETERENDERBUFFERS,DeleteRenderbuffers)
f(GENRENDERBUFFERS,GenRenderbuffers)
f(RENDERBUFFERSTORAGE,RenderbufferStorage)
f(GETRENDERBUFFERPARAMETERIV,GetRenderbufferParameteriv)
f(ISFRAMEBUFFER,IsFramebuffer)
f(BINDFRAMEBUFFER,BindFramebuffer)
f(DELETEFRAMEBUFFERS,DeleteFramebuffers)
f(GENFRAMEBUFFERS,GenFramebuffers)
f(CHECKFRAMEBUFFERSTATUS,CheckFramebufferStatus)
f(FRAMEBUFFERTEXTURE1D,FramebufferTexture1D)
f(FRAMEBUFFERTEXTURE2D,FramebufferTexture2D)
f(FRAMEBUFFERTEXTURE3D,FramebufferTexture3D)
f(FRAMEBUFFERRENDERBUFFER,FramebufferRenderbuffer)
f(GETFRAMEBUFFERATTACHMENTPARAMETERIV,GetFramebufferAttachmentParameteriv)
f(GENERATEMIPMAP,GenerateMipmap)
f(BLITFRAMEBUFFER,BlitFramebuffer)
f(RENDERBUFFERSTORAGEMULTISAMPLE,RenderbufferStorageMultisample)
f(FRAMEBUFFERTEXTURELAYER,FramebufferTextureLayer)
f(MAPBUFFERRANGE,MapBufferRange)
f(FLUSHMAPPEDBUFFERRANGE,FlushMappedBufferRange)
f(BINDVERTEXARRAY,BindVertexArray)
f(DELETEVERTEXARRAYS,DeleteVertexArrays)
f(GENVERTEXARRAYS,GenVertexArrays)
f(ISVERTEXARRAY,IsVertexArray)
f(DRAWARRAYSINSTANCED,DrawArraysInstanced)
f(DRAWELEMENTSINSTANCED,DrawElementsInstanced)
f(TEXBUFFER,TexBuffer)
f(PRIMITIVERESTARTINDEX,PrimitiveRestartIndex)
f(COPYBUFFERSUBDATA,CopyBufferSubData)
f(GETUNIFORMINDICES,GetUniformIndices)
f(GETACTIVEUNIFORMSIV,GetActiveUniformsiv)
f(GETACTIVEUNIFORMNAME,GetActiveUniformName)
f(GETUNIFORMBLOCKINDEX,GetUniformBlockIndex)
f(GETACTIVEUNIFORMBLOCKIV,GetActiveUniformBlockiv)
f(GETACTIVEUNIFORMBLOCKNAME,GetActiveUniformBlockName)
f(UNIFORMBLOCKBINDING,UniformBlockBinding)
f(DRAWELEMENTSBASEVERTEX,DrawElementsBaseVertex)
f(DRAWRANGEELEMENTSBASEVERTEX,DrawRangeElementsBaseVertex)
f(DRAWELEMENTSINSTANCEDBASEVERTEX,DrawElementsInstancedBaseVertex)
f(MULTIDRAWELEMENTSBASEVERTEX,MultiDrawElementsBaseVertex)
f(PROVOKINGVERTEX,ProvokingVertex)
f(FENCESYNC,FenceSync)
f(ISSYNC,IsSync)
f(DELETESYNC,DeleteSync)
f(CLIENTWAITSYNC,ClientWaitSync)
f(WAITSYNC,WaitSync)
f(GETINTEGER64V,GetInteger64v)
f(GETSYNCIV,GetSynciv)
f(GETINTEGER64I_V,GetInteger64i_v)
f(GETBUFFERPARAMETERI64V,GetBufferParameteri64v)
f(FRAMEBUFFERTEXTURE,FramebufferTexture)
f(TEXIMAGE2DMULTISAMPLE,TexImage2DMultisample)
f(TEXIMAGE3DMULTISAMPLE,TexImage3DMultisample)
f(GETMULTISAMPLEFV,GetMultisamplefv)
f(SAMPLEMASKI,SampleMaski)
f(BINDFRAGDATALOCATIONINDEXED,BindFragDataLocationIndexed)
f(GETFRAGDATAINDEX,GetFragDataIndex)
f(GENSAMPLERS,GenSamplers)
f(DELETESAMPLERS,DeleteSamplers)
f(ISSAMPLER,IsSampler)
f(BINDSAMPLER,BindSampler)
f(SAMPLERPARAMETERI,SamplerParameteri)
f(SAMPLERPARAMETERIV,SamplerParameteriv)
f(SAMPLERPARAMETERF,SamplerParameterf)
f(SAMPLERPARAMETERFV,SamplerParameterfv)
f(SAMPLERPARAMETERIIV,SamplerParameterIiv)
f(SAMPLERPARAMETERIUIV,SamplerParameterIuiv)
f(GETSAMPLERPARAMETERIV,GetSamplerParameteriv)
f(GETSAMPLERPARAMETERIIV,GetSamplerParameterIiv)
f(GETSAMPLERPARAMETERFV,GetSamplerParameterfv)
f(GETSAMPLERPARAMETERIUIV,GetSamplerParameterIuiv)
f(QUERYCOUNTER,QueryCounter)
f(GETQUERYOBJECTI64V,GetQueryObjecti64v)
f(GETQUERYOBJECTUI64V,GetQueryObjectui64v)
f(VERTEXATTRIBDIVISOR,VertexAttribDivisor)
f(VERTEXATTRIBP1UI,VertexAttribP1ui)
f(VERTEXATTRIBP1UIV,VertexAttribP1uiv)
f(VERTEXATTRIBP2UI,VertexAttribP2ui)
f(VERTEXATTRIBP2UIV,VertexAttribP2uiv)
f(VERTEXATTRIBP3UI,VertexAttribP3ui)
f(VERTEXATTRIBP3UIV,VertexAttribP3uiv)
f(VERTEXATTRIBP4UI,VertexAttribP4ui)
f(VERTEXATTRIBP4UIV,VertexAttribP4uiv)
f(MINSAMPLESHADING,MinSampleShading)
f(BLENDEQUATIONI,BlendEquationi)
f(BLENDEQUATIONSEPARATEI,BlendEquationSeparatei)
f(BLENDFUNCI,BlendFunci)
f(BLENDFUNCSEPARATEI,BlendFuncSeparatei)
f(DRAWARRAYSINDIRECT,DrawArraysIndirect)
f(DRAWELEMENTSINDIRECT,DrawElementsIndirect)
f(UNIFORM1D,Uniform1d)
f(UNIFORM2D,Uniform2d)
f(UNIFORM3D,Uniform3d)
f(UNIFORM4D,Uniform4d)
f(UNIFORM1DV,Uniform1dv)
f(UNIFORM2DV,Uniform2dv)
f(UNIFORM3DV,Uniform3dv)
f(UNIFORM4DV,Uniform4dv)
f(UNIFORMMATRIX2DV,UniformMatrix2dv)
f(UNIFORMMATRIX3DV,UniformMatrix3dv)
f(UNIFORMMATRIX4DV,UniformMatrix4dv)
f(UNIFORMMATRIX2X3DV,UniformMatrix2x3dv)
f(UNIFORMMATRIX2X4DV,UniformMatrix2x4dv)
f(UNIFORMMATRIX3X2DV,UniformMatrix3x2dv)
f(UNIFORMMATRIX3X4DV,UniformMatrix3x4dv)
f(UNIFORMMATRIX4X2DV,UniformMatrix4x2dv)
f(UNIFORMMATRIX4X3DV,UniformMatrix4x3dv)
f(GETUNIFORMDV,GetUniformdv)
f(GETSUBROUTINEUNIFORMLOCATION,GetSubroutineUniformLocation)
f(GETSUBROUTINEINDEX,GetSubroutineIndex)
f(GETACTIVESUBROUTINEUNIFORMIV,GetActiveSubroutineUniformiv)
f(GETACTIVESUBROUTINEUNIFORMNAME,GetActiveSubroutineUniformName)
f(GETACTIVESUBROUTINENAME,GetActiveSubroutineName)
f(UNIFORMSUBROUTINESUIV,UniformSubroutinesuiv)
f(GETUNIFORMSUBROUTINEUIV,GetUniformSubroutineuiv)
f(GETPROGRAMSTAGEIV,GetProgramStageiv)
f(PATCHPARAMETERI,PatchParameteri)
f(PATCHPARAMETERFV,PatchParameterfv)
f(BINDTRANSFORMFEEDBACK,BindTransformFeedback)
f(DELETETRANSFORMFEEDBACKS,DeleteTransformFeedbacks)
f(GENTRANSFORMFEEDBACKS,GenTransformFeedbacks)
f(ISTRANSFORMFEEDBACK,IsTransformFeedback)
f(PAUSETRANSFORMFEEDBACK,PauseTransformFeedback)
f(RESUMETRANSFORMFEEDBACK,ResumeTransformFeedback)
f(DRAWTRANSFORMFEEDBACK,DrawTransformFeedback)
f(DRAWTRANSFORMFEEDBACKSTREAM,DrawTransformFeedbackStream)
f(BEGINQUERYINDEXED,BeginQueryIndexed)
f(ENDQUERYINDEXED,EndQueryIndexed)
f(GETQUERYINDEXEDIV,GetQueryIndexediv)
f(RELEASESHADERCOMPILER,ReleaseShaderCompiler)
f(SHADERBINARY,ShaderBinary)
f(GETSHADERPRECISIONFORMAT,GetShaderPrecisionFormat)
f(DEPTHRANGEF,DepthRangef)
f(CLEARDEPTHF,ClearDepthf)
f(GETPROGRAMBINARY,GetProgramBinary)
f(PROGRAMBINARY,ProgramBinary)
f(PROGRAMPARAMETERI,ProgramParameteri)
f(USEPROGRAMSTAGES,UseProgramStages)
f(ACTIVESHADERPROGRAM,ActiveShaderProgram)
f(CREATESHADERPROGRAMV,CreateShaderProgramv)
f(BINDPROGRAMPIPELINE,BindProgramPipeline)
f(DELETEPROGRAMPIPELINES,DeleteProgramPipelines)
f(GENPROGRAMPIPELINES,GenProgramPipelines)
f(ISPROGRAMPIPELINE,IsProgramPipeline)
f(GETPROGRAMPIPELINEIV,GetProgramPipelineiv)
f(PROGRAMUNIFORM1I,ProgramUniform1i)
f(PROGRAMUNIFORM1IV,ProgramUniform1iv)
f(PROGRAMUNIFORM1F,ProgramUniform1f)
f(PROGRAMUNIFORM1FV,ProgramUniform1fv)
f(PROGRAMUNIFORM1D,ProgramUniform1d)
f(PROGRAMUNIFORM1DV,ProgramUniform1dv)
f(PROGRAMUNIFORM1UI,ProgramUniform1ui)
f(PROGRAMUNIFORM1UIV,ProgramUniform1uiv)
f(PROGRAMUNIFORM2I,ProgramUniform2i)
f(PROGRAMUNIFORM2IV,ProgramUniform2iv)
f(PROGRAMUNIFORM2F,ProgramUniform2f)
f(PROGRAMUNIFORM2FV,ProgramUniform2fv)
f(PROGRAMUNIFORM2D,ProgramUniform2d)
f(PROGRAMUNIFORM2DV,ProgramUniform2dv)
f(PROGRAMUNIFORM2UI,ProgramUniform2ui)
f(PROGRAMUNIFORM2UIV,ProgramUniform2uiv)
f(PROGRAMUNIFORM3I,ProgramUniform3i)
f(PROGRAMUNIFORM3IV,ProgramUniform3iv)
f(PROGRAMUNIFORM3F,ProgramUniform3f)
f(PROGRAMUNIFORM3FV,ProgramUniform3fv)
f(PROGRAMUNIFORM3D,ProgramUniform3d)
f(PROGRAMUNIFORM3DV,ProgramUniform3dv)
f(PROGRAMUNIFORM3UI,ProgramUniform3ui)
f(PROGRAMUNIFORM3UIV,ProgramUniform3uiv)
f(PROGRAMUNIFORM4I,ProgramUniform4i)
f(PROGRAMUNIFORM4IV,ProgramUniform4iv)
f(PROGRAMUNIFORM4F,ProgramUniform4f)
f(PROGRAMUNIFORM4FV,ProgramUniform4fv)
f(PROGRAMUNIFORM4D,ProgramUniform4d)
f(PROGRAMUNIFORM4DV,ProgramUniform4dv)
f(PROGRAMUNIFORM4UI,ProgramUniform4ui)
f(PROGRAMUNIFORM4UIV,ProgramUniform4uiv)
f(PROGRAMUNIFORMMATRIX2FV,ProgramUniformMatrix2fv)
f(PROGRAMUNIFORMMATRIX3FV,ProgramUniformMatrix3fv)
f(PROGRAMUNIFORMMATRIX4FV,ProgramUniformMatrix4fv)
f(PROGRAMUNIFORMMATRIX2DV,ProgramUniformMatrix2dv)
f(PROGRAMUNIFORMMATRIX3DV,ProgramUniformMatrix3dv)
f(PROGRAMUNIFORMMATRIX4DV,ProgramUniformMatrix4dv)
f(PROGRAMUNIFORMMATRIX2X3FV,ProgramUniformMatrix2x3fv)
f(PROGRAMUNIFORMMATRIX3X2FV,ProgramUniformMatrix3x2fv)
f(PROGRAMUNIFORMMATRIX2X4FV,ProgramUniformMatrix2x4fv)
f(PROGRAMUNIFORMMATRIX4X2FV,ProgramUniformMatrix4x2fv)
f(PROGRAMUNIFORMMATRIX3X4FV,ProgramUniformMatrix3x4fv)
f(PROGRAMUNIFORMMATRIX4X3FV,ProgramUniformMatrix4x3fv)
f(PROGRAMUNIFORMMATRIX2X3DV,ProgramUniformMatrix2x3dv)
f(PROGRAMUNIFORMMATRIX3X2DV,ProgramUniformMatrix3x2dv)
f(PROGRAMUNIFORMMATRIX2X4DV,ProgramUniformMatrix2x4dv)
f(PROGRAMUNIFORMMATRIX4X2DV,ProgramUniformMatrix4x2dv)
f(PROGRAMUNIFORMMATRIX3X4DV,ProgramUniformMatrix3x4dv)
f(PROGRAMUNIFORMMATRIX4X3DV,ProgramUniformMatrix4x3dv)
f(VALIDATEPROGRAMPIPELINE,ValidateProgramPipeline)
f(GETPROGRAMPIPELINEINFOLOG,GetProgramPipelineInfoLog)
f(VERTEXATTRIBL1D,VertexAttribL1d)
f(VERTEXATTRIBL2D,VertexAttribL2d)
f(VERTEXATTRIBL3D,VertexAttribL3d)
f(VERTEXATTRIBL4D,VertexAttribL4d)
f(VERTEXATTRIBL1DV,VertexAttribL1dv)
f(VERTEXATTRIBL2DV,VertexAttribL2dv)
f(VERTEXATTRIBL3DV,VertexAttribL3dv)
f(VERTEXATTRIBL4DV,VertexAttribL4dv)
f(VERTEXATTRIBLPOINTER,VertexAttribLPointer)
f(GETVERTEXATTRIBLDV,GetVertexAttribLdv)
f(VIEWPORTARRAYV,ViewportArrayv)
f(VIEWPORTINDEXEDF,ViewportIndexedf)
f(VIEWPORTINDEXEDFV,ViewportIndexedfv)
f(SCISSORARRAYV,ScissorArrayv)
f(SCISSORINDEXED,ScissorIndexed)
f(SCISSORINDEXEDV,ScissorIndexedv)
f(DEPTHRANGEARRAYV,DepthRangeArrayv)
f(DEPTHRANGEINDEXED,DepthRangeIndexed)
f(GETFLOATI_V,GetFloati_v)
f(GETDOUBLEI_V,GetDoublei_v)
f(DRAWARRAYSINSTANCEDBASEINSTANCE,DrawArraysInstancedBaseInstance)
f(DRAWELEMENTSINSTANCEDBASEINSTANCE,DrawElementsInstancedBaseInstance)
f(DRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE,DrawElementsInstancedBaseVertexBaseInstance)
f(GETINTERNALFORMATIV,GetInternalformativ)
f(GETACTIVEATOMICCOUNTERBUFFERIV,GetActiveAtomicCounterBufferiv)
f(BINDIMAGETEXTURE,BindImageTexture)
f(MEMORYBARRIER,MemoryBarrier)
f(TEXSTORAGE1D,TexStorage1D)
f(TEXSTORAGE2D,TexStorage2D)
f(TEXSTORAGE3D,TexStorage3D)
f(DRAWTRANSFORMFEEDBACKINSTANCED,DrawTransformFeedbackInstanced)
f(DRAWTRANSFORMFEEDBACKSTREAMINSTANCED,DrawTransformFeedbackStreamInstanced)
f(CLEARBUFFERDATA,ClearBufferData)
f(CLEARBUFFERSUBDATA,ClearBufferSubData)
f(DISPATCHCOMPUTE,DispatchCompute)
f(DISPATCHCOMPUTEINDIRECT,DispatchComputeIndirect)
f(COPYIMAGESUBDATA,CopyImageSubData)
f(FRAMEBUFFERPARAMETERI,FramebufferParameteri)
f(GETFRAMEBUFFERPARAMETERIV,GetFramebufferParameteriv)
f(GETINTERNALFORMATI64V,GetInternalformati64v)
f(INVALIDATETEXSUBIMAGE,InvalidateTexSubImage)
f(INVALIDATETEXIMAGE,InvalidateTexImage)
f(INVALIDATEBUFFERSUBDATA,InvalidateBufferSubData)
f(INVALIDATEBUFFERDATA,InvalidateBufferData)
f(INVALIDATEFRAMEBUFFER,InvalidateFramebuffer)
f(INVALIDATESUBFRAMEBUFFER,InvalidateSubFramebuffer)
f(MULTIDRAWARRAYSINDIRECT,MultiDrawArraysIndirect)
f(MULTIDRAWELEMENTSINDIRECT,MultiDrawElementsIndirect)
f(GETPROGRAMINTERFACEIV,GetProgramInterfaceiv)
f(GETPROGRAMRESOURCEINDEX,GetProgramResourceIndex)
f(GETPROGRAMRESOURCENAME,GetProgramResourceName)
f(GETPROGRAMRESOURCEIV,GetProgramResourceiv)
f(GETPROGRAMRESOURCELOCATION,GetProgramResourceLocation)
f(GETPROGRAMRESOURCELOCATIONINDEX,GetProgramResourceLocationIndex)
f(SHADERSTORAGEBLOCKBINDING,ShaderStorageBlockBinding)
f(TEXBUFFERRANGE,TexBufferRange)
f(TEXSTORAGE2DMULTISAMPLE,TexStorage2DMultisample)
f(TEXSTORAGE3DMULTISAMPLE,TexStorage3DMultisample)
f(TEXTUREVIEW,TextureView)
f(BINDVERTEXBUFFER,BindVertexBuffer)
f(VERTEXATTRIBFORMAT,VertexAttribFormat)
f(VERTEXATTRIBIFORMAT,VertexAttribIFormat)
f(VERTEXATTRIBLFORMAT,VertexAttribLFormat)
f(VERTEXATTRIBBINDING,VertexAttribBinding)
f(VERTEXBINDINGDIVISOR,VertexBindingDivisor)
f(DEBUGMESSAGECONTROL,DebugMessageControl)
f(DEBUGMESSAGEINSERT,DebugMessageInsert)
f(DEBUGMESSAGECALLBACK,DebugMessageCallback)
f(GETDEBUGMESSAGELOG,GetDebugMessageLog)
f(PUSHDEBUGGROUP,PushDebugGroup)
f(POPDEBUGGROUP,PopDebugGroup)
f(OBJECTLABEL,ObjectLabel)
f(GETOBJECTLABEL,GetObjectLabel)
f(OBJECTPTRLABEL,ObjectPtrLabel)
f(GETOBJECTPTRLABEL,GetObjectPtrLabel)
f(BUFFERSTORAGE,BufferStorage)
f(CLEARTEXIMAGE,ClearTexImage)
f(CLEARTEXSUBIMAGE,ClearTexSubImage)
f(BINDBUFFERSBASE,BindBuffersBase)
f(BINDBUFFERSRANGE,BindBuffersRange)
f(BINDTEXTURES,BindTextures)
f(BINDSAMPLERS,BindSamplers)
f(BINDIMAGETEXTURES,BindImageTextures)
f(BINDVERTEXBUFFERS,BindVertexBuffers)
f(CLIPCONTROL,ClipControl)
f(CREATETRANSFORMFEEDBACKS,CreateTransformFeedbacks)
f(TRANSFORMFEEDBACKBUFFERBASE,TransformFeedbackBufferBase)
f(TRANSFORMFEEDBACKBUFFERRANGE,TransformFeedbackBufferRange)
f(GETTRANSFORMFEEDBACKIV,GetTransformFeedbackiv)
f(GETTRANSFORMFEEDBACKI_V,GetTransformFeedbacki_v)
f(GETTRANSFORMFEEDBACKI64_V,GetTransformFeedbacki64_v)
f(CREATEBUFFERS,CreateBuffers)
f(NAMEDBUFFERSTORAGE,NamedBufferStorage)
f(NAMEDBUFFERDATA,NamedBufferData)
f(NAMEDBUFFERSUBDATA,NamedBufferSubData)
f(COPYNAMEDBUFFERSUBDATA,CopyNamedBufferSubData)
f(CLEARNAMEDBUFFERDATA,ClearNamedBufferData)
f(CLEARNAMEDBUFFERSUBDATA,ClearNamedBufferSubData)
f(MAPNAMEDBUFFER,MapNamedBuffer)
f(MAPNAMEDBUFFERRANGE,MapNamedBufferRange)
f(UNMAPNAMEDBUFFER,UnmapNamedBuffer)
f(FLUSHMAPPEDNAMEDBUFFERRANGE,FlushMappedNamedBufferRange)
f(GETNAMEDBUFFERPARAMETERIV,GetNamedBufferParameteriv)
f(GETNAMEDBUFFERPARAMETERI64V,GetNamedBufferParameteri64v)
f(GETNAMEDBUFFERPOINTERV,GetNamedBufferPointerv)
f(GETNAMEDBUFFERSUBDATA,GetNamedBufferSubData)
f(CREATEFRAMEBUFFERS,CreateFramebuffers)
f(NAMEDFRAMEBUFFERRENDERBUFFER,NamedFramebufferRenderbuffer)
f(NAMEDFRAMEBUFFERPARAMETERI,NamedFramebufferParameteri)
f(NAMEDFRAMEBUFFERTEXTURE,NamedFramebufferTexture)
f(NAMEDFRAMEBUFFERTEXTURELAYER,NamedFramebufferTextureLayer)
f(NAMEDFRAMEBUFFERDRAWBUFFER,NamedFramebufferDrawBuffer)
f(NAMEDFRAMEBUFFERDRAWBUFFERS,NamedFramebufferDrawBuffers)
f(NAMEDFRAMEBUFFERREADBUFFER,NamedFramebufferReadBuffer)
f(INVALIDATENAMEDFRAMEBUFFERDATA,InvalidateNamedFramebufferData)
f(INVALIDATENAMEDFRAMEBUFFERSUBDATA,InvalidateNamedFramebufferSubData)
f(CLEARNAMEDFRAMEBUFFERIV,ClearNamedFramebufferiv)
f(CLEARNAMEDFRAMEBUFFERUIV,ClearNamedFramebufferuiv)
f(CLEARNAMEDFRAMEBUFFERFV,ClearNamedFramebufferfv)
f(CLEARNAMEDFRAMEBUFFERFI,ClearNamedFramebufferfi)
f(BLITNAMEDFRAMEBUFFER,BlitNamedFramebuffer)
f(CHECKNAMEDFRAMEBUFFERSTATUS,CheckNamedFramebufferStatus)
f(GETNAMEDFRAMEBUFFERPARAMETERIV,GetNamedFramebufferParameteriv)
f(GETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIV,GetNamedFramebufferAttachmentParameteriv)
f(CREATERENDERBUFFERS,CreateRenderbuffers)
f(NAMEDRENDERBUFFERSTORAGE,NamedRenderbufferStorage)
f(NAMEDRENDERBUFFERSTORAGEMULTISAMPLE,NamedRenderbufferStorageMultisample)
f(GETNAMEDRENDERBUFFERPARAMETERIV,GetNamedRenderbufferParameteriv)
f(CREATETEXTURES,CreateTextures)
f(TEXTUREBUFFER,TextureBuffer)
f(TEXTUREBUFFERRANGE,TextureBufferRange)
f(TEXTURESTORAGE1D,TextureStorage1D)
f(TEXTURESTORAGE2D,TextureStorage2D)
f(TEXTURESTORAGE3D,TextureStorage3D)
f(TEXTURESTORAGE2DMULTISAMPLE,TextureStorage2DMultisample)
f(TEXTURESTORAGE3DMULTISAMPLE,TextureStorage3DMultisample)
f(TEXTURESUBIMAGE1D,TextureSubImage1D)
f(TEXTURESUBIMAGE2D,TextureSubImage2D)
f(TEXTURESUBIMAGE3D,TextureSubImage3D)
f(COMPRESSEDTEXTURESUBIMAGE1D,CompressedTextureSubImage1D)
f(COMPRESSEDTEXTURESUBIMAGE2D,CompressedTextureSubImage2D)
f(COMPRESSEDTEXTURESUBIMAGE3D,CompressedTextureSubImage3D)
f(COPYTEXTURESUBIMAGE1D,CopyTextureSubImage1D)
f(COPYTEXTURESUBIMAGE2D,CopyTextureSubImage2D)
f(COPYTEXTURESUBIMAGE3D,CopyTextureSubImage3D)
f(TEXTUREPARAMETERF,TextureParameterf)
f(TEXTUREPARAMETERFV,TextureParameterfv)
f(TEXTUREPARAMETERI,TextureParameteri)
f(TEXTUREPARAMETERIIV,TextureParameterIiv)
f(TEXTUREPARAMETERIUIV,TextureParameterIuiv)
f(TEXTUREPARAMETERIV,TextureParameteriv)
f(GENERATETEXTUREMIPMAP,GenerateTextureMipmap)
f(BINDTEXTUREUNIT,BindTextureUnit)
f(GETTEXTUREIMAGE,GetTextureImage)
f(GETCOMPRESSEDTEXTUREIMAGE,GetCompressedTextureImage)
f(GETTEXTURELEVELPARAMETERFV,GetTextureLevelParameterfv)
f(GETTEXTURELEVELPARAMETERIV,GetTextureLevelParameteriv)
f(GETTEXTUREPARAMETERFV,GetTextureParameterfv)
f(GETTEXTUREPARAMETERIIV,GetTextureParameterIiv)
f(GETTEXTUREPARAMETERIUIV,GetTextureParameterIuiv)
f(GETTEXTUREPARAMETERIV,GetTextureParameteriv)
f(CREATEVERTEXARRAYS,CreateVertexArrays)
f(DISABLEVERTEXARRAYATTRIB,DisableVertexArrayAttrib)
f(ENABLEVERTEXARRAYATTRIB,EnableVertexArrayAttrib)
f(VERTEXARRAYELEMENTBUFFER,VertexArrayElementBuffer)
f(VERTEXARRAYVERTEXBUFFER,VertexArrayVertexBuffer)
f(VERTEXARRAYVERTEXBUFFERS,VertexArrayVertexBuffers)
f(VERTEXARRAYATTRIBBINDING,VertexArrayAttribBinding)
f(VERTEXARRAYATTRIBFORMAT,VertexArrayAttribFormat)
f(VERTEXARRAYATTRIBIFORMAT,VertexArrayAttribIFormat)
f(VERTEXARRAYATTRIBLFORMAT,VertexArrayAttribLFormat)
f(VERTEXARRAYBINDINGDIVISOR,VertexArrayBindingDivisor)
f(GETVERTEXARRAYIV,GetVertexArrayiv)
f(GETVERTEXARRAYINDEXEDIV,GetVertexArrayIndexediv)
f(GETVERTEXARRAYINDEXED64IV,GetVertexArrayIndexed64iv)
f(CREATESAMPLERS,CreateSamplers)
f(CREATEPROGRAMPIPELINES,CreateProgramPipelines)
f(CREATEQUERIES,CreateQueries)
f(GETQUERYBUFFEROBJECTI64V,GetQueryBufferObjecti64v)
f(GETQUERYBUFFEROBJECTIV,GetQueryBufferObjectiv)
f(GETQUERYBUFFEROBJECTUI64V,GetQueryBufferObjectui64v)
f(GETQUERYBUFFEROBJECTUIV,GetQueryBufferObjectuiv)
f(MEMORYBARRIERBYREGION,MemoryBarrierByRegion)
f(GETTEXTURESUBIMAGE,GetTextureSubImage)
f(GETCOMPRESSEDTEXTURESUBIMAGE,GetCompressedTextureSubImage)
f(GETGRAPHICSRESETSTATUS,GetGraphicsResetStatus)
f(GETNCOMPRESSEDTEXIMAGE,GetnCompressedTexImage)
f(GETNTEXIMAGE,GetnTexImage)
f(GETNUNIFORMDV,GetnUniformdv)
f(GETNUNIFORMFV,GetnUniformfv)
f(GETNUNIFORMIV,GetnUniformiv)
f(GETNUNIFORMUIV,GetnUniformuiv)
f(READNPIXELS,ReadnPixels)
f(TEXTUREBARRIER,TextureBarrier)
f(SPECIALIZESHADER,SpecializeShader)
f(MULTIDRAWARRAYSINDIRECTCOUNT,MultiDrawArraysIndirectCount)
f(MULTIDRAWELEMENTSINDIRECTCOUNT,MultiDrawElementsIndirectCount)
f(POLYGONOFFSETCLAMP,PolygonOffsetClamp)
f(PRIMITIVEBOUNDINGBOXARB,PrimitiveBoundingBoxARB)
f(GETTEXTUREHANDLEARB,GetTextureHandleARB)
f(GETTEXTURESAMPLERHANDLEARB,GetTextureSamplerHandleARB)
f(MAKETEXTUREHANDLERESIDENTARB,MakeTextureHandleResidentARB)
f(MAKETEXTUREHANDLENONRESIDENTARB,MakeTextureHandleNonResidentARB)
f(GETIMAGEHANDLEARB,GetImageHandleARB)
f(MAKEIMAGEHANDLERESIDENTARB,MakeImageHandleResidentARB)
f(MAKEIMAGEHANDLENONRESIDENTARB,MakeImageHandleNonResidentARB)
f(UNIFORMHANDLEUI64ARB,UniformHandleui64ARB)
f(UNIFORMHANDLEUI64VARB,UniformHandleui64vARB)
f(PROGRAMUNIFORMHANDLEUI64ARB,ProgramUniformHandleui64ARB)
f(PROGRAMUNIFORMHANDLEUI64VARB,ProgramUniformHandleui64vARB)
f(ISTEXTUREHANDLERESIDENTARB,IsTextureHandleResidentARB)
f(ISIMAGEHANDLERESIDENTARB,IsImageHandleResidentARB)
f(VERTEXATTRIBL1UI64ARB,VertexAttribL1ui64ARB)
f(VERTEXATTRIBL1UI64VARB,VertexAttribL1ui64vARB)
f(GETVERTEXATTRIBLUI64VARB,GetVertexAttribLui64vARB)
f(DISPATCHCOMPUTEGROUPSIZEARB,DispatchComputeGroupSizeARB)
f(DEBUGMESSAGECONTROLARB,DebugMessageControlARB)
f(DEBUGMESSAGEINSERTARB,DebugMessageInsertARB)
f(DEBUGMESSAGECALLBACKARB,DebugMessageCallbackARB)
f(GETDEBUGMESSAGELOGARB,GetDebugMessageLogARB)
f(BLENDEQUATIONIARB,BlendEquationiARB)
f(BLENDEQUATIONSEPARATEIARB,BlendEquationSeparateiARB)
f(BLENDFUNCIARB,BlendFunciARB)
f(BLENDFUNCSEPARATEIARB,BlendFuncSeparateiARB)
f(DRAWARRAYSINSTANCEDARB,DrawArraysInstancedARB)
f(DRAWELEMENTSINSTANCEDARB,DrawElementsInstancedARB)
f(PROGRAMPARAMETERIARB,ProgramParameteriARB)
f(FRAMEBUFFERTEXTUREARB,FramebufferTextureARB)
f(FRAMEBUFFERTEXTURELAYERARB,FramebufferTextureLayerARB)
f(FRAMEBUFFERTEXTUREFACEARB,FramebufferTextureFaceARB)
f(SPECIALIZESHADERARB,SpecializeShaderARB)
f(UNIFORM1I64ARB,Uniform1i64ARB)
f(UNIFORM2I64ARB,Uniform2i64ARB)
f(UNIFORM3I64ARB,Uniform3i64ARB)
f(UNIFORM4I64ARB,Uniform4i64ARB)
f(UNIFORM1I64VARB,Uniform1i64vARB)
f(UNIFORM2I64VARB,Uniform2i64vARB)
f(UNIFORM3I64VARB,Uniform3i64vARB)
f(UNIFORM4I64VARB,Uniform4i64vARB)
f(UNIFORM1UI64ARB,Uniform1ui64ARB)
f(UNIFORM2UI64ARB,Uniform2ui64ARB)
f(UNIFORM3UI64ARB,Uniform3ui64ARB)
f(UNIFORM4UI64ARB,Uniform4ui64ARB)
f(UNIFORM1UI64VARB,Uniform1ui64vARB)
f(UNIFORM2UI64VARB,Uniform2ui64vARB)
f(UNIFORM3UI64VARB,Uniform3ui64vARB)
f(UNIFORM4UI64VARB,Uniform4ui64vARB)
f(GETUNIFORMI64VARB,GetUniformi64vARB)
f(GETUNIFORMUI64VARB,GetUniformui64vARB)
f(GETNUNIFORMI64VARB,GetnUniformi64vARB)
f(GETNUNIFORMUI64VARB,GetnUniformui64vARB)
f(PROGRAMUNIFORM1I64ARB,ProgramUniform1i64ARB)
f(PROGRAMUNIFORM2I64ARB,ProgramUniform2i64ARB)
f(PROGRAMUNIFORM3I64ARB,ProgramUniform3i64ARB)
f(PROGRAMUNIFORM4I64ARB,ProgramUniform4i64ARB)
f(PROGRAMUNIFORM1I64VARB,ProgramUniform1i64vARB)
f(PROGRAMUNIFORM2I64VARB,ProgramUniform2i64vARB)
f(PROGRAMUNIFORM3I64VARB,ProgramUniform3i64vARB)
f(PROGRAMUNIFORM4I64VARB,ProgramUniform4i64vARB)
f(PROGRAMUNIFORM1UI64ARB,ProgramUniform1ui64ARB)
f(PROGRAMUNIFORM2UI64ARB,ProgramUniform2ui64ARB)
f(PROGRAMUNIFORM3UI64ARB,ProgramUniform3ui64ARB)
f(PROGRAMUNIFORM4UI64ARB,ProgramUniform4ui64ARB)
f(PROGRAMUNIFORM1UI64VARB,ProgramUniform1ui64vARB)
f(PROGRAMUNIFORM2UI64VARB,ProgramUniform2ui64vARB)
f(PROGRAMUNIFORM3UI64VARB,ProgramUniform3ui64vARB)
f(PROGRAMUNIFORM4UI64VARB,ProgramUniform4ui64vARB)
f(MULTIDRAWARRAYSINDIRECTCOUNTARB,MultiDrawArraysIndirectCountARB)
f(MULTIDRAWELEMENTSINDIRECTCOUNTARB,MultiDrawElementsIndirectCountARB)
f(VERTEXATTRIBDIVISORARB,VertexAttribDivisorARB)
f(MAXSHADERCOMPILERTHREADSARB,MaxShaderCompilerThreadsARB)
f(GETGRAPHICSRESETSTATUSARB,GetGraphicsResetStatusARB)
f(GETNTEXIMAGEARB,GetnTexImageARB)
f(READNPIXELSARB,ReadnPixelsARB)
f(GETNCOMPRESSEDTEXIMAGEARB,GetnCompressedTexImageARB)
f(GETNUNIFORMFVARB,GetnUniformfvARB)
f(GETNUNIFORMIVARB,GetnUniformivARB)
f(GETNUNIFORMUIVARB,GetnUniformuivARB)
f(GETNUNIFORMDVARB,GetnUniformdvARB)
f(FRAMEBUFFERSAMPLELOCATIONSFVARB,FramebufferSampleLocationsfvARB)
f(NAMEDFRAMEBUFFERSAMPLELOCATIONSFVARB,NamedFramebufferSampleLocationsfvARB)
f(EVALUATEDEPTHVALUESARB,EvaluateDepthValuesARB)
f(MINSAMPLESHADINGARB,MinSampleShadingARB)
f(NAMEDSTRINGARB,NamedStringARB)
f(DELETENAMEDSTRINGARB,DeleteNamedStringARB)
f(COMPILESHADERINCLUDEARB,CompileShaderIncludeARB)
f(ISNAMEDSTRINGARB,IsNamedStringARB)
f(GETNAMEDSTRINGARB,GetNamedStringARB)
f(GETNAMEDSTRINGIVARB,GetNamedStringivARB)
f(BUFFERPAGECOMMITMENTARB,BufferPageCommitmentARB)
f(NAMEDBUFFERPAGECOMMITMENTEXT,NamedBufferPageCommitmentEXT)
f(TEXPAGECOMMITMENTARB,TexPageCommitmentARB)
f(TEXBUFFERARB,TexBufferARB)
f(DEPTHRANGEARRAYDVNV,DepthRangeArraydvNV)
f(DEPTHRANGEINDEXEDDNV,DepthRangeIndexeddNV)
f(FRAMEBUFFERPARAMETERIMESA,FramebufferParameteriMESA)
f(GETFRAMEBUFFERPARAMETERIVMESA,GetFramebufferParameterivMESA)
f(SUBPIXELPRECISIONBIASNV,SubpixelPrecisionBiasNV)
f(CONSERVATIVERASTERPARAMETERFNV,ConservativeRasterParameterfNV)
f(CONSERVATIVERASTERPARAMETERINV,ConservativeRasterParameteriNV)
#undef f

#if !defined __msvc__ && defined _WIN32 && defined _MSC_VER && !defined __clang__
	#define __msvc__
#elif !defined __gcc__ && defined __GNUC__
	#define __gcc__
#endif

#ifdef __msvc__
	#define __noinline __declspec(noinline) inline
#elif defined __gcc__
	#define __noinline __attribute__((noinline)) inline
	#include <dlfcn.h>
#endif

#include <string>
#include <vector>
#include <unordered_set>

using std::string;
using std::vector;

inline void* __hOpenGL32 = nullptr;
inline std::vector<std::string> gxcorearb_unsupported_functions;
inline std::vector<std::string> gxcorearb_unsupported_extensions;
#ifdef __msvc__
inline PROC (* __wglGetProcAddress)(const char*) = nullptr;
inline void* (*__xglGetProcAddress)(const char*) = nullptr;
inline const char** (*__xglGetExtensions)(int*) = nullptr;
#endif

__noinline void* get_gxcorearb_proc( const char* fname )
{
	void* p = nullptr;;
#ifdef __msvc__
	p = __xglGetProcAddress?__xglGetProcAddress(fname):nullptr;	if(p) return p;
	p = (void*) __wglGetProcAddress(fname);						if(p) return p;
	p = (void*) GetProcAddress((HMODULE)__hOpenGL32,fname);		if(p) return p;
#elif defined __gcc__
	p = (void*) dlsym(__hOpenGL32,fname);						if(p) return p;
#endif
	gxcorearb_unsupported_functions.emplace_back(fname);		return p;
}

__noinline std::unordered_set<std::string>& get_gxcorearb_extensions()
{
	static std::unordered_set<std::string> extension_set; if(!extension_set.empty()) return extension_set;
#ifdef __msvc__
	if(__xglGetExtensions)
	{
		int kn; const char** p_extensions = __xglGetExtensions(&kn);
		if(kn){ for(int k=0;k<kn;k++) extension_set.emplace(p_extensions[k]); return extension_set; }
	}
#endif
#ifdef GL_ES_VERSION_2_0
	char* e=(char*)glGetString(GL_EXTENSIONS); std::vector<char> ext(e,e+strlen(e)+2);
	for(char* t=strtok(&ext[0]," \t\n" );t;t=strtok(nullptr," \t\n")) extension_set.emplace(t);
#else
	int kn; glGetIntegerv(GL_NUM_EXTENSIONS,&kn);
	for(int k=0;k<kn;k++) extension_set.emplace((char*)glGetStringi(GL_EXTENSIONS,k));
#endif
	return extension_set;
}

__noinline bool gxcorearb_extension_exists( const char* extension )
{
	static const std::unordered_set<std::string>& extension_set = get_gxcorearb_extensions();
	if(extension_set.find(extension)!=extension_set.end()) return true;
	gxcorearb_unsupported_extensions.emplace_back(extension); return false;
}

__noinline void gxcorearb( void* hOpenGL32 )
{
#ifdef __msvc__
	__hOpenGL32 = hOpenGL32; if(__hOpenGL32==nullptr) __hOpenGL32=LoadLibraryA("OpenGL32.dll"); if(__hOpenGL32==nullptr){ printf( "gxcorearb(): unable to load OpenGL32.dll" ); return; }
	__wglGetProcAddress = (decltype(__wglGetProcAddress)) GetProcAddress((HMODULE)__hOpenGL32,"wglGetProcAddress"); if(__wglGetProcAddress==nullptr){ printf( "gxcorearb(): __wglGetProcAddress==nullptr" ); return; }
	__xglGetProcAddress = (decltype(__xglGetProcAddress)) GetProcAddress(GetModuleHandleW(nullptr),"xglGetProcAddress");
	__xglGetExtensions = (decltype(__xglGetExtensions)) GetProcAddress(GetModuleHandleW(nullptr),"xglGetExtensions");
#elif defined __gcc__
	__hOpenGL32 = hOpenGL32; if(__hOpenGL32==nullptr) __hOpenGL32=dlopen("libGL.so",RTLD_LAZY); if(__hOpenGL32==nullptr){ printf( "gxcorearb(): unable to load libGL.so" ); return; }
#endif
// query if functions exist
#define g(proc,func) gl##func = (PFNGL##proc##PROC) get_gxcorearb_proc( "gl" #func );
g(CULLFACE,CullFace)
g(FRONTFACE,FrontFace)
g(HINT,Hint)
g(LINEWIDTH,LineWidth)
g(POINTSIZE,PointSize)
g(POLYGONMODE,PolygonMode)
g(SCISSOR,Scissor)
g(TEXPARAMETERF,TexParameterf)
g(TEXPARAMETERFV,TexParameterfv)
g(TEXPARAMETERI,TexParameteri)
g(TEXPARAMETERIV,TexParameteriv)
g(TEXIMAGE1D,TexImage1D)
g(TEXIMAGE2D,TexImage2D)
g(DRAWBUFFER,DrawBuffer)
g(CLEAR,Clear)
g(CLEARCOLOR,ClearColor)
g(CLEARSTENCIL,ClearStencil)
g(CLEARDEPTH,ClearDepth)
g(STENCILMASK,StencilMask)
g(COLORMASK,ColorMask)
g(DEPTHMASK,DepthMask)
g(DISABLE,Disable)
g(ENABLE,Enable)
g(FINISH,Finish)
g(FLUSH,Flush)
g(BLENDFUNC,BlendFunc)
g(LOGICOP,LogicOp)
g(STENCILFUNC,StencilFunc)
g(STENCILOP,StencilOp)
g(DEPTHFUNC,DepthFunc)
g(PIXELSTOREF,PixelStoref)
g(PIXELSTOREI,PixelStorei)
g(READBUFFER,ReadBuffer)
g(READPIXELS,ReadPixels)
g(GETBOOLEANV,GetBooleanv)
g(GETDOUBLEV,GetDoublev)
g(GETERROR,GetError)
g(GETFLOATV,GetFloatv)
g(GETINTEGERV,GetIntegerv)
g(GETSTRING,GetString)
g(GETTEXIMAGE,GetTexImage)
g(GETTEXPARAMETERFV,GetTexParameterfv)
g(GETTEXPARAMETERIV,GetTexParameteriv)
g(GETTEXLEVELPARAMETERFV,GetTexLevelParameterfv)
g(GETTEXLEVELPARAMETERIV,GetTexLevelParameteriv)
g(ISENABLED,IsEnabled)
g(DEPTHRANGE,DepthRange)
g(VIEWPORT,Viewport)
g(DRAWARRAYS,DrawArrays)
g(DRAWELEMENTS,DrawElements)
g(GETPOINTERV,GetPointerv)
g(POLYGONOFFSET,PolygonOffset)
g(COPYTEXIMAGE1D,CopyTexImage1D)
g(COPYTEXIMAGE2D,CopyTexImage2D)
g(COPYTEXSUBIMAGE1D,CopyTexSubImage1D)
g(COPYTEXSUBIMAGE2D,CopyTexSubImage2D)
g(TEXSUBIMAGE1D,TexSubImage1D)
g(TEXSUBIMAGE2D,TexSubImage2D)
g(BINDTEXTURE,BindTexture)
g(DELETETEXTURES,DeleteTextures)
g(GENTEXTURES,GenTextures)
g(ISTEXTURE,IsTexture)
g(DRAWRANGEELEMENTS,DrawRangeElements)
g(TEXIMAGE3D,TexImage3D)
g(TEXSUBIMAGE3D,TexSubImage3D)
g(COPYTEXSUBIMAGE3D,CopyTexSubImage3D)
g(ACTIVETEXTURE,ActiveTexture)
g(SAMPLECOVERAGE,SampleCoverage)
g(COMPRESSEDTEXIMAGE3D,CompressedTexImage3D)
g(COMPRESSEDTEXIMAGE2D,CompressedTexImage2D)
g(COMPRESSEDTEXIMAGE1D,CompressedTexImage1D)
g(COMPRESSEDTEXSUBIMAGE3D,CompressedTexSubImage3D)
g(COMPRESSEDTEXSUBIMAGE2D,CompressedTexSubImage2D)
g(COMPRESSEDTEXSUBIMAGE1D,CompressedTexSubImage1D)
g(GETCOMPRESSEDTEXIMAGE,GetCompressedTexImage)
g(BLENDFUNCSEPARATE,BlendFuncSeparate)
g(MULTIDRAWARRAYS,MultiDrawArrays)
g(MULTIDRAWELEMENTS,MultiDrawElements)
g(POINTPARAMETERF,PointParameterf)
g(POINTPARAMETERFV,PointParameterfv)
g(POINTPARAMETERI,PointParameteri)
g(POINTPARAMETERIV,PointParameteriv)
g(BLENDCOLOR,BlendColor)
g(BLENDEQUATION,BlendEquation)
g(GENQUERIES,GenQueries)
g(DELETEQUERIES,DeleteQueries)
g(ISQUERY,IsQuery)
g(BEGINQUERY,BeginQuery)
g(ENDQUERY,EndQuery)
g(GETQUERYIV,GetQueryiv)
g(GETQUERYOBJECTIV,GetQueryObjectiv)
g(GETQUERYOBJECTUIV,GetQueryObjectuiv)
g(BINDBUFFER,BindBuffer)
g(DELETEBUFFERS,DeleteBuffers)
g(GENBUFFERS,GenBuffers)
g(ISBUFFER,IsBuffer)
g(BUFFERDATA,BufferData)
g(BUFFERSUBDATA,BufferSubData)
g(GETBUFFERSUBDATA,GetBufferSubData)
g(MAPBUFFER,MapBuffer)
g(UNMAPBUFFER,UnmapBuffer)
g(GETBUFFERPARAMETERIV,GetBufferParameteriv)
g(GETBUFFERPOINTERV,GetBufferPointerv)
g(BLENDEQUATIONSEPARATE,BlendEquationSeparate)
g(DRAWBUFFERS,DrawBuffers)
g(STENCILOPSEPARATE,StencilOpSeparate)
g(STENCILFUNCSEPARATE,StencilFuncSeparate)
g(STENCILMASKSEPARATE,StencilMaskSeparate)
g(ATTACHSHADER,AttachShader)
g(BINDATTRIBLOCATION,BindAttribLocation)
g(COMPILESHADER,CompileShader)
g(CREATEPROGRAM,CreateProgram)
g(CREATESHADER,CreateShader)
g(DELETEPROGRAM,DeleteProgram)
g(DELETESHADER,DeleteShader)
g(DETACHSHADER,DetachShader)
g(DISABLEVERTEXATTRIBARRAY,DisableVertexAttribArray)
g(ENABLEVERTEXATTRIBARRAY,EnableVertexAttribArray)
g(GETACTIVEATTRIB,GetActiveAttrib)
g(GETACTIVEUNIFORM,GetActiveUniform)
g(GETATTACHEDSHADERS,GetAttachedShaders)
g(GETATTRIBLOCATION,GetAttribLocation)
g(GETPROGRAMIV,GetProgramiv)
g(GETPROGRAMINFOLOG,GetProgramInfoLog)
g(GETSHADERIV,GetShaderiv)
g(GETSHADERINFOLOG,GetShaderInfoLog)
g(GETSHADERSOURCE,GetShaderSource)
g(GETUNIFORMLOCATION,GetUniformLocation)
g(GETUNIFORMFV,GetUniformfv)
g(GETUNIFORMIV,GetUniformiv)
g(GETVERTEXATTRIBDV,GetVertexAttribdv)
g(GETVERTEXATTRIBFV,GetVertexAttribfv)
g(GETVERTEXATTRIBIV,GetVertexAttribiv)
g(GETVERTEXATTRIBPOINTERV,GetVertexAttribPointerv)
g(ISPROGRAM,IsProgram)
g(ISSHADER,IsShader)
g(LINKPROGRAM,LinkProgram)
g(SHADERSOURCE,ShaderSource)
g(USEPROGRAM,UseProgram)
g(UNIFORM1F,Uniform1f)
g(UNIFORM2F,Uniform2f)
g(UNIFORM3F,Uniform3f)
g(UNIFORM4F,Uniform4f)
g(UNIFORM1I,Uniform1i)
g(UNIFORM2I,Uniform2i)
g(UNIFORM3I,Uniform3i)
g(UNIFORM4I,Uniform4i)
g(UNIFORM1FV,Uniform1fv)
g(UNIFORM2FV,Uniform2fv)
g(UNIFORM3FV,Uniform3fv)
g(UNIFORM4FV,Uniform4fv)
g(UNIFORM1IV,Uniform1iv)
g(UNIFORM2IV,Uniform2iv)
g(UNIFORM3IV,Uniform3iv)
g(UNIFORM4IV,Uniform4iv)
g(UNIFORMMATRIX2FV,UniformMatrix2fv)
g(UNIFORMMATRIX3FV,UniformMatrix3fv)
g(UNIFORMMATRIX4FV,UniformMatrix4fv)
g(VALIDATEPROGRAM,ValidateProgram)
g(VERTEXATTRIB1D,VertexAttrib1d)
g(VERTEXATTRIB1DV,VertexAttrib1dv)
g(VERTEXATTRIB1F,VertexAttrib1f)
g(VERTEXATTRIB1FV,VertexAttrib1fv)
g(VERTEXATTRIB1S,VertexAttrib1s)
g(VERTEXATTRIB1SV,VertexAttrib1sv)
g(VERTEXATTRIB2D,VertexAttrib2d)
g(VERTEXATTRIB2DV,VertexAttrib2dv)
g(VERTEXATTRIB2F,VertexAttrib2f)
g(VERTEXATTRIB2FV,VertexAttrib2fv)
g(VERTEXATTRIB2S,VertexAttrib2s)
g(VERTEXATTRIB2SV,VertexAttrib2sv)
g(VERTEXATTRIB3D,VertexAttrib3d)
g(VERTEXATTRIB3DV,VertexAttrib3dv)
g(VERTEXATTRIB3F,VertexAttrib3f)
g(VERTEXATTRIB3FV,VertexAttrib3fv)
g(VERTEXATTRIB3S,VertexAttrib3s)
g(VERTEXATTRIB3SV,VertexAttrib3sv)
g(VERTEXATTRIB4NBV,VertexAttrib4Nbv)
g(VERTEXATTRIB4NIV,VertexAttrib4Niv)
g(VERTEXATTRIB4NSV,VertexAttrib4Nsv)
g(VERTEXATTRIB4NUB,VertexAttrib4Nub)
g(VERTEXATTRIB4NUBV,VertexAttrib4Nubv)
g(VERTEXATTRIB4NUIV,VertexAttrib4Nuiv)
g(VERTEXATTRIB4NUSV,VertexAttrib4Nusv)
g(VERTEXATTRIB4BV,VertexAttrib4bv)
g(VERTEXATTRIB4D,VertexAttrib4d)
g(VERTEXATTRIB4DV,VertexAttrib4dv)
g(VERTEXATTRIB4F,VertexAttrib4f)
g(VERTEXATTRIB4FV,VertexAttrib4fv)
g(VERTEXATTRIB4IV,VertexAttrib4iv)
g(VERTEXATTRIB4S,VertexAttrib4s)
g(VERTEXATTRIB4SV,VertexAttrib4sv)
g(VERTEXATTRIB4UBV,VertexAttrib4ubv)
g(VERTEXATTRIB4UIV,VertexAttrib4uiv)
g(VERTEXATTRIB4USV,VertexAttrib4usv)
g(VERTEXATTRIBPOINTER,VertexAttribPointer)
g(UNIFORMMATRIX2X3FV,UniformMatrix2x3fv)
g(UNIFORMMATRIX3X2FV,UniformMatrix3x2fv)
g(UNIFORMMATRIX2X4FV,UniformMatrix2x4fv)
g(UNIFORMMATRIX4X2FV,UniformMatrix4x2fv)
g(UNIFORMMATRIX3X4FV,UniformMatrix3x4fv)
g(UNIFORMMATRIX4X3FV,UniformMatrix4x3fv)
g(COLORMASKI,ColorMaski)
g(GETBOOLEANI_V,GetBooleani_v)
g(GETINTEGERI_V,GetIntegeri_v)
g(ENABLEI,Enablei)
g(DISABLEI,Disablei)
g(ISENABLEDI,IsEnabledi)
g(BEGINTRANSFORMFEEDBACK,BeginTransformFeedback)
g(ENDTRANSFORMFEEDBACK,EndTransformFeedback)
g(BINDBUFFERRANGE,BindBufferRange)
g(BINDBUFFERBASE,BindBufferBase)
g(TRANSFORMFEEDBACKVARYINGS,TransformFeedbackVaryings)
g(GETTRANSFORMFEEDBACKVARYING,GetTransformFeedbackVarying)
g(CLAMPCOLOR,ClampColor)
g(BEGINCONDITIONALRENDER,BeginConditionalRender)
g(ENDCONDITIONALRENDER,EndConditionalRender)
g(VERTEXATTRIBIPOINTER,VertexAttribIPointer)
g(GETVERTEXATTRIBIIV,GetVertexAttribIiv)
g(GETVERTEXATTRIBIUIV,GetVertexAttribIuiv)
g(VERTEXATTRIBI1I,VertexAttribI1i)
g(VERTEXATTRIBI2I,VertexAttribI2i)
g(VERTEXATTRIBI3I,VertexAttribI3i)
g(VERTEXATTRIBI4I,VertexAttribI4i)
g(VERTEXATTRIBI1UI,VertexAttribI1ui)
g(VERTEXATTRIBI2UI,VertexAttribI2ui)
g(VERTEXATTRIBI3UI,VertexAttribI3ui)
g(VERTEXATTRIBI4UI,VertexAttribI4ui)
g(VERTEXATTRIBI1IV,VertexAttribI1iv)
g(VERTEXATTRIBI2IV,VertexAttribI2iv)
g(VERTEXATTRIBI3IV,VertexAttribI3iv)
g(VERTEXATTRIBI4IV,VertexAttribI4iv)
g(VERTEXATTRIBI1UIV,VertexAttribI1uiv)
g(VERTEXATTRIBI2UIV,VertexAttribI2uiv)
g(VERTEXATTRIBI3UIV,VertexAttribI3uiv)
g(VERTEXATTRIBI4UIV,VertexAttribI4uiv)
g(VERTEXATTRIBI4BV,VertexAttribI4bv)
g(VERTEXATTRIBI4SV,VertexAttribI4sv)
g(VERTEXATTRIBI4UBV,VertexAttribI4ubv)
g(VERTEXATTRIBI4USV,VertexAttribI4usv)
g(GETUNIFORMUIV,GetUniformuiv)
g(BINDFRAGDATALOCATION,BindFragDataLocation)
g(GETFRAGDATALOCATION,GetFragDataLocation)
g(UNIFORM1UI,Uniform1ui)
g(UNIFORM2UI,Uniform2ui)
g(UNIFORM3UI,Uniform3ui)
g(UNIFORM4UI,Uniform4ui)
g(UNIFORM1UIV,Uniform1uiv)
g(UNIFORM2UIV,Uniform2uiv)
g(UNIFORM3UIV,Uniform3uiv)
g(UNIFORM4UIV,Uniform4uiv)
g(TEXPARAMETERIIV,TexParameterIiv)
g(TEXPARAMETERIUIV,TexParameterIuiv)
g(GETTEXPARAMETERIIV,GetTexParameterIiv)
g(GETTEXPARAMETERIUIV,GetTexParameterIuiv)
g(CLEARBUFFERIV,ClearBufferiv)
g(CLEARBUFFERUIV,ClearBufferuiv)
g(CLEARBUFFERFV,ClearBufferfv)
g(CLEARBUFFERFI,ClearBufferfi)
g(GETSTRINGI,GetStringi)
g(ISRENDERBUFFER,IsRenderbuffer)
g(BINDRENDERBUFFER,BindRenderbuffer)
g(DELETERENDERBUFFERS,DeleteRenderbuffers)
g(GENRENDERBUFFERS,GenRenderbuffers)
g(RENDERBUFFERSTORAGE,RenderbufferStorage)
g(GETRENDERBUFFERPARAMETERIV,GetRenderbufferParameteriv)
g(ISFRAMEBUFFER,IsFramebuffer)
g(BINDFRAMEBUFFER,BindFramebuffer)
g(DELETEFRAMEBUFFERS,DeleteFramebuffers)
g(GENFRAMEBUFFERS,GenFramebuffers)
g(CHECKFRAMEBUFFERSTATUS,CheckFramebufferStatus)
g(FRAMEBUFFERTEXTURE1D,FramebufferTexture1D)
g(FRAMEBUFFERTEXTURE2D,FramebufferTexture2D)
g(FRAMEBUFFERTEXTURE3D,FramebufferTexture3D)
g(FRAMEBUFFERRENDERBUFFER,FramebufferRenderbuffer)
g(GETFRAMEBUFFERATTACHMENTPARAMETERIV,GetFramebufferAttachmentParameteriv)
g(GENERATEMIPMAP,GenerateMipmap)
g(BLITFRAMEBUFFER,BlitFramebuffer)
g(RENDERBUFFERSTORAGEMULTISAMPLE,RenderbufferStorageMultisample)
g(FRAMEBUFFERTEXTURELAYER,FramebufferTextureLayer)
g(MAPBUFFERRANGE,MapBufferRange)
g(FLUSHMAPPEDBUFFERRANGE,FlushMappedBufferRange)
g(BINDVERTEXARRAY,BindVertexArray)
g(DELETEVERTEXARRAYS,DeleteVertexArrays)
g(GENVERTEXARRAYS,GenVertexArrays)
g(ISVERTEXARRAY,IsVertexArray)
g(DRAWARRAYSINSTANCED,DrawArraysInstanced)
g(DRAWELEMENTSINSTANCED,DrawElementsInstanced)
g(TEXBUFFER,TexBuffer)
g(PRIMITIVERESTARTINDEX,PrimitiveRestartIndex)
g(COPYBUFFERSUBDATA,CopyBufferSubData)
g(GETUNIFORMINDICES,GetUniformIndices)
g(GETACTIVEUNIFORMSIV,GetActiveUniformsiv)
g(GETACTIVEUNIFORMNAME,GetActiveUniformName)
g(GETUNIFORMBLOCKINDEX,GetUniformBlockIndex)
g(GETACTIVEUNIFORMBLOCKIV,GetActiveUniformBlockiv)
g(GETACTIVEUNIFORMBLOCKNAME,GetActiveUniformBlockName)
g(UNIFORMBLOCKBINDING,UniformBlockBinding)
g(DRAWELEMENTSBASEVERTEX,DrawElementsBaseVertex)
g(DRAWRANGEELEMENTSBASEVERTEX,DrawRangeElementsBaseVertex)
g(DRAWELEMENTSINSTANCEDBASEVERTEX,DrawElementsInstancedBaseVertex)
g(MULTIDRAWELEMENTSBASEVERTEX,MultiDrawElementsBaseVertex)
g(PROVOKINGVERTEX,ProvokingVertex)
g(FENCESYNC,FenceSync)
g(ISSYNC,IsSync)
g(DELETESYNC,DeleteSync)
g(CLIENTWAITSYNC,ClientWaitSync)
g(WAITSYNC,WaitSync)
g(GETINTEGER64V,GetInteger64v)
g(GETSYNCIV,GetSynciv)
g(GETINTEGER64I_V,GetInteger64i_v)
g(GETBUFFERPARAMETERI64V,GetBufferParameteri64v)
g(FRAMEBUFFERTEXTURE,FramebufferTexture)
g(TEXIMAGE2DMULTISAMPLE,TexImage2DMultisample)
g(TEXIMAGE3DMULTISAMPLE,TexImage3DMultisample)
g(GETMULTISAMPLEFV,GetMultisamplefv)
g(SAMPLEMASKI,SampleMaski)
g(BINDFRAGDATALOCATIONINDEXED,BindFragDataLocationIndexed)
g(GETFRAGDATAINDEX,GetFragDataIndex)
g(GENSAMPLERS,GenSamplers)
g(DELETESAMPLERS,DeleteSamplers)
g(ISSAMPLER,IsSampler)
g(BINDSAMPLER,BindSampler)
g(SAMPLERPARAMETERI,SamplerParameteri)
g(SAMPLERPARAMETERIV,SamplerParameteriv)
g(SAMPLERPARAMETERF,SamplerParameterf)
g(SAMPLERPARAMETERFV,SamplerParameterfv)
g(SAMPLERPARAMETERIIV,SamplerParameterIiv)
g(SAMPLERPARAMETERIUIV,SamplerParameterIuiv)
g(GETSAMPLERPARAMETERIV,GetSamplerParameteriv)
g(GETSAMPLERPARAMETERIIV,GetSamplerParameterIiv)
g(GETSAMPLERPARAMETERFV,GetSamplerParameterfv)
g(GETSAMPLERPARAMETERIUIV,GetSamplerParameterIuiv)
g(QUERYCOUNTER,QueryCounter)
g(GETQUERYOBJECTI64V,GetQueryObjecti64v)
g(GETQUERYOBJECTUI64V,GetQueryObjectui64v)
g(VERTEXATTRIBDIVISOR,VertexAttribDivisor)
g(VERTEXATTRIBP1UI,VertexAttribP1ui)
g(VERTEXATTRIBP1UIV,VertexAttribP1uiv)
g(VERTEXATTRIBP2UI,VertexAttribP2ui)
g(VERTEXATTRIBP2UIV,VertexAttribP2uiv)
g(VERTEXATTRIBP3UI,VertexAttribP3ui)
g(VERTEXATTRIBP3UIV,VertexAttribP3uiv)
g(VERTEXATTRIBP4UI,VertexAttribP4ui)
g(VERTEXATTRIBP4UIV,VertexAttribP4uiv)
g(MINSAMPLESHADING,MinSampleShading)
g(BLENDEQUATIONI,BlendEquationi)
g(BLENDEQUATIONSEPARATEI,BlendEquationSeparatei)
g(BLENDFUNCI,BlendFunci)
g(BLENDFUNCSEPARATEI,BlendFuncSeparatei)
g(DRAWARRAYSINDIRECT,DrawArraysIndirect)
g(DRAWELEMENTSINDIRECT,DrawElementsIndirect)
g(UNIFORM1D,Uniform1d)
g(UNIFORM2D,Uniform2d)
g(UNIFORM3D,Uniform3d)
g(UNIFORM4D,Uniform4d)
g(UNIFORM1DV,Uniform1dv)
g(UNIFORM2DV,Uniform2dv)
g(UNIFORM3DV,Uniform3dv)
g(UNIFORM4DV,Uniform4dv)
g(UNIFORMMATRIX2DV,UniformMatrix2dv)
g(UNIFORMMATRIX3DV,UniformMatrix3dv)
g(UNIFORMMATRIX4DV,UniformMatrix4dv)
g(UNIFORMMATRIX2X3DV,UniformMatrix2x3dv)
g(UNIFORMMATRIX2X4DV,UniformMatrix2x4dv)
g(UNIFORMMATRIX3X2DV,UniformMatrix3x2dv)
g(UNIFORMMATRIX3X4DV,UniformMatrix3x4dv)
g(UNIFORMMATRIX4X2DV,UniformMatrix4x2dv)
g(UNIFORMMATRIX4X3DV,UniformMatrix4x3dv)
g(GETUNIFORMDV,GetUniformdv)
g(GETSUBROUTINEUNIFORMLOCATION,GetSubroutineUniformLocation)
g(GETSUBROUTINEINDEX,GetSubroutineIndex)
g(GETACTIVESUBROUTINEUNIFORMIV,GetActiveSubroutineUniformiv)
g(GETACTIVESUBROUTINEUNIFORMNAME,GetActiveSubroutineUniformName)
g(GETACTIVESUBROUTINENAME,GetActiveSubroutineName)
g(UNIFORMSUBROUTINESUIV,UniformSubroutinesuiv)
g(GETUNIFORMSUBROUTINEUIV,GetUniformSubroutineuiv)
g(GETPROGRAMSTAGEIV,GetProgramStageiv)
g(PATCHPARAMETERI,PatchParameteri)
g(PATCHPARAMETERFV,PatchParameterfv)
g(BINDTRANSFORMFEEDBACK,BindTransformFeedback)
g(DELETETRANSFORMFEEDBACKS,DeleteTransformFeedbacks)
g(GENTRANSFORMFEEDBACKS,GenTransformFeedbacks)
g(ISTRANSFORMFEEDBACK,IsTransformFeedback)
g(PAUSETRANSFORMFEEDBACK,PauseTransformFeedback)
g(RESUMETRANSFORMFEEDBACK,ResumeTransformFeedback)
g(DRAWTRANSFORMFEEDBACK,DrawTransformFeedback)
g(DRAWTRANSFORMFEEDBACKSTREAM,DrawTransformFeedbackStream)
g(BEGINQUERYINDEXED,BeginQueryIndexed)
g(ENDQUERYINDEXED,EndQueryIndexed)
g(GETQUERYINDEXEDIV,GetQueryIndexediv)
g(RELEASESHADERCOMPILER,ReleaseShaderCompiler)
g(SHADERBINARY,ShaderBinary)
g(GETSHADERPRECISIONFORMAT,GetShaderPrecisionFormat)
g(DEPTHRANGEF,DepthRangef)
g(CLEARDEPTHF,ClearDepthf)
g(GETPROGRAMBINARY,GetProgramBinary)
g(PROGRAMBINARY,ProgramBinary)
g(PROGRAMPARAMETERI,ProgramParameteri)
g(USEPROGRAMSTAGES,UseProgramStages)
g(ACTIVESHADERPROGRAM,ActiveShaderProgram)
g(CREATESHADERPROGRAMV,CreateShaderProgramv)
g(BINDPROGRAMPIPELINE,BindProgramPipeline)
g(DELETEPROGRAMPIPELINES,DeleteProgramPipelines)
g(GENPROGRAMPIPELINES,GenProgramPipelines)
g(ISPROGRAMPIPELINE,IsProgramPipeline)
g(GETPROGRAMPIPELINEIV,GetProgramPipelineiv)
g(PROGRAMUNIFORM1I,ProgramUniform1i)
g(PROGRAMUNIFORM1IV,ProgramUniform1iv)
g(PROGRAMUNIFORM1F,ProgramUniform1f)
g(PROGRAMUNIFORM1FV,ProgramUniform1fv)
g(PROGRAMUNIFORM1D,ProgramUniform1d)
g(PROGRAMUNIFORM1DV,ProgramUniform1dv)
g(PROGRAMUNIFORM1UI,ProgramUniform1ui)
g(PROGRAMUNIFORM1UIV,ProgramUniform1uiv)
g(PROGRAMUNIFORM2I,ProgramUniform2i)
g(PROGRAMUNIFORM2IV,ProgramUniform2iv)
g(PROGRAMUNIFORM2F,ProgramUniform2f)
g(PROGRAMUNIFORM2FV,ProgramUniform2fv)
g(PROGRAMUNIFORM2D,ProgramUniform2d)
g(PROGRAMUNIFORM2DV,ProgramUniform2dv)
g(PROGRAMUNIFORM2UI,ProgramUniform2ui)
g(PROGRAMUNIFORM2UIV,ProgramUniform2uiv)
g(PROGRAMUNIFORM3I,ProgramUniform3i)
g(PROGRAMUNIFORM3IV,ProgramUniform3iv)
g(PROGRAMUNIFORM3F,ProgramUniform3f)
g(PROGRAMUNIFORM3FV,ProgramUniform3fv)
g(PROGRAMUNIFORM3D,ProgramUniform3d)
g(PROGRAMUNIFORM3DV,ProgramUniform3dv)
g(PROGRAMUNIFORM3UI,ProgramUniform3ui)
g(PROGRAMUNIFORM3UIV,ProgramUniform3uiv)
g(PROGRAMUNIFORM4I,ProgramUniform4i)
g(PROGRAMUNIFORM4IV,ProgramUniform4iv)
g(PROGRAMUNIFORM4F,ProgramUniform4f)
g(PROGRAMUNIFORM4FV,ProgramUniform4fv)
g(PROGRAMUNIFORM4D,ProgramUniform4d)
g(PROGRAMUNIFORM4DV,ProgramUniform4dv)
g(PROGRAMUNIFORM4UI,ProgramUniform4ui)
g(PROGRAMUNIFORM4UIV,ProgramUniform4uiv)
g(PROGRAMUNIFORMMATRIX2FV,ProgramUniformMatrix2fv)
g(PROGRAMUNIFORMMATRIX3FV,ProgramUniformMatrix3fv)
g(PROGRAMUNIFORMMATRIX4FV,ProgramUniformMatrix4fv)
g(PROGRAMUNIFORMMATRIX2DV,ProgramUniformMatrix2dv)
g(PROGRAMUNIFORMMATRIX3DV,ProgramUniformMatrix3dv)
g(PROGRAMUNIFORMMATRIX4DV,ProgramUniformMatrix4dv)
g(PROGRAMUNIFORMMATRIX2X3FV,ProgramUniformMatrix2x3fv)
g(PROGRAMUNIFORMMATRIX3X2FV,ProgramUniformMatrix3x2fv)
g(PROGRAMUNIFORMMATRIX2X4FV,ProgramUniformMatrix2x4fv)
g(PROGRAMUNIFORMMATRIX4X2FV,ProgramUniformMatrix4x2fv)
g(PROGRAMUNIFORMMATRIX3X4FV,ProgramUniformMatrix3x4fv)
g(PROGRAMUNIFORMMATRIX4X3FV,ProgramUniformMatrix4x3fv)
g(PROGRAMUNIFORMMATRIX2X3DV,ProgramUniformMatrix2x3dv)
g(PROGRAMUNIFORMMATRIX3X2DV,ProgramUniformMatrix3x2dv)
g(PROGRAMUNIFORMMATRIX2X4DV,ProgramUniformMatrix2x4dv)
g(PROGRAMUNIFORMMATRIX4X2DV,ProgramUniformMatrix4x2dv)
g(PROGRAMUNIFORMMATRIX3X4DV,ProgramUniformMatrix3x4dv)
g(PROGRAMUNIFORMMATRIX4X3DV,ProgramUniformMatrix4x3dv)
g(VALIDATEPROGRAMPIPELINE,ValidateProgramPipeline)
g(GETPROGRAMPIPELINEINFOLOG,GetProgramPipelineInfoLog)
g(VERTEXATTRIBL1D,VertexAttribL1d)
g(VERTEXATTRIBL2D,VertexAttribL2d)
g(VERTEXATTRIBL3D,VertexAttribL3d)
g(VERTEXATTRIBL4D,VertexAttribL4d)
g(VERTEXATTRIBL1DV,VertexAttribL1dv)
g(VERTEXATTRIBL2DV,VertexAttribL2dv)
g(VERTEXATTRIBL3DV,VertexAttribL3dv)
g(VERTEXATTRIBL4DV,VertexAttribL4dv)
g(VERTEXATTRIBLPOINTER,VertexAttribLPointer)
g(GETVERTEXATTRIBLDV,GetVertexAttribLdv)
g(VIEWPORTARRAYV,ViewportArrayv)
g(VIEWPORTINDEXEDF,ViewportIndexedf)
g(VIEWPORTINDEXEDFV,ViewportIndexedfv)
g(SCISSORARRAYV,ScissorArrayv)
g(SCISSORINDEXED,ScissorIndexed)
g(SCISSORINDEXEDV,ScissorIndexedv)
g(DEPTHRANGEARRAYV,DepthRangeArrayv)
g(DEPTHRANGEINDEXED,DepthRangeIndexed)
g(GETFLOATI_V,GetFloati_v)
g(GETDOUBLEI_V,GetDoublei_v)
g(DRAWARRAYSINSTANCEDBASEINSTANCE,DrawArraysInstancedBaseInstance)
g(DRAWELEMENTSINSTANCEDBASEINSTANCE,DrawElementsInstancedBaseInstance)
g(DRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE,DrawElementsInstancedBaseVertexBaseInstance)
g(GETINTERNALFORMATIV,GetInternalformativ)
g(GETACTIVEATOMICCOUNTERBUFFERIV,GetActiveAtomicCounterBufferiv)
g(BINDIMAGETEXTURE,BindImageTexture)
g(MEMORYBARRIER,MemoryBarrier)
g(TEXSTORAGE1D,TexStorage1D)
g(TEXSTORAGE2D,TexStorage2D)
g(TEXSTORAGE3D,TexStorage3D)
g(DRAWTRANSFORMFEEDBACKINSTANCED,DrawTransformFeedbackInstanced)
g(DRAWTRANSFORMFEEDBACKSTREAMINSTANCED,DrawTransformFeedbackStreamInstanced)
g(CLEARBUFFERDATA,ClearBufferData)
g(CLEARBUFFERSUBDATA,ClearBufferSubData)
g(DISPATCHCOMPUTE,DispatchCompute)
g(DISPATCHCOMPUTEINDIRECT,DispatchComputeIndirect)
g(COPYIMAGESUBDATA,CopyImageSubData)
g(FRAMEBUFFERPARAMETERI,FramebufferParameteri)
g(GETFRAMEBUFFERPARAMETERIV,GetFramebufferParameteriv)
g(GETINTERNALFORMATI64V,GetInternalformati64v)
g(INVALIDATETEXSUBIMAGE,InvalidateTexSubImage)
g(INVALIDATETEXIMAGE,InvalidateTexImage)
g(INVALIDATEBUFFERSUBDATA,InvalidateBufferSubData)
g(INVALIDATEBUFFERDATA,InvalidateBufferData)
g(INVALIDATEFRAMEBUFFER,InvalidateFramebuffer)
g(INVALIDATESUBFRAMEBUFFER,InvalidateSubFramebuffer)
g(MULTIDRAWARRAYSINDIRECT,MultiDrawArraysIndirect)
g(MULTIDRAWELEMENTSINDIRECT,MultiDrawElementsIndirect)
g(GETPROGRAMINTERFACEIV,GetProgramInterfaceiv)
g(GETPROGRAMRESOURCEINDEX,GetProgramResourceIndex)
g(GETPROGRAMRESOURCENAME,GetProgramResourceName)
g(GETPROGRAMRESOURCEIV,GetProgramResourceiv)
g(GETPROGRAMRESOURCELOCATION,GetProgramResourceLocation)
g(GETPROGRAMRESOURCELOCATIONINDEX,GetProgramResourceLocationIndex)
g(SHADERSTORAGEBLOCKBINDING,ShaderStorageBlockBinding)
g(TEXBUFFERRANGE,TexBufferRange)
g(TEXSTORAGE2DMULTISAMPLE,TexStorage2DMultisample)
g(TEXSTORAGE3DMULTISAMPLE,TexStorage3DMultisample)
g(TEXTUREVIEW,TextureView)
g(BINDVERTEXBUFFER,BindVertexBuffer)
g(VERTEXATTRIBFORMAT,VertexAttribFormat)
g(VERTEXATTRIBIFORMAT,VertexAttribIFormat)
g(VERTEXATTRIBLFORMAT,VertexAttribLFormat)
g(VERTEXATTRIBBINDING,VertexAttribBinding)
g(VERTEXBINDINGDIVISOR,VertexBindingDivisor)
g(DEBUGMESSAGECONTROL,DebugMessageControl)
g(DEBUGMESSAGEINSERT,DebugMessageInsert)
g(DEBUGMESSAGECALLBACK,DebugMessageCallback)
g(GETDEBUGMESSAGELOG,GetDebugMessageLog)
g(PUSHDEBUGGROUP,PushDebugGroup)
g(POPDEBUGGROUP,PopDebugGroup)
g(OBJECTLABEL,ObjectLabel)
g(GETOBJECTLABEL,GetObjectLabel)
g(OBJECTPTRLABEL,ObjectPtrLabel)
g(GETOBJECTPTRLABEL,GetObjectPtrLabel)
g(BUFFERSTORAGE,BufferStorage)
g(CLEARTEXIMAGE,ClearTexImage)
g(CLEARTEXSUBIMAGE,ClearTexSubImage)
g(BINDBUFFERSBASE,BindBuffersBase)
g(BINDBUFFERSRANGE,BindBuffersRange)
g(BINDTEXTURES,BindTextures)
g(BINDSAMPLERS,BindSamplers)
g(BINDIMAGETEXTURES,BindImageTextures)
g(BINDVERTEXBUFFERS,BindVertexBuffers)
g(CLIPCONTROL,ClipControl)
g(CREATETRANSFORMFEEDBACKS,CreateTransformFeedbacks)
g(TRANSFORMFEEDBACKBUFFERBASE,TransformFeedbackBufferBase)
g(TRANSFORMFEEDBACKBUFFERRANGE,TransformFeedbackBufferRange)
g(GETTRANSFORMFEEDBACKIV,GetTransformFeedbackiv)
g(GETTRANSFORMFEEDBACKI_V,GetTransformFeedbacki_v)
g(GETTRANSFORMFEEDBACKI64_V,GetTransformFeedbacki64_v)
g(CREATEBUFFERS,CreateBuffers)
g(NAMEDBUFFERSTORAGE,NamedBufferStorage)
g(NAMEDBUFFERDATA,NamedBufferData)
g(NAMEDBUFFERSUBDATA,NamedBufferSubData)
g(COPYNAMEDBUFFERSUBDATA,CopyNamedBufferSubData)
g(CLEARNAMEDBUFFERDATA,ClearNamedBufferData)
g(CLEARNAMEDBUFFERSUBDATA,ClearNamedBufferSubData)
g(MAPNAMEDBUFFER,MapNamedBuffer)
g(MAPNAMEDBUFFERRANGE,MapNamedBufferRange)
g(UNMAPNAMEDBUFFER,UnmapNamedBuffer)
g(FLUSHMAPPEDNAMEDBUFFERRANGE,FlushMappedNamedBufferRange)
g(GETNAMEDBUFFERPARAMETERIV,GetNamedBufferParameteriv)
g(GETNAMEDBUFFERPARAMETERI64V,GetNamedBufferParameteri64v)
g(GETNAMEDBUFFERPOINTERV,GetNamedBufferPointerv)
g(GETNAMEDBUFFERSUBDATA,GetNamedBufferSubData)
g(CREATEFRAMEBUFFERS,CreateFramebuffers)
g(NAMEDFRAMEBUFFERRENDERBUFFER,NamedFramebufferRenderbuffer)
g(NAMEDFRAMEBUFFERPARAMETERI,NamedFramebufferParameteri)
g(NAMEDFRAMEBUFFERTEXTURE,NamedFramebufferTexture)
g(NAMEDFRAMEBUFFERTEXTURELAYER,NamedFramebufferTextureLayer)
g(NAMEDFRAMEBUFFERDRAWBUFFER,NamedFramebufferDrawBuffer)
g(NAMEDFRAMEBUFFERDRAWBUFFERS,NamedFramebufferDrawBuffers)
g(NAMEDFRAMEBUFFERREADBUFFER,NamedFramebufferReadBuffer)
g(INVALIDATENAMEDFRAMEBUFFERDATA,InvalidateNamedFramebufferData)
g(INVALIDATENAMEDFRAMEBUFFERSUBDATA,InvalidateNamedFramebufferSubData)
g(CLEARNAMEDFRAMEBUFFERIV,ClearNamedFramebufferiv)
g(CLEARNAMEDFRAMEBUFFERUIV,ClearNamedFramebufferuiv)
g(CLEARNAMEDFRAMEBUFFERFV,ClearNamedFramebufferfv)
g(CLEARNAMEDFRAMEBUFFERFI,ClearNamedFramebufferfi)
g(BLITNAMEDFRAMEBUFFER,BlitNamedFramebuffer)
g(CHECKNAMEDFRAMEBUFFERSTATUS,CheckNamedFramebufferStatus)
g(GETNAMEDFRAMEBUFFERPARAMETERIV,GetNamedFramebufferParameteriv)
g(GETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIV,GetNamedFramebufferAttachmentParameteriv)
g(CREATERENDERBUFFERS,CreateRenderbuffers)
g(NAMEDRENDERBUFFERSTORAGE,NamedRenderbufferStorage)
g(NAMEDRENDERBUFFERSTORAGEMULTISAMPLE,NamedRenderbufferStorageMultisample)
g(GETNAMEDRENDERBUFFERPARAMETERIV,GetNamedRenderbufferParameteriv)
g(CREATETEXTURES,CreateTextures)
g(TEXTUREBUFFER,TextureBuffer)
g(TEXTUREBUFFERRANGE,TextureBufferRange)
g(TEXTURESTORAGE1D,TextureStorage1D)
g(TEXTURESTORAGE2D,TextureStorage2D)
g(TEXTURESTORAGE3D,TextureStorage3D)
g(TEXTURESTORAGE2DMULTISAMPLE,TextureStorage2DMultisample)
g(TEXTURESTORAGE3DMULTISAMPLE,TextureStorage3DMultisample)
g(TEXTURESUBIMAGE1D,TextureSubImage1D)
g(TEXTURESUBIMAGE2D,TextureSubImage2D)
g(TEXTURESUBIMAGE3D,TextureSubImage3D)
g(COMPRESSEDTEXTURESUBIMAGE1D,CompressedTextureSubImage1D)
g(COMPRESSEDTEXTURESUBIMAGE2D,CompressedTextureSubImage2D)
g(COMPRESSEDTEXTURESUBIMAGE3D,CompressedTextureSubImage3D)
g(COPYTEXTURESUBIMAGE1D,CopyTextureSubImage1D)
g(COPYTEXTURESUBIMAGE2D,CopyTextureSubImage2D)
g(COPYTEXTURESUBIMAGE3D,CopyTextureSubImage3D)
g(TEXTUREPARAMETERF,TextureParameterf)
g(TEXTUREPARAMETERFV,TextureParameterfv)
g(TEXTUREPARAMETERI,TextureParameteri)
g(TEXTUREPARAMETERIIV,TextureParameterIiv)
g(TEXTUREPARAMETERIUIV,TextureParameterIuiv)
g(TEXTUREPARAMETERIV,TextureParameteriv)
g(GENERATETEXTUREMIPMAP,GenerateTextureMipmap)
g(BINDTEXTUREUNIT,BindTextureUnit)
g(GETTEXTUREIMAGE,GetTextureImage)
g(GETCOMPRESSEDTEXTUREIMAGE,GetCompressedTextureImage)
g(GETTEXTURELEVELPARAMETERFV,GetTextureLevelParameterfv)
g(GETTEXTURELEVELPARAMETERIV,GetTextureLevelParameteriv)
g(GETTEXTUREPARAMETERFV,GetTextureParameterfv)
g(GETTEXTUREPARAMETERIIV,GetTextureParameterIiv)
g(GETTEXTUREPARAMETERIUIV,GetTextureParameterIuiv)
g(GETTEXTUREPARAMETERIV,GetTextureParameteriv)
g(CREATEVERTEXARRAYS,CreateVertexArrays)
g(DISABLEVERTEXARRAYATTRIB,DisableVertexArrayAttrib)
g(ENABLEVERTEXARRAYATTRIB,EnableVertexArrayAttrib)
g(VERTEXARRAYELEMENTBUFFER,VertexArrayElementBuffer)
g(VERTEXARRAYVERTEXBUFFER,VertexArrayVertexBuffer)
g(VERTEXARRAYVERTEXBUFFERS,VertexArrayVertexBuffers)
g(VERTEXARRAYATTRIBBINDING,VertexArrayAttribBinding)
g(VERTEXARRAYATTRIBFORMAT,VertexArrayAttribFormat)
g(VERTEXARRAYATTRIBIFORMAT,VertexArrayAttribIFormat)
g(VERTEXARRAYATTRIBLFORMAT,VertexArrayAttribLFormat)
g(VERTEXARRAYBINDINGDIVISOR,VertexArrayBindingDivisor)
g(GETVERTEXARRAYIV,GetVertexArrayiv)
g(GETVERTEXARRAYINDEXEDIV,GetVertexArrayIndexediv)
g(GETVERTEXARRAYINDEXED64IV,GetVertexArrayIndexed64iv)
g(CREATESAMPLERS,CreateSamplers)
g(CREATEPROGRAMPIPELINES,CreateProgramPipelines)
g(CREATEQUERIES,CreateQueries)
g(GETQUERYBUFFEROBJECTI64V,GetQueryBufferObjecti64v)
g(GETQUERYBUFFEROBJECTIV,GetQueryBufferObjectiv)
g(GETQUERYBUFFEROBJECTUI64V,GetQueryBufferObjectui64v)
g(GETQUERYBUFFEROBJECTUIV,GetQueryBufferObjectuiv)
g(MEMORYBARRIERBYREGION,MemoryBarrierByRegion)
g(GETTEXTURESUBIMAGE,GetTextureSubImage)
g(GETCOMPRESSEDTEXTURESUBIMAGE,GetCompressedTextureSubImage)
g(GETGRAPHICSRESETSTATUS,GetGraphicsResetStatus)
g(GETNCOMPRESSEDTEXIMAGE,GetnCompressedTexImage)
g(GETNTEXIMAGE,GetnTexImage)
g(GETNUNIFORMDV,GetnUniformdv)
g(GETNUNIFORMFV,GetnUniformfv)
g(GETNUNIFORMIV,GetnUniformiv)
g(GETNUNIFORMUIV,GetnUniformuiv)
g(READNPIXELS,ReadnPixels)
g(TEXTUREBARRIER,TextureBarrier)
g(SPECIALIZESHADER,SpecializeShader)
g(MULTIDRAWARRAYSINDIRECTCOUNT,MultiDrawArraysIndirectCount)
g(MULTIDRAWELEMENTSINDIRECTCOUNT,MultiDrawElementsIndirectCount)
g(POLYGONOFFSETCLAMP,PolygonOffsetClamp)
g(PRIMITIVEBOUNDINGBOXARB,PrimitiveBoundingBoxARB)
g(GETTEXTUREHANDLEARB,GetTextureHandleARB)
g(GETTEXTURESAMPLERHANDLEARB,GetTextureSamplerHandleARB)
g(MAKETEXTUREHANDLERESIDENTARB,MakeTextureHandleResidentARB)
g(MAKETEXTUREHANDLENONRESIDENTARB,MakeTextureHandleNonResidentARB)
g(GETIMAGEHANDLEARB,GetImageHandleARB)
g(MAKEIMAGEHANDLERESIDENTARB,MakeImageHandleResidentARB)
g(MAKEIMAGEHANDLENONRESIDENTARB,MakeImageHandleNonResidentARB)
g(UNIFORMHANDLEUI64ARB,UniformHandleui64ARB)
g(UNIFORMHANDLEUI64VARB,UniformHandleui64vARB)
g(PROGRAMUNIFORMHANDLEUI64ARB,ProgramUniformHandleui64ARB)
g(PROGRAMUNIFORMHANDLEUI64VARB,ProgramUniformHandleui64vARB)
g(ISTEXTUREHANDLERESIDENTARB,IsTextureHandleResidentARB)
g(ISIMAGEHANDLERESIDENTARB,IsImageHandleResidentARB)
g(VERTEXATTRIBL1UI64ARB,VertexAttribL1ui64ARB)
g(VERTEXATTRIBL1UI64VARB,VertexAttribL1ui64vARB)
g(GETVERTEXATTRIBLUI64VARB,GetVertexAttribLui64vARB)
g(DISPATCHCOMPUTEGROUPSIZEARB,DispatchComputeGroupSizeARB)
g(DEBUGMESSAGECONTROLARB,DebugMessageControlARB)
g(DEBUGMESSAGEINSERTARB,DebugMessageInsertARB)
g(DEBUGMESSAGECALLBACKARB,DebugMessageCallbackARB)
g(GETDEBUGMESSAGELOGARB,GetDebugMessageLogARB)
g(BLENDEQUATIONIARB,BlendEquationiARB)
g(BLENDEQUATIONSEPARATEIARB,BlendEquationSeparateiARB)
g(BLENDFUNCIARB,BlendFunciARB)
g(BLENDFUNCSEPARATEIARB,BlendFuncSeparateiARB)
g(DRAWARRAYSINSTANCEDARB,DrawArraysInstancedARB)
g(DRAWELEMENTSINSTANCEDARB,DrawElementsInstancedARB)
g(PROGRAMPARAMETERIARB,ProgramParameteriARB)
g(FRAMEBUFFERTEXTUREARB,FramebufferTextureARB)
g(FRAMEBUFFERTEXTURELAYERARB,FramebufferTextureLayerARB)
g(FRAMEBUFFERTEXTUREFACEARB,FramebufferTextureFaceARB)
g(SPECIALIZESHADERARB,SpecializeShaderARB)
g(UNIFORM1I64ARB,Uniform1i64ARB)
g(UNIFORM2I64ARB,Uniform2i64ARB)
g(UNIFORM3I64ARB,Uniform3i64ARB)
g(UNIFORM4I64ARB,Uniform4i64ARB)
g(UNIFORM1I64VARB,Uniform1i64vARB)
g(UNIFORM2I64VARB,Uniform2i64vARB)
g(UNIFORM3I64VARB,Uniform3i64vARB)
g(UNIFORM4I64VARB,Uniform4i64vARB)
g(UNIFORM1UI64ARB,Uniform1ui64ARB)
g(UNIFORM2UI64ARB,Uniform2ui64ARB)
g(UNIFORM3UI64ARB,Uniform3ui64ARB)
g(UNIFORM4UI64ARB,Uniform4ui64ARB)
g(UNIFORM1UI64VARB,Uniform1ui64vARB)
g(UNIFORM2UI64VARB,Uniform2ui64vARB)
g(UNIFORM3UI64VARB,Uniform3ui64vARB)
g(UNIFORM4UI64VARB,Uniform4ui64vARB)
g(GETUNIFORMI64VARB,GetUniformi64vARB)
g(GETUNIFORMUI64VARB,GetUniformui64vARB)
g(GETNUNIFORMI64VARB,GetnUniformi64vARB)
g(GETNUNIFORMUI64VARB,GetnUniformui64vARB)
g(PROGRAMUNIFORM1I64ARB,ProgramUniform1i64ARB)
g(PROGRAMUNIFORM2I64ARB,ProgramUniform2i64ARB)
g(PROGRAMUNIFORM3I64ARB,ProgramUniform3i64ARB)
g(PROGRAMUNIFORM4I64ARB,ProgramUniform4i64ARB)
g(PROGRAMUNIFORM1I64VARB,ProgramUniform1i64vARB)
g(PROGRAMUNIFORM2I64VARB,ProgramUniform2i64vARB)
g(PROGRAMUNIFORM3I64VARB,ProgramUniform3i64vARB)
g(PROGRAMUNIFORM4I64VARB,ProgramUniform4i64vARB)
g(PROGRAMUNIFORM1UI64ARB,ProgramUniform1ui64ARB)
g(PROGRAMUNIFORM2UI64ARB,ProgramUniform2ui64ARB)
g(PROGRAMUNIFORM3UI64ARB,ProgramUniform3ui64ARB)
g(PROGRAMUNIFORM4UI64ARB,ProgramUniform4ui64ARB)
g(PROGRAMUNIFORM1UI64VARB,ProgramUniform1ui64vARB)
g(PROGRAMUNIFORM2UI64VARB,ProgramUniform2ui64vARB)
g(PROGRAMUNIFORM3UI64VARB,ProgramUniform3ui64vARB)
g(PROGRAMUNIFORM4UI64VARB,ProgramUniform4ui64vARB)
g(MULTIDRAWARRAYSINDIRECTCOUNTARB,MultiDrawArraysIndirectCountARB)
g(MULTIDRAWELEMENTSINDIRECTCOUNTARB,MultiDrawElementsIndirectCountARB)
g(VERTEXATTRIBDIVISORARB,VertexAttribDivisorARB)
g(MAXSHADERCOMPILERTHREADSARB,MaxShaderCompilerThreadsARB)
g(GETGRAPHICSRESETSTATUSARB,GetGraphicsResetStatusARB)
g(GETNTEXIMAGEARB,GetnTexImageARB)
g(READNPIXELSARB,ReadnPixelsARB)
g(GETNCOMPRESSEDTEXIMAGEARB,GetnCompressedTexImageARB)
g(GETNUNIFORMFVARB,GetnUniformfvARB)
g(GETNUNIFORMIVARB,GetnUniformivARB)
g(GETNUNIFORMUIVARB,GetnUniformuivARB)
g(GETNUNIFORMDVARB,GetnUniformdvARB)
g(FRAMEBUFFERSAMPLELOCATIONSFVARB,FramebufferSampleLocationsfvARB)
g(NAMEDFRAMEBUFFERSAMPLELOCATIONSFVARB,NamedFramebufferSampleLocationsfvARB)
g(EVALUATEDEPTHVALUESARB,EvaluateDepthValuesARB)
g(MINSAMPLESHADINGARB,MinSampleShadingARB)
g(NAMEDSTRINGARB,NamedStringARB)
g(DELETENAMEDSTRINGARB,DeleteNamedStringARB)
g(COMPILESHADERINCLUDEARB,CompileShaderIncludeARB)
g(ISNAMEDSTRINGARB,IsNamedStringARB)
g(GETNAMEDSTRINGARB,GetNamedStringARB)
g(GETNAMEDSTRINGIVARB,GetNamedStringivARB)
g(BUFFERPAGECOMMITMENTARB,BufferPageCommitmentARB)
g(NAMEDBUFFERPAGECOMMITMENTEXT,NamedBufferPageCommitmentEXT)
g(TEXPAGECOMMITMENTARB,TexPageCommitmentARB)
g(TEXBUFFERARB,TexBufferARB)
g(DEPTHRANGEARRAYDVNV,DepthRangeArraydvNV)
g(DEPTHRANGEINDEXEDDNV,DepthRangeIndexeddNV)
g(FRAMEBUFFERPARAMETERIMESA,FramebufferParameteriMESA)
g(GETFRAMEBUFFERPARAMETERIVMESA,GetFramebufferParameterivMESA)
g(SUBPIXELPRECISIONBIASNV,SubpixelPrecisionBiasNV)
g(CONSERVATIVERASTERPARAMETERFNV,ConservativeRasterParameterfNV)
g(CONSERVATIVERASTERPARAMETERINV,ConservativeRasterParameteriNV)
#undef g

// query if extensions exist
#define e(ext) GX_##ext = gxcorearb_extension_exists( "GL_"#ext );
e(ARB_ES2_compatibility)
e(ARB_ES3_1_compatibility)
e(ARB_ES3_2_compatibility)
e(ARB_ES3_compatibility)
e(ARB_arrays_of_arrays)
e(ARB_base_instance)
e(ARB_bindless_texture)
e(ARB_blend_func_extended)
e(ARB_buffer_storage)
e(ARB_cl_event)
e(ARB_clear_buffer_object)
e(ARB_clear_texture)
e(ARB_clip_control)
e(ARB_compressed_texture_pixel_storage)
e(ARB_compute_shader)
e(ARB_compute_variable_group_size)
e(ARB_conditional_render_inverted)
e(ARB_conservative_depth)
e(ARB_copy_buffer)
e(ARB_copy_image)
e(ARB_cull_distance)
e(ARB_debug_output)
e(ARB_depth_buffer_float)
e(ARB_depth_clamp)
e(ARB_derivative_control)
e(ARB_direct_state_access)
e(ARB_draw_buffers_blend)
e(ARB_draw_elements_base_vertex)
e(ARB_draw_indirect)
e(ARB_draw_instanced)
e(ARB_enhanced_layouts)
e(ARB_explicit_attrib_location)
e(ARB_explicit_uniform_location)
e(ARB_fragment_coord_conventions)
e(ARB_fragment_layer_viewport)
e(ARB_fragment_shader_interlock)
e(ARB_framebuffer_no_attachments)
e(ARB_framebuffer_object)
e(ARB_framebuffer_sRGB)
e(ARB_geometry_shader4)
e(ARB_get_program_binary)
e(ARB_get_texture_sub_image)
e(ARB_gl_spirv)
e(ARB_gpu_shader5)
e(ARB_gpu_shader_fp64)
e(ARB_gpu_shader_int64)
e(ARB_half_float_vertex)
e(ARB_imaging)
e(ARB_indirect_parameters)
e(ARB_instanced_arrays)
e(ARB_internalformat_query)
e(ARB_internalformat_query2)
e(ARB_invalidate_subdata)
e(ARB_map_buffer_alignment)
e(ARB_map_buffer_range)
e(ARB_multi_bind)
e(ARB_multi_draw_indirect)
e(ARB_occlusion_query2)
e(ARB_parallel_shader_compile)
e(ARB_pipeline_statistics_query)
e(ARB_pixel_buffer_object)
e(ARB_polygon_offset_clamp)
e(ARB_post_depth_coverage)
e(ARB_program_interface_query)
e(ARB_provoking_vertex)
e(ARB_query_buffer_object)
e(ARB_robust_buffer_access_behavior)
e(ARB_robustness)
e(ARB_robustness_isolation)
e(ARB_sample_locations)
e(ARB_sample_shading)
e(ARB_sampler_objects)
e(ARB_seamless_cube_map)
e(ARB_seamless_cubemap_per_texture)
e(ARB_separate_shader_objects)
e(ARB_shader_atomic_counter_ops)
e(ARB_shader_atomic_counters)
e(ARB_shader_ballot)
e(ARB_shader_bit_encoding)
e(ARB_shader_clock)
e(ARB_shader_draw_parameters)
e(ARB_shader_group_vote)
e(ARB_shader_image_load_store)
e(ARB_shader_image_size)
e(ARB_shader_precision)
e(ARB_shader_stencil_export)
e(ARB_shader_storage_buffer_object)
e(ARB_shader_subroutine)
e(ARB_shader_texture_image_samples)
e(ARB_shader_viewport_layer_array)
e(ARB_shading_language_420pack)
e(ARB_shading_language_include)
e(ARB_shading_language_packing)
e(ARB_sparse_buffer)
e(ARB_sparse_texture)
e(ARB_sparse_texture2)
e(ARB_sparse_texture_clamp)
e(ARB_spirv_extensions)
e(ARB_stencil_texturing)
e(ARB_sync)
e(ARB_tessellation_shader)
e(ARB_texture_barrier)
e(ARB_texture_border_clamp)
e(ARB_texture_buffer_object)
e(ARB_texture_buffer_object_rgb32)
e(ARB_texture_buffer_range)
e(ARB_texture_compression_bptc)
e(ARB_texture_compression_rgtc)
e(ARB_texture_cube_map_array)
e(ARB_texture_filter_anisotropic)
e(ARB_texture_filter_minmax)
e(ARB_texture_gather)
e(ARB_texture_mirror_clamp_to_edge)
e(ARB_texture_mirrored_repeat)
e(ARB_texture_multisample)
e(ARB_texture_non_power_of_two)
e(ARB_texture_query_levels)
e(ARB_texture_query_lod)
e(ARB_texture_rg)
e(ARB_texture_rgb10_a2ui)
e(ARB_texture_stencil8)
e(ARB_texture_storage)
e(ARB_texture_storage_multisample)
e(ARB_texture_swizzle)
e(ARB_texture_view)
e(ARB_timer_query)
e(ARB_transform_feedback2)
e(ARB_transform_feedback3)
e(ARB_transform_feedback_instanced)
e(ARB_transform_feedback_overflow_query)
e(ARB_uniform_buffer_object)
e(ARB_vertex_array_bgra)
e(ARB_vertex_array_object)
e(ARB_vertex_attrib_64bit)
e(ARB_vertex_attrib_binding)
e(ARB_vertex_type_10f_11f_11f_rev)
e(ARB_vertex_type_2_10_10_10_rev)
e(ARB_viewport_array)
e(KHR_texture_compression_astc_hdr)
e(KHR_texture_compression_astc_ldr)
e(KHR_texture_compression_astc_sliced_3d)
e(MESA_framebuffer_flip_x)
e(MESA_framebuffer_flip_y)
e(MESA_framebuffer_swap_xy)
e(NV_conservative_raster)
e(NV_conservative_raster_dilate)
e(NV_conservative_raster_pre_snap)
e(NV_conservative_raster_pre_snap_triangles)
e(NV_conservative_raster_underestimation)
#undef e

	// release dll
#ifdef __msvc__
	if(__hOpenGL32&&!hOpenGL32) FreeLibrary((HMODULE)__hOpenGL32);
#elif defined __gcc__
	if(__hOpenGL32&&!hOpenGL32) dlclose(__hOpenGL32);
#endif

	// return if unsupported exists or logging is disabled
	if(gxcorearb_unsupported_functions.empty()&&gxcorearb_unsupported_extensions.empty()) return;

#ifdef GXCOREARB_LOG

	// get a lower-case computer name
	wchar_t cname[1024]={0}; DWORD cl=sizeof(cname)/sizeof(cname[0]);
	GetComputerNameW( cname, &cl ); for(size_t k=0,kn=wcslen(cname);k<kn;k++) cname[k]=tolower(cname[k]);

	// build a log path
	wchar_t log_dir[_MAX_PATH] = {};
	const wchar_t*(*rex_temp_dir)() = (const wchar_t*(*)()) GetProcAddress(GetModuleHandleW(nullptr),"rex_temp_dir");
	if(rex_temp_dir){ swprintf(log_dir,_MAX_PATH,L"%slog\\",rex_temp_dir()); if(_waccess(log_dir,0)!=0)_wmkdir(log_dir); }
	wchar_t log_path[_MAX_PATH]={0}; swprintf( log_path, _MAX_PATH, L"%sgxcorearb.%s.log", log_dir, cname );

	// bypass when a log already exists
	if(_waccess(log_path,0)==0) return;

	// leave a log for unsupported
	FILE* fp = _wfopen( log_path, L"w" ); if(!fp) return;
	if(!gxcorearb_unsupported_extensions.empty()){ fprintf( fp, "# Unavailable OpenGL extensions\n" ); for( auto& s : gxcorearb_unsupported_extensions ) fprintf(fp,"%s\n", s.c_str() ); fprintf( fp, "\n" ); }
	if(!gxcorearb_unsupported_functions.empty()){ fprintf( fp, "# Unavailable OpenGL core-profile functions\n" ); for( auto& s : gxcorearb_unsupported_functions ) fprintf(fp,"%s\n", s.c_str() ); }
	fclose(fp);

	wprintf( L"[gxcorearb] see %s for unavailable OpenGL extensions\n", log_path );

#endif
}

#endif /* __gxcorearb_h_ */
