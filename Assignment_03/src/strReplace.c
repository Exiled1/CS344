// This code was gotten off of geeksforgeeks.org for string replacement.
// This code is not mine, and was used to correctly use a string replacement algorithm.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* replaceWord(const char* old_string, const char* old_word, const char* replacement_word)
{
    char* result;
    int occ_count, occurrences = 0;
    int newWlen = strlen(replacement_word);
    int oldWlen = strlen(old_word);
  
    // Counting the number of times old word
    // occur in the string
    for (occ_count = 0; old_string[occ_count] != '\0'; occ_count++) {
        if (strstr(&old_string[occ_count], old_word) == &old_string[occ_count]) {
            occurrences++;
  
            // Jumping to index after the old word.
            occ_count += oldWlen - 1;
        }
    }
  
    // Making new string of enough length
    result = (char*)malloc(occ_count + occurrences * (newWlen - oldWlen) + 1);
  
    occ_count = 0;
    while (*old_string) { // While the pointer exists.
        // compare the substring with the result
        if (strstr(old_string, old_word) == old_string) {
            strcpy(&result[occ_count], replacement_word);
            occ_count += newWlen;
            old_string += oldWlen;
        }
        else
            result[occ_count++] = *old_string++;
    }

    result[occ_count] = '\0'; // Replace the last space as a null terminator for strings.
    return result;
}