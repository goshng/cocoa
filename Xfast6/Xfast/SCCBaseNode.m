

#import "SCCBaseNode.h"

@implementation SCCBaseNode

@synthesize nodeTitle, children, isLeaf, urlString, key, nodeIcon;

// -------------------------------------------------------------------------------
//	init
// -------------------------------------------------------------------------------
- (id)init
{
	self = [super init];
    if (self)
	{
        self.nodeTitle = @"BaseNode Untitled";
        
		[self setChildren:[NSMutableArray array]];
		[self setLeaf:NO];			// container by default
	}
	return self;
}

// -------------------------------------------------------------------------------
//	initLeaf
// -------------------------------------------------------------------------------
- (id)initLeaf
{
	self = [self init];
    if (self)
	{
		[self setLeaf:YES];
	}
	return self;
}

// -------------------------------------------------------------------------------
//	setLeaf:flag
// -------------------------------------------------------------------------------
- (void)setLeaf:(BOOL)flag
{
	isLeaf = flag;
	if (isLeaf)
		[self setChildren:[NSMutableArray arrayWithObject:self]];
	else
		[self setChildren:[NSMutableArray array]];
}

// -------------------------------------------------------------------------------
//	compare:aNode
// -------------------------------------------------------------------------------
- (NSComparisonResult)compare:(SCCBaseNode *)aNode
{
	return [[[self nodeTitle] lowercaseString] compare:[[aNode nodeTitle] lowercaseString]];
}


#pragma mark - Drag and Drop

// -------------------------------------------------------------------------------
//	isDraggable
// -------------------------------------------------------------------------------
- (BOOL)isDraggable
{
	BOOL result = YES;
	if ([[self urlString] isAbsolutePath] || [self nodeIcon] == nil)
		result = NO;	// don't allow file system objects to be dragged or special group nodes
	return result;
}

// -------------------------------------------------------------------------------
//	parentFromArray:array
//
//	Finds the receiver's parent from the nodes contained in the array.
// -------------------------------------------------------------------------------
- (id)parentFromArray:(NSArray *)array
{
	id result = nil;
	
	for (id node in array)
	{
		if (node == self)	// If we are in the root array, return nil
			break;
		
		if ([[node children] indexOfObjectIdenticalTo:self] != NSNotFound)
        {
            result = node;
            break;
        }
            
		if (![node isLeaf])
		{
			id innerNode = [self parentFromArray:[node children]];
			if (innerNode)
			{
				result = innerNode;
				break;
			}
		}
	}
    
	return result;
}

// -------------------------------------------------------------------------------
//	removeObjectFromChildren:obj
//
//	Recursive method which searches children and children of all sub-nodes
//	to remove the given object.
// -------------------------------------------------------------------------------
- (void)removeObjectFromChildren:(id)obj
{
	// remove object from children or the children of any sub-nodes
    for (id node in self.children)
	{
		if (node == obj)
		{
			[self.children removeObjectIdenticalTo:obj];
			return;
		}
		
		if (![node isLeaf])
			[node removeObjectFromChildren:obj];
	}
}

// -------------------------------------------------------------------------------
//	descendants
//
//	Generates an array of all descendants.
// -------------------------------------------------------------------------------
- (NSArray *)descendants
{
    NSMutableArray	*descendants = [NSMutableArray array];
	id node = nil;
	for (node in self.children)
	{
		[descendants addObject:node];
		
		if (![node isLeaf])
			[descendants addObjectsFromArray:[node descendants]];	// Recursive - will go down the chain to get all
	}
	return descendants;
}

// -------------------------------------------------------------------------------
//	allChildLeafs:
//
//	Generates an array of all leafs in children and children of all sub-nodes.
//	Useful for generating a list of leaf-only nodes.
// -------------------------------------------------------------------------------
- (NSArray *)allChildLeafs
{
    NSMutableArray	*childLeafs = [NSMutableArray array];
	id node = nil;
	
	for (node in self.children)
	{
		if ([node isLeaf])
			[childLeafs addObject:node];
		else
			[childLeafs addObjectsFromArray:[node allChildLeafs]];	// Recursive - will go down the chain to get all
	}
	return childLeafs;
}

// -------------------------------------------------------------------------------
//	groupChildren
//
//	Returns only the children that are group nodes.
// -------------------------------------------------------------------------------
- (NSArray *)groupChildren
{
    NSMutableArray	*groupChildren = [NSMutableArray array];
	SCCBaseNode		*child;
	
	for (child in self.children)
	{
		if (![child isLeaf])
			[groupChildren addObject:child];
	}
	return groupChildren;
}

// -------------------------------------------------------------------------------
//	isDescendantOfOrOneOfNodes:nodes
//
//	Returns YES if self is contained anywhere inside the children or children of
//	sub-nodes of the nodes contained inside the given array.
// -------------------------------------------------------------------------------
- (BOOL)isDescendantOfOrOneOfNodes:(NSArray *)nodes
{
    // returns YES if we are contained anywhere inside the array passed in, including inside sub-nodes
 	id node = nil;
    for (node in nodes)
	{
		if (node == self)
			return YES;		// we found ourselv
		
		// check all the sub-nodes
		if (![node isLeaf])
		{
			if ([self isDescendantOfOrOneOfNodes:[node children]])
				return YES;
		}
    }
	
    return NO;
}

