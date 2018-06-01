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
 */
void	bubble_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings);

/**
 Selection Sort from http://www.algolist.net/Algorithms/Sorting/Selection_sort
 
 @param arr The Array
 @param n Array Length
 @param test The compare function
 @param settings The PPM file settings
 */
void	selctn_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings);

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
 */
void	heap_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings);


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
 @param arr_len The length of the array
 */
void	merge_sort(uint32_t arr[], int l, int r, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, const int arr_len);

/**
 Do a radix sort http://www.geeksforgeeks.org/radix-sort/

 @param arr The array to sort
 @param n It's length
 @param settings The PPM settings
 */
void	radix_sort(uint32_t arr[], const int n, const struct ppm_opts_t * const settings);

/**
 Count sorts on the digits in the values of the array

 @param arr The Array to sort
 @param n The length of it
 @param exp The multiplier, eg the units, the tens, the hundreds
 @param settings the PPM file settings
 */
void	count_sort(uint32_t arr[], const int n, const int exp, const struct ppm_opts_t * const settings);

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
	static const int num_sorts	= 5;							// The number of sortin algorithms

	static const int ppm_len	= (num_sorts*numbers) +			// There will be an image for each sort
								  ((num_sorts*2) * border) +	// There will be a border above and below each sort
								  numbers/2 +					// The heap sort needs extra time as it needs to put the array into a heap first
								  numbers*2;					// The radix sort has loads of extra detail
	
	struct ppm_opts_t settings	= { "test.ppm",					// The filename output
									NULL,						// File pointer init to null
									numbers,					// The width of the output image
									ppm_len,					// The height of the image
									UINT8_MAX					// The maximum value for a pixel r or g or b value will be
									};
	
	strncpy(settings.file_name, argv[OUTPUT_LOC], PPM_FILEPATH_BUFF_LEN);
	strncat(settings.file_name, ".ppm", PPM_FILEPATH_BUFF_LEN);
	
	// Initialisation Routines
	if(ppm_init(&settings) != PPM_ERR_NONE){
		return 1;
	}
	if(!settings.fp){
		fprintf(stderr, "[%d] Nothing in fp\n", __LINE__);
		return PPM_ERR_FILE_FP;
	}
	
	// Initialise arrays to a bunch of random values
	uint32_t arr[numbers] = { 0, };
	uint32_t second[numbers] = { 0 ,};
	uint32_t third[numbers] = { 0, };
	uint32_t fourth[numbers] = { 0, };
	uint32_t fifth[numbers] = { 0, };
	
	for(int i = 0; i < numbers; i++){
		fifth[i] = fourth[i] = third[i] = second[i] = arr[i] = random()%UINT32_MAX;
	}
	
	printf("Now sorting\n");
	bool (*order)(uint32_t, uint32_t) = &gt_than;
	settings.write = & ppm_pix_array_write;
	
	ppm_strip_write(arr, numbers, &settings, border);
	bubble_sort(arr,	numbers, order, &settings);
	ppm_strip_write(fifth, numbers, &settings, border);
	
	ppm_strip_write(second, numbers, &settings, border);
	selctn_sort(second, numbers, order, &settings);
	ppm_strip_write(second, numbers, &settings, border);
	
	ppm_strip_write(third, numbers, &settings, border);
	heap_sort  (third,  numbers, order, &settings);
	ppm_strip_write(third, numbers, &settings, border);
	
	ppm_strip_write(fourth, numbers, &settings, border);
	merge_sort (fourth, 0, numbers-1, order, &settings, numbers);
	ppm_strip_write(fourth, numbers, &settings, border);
	
	ppm_strip_write(fifth, numbers, &settings, border);
	radix_sort(fifth, numbers, &settings);
	ppm_strip_write(fifth, numbers, &settings, border);

	// Cleanup
	ppm_deinit(&settings);
	printf("Complete and written to %s\n", settings.file_name);
	
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
void count_sort(uint32_t arr[], const int n, const int exp, const struct ppm_opts_t * const settings){
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
		if(settings->write != NULL) { settings->write(output, n, settings); }
	}
 
	// Copy the output array to arr[], so that arr[] now
	// contains sorted numbers according to current digit
	for (i = 0; i < n; i++)
		arr[i] = output[i];
}

//-----------------------------------------------------
void radix_sort(uint32_t arr[], const int n, const struct ppm_opts_t * const settings)
{
	// Find the maximum number to know number of digits
	uint32_t m = get_max_average(arr, n);
 
	// Do counting sort for every digit. Note that instead
	// of passing digit number, exp is passed. exp is 10^i
	// where i is current digit number
	for (uint32_t exp = 1; m/exp > 0; exp *= 10){
		count_sort(arr, n, exp, settings);
		if(settings->write != NULL) { settings->write(arr, n, settings); }
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
void bubble_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings){
	int i, j;
	for (i = 0; i < n-1; i++){
		
		// Last i elements are already in place
		for (j = 0; j < n-i-1; j++){
			if (test(ppm_pix_get_average((union pixel_t)arr[j]), ppm_pix_get_average((union pixel_t)arr[j+1])))
			{
				swap(&arr[j], &arr[j+1]);
			}
		}
		if(settings->write != NULL) { settings->write(arr, n, settings); }
	}
}

//-----------------------------------------------------
uint32_t ppm_pix_get_average(const union pixel_t p){
	return (p.r+p.g+p.g)/3;
}

//-----------------------------------------------------
void selctn_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings) {
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
		if(settings->write != NULL) { settings->write(arr, n, settings); }
	}
	if(settings->write != NULL) { settings->write(arr, n, settings); }
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
void heap_sort(uint32_t arr[], int n, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings)
{
	
	assert(test);
	// Build heap (rearrange array)
	for (int i = n / 2 - 1; i >= 0; i--){
		heapify(arr, n, i, test);
		if(settings->write != NULL) { settings->write(arr, n, settings); }
	}
 
	// One by one extract an element from heap
	for (int i=n-1; i>=0; i--)
	{
		// Move current root to end
		swap(&arr[0], &arr[i]);
		
		// call max heapify on the reduced heap
		heapify(arr, i, 0, test);
		
		if(settings->write != NULL) { settings->write(arr, n, settings); }
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
void merge_sort(uint32_t arr[], int l, int r, bool (*test)(uint32_t, uint32_t), const struct ppm_opts_t * const settings, const int arr_len){
	
	assert(test);
	
	if (l < r)
	{
		// Same as (l+r)/2, but avoids overflow for
		// large l and h
		int m = l+(r-l)/2;
		
		// Sort first and second halves
		merge_sort(arr, l, m, test, settings, arr_len);
		merge_sort(arr, m+1, r,test,settings, arr_len);
		merge(arr, l, m, r, test);
		if(settings->write != NULL) { settings->write(arr, arr_len, settings); }
		
	}
	
}


