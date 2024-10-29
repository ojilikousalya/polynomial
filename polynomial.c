#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cJSON.h"

// Function to decode a value from a given base
unsigned long long decode_value(const char *value, int base) {
    unsigned long long result = 0;
    int length = strlen(value);
    
    for (int i = 0; i < length; i++) {
        char digit = value[length - 1 - i];
        unsigned long long digit_value;
        
        if (digit >= '0' && digit <= '9') {
            digit_value = digit - '0';
        } else if (digit >= 'a' && digit <= 'z') {
            digit_value = digit - 'a' + 10;
        } else if (digit >= 'A' && digit <= 'Z') {
            digit_value = digit - 'A' + 10;
        } else {
            continue; // invalid character, ignore
        }
        
        result += digit_value * pow(base, i);
    }
    
    return result;
}

int main() {
    // Open the JSON file
    FILE *file = fopen("input.json", "r");
    if (!file) {
        fprintf(stderr, "Could not open JSON file.\n");
        return 1;
    }
    
    // Read the file content into a string
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *json_data = malloc(length + 1);
    fread(json_data, 1, length, file);
    json_data[length] = '\0'; // Null-terminate the string
    fclose(file);
    
    // Parse the JSON data
    cJSON *json = cJSON_Parse(json_data);
    free(json_data); // Free the JSON data buffer after parsing

    if (!json) {
        fprintf(stderr, "Error parsing JSON data.\n");
        return 1;
    }
    
    // Extract the keys
    cJSON *keys = cJSON_GetObjectItem(json, "keys");
    int n = cJSON_GetObjectItem(keys, "n")->valueint;
    int k = cJSON_GetObjectItem(keys, "k")->valueint;

    unsigned long long roots[k];

    // Decode the first k values
    for (int i = 0; i < k; i++) {
        char key[3];
        snprintf(key, sizeof(key), "%d", i + 1);
        cJSON *item = cJSON_GetObjectItem(json, key);
        int base = atoi(cJSON_GetObjectItem(item, "base")->valuestring);
        const char *value = cJSON_GetObjectItem(item, "value")->valuestring;

        roots[i] = decode_value(value, base);
    }

    // Calculate the constant term c
    unsigned long long c = 1; // Start with 1 for multiplication
    for (int i = 0; i < k; i++) {
        c *= roots[i]; // Product of the roots
    }
    
    // Since m = k - 1, we determine (-1)^m
    int m = k - 1;
    if (m % 2 != 0) {
        c = -c; // Apply (-1)^m
    }
    
    // Output the secret constant c
    printf("The secret constant c is: %llu\n", c);
    
    // Clean up
    cJSON_Delete(json);
    return 0;
}
