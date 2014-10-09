/*!The Graphic Box Library
 * 
 * GBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * GBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2014 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        geometry.c
 * @ingroup     core
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "geometry.h"
#include "quad.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the quad points of the clockwise unit circle
static gb_point_t g_quad_points_of_unit_circle[] = 
{
#if 1

    // move-to
    { GB_ONE,           0               }

    // part1
,   { GB_ONE,           GB_TAN_PIOVER8  }
,   { GB_SQRT2_OVER2,   GB_SQRT2_OVER2  }
,   { GB_TAN_PIOVER8,   GB_ONE          }
,   { 0,                GB_ONE          }

    // part2
,   { -GB_TAN_PIOVER8,  GB_ONE          }
,   { -GB_SQRT2_OVER2,  GB_SQRT2_OVER2  }
,   { -GB_ONE,          GB_TAN_PIOVER8  }
,   { -GB_ONE,          0               }

    // part3
,   { -GB_ONE,          -GB_TAN_PIOVER8 }
,   { -GB_SQRT2_OVER2,  -GB_SQRT2_OVER2 }
,   { -GB_TAN_PIOVER8,  -GB_ONE         }
,   { 0,                -GB_ONE         }

    // part4
,   { GB_TAN_PIOVER8,   -GB_ONE         }
,   { GB_SQRT2_OVER2,   -GB_SQRT2_OVER2 }
,   { GB_ONE,           -GB_TAN_PIOVER8 }
,   { GB_ONE,           0               }

#else // counter-clockwise

    // move-to
    { GB_ONE,            0}

    // part1
,   { GB_ONE,           -GB_TAN_PIOVER8 }    
,   { GB_SQRT2_OVER2,   -GB_SQRT2_OVER2 }
,   { GB_TAN_PIOVER8,   -GB_ONE         }
,   { 0,                -GB_ONE         }

    // part2
,   { -GB_TAN_PIOVER8,  -GB_ONE         }
,   { -GB_SQRT2_OVER2,  -GB_SQRT2_OVER2 }
,   { -GB_ONE,          -GB_TAN_PIOVER8 }
,   { -GB_ONE,          0               }

    // part3
,   { -GB_ONE,          GB_TAN_PIOVER8  }
,   { -GB_SQRT2_OVER2,  GB_SQRT2_OVER2  }
,   { -GB_TAN_PIOVER8,  GB_ONE          }
,   { 0,                GB_ONE          }

    // part4
,   { GB_TAN_PIOVER8,   GB_ONE          }
,   { GB_SQRT2_OVER2,   GB_SQRT2_OVER2  }
,   { GB_ONE,           GB_TAN_PIOVER8  }
,   { GB_ONE,           0               }
#endif
};


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/*
 *
 *          cpb      cp0      cpe
 *          --------------------
 *         /                    \
 *        /    pb0---------pe0   \
 *       /           p0           \
 *  cp1 /                          \ cp2
 *     /                            \
 *    /                              \
 *   /                                \
 *  /                                  \
 * /                                    \
 * pb                                   pe
 *
 * e = min(|(cpb - pb) * 2 + cpb - pe|, |(cpe - pe) * 2 + cpe - pb|) <= 1
 *
 */
