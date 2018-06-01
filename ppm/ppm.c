//
//  ppm.c
//  Visualiser
//

#include "ppm.h"

//-----------------------------------------------------
void ppm_strip_write(const uint32_t arr[], const int n, const struct ppm_opts_t * const settings, const int height){
	for(int i = 0; i < height; i++){
		ppm_pix_array_write(arr, n, settings);
	}
}

//-----------------------------------------------------
int ppm_pix_write(const union pixel_t * const pixel, FILE * const out){
	if(!out){
		fprintf(stderr, "[%d] Nothing in the &(FILE*)\n", __LINE__);
		return ERR_FILE_FP;
	}
	fprintf(out, "%d %d %d %c", pixel->r, pixel->g, pixel->b, pixel->eol);
	return ERR_NONE;
}

//-----------------------------------------------------
int ppm_init(struct ppm_opts_t * const opts){
	
	opts->fp = fopen(opts->file_name, "w");
	if(!opts->fp){
		perror("Error opening file\n");
		return ERR_FILE_OPENING;
	}
	
	// Header
	fprintf(opts->fp, "P3\n%d %d\n255\n", opts->width, opts->height);
	
	return ERR_NONE;
}

//-----------------------------------------------------
void ppm_pix_printf(const union pixel_t * const pixel){
	printf("[%8X] (%2X, %2X, %2X)\n", pixel->rgbeol, pixel->r, pixel->g, pixel->b);
}

//-----------------------------------------------------
int ppm_deinit(struct ppm_opts_t * const opts){
	fclose(opts->fp);
	opts->fp = NULL;
	return ERR_NONE;
}

//-----------------------------------------------------
void ppm_pix_array_write(const uint32_t arr[], const int n, const struct ppm_opts_t * const settings){
	for(int item = 0; item < n; item++ ){
		union pixel_t p;
		p.rgbeol = arr[item];
		item == n-1 ? (ppm_pix_set_eol(p)) : (ppm_pix_clr_eol(p));
		ppm_pix_write(&p, settings->fp);
	}
}
