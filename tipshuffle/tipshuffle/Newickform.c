/* See 
 * http://yuweibioinfo.blogspot.com/2008/10/newick-tree-parser-in-c-make-use-of.html
 * for the original author of this C file.
 * */
#define __NEWICKFORM_C__

#include "seqUtil.h"
#include "Newickform.h"

static newick_node** globalTipNode;
static size_t globalTipNodeI;

newick_node* parseTree(char *str)
{
	newick_node *node;
	newick_child *child;
	char *pcCurrent;
	char *pcStart;
	char *pcColon = NULL;
	char cTemp;
	int iCount;

	if (sDEBUG == 1)
	{
		printf("%s\n", str);
	}
	pcStart = str;

	if (*pcStart != '(')
	{
		// Leaf node. Separate taxon name from distance. If distance not exist then take care of taxon name only
		pcCurrent = str;
		while (*pcCurrent != '\0')
		{
			if (*pcCurrent == ':')
			{
				pcColon = pcCurrent;
			}
			pcCurrent++;
		}
		node = (newick_node*)seqMalloc(sizeof(newick_node));
		if (pcColon == NULL)
		{
			// Taxon only
			node->taxon = (char*)seqMalloc(strlen(pcStart) + 1);
			memcpy(node->taxon, pcStart, strlen(pcStart));
		}
		else
		{
			// Taxon
			*pcColon = '\0';
			node->taxon = (char*)seqMalloc(strlen(pcStart) + 1);
			memcpy(node->taxon, pcStart, strlen(pcStart));
			*pcColon = ':';
			// Distance
			pcColon++;
			node->dist = (float)atof(pcColon);
		}
		node->childNum = 0;
	}
	else
	{
		// Create node
		node = (newick_node*)seqMalloc(sizeof(newick_node));
		child = NULL;
		// Search for all child nodes
		// Find all ',' until corresponding ')' is encountered
		iCount = 0;
		pcStart++;
		pcCurrent = pcStart;
		while (iCount >= 0)
		{
			switch (*pcCurrent)
			{
				case '(':
					// Find corresponding ')' by counting
					pcStart = pcCurrent;
					pcCurrent++;
					iCount++;
					while (iCount > 0)
					{
						if (*pcCurrent == '(')
						{
							iCount++;
						}
						else if (*pcCurrent == ')')
						{
							iCount--;
						}
						pcCurrent++;
					}
					while (*pcCurrent != ',' && *pcCurrent != ')')
					{
						pcCurrent++;
					}
 					cTemp = *pcCurrent;
					*pcCurrent = '\0';
					// Create a child node
					if (child == NULL)
					{
						node->child = (newick_child*)seqMalloc(sizeof(newick_child));
						node->childNum = 1;
						child = node->child;
					}
					else
					{
						child->next = (newick_child*)seqMalloc(sizeof(newick_child));
						node->childNum++;
						child = child->next;
					}
					child->node = parseTree(pcStart);
					*pcCurrent = cTemp;
					if (*pcCurrent != ')')
					{
						pcCurrent++;
					}
				break;

				case ')':
					// End of tihs tree. Go to next part to retrieve distance
					iCount--;
				break;

				case ',':
					// Impossible separation since according to the algorithm, this symbol will never encountered.
					// Currently don't handle this and don't create any node
				break;

				default:
					// leaf node encountered
					pcStart = pcCurrent;
					while (*pcCurrent != ',' && *pcCurrent != ')')
					{
						pcCurrent++;
					}
					cTemp = *pcCurrent;
					*pcCurrent = '\0';
					// Create a child node
					if (child == NULL)
					{
						node->child = (newick_child*)seqMalloc(sizeof(newick_child));
						node->childNum = 1;
						child = node->child;
					}
					else
					{
						child->next = (newick_child*)seqMalloc(sizeof(newick_child));
						node->childNum++;
						child = child->next;
					}
					child->node = parseTree(pcStart);
					*pcCurrent = cTemp;
					if (*pcCurrent != ')')
					{
						pcCurrent++;
					}
				break;
			}
		}

		// If start at ':', then the internal node has no name.
		pcCurrent++;
		if (*pcCurrent == ':')
		{
			pcStart = pcCurrent + 1;
			while (*pcCurrent != '\0' && *pcCurrent != ';')
			{
				pcCurrent++;
			}
			cTemp = *pcCurrent;
			*pcCurrent = '\0';
			node->dist = (float)atof(pcStart);
			*pcCurrent = cTemp;
		}
		else if (*pcCurrent != ';' && *pcCurrent != '\0')
		{
			// Find ':' to retrieve distance, if any.
			// At this time *pcCurrent should equal to ')'
			pcStart = pcCurrent;
			while (*pcCurrent != ':')
			{
				pcCurrent++;
			}
			cTemp = *pcCurrent;
			*pcCurrent = '\0';
			node->taxon = seqMalloc(strlen(pcStart) + 1);
			memcpy(node->taxon, pcStart, strlen(pcStart));
			*pcCurrent = cTemp;
			pcCurrent++;
			pcStart = pcCurrent;
			while (*pcCurrent != '\0' && *pcCurrent != ';')
			{
				pcCurrent++;
			}
			cTemp = *pcCurrent;
			*pcCurrent = '\0';
			node->dist = (float)atof(pcStart);
			*pcCurrent = cTemp;
		}
	}

	return node;
}

