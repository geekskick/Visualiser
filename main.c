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

#include "ppm.h"
#include "gif-h/gif.h"

// -----------------------------------------------------
// These values are used in the gif_write_frame callback so they are defined globally
static const int        numbers    = 250;           ///< Length of the array to sort
static const int        height     = 50;            ///< Output image strip height
static union pixel_t    gif[numbers * height];      ///< The Image buffer
struct gif_writer       writer;                     ///< The writer

/// The arguments as enum
enum {  APP_NAME = 0,   ///< The first is always the application name
        OUTPUT_LOC,     ///< The second should always be the output filename
        ARG_COUNT       ///< There are only two args, but use this as a count value as comparison against argc.
};

typedef void (*gif_cb)(const uint32_t arr[], const int n);

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
 @param cb The callback to the function which actually writes it to the gif
 */
void	bubble_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), gif_cb cb);

/**
 Selection Sort from http://www.algolist.net/Algorithms/Sorting/Selection_sort
 
 @param arr The Array
 @param n Array Length
 @param test The compare function
 @param cb The callback to the function which actually writes it to the gif
 */
void	selctn_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), gif_cb cb);

/**
 Puts the element at n in the correct place in the heap

 @param arr The array
 @param n The length of it
 @param i The thing to place
 @param test The evaluation function
 */
void	heapify(uint32_t arr[], int n, const int i, bool (*test)(uint32_t, uint32_t));

/**
 Heap sort from http://www.geeksforgeeks.org/heap-sort/

 @param arr The array to sort
 @param n The length of it
 @param test The evaluation function
 @param cb The callback to the function which actually writes it to the gif
 */
void	heap_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), gif_cb cb);


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
 @param cb The callback to the function which actually writes it to the gif
 @param arr_len The length of the array
 */
void	merge_sort(uint32_t arr[], int l, int r, bool (*test)(uint32_t, uint32_t), gif_cb cb, const int arr_len);

/**
 Do a radix sort http://www.geeksforgeeks.org/radix-sort/

 @param arr The array to sort
 @param n It's length
 @param cb The callback to the function which actually writes it to the gif
 */
void	radix_sort(uint32_t arr[], const int n, gif_cb cb);

/**
 Count sorts on the digits in the values of the array

 @param arr The Array to sort
 @param n The length of it
 @param exp The multiplier, eg the units, the tens, the hundreds
 @param cb The callback to the function which actually writes it to the gif
 */
void	count_sort(uint32_t arr[], const int n, const int exp, gif_cb cb);


/// Write the new array to a gif frame
/// @param arr The array to put in
/// @param n The length of the array
void gif_pix_array_write(const uint32_t arr[], const int n){

    assert(n == numbers);
    
    for(int item = 0; item < n; item++ ){
        union pixel_t p;
        p.rgbeol = arr[item];
        p.eol = 0;
        for(int r = 0; r < height; r++){
            gif[(r*numbers) + item] = p;
        }
    }
    gif_write_frame(&writer, (uint8_t*)gif, 8, false);
}

int main(int argc, const char * argv[]) {
	
//    if(argc != ARG_COUNT) {
//        printf("Usage: ./%s outputfilename\n", argv[APP_NAME]);
//        return 0;
//    }
	
	srand((unsigned int)time(NULL));
	
    char filename[PPM_FILEPATH_BUFF_LEN];
    if(argc == ARG_COUNT){
        strncpy(filename, argv[OUTPUT_LOC], strlen(argv[OUTPUT_LOC]));
        strncat(filename, ".gif", strlen(".gif"));
    }
    else{
        strncpy(filename, "TestGif.gif", strlen("TestGif.gif"));
    }
    
	// Initialise arrays to a bunch of random values
	uint32_t arr[numbers]= { 0, };
	uint32_t second[numbers] = { 0 ,};
	uint32_t third[numbers] = { 0, };
	uint32_t fourth[numbers] = { 0, };
	uint32_t fifth[numbers] = { 0, };
    
    memset(gif, UINT32_MAX, numbers * height);
	
	for(int i = 0; i < numbers; i++){
		fifth[i] = fourth[i] = third[i] = second[i] = arr[i] = random()%UINT32_MAX;
        for(int r = 0; r < height; r++){
            gif[(r*numbers) + i].rgbeol = fifth[i];
        }
	}
    
	//-------------------------
    const unsigned int w = numbers;
    const unsigned int h = height; // O(n2)
    
    writer.delay = 1;
    writer.size.width = w;
    writer.size.height = h;
    
    bool rc = gif_begin(&writer, filename);
    if(!rc){
        fprintf(stderr, "Unsuccessful gif_begin\n");
        return 0;
    }
    //-------------------------

	printf("Now sorting\n");
	bool (*order)(uint32_t, uint32_t) = &gt_than;
	
	bubble_sort(arr,	numbers, order, gif_pix_array_write);
	selctn_sort(second, numbers, order, gif_pix_array_write);
	heap_sort  (third,  numbers, order, gif_pix_array_write);
	merge_sort (fourth, 0, numbers-1, order, gif_pix_array_write, numbers);
	radix_sort  (fifth, numbers, gif_pix_array_write);
    
	// Cleanup
    gif_end(&writer);
	printf("Complete and written to %s\n", filename);
	
	return PPM_ERR_NONE;
}