tb_void_t gb_geometry_make_cube(gb_point_ref_t pb, gb_point_ref_t cpb, gb_point_ref_t cpe, gb_point_ref_t pe, gb_geometry_line_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_return(func && pb && cpb && cpe && pe && priv);

    // init
    gb_float_t mxb = gb_lsh(cpb->x - pb->x, 1) + cpb->x - pe->x;
    gb_float_t myb = gb_lsh(cpb->y - pb->y, 1) + cpb->y - pe->y;
    gb_float_t mxe = gb_lsh(cpe->x - pe->x, 1) + cpe->x - pb->x;
    gb_float_t mye = gb_lsh(cpe->y - pe->y, 1) + cpe->y - pb->y;

    // compute error
    mxb = gb_fabs(mxb);
    myb = gb_fabs(myb);
    mxe = gb_fabs(mxe);
    mye = gb_fabs(mye);
    if (mxe < mxb) mxb = mxe;
    if (mye < myb) myb = mye;

    // error <= 1?
    if (mxb + myb <= GB_ONE) func(pe, priv);
    else
    {
        // compute cube points
        gb_point_t cp0, cp1, cp2, pb0, pe0, p0;
        cp0.x   = gb_rsh(cpb->x + cpe->x, 1);
        cp0.y   = gb_rsh(cpb->y + cpe->y, 1);
        cp1.x   = gb_rsh(pb->x + cpb->x, 1);
        cp1.y   = gb_rsh(pb->y + cpb->y, 1);
        cp2.x   = gb_rsh(cpe->x + pe->x, 1);
        cp2.y   = gb_rsh(cpe->y + pe->y, 1);
        pb0.x   = gb_rsh(cp0.x + cp1.x, 1);
        pb0.y   = gb_rsh(cp0.y + cp1.y, 1);
        pe0.x   = gb_rsh(cp0.x + cp2.x, 1);
        pe0.y   = gb_rsh(cp0.y + cp2.y, 1);
        p0.x    = gb_rsh(pb0.x + pe0.x, 1);
        p0.y    = gb_rsh(pb0.y + pe0.y, 1);

        // make cube: pb => cp1 => pb0 => p0
        gb_geometry_make_cube(pb, &cp1, &pb0, &p0, func, priv);

        // make cube: p0 => pe0 => cp2 => pe
        gb_geometry_make_cube(&p0, &pe0, &cp2, pe, func, priv);
    }
}
tb_void_t gb_geometry_make_arc(gb_arc_ref_t arc, gb_geometry_quad_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(arc && func);

    // done the degenerated arc
    if (gb_ez(arc->rx) && gb_ez(arc->ry))
    {
        gb_point_t point;
        gb_point_make(&point, arc->c.x, arc->c.y);
        func(tb_null, &point, priv);
        return ;
    }

    // the start and stop unit vector for arc
    gb_vector_t start;
    gb_vector_t stop;
    gb_sincos(gb_degree_to_radian(arc->ab), &start.y, &start.x);
    gb_sincos(gb_degree_to_radian(arc->ab + arc->an), &stop.y, &stop.x);

    // init matrix
    gb_matrix_t matrix;
    gb_matrix_init_scale(&matrix, arc->rx, arc->ry);
    gb_matrix_translate_lhs(&matrix, arc->c.x, arc->c.y);

    /* make arc
     *
     * arc = matrix * unit_arc
     */
    gb_geometry_make_arc2(&start, &stop, &matrix, gb_bz(arc->an)? GB_ROTATE_DIRECTION_CW : GB_ROTATE_DIRECTION_CCW, func, priv);
}
tb_void_t gb_geometry_make_arc2(gb_vector_ref_t start, gb_vector_ref_t stop, gb_matrix_ref_t matrix, tb_size_t direction, gb_geometry_quad_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_abort(start && stop && func);
   
    // the cos and sin value of the sweep angle
    gb_float_t sweep_x = gb_vector_dot(start, stop);
    gb_float_t sweep_y = gb_vector_cross(start, stop);

    // the absolute value of the sweep x and y
    gb_float_t sweep_abs_x = gb_fabs(sweep_x);
    gb_float_t sweep_abs_y = gb_fabs(sweep_y);

    // the points and count
    tb_size_t   count = 0;
    gb_point_t  points[tb_arrayn(g_quad_points_of_unit_circle)];

    // the sweep angle is nearly zero? only one start point 
    if (    sweep_abs_y <= GB_NEAR0 && gb_bz(sweep_x)
        &&  (   (!gb_lz(sweep_y) && direction == GB_ROTATE_DIRECTION_CW)
            ||  (!gb_bz(sweep_y) && direction == GB_ROTATE_DIRECTION_CCW))) 
    {
        gb_point_make(&points[count++], GB_ONE, 0);
    } 
    else
    {
        // counter-clockwise? reverse to the clockwise direction
        if (direction == GB_ROTATE_DIRECTION_CCW) sweep_y = -sweep_y;

        // init the point count for the move-to point first
        count++;

        /* compute the point count of the quadratic curve
         *
         *   .             .             .
         *     .           .           .
         *       .         .         .
         *         .       .       .
         *           .     .     .             sweep_y < 0
         *             .   .   .
         *               . . .
         *   . . . . . . . . . . . . . . . 
         *               . . .                 |
         *             .   .   .               | direction: clockwise
         *           .     .     .            \|/
         *         .       .       .
         *       .         .         .
         *     .           .           .
         *   .             .             .     sweep_y > 0
         *
         */
        if (gb_ez(sweep_y))
        {
            // check
            tb_assert_abort(gb_fabs(sweep_x + GB_ONE) <= GB_NEAR0);

            // 180 degrees
            count += 8;
        }
        else if (gb_ez(sweep_x))
        {
            // check
            tb_assert_abort(sweep_abs_y - GB_ONE <= GB_NEAR0);

            // 90 or 270 degrees
            count += sweep_y > 0 ? 4 : 12;
        }
        else
        {
            // > 180 degrees
            if (gb_lz(sweep_y)) count += 8;
            
            // > 90 or 270 degrees
            tb_long_t same = 1;
            if ((gb_lz(sweep_x)) != (gb_lz(sweep_y)))
            {
                count += 4;
                same = 0;
            }

            // > 45 or 135 or 225 or 315 degrees
            if ((sweep_abs_x < sweep_abs_y) == same)
                count += 2;
        }
 
        // check
        tb_assert_abort((count & 0x1) && count <= tb_arrayn(g_quad_points_of_unit_circle));

        // make points
        tb_memcpy(points, g_quad_points_of_unit_circle, count * sizeof(gb_point_t));

        // patch the last quadratic curve
        if (    gb_nz(sweep_abs_x)
            &&  gb_nz(sweep_abs_y)
            &&  sweep_abs_x != GB_SQRT2_OVER2
            &&  sweep_abs_y != GB_SQRT2_OVER2)
        {
            // the patched start vector
            gb_vector_t patched_start;
            gb_vector_make_from_point(&patched_start, &points[count - 1]);

            // the patched stop vector
            gb_vector_t patched_stop;
            gb_vector_make(&patched_stop, sweep_x, sweep_y);
 
            // init the applied matrix
            gb_matrix_t applied_matrix;
            gb_matrix_init_sincos(&applied_matrix, patched_start.y, patched_start.x);

            /* compute the tan(a/2)
             *
             * tan(a/2) = sin(a) / (1 + cos(a))
             */
            gb_float_t cos_a = gb_vector_dot(&patched_start, &patched_stop);
            gb_float_t sin_a = gb_vector_cross(&patched_start, &patched_stop);
            gb_float_t tan_a = gb_div(sin_a, cos_a + GB_ONE);

            /* compute the ctrl point, start => ctrl => stop
             *
             * .       start
             * .        .        ctrl
             * .       .       .
             * .      .      .
             * .     .     .
             * .    .    .         . stop 
             * .   .   .       .  
             * .  .  .     .   
             * . . a   .        1
             * . . . . . . . . . . . . start^
             *       .    a/2        .
             *           .           .
             *               .       .  tan(a/2)
             *                   .   .
             *                       . ctrl^
             *                         
             *
             * ctrl^    = (1, tan(a/2))
             * matrix   = rotate_sincos(start.y, start.x)
             * ctrl     = ctrl^ * matrix
             */
            gb_point_t ctrl;
            gb_point_make(&ctrl, GB_ONE, tan_a);
            gb_point_apply(&ctrl, &applied_matrix);

            // patch the last quadratic curve
            points[count++] = ctrl;
            gb_point_make(&points[count++], patched_stop.x, patched_stop.y);
        }
    }
 
    // init the applied matrix
    gb_matrix_t applied_matrix;
    gb_matrix_init_sincos(&applied_matrix, start->y, start->x);
    if (direction == GB_ROTATE_DIRECTION_CCW) gb_matrix_scale(&applied_matrix, GB_ONE, -GB_ONE);
    if (matrix) gb_matrix_multiply_lhs(&applied_matrix, matrix);

    // apply matrix for the first point
    gb_point_apply(points, &applied_matrix);

    // done func
    func(tb_null, points, priv);

    // walk points
    gb_point_ref_t pb = points + 1;
    gb_point_ref_t pe = points + count;
    for (; pb < pe; pb += 2) 
    {
        // apply matrix for the quad points
        gb_point_apply(pb, &applied_matrix);
        gb_point_apply(pb + 1, &applied_matrix);

        // done
        func(pb, pb + 1, priv);
    }
}