// -------------------------------------------------------------------------------
//	isDescendantOfNodes:nodes
//
//	Returns YES if any node in the array passed in is an ancestor of ours.
// -------------------------------------------------------------------------------
- (BOOL)isDescendantOfNodes:(NSArray *)nodes
{
	id node = nil;
    for (node in nodes)
	{
		// check all the sub-nodes
		if (![node isLeaf])
		{
			if ([self isDescendantOfOrOneOfNodes:[node children]])
				return YES;
		}
    }
    
	return NO;
}

// -------------------------------------------------------------------------------
//	indexPathInArray:array
//
//	Returns the index path of within the given array, useful for drag and drop.
// -------------------------------------------------------------------------------
- (NSIndexPath *)indexPathInArray:(NSArray *)array
{
	NSIndexPath	*indexPath = nil;
	NSMutableArray *reverseIndexes = [NSMutableArray array];
	id parent, doc = self;
	NSInteger index;
	
	parent = [doc parentFromArray:array];
    while (parent)
	{
		index = [[parent children] indexOfObjectIdenticalTo:doc];
		if (index == NSNotFound)
			return nil;
		
		[reverseIndexes addObject:[NSNumber numberWithInt:(int)index]];
		doc = parent;
	}
	
	// If parent is nil, we should just be in the parent array
	index = [array indexOfObjectIdenticalTo:doc];
	if (index == NSNotFound)
		return nil;
	[reverseIndexes addObject:[NSNumber numberWithInt:(int)index]];
	
	// now build the index path
    NSEnumerator *re = [reverseIndexes reverseObjectEnumerator];
    NSNumber *indexNumber;
    for (indexNumber in re)
    {
        if (indexPath == nil)
            indexPath = [NSIndexPath indexPathWithIndex:[indexNumber intValue]];
        else
            indexPath = [indexPath indexPathByAddingIndex:[indexNumber intValue]];
    }
	
	return indexPath;
}


#pragma mark - Archiving And Copying Support

// -------------------------------------------------------------------------------
//	mutableKeys:
//
//	Override this method to maintain support for archiving and copying.
// -------------------------------------------------------------------------------
- (NSArray *)mutableKeys
{
	return [NSArray arrayWithObjects:
                @"nodeTitle",
                @"isLeaf",		// isLeaf MUST come before children for initWithDictionary: to work
                @"children", 
                @"nodeIcon",
                @"urlString",
            nil];
}

// -------------------------------------------------------------------------------
//	initWithDictionary:dictionary
// -------------------------------------------------------------------------------
- (id)initWithDictionary:(NSDictionary *)dictionary
{
	self = [self init];
    if (self)
    {
        NSString *aKey;
        for (aKey in [self mutableKeys])
        {
            if ([aKey isEqualToString:@"children"])
            {
                if ([[dictionary objectForKey:@"isLeaf"] boolValue])
                    [self setChildren:[NSMutableArray arrayWithObject:self]];
                else
                {
                    NSArray *dictChildren = [dictionary objectForKey:aKey];
                    NSMutableArray *newChildren = [NSMutableArray array];
                    
                    for (id node in dictChildren)
                    {
                        id newNode = [[[self class] alloc] initWithDictionary:node];
                        [newChildren addObject:newNode];
                    }
                    [self setChildren:newChildren];
                }
            }
            else
            {
                [self setValue:[dictionary objectForKey:aKey] forKey:aKey];
            }
        }
    }
	return self;
}

// -------------------------------------------------------------------------------
//	dictionaryRepresentation
// -------------------------------------------------------------------------------
- (NSDictionary *)dictionaryRepresentation
{
	NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];

	for (NSString *aKey in [self mutableKeys])
    {
		// convert all children to dictionaries
		if ([aKey isEqualToString:@"children"])
		{
			if (!self.isLeaf)
			{
				NSMutableArray *dictChildren = [NSMutableArray array];
				for (id node in self.children)
				{
					[dictChildren addObject:[node dictionaryRepresentation]];
				}
				
				[dictionary setObject:dictChildren forKey:aKey];
			}
		}
		else if ([self valueForKey:aKey])
		{
			[dictionary setObject:[self valueForKey:aKey] forKey:aKey];
		}
	}
	return dictionary;
}

// -------------------------------------------------------------------------------
//	initWithCoder:coder
// -------------------------------------------------------------------------------
- (id)initWithCoder:(NSCoder *)coder
{		
	self = [self init];
	if (self)
    {
        for (NSString *akey in [self mutableKeys])
            [self setValue:[coder decodeObjectForKey:akey] forKey:akey];
	}
	return self;
}

// -------------------------------------------------------------------------------
//	encodeWithCoder:coder
// -------------------------------------------------------------------------------
- (void)encodeWithCoder:(NSCoder *)coder
{
    for (NSString *akey in [self mutableKeys])
    {
		[coder encodeObject:[self valueForKey:akey] forKey:akey];
    }
}

// -------------------------------------------------------------------------------
//	copyWithZone:zone
// -------------------------------------------------------------------------------
- (id)copyWithZone:(NSZone *)zone
{
	id newNode = [[[self class] allocWithZone:zone] init];
	
	for (NSString *akey in [self mutableKeys])
		[newNode setValue:[self valueForKey:akey] forKey:akey];
	
	return newNode;
}

// -------------------------------------------------------------------------------
//	setNilValueForKey:key
//
//	Override this for any non-object values
// -------------------------------------------------------------------------------
- (void)setNilValueForKey:(NSString *)aKey
{
	if ([aKey isEqualToString:@"isLeaf"])
		isLeaf = NO;
	else
		[super setNilValueForKey:aKey];
}

@end
