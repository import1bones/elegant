#include <stdio.h>
#include <elegant.h>

// Functional quicksort using Elegant library
// Returns a new sorted array, does not modify input

elegant_array_t* functional_quicksort(elegant_array_t* arr) {
    size_t len = ELEGANT_LENGTH(arr);
    if (len <= 1) return arr;

    int pivot = ELEGANT_GET(arr, 0, int);
    // Partition: less, equal, greater
    AUTO(less, FILTER_INT(arr, x < pivot));
    AUTO(equal, FILTER_INT(arr, x == pivot));
    AUTO(greater, FILTER_INT(arr, x > pivot));

    AUTO(sorted_less, functional_quicksort(less));
    AUTO(sorted_greater, functional_quicksort(greater));

    // Concatenate: sorted_less + equal + sorted_greater
    size_t l1 = ELEGANT_LENGTH(sorted_less);
    size_t l2 = ELEGANT_LENGTH(equal);
    size_t l3 = ELEGANT_LENGTH(sorted_greater);
    size_t total = l1 + l2 + l3;
    elegant_array_t* result = elegant_create_array_impl(sizeof(int), NULL, total);
    int* data = (int*)ELEGANT_RAW_PTR(result);
    if (l1) memcpy(data, ELEGANT_RAW_PTR(sorted_less), l1 * sizeof(int));
    if (l2) memcpy(data + l1, ELEGANT_RAW_PTR(equal), l2 * sizeof(int));
    if (l3) memcpy(data + l1 + l2, ELEGANT_RAW_PTR(sorted_greater), l3 * sizeof(int));
    return result;
}

int main() {
    puts("Functional Quicksort Demo (Elegant)");
    AUTO(arr, elegant_create_array_int(9, 3, 7, 1, 4, 8, 2, 5, 6));
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(arr); i++) printf("%d ", ELEGANT_GET(arr, i, int));
    printf("\n");

    AUTO(sorted, functional_quicksort(arr));
    printf("Sorted:   ");
    for (size_t i = 0; i < ELEGANT_LENGTH(sorted); i++) printf("%d ", ELEGANT_GET(sorted, i, int));
    printf("\n");
    return 0;
}
