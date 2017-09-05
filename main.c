//
//  main.c
//  Visualiser
//  https://en.wikipedia.org/wiki/Netpbm_format#PPM_example

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define ERR_NONE						0
#define ERR_FILE_OPENING				1
#define ERR_FILE_FP						2
#define FILEPATH_BUFF_LEN				1024

/**
 Set the red part of the pixel

 @param p The pixel
 @param rval The value to set to
 @return None
 */
#define ppm_pix_set_red( p, rval )		p.r = rval

/**
 Set the gree part of the pixel
 
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

void ppm_strip_write(const uint32_t arr[], const int n, const struct ppm_opts_t * const settings, const int height){
	for(int i = 0; i < height; i++){
		ppm_pix_array_write(arr, n, settings);
	}
}

// ----------- SORTING LIFTED FROM INTERNET -----------
/**
 A is greater than B
 
 @param a A value
 @param b B Value
 @return True/False
 */
bool	gt_than(const uint32_t a, const uint32_t b);

/**
 A is less than B
 
 @param a A value
 @param b B Value
 @return True/False
 */
bool	less_than(const uint32_t a, const uint32_t b);

/**
 Swaps two variables contents
 
 @param xp A, will have B
 @param yp B, will have A
 */
void	swap(uint32_t * const xp, uint32_t * const yp);

/**
 Bubble Sort from http://www.geeksforgeeks.org/bubble-sort/
 
 @param arr The Array
 @param n Array Length
 @param test The compare function
 @param settings The PPM file settings
 @param array_write The function to print the array to the ppm file
 */
void	bubble_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const));

/**
 Selection Sort from http://www.algolist.net/Algorithms/Sorting/Selection_sort
 
 @param arr The Array
 @param n Array Length
 @param test The compare function
 @param settings The PPM file settings
 @param array_write The function to print the array to the ppm file
 */
void	selctn_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const));

/**
 Puts the element at n in the correct place in the heap

 @param arr The array
 @param n The length of it
 @param i The thing to place
 @param test The evavulation function
 */
void	heapify(uint32_t arr[], int n, const int i, bool (*test)(uint32_t, uint32_t));

/**
 Heap sort from http://www.geeksforgeeks.org/heap-sort/

 @param arr The array to sort
 @param n The length of it
 @param test The evaluation function
 @param settings The PPM file settings
 @param array_write The function to print the array to the PPM file
 */
void	heap_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const));


/**
 Merge the two asubarrays

 @param arr The total array
 @param l The left
 @param m The middle
 @param r The right
 @param test The evaluation function
 */
void	merge(uint32_t arr[], int l, int m, int r, bool (*test)(uint32_t, uint32_t));

/**
 Perform the mergesort http://www.geeksforgeeks.org/merge-sort/

 @param arr The array to sort
 @param l The left edge of the array
 @param r The right edge of the array
 @param test The evaluation function
 @param arr_len The length of the array to sort
 */
void	merge_sort(uint32_t arr[], int l, int r, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const), const int arr_len);


enum { APP_NAME = 0, OUTPUT_LOC, ARG_COUNT };
// ------------------ END PLAGURISM  ------------------

int main(int argc, const char * argv[]) {

	if(argc != ARG_COUNT) {
		printf("Usage: ./%s outputfilename\n", argv[APP_NAME]);
		return 0;
	}
	
	srand((unsigned int)time(NULL));
	static const int numbers	= 250;							// Length of the array to sort
	static const int border		= 6;							// The height of the strip in pixels
	static const int num_sorts	= 4;							// The number of sortin algorithms

	static const int ppm_len	= (num_sorts*numbers) +			// There will be an image for each sort
								  ((num_sorts*2) * border) +	// There will be a border above and below each sort
								  numbers/2;					// The merge sort needs extra time as it needs to put the array into a heap first
	
	struct ppm_opts_t settings	= { "test.ppm",					// The filename output
									NULL,						// File pointer init to null
									numbers,					// The width of the output image
									ppm_len,					// The height of the image
									UINT8_MAX					// The maximum value for a pixel r or g or b value will be
									};
	
	strncpy(settings.file_name, argv[OUTPUT_LOC], FILEPATH_BUFF_LEN);
	strncat(settings.file_name, ".ppm", FILEPATH_BUFF_LEN);
	
	// Initialisation Routines
	if(ppm_init(&settings) != ERR_NONE){
		return 1;
	}
	if(!settings.fp){
		fprintf(stderr, "[%d] Nothing in fp\n", __LINE__);
		return ERR_FILE_FP;
	}
	
	// Initialise arrays to a bunch of random values
	uint32_t arr[numbers] = { 0, };
	uint32_t second[numbers] = { 0 ,};
	uint32_t third[numbers] = { 0, };
	uint32_t fourth[numbers] = {0, };
	
	for(int i = 0; i < numbers; i++){
		fourth[i] = third[i] = second[i] = arr[i] = random()%UINT32_MAX;
	}
	
	printf("Now sorting\n");
	bool (*order)(uint32_t, uint32_t) = &less_than;
	
	ppm_strip_write(arr, numbers, &settings, border);
	bubble_sort(arr,	numbers, order, &settings, &ppm_pix_array_write);
	ppm_strip_write(arr, numbers, &settings, border);
	
	ppm_strip_write(second, numbers, &settings, border);
	selctn_sort(second, numbers, order, &settings, &ppm_pix_array_write);
	ppm_strip_write(second, numbers, &settings, border);
	
	ppm_strip_write(third, numbers, &settings, border);
	heap_sort  (third,  numbers, order, &settings, &ppm_pix_array_write);
	ppm_strip_write(third, numbers, &settings, border);
	
	ppm_strip_write(fourth, numbers, &settings, border);
	merge_sort (fourth, 0, numbers-1, order, &settings, &ppm_pix_array_write, numbers);
	ppm_strip_write(fourth, numbers, &settings, border);

	// Cleanup
	ppm_deinit(&settings);
	printf("Complete and written to %s\n", settings.file_name);
	
	return ERR_NONE;
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

//-----------------------------------------------------
void swap(uint32_t * const xp, uint32_t * const yp){
	uint32_t temp = *xp;
	*xp = *yp;
	*yp = temp;
}

//-----------------------------------------------------
bool gt_than(const uint32_t a, const uint32_t b){
	return a > b;
}

//-----------------------------------------------------
bool less_than(const uint32_t a, const uint32_t b){
	return a < b;
}

//-----------------------------------------------------
void bubble_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const)){
	int i, j;
	for (i = 0; i < n-1; i++){
		
		// Last i elements are already in place
		for (j = 0; j < n-i-1; j++){
			if (test(ppm_pix_get_average((union pixel_t)arr[j]), ppm_pix_get_average((union pixel_t)arr[j+1])))
			{
				swap(&arr[j], &arr[j+1]);
			}
			
		}
		
		if(array_write != NULL){ array_write(arr, n, settings);}
		
	}
	
}

