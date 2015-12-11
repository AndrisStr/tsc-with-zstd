/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2015, Leibniz Universitaet Hannover, Institut fuer
 * Informationsverarbeitung (TNT)
 * Contact: <voges@tnt.uni-hannover.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the TNT nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "tsclib.h"
#include "samcodec.h"
#include "version.h"
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Options/flags from getopt
static const char *opt_input = NULL;
static const char *opt_output = NULL;
static const char *opt_blocksz = NULL;
static bool opt_flag_force = false;
static bool opt_flag_stats = false;

// Initializing global vars from 'tsclib.h'
str_t *tsc_prog_name = NULL;
str_t *tsc_version = NULL;
str_t *tsc_in_fname = NULL;
str_t *tsc_out_fname = NULL;
FILE *tsc_in_fp = NULL;
FILE *tsc_out_fp = NULL;
tsc_mode_t tsc_mode = TSC_MODE_COMPRESS;
tsc_loglvl_t tsc_loglvl = TSC_LOG_DEFAULT;
bool tsc_stats = false;
unsigned int tsc_blocksz = 0;

static void print_version(void)
{
    printf("%s %s\n", tsc_prog_name->s, tsc_version->s);
}

static void print_copyright(void)
{
    printf("Copyright (c) 2015\n");
    printf("Leibniz Universitaet Hannover, Institut fuer "
           "Informationsverarbeitung (TNT)\n");
    printf("Contact: Jan Voges <voges@tnt.uni-hannover.de>\n");
}

static void print_help(void)
{
    print_version();
    print_copyright();
    printf("\n");
    printf("Usage:\n");
    printf("  Compress  : tsc [-o output] [-fsv] <file.sam>\n");
    printf("  Decompress: tsc -d [-o output] [-fsv] <file.tsc>\n");
    printf("  Info      : tsc -i [-v] <file.tsc>\n");
    printf("\n");
    printf("Options:\n");
    printf("  -b  --blocksz     Specify block size\n");
    printf("  -d  --decompress  Decompress\n");
    printf("  -f, --force       Force overwriting of output file(s)\n");
    printf("  -h, --help        Print this help\n");
    printf("  -i, --info        Print information about tsc file\n");
    printf("  -l, --log         Log level (0-3, default: 0)\n");
    printf("  -o, --output      Specify output file\n");
    printf("  -s, --stats       Print (de-)compression statistics\n");
    printf("  -v, --verbose     Verbose output (equals --log=2)\n");
    printf("  -V, --version     Display program version\n");
    printf("\n");
}

static void parse_options(int argc, char *argv[])
{
    int opt;

    static struct option long_options[] = {
        { "blocksz",    required_argument, NULL, 'b'},
        { "decompress", no_argument,       NULL, 'd'},
        { "force",      no_argument,       NULL, 'f'},
        { "help",       no_argument,       NULL, 'h'},
        { "info",       no_argument,       NULL, 'i'},
        { "log",        required_argument, NULL, 'l'},
        { "output",     required_argument, NULL, 'o'},
        { "stats",      no_argument,       NULL, 's'},
        { "verbose",    no_argument,       NULL, 'v'},
        { "version",    no_argument,       NULL, 'V'},
        { NULL,         0,                 NULL,  0 }
    };

    const char *short_options = "b:dfhil:o:svV";

    do {
        int opt_idx = 0;
        opt = getopt_long(argc, argv, short_options, long_options, &opt_idx);
        switch (opt) {
        case -1:
            break;
        case 'b':
            opt_blocksz = optarg;
            break;
        case 'd':
            tsc_mode = TSC_MODE_DECOMPRESS;
            break;
        case 'f':
            opt_flag_force = true;
            break;
        case 'h':
            print_help();
            exit(EXIT_SUCCESS);
            break;
        case 'i':
            tsc_mode = TSC_MODE_INFO;
            break;
        case 'l':
            if (atoi(optarg) >= 0 && atoi(optarg) <= 3) {
                tsc_loglvl = atoi(optarg);
            } else {
                tsc_error("Log level must be 0-3\n");
            }
            break;
        case 'o':
            opt_output = optarg;
            break;
        case 's':
            opt_flag_stats = true;
            break;
        case 'v':
            tsc_loglvl = TSC_LOG_VERBOSE;
            break;
        case 'V':
            print_version();
            print_copyright();
            exit(EXIT_SUCCESS);
            break;
        default:
            exit(EXIT_FAILURE);
        }
    } while (opt != -1);

    // The input file must be the one remaining command line argument
    if (argc - optind > 1)
        tsc_error("Only one input file allowed\n");
    else if (argc - optind < 1)
        tsc_error("Input file missing\n");
    else
        opt_input = argv[optind];
}

static const char *fext(const char* path)
{
    const char *dot = strrchr(path, '.');
    if (!dot || dot == path) { return ""; }
    return (dot + 1);
}

static void handle_signal(int sig)
{
    signal(sig, SIG_IGN); // Ignore the signal
    tsc_log(TSC_LOG_DEFAULT, TSC_LOG_DEFAULT, "Catched signal: %d\n", sig);
    tsc_log(TSC_LOG_DEFAULT, TSC_LOG_DEFAULT, "Cleaning up ...\n");
    tsc_cleanup();
    signal(sig, SIG_DFL); // Invoke default signal action
    raise(sig);
}

