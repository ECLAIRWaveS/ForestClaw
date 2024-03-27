/*
Copyright (c) 2012-2022 Carsten Burstedde, Donna Calhoun, Scott Aiton
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

#include <fclaw_global.h>
#include <fclaw_clawpatch.h>
#include <fclaw_clawpatch_options.h>
#include <fclaw_forestclaw.h>
#include <fclaw_convenience.h>
#include <test.hpp>

TEST_CASE("2d fclaw_clawpatch_vtable_initialize stores two separate vtables in two separate globs")
{
	fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, 1);
	fclaw_global_t* glob1 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob1, domain);
	fclaw_global_t* glob2 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob2, domain);

	fclaw_clawpatch_options_t* opts = fclaw_clawpatch_options_new(2);
	fclaw_clawpatch_options_store(glob1, opts);
	fclaw_clawpatch_options_store(glob2, opts);

	fclaw_vtables_initialize(glob1);
	fclaw_vtables_initialize(glob2);

	fclaw_clawpatch_vtable_initialize(glob1,4);
	fclaw_clawpatch_vtable_initialize(glob2,4);

	CHECK_NE(fclaw_clawpatch_vt(glob1), fclaw_clawpatch_vt(glob2));

	fclaw_clawpatch_options_destroy(opts);
	fclaw_domain_destroy(domain);
	fclaw_global_destroy(glob1);
	fclaw_global_destroy(glob2);
}

TEST_CASE("3dx fclaw_clawpatch_vtable_initialize stores two separate vtables in two separate globs")
{
	fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, 1);
	fclaw_global_t* glob1 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob1, domain);
	fclaw_global_t* glob2 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob2, domain);

	fclaw_clawpatch_options_t* opts = fclaw_clawpatch_options_new(3);
	fclaw_clawpatch_options_store(glob1, opts);
	fclaw_clawpatch_options_store(glob2, opts);

	fclaw_vtables_initialize(glob1);
	fclaw_vtables_initialize(glob2);

	fclaw_clawpatch_vtable_initialize(glob1,4);
	fclaw_clawpatch_vtable_initialize(glob2,4);

	CHECK_NE(fclaw_clawpatch_vt(glob1), fclaw_clawpatch_vt(glob2));

	fclaw_clawpatch_options_destroy(opts);
	fclaw_domain_destroy(domain);
	fclaw_global_destroy(glob1);
	fclaw_global_destroy(glob2);
}

TEST_CASE("2d fclaw_clawpatch_vtable_initialize sets is_set flag")
{
	fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, 1);
	fclaw_global_t* glob = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob, domain);

	fclaw_clawpatch_options_t* opts = fclaw_clawpatch_options_new(2);
	fclaw_clawpatch_options_store(glob, opts);

	fclaw_vtables_initialize(glob);

	fclaw_clawpatch_vtable_initialize(glob, 4);

	CHECK_UNARY(fclaw_clawpatch_vt(glob)->is_set);

	fclaw_clawpatch_options_destroy(opts);
	fclaw_domain_destroy(domain);
	fclaw_global_destroy(glob);
}

TEST_CASE("3dx fclaw_clawpatch_vtable_initialize sets is_set flag")
{
	fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, 1);
	fclaw_global_t* glob = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob, domain);

	fclaw_clawpatch_options_t* opts = fclaw_clawpatch_options_new(3);
	fclaw_clawpatch_options_store(glob, opts);

	fclaw_vtables_initialize(glob);

	fclaw_clawpatch_vtable_initialize(glob, 4);

	CHECK_UNARY(fclaw_clawpatch_vt(glob)->is_set);

	fclaw_clawpatch_options_destroy(opts);
	fclaw_domain_destroy(domain);
	fclaw_global_destroy(glob);
}



TEST_CASE("2d fclaw_clawpatch_vtable_initialize fails if called twice on a glob")
{
	fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, 1);
	fclaw_global_t* glob1 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob1, domain);
	fclaw_global_t* glob2 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob2, domain);

	fclaw_clawpatch_options_t* opts = fclaw_clawpatch_options_new(2);
	fclaw_clawpatch_options_store(glob1, opts);
	fclaw_clawpatch_options_store(glob2, opts);

	fclaw_vtables_initialize(glob1);
	fclaw_vtables_initialize(glob2);

	fclaw_clawpatch_vtable_initialize(glob1,4);
	CHECK_SC_ABORTED(fclaw_clawpatch_vtable_initialize(glob1,4));
	fclaw_clawpatch_vtable_initialize(glob2,4);
	CHECK_SC_ABORTED(fclaw_clawpatch_vtable_initialize(glob2,4));

	fclaw_clawpatch_options_destroy(opts);
	fclaw_domain_destroy(domain);
	fclaw_global_destroy(glob1);
	fclaw_global_destroy(glob2);
}

TEST_CASE("3dx fclaw_clawpatch_vtable_initialize fails if called twice on a glob")
{
	fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, 1);
	fclaw_global_t* glob1 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob1, domain);
	fclaw_global_t* glob2 = fclaw_global_new_comm(sc_MPI_COMM_SELF, 1, 0);;
	fclaw_global_store_domain(glob2, domain);

	fclaw_clawpatch_options_t* opts = fclaw_clawpatch_options_new(3);
	fclaw_clawpatch_options_store(glob1, opts);
	fclaw_clawpatch_options_store(glob2, opts);

	fclaw_vtables_initialize(glob1);
	fclaw_vtables_initialize(glob2);

	fclaw_clawpatch_vtable_initialize(glob1,4);
	CHECK_SC_ABORTED(fclaw_clawpatch_vtable_initialize(glob1,4));
	fclaw_clawpatch_vtable_initialize(glob2,4);
	CHECK_SC_ABORTED(fclaw_clawpatch_vtable_initialize(glob2,4));

	fclaw_clawpatch_options_destroy(opts);
	fclaw_domain_destroy(domain);
	fclaw_global_destroy(glob1);
	fclaw_global_destroy(glob2);
}