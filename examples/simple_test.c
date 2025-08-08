#include <stdio.h>
#include <elegant.h>

int main() {
    printf("Testing basic array creation...\n");
    
    AUTO(numbers, elegant_create_array_int(1, 2, 3, 4, 5, 6));
    printf("Array created successfully\n");
    
    printf("Array length: %zu\n", ELEGANT_LENGTH(numbers));
    
    printf("Elements: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(numbers); i++) {
        printf("%d ", ELEGANT_GET(numbers, i, int));
    }
    printf("\n");
    
    printf("Testing MAP...\n");
    AUTO(doubled, MAP_INT(numbers, x * 2));
    printf("MAP completed\n");
    
    printf("Doubled elements: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(doubled); i++) {
        printf("%d ", ELEGANT_GET(doubled, i, int));
    }
    printf("\n");
    
    return 0;
}