//-----------------------------------------------------
uint32_t ppm_pix_get_average(const union pixel_t p){
	return (p.r+p.g+p.g)/3;
}

//-----------------------------------------------------
void selctn_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const)) {
	int i, j, minIndex, tmp;
	for (i = 0; i < n - 1; i++) {
		minIndex = i;
		for (j = i + 1; j < n; j++)
			if(!test(ppm_pix_get_average((union pixel_t)arr[j]), ppm_pix_get_average((union pixel_t)arr[minIndex])))
				minIndex = j;
		if (minIndex != i) {
			tmp = arr[i];
			arr[i] = arr[minIndex];
			arr[minIndex] = tmp;
		}
		if(array_write != NULL){ array_write(arr, n, settings); }
	}
	if(array_write != NULL){ array_write(arr, n, settings); }
}

//-----------------------------------------------------
void heapify(uint32_t arr[], int n, const int i, bool (*test)(uint32_t, uint32_t)){
	
	assert(test);
	int smallest = i;  // Initialize largest as root
	int l = 2*i + 1;  // left = 2*i + 1
	int r = 2*i + 2;  // right = 2*i + 2
 
	// If left child is smaller than root
	if (l < n && test(ppm_pix_get_average((union pixel_t)arr[l]), ppm_pix_get_average((union pixel_t)arr[smallest])))
		smallest = l;
 
	// If right child is smaller than the smallest so far
	if (r < n && test(ppm_pix_get_average((union pixel_t)arr[r]), ppm_pix_get_average((union pixel_t)arr[smallest])))
		smallest = r;
 
	// If largest is not root
	if (smallest != i)
	{
		swap(&arr[i], &arr[smallest]);
		
		// Recursively heapify the affected sub-tree
		heapify(arr, n, smallest, test);
	}
}

//-----------------------------------------------------
void heap_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const))
{
	
	assert(test);
	// Build heap (rearrange array)
	for (int i = n / 2 - 1; i >= 0; i--){
		heapify(arr, n, i, test);
		if( array_write!= NULL ) { array_write(arr, n, settings); }
	}
 
	// One by one extract an element from heap
	for (int i=n-1; i>=0; i--)
	{
		// Move current root to end
		swap(&arr[0], &arr[i]);
		
		// call max heapify on the reduced heap
		heapify(arr, i, 0, test);
		
		if( array_write!= NULL ) { array_write(arr, n, settings); }
	}
}

//-----------------------------------------------------
void merge(uint32_t arr[], int l, int m, int r, bool (*test)(uint32_t, uint32_t)){
	
	assert(test);
	uint32_t i, j, k;
	uint32_t n1 = m - l + 1;
	uint32_t n2 =  r - m;
 
	/* create temp arrays */
	uint32_t L[n1], R[n2];
 
	/* Copy data to temp arrays L[] and R[] */
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1+ j];
 
	/* Merge the temp arrays back into arr[l..r]*/
	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray
	while (i < n1 && j < n2)
	{
		if (!test(ppm_pix_get_average((union pixel_t)L[i]), ppm_pix_get_average((union pixel_t)R[j])))
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}
 
	/* Copy the remaining elements of L[], if there
	 are any */
	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}
 
	/* Copy the remaining elements of R[], if there
	 are any */
	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}

//-----------------------------------------------------
void merge_sort(uint32_t arr[], int l, int r, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, void (*array_write)(const uint32_t[], const int, const struct ppm_opts_t * const), const int arr_len){
	
	assert(test);
	
	if (l < r)
	{
		// Same as (l+r)/2, but avoids overflow for
		// large l and h
		int m = l+(r-l)/2;
		
		// Sort first and second halves
		merge_sort(arr, l, m, test, settings, array_write, arr_len);
		merge_sort(arr, m+1, r,test,settings, array_write, arr_len);
		merge(arr, l, m, r, test);
		if( array_write != NULL ){ array_write(arr, arr_len, settings); }
		
	}
	
}