//-----------------------------------------------------
uint32_t get_max_average(uint32_t arr[], int n)
{
	uint32_t mx = ppm_pix_get_average((union pixel_t)arr[0]);
	for (int i = 1; i < n; i++){
		if (ppm_pix_get_average((union pixel_t)arr[i]) > mx){
			mx = ppm_pix_get_average((union pixel_t)arr[i]);
		}
	}
	return mx;
}

//-----------------------------------------------------
void count_sort(uint32_t arr[], const int n, const int exp, gif_cb cb){
	uint32_t output[n]; // output array
	int64_t i, count[10] = {0};
 
	// Store count of occurrences in count[]
	for (i = 0; i < n; i++){
		count[ (ppm_pix_get_average((union pixel_t)arr[i])/exp)%10 ]++;
	}
	// Change count[i] so that count[i] now contains actual
	//  position of this digit in output[]
	for (i = 1; i < 10; i++){
		count[i] += count[i - 1];
	}
	
	// Build the output array
	for (i = n - 1; i >= 0; i--)
	{
		output[count[ (ppm_pix_get_average((union pixel_t)arr[i])/exp)%10 ] - 1] = arr[i];
		count[ (ppm_pix_get_average((union pixel_t)arr[i])/exp)%10 ]--;
		if(cb != NULL) { cb(output, n); }
	}
 
	// Copy the output array to arr[], so that arr[] now
	// contains sorted numbers according to current digit
	for (i = 0; i < n; i++)
		arr[i] = output[i];
}

//-----------------------------------------------------
void radix_sort(uint32_t arr[], const int n,gif_cb cb)
{
	// Find the maximum number to know number of digits
	uint32_t m = get_max_average(arr, n);
 
	// Do counting sort for every digit. Note that instead
	// of passing digit number, exp is passed. exp is 10^i
	// where i is current digit number
	for (uint32_t exp = 1; m/exp > 0; exp *= 10){
		count_sort(arr, n, exp, cb);
		if(cb != NULL) { cb(arr, n); }
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
void bubble_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), gif_cb cb){
	int i, j;
	for (i = 0; i < n-1; i++){
		
		// Last i elements are already in place
		for (j = 0; j < n-i-1; j++){
			if (test(ppm_pix_get_average((union pixel_t)arr[j]), ppm_pix_get_average((union pixel_t)arr[j+1])))
			{
				swap(&arr[j], &arr[j+1]);
			}
		}
        if(cb != NULL) { cb(arr, n); }
	}
}

//-----------------------------------------------------
uint32_t ppm_pix_get_average(const union pixel_t p){
	return (p.r+p.g+p.g)/3;
}

//-----------------------------------------------------
void selctn_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), gif_cb cb) {
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
        if(cb != NULL) { cb(arr, n); }
	}
    if(cb != NULL) { cb(arr, n); }
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
void heap_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), gif_cb cb)
{
	
	assert(test);
	// Build heap (rearrange array)
	for (int i = n / 2 - 1; i >= 0; i--){
		heapify(arr, n, i, test);
        if(cb != NULL) { cb(arr, n); }
	}
 
	// One by one extract an element from heap
	for (int i=n-1; i>=0; i--)
	{
		// Move current root to end
		swap(&arr[0], &arr[i]);
		
		// call max heapify on the reduced heap
		heapify(arr, i, 0, test);
		
        if(cb != NULL) { cb(arr, n); }
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
void merge_sort(uint32_t arr[], int l, int r, bool (*test)(uint32_t, uint32_t), gif_cb cb, const int arr_len){
	
	assert(test);
	
	if (l < r)
	{
		// Same as (l+r)/2, but avoids overflow for
		// large l and h
		int m = l+(r-l)/2;
		
		// Sort first and second halves
		merge_sort(arr, l, m, test, cb, arr_len);
		merge_sort(arr, m+1, r,test,cb, arr_len);
		merge(arr, l, m, r, test);
        if(cb != NULL) { cb(arr, arr_len); }

	}
	
}


