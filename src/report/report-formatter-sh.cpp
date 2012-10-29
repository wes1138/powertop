/* Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *	http://www.samsung.com/
 *
 * This file is part of PowerTOP
 *
 * This program file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 * or just google for it.
 *
 * CSV report generator.
 * Written by Igor Zhbanov <i.zhbanov@samsung.com>
 * 2012.10 */

#define _BSD_SOURCE

/* Uncomment to disable asserts */
/*#define NDEBUG*/

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "report-formatter-sh.h"

static const char report_sh_header[] = "PowerTOP Report";

/* ************************************************************************ */

report_formatter_sh::report_formatter_sh()
{
	add_doc_header();
}

/* ************************************************************************ */

void
report_formatter_sh::finish_report()
{
	/* Do nothing special */
}

/* ************************************************************************ */

void
report_formatter_sh::add_doc_header()
{
	add_header(report_sh_header, 1);
}

/* ************************************************************************ */

void
report_formatter_sh::add_header(const char *header, int level)
{
	assert(header);

	text_start = result.length();
	csv_need_quotes = false;
	addf("# %.*s%s%.*s", 4 - level, "***", header, 4 - level, "***");
	add_quotes();
	add_exact("\n");
}

/* ************************************************************************ */

void
report_formatter_sh::begin_table(table_type ttype)
{
	add_exact("\n");
}

/* ************************************************************************ */

void
report_formatter_sh::end_table()
{
	add_exact("\n");
}

/* ************************************************************************ */

void
report_formatter_sh::begin_row(row_type rtype)
{
	// TODO: whenever you are passed row_type as
	// ROW_TUNABLE_BAD, then you should be listening.
	// you'll then get two entries; the first being the
	// description, and the second the shell command.
	// NOTE: you get these two from calls to begin_cell()
	table_cell_number = 0;
}

/* ************************************************************************ */

void
report_formatter_sh::end_row()
{
	add_exact("\n");
}

/* ************************************************************************ */

void
report_formatter_sh::begin_cell(cell_type ctype)
{
	// TODO: You will get this information from tuning.cpp (and others!)
	// so you need to distinguish between calls to ->description() and
	// ->toggle_script().  Maybe a report is not the way to go here.
	// Perhaps starting more from scratch is desirable : (
	/* Update: upon begin_row, you should set a switch to be 0, if the
	 * parameter is ROW_TUNABLE_BAD, indicating that the next call here
	 * will be the description.
	 * */
	if (table_cell_number > 0) {
		addf_exact("%c", REPORT_CSV_DELIMITER);
#ifdef REPORT_CSV_ADD_SPACE
		add_exact(" ");
#endif /* !REPORT_CSV_ADD_SPACE */
	}

	text_start = result.length();
	csv_need_quotes = false;
}

/* ************************************************************************ */

void
report_formatter_sh::add_quotes()
{
#ifdef REPORT_CSV_ESCAPE_EMPTY
	if (csv_need_quotes || result.length() == text_start) {
#else /* !REPORT_CSV_ESCAPE_EMPTY */
	if (csv_need_quotes) {
#endif /* !REPORT_CSV_ESCAPE_EMPTY */
		result.insert(text_start, "\"");
		add_exact("\"");
	}
}

/* ************************************************************************ */

void
report_formatter_sh::end_cell()
{
	add_quotes();
	table_cell_number++;
}

/* ************************************************************************ */

void
report_formatter_sh::begin_paragraph()
{
	text_start = result.length();
	csv_need_quotes = false;
}

/* ************************************************************************ */

void
report_formatter_sh::end_paragraph()
{
	add_quotes();
	add_exact("\n");
}

/* ************************************************************************ */

std::string
report_formatter_sh::escape_string(const char *str)
{
	std::string res;

	assert(str);

	for (const char *i = str; *i; i++) {
		switch (*i) {
			case '"':
				res += '"';
#ifdef REPORT_CSV_SPACE_NEED_QUOTES
			case ' ':
#endif /* REPORT_CSV_SPACE_NEED_QUOTES */
			case '\n':
			case REPORT_CSV_DELIMITER:
				csv_need_quotes = true;
				break;
		}

		res += *i;
	}

	return res;
}
