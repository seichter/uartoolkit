#ifndef enums_h
#define enums_h

/**
 * Error codes
 */
typedef enum {
	artError		= -1,	/**< return value if a function encountered a problem */
	artOk			=  0,	/**< everything fine */
	artThreshold,			/**< set the threshold for AR Toolkit recognition */
	artDebug				/**< debugging */
};

/** 
 * These enums are (will be) lined up with OpenGL,
 * the most likely target platform for
 * AR Toolkit content.
 */
typedef enum {
	ART_PIXFORMAT_ARGB = 0,	/**< Alpha, Red, Green, Blue */
    ART_PIXFORMAT_ABGR,
	ART_PIXFORMAT_RGB,
	ART_PIXFORMAT_BGR,
	ART_PIXFORMAT_BGRA,
	ART_PIXFORMAT_RGBA,
    ART_PIXFORMAT_2vuy,
	ART_PIXFORMAT_yuvs
};

/** 
 * For retrieving an actual matrix the 
 * client system needs to be known.
 */
typedef enum 
{
	ART_BASIC = 0,		/**< only basic information will be updated */
	ART_OPENGL,			/**< OpenGL transform matrix will be created */
	ART_DIRECTX			/**< DirectX transform matrix will be created */
};

#endif
