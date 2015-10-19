/*
 * Copyright (c) 2015 
 * Leibniz Universitaet Hannover, Institut fuer Informationsverarbeitung (TNT)
 * Contact: Jan Voges <voges@tnt.uni-hannover.de>
 */

/*
 * This file is part of gomp.
 *
 * Gomp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gomp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gomp. If not, see <http://www.gnu.org/licenses/>
 */
 
#include "fqparser.h"
#include <string.h>

static void fqparser_init(fqparser_t* fqparser, FILE* fp)
{
    fqparser->fp = fp;
}

fqparser_t* fqparser_new(FILE* fp)
{
    fqparser_t* fqparser = (fqparser_t*)gomp_malloc(sizeof(fqparser_t));
    fqparser_init(fqparser, fp);
    return fqparser;
}

void fqparser_free(fqparser_t* fqparser)
{
    if (fqparser != NULL) {
        free(fqparser);
        fqparser = NULL;
    } else { /* fqparser == NULL */
        gomp_error("Tried to free NULL pointer.\n");
    }
}

bool fqparser_next(fqparser_t* fqparser)
{
    /* Try to read and parse the next four lines */
    
    /* RHEAD: starts with '@' */
    if (fgets(fqparser->curr.line, sizeof(fqparser->curr.line), fqparser->fp)) {
        if (*(fqparser->curr.line) == '@')
            fqparser->curr.rname = fqparser->curr.line;
        else
            gomp_error("Broken FASTQ RHEAD!\n");
    } else {
    	return false;
    }
    
    /* SEQ: A, C, G, T, N, a, c, g, t, n */
    if (fgets(fqparser->curr.line, sizeof(fqparser->curr.line), fqparser->fp)) {
    	/* We only check the first symbols for conformance */
        if (*(fqparser->curr.line) == 'A' || *(fqparser->curr.line) == 'a' ||
        	*(fqparser->curr.line) == 'C' || *(fqparser->curr.line) == 'c' ||
        	*(fqparser->curr.line) == 'G' || *(fqparser->curr.line) == 'g' ||
        	*(fqparser->curr.line) == 'T' || *(fqparser->curr.line) == 't' ||
        	*(fqparser->curr.line) == 'N' || *(fqparser->curr.line) == 'n' ||)
            fqparser->curr.rname = fqparser->curr.line;
        else
            gomp_error("Broken FASTQ SEQ!\n");
    } else {
    	return false;
    }
    
    /* DESC: starts with '+' */
    if (fgets(fqparser->curr.line, sizeof(fqparser->curr.line), fqparser->fp)) {
        if (*(fqparser->curr.line) == '+')
            fqparser->curr.desc = fqparser->curr.line;
        else
            gomp_error("Broken FASTQ DESC!\n");
    } else {
    	return false;
    }
    
    /* QUAL */
    if (fgets(fqparser->curr.line, sizeof(fqparser->curr.line), fqparser->fp)) {
        if (true)
            fqparser->curr.qual = fqparser->curr.line;
        else
            gomp_error("Broken FASTQ QUAL!\n");
    } else {
    	return false;
    }	
    
    return true;
}

