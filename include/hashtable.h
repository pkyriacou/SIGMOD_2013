/**
 * @file hashtable.h
 * @author Stefanos Taranto 1002662
 * @version 1.0
 * 
 * @brief This is the header file of the hashtable(chained)
 * 
 * 
 * 
 */

#ifndef _HT_H_
#define _HT_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/** @brief This is the structure of the hashtable node
 *
 *
 *  @param str The directory it holds
 *  @param valid If the record is valid(not used)
 *  @param next A pointer to the next node
 */
typedef struct _hnode{
	char *str;
	int valid;
	struct _hnode *next;
}hnode;

/** @brief Creates and Initializes the hashtable
 *
 *
 *  @param size The size of the hash table
 *  @return The hashtable
 */
hnode *hashInit(int size);

/** @brief Inserts a string to the hashtable
 *
 *
 *  @param ht The hashtable
 *  @param str The string
 *  @param size The size of the hashtable
 *  @return If the insert was done correctly
 */
int hashInsert(hnode *ht,char *str,int size);

/** @brief Checks if a string is in the hashtable
 *
 *
 *  @param ht The hashtable
 *  @param str The string
 *  @param size The size of the hashtable
 *  @return If the string was found
 */
int isInside(hnode *ht,char *str,int size);

/** @brief The hashfunction
 *
 *
 *  @param str The string
 *  @param size The size of the hashtable
 *  @return The index that maps with the string
 */
int hashFunction(char *str,int size);

/** @brief Frees the hashtable
 *
 *
 *  @param ht The hashtable
 *  @param size The size of the hashtable
 *  @return If the free was done correctly
 */
int freeHash(hnode *ht,int size);

#endif
