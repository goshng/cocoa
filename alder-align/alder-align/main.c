//
//  main.c
//  alder-align
//
//  Created by Sang Chul Choi on 8/7/13.
//  Copyright (c) 2013 Sang Chul Choi. All rights reserved.
//

#include <stdio.h>
#include <nglogc/log.h>
#include "alder_genome.h"
#include "PackedArray.h"
#include "alder_suffix_array.h"
#include "alder_logger.h"
#include "alder_index_genome.h"
#include "alder_align_reads.h"
#include "Parameters.h"
#include "mem/sparse_sa_test.h"

int main(int argc, const char * argv[])
{
    alder_logger_initialize();
    logc_log(MAIN_LOGGER, LOG_BASIC,
             "alder-align starts.");
    
    alder_logger_finalize();
    return 0;
}

