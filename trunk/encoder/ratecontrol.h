/*****************************************************************************
 * ratecontrol.h: xavs encoder library (Rate Control)
 *****************************************************************************
 * Copyright (C) 2009 xavs project
 *
 * Authors: 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

#ifndef _XAVS_RATECONTROL_H_
#define _XAVS_RATECONTROL_H_

int  xavs_ratecontrol_new   ( xavs_t * );
void xavs_ratecontrol_delete( xavs_t * );

void xavs_ratecontrol_start( xavs_t *, int i_slice_type, int i_force_qp );
void xavs_ratecontrol_threads_start( xavs_t * );
int  xavs_ratecontrol_slice_type( xavs_t *, int i_frame );
void xavs_ratecontrol_mb( xavs_t *, int bits );
int  xavs_ratecontrol_qp( xavs_t * );
void xavs_ratecontrol_end( xavs_t *, int bits );
void xavs_ratecontrol_summary( xavs_t * );

#endif
