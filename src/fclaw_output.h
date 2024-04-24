/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FCLAW_OUTPUT_H
#define FCLAW_OUTPUT_H

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

struct fclaw_global;  /* This is a hack !! */

void fclaw_output_frame(struct fclaw_global * glob, int iframe);

/**
 * @brief Write out a checkpoint file. 
 *
 * This will only output if the checkpoint_out flag is set in fclaw_options.
 *
 * This will write two files: a fort_frame_[iframe].checkpoint file and a
 * fort_frame_[iframe].partition file.
 *
 * The partition file is optionally used when restarting to ensure that the
 * domain is partitioned in the same way as before.
 * 
 * @param glob the global context
 * @param iframe the frame number
 */
void fclaw_output_checkpoint(struct fclaw_global * glob, int iframe);

void fclaw2d_output_frame_tikz(struct fclaw_global* glob, int iframe);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
