/*
 * Copyright (C) 2009 Karel Zak <kzak@redhat.com>
 *
 * This file may be redistributed under the terms of the
 * GNU Lesser General Public License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

#include <blkid.h>

#ifndef TRUE
# define TRUE 1
# define FALSE 0
#endif

int main(int argc, char *argv[])
{
	int fd, rc;
	char *devname;
	blkid_probe pr;
	blkid_topology tp;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <device>  "
			"-- checks based on libblkid for mkfs-like programs.\n",
			program_invocation_short_name);
		return EXIT_FAILURE;
	}

	devname = argv[1];

	fd = open(devname, O_RDONLY);
	if (fd < 0)
		err(EXIT_FAILURE, "%s: open() failed", devname);

	pr = blkid_new_probe();
	if (!pr)
		err(EXIT_FAILURE, "faild to allocate a new libblkid probe");

	if (blkid_probe_set_device(pr, fd, 0, 0) != 0)
		err(EXIT_FAILURE, "failed to assign device to libblkid probe");

	/*
	 * check Filesystems / Partitions overwrite
	 */

	/* enable partitions probing (superblocks are enabled by default) */
	blkid_probe_enable_partitions(pr, TRUE);

	rc = blkid_do_fullprobe(pr);
	if (rc == -1)
		errx(EXIT_FAILURE, "%s: blkid_do_fullprobe() failed", devname);
	else if (rc == 0) {
		const char *type;

		if (!blkid_probe_lookup_value(pr, "TYPE", &type, NULL))
			errx(EXIT_FAILURE, "%s: appears to contain an existing "
					"filesystem (%s)", devname, type);

		if (!blkid_probe_lookup_value(pr, "PTTYPE", &type, NULL))
			errx(EXIT_FAILURE, "%s: appears to contain an partition "
					"table (%s)", devname, type);
	}

	/*
	 * get topology details
	 */
	tp = blkid_probe_get_topology(pr);
	if (!tp)
		err(EXIT_FAILURE, "%s: failed to read topology", devname);


	/* ... your mkfs.<type> code or so ...

	off = blkid_topology_get_alignment_offset(tp);


	 */

	return EXIT_SUCCESS;
}