void printTree(newick_node *root)
{
	newick_child *child;
	if (root->childNum == 0)
	{
		printf("%s:%0.6f", root->taxon, root->dist);
	}
	else
	{
		child = root->child;
		printf("(");
		while (child != NULL)
		{
			printTree(child->node);
			if (child->next != NULL)
			{
				printf(",");
			}
			child = child->next;
		}
		if (root->taxon != NULL)
		{
			printf(")%s:%0.6f", root->taxon, root->dist);
		}
		else
		{
			printf("):%0.6f", root->dist);
		}
	}
}

void collectTipOfTree(newick_node *root)
{
	newick_child *child;
	if (root->childNum == 0)
	{
        globalTipNode[globalTipNodeI++] = root;
	}
	else
	{
		child = root->child;
		while (child != NULL)
		{
            collectTipOfTree(child->node);
			child = child->next;
		}
	}
}

void shuffleTipOfTree(newick_node *root, size_t *a)
{
    size_t i;
    /* Count the number of tips. */
    size_t c = leafCount(root);
    
    globalTipNode = (newick_node**) malloc(c * sizeof(newick_node*));
    globalTipNodeI = 0;
    collectTipOfTree(root);
    
    /* Use the collected leaves. */
    char **taxa = (char**)malloc(c * sizeof(char*));
    for (i = 0; i < c; i++) {
        taxa[i] = globalTipNode[a[i]]->taxon;
    }
    for (i = 0; i < c; i++) {
        globalTipNode[i]->taxon = taxa[i];
    }
    
    free(globalTipNode);
    free(taxa);
    
//    printf("Number of leaves in the tree is %zu\n", globalTipNodeI);
//    char *t;
//    printf("B: %s - %s\n", globalTipNode[0]->taxon, globalTipNode[1]->taxon);
//    t = globalTipNode[0]->taxon;
//    globalTipNode[0]->taxon = globalTipNode[1]->taxon;
//    globalTipNode[1]->taxon = t;
//    printf("A: %s - %s\n", globalTipNode[0]->taxon, globalTipNode[1]->taxon);
}

/* Count leaf nodes in a binary tree. */
size_t leafCount(newick_node *root)
{
    size_t c = 0;
	newick_child *child;
	if (root->childNum == 0)
	{
        c = 1;
	}
	else
	{
		child = root->child;
		while (child != NULL)
		{
            c += leafCount(child->node);
			child = child->next;
		}
	}
    return c;
}

