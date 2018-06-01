//
//  ppm.h
//  Visualiser
//


#ifndef ppm_h
#define ppm_h

#define ERR_NONE						0
#define ERR_FILE_OPENING				1
#define ERR_FILE_FP						2
#define FILEPATH_BUFF_LEN				1024

#include <stdlib.h>
#include <stdio.h>

/**
 Set the red part of the pixel
 
 @param p The pixel
 @param rval The value to set to
 @return None
 */
#define ppm_pix_set_red( p, rval )		p.r = rval

/**
 Set the green part of the pixel
 
 @param p The pixel
 @param gval The value to set to
 @return None
 */
#define ppm_pix_set_green( p, gval )	p.g = gval

/**
 Set the blue part of the pixel
 
 @param p The pixel
 @param bval The value to set to
 @return None
 */
#define ppm_pix_set_blue( p, bval )		p.b = bval

/**
 Set the bit that comes after the pixel in the ppm to newline
 
 @param p The pixel
 @return None
 */
#define ppm_pix_set_eol( p )			p.eol = '\n'

/**
 Set the bit that comes after the pixel in the ppm to whitespace
 
 @param p The pixel
 @return None
 */
#define ppm_pix_clr_eol( p )			p.eol = ' '

/**
 Get the pixel to be black with a whitespace after
 
 @param p The pixel
 @return None
 */
#define ppm_pix_create_black( p )	    p.rgbeol = 0x00000000 | ' '

/**
 Get the pixel to be white with a whitespace after
 
 @param p The pixel
 @return None
 */
#define ppm_pix_create_white( p )	    p.rgbeol = 0xFFFFFFFF | ' '

/**
 The scale the value to go into the pixel between 0-max down to 0-255
 
 @param stgs The PPM settings
 @param p The pixel
 @return The scaled value as an integer
 */
#define ppm_pix_scale( stgs, p )		(int)(( 255.0/stgs.max ) * p )


/**
 A Pixel is an RGB value, and then either a whitespace or an EOL
 */
union pixel_t{
	struct{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t eol;
	};
	uint32_t	rgbeol;
};


/**
 Options for the PPM file output
 */
struct ppm_opts_t{
	char	file_name[FILEPATH_BUFF_LEN];
	FILE	*fp;				/// The File pointer
	int		width;
	int		height;
	int		max;				/// This will scale to 255 in the image using the ppm_pix_scale macro
	void    (*write)(const uint32_t[], const int, const struct ppm_opts_t * const);
};

//-----------------------------------------------------
/**
 Initialise the PPM File
 
 @param opts The options for opening the file
 @return ERR_NONE or relevant error code
 */
int		 ppm_init(struct ppm_opts_t * const opts);

/**
 Clean up
 
 @param opts The options of the PPM file
 @return ERR_NONE
 */
int		 ppm_deinit(struct ppm_opts_t * const opts);

/**
 Writes a pixel to the file
 
 @param pixel The pixel
 @param out The file
 @return ERR_NONE or relevant error code
 */
int		 ppm_pix_write(const union pixel_t * const pixel, FILE * const out);

/**
 Print a pixel to stdout
 
 @param pixel The Pixel
 */
void	 ppm_pix_printf(const union pixel_t * const pixel);

/**
 Write an array of pixels to a line of the PPM file
 
 @param arr The array
 @param n The array length
 @param settings The PPM settings
 */
void	 ppm_pix_array_write(const uint32_t arr[], const int n, const struct ppm_opts_t * const settings);

/**
 Averages the R, G and B values
 
 @param p The Pixel
 @return The average
 */
uint32_t ppm_pix_get_average(const union pixel_t p);


/**
 Put a the array repeated a certain number of times
 
 @param arr The array
 @param n The length of it
 @param settings The settings of PPM file
 @param height The number of strips in pixels
 */
void	ppm_strip_write(const uint32_t arr[], const int n, const struct ppm_opts_t * const settings, const int height);

#endif /* ppm_h */