int main(int argc, char *argv[])
{
    tsc_prog_name = str_new();
    tsc_version = str_new();
    str_append_cstr(tsc_version, VERSION);
    tsc_in_fname = str_new();
    tsc_out_fname = str_new();

    // Determine program name and truncate path if needed
    const char *prog_name = argv[0];
    char *p;
    if ((p = strrchr(argv[0], '/')) != NULL)
        prog_name = p + 1;
    str_copy_cstr(tsc_prog_name, prog_name);

    // If invoked as 'de...' switch to decompressor mode
    if (!strncmp(tsc_prog_name->s, "de", 2)) tsc_mode = TSC_MODE_DECOMPRESS;

    // Invoke custom signal handler
    signal(SIGHUP,  handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGABRT, handle_signal);
    signal(SIGPIPE, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGXCPU, handle_signal);
    signal(SIGXFSZ, handle_signal);

    // Parse command line options and check them for sanity
    parse_options(argc, argv);
    str_copy_cstr(tsc_in_fname, opt_input);
    if (opt_flag_stats) tsc_stats = true;

    if (tsc_mode == TSC_MODE_COMPRESS) {
        // All possible options are legal in this mode
        if (!opt_blocksz) {
            tsc_blocksz = 10000; // Default value
        } else {
            if (atoi(opt_blocksz) <= 0) {
                tsc_error("Block size must be positive\n");
            } else {
                tsc_blocksz = (unsigned int)atoi(opt_blocksz);
            }
        }
    } else if (tsc_mode == TSC_MODE_DECOMPRESS) {
        // Option -b is illegal in this mode
        if (opt_blocksz) {
            tsc_error("Illegal option(s) detected\n");
        }
    } else { // TSC_MODE_INFO */
        // Options -bfos are illegal in this mode
        if (opt_blocksz || opt_flag_force || opt_output || opt_flag_stats) {
            tsc_error("Illegal option(s) detected\n");
        }
    }

    if (access((const char *)tsc_in_fname->s, F_OK | R_OK))
        tsc_error("Cannot access input file: %s\n", tsc_in_fname->s);

    if (tsc_mode == TSC_MODE_COMPRESS) {
        // Check I/O
        if (strcmp(fext((const char *)tsc_in_fname->s), "sam"))
            tsc_error("Input file extension must be 'sam'\n");

        if (opt_output == NULL) {
            str_copy_str(tsc_out_fname, tsc_in_fname);
            str_append_cstr(tsc_out_fname, ".tsc");
        } else {
            str_copy_cstr(tsc_out_fname, opt_output);
        }

        if (!access((const char *)tsc_out_fname->s, F_OK | W_OK)
                && opt_flag_force == false) {
            tsc_error("Output file already exists: %s\n", tsc_out_fname->s);
            exit(EXIT_FAILURE);
        }

        // Invoke compressor
        tsc_in_fp = tsc_fopen((const char *)tsc_in_fname->s, "r");
        tsc_out_fp = tsc_fopen((const char *)tsc_out_fname->s, "wb");
        tsc_log(TSC_LOG_DEFAULT, "Compressing: %s\n", tsc_in_fname->s);
        samenc_t *samenc = samenc_new(tsc_in_fp, tsc_out_fp, tsc_blocksz);
        samenc_encode(samenc);
        samenc_free(samenc);
        tsc_log(TSC_LOG_DEFAULT, "Finished: %s\n", tsc_out_fname->s);
        tsc_fclose(tsc_in_fp);
        tsc_fclose(tsc_out_fp);
    } else  if (tsc_mode == TSC_MODE_DECOMPRESS) {
        // Check I/O
        if (strcmp(fext((const char *)tsc_in_fname->s), "tsc"))
            tsc_error("Input file extension must be 'tsc'\n");

        if (opt_output == NULL) {
            str_copy_str(tsc_out_fname, tsc_in_fname);
            str_trunc(tsc_out_fname, 4); // strip '.tsc'
            if (strcmp(fext((const char *)tsc_out_fname->s), "sam"))
                str_append_cstr(tsc_out_fname, ".sam");
        } else {
            str_copy_cstr(tsc_out_fname, opt_output);
        }

        if (!access((const char *)tsc_out_fname->s, F_OK | W_OK)
                && opt_flag_force == false) {
            tsc_error("Output file already exists: %s\n", tsc_out_fname->s);
            exit(EXIT_FAILURE);
        }

        // Invoke decompressor
        tsc_in_fp = tsc_fopen((const char *)tsc_in_fname->s, "rb");
        tsc_out_fp = tsc_fopen((const char *)tsc_out_fname->s, "w");
        tsc_log(TSC_LOG_DEFAULT, "Decompressing: %s\n", tsc_in_fname->s);
        samdec_t * samdec = samdec_new(tsc_in_fp, tsc_out_fp);
        samdec_decode(samdec);
        samdec_free(samdec);
        tsc_log(TSC_LOG_DEFAULT, "Finished: %s\n", tsc_out_fname->s);
        tsc_fclose(tsc_in_fp);
        tsc_fclose(tsc_out_fp);
    } else { // TSC_MODE_INFO
        // Check I/O
        if (strcmp(fext((const char *)tsc_in_fname->s), "tsc"))
            tsc_error("Input file extension must be 'tsc'\n");

        // Read information from compressed tsc file
        tsc_in_fp = tsc_fopen((const char *)tsc_in_fname->s, "rb");
        tsc_log(TSC_LOG_DEFAULT, "Reading information: %s\n", tsc_in_fname->s);
        samdec_t *samdec = samdec_new(tsc_in_fp, NULL);
        samdec_info(samdec);
        samdec_free(samdec);
        tsc_fclose(tsc_in_fp);
    }

    str_free(tsc_prog_name);
    str_free(tsc_version);
    str_free(tsc_in_fname);
    str_free(tsc_out_fname);

    return EXIT_SUCCESS;
}

